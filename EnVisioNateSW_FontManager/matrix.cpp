
#include "EnVisioNateSW_FontManager.h"

#include "matrix.h"

int Mx3Inverse(double *sourceMatrix,double *targetMatrix);

    matrix::matrix() {
    identity();
    return;
    }

    matrix::matrix(FLOAT a,FLOAT b,FLOAT c,FLOAT d,FLOAT tx,FLOAT ty) {
    xForm.eM11 = a;
    xForm.eM12 = b;
    xForm.eM21 = c;
    xForm.eM22 = d;
    xForm.eDx = tx;
    xForm.eDy = ty;
    return;
    }

    matrix::matrix(FLOAT *pv) {
    xForm.eM11 = pv[0];
    xForm.eM12 = pv[1];
    xForm.eM21 = pv[2];
    xForm.eM22 = pv[3];
    xForm.eDx = pv[4];
    xForm.eDy = pv[5];
    return;
    }

    matrix::matrix(matrix &rhs) {
    xForm.eM11 = rhs.xForm.eM11;
    xForm.eM12 = rhs.xForm.eM12;
    xForm.eM21 = rhs.xForm.eM21;
    xForm.eM22 = rhs.xForm.eM22;
    xForm.eDx = rhs.xForm.eDx;
    xForm.eDy = rhs.xForm.eDy;
    return;
    }


    matrix::~matrix() {
    return;
    }



    //void matrix::SetValue(long index,FLOAT value) {
    //values[index] = value;
    //return;
    //}


    //void matrix::SetValues(FLOAT *pValues) {
    //a(pValues[A]);
    //b(pValues[B]);
    //c(pValues[C]);
    //d(pValues[D]);
    //tx(pValues[TX]);
    //ty(pValues[TY]);
    //return;
    //}


    void matrix::identity() {
    xForm.eM11 = 1.0f;
    xForm.eM12 = 0.0f;
    xForm.eM21 = 0.0f;
    xForm.eM22 = 1.0f;
    xForm.eDx = 0.0f;
    xForm.eDy = 0.0f;
    return;
    }


    void matrix::copyFrom(matrix *pOther) {
    xForm.eM11 = pOther -> xForm.eM11;
    xForm.eM12 = pOther -> xForm.eM12;
    xForm.eM21 = pOther -> xForm.eM21;
    xForm.eM22 = pOther -> xForm.eM22;
    xForm.eDx = pOther -> xForm.eDx;
    xForm.eDy = pOther -> xForm.eDy;
    return;
    }


    void matrix::SetMatrix(XFORM *pSource) {
    xForm.eM11 = pSource -> eM11;
    xForm.eM12 = pSource -> eM12;
    xForm.eM21 = pSource -> eM21;
    xForm.eM22 = pSource -> eM22;
    xForm.eDx = pSource -> eDx;
    xForm.eDy = pSource -> eDy;
    return;
    }


    void matrix::concat(matrix *pSource) {
    concat(&pSource -> xForm);
    return;
    }


    //void matrix::concat(XFORM &winXForm) {
    //matrix::concat(reinterpret_cast<FLOAT *>(&winXForm));
    //return;
    //}

    void matrix::concat(XFORM *pPrime) {

//                                              A                       B
//   |  a'   b'   0  |   | a  b  0 |    |  a'*a + b'*c + 0*tx ,  a'*b + b'*d + 0*ty ,  a'*0 + b'*0 + 0*1
//                                              C                       D
//   |  c'   d'   0  | * | c  d  0 | =  |  c'*a + d'*c + 0*tx ,  c'*b + d'*d + 0*ty ,  c'*0 + d'*0 + 0*1
//                                              TX                      TY
//   |  tx'  ty'  1  |   | tx ty 1 |    | tx'*a + ty'*c + 1*tx , tx'*b + ty'*d + 1*ty , tx'*0 + ty'*0 + 1*1
//
    XFORM result;
    CombineTransform(&result,pPrime,&xForm);
    memcpy(&xForm,&result,sizeof(XFORM));
    return;
    }


    void matrix::scale(FLOAT scaleFactor) {
    XFORM scaleMatrix{scaleFactor,0.0f,0.0f,scaleFactor,0.0f,0.0f};
    concat(&scaleMatrix);
    return;
    }


    void matrix::scale(FLOAT scaleX,FLOAT scaleY) {
    XFORM scaleMatrix{scaleX,0.0f,0.0f,scaleY,0.0f,0.0f};
    concat(&scaleMatrix);
    return;
    }


    void matrix::translate(GS_POINT& translateTo) {
    XFORM translateMatrix{1.0f,0.0f,0.0f,1.0f,translateTo.x,translateTo.y};
    concat(&translateMatrix);
    return;
    }


    void matrix::move(GS_POINT& translateTo) {
    xForm.eDx = translateTo.x;
    xForm.eDy = translateTo.y;
    return;
    }


    void matrix::move(FLOAT x,FLOAT y) {
    xForm.eDx = x;
    xForm.eDy = y;
    return;
    }


    void matrix::transformPoints(GS_POINT *pPoints,uint16_t pointCount) {
    for ( long k = 0; k < pointCount; k++ ) {
        FLOAT x = pPoints[k].x;
        FLOAT y = pPoints[k].y;
        pPoints[k].x = xForm.eM11 * x + xForm.eM12 * y + xForm.eDx;//values[A] * x + values[B] * y + values[TX];
        pPoints[k].y = xForm.eM21 * x + xForm.eM22 * y + xForm.eDy;//values[C] * x + values[D] * y + values[TY];
    }
    return;
    }


    void matrix::transformPoints(POINT *pPoints,uint16_t pointCount) {
    for ( long k = 0; k < pointCount; k++ ) {
        FLOAT x = (FLOAT)pPoints[k].x;
        FLOAT y = (FLOAT)pPoints[k].y;
        FLOAT x2 = xForm.eM11 * x + xForm.eM12 * y + xForm.eDx;//values[A] * x + values[B] * y + values[TX];
        FLOAT y2 = xForm.eM21 * x + xForm.eM22 * y + xForm.eDy;//values[C] * x + values[D] * y + values[TY];
        pPoints[k].x = (LONG)x2;
        pPoints[k].y = (LONG)y2;
    }
    return;
    }


    void matrix::transformPoints(XFORM *pXForm,GS_POINT *pPoints,uint16_t pointCount) {
    for ( long k = 0; k < pointCount; k++ ) {
        FLOAT x = pPoints[k].x;
        FLOAT y = pPoints[k].y;
        FLOAT x2 = pXForm -> eM11 * x + pXForm -> eM12 * y + pXForm -> eDx;//xForm[A] * x + xForm[B] * y + xForm[TX];
        FLOAT y2 = pXForm -> eM21 * x + pXForm -> eM22 * y + pXForm -> eDy;//xForm[C] * x + xForm[D] * y + xForm[TY];
        pPoints[k].x = x2;
        pPoints[k].y = y2;
    }
    return;
    }