/*
Copyright 2025 EnVisioNate LLC

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the “Software”), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is the MIT License
*/

#include "job.h"

    AdobeType1Fonts adobeType1Fonts;

    void AdobeType1Fonts::drawGlyph(font *pFont,uint16_t bGlyph,
                                        XFORM *pPSXForm,POINTF *pStartPoint,POINTF *pEndPoint,POINTF *pStartPointPDF,POINTF *pEndPointPDF) {

/*
    In contrast, Type 1 font programs implicitly reference a special
    BuildChar procedure called Type 1 BuildChar that is internal to the
    PostScript interpreter.
        .
        .
        .
    Type 1 BuildChar begins by using the character code as an index
    into the Encoding array in the font dictionary to obtain the name
    of the character to be built.
*/

    object *pCharacterName = pFont -> Encoding() -> getElement((uint16_t)bGlyph);

/*
    Type 1 BuildChar then uses the name of the character to be built as a key in
    the CharStrings dictionary (contained in the font dictionary) to
    obtain a binary string. The string is an encoded and encrypted
    representation of a PostScript language program for that character’s outline.
*/

    object *pStrObject = pFont -> CharStrings() -> retrieve(pCharacterName -> Contents());

    if ( ! ( object::valueType::binaryString == pStrObject -> ValueType() ) ) {
        throw new typecheck("Invalid CharStrings object for type 1 font");
        return;
    }

    binaryString *pString = reinterpret_cast<binaryString *>(pStrObject);

    if ( ! ( NULL == pType1Glyph ) ) 
        delete pType1Glyph;

    pType1Glyph = new type1Glyph(reinterpret_cast<dictionary *>(pFont));

    uint32_t countTokens = AdobeType1Fonts::decryptType1CharString(pString -> getData(),(uint32_t)pString -> length(),pType1Glyph -> countRandomBytes);

    type1Token **ppTokens = new type1Token *[countTokens];

    AdobeType1Fonts::decryptType1CharString(pString -> getData(),(uint32_t)pString -> length(),pType1Glyph -> countRandomBytes,ppTokens);

    XFORM xForm{0};
    XFORM *pFontXForm = pFont -> getFontMatrix();
    XFORM glyphSpaceToUserSpace{0};
    XFORM glyphSpaceToPageSpace{0};

    xForm.eM11 = pType1Glyph -> pFontMatrixArray -> getElement(A) -> FloatValue();
    xForm.eM12 = pType1Glyph -> pFontMatrixArray -> getElement(B) -> FloatValue();
    xForm.eM21 = pType1Glyph -> pFontMatrixArray -> getElement(C) -> FloatValue();
    xForm.eM22 = pType1Glyph -> pFontMatrixArray -> getElement(D) -> FloatValue();
    xForm.eDx = pType1Glyph -> pFontMatrixArray -> getElement(TX) -> FloatValue();
    xForm.eDy = pType1Glyph -> pFontMatrixArray -> getElement(TY) -> FloatValue();

/*
    All coordinates in these descriptions are in character space
    units, and assume the 1000 to 1 character space to USER SPACE 
    scaling that is typical of the Type 1 font format. 
*/

    CombineTransform(&glyphSpaceToUserSpace,&xForm,pFontXForm);
    CombineTransform(&glyphSpaceToPageSpace,&glyphSpaceToUserSpace,pPSXForm);

    // 
    // IMPORTANT - if you are going to set an origin for IRenderer,
    // for example, when the data contains a block of points which
    // are intended to be drawn at an arbitrary point,
    // THEN, be sure to take the translation component out of the
    // transform(s) that are passed to the renderer.
    //
    // Specifically, to draw a glyph, the points in the glyph should
    // be translated to PAGE space - the raw dimensions of the page
    // typically 612x792.
    // Therefore, transforms to the Renderer should contain both the 
    // font matrix AND the current postscript matrix
    // However the origin should be translated to page space (i.e., using the
    // current postscript matrix, but NOT the font matrix)
    //

    glyphSpaceToPageSpace.eDx = 0.0f;
    glyphSpaceToPageSpace.eDy = 0.0f;

    pIRenderer -> SaveState();

    POINTF sp{pStartPoint -> x,pStartPoint -> y};

    matrix::transformPoint(pPSXForm,&sp);

    if ( ! ( NULL == pStartPointPDF ) )
        *pStartPointPDF = sp;

    pIRenderer -> put_Origin(sp);

    pIRenderer -> put_ToPageTransform((UINT_PTR)&glyphSpaceToPageSpace);

    try {

    processTokens(countTokens,ppTokens);

    } catch ( nonPostscriptException *ex ) {
        pPostScriptInterpreter -> queueLog(true,ex -> Message(),NULL,true);
    } catch ( PStoPDFException *pe ) {
        char szMessage[1024];
        sprintf(szMessage,"\n\nA %s exception occurred: %s\n",pe -> ExceptionName(),pe -> Message());
        pPostScriptInterpreter -> queueLog(true,szMessage,NULL,true);
    }

    for ( uint32_t k = 0; k < countTokens; k++ )
        delete ppTokens[k];

    delete [] ppTokens;

    if ( ! ( NULL == pEndPoint ) ) {
        POINTF delta{pType1Glyph -> leftSideBearing[0] + pType1Glyph -> characterWidth[0],
                        pType1Glyph -> leftSideBearing[1]};
        matrix::transformPoint(&glyphSpaceToUserSpace,&delta);
        pEndPoint -> x = pStartPoint -> x + delta.x;
        pEndPoint -> y = pStartPoint -> y + delta.y;
    }

    if ( ! ( NULL == pEndPointPDF ) ) {
        POINTF delta{pType1Glyph -> leftSideBearing[0] + pType1Glyph -> characterWidth[0],
                        pType1Glyph -> leftSideBearing[1]};
        matrix::transformPoint(&glyphSpaceToUserSpace,&delta);
        POINTF ep{pStartPoint -> x + delta.x,pStartPoint -> y + delta.y};
        matrix::transformPoint(pPSXForm,&ep);
        *pEndPointPDF = ep;
    }

    pIRenderer -> RestoreState();

    return;
    }


    void AdobeType1Fonts::processTokens(uint32_t countTokens,type1Token *pTokens[]) {

    for ( uint32_t k = 0; k < countTokens; k++ ) {

        type1Token *pToken = pTokens[k];

        switch ( pToken -> theKind ) {
        case type1Token::tokenKind::isNumber:
            pOperandStack -> push(new (pJob -> CurrentObjectHeap()) object(pJob,(long)pToken -> value));
            break;

        case type1Token::tokenKind::isCommand: {
            pPostScriptInterpreter -> queueLog(true,pToken -> pszCommand);
            directExec *pDirectExec = reinterpret_cast<directExec *>(pOperators -> retrieve(pToken -> pszCommand));
            void (__thiscall AdobeType1Fonts::*pOperator)() = pDirectExec -> AdobeType1FontsOperator();
            (&adobeType1Fonts ->* pOperator)();
            }
            break;
        }

        if ( pType1Glyph -> terminationRequested )
            return;

    }
    return;
    }