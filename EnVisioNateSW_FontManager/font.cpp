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

#include "font.h"
#include "adobeGlyphList.h"

    font::font(char *pszClientName) {

    if ( pszClientName )
        strcpy(szClientFamily,pszClientName);
    else
        memset(szClientFamily,0,sizeof(szClientFamily));

    matrixStack.push(new matrix());

    return;
    }


    font::font(font &rhs) {

    strcpy(szClientFamily,rhs.szClientFamily);
    strcpy(szInstalledFamily,rhs.szInstalledFamily);
    strcpy(szInstalledQualifier,rhs.szInstalledQualifier);

    matrix *pFontMatrix = new matrix();
    pFontMatrix -> copyFrom(rhs.matrixStack.top());

    matrixStack.push(pFontMatrix);

    theFontType = rhs.theFontType;

    dupCount = rhs.dupCount + 1;

    cbFontData = rhs.cbFontData;

    pbFontData = new BYTE[cbFontData];

    memcpy(pbFontData,rhs.pbFontData,cbFontData);

    if ( FontType::type42 == theFontType )
        type42Load(pbFontData);

    for ( std::pair<uint32_t,char *> pPair : rhs.encodingTable ) {
        char *pszCopy = new char[strlen(pPair.second) + 1];
        strcpy(pszCopy,pPair.second);
        encodingTable[pPair.first] = pszCopy;
    }

    for ( std::pair<uint32_t,char *> pPair : rhs.charStringsTable ) {
        char *pszCopy = new char[strlen(pPair.second) + 1];
        strcpy(pszCopy,pPair.second);
        charStringsTable[pPair.first] = pszCopy;
    }

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

    for ( std::pair<uint32_t,char *> pPair : charStringsTable )
        delete [] pPair.second;

    charStringsTable.clear();

    for ( std::pair<uint32_t,char *> pPair : encodingTable )
        delete [] pPair.second;

    encodingTable.clear();

    delete [] pbFontData;

#if 0
    if ( ! ( NULL == pbFontData ) )
        delete pbFontData;

    if ( ! ( NULL == pGlyfTable ) )
        delete pGlyfTable;

    if ( ! ( NULL == pLocaTable ) )
        delete pLocaTable;

    if ( ! ( NULL == pMaxProfileTable ) )
        delete pMaxProfileTable;

    if ( ! ( NULL == pHeadTable ) )
        delete pHeadTable;

    if ( ! ( NULL == pHorizHeadTable ) )
        delete pHorizHeadTable;

    if ( ! ( NULL == pVertHeadTable ) )
        delete pVertHeadTable;

    if ( ! ( NULL == pHorizontalMetricsTable ) )
        delete pHorizontalMetricsTable;

    if ( ! ( NULL == pVerticalMetricsTable ) )
        delete pVerticalMetricsTable;

    if ( ! ( NULL == pOS2Table ) )
        delete pOS2Table;

    if ( ! ( NULL == pPostTable ) )
        delete pPostTable;

    if ( ! ( NULL == pFontVariationsTable ) )
        delete pFontVariationsTable;
#endif

    if ( ! ( NULL == pMaxProfileTable ) )
        delete pMaxProfileTable;

    if ( ! ( NULL == pHeadTable ) )
        delete pHeadTable;

    if ( ! ( NULL == pHorizHeadTable ) )
        delete pHorizHeadTable;

    if ( ! ( NULL == pHorizontalMetricsTable ) )
        delete pHorizontalMetricsTable;

    if ( ! ( NULL == pCmapSubtableFormat4 ) )
        delete pCmapSubtableFormat4;

    if ( ! ( NULL == pPostTable ) )
        delete pPostTable;

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
        uint16_t glyphOffset;
        uint16_t nextOffset;
        BE_TO_LE_16(pGlyphOffset,glyphOffset)
        BE_TO_LE_16(pNextOffset,nextOffset)
        if ( glyphOffset == nextOffset )
            return NULL;
        return (uint8_t *)pGlyfTable + 2 * glyphOffset;
    }

    uint32_t *pGlyphOffset = (uint32_t *)pLocaTable + glyphID;
    uint32_t *pNextOffset = (uint32_t *)pLocaTable + glyphID + 1;
    uint32_t glyphOffset;
    uint32_t nextOffset;
    BE_TO_LE_32(pGlyphOffset,glyphOffset)
    BE_TO_LE_32(pNextOffset,nextOffset)
    if ( glyphOffset == nextOffset )
        return NULL;
    return (uint8_t *)pGlyfTable + glyphOffset;
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