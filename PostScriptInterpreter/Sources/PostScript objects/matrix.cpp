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

#include "job.h"

#include "PostScript objects/graphicsState.h"
#include "PostScript objects/matrix.h"
#include <stdio.h>

int Mx3Inverse(double *sourceMatrix,double *targetMatrix);


    matrix::matrix(job *pJob) : array(pJob,6) {
    theObjectType = object::objectType::objTypeMatrix;
    identity();
    return;
    }


    void matrix::SetValue(long index,FLOAT value) {
    entries[index] = new (pJob -> CurrentObjectHeap()) object(pJob,value);
    switch ( index ) {
    case 0:
        xForm.eM11 = value;
        break;
    case 1:
        xForm.eM21 = value;
        break;
    case 2:
        xForm.eM21 = value;
        break;
    case 3:
        xForm.eM22 = value;
        break;
    case 4:
        xForm.eDx = value;
        break;
    case 5:
        xForm.eDy = value;
        break;
    }
    invalidated = true;
    return;
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


    FLOAT *matrix::Values() {
    xForm.eM11 = a();
    xForm.eM12 = b();
    xForm.eM21 = c();
    xForm.eM22 = d();
    xForm.eDx = tx();
    xForm.eDy = ty();
    return (FLOAT *)&xForm;
    }


    XFORM *matrix::XForm() {
    xForm.eM11 = a();
    xForm.eM12 = b();
    xForm.eM21 = c();
    xForm.eM22 = d();
    xForm.eDx = tx();
    xForm.eDy = ty();
    return &xForm;
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


    FLOAT matrix::a() { return getElement(A) -> FloatValue(); }
    FLOAT matrix::b() { return getElement(B) -> FloatValue(); }
    FLOAT matrix::c() { return getElement(C) -> FloatValue(); }
    FLOAT matrix::d() { return getElement(D) -> FloatValue(); }
    FLOAT matrix::tx() { return getElement(TX) -> FloatValue(); }
    FLOAT matrix::ty() { return getElement(TY) -> FloatValue(); }

    FLOAT matrix::aInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[0]; }
    FLOAT matrix::bInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[1]; }
    FLOAT matrix::cInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[2]; }
    FLOAT matrix::dInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[3]; }
    FLOAT matrix::txInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[4]; }
    FLOAT matrix::tyInverse(boolean force) { if ( force || invalidated ) invert(); return inverse[5]; }

    void matrix::a(FLOAT v) { invalidated = true; getElement(A) -> FloatValue(v); xForm.eM11 = v; }
    void matrix::b(FLOAT v) { invalidated = true; getElement(B) -> FloatValue(v); xForm.eM12 = v; }
    void matrix::c(FLOAT v) { invalidated = true; getElement(C) -> FloatValue(v); xForm.eM21 = v; }
    void matrix::d(FLOAT v) { invalidated = true; getElement(D) -> FloatValue(v); xForm.eM22 = v; }
    void matrix::tx(FLOAT v) { invalidated = true; getElement(TX) -> FloatValue(v); xForm.eDx = v; }
    void matrix::ty(FLOAT v) { invalidated = true; getElement(TY) -> FloatValue(v); xForm.eDy = v; }


    void matrix::concat(matrix *pSource) {
    concat((FLOAT *)pSource -> XForm());
    return;
    }


    void matrix::concat(XFORM *pXForm) {
    concat((FLOAT *)pXForm);
    return;
    }


    void matrix::concat(array *pSource) {
    FLOAT arrayValues[] = {
            pSource -> getElement(A) -> OBJECT_POINT_TYPE_VALUE,pSource -> getElement(B) -> OBJECT_POINT_TYPE_VALUE,
            pSource -> getElement(C) -> OBJECT_POINT_TYPE_VALUE,pSource -> getElement(D) -> OBJECT_POINT_TYPE_VALUE,
            pSource -> getElement(TX) -> OBJECT_POINT_TYPE_VALUE,pSource -> getElement(TY) -> OBJECT_POINT_TYPE_VALUE};
    matrix::concat(arrayValues);
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
    FLOAT sfValues[]{scaleFactor,0.0,0.0,scaleFactor,0.0,0.0};
    concat(sfValues);
    return;
    }


    void matrix::scale(FLOAT scaleX,FLOAT scaleY) {
    FLOAT sfValues[]{scaleX,0.0,0.0,scaleY,0.0,0.0};
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


    void matrix::translate(FLOAT x,FLOAT y) {
    FLOAT transValues[]{1.0,0.0,0.0,1.0,x,y};
    concat(transValues);
    return;
    }


    void matrix::rotate(FLOAT angle) {
    FLOAT cosAngle = cos(-angle * graphicsState::degToRad);
    FLOAT sinAngle = sin(-angle * graphicsState::degToRad);
    FLOAT rotValues[] = {cosAngle,sinAngle,-sinAngle,cosAngle,0.0,0.0};
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

    inverse[A] = (FLOAT)theMatrixInverted[0][0];
    inverse[B] = (FLOAT)theMatrixInverted[0][1];
    inverse[C] = (FLOAT)theMatrixInverted[1][0];
    inverse[D] = (FLOAT)theMatrixInverted[1][1];

    inverse[TX] = (FLOAT)theMatrixInverted[0][2];
    inverse[TY] = (FLOAT)theMatrixInverted[1][2];

    invalidated = false;

    return;
    }


    void matrix::transformPoint(XFORM *pXForm,POINTF *ptIn,POINTF *ptOut) {
    FLOAT xResult = pXForm -> eM11 * ptIn -> x + pXForm -> eM12 * ptIn -> y + pXForm -> eDx;
    FLOAT yResult = pXForm -> eM21 * ptIn -> x + pXForm -> eM22 * ptIn -> y + pXForm -> eDy;
    if ( NULL == ptOut )
        ptOut = ptIn;
    ptOut -> x = xResult;
    ptOut -> y = yResult;
    return;
    }


    void matrix::transformPointInverse(XFORM *pXFormIn,POINTF *ptIn,POINTF *ptOut) {
    XFORM *pXForm = invert(pXFormIn);
    FLOAT xResult = pXForm -> eM11 * ptIn -> x + pXForm -> eM12 * ptIn -> y + pXForm -> eDx;
    FLOAT yResult = pXForm -> eM21 * ptIn -> x + pXForm -> eM22 * ptIn -> y + pXForm -> eDy;
    if ( NULL == ptOut )
        ptOut = ptIn;
    ptOut -> x = xResult;
    ptOut -> y = yResult;
    return;
    }


    XFORM *matrix::invert(XFORM *pXForm) {

    double theMatrix[3][3];
    double theMatrixInverted[3][3];

    theMatrix[0][0] = (double)pXForm -> eM11;
    theMatrix[0][1] = (double)pXForm -> eM12;
    theMatrix[0][2] = (double)pXForm -> eDx;
    theMatrix[1][0] = (double)pXForm -> eM21;
    theMatrix[1][1] = (double)pXForm -> eM22;
    theMatrix[1][2] = (double)pXForm -> eDy;

    theMatrix[2][0] = 0.0;
    theMatrix[2][1] = 0.0;
    theMatrix[2][2] = 1.0f;

    Mx3Inverse(&theMatrix[0][0],&theMatrixInverted[0][0]);

    static XFORM xForm{(FLOAT)theMatrixInverted[0][0],(FLOAT)theMatrixInverted[0][1],
                        (FLOAT)theMatrixInverted[1][0],(FLOAT)theMatrixInverted[1][1],
                        (FLOAT)theMatrixInverted[0][2],(FLOAT)theMatrixInverted[1][2]};

    return &xForm;
    }