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

#include "pageParameters.h"

    void graphicsState::drawTextChar(BYTE glyphIndex,boolean emitEvent,POINT *pStartPDFResult,POINT *pEndPDFResult) {

    POINTF startPoint{currentUserSpacePoint.x,currentUserSpacePoint.y};
    POINTF endPoint;

    POINTF startPointPDF{0.0f,0.0f};
    POINTF endPointPDF{0.0f,0.0f};

    if ( FontType::type1 == CurrentFont() -> TheFontType() ) {

        drawType1Glyph(glyphIndex,&startPoint,&endPoint,&startPointPDF,&endPointPDF);
        moveto(&endPoint);

    } else if ( FontType::type3 == CurrentFont() -> TheFontType() ) {

        drawType3Glyph(glyphIndex,&startPointPDF,&endPointPDF);

    } else {

        drawType42Glyph(glyphIndex,&startPoint,&endPoint,&startPointPDF,&endPointPDF);
        moveto(&endPoint);

    }

    POINT ps{(long)startPointPDF.x,(long)startPointPDF.y};
    POINT pe{(long)endPointPDF.x,(long)endPointPDF.y};

    if ( emitEvent )
        pPostScriptInterpreter -> pIConnectionPointContainer -> fire_RenderChar(&ps,&pe,(char)glyphIndex);

    if ( pStartPDFResult )
        *pStartPDFResult = ps;

    if ( pEndPDFResult )
        *pEndPDFResult = pe;

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

    POINTF startPoint{currentUserSpacePoint.x,currentUserSpacePoint.y};
    POINTF endPoint{0.0f,0.0f};

    POINTF startPointPDF{0.0f,0.0f};
    POINTF endPointPDF{0.0f,0.0f};

    FLOAT maxY = -FLT_MAX;
    FLOAT minY = FLT_MAX;

    if ( FontType::type1 == CurrentFont() -> TheFontType() || 
            FontType::type3 == CurrentFont() -> TheFontType() ) {

        for ( long k = 0; k < strSize; k++ ) {

            BYTE glyphIndex;

            if ( ! ( NULL == pBinary ) )
                glyphIndex = pBinary -> get(k);
            else
                glyphIndex = pString -> get(k);

            if ( FontType::type1 == CurrentFont() -> TheFontType() )
                drawType1Glyph(glyphIndex,&startPoint,&endPoint,0 == k ? &startPointPDF : NULL,&endPointPDF);
            else {
                drawType3Glyph(glyphIndex,0 == k ? &startPointPDF : NULL,&endPointPDF);
                endPoint = {currentUserSpacePoint.x,currentUserSpacePoint.y};
            }

            if ( endPointPDF.y > maxY )
                maxY = endPointPDF.y;

            if ( endPointPDF.y < minY )
                minY = endPointPDF.y;

            startPoint = endPoint;

        }

        if ( FontType::type1 == CurrentFont() -> TheFontType() ) 
            moveto(endPoint.x,endPoint.y);

    } else for ( long k = 0; k < strSize; k++ ) {

        BYTE glyphIndex;

        if ( ! ( NULL == pBinary ) )
            glyphIndex = pBinary -> get(k);
        else
            glyphIndex = pString -> get(k);

        drawType42Glyph(glyphIndex,&startPoint,&endPoint,0 == k ? &startPointPDF : NULL,&endPointPDF);

        if ( endPointPDF.y > maxY ) 
            maxY = endPointPDF.y;

        if ( endPointPDF.y < minY )
            minY = endPointPDF.y;

        startPoint = endPoint;

    }

    endPointPDF.y = maxY;
    startPointPDF.y = minY;

    POINT ps{(long)startPointPDF.x,(long)startPointPDF.y};
    POINT pe{(long)endPointPDF.x,(long)endPointPDF.y};

    if ( ! ( NULL == pBinary ) )
        pPostScriptInterpreter -> pIConnectionPointContainer -> fire_RenderString(&ps,&pe,pBinary -> Contents());
    else
        pPostScriptInterpreter -> pIConnectionPointContainer -> fire_RenderString(&ps,&pe,pString -> Contents());

    return;
    }


    void graphicsState::drawType1Glyph(uint16_t bGlyph,POINTF *pStartPoint,POINTF *pEndPoint,POINTF *pStartPointPDF,POINTF *pEndPointPDF) {
    AdobeType1Fonts::drawGlyph(CurrentFont(),bGlyph,
                                pPSXformsStack -> CurrentTransform(),pStartPoint,pEndPoint,pStartPointPDF,pEndPointPDF);
    return;
    }


    void graphicsState::drawType3Glyph(uint16_t bGlyph,POINTF *pStartPDF,POINTF *pEndPDF) {

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

    object *pCharacterName = CurrentFont() -> Encoding() -> getElement((uint16_t)bGlyph);

    pJob -> push(CurrentFont());
    pJob -> push(pCharacterName);

    pPSXformsStack -> gSave();

    pPSXformsStack -> translate(currentUserSpacePoint.x,currentUserSpacePoint.y);

    pPSXformsStack -> concat(CurrentFont() -> getFontMatrix());

    if ( ! ( NULL == pStartPDF ) ) {
        POINTF ps{currentUserSpacePoint.x,currentUserSpacePoint.y};
        matrix::transformPoint(pPSXformsStack -> CurrentTransform(),&ps,pStartPDF);
    }

    pJob -> executeProcedure(pBuildGlyph);

    if ( ! ( NULL == pEndPDF ) ) {
        POINTF ps{currentUserSpacePoint.x,currentUserSpacePoint.y};
        matrix::transformPoint(pPSXformsStack -> CurrentTransform(),&ps,pEndPDF);
    }

    pPSXformsStack -> gRestore();

    return;
    }


    void graphicsState::drawType42Glyph(uint16_t glyphIndex,POINTF *pStartPoint,POINTF *pEndPoint,POINTF *pStartPDF,POINTF *pEndPDF) {

    font *pFont = CurrentFont();

    if ( 0 < pFont -> pEncoding -> size() ) {
        object *pGlyphName = pFont -> pEncoding -> getElement(glyphIndex);
        if ( ! ( NULL == pGlyphName ) ) {
            boolean found = false;
            object *pGlyphIndex = pFont -> pCharStrings -> retrieve(pGlyphName -> Contents());
            if ( ! ( NULL == pGlyphIndex ) ) {
                found = true;
                glyphIndex = (uint16_t)pGlyphIndex -> IntValue();
            }
            if ( ! found ) {
                char *pszCharTableIndex = pGlyphName -> Contents();
                for ( std::pair<uint32_t,char *> pPair : font::adobeGlyphList ) {
                    if ( 0 == strcmp(pszCharTableIndex,pPair.second) ) {
                        glyphIndex = pPair.first;
                        break;
                    }
                }
            }
        }
    }

    object *pGlyphDirObj = pFont -> retrieve("GlyphDirectory");
    font::IProvideGlyphData *pIProvideData = NULL;

    if ( ! ( NULL == pGlyphDirObj ) ) {
        dictionary *pGlyphDirectory = reinterpret_cast<dictionary *>(pGlyphDirObj);
        char szIndex[16];
        sprintf_s<16>(szIndex,"%d",glyphIndex);
        object *pGlyphDataObj = pGlyphDirectory -> retrieve(szIndex);
        if ( ! ( NULL == pGlyphDataObj ) ) {
            binaryString *pGlyphData = reinterpret_cast<binaryString *>(pGlyphDataObj);
            if ( 0 == pGlyphData -> length() ) 
                return;
            pIProvideData = &font::ifmClient_ProvideGlyphData;
        }
    }

    PostScriptInterpreter::pIFontManager -> RenderGlyph(glyphIndex,(UINT_PTR)pIProvideData,(UINT_PTR)pPSXformsStack -> CurrentTransform(),
            pStartPoint,pEndPoint,pStartPDF,pEndPDF);

    return;
    }


    HRESULT font::IProvideGlyphData::GetGlyphData(unsigned short bGlyph,UINT_PTR *ppGlyphData) {

    font *pFont = CurrentFont();

    object *pGlyphDirObj = pFont -> retrieve("GlyphDirectory");

    if ( NULL == pGlyphDirObj ) 
        return E_FAIL;

    dictionary *pGlyphDirectory = reinterpret_cast<dictionary *>(pGlyphDirObj);
    char szIndex[16];
    sprintf_s<16>(szIndex,"%d",bGlyph);
    object *pGlyphDataObj = pGlyphDirectory -> retrieve(szIndex);

    if ( NULL == pGlyphDataObj ) 
        return E_FAIL;

    class binaryString *pGlyphData = reinterpret_cast<class binaryString *>(pGlyphDataObj);
    if ( 0 == pGlyphData -> length() ) 
        return E_FAIL;

    uint8_t *pbGlyphData = (uint8_t *)CoTaskMemAlloc(pGlyphData -> length() + 32);
    memset(pbGlyphData,0,pGlyphData -> length() + 32);
    memcpy(pbGlyphData,pGlyphData -> getData(),pGlyphData -> length());

    *ppGlyphData = (UINT_PTR)pbGlyphData;

    return S_OK;
    }