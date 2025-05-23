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

#include "FontManager.h"


    HRESULT font::QueryInterface(REFIID refIID,void **pvResult) {

    if ( ! pvResult )
        return E_POINTER;

    *pvResult = NULL;

    if ( IID_IUnknown == refIID ) 
        *pvResult = static_cast<IUnknown *>(this);

    else if ( IID_IFont_EVNSW == refIID )
        *pvResult = static_cast<IFont_EVNSW *>(this);

    if ( * pvResult ) {
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
    }


    ULONG font::AddRef() {
    return ++refCount;
    }


    ULONG font::Release() {
    if ( 1 == refCount ) {
        delete this;
        return 0;
    }
    return --refCount;
    }


    HRESULT font::put_Matrix(UINT_PTR pMatrix) {
    matrixStack.top() -> SetMatrix((XFORM *)pMatrix);
    return S_OK;
    }


    HRESULT font::get_Matrix(LPVOID pResult) {
    if ( ! pResult )
        return E_POINTER;
    memcpy((void *)pResult,matrixStack.top() -> XForm(),sizeof(XFORM));
    return S_OK;
    }


    HRESULT font::put_PointSize(FLOAT pointSize) {
    PointSize(pointSize);
    return S_OK;
    }


    HRESULT font::get_PointSize(FLOAT *pPointSize) {
    if ( ! pPointSize )
        return E_POINTER;
    *pPointSize = PointSize();
    return S_OK;
    }


    HRESULT font::FontBoundingBox(POINTF *pLowerLeft,POINTF *pUpperRight) {
    if ( ! pLowerLeft && ! pUpperRight )
        return E_POINTER;
    if ( pLowerLeft ) {
        pLowerLeft -> x = PointSize() * (FLOAT)pHeadTable -> xMin / (FLOAT)pHeadTable -> unitsPerEm;
        pLowerLeft -> y = PointSize() * (FLOAT)pHeadTable -> yMin/ (FLOAT)pHeadTable -> unitsPerEm;
    }
    if ( pUpperRight ) {
        pUpperRight -> x = PointSize() * (FLOAT)pHeadTable -> xMax / (FLOAT)pHeadTable -> unitsPerEm;
        pUpperRight -> y = PointSize() * (FLOAT)pHeadTable -> yMax/ (FLOAT)pHeadTable -> unitsPerEm;
    }
    return S_OK;
    }


    HRESULT font::Scale(FLOAT scaleX,FLOAT scaleY) {
    XFORM scaleMatrix{scaleX / currentScale,0.0f,0.0f,scaleY / currentScale,0.0f,0.0f};
    ConcatMatrix((UINT_PTR)&scaleMatrix);
    PointSize(scaleX);
    currentScale = scaleX;
    return S_OK;
    }


    HRESULT font::Translate(FLOAT translateX,FLOAT translateY) {
    XFORM xForm{1.0f,0.0f,0.0f,1.0f,translateX,translateY};
    ConcatMatrix((UINT_PTR)&xForm);
    return S_OK;
    }


    HRESULT font::ConcatMatrix(UINT_PTR pXForm) {
    XFORM result{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};
    CombineTransform(&result,(XFORM *)pXForm,(XFORM *)matrixStack.top() -> XForm());
    memcpy(matrixStack.top() -> XForm(),&result,sizeof(XFORM));
    return S_OK;
    }


    HRESULT font::FontName(long cb,char *pszName) {
    memset(pszName,0,cb * sizeof(char));
    strncpy(pszName,szClientFamily,cb);
    return S_OK;
    }


    HRESULT font::get_FontCookie(UINT_PTR *pCookie) {
    if ( ! pCookie )
        return E_POINTER;
    *pCookie = cookie;
    return S_OK;
    }


    HRESULT font::get_FontType(FontType *pFontType) {
    if ( ! pFontType )
        return E_POINTER;
    *pFontType = theFontType;
    return S_OK;
    }


    HRESULT font::get_GlyphIndex(uint16_t charCode,uint16_t *pResult) {

    if ( ! pResult )
        return E_POINTER;

    *pResult = 0;

    if ( NULL == pCmapSubtableFormat4 )
        return E_UNEXPECTED;

    /*
    You search for the first endCode that is greater than or equal 
    to the character code you want to map
    */

    uint16_t endCodeIndex = 0xFFFF;
    for ( uint16_t k = 0; 1; k++ ) {
        if ( 0xFFFF == pCmapSubtableFormat4 -> pEndCode[k] || pCmapSubtableFormat4 -> pEndCode[k] >= charCode ) {
            endCodeIndex = k;
            break;
        }
    }

    uint16_t startCode = pCmapSubtableFormat4 -> pStartCode[endCodeIndex];

    if ( charCode < startCode )
        return S_OK;

    uint16_t idRangeOffset = pCmapSubtableFormat4 -> pIdRangeOffsets[endCodeIndex];
    uint16_t idDelta = pCmapSubtableFormat4 -> pIdDelta[endCodeIndex];

    /*
    If the corresponding startCode is less than or equal to the 
    character code, then you use the corresponding idDelta and 
    idRangeOffset to map the character code to a glyph index 
    (otherwise, the missingGlyph is returned). 
    */

    if ( 0 == idRangeOffset ) {

        /*
        If the idRangeOffset is 0, the idDelta value is added directly 
        to the character code offset (i.e. idDelta[i] + c) to get the 
        corresponding glyph index. Again, the idDelta arithmetic is modulo 65536. 
        If the result after adding idDelta[i] + c is less than zero, add 65536 
        to obtain a valid glyph ID.
        */

        *pResult = idDelta + charCode;

        if ( 0 > *pResult)
            *pResult += (uint16_t)65536;

        return S_OK;

    } 

    /*
    If the idRangeOffset value for the segment is not 0, the mapping of character codes 
    relies on glyphIdArray. The character code offset from startCode is added to the 
    idRangeOffset value. This sum is used as an offset from the current location 
    within idRangeOffset itself to index out the correct glyphIdArray value.
    This obscure indexing trick works because glyphIdArray immediately follows 
    idRangeOffset in the font file. The C expression that yields the glyph index is:

    glyphId = *(idRangeOffset[i] / 2 + (c - startCode[i]) + &idRangeOffset[i])
    */

    uint16_t *pbGlyphId = &pCmapSubtableFormat4 -> pIdRangeOffsets[endCodeIndex];
    pbGlyphId += idRangeOffset / 2;
    pbGlyphId += (charCode - startCode);

    *pResult = *pbGlyphId;

    /*
    The value c is the character code in question, and i is the segment index in which c appears. 
    If the value obtained from the indexing operation is not 0 (which indicates missingGlyph), 
    idDelta[i] is added to it to get the glyph index. The idDelta arithmetic is modulo 65536.
    */

    if ( ! ( 0 == *pResult ) ) {
        *pResult += idDelta;
        if ( 0 > *pResult)
            *pResult += (uint16_t)65536;
    }

    return S_OK;
    }


    HRESULT font::SetFontData(long cbSfntsData,UINT_PTR pSfntsData) {

    if ( NULL == pSfntsData )
        return E_FAIL;

    cbFontData = cbSfntsData;
    pbFontData = new uint8_t[cbFontData];

    memcpy(pbFontData,(uint8_t *)pSfntsData,cbFontData);

    type42Load(pbFontData,false);

    theFontType = FontType::type42;

    return S_OK;
    }


    HRESULT font::GetCharacteristics(long cbName,char *pszName,long cbStyle,char *pszStyle,
                                        long cbScript,char *pszScript,long *pFontWeight,FLOAT *pSize,
                                        UINT_PTR *pAvailableFonts,UINT_PTR *pAvailableNames,
                                        UINT_PTR *pAvailableStyles,UINT_PTR *pAvailableScripts) {

    font *pFont = static_cast<font *>(this);

    if ( pszName )
        strncpy(pszName,pFont -> szSelectedFontFullName,cbName);

    if ( pszStyle )
        strncpy(pszStyle,pFont -> szSelectedFontStyleName,cbStyle);

    if ( pszScript )
        strncpy(pszScript,pFont -> szSelectedFontScriptName,cbScript);

    if ( pFontWeight )
        *pFontWeight = pFont -> selectedFontWeight;

    if ( pSize ) 
        *pSize = pFont -> PointSize();

    UINT_PTR *targets[]{pAvailableNames,pAvailableStyles,pAvailableScripts};
    std::vector<char *> *sources[]{&pFont -> fontFullNames,&pFont -> fontStyleNames,&pFont -> fontScriptNames};

    long cbTotal = 0L;
    for ( long k = 0; k < sizeof(targets) / sizeof(UINT_PTR *); k++ ) {
        if ( NULL == targets[k] ) 
            continue;
        long cb = 0L;
        for ( char *p : *sources[k] )
            cb += (long)strlen(p) + 1;
        cbTotal += cb + 1;
        *targets[k] = (UINT_PTR)CoTaskMemAlloc(cb + 1);
        memset((void *)*targets[k],0,cb + 1);
        char *pTarget = (char *)*targets[k];
        for ( char *p : *sources[k] ) {
            strcpy(pTarget,p);
            pTarget += (long)strlen(p) + 1;
        }
    }

    *pAvailableFonts = (UINT_PTR)CoTaskMemAlloc(cbTotal + 1);

    char *pTarget = (char *)*pAvailableFonts;

    for ( long k = 0; k < (long)pFont -> fontFullNames.size(); k++ ) {

        strcpy(pTarget,pFont -> fontFullNames[k]);
        long n = (long)strlen(pFont -> fontFullNames[k]);
        pTarget[n] = ' ';
        char *p = pTarget;
        while ( p - pTarget < n ) {
            if ( *p == ' ' )
                *p = '`';
            p++;
        }
        pTarget += n + 1;

        strcpy(pTarget,pFont -> fontStyleNames[k]);
        n = (long)strlen(pFont -> fontStyleNames[k]);
        pTarget[n] = ':';
        pTarget += n + 1;

        strcpy(pTarget,pFont -> fontScriptNames[k]);
        n = (long)strlen(pFont -> fontScriptNames[k]);
        pTarget[n] = '\0';
        pTarget += n + 1;

    }

    return S_OK;
    }


    HRESULT font::SaveState() {
    matrixStack.push(new matrix(*matrixStack.top()));
    return S_OK;
    }


    HRESULT font::RestoreState() {
    if ( 1 == matrixStack.size() ) 
        return S_OK;
    FLOAT scale = matrixStack.top() -> XForm() -> eM11;
    delete matrixStack.top();
    matrixStack.pop();
    FLOAT restoredScale = matrixStack.top() -> XForm() -> eM11;
    PointSize(restoredScale * PointSize() / scale);
    currentScale = restoredScale;
    return S_OK;
    }