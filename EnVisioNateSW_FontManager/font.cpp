
#include "font.h"

    char font::szFailureMessage[]{'\0'};

    font::font(char *pszClientName) {

    if ( pszClientName )
        strcpy(szClientFamily,pszClientName);
    else
        memset(szClientFamily,0,sizeof(szClientFamily));

    matrix *pFontMatrix = new matrix();

    matrixStack.push(pFontMatrix);

    return;
    }


    font::font(font &rhs) {

    strcpy(szClientFamily,rhs.szClientFamily);
    strcpy(szInstalledFamily,rhs.szInstalledFamily);
    strcpy(szInstalledQualifier,rhs.szInstalledQualifier);

    matrix *pFontMatrix = new matrix();
    pFontMatrix -> copyFrom(rhs.matrixStack.top());

    matrixStack.push(pFontMatrix);

    fontType = rhs.fontType;

    dupCount = rhs.dupCount + 1;

    cbFontData = rhs.cbFontData;

    pbFontData = new BYTE[cbFontData];

    memcpy(pbFontData,rhs.pbFontData,cbFontData);

    if ( ftype42 == fontType )
        type42Load(pbFontData);

    return;
    }


    font::~font() {
    while ( 0 < matrixStack.size() ) {
        delete matrixStack.top();
        matrixStack.pop();
    }
    for ( char *p : fontFullNames )
        delete [] p;
    for ( char *p : fontStyleNames )
        delete [] p;
    for ( char *p : fontScriptNames )
        delete [] p;
    fontFullNames.clear();
    fontStyleNames.clear();
    fontScriptNames.clear();
    fontWeights.clear();
    return;
    }


    char *font::InstalledFontName() {
    static char szFullName[256];
    sprintf_s<256>(szFullName,"%s",szInstalledFamily);
    if ( ! ( '\0' == szInstalledQualifier[0] ) ) {
        strcat(szFullName," ");
        strcat(szFullName,szInstalledQualifier);
    }
    return szFullName;
    }


    uint8_t *font::getGlyphData(unsigned short glyphID) {

    /*
    There are two formats of the loca table: a short format, and a long format. 
    The format is specified by the indexToLocFormat entry in the 'head' table.
    Short format
        Offset16    offsets[numGlyphs + 1]  The local offset divided by 2 is stored.
    Long format
        Offset32    offsets[numGlyphs + 1]  The actual local offset is stored.
    */

    if ( 0 == pHeadTable -> indexToLocFormat ) {
        uint16_t *pGlyphOffset = (uint16_t *)pLocaTable + glyphID;
        uint16_t *pNextOffset = (uint16_t *)pLocaTable + glyphID + 1;
        if ( *pGlyphOffset == *pNextOffset ) 
            return NULL;
        uint16_t offsetInGlyphTable = 0;
        BE_TO_LE_U16(pGlyphOffset,offsetInGlyphTable)
        return (uint8_t *)pGlyfTable + 2 * offsetInGlyphTable;
    }

    uint32_t *pGlyphOffset = (uint32_t *)pLocaTable + glyphID;
    uint32_t *pNextOffset = (uint32_t *)pLocaTable + glyphID + 1;
    if ( *pGlyphOffset == *pNextOffset ) 
        return NULL;
    uint32_t offsetInGlyphTable = 0;
    BE_TO_LE_U32(pGlyphOffset,offsetInGlyphTable)
    return (uint8_t *)pGlyfTable + offsetInGlyphTable;
    }


    FLOAT font::PointSize(FLOAT v) { 
    if ( -FLT_MAX == v ) 
        return pointSize; 
    FLOAT oldPointSize = pointSize; 
    pointSize = v; 
    return oldPointSize; 
    }


    void font::concat(matrix *pConcat) {
    matrix *pNewMatrix = new matrix();
    pNewMatrix -> copyFrom(matrixStack.top());
    pNewMatrix -> concat(pConcat);
    pushMatrix(pNewMatrix);
    return;
    }


    void font::pushMatrix(matrix *pMatrix) {
    matrixStack.push(pMatrix);
    return;
    }


    void font::restoreMatrix() {
    matrix *pMatrix = matrixStack.top();
    matrixStack.pop();
    delete pMatrix;
    return;
    }


    void font::scaleFont(FLOAT scale) {
    matrix *pFontMatrix = matrixStack.top();
    pFontMatrix -> scale(scale);
    return;
    }


    void font::transformGlyph(GS_POINT *pPoints,long countPoints) {
    matrix *pFontMatrix = matrixStack.top();
    FLOAT aValue = pFontMatrix -> XForm() -> eM11;
    FLOAT bValue = pFontMatrix -> XForm() -> eM12;
    FLOAT cValue = pFontMatrix -> XForm() -> eM21;
    FLOAT dValue = pFontMatrix -> XForm() -> eM22;
    FLOAT txValue = pFontMatrix -> XForm() -> eDx;
    FLOAT tyValue = pFontMatrix -> XForm() -> eDy;
    for ( long k = 0; k < countPoints; k++ ) {
        FLOAT x = pPoints[k].x;
        FLOAT y = pPoints[k].y;
        pPoints[k].x = aValue * x + bValue * y + txValue;
        pPoints[k].y = cValue * x + dValue * y + tyValue;
    }
    return;
    }


    void font::transformGlyphInPlace(GS_POINT *pPoints,long countPoints) {
    matrix *pFontMatrix = matrixStack.top();
    FLOAT aValue = pFontMatrix -> XForm() -> eM11;
    FLOAT bValue = pFontMatrix -> XForm() -> eM12;
    FLOAT cValue = pFontMatrix -> XForm() -> eM21;
    FLOAT dValue = pFontMatrix -> XForm() -> eM22;
    for ( long k = 0; k < countPoints; k++ ) {
        FLOAT x = pPoints[k].x;
        FLOAT y = pPoints[k].y;
        pPoints[k].x = aValue * x + bValue * y;
        pPoints[k].y = cValue * x + dValue * y;
    }
    return;
    }


    void font::transformGlyph(GS_POINT *pPoints,long countPoints,matrix *pSource) {
    FLOAT aValue = pSource -> XForm() -> eM11;
    FLOAT bValue = pSource -> XForm() -> eM12;
    FLOAT cValue = pSource -> XForm() -> eM21;
    FLOAT dValue = pSource -> XForm() -> eM22;
    FLOAT txValue = pSource -> XForm() -> eDx;
    FLOAT tyValue = pSource -> XForm() -> eDy;
    for ( long k = 0; k < countPoints; k++ ) {
        FLOAT x = pPoints[k].x;
        FLOAT y = pPoints[k].y;
        pPoints[k].x = pSource -> XForm() -> eM11 * x + bValue * y + txValue;
        pPoints[k].y = cValue * x + dValue * y + tyValue;
    }
    return;
    }


    void font::transformGlyphInPlace(GS_POINT *pPoints,long countPoints,matrix *pSource) {
    FLOAT aValue = pSource -> XForm() -> eM11;
    FLOAT bValue = pSource -> XForm() -> eM12;
    FLOAT cValue = pSource -> XForm() -> eM21;
    FLOAT dValue = pSource -> XForm() -> eM22;
    for ( long k = 0; k < countPoints; k++ ) {
        FLOAT x = pPoints[k].x;
        FLOAT y = pPoints[k].y;
        pPoints[k].x = aValue * x + bValue * y;
        pPoints[k].y = cValue * x + dValue * y;
    }
    return;
    }