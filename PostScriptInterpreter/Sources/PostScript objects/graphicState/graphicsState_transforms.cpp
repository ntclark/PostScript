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


#include "pathParameters.h"
#include "gdiParameters.h"


    void graphicsState::initMatrix(HWND hwndSurface,long pageNumber) {
    thePathParameters.initMatrix(hwndSurface,pageNumber,pageHeightPoints,pageWidthPoints);
    return;
    }


    void graphicsState::concat(matrix *pSource) {
    pPSXformsStack -> back() -> concat(pSource);
    return;
    }


    void graphicsState::concat(array *pSource) {
    pPSXformsStack -> back() -> concat(pSource);
    return;
    }


    void graphicsState::concat(XFORM *pXForm) {
    pPSXformsStack -> back() -> concat((FLOAT *)pXForm);
    return;
    }


    void graphicsState::concat(FLOAT *pValues) {
    pPSXformsStack -> back() -> concat(pValues);
    return;
    }


    void graphicsState::setMatrix(object *pSource) {

    switch ( pSource -> ObjectType() ) {
    case object::objectType::objTypeArray: {
        array *pArray = reinterpret_cast<class array *>(pSource);
        pPSXformsStack -> back() -> a(pArray -> getElement(A) -> OBJECT_POINT_TYPE_VALUE);
        pPSXformsStack -> back() -> b(pArray -> getElement(B) -> OBJECT_POINT_TYPE_VALUE);
        pPSXformsStack -> back() -> c(pArray -> getElement(C) -> OBJECT_POINT_TYPE_VALUE);
        pPSXformsStack -> back() -> d(pArray -> getElement(D) -> OBJECT_POINT_TYPE_VALUE);
        pPSXformsStack -> back() -> tx(pArray -> getElement(TX) -> OBJECT_POINT_TYPE_VALUE);
        pPSXformsStack -> back() -> ty(pArray -> getElement(TY) -> OBJECT_POINT_TYPE_VALUE);
        }
        break;

    case object::objectType::objTypeMatrix: {
        matrix *pMatrix = reinterpret_cast<matrix *>(pSource);
        pPSXformsStack -> back() -> a(pMatrix -> a());
        pPSXformsStack -> back() -> b(pMatrix -> b());
        pPSXformsStack -> back() -> c(pMatrix -> c());
        pPSXformsStack -> back() -> d(pMatrix -> d());
        pPSXformsStack -> back() -> tx(pMatrix -> tx());
        pPSXformsStack -> back() -> ty(pMatrix -> ty());
        }
        break;

    default: {
        char szMessage[256];
        sprintf(szMessage,"operator putinterval: the type of object %s is invalid (%s) should be array or matrix",pSource -> Name(),pSource -> TypeName());
        throw typecheck(szMessage);
        }

    }

    return;
    }


    void graphicsState::currentMatrix() {
    matrix *pMatrix = reinterpret_cast<matrix *>(pJob -> pop());
    pMatrix -> a(pPSXformsStack -> back() -> a());
    pMatrix -> b(pPSXformsStack -> back() -> b());
    pMatrix -> c(pPSXformsStack -> back() -> c());
    pMatrix -> d(pPSXformsStack -> back() -> d());
    pMatrix -> tx(pPSXformsStack -> back() -> tx());
    pMatrix -> ty(pPSXformsStack -> back() -> ty());
    pPostScriptInterpreter -> currentJob() -> push(pMatrix);
    return;
    }

    void graphicsState::revertMatrix() {
    pPSXformsStack -> back() -> revertMatrix();
    return;
    }


    void graphicsState::defaultMatrix() {
    pPSXformsStack -> push_back(new (pJob -> CurrentObjectHeap()) matrix(pJob));
    return;
    }


    void graphicsState::setTranslation(FLOAT x,FLOAT y) {
    pPSXformsStack -> back() -> tx(x);
    pPSXformsStack -> back() -> ty(y);
    return;
    }


    void graphicsState::translate(FLOAT x,FLOAT y) {
    pPSXformsStack -> back() -> translate(x,y);
    return;
    }


    void graphicsState::scale(FLOAT scaleX,FLOAT scaleY) {
    pPSXformsStack -> back() -> scale(scaleX,scaleY);
    return;
    }


    void graphicsState::rotate(FLOAT angle) {
    pPSXformsStack -> back() -> rotate(angle);
    return;
    }


    void graphicsState::transformPoint(matrix *pMatrix,FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    FLOAT xResult = pMatrix -> a() * x + pMatrix -> b() * y + pMatrix -> tx();
    FLOAT yResult = pMatrix -> c() * x + pMatrix -> d() * y + pMatrix -> ty();
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void graphicsState::transformPoint(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    transformPoint(pPSXformsStack -> back(),x,y,pX2,pY2);
    return;
    }


    void graphicsState::transformPoint(GS_POINT *ptIn,GS_POINT *ptOut) {
    transformPoint(pPSXformsStack -> back(),ptIn -> x,ptIn -> y,&ptOut -> x,&ptOut -> y);
    return;
    }


    void graphicsState::transformPointInPlace(class matrix *pMatrix,FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    FLOAT xResult = pMatrix -> a() * x + pMatrix -> b() * y;
    FLOAT yResult = pMatrix -> c() * x + pMatrix -> d() * y;
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void graphicsState::transformPointInPlace(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    transformPointInPlace(pPSXformsStack -> back(),x,y,pX2,pY2);
    return;
    }


    void graphicsState::untransformPoint(class matrix *pMatrix,FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    FLOAT xResult = pMatrix -> aInverse() * x + pMatrix -> bInverse() * y + pMatrix -> txInverse();
    FLOAT yResult = pMatrix -> cInverse() * x + pMatrix -> dInverse() * y + pMatrix -> tyInverse();
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void graphicsState::untransformPoint(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    untransformPoint(pPSXformsStack -> back(),x,y,pX2,pY2);
    return;
    }


    void graphicsState::untransformPoint(POINTF *ptIn,POINTF *ptOut) {
    FLOAT xResult;
    FLOAT yResult;
    untransformPoint(ptIn -> x,ptIn -> y,&xResult,&yResult);
    ptOut -> x = xResult;
    ptOut -> y = yResult;
    return;
    }


    void graphicsState::untransformPointInPlace(class matrix *pMatrix,FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    FLOAT xResult = pMatrix -> aInverse() * x + pMatrix -> bInverse() * y;
    FLOAT yResult = pMatrix -> cInverse() * x + pMatrix -> dInverse() * y;
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void graphicsState::untransformPointInPlace(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    untransformPointInPlace(pPSXformsStack -> back(),x,y,pX2,pY2);
    return;
    }


    void graphicsState::scalePoint(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    *pX2 = pPSXformsStack -> back() -> a() * x;
    *pY2 = pPSXformsStack -> back() -> d() * y;
    return;
    }