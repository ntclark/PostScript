
#include "font.h"

    char font::szFailureMessage[]{'\0'};


    font::font(char *pszName) {

    if ( pszName )
        strcpy(szFamily,pszName);
    else
        memset(szFamily,0,sizeof(szFamily));

    MultiByteToWideChar(CP_ACP,0,szFamily,-1,szwFamily,64);

    matrix *pFontMatrix = new matrix();

    matrixStack.push(pFontMatrix);

    return;
    }


    font::font(font &rhs) {

    strcpy(szFamily,rhs.szFamily);
    wcscpy(szwFamily,rhs.szwFamily);

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
    return;
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
    FLOAT aValue = pFontMatrix -> a();
    FLOAT bValue = pFontMatrix -> b();
    FLOAT cValue = pFontMatrix -> c();
    FLOAT dValue = pFontMatrix -> d();
    FLOAT txValue = pFontMatrix -> tx();
    FLOAT tyValue = pFontMatrix -> ty();
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
    FLOAT aValue = pFontMatrix -> a();
    FLOAT bValue = pFontMatrix -> b();
    FLOAT cValue = pFontMatrix -> c();
    FLOAT dValue = pFontMatrix -> d();
    for ( long k = 0; k < countPoints; k++ ) {
        FLOAT x = pPoints[k].x;
        FLOAT y = pPoints[k].y;
        pPoints[k].x = aValue * x + bValue * y;
        pPoints[k].y = cValue * x + dValue * y;
    }
    return;
    }


    void font::transformGlyph(GS_POINT *pPoints,long countPoints,class matrix *pSource) {
    FLOAT aValue = pSource -> a();
    FLOAT bValue = pSource -> b();
    FLOAT cValue = pSource -> c();
    FLOAT dValue = pSource -> d();
    FLOAT txValue = pSource -> tx();
    FLOAT tyValue = pSource -> ty();
    for ( long k = 0; k < countPoints; k++ ) {
        FLOAT x = pPoints[k].x;
        FLOAT y = pPoints[k].y;
        pPoints[k].x = aValue * x + bValue * y + txValue;
        pPoints[k].y = cValue * x + dValue * y + tyValue;
    }
    return;
    }


    void font::transformGlyphInPlace(GS_POINT *pPoints,long countPoints,class matrix *pSource) {
    FLOAT aValue = pSource -> a();
    FLOAT bValue = pSource -> b();
    FLOAT cValue = pSource -> c();
    FLOAT dValue = pSource -> d();
    for ( long k = 0; k < countPoints; k++ ) {
        FLOAT x = pPoints[k].x;
        FLOAT y = pPoints[k].y;
        pPoints[k].x = aValue * x + bValue * y;
        pPoints[k].y = cValue * x + dValue * y;
    }
    return;
    }