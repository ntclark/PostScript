#include "job.h"

#include "pathParameters.h"
#include "gdiParameters.h"

    void graphicsState::initMatrix(HWND hwndClient,long pageNumber) {

    if ( ! ( NULL == hwndClient ) ) {

        RECT rcClient;
        GetClientRect(hwndClient,&rcClient);

        cxClient = (rcClient.right - rcClient.left);
        cyClient = (rcClient.bottom - rcClient.top);

        cyWindow = (long)((double)cyClient / (double)pageNumber);

        SetWindowExtEx(pPStoPDF -> GetDC(),cxClient,cyWindow,NULL);
        SetViewportExtEx(pPStoPDF -> GetDC(),cxClient,cyWindow,NULL);

        /*
        Note: At this point, pageHeightPoints was not set by interpreting PostScript,
        it is a static value and PS is not being parsed yet. Move this portion
        of transform setup until after PS specifies the page dimensions
        */
        scalePointsToPixels = (POINT_TYPE)cyWindow / (POINT_TYPE)pageHeightPoints;

    }

    pathParametersStack.top() -> pUserSpaceToDeviceSpace -> a(scalePointsToPixels);
    pathParametersStack.top() -> pUserSpaceToDeviceSpace -> d(-scalePointsToPixels);
    pathParametersStack.top() -> pUserSpaceToDeviceSpace -> ty((POINT_TYPE)(cyWindow));

    SetWorldTransform(pPStoPDF -> GetDC(),pathParameters::pTransform());

    return;
    }


    void graphicsState::concat(matrix *pSource) {
    pathParametersStack.top() -> concat(pSource);
    return;
    }


    void graphicsState::concat(array *pSource) {
    pathParametersStack.top() -> concat(pSource);
    return;
    }


    void graphicsState::concat(XFORM &winXForm) {
    pathParametersStack.top() -> concat(winXForm);
    return;
    }


    void graphicsState::concat(POINT_TYPE *pValues) {
    pathParametersStack.top() -> concat(pValues);
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

    POINT_TYPE values[] = {
        pArray -> getElement(A) -> OBJECT_POINT_TYPE_VALUE,
        pArray -> getElement(B) -> OBJECT_POINT_TYPE_VALUE,
        pArray -> getElement(C) -> OBJECT_POINT_TYPE_VALUE,
        pArray -> getElement(D) -> OBJECT_POINT_TYPE_VALUE,
        pArray -> getElement(TX) -> OBJECT_POINT_TYPE_VALUE,
        pArray -> getElement(TY) -> OBJECT_POINT_TYPE_VALUE
    };

    pathParametersStack.top() -> setMatrix(values);
    return;
    }


    void graphicsState::currentMatrix() {
    pathParametersStack.top() -> currentMatrix();
    return;
    }

    void graphicsState::revertMatrix() {
    pathParametersStack.top() -> revertMatrix();
    return;
    }


    void graphicsState::translate(POINT_TYPE x,POINT_TYPE y) {
    pathParametersStack.top() -> translate(x,y);
    return;
    }


    void graphicsState::scale(POINT_TYPE scaleX,POINT_TYPE scaleY) {
    pathParametersStack.top() -> scale(scaleX,scaleY);
    return;
    }


    void graphicsState::rotate(POINT_TYPE angle) {
    pathParametersStack.top() -> rotate(angle);
    return;
    }


    void graphicsState::transformPoint(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    pathParametersStack.top() -> transformPoint(pMatrix,x,y,pX2,pY2);
    return;
    }


    void graphicsState::transformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    pathParametersStack.top() -> transformPoint(x,y,pX2,pY2);
    return;
    }


    void graphicsState::transformPointInPlace(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    pathParametersStack.top() -> transformPointInPlace(pMatrix,x,y,pX2,pY2);
    return;
    }


    void graphicsState::transformPointInPlace(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    pathParametersStack.top() -> transformPointInPlace(x,y,pX2,pY2);
    return;
    }


    void graphicsState::untransformPoint(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    pathParametersStack.top() -> untransformPoint(pMatrix,x,y,pX2,pY2);
    return;
    }


    void graphicsState::untransformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    pathParametersStack.top() -> untransformPoint(x,y,pX2,pY2);
    return;
    }


    void graphicsState::untransformPointInPlace(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    pathParametersStack.top() -> untransformPointInPlace(pMatrix,x,y,pX2,pY2);
    return;
    }


    void graphicsState::untransformPointInPlace(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    pathParametersStack.top() -> untransformPointInPlace(x,y,pX2,pY2);
    return;
    }


    void graphicsState::scalePoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    pathParametersStack.top() -> scalePoint(x,y,pX2,pY2);
    return;
    }


    void graphicsState::transform(GS_POINT *pPoints,uint16_t pointCount) {
    pathParametersStack.top() -> transform(pPoints,pointCount);
    return;
    }


    void graphicsState::transformInPlace(GS_POINT *pPoints,uint16_t pointCount) {
    pathParametersStack.top() -> transformInPlace(pPoints,pointCount);
    return;
    }


    void graphicsState::translate(GS_POINT *pPoints,uint16_t pointCount,GS_POINT *pToPoint) {
    pathParametersStack.top() -> translate(pPoints,pointCount,pToPoint);
    return;
    }


    void graphicsState::scale(GS_POINT *pPoints,uint16_t pointCount,POINT_TYPE scale) {
    pathParametersStack.top() -> scale(pPoints,pointCount,scale);
    return;
    }


