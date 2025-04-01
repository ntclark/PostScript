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
*/#include "job.h"


#include "pathParameters.h"

    void graphicsState::drawTextChar(BYTE glyphIndex) {

    pJob -> push(CurrentFont());
    pJob -> push(pJob -> pFontTypeLiteral);
    pJob -> operatorGet();

    object *pFontType = pJob -> pop();

    if ( 0 == strcmp(pFontType -> Contents(),"3") ) {
        drawType3Glyph(glyphIndex);
        return;
    }

    POINTF startPoint{currentPageSpacePoint.x,currentPageSpacePoint.y};
    POINTF endPoint;

    drawType42Glyph(glyphIndex,&startPoint,&endPoint);

    untransformPoint(&endPoint,&endPoint);

    moveto(&endPoint);

    POINT ptStart{(int)startPoint.x,(int)startPoint.y};

    if ( NULL == pPStoPDF -> HwndClient() ) {
        ptStart.x = -1;
        ptStart.y = -1;
    }

    pPStoPDF -> pIConnectionPointContainer -> fire_RenderChar(&ptStart,(char)glyphIndex);

    return;
    }


    void graphicsState::drawTextString() {

    object *pObject = pJob -> top();

    pJob -> operatorLength();

    long strSize = pJob -> pop() -> IntValue();

    binaryString *pBinary = NULL;
    string *pString = NULL;

    if ( object::valueType::binaryString == pObject -> ValueType() )
        pBinary = reinterpret_cast<binaryString *>(pObject);
    else
        pString = reinterpret_cast<string *>(pObject);

    pJob -> push(CurrentFont());
    pJob -> push(pJob -> pFontTypeLiteral);
    pJob -> operatorGet();

    object *pFontType = pJob -> pop();

    if ( 0 == strcmp(pFontType -> Contents(),"3") ) {

        for ( long k = 0; k < strSize; k++ ) {

            BYTE glyphIndex;

            if ( ! ( NULL == pBinary ) )
                glyphIndex = pBinary -> get(k);
            else
                glyphIndex = pString -> get(k);

            drawType3Glyph(glyphIndex);

        }

        return;
    }

    POINTF startPoint{currentPageSpacePoint.x,currentPageSpacePoint.y};
    POINTF endPoint;

    for ( long k = 0; k < strSize; k++ ) {

        BYTE glyphIndex;

        if ( ! ( NULL == pBinary ) )
            glyphIndex = pBinary -> get(k);
        else
            glyphIndex = pString -> get(k);

        drawType42Glyph(glyphIndex,&startPoint,&endPoint);

        startPoint = endPoint;

    }

    POINT ptStart{(int)currentPageSpacePoint.x,(int)currentPageSpacePoint.y};

    if ( NULL == pPStoPDF -> HwndClient() ) {
        ptStart.x = -1;
        ptStart.y = -1;
    }

    if ( ! ( NULL == pBinary ) )
        pPStoPDF -> pIConnectionPointContainer -> fire_RenderString(&ptStart,pBinary -> Contents());
    else
        pPStoPDF -> pIConnectionPointContainer -> fire_RenderString(&ptStart,pString -> Contents());

    return;
    }


    void graphicsState::drawType42Glyph(uint16_t bGlyph,POINTF *pStartPoint,POINTF *pEndPoint) {

    font::pIFontManager -> RenderGlyph(pPStoPDF -> GetDC(),bGlyph,
                                        (UINT_PTR)psXformsStack.top() -> XForm(),
                                        (UINT_PTR)pathParameters::ToDeviceSpace(),
                                            pStartPoint,pEndPoint);
    return;
    }


    void graphicsState::drawType3Glyph(uint16_t bGlyph) {

    pJob -> push(CurrentFont());
    pJob -> operatorBegin();
    pJob -> push(pJob -> pBuildGlyphLiteral);
    pJob -> operatorDup();
    pJob -> operatorWhere();

    if ( pJob -> pFalseConstant == pJob -> pop() ) {
        pJob -> pop();
        pJob -> push(CurrentFont());
        pJob -> push(pJob -> pBuildCharLiteral);
    } else
        pJob -> operatorExch();

    pJob -> operatorGet();

    pJob -> operatorEnd();

    procedure *pBuildGlyph = reinterpret_cast<procedure *>(pJob -> pop());

    pJob -> push(CurrentFont());
    pJob -> push(pJob -> pEncodingArrayLiteral);
    pJob -> operatorGet();

/*
    When a PostScript program tries to show a glyph from a Type 3 font, and the
    glyph is not already present in the font cache, the PostScript interpreter:

    1. Uses the character code as an index into the current font’s Encoding array,
        obtaining the corresponding character name. (This step is omitted during a
        glyphshow operation.)

    2. Pushes the current font dictionary and the character name on the operand stack.

    3. Executes the font’s BuildGlyph procedure. BuildGlyph must remove these two
        objects from the operand stack and use this information to construct the 
        requested glyph. This typically involves determining the glyph description 
        needed, supplying glyph metric information, constructing the glyph, and painting it.

    All Type 3 fonts must include a character named .notdef. The BuildGlyph procedure 
    should be able to accept that character name regardless of whether such a character 
    is encoded in the Encoding array. If the BuildGlyph procedure is given a character 
    name it does not recognize, it can handle that condition by painting the glyph for 
    the .notdef character instead. 

*/

    // The BYTE bGlyph is the "index" into the Encoding array

    pJob -> push(new (pJob -> CurrentObjectHeap()) object(pJob,(uint16_t)bGlyph));
    pJob -> operatorGet();

    object *pCharacterName = pJob -> pop();

    pJob -> push(CurrentFont());
    pJob -> push(pCharacterName);

    psXformsStack.gSave();

    translate(currentUserSpacePoint.x,currentUserSpacePoint.y);

    concat(CurrentFont() -> getFontMatrix());

    pathParametersStack.top() -> redirectType3();

    pJob -> executeProcedure(pBuildGlyph);

    pathParametersStack.top() -> unRedirectType3();

    psXformsStack.gRestore();

    return;
    }