
#include "FontManager.h"

    long hashCode(char *szLineSettings);


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


    HRESULT font::get_FontType(int *pFontType) {
    if ( ! pFontType )
        return E_POINTER;
    *pFontType = (int)fontType;
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
            *pResult += 65536;

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


    HRESULT font::SetEncoding(UINT_PTR ptrEncoding) {

    encodingTable.clear();

    char *pszEncoding = (char *)ptrEncoding;

    char *p = pszEncoding;
    while ( *p ) {
        *(p + 4) = '\0';
        uint32_t index = atol(p);
        p += 5;
        char szEntry[128];
        strcpy(szEntry,p);
        encodingTable[index] = hashCode(szEntry);
        p += strlen(p) + 1;
    }

    return S_OK;
    }


    HRESULT font::SetCharStrings(UINT_PTR ptrCharStrings) {

    charStringsTable.clear();

    char *pszCharStrings = (char *)ptrCharStrings;

    char *p = pszCharStrings;
    while ( *p ) {
        char *pStart = p;
        while ( ! ( ';' == *p ) )
            p++;
        *p = '\0';
        uint32_t index = hashCode(pStart);
        p++;
        charStringsTable[index] = atol(p);
        p += strlen(p) + 1;
    }

    return S_OK;
    }


    HRESULT font::SaveState() {
    matrixStack.push(new matrix(*matrixStack.top()));
    return S_OK;
    }


    HRESULT font::RestoreState() {
    if ( 1 == matrixStack.size() ) {
        //matrixStack.top() -> identity();
        //currentScale = 1.0f;
        //PointSize(1.0f);
        return S_OK;
    }
    FLOAT scale = matrixStack.top() -> XForm() -> eM11;
    delete matrixStack.top();
    matrixStack.pop();
    FLOAT restoredScale = matrixStack.top() -> XForm() -> eM11;
    PointSize(restoredScale * PointSize() / scale);
    currentScale = restoredScale;
    return S_OK;
    }


    long hashCode(char *szLineSettings) {
    long hashCode = 0L;
    long part = 0L;
    long n = (DWORD)strlen(szLineSettings);
    char *psz = new char[n + 4];
    memset(psz,0,(n + 4) * sizeof(char));
    strcpy(psz,szLineSettings);
    char *p = psz;
    for ( long k = 0; k < n; k += 4 ) {
        memcpy(&part,p,4 * sizeof(char));
        hashCode ^= part;
        p += 4;
    }
    delete [] psz;
    return hashCode;
    }