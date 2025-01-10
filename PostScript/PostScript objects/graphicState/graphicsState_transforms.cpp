#include "job.h"

#include "pathParameters.h"
#include "gdiParameters.h"


    void graphicsState::initMatrix(HWND hwndClient,long pageNumber) {
    pathParametersStack.top() -> initMatrix(hwndClient,pageNumber,pageHeightPoints);
//outlinePage();
    return;
    }


    void graphicsState::concat(matrix *pSource) {
    psXformsStack.top() -> concat(pSource);
    return;
    }


    void graphicsState::concat(array *pSource) {
    psXformsStack.top() -> concat(pSource);
    return;
    }


    void graphicsState::concat(XFORM &winXForm) {
    psXformsStack.top() -> concat(winXForm);
    return;
    }


    void graphicsState::concat(XFORM *pXForm) {
    psXformsStack.top() -> concat((FLOAT *)pXForm);
    return;
    }


    void graphicsState::concat(POINT_TYPE *pValues) {
    psXformsStack.top() -> concat(pValues);
    return;
    }


    void graphicsState::setMatrix(object *pMatrix) {

    array *pArray = NULL;
    switch ( pMatrix -> ObjectType() ) {
    case object::objectType::array: 
        pArray = reinterpret_cast<class array *>(pMatrix);
        break;

    case object::objectType::matrix:
        pArray = static_cast<class array *>(reinterpret_cast<matrix *>(pMatrix));
        break;

    default: {
        char szMessage[256];
        sprintf(szMessage,"operator putinterval: the type of object %s is invalid (%s) should be array or matrix",pMatrix -> Name(),pMatrix -> TypeName());
        throw typecheck(szMessage);
        }

    }

    psXformsStack.top() -> a(pArray -> getElement(A) -> OBJECT_POINT_TYPE_VALUE);
    psXformsStack.top() -> b(pArray -> getElement(B) -> OBJECT_POINT_TYPE_VALUE);
    psXformsStack.top() -> c(pArray -> getElement(C) -> OBJECT_POINT_TYPE_VALUE);
    psXformsStack.top() -> d(pArray -> getElement(D) -> OBJECT_POINT_TYPE_VALUE);
    psXformsStack.top() -> tx(pArray -> getElement(TX) -> OBJECT_POINT_TYPE_VALUE);
    psXformsStack.top() -> ty(pArray -> getElement(TY) -> OBJECT_POINT_TYPE_VALUE);

    return;
    }


    void graphicsState::currentMatrix() {
    matrix *pMatrix = reinterpret_cast<matrix *>(pPStoPDF -> currentJob() -> pop());
    pMatrix -> a(psXformsStack.top() -> a());
    pMatrix -> b(psXformsStack.top() -> b());
    pMatrix -> c(psXformsStack.top() -> c());
    pMatrix -> d(psXformsStack.top() -> d());
    pMatrix -> tx(psXformsStack.top() -> tx());
    pMatrix -> ty(psXformsStack.top() -> ty());
    pPStoPDF -> currentJob() -> push(pMatrix);
    return;
    }

    void graphicsState::revertMatrix() {
    psXformsStack.top() -> revertMatrix();
    return;
    }


    void graphicsState::setTranslation(POINT_TYPE x,POINT_TYPE y) {
    psXformsStack.top() -> tx(x);
    psXformsStack.top() -> ty(y);
    return;
    }


    void graphicsState::translate(POINT_TYPE x,POINT_TYPE y) {
    psXformsStack.top() -> translate(x,y);
    return;
    }


    void graphicsState::scale(POINT_TYPE scaleX,POINT_TYPE scaleY) {
    psXformsStack.top() -> scale(scaleX,scaleY);
    return;
    }


    void graphicsState::rotate(POINT_TYPE angle) {
    psXformsStack.top() -> rotate(angle);
    return;
    }


    void graphicsState::transformPoint(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult = pMatrix -> a() * x + pMatrix -> b() * y + pMatrix -> tx();
    POINT_TYPE yResult = pMatrix -> c() * x + pMatrix -> d() * y + pMatrix -> ty();
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void graphicsState::transformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    transformPoint(psXformsStack.top(),x,y,pX2,pY2);
    return;
    }


    void graphicsState::transformPointInPlace(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult = pMatrix -> a() * x + pMatrix -> b() * y;
    POINT_TYPE yResult = pMatrix -> c() * x + pMatrix -> d() * y;
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void graphicsState::transformPointInPlace(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    transformPointInPlace(psXformsStack.top(),x,y,pX2,pY2);
    return;
    }


    void graphicsState::untransformPoint(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult = pMatrix -> aInverse() * x + pMatrix -> bInverse() * y + pMatrix -> txInverse();
    POINT_TYPE yResult = pMatrix -> cInverse() * x + pMatrix -> dInverse() * y + pMatrix -> tyInverse();
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void graphicsState::untransformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    untransformPoint(psXformsStack.top(),x,y,pX2,pY2);
    return;
    }


    void graphicsState::untransformPointInPlace(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult = pMatrix -> aInverse() * x + pMatrix -> bInverse() * y;
    POINT_TYPE yResult = pMatrix -> cInverse() * x + pMatrix -> dInverse() * y;
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void graphicsState::untransformPointInPlace(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    untransformPointInPlace(psXformsStack.top(),x,y,pX2,pY2);
    return;
    }


    void graphicsState::scalePoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    *pX2 = psXformsStack.top() -> a() * x;
    *pY2 = psXformsStack.top() -> d() * y;
    return;
    }