
#include "job.h"

#include "PostScript objects/graphicsState.h"
#include "PostScript objects/matrix.h"
#include <stdio.h>

int Mx3Inverse(double *sourceMatrix,double *targetMatrix);


    matrix::matrix(job *pJob) :
        values{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f},
        array(pJob,6)
    {
    theObjectType = object::objectType::matrix;
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

    void matrix::SetValue(long index,POINT_TYPE value) {
    entries[index] = new (pJob -> CurrentObjectHeap()) object(pJob,value);
    values[index] = entries[index] -> FloatValue();
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
    values[A] = 1.0f;
    values[B] = 0.0f;
    values[C] = 0.0f;
    values[D] = 1.0f;
    values[TX] = 0.0f;
    values[TY] = 0.0f;
    return;
    }


    void matrix::copyFrom(matrix *pOther) {
    values[A] = pOther -> a();
    values[B] = pOther -> b();
    values[C] = pOther -> c();
    values[D] = pOther -> d();
    values[TX] = pOther -> tx();
    values[TY] = pOther -> ty();
    a(values[A]);
    b(values[B]);
    c(values[C]);
    d(values[D]);
    tx(values[TX]);
    ty(values[TY]);
    return;
    }

    POINT_TYPE matrix::a() { return values[A]; }
    POINT_TYPE matrix::b() { return values[B]; }
    POINT_TYPE matrix::c() { return values[C]; }
    POINT_TYPE matrix::d() { return values[D]; }
    POINT_TYPE matrix::tx() { return values[TX]; }
    POINT_TYPE matrix::ty() { return values[TY]; }

    POINT_TYPE matrix::aInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[0]; }
    POINT_TYPE matrix::bInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[1]; }
    POINT_TYPE matrix::cInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[2]; }
    POINT_TYPE matrix::dInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[3]; }
    POINT_TYPE matrix::txInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[4]; }
    POINT_TYPE matrix::tyInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[5]; }

    void matrix::a(POINT_TYPE v) { invalidated = true; getElement(A) -> OBJECT_SET_POINT_TYPE_VALUE(v); values[A] = v; }
    void matrix::b(POINT_TYPE v) { invalidated = true; getElement(B) -> OBJECT_SET_POINT_TYPE_VALUE(v); values[B] = v; }
    void matrix::c(POINT_TYPE v) { invalidated = true; getElement(C) -> OBJECT_SET_POINT_TYPE_VALUE(v); values[C] = v; }
    void matrix::d(POINT_TYPE v) { invalidated = true; getElement(D) -> OBJECT_SET_POINT_TYPE_VALUE(v); values[D] = v; }
    void matrix::tx(POINT_TYPE v) { invalidated = true; getElement(TX) -> OBJECT_SET_POINT_TYPE_VALUE(v); values[TX] = v; }
    void matrix::ty(POINT_TYPE v) { invalidated = true; getElement(TY) -> OBJECT_SET_POINT_TYPE_VALUE(v); values[TY] = v; }

    void matrix::concat(matrix *pSource) {
    concat(pSource -> values);
    return;
    }

    void matrix::concat(array *pSource) {
    POINT_TYPE arrayValues[] = {
            pSource -> getElement(A) -> OBJECT_POINT_TYPE_VALUE,pSource -> getElement(B) -> OBJECT_POINT_TYPE_VALUE,
            pSource -> getElement(C) -> OBJECT_POINT_TYPE_VALUE,pSource -> getElement(D) -> OBJECT_POINT_TYPE_VALUE,
            pSource -> getElement(TX) -> OBJECT_POINT_TYPE_VALUE,pSource -> getElement(TY) -> OBJECT_POINT_TYPE_VALUE};
    matrix::concat(arrayValues);
    return;
    }

    void matrix::concat(XFORM &winXForm) {
    matrix::concat(reinterpret_cast<POINT_TYPE *>(&winXForm));
    return;
    }

    void matrix::concat(POINT_TYPE *pPrime) {

//                                              A                       B
//   |  a'   b'   0  |   | a  b  0 |    |  a'*a + b'*c + 0*tx ,  a'*b + b'*d + 0*ty ,  a'*0 + b'*0 + 0*1
//                                              C                       D
//   |  c'   d'   0  | * | c  d  0 | =  |  c'*a + d'*c + 0*tx ,  c'*b + d'*d + 0*ty ,  c'*0 + d'*0 + 0*1
//                                              TX                      TY
//   |  tx'  ty'  1  |   | tx ty 1 |    | tx'*a + ty'*c + 1*tx , tx'*b + ty'*d + 1*ty , tx'*0 + ty'*0 + 1*1
//
    POINT_TYPE newValues[]{0.0,0.0,0.0,0.0,0.0,0.0};

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


    void matrix::scale(POINT_TYPE scaleFactor) {
    POINT_TYPE sfValues[]{scaleFactor,0.0,0.0,scaleFactor,0.0,0.0};
    concat(sfValues);
    return;
    }


    void matrix::scale(POINT_TYPE scaleX,POINT_TYPE scaleY) {
    POINT_TYPE sfValues[]{scaleX,0.0,0.0,scaleY,0.0,0.0};
    concat(sfValues);
    return;
    }


    void matrix::scale(XFORM *pXForm,FLOAT scale) {
    FLOAT sfValues[]{scale,0.0,0.0,scale,0.0,0.0};
    FLOAT newValues[]{0.0,0.0,0.0,0.0,0.0,0.0};
    newValues[A] = sfValues[A] * pXForm -> eM11 + sfValues[B] * pXForm -> eM21;
    newValues[B] = sfValues[A] * pXForm -> eM12 + sfValues[B] * pXForm -> eM22;
    newValues[C] = sfValues[C] * pXForm -> eM11 + sfValues[D] * pXForm -> eM21;
    newValues[D] = sfValues[C] * pXForm -> eM12 + sfValues[D] * pXForm -> eM22;
    newValues[TX] = sfValues[TX] * pXForm -> eM11 + sfValues[TY] * pXForm -> eM21 + pXForm -> eDx;
    newValues[TY] = sfValues[TX] * pXForm -> eM12 + sfValues[TY] * pXForm -> eM22 + pXForm -> eDy;
    memcpy(pXForm,newValues,sizeof(XFORM));
    return;
    }


    void matrix::translate(POINT_TYPE x,POINT_TYPE y) {
    POINT_TYPE transValues[]{1.0,0.0,0.0,1.0,x,y};
    concat(transValues);
    return;
    }


    void matrix::rotate(POINT_TYPE angle) {
    POINT_TYPE cosAngle = cos(-angle * graphicsState::degToRad);
    POINT_TYPE sinAngle = sin(-angle * graphicsState::degToRad);
    POINT_TYPE rotValues[] = {cosAngle,sinAngle,-sinAngle,cosAngle,0.0,0.0};
    concat(rotValues);
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

    inverse[A] = (POINT_TYPE)theMatrixInverted[0][0];
    inverse[B] = (POINT_TYPE)theMatrixInverted[0][1];
    inverse[C] = (POINT_TYPE)theMatrixInverted[1][0];
    inverse[D] = (POINT_TYPE)theMatrixInverted[1][1];

    inverse[TX] = (POINT_TYPE)theMatrixInverted[0][2];
    inverse[TY] = (POINT_TYPE)theMatrixInverted[1][2];

    invalidated = false;

    return;
    }