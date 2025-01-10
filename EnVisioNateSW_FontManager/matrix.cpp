
#include "EnVisioNateSW_FontManager.h"

#include "matrix.h"

int Mx3Inverse(double *sourceMatrix,double *targetMatrix);

    matrix::matrix() {
    a(1.0f);
    b(0.0f);
    c(0.0f);
    d(1.0f);
    tx(0.0f);
    ty(0.0f);
    return;
    }

    matrix::~matrix() {
    return;
    }


    void matrix::SetValue(long index,FLOAT value) {
    values[index] = value;
    return;
    }


    void matrix::SetValues(FLOAT *pValues) {
    a(pValues[A]);
    b(pValues[B]);
    c(pValues[C]);
    d(pValues[D]);
    tx(pValues[TX]);
    ty(pValues[TY]);
    return;
    }


    char *matrix::ToString() {
    static char szMatrix[64];
    sprintf_s<64>(szMatrix,"%5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f",a(),b(),c(),d(),tx(),ty());
    return szMatrix;
    }


    void matrix::identity() {
    a(1.0);
    b(0.0);
    c(0.0);
    d(1.0);
    tx(0.0);
    ty(0.0);
    return;
    }


    void matrix::copyFrom(matrix *pOther) {
    a(pOther -> a());
    b(pOther -> b());
    c(pOther -> c());
    d(pOther -> d());
    tx(pOther -> tx());
    ty(pOther -> ty());
    return;
    }

    FLOAT matrix::a() { return values[A]; }
    FLOAT matrix::b() { return values[B]; }
    FLOAT matrix::c() { return values[C]; }
    FLOAT matrix::d() { return values[D]; }
    FLOAT matrix::tx() { return values[TX]; }
    FLOAT matrix::ty() { return values[TY]; }

    FLOAT matrix::aInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[A]; }
    FLOAT matrix::bInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[B]; }
    FLOAT matrix::cInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[C]; }
    FLOAT matrix::dInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[D]; }
    FLOAT matrix::txInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[TX]; }
    FLOAT matrix::tyInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[TY]; }

    void matrix::a(FLOAT v) { invalidated = true; values[A] = v; }
    void matrix::b(FLOAT v) { invalidated = true; values[B] = v; }
    void matrix::c(FLOAT v) { invalidated = true; values[C] = v; }
    void matrix::d(FLOAT v) { invalidated = true; values[D] = v; }
    void matrix::tx(FLOAT v) { invalidated = true; values[TX] = v; }
    void matrix::ty(FLOAT v) { invalidated = true; values[TY] = v; }

    void matrix::concat(matrix *pSource) {
    concat(pSource -> values);
    return;
    }


    void matrix::concat(XFORM &winXForm) {
    matrix::concat(reinterpret_cast<FLOAT *>(&winXForm));
    return;
    }

    void matrix::concat(FLOAT *pPrime) {

//                                              A                       B
//   |  a'   b'   0  |   | a  b  0 |    |  a'*a + b'*c + 0*tx ,  a'*b + b'*d + 0*ty ,  a'*0 + b'*0 + 0*1
//                                              C                       D
//   |  c'   d'   0  | * | c  d  0 | =  |  c'*a + d'*c + 0*tx ,  c'*b + d'*d + 0*ty ,  c'*0 + d'*0 + 0*1
//                                              TX                      TY
//   |  tx'  ty'  1  |   | tx ty 1 |    | tx'*a + ty'*c + 1*tx , tx'*b + ty'*d + 1*ty , tx'*0 + ty'*0 + 1*1
//
    FLOAT newValues[]{0.0,0.0,0.0,0.0,0.0,0.0};

    newValues[A] = pPrime[A] * a() + pPrime[B] * c();
    newValues[B] = pPrime[A] * b() + pPrime[B] * d();
    newValues[C] = pPrime[C] * a() + pPrime[D] * c();
    newValues[D] = pPrime[C] * b() + pPrime[D] * d();

    newValues[TX] = pPrime[TX] * a() + pPrime[TY] * c() + tx();
    newValues[TY] = pPrime[TX] * b() + pPrime[TY] * d() + ty();

    revertValues[A] = a();
    revertValues[B] = b();
    revertValues[C] = c();
    revertValues[D] = d();
    revertValues[TX] = tx();
    revertValues[TY] = ty();

    a(newValues[A]);
    b(newValues[B]);
    c(newValues[C]);
    d(newValues[D]);
    tx(newValues[TX]);
    ty(newValues[TY]);

    return;
    }


    void matrix::revertMatrix() {
    a(revertValues[A]);
    b(revertValues[B]);
    c(revertValues[C]);
    d(revertValues[D]);
    tx(revertValues[TX]);
    ty(revertValues[TY]);
    invalidated = true;
    return;
    }


    void matrix::scale(FLOAT scaleFactor) {
    FLOAT scaleMatrix[]{scaleFactor,0.0f,0.0f,scaleFactor,0.0f,0.0f};
    concat(scaleMatrix);
    return;
    }


    void matrix::scale(FLOAT scaleX,FLOAT scaleY) {
    matrix scaleMatrix{scaleX,0.0f,0.0f,scaleY,0.0f,0.0f};
    concat(&scaleMatrix);
    return;
    }


    void matrix::translate(GS_POINT& translateTo) {
    matrix translateMatrix{1.0f,0.0f,0.0f,1.0f,translateTo.x,translateTo.y};
    concat(&translateMatrix);
    return;
    }


    void matrix::move(GS_POINT& translateTo) {
    matrix translateMatrix{1.0f,0.0f,0.0f,1.0f,translateTo.x,translateTo.y};
    tx(translateTo.x);
    ty(translateTo.y);
    return;
    }


    void matrix::invert() {

    double theMatrix[3][3];
    double theMatrixInverted[3][3];

    theMatrix[0][0] = (double)a();
    theMatrix[0][1] = (double)b();
    theMatrix[0][2] = (double)tx();
    theMatrix[1][0] = (double)c();
    theMatrix[1][1] = (double)d();
    theMatrix[1][2] = (double)ty();

    theMatrix[2][0] = 0.0;
    theMatrix[2][1] = 0.0;
    theMatrix[2][2] = 1.0f;

    Mx3Inverse(&theMatrix[0][0],&theMatrixInverted[0][0]);

    inverse[A] = (FLOAT)theMatrixInverted[0][0];
    inverse[B] = (FLOAT)theMatrixInverted[0][1];
    inverse[C] = (FLOAT)theMatrixInverted[1][0];
    inverse[D] = (FLOAT)theMatrixInverted[1][1];

    inverse[TX] = (FLOAT)theMatrixInverted[0][2];
    inverse[TY] = (FLOAT)theMatrixInverted[1][2];

    invalidated = false;

    return;
    }


    void matrix::transformPoints(GS_POINT *pPoints,uint16_t pointCount) {
    for ( long k = 0; k < pointCount; k++ ) {
        FLOAT x = pPoints[k].x;
        FLOAT y = pPoints[k].y;
        pPoints[k].x = values[A] * x + values[B] * y + values[TX];
        pPoints[k].y = values[C] * x + values[D] * y + values[TY];
    }
    return;
    }


    void matrix::transformPoints(POINT *pPoints,uint16_t pointCount) {
    for ( long k = 0; k < pointCount; k++ ) {
        FLOAT x = (FLOAT)pPoints[k].x;
        FLOAT y = (FLOAT)pPoints[k].y;
        FLOAT x2 = values[A] * x + values[B] * y + values[TX];
        FLOAT y2 = values[C] * x + values[D] * y + values[TY];
        pPoints[k].x = (LONG)x2;
        pPoints[k].y = (LONG)y2;
    }
    return;
    }


    void matrix::transformPoints(FLOAT *xForm,GS_POINT *pPoints,uint16_t pointCount) {
    for ( long k = 0; k < pointCount; k++ ) {
        FLOAT x = pPoints[k].x;
        FLOAT y = pPoints[k].y;
        FLOAT x2 = xForm[A] * x + xForm[B] * y + xForm[TX];
        FLOAT y2 = xForm[C] * x + xForm[D] * y + xForm[TY];
        pPoints[k].x = x2;
        pPoints[k].y = y2;
    }
    return;
    }