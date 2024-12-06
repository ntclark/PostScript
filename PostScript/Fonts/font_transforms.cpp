
#include "job.h"

    class matrix *font::getMatrix(object *pFontMatrixName) {
    pJob -> push(this);
    if ( NULL == pFontMatrixName ) 
        pJob -> push(pJob -> pFontMatrixLiteral);
    else
        pJob -> push(pFontMatrixName);
    pJob -> operatorKnown();
    if ( pJob -> pFalseConstant == pJob -> pop() ) 
        return NULL;
    pJob -> push(this);
    if ( NULL == pFontMatrixName ) 
        pJob -> push(pJob -> pFontMatrixLiteral);
    else
        pJob -> push(pFontMatrixName);
    pJob -> operatorGet();
    return reinterpret_cast<class matrix *>(pJob -> pop());
    }


    void font::putMatrix(class matrix *pFontMatrix,object *pFontMatrixName) {
    pJob -> push(this);
    if ( NULL == pFontMatrixName )
        pJob -> push(pJob -> pFontMatrixLiteral);
    else
        pJob -> push(pFontMatrixName);
    pJob -> push(pFontMatrix);
    pJob -> operatorPut();
    return;
    }


    void font::setFontMatrix(class matrix *pMatrix) {
    class matrix *pFontMatrix = getMatrix();
    pFontMatrix -> concat(pMatrix);
    putMatrix(pFontMatrix);
    return;
    }


    void font::scalefont(POINT_TYPE scale) {
    class matrix *pFontMatrix = getMatrix();
    pFontMatrix -> scale(scale);
    pointSize = pFontMatrix -> a();
    return;
    }


    void font::transformGlyph(GS_POINT *pPoints,long countPoints) {
    class matrix *pFontMatrix = getMatrix();
    for ( long k = 0; k < countPoints; k++ ) {
        POINT_TYPE x = pPoints[k].x;
        POINT_TYPE y = pPoints[k].y;
        pPoints[k].x = pFontMatrix -> a() * x + pFontMatrix -> b() * y + pFontMatrix -> tx();
        pPoints[k].y = pFontMatrix -> c() * x + pFontMatrix -> d() * y + pFontMatrix -> ty();
    }
    return;
    }


    void font::transformGlyphInPlace(GS_POINT *pPoints,long countPoints) {
    class matrix *pFontMatrix = getMatrix();
    for ( long k = 0; k < countPoints; k++ ) {
        POINT_TYPE x = pPoints[k].x;
        POINT_TYPE y = pPoints[k].y;
        pPoints[k].x = pFontMatrix -> a() * x + pFontMatrix -> b() * y;
        pPoints[k].y = pFontMatrix -> c() * x + pFontMatrix -> d() * y;
    }
    return;
    }


    void font::transformGlyph(GS_POINT *pPoints,long countPoints,class matrix *pSource) {
    for ( long k = 0; k < countPoints; k++ ) {
        POINT_TYPE x = pPoints[k].x;
        POINT_TYPE y = pPoints[k].y;
        pPoints[k].x = pSource -> a() * x + pSource -> b() * y + pSource -> tx();
        pPoints[k].y = pSource -> c() * x + pSource -> d() * y + pSource -> ty();
    }
    return;
    }


    void font::transformGlyphInPlace(GS_POINT *pPoints,long countPoints,class matrix *pSource) {
    for ( long k = 0; k < countPoints; k++ ) {
        POINT_TYPE x = pPoints[k].x;
        POINT_TYPE y = pPoints[k].y;
        pPoints[k].x = pSource -> a() * x + pSource -> b() * y;
        pPoints[k].y = pSource -> c() * x + pSource -> d() * y;
    }
    return;
    }