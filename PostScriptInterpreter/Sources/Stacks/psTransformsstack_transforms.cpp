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

    void psTransformsStack::setMatrix(object *pSource) {

    switch ( pSource -> ObjectType() ) {
    case object::objectType::objTypeArray: {
        array *pArray = reinterpret_cast<class array *>(pSource);
        back() -> a(pArray -> getElement(A) -> FloatValue());
        back() -> b(pArray -> getElement(B) -> FloatValue());
        back() -> c(pArray -> getElement(C) -> FloatValue());
        back() -> d(pArray -> getElement(D) -> FloatValue());
        back() -> tx(pArray -> getElement(TX) -> FloatValue());
        back() -> ty(pArray -> getElement(TY) -> FloatValue());
        }
        break;

    case object::objectType::objTypeMatrix: {
        matrix *pMatrix = reinterpret_cast<matrix *>(pSource);
        back() -> a(pMatrix -> a());
        back() -> b(pMatrix -> b());
        back() -> c(pMatrix -> c());
        back() -> d(pMatrix -> d());
        back() -> tx(pMatrix -> tx());
        back() -> ty(pMatrix -> ty());
        }
        break;

    default: {
        char szMessage[256];
        sprintf(szMessage,"operator putinterval: the type of object %s is invalid (%s) should be array or matrix",pSource -> Name(),pSource -> TypeName());
        throw typecheck(szMessage);
        }

    }

    PostScriptInterpreter::pIRenderer -> put_ToPageTransform((UINT_PTR)back() -> XForm());

    return;
    }


    void psTransformsStack::defaultMatrix() {
    push_back(new (pJob -> CurrentObjectHeap()) matrix(pJob));
    PostScriptInterpreter::pIRenderer -> put_ToPageTransform((UINT_PTR)back() -> XForm());
    return;
    }


    matrix *psTransformsStack::CurrentMatrix() {
    return back();
    }


    void psTransformsStack::revertMatrix() {
    back() -> revertMatrix();
    PostScriptInterpreter::pIRenderer -> put_ToPageTransform((UINT_PTR)back() -> XForm());
    return;
    }


    void psTransformsStack::concat(matrix *pSource) {
    back() -> concat(pSource);
    PostScriptInterpreter::pIRenderer -> put_ToPageTransform((UINT_PTR)back() -> XForm());
    return;
    }


    void psTransformsStack::concat(array *pSource) {
    back() -> concat(pSource);
    PostScriptInterpreter::pIRenderer -> put_ToPageTransform((UINT_PTR)back() -> XForm());
    return;
    }


    void psTransformsStack::concat(XFORM *pXForm) {
    back() -> concat((FLOAT *)pXForm);
    PostScriptInterpreter::pIRenderer -> put_ToPageTransform((UINT_PTR)back() -> XForm());
    return;
    }


    void psTransformsStack::concat(FLOAT *pValues) {
    back() -> concat(pValues);
    PostScriptInterpreter::pIRenderer -> put_ToPageTransform((UINT_PTR)back() -> XForm());
    return;
    }


    void psTransformsStack::setTranslation(FLOAT x,FLOAT y) {
    back() -> tx(x);
    back() -> ty(y);
    PostScriptInterpreter::pIRenderer -> put_ToPageTransform((UINT_PTR)back() -> XForm());
    return;
    }


    void psTransformsStack::translate(FLOAT x,FLOAT y) {
    back() -> translate(x,y);
    PostScriptInterpreter::pIRenderer -> put_ToPageTransform((UINT_PTR)back() -> XForm());
    return;
    }


    void psTransformsStack::scale(FLOAT scaleX,FLOAT scaleY) {
    back() -> scale(scaleX,scaleY);
    PostScriptInterpreter::pIRenderer -> put_ToPageTransform((UINT_PTR)back() -> XForm());
    return;
    }


    void psTransformsStack::rotate(FLOAT angle) {
    back() -> rotate(angle);
    PostScriptInterpreter::pIRenderer -> put_ToPageTransform((UINT_PTR)back() -> XForm());
    return;
    }


    void psTransformsStack::transformPoint(matrix *pMatrix,FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    FLOAT xResult = pMatrix -> a() * x + pMatrix -> b() * y + pMatrix -> tx();
    FLOAT yResult = pMatrix -> c() * x + pMatrix -> d() * y + pMatrix -> ty();
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void psTransformsStack::transformPoint(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    transformPoint(back(),x,y,pX2,pY2);
    return;
    }


    void psTransformsStack::transformPoint(GS_POINT *ptIn,GS_POINT *ptOut) {
    transformPoint(back(),ptIn -> x,ptIn -> y,&ptOut -> x,&ptOut -> y);
    return;
    }


    void psTransformsStack::transformPointInPlace(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    transformPointInPlace(back(),x,y,pX2,pY2);
    return;
    }


    void psTransformsStack::transformPointInPlace(class matrix *pMatrix,FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    FLOAT xResult = pMatrix -> a() * x + pMatrix -> b() * y;
    FLOAT yResult = pMatrix -> c() * x + pMatrix -> d() * y;
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void psTransformsStack::untransformPoint(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    untransformPoint(back(),x,y,pX2,pY2);
    return;
    }


    void psTransformsStack::untransformPoint(class matrix *pMatrix,FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    FLOAT xResult = pMatrix -> aInverse() * x + pMatrix -> bInverse() * y + pMatrix -> txInverse();
    FLOAT yResult = pMatrix -> cInverse() * x + pMatrix -> dInverse() * y + pMatrix -> tyInverse();
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void psTransformsStack::untransformPoint(POINTF *ptIn,POINTF *ptOut) {
    FLOAT xResult;
    FLOAT yResult;
    untransformPoint(ptIn -> x,ptIn -> y,&xResult,&yResult);
    ptOut -> x = xResult;
    ptOut -> y = yResult;
    return;
    }


    void psTransformsStack::untransformPointInPlace(class matrix *pMatrix,FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    FLOAT xResult = pMatrix -> aInverse() * x + pMatrix -> bInverse() * y;
    FLOAT yResult = pMatrix -> cInverse() * x + pMatrix -> dInverse() * y;
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void psTransformsStack::untransformPointInPlace(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    untransformPointInPlace(back(),x,y,pX2,pY2);
    return;
    }


    void psTransformsStack::scalePoint(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    *pX2 = back() -> a() * x;
    *pY2 = back() -> d() * y;
    return;
    }