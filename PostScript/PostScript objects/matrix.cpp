
#include "job.h"

#include "PostScript objects/graphicsState.h"
#include "PostScript objects/matrix.h"
#include <stdio.h>

int Mx3Inverse(double *sourceMatrix,double *targetMatrix);

    matrix::matrix(job *pJob) :
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

    POINT_TYPE matrix::a() { return getElement(A) -> OBJECT_POINT_TYPE_VALUE; }
    POINT_TYPE matrix::b() { return getElement(B) -> OBJECT_POINT_TYPE_VALUE; }
    POINT_TYPE matrix::c() { return getElement(C) -> OBJECT_POINT_TYPE_VALUE; }
    POINT_TYPE matrix::d() { return getElement(D) -> OBJECT_POINT_TYPE_VALUE; }
    POINT_TYPE matrix::tx() { return getElement(TX) -> OBJECT_POINT_TYPE_VALUE; }
    POINT_TYPE matrix::ty() { return getElement(TY) -> OBJECT_POINT_TYPE_VALUE; }

    POINT_TYPE matrix::aInverse() { if ( invalidated ) invert(); return inverse[0]; }
    POINT_TYPE matrix::bInverse() { if ( invalidated ) invert(); return inverse[1]; }
    POINT_TYPE matrix::cInverse() { if ( invalidated ) invert(); return inverse[2]; }
    POINT_TYPE matrix::dInverse() { if ( invalidated ) invert(); return inverse[3]; }
    POINT_TYPE matrix::txInverse() { if ( invalidated ) invert(); return inverse[4]; }
    POINT_TYPE matrix::tyInverse() { if ( invalidated ) invert(); return inverse[5]; }

    void matrix::a(POINT_TYPE v) { invalidated = true; getElement(A) -> OBJECT_SET_POINT_TYPE_VALUE(v); }
    void matrix::b(POINT_TYPE v) { invalidated = true; getElement(B) -> OBJECT_SET_POINT_TYPE_VALUE(v); }
    void matrix::c(POINT_TYPE v) { invalidated = true; getElement(C) -> OBJECT_SET_POINT_TYPE_VALUE(v); }
    void matrix::d(POINT_TYPE v) { invalidated = true; getElement(D) -> OBJECT_SET_POINT_TYPE_VALUE(v); }
    void matrix::tx(POINT_TYPE v) { invalidated = true; getElement(TX) -> OBJECT_SET_POINT_TYPE_VALUE(v); }
    void matrix::ty(POINT_TYPE v) { invalidated = true; getElement(TY) -> OBJECT_SET_POINT_TYPE_VALUE(v); }

    void matrix::concat(matrix *pSource) {
    concat(reinterpret_cast<array *>(pSource));
    return;
    }

    void matrix::concat(array *pSource) {
    POINT_TYPE values[] = {pSource -> getElement(A) -> OBJECT_POINT_TYPE_VALUE,pSource -> getElement(B) -> OBJECT_POINT_TYPE_VALUE,
                        pSource -> getElement(C) -> OBJECT_POINT_TYPE_VALUE,pSource -> getElement(D) -> OBJECT_POINT_TYPE_VALUE,
                        pSource -> getElement(TX) -> OBJECT_POINT_TYPE_VALUE,pSource -> getElement(TY) -> OBJECT_POINT_TYPE_VALUE};
    matrix::concat(values);
    return;
    }

    void matrix::concat(XFORM &winXForm) {
    matrix::concat(reinterpret_cast<POINT_TYPE *>(&winXForm));
    return;
    }

    void matrix::concat(POINT_TYPE *pPrime) {

//                                              A                       B
//   |  a'   b'   0  |   | a  b  0 |    |  a'*a +  b'*c + 0*tx ,  a'*b +  b'*d + 0*ty ,  a'*0 +  b'*0 + 0*1
//                                              C                       D
//   |  c'   d'   0  | * | c  d  0 | =  |  c'*a +  d'*c + 0*tx ,  c'*b +  d'*d + 0*ty ,  c'*0 +  d'*0 + 0*1
//                                              TX                      TY
//   |  tx'  ty'  1  |   | tx ty 1 |    | tx'*a + ty'*c + 1*tx , tx'*b + ty'*d + 1*ty , tx'*0 + ty'*0 + 1*1
//
    POINT_TYPE newValues[] = {0.0,0.0,0.0,0.0,0.0,0.0};

    newValues[A] = pPrime[A] * a() - pPrime[B] * c();   newValues[B] = pPrime[A] * b() - pPrime[B] * d();
    newValues[C] = -pPrime[C] * a() + pPrime[D] * c();   newValues[D] = -pPrime[C] * b() + pPrime[D] * d();

    newValues[TX] = pPrime[TX] * a() + pPrime[TY] * c() + tx();     newValues[TY] = pPrime[TX] * b() + pPrime[TY] * d() + ty();

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


    void matrix::revertTransform() {
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
    a(a() * scaleFactor);
    d(d() * scaleFactor);
    return;
    }


    void matrix::invert() {

    double theMatrix[3][3];
    double theMatrixInverted[3][3];

    theMatrix[0][0] = (double)a();
    theMatrix[0][1] = (double)b();
    theMatrix[0][2] = 0.0f;
    theMatrix[1][0] = (double)c();
    theMatrix[1][1] = (double)d();
    theMatrix[1][2] = 0.0f;
    theMatrix[2][0] = (double)tx();
    theMatrix[2][1] = (double)ty();
    theMatrix[2][2] = 1.0f;

    Mx3Inverse(&theMatrix[0][0],&theMatrixInverted[0][0]);

    inverse[A] = (POINT_TYPE)theMatrixInverted[0][0];
    inverse[B] = (POINT_TYPE)theMatrixInverted[0][1];
    inverse[C] = (POINT_TYPE)theMatrixInverted[1][0];
    inverse[D] = (POINT_TYPE)theMatrixInverted[1][1];
    inverse[TX] = (POINT_TYPE)theMatrixInverted[2][0];
    inverse[TY] = (POINT_TYPE)theMatrixInverted[2][1];

    invalidated = false;

    return;
    }