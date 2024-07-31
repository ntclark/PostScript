
#include "job.h"
#include "PostScript objects/graphicsState.h"

    void graphicsState::initMatrix(HWND hwndClient,long pageNumber) {

    RECT rcClient;
    GetClientRect(hwndClient,&rcClient);

    cxClient = (rcClient.right - rcClient.left);
    cyClient = (rcClient.bottom - rcClient.top);

    long cyWindow = (long)((double)cyClient / (double)pageNumber);

    SetWindowExtEx(pPStoPDF -> GetDC(),PICA_FACTOR * cxClient,PICA_FACTOR * cyWindow,NULL);
    SetViewportExtEx(pPStoPDF -> GetDC(),cxClient,cyWindow,NULL);

    /*
    Note: At this point, pageHeightPoints was not set by interpreting PostScript,
    it is a static value and PS is not being parsed yet. Move this portion
    of transform setup until after PS specifies the page dimensions
    */
    scalePointsToPixels = (POINT_TYPE)(cyClient) / (POINT_TYPE)(pageNumber * pageHeightPoints);

    pUserSpaceToDeviceSpace -> a(scalePointsToPixels);
    pUserSpaceToDeviceSpace -> d(-scalePointsToPixels);
    pUserSpaceToDeviceSpace -> ty((POINT_TYPE)(PICA_FACTOR * cyWindow));

    SetWorldTransform(pPStoPDF -> GetDC(),pTransform());

    return;
    }


    XFORM *graphicsState::pTransform() {
    static XFORM theXForm;
    theXForm.eM11 = (float)pUserSpaceToDeviceSpace -> a();
    theXForm.eM12 = (float)pUserSpaceToDeviceSpace -> b();
    theXForm.eM21 = (float)pUserSpaceToDeviceSpace -> c();
    theXForm.eM22 = (float)pUserSpaceToDeviceSpace -> d();
    theXForm.eDx = (float)pUserSpaceToDeviceSpace -> tx();
    theXForm.eDy = (float)pUserSpaceToDeviceSpace -> ty();
    return &theXForm;
    }


    matrix *graphicsState::ToUserSpace() {
    return pPStoPDF -> currentJob() -> currentGS() -> pToUserSpace;
    }


    void graphicsState::concat(matrix *pSource) {
    ToUserSpace() -> concat(pSource);
    return;
    }


    void graphicsState::concat(array *pSource) {
    ToUserSpace() -> concat(pSource);
    return;
    }


    void graphicsState::concat(XFORM &winXForm) {
    ToUserSpace() -> concat(winXForm);
    return;
    }


    void graphicsState::concat(POINT_TYPE *pValues) {
    ToUserSpace() -> concat(pValues);
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

    ToUserSpace() -> a(pArray -> getElement(A) -> OBJECT_POINT_TYPE_VALUE);
    ToUserSpace() -> b(pArray -> getElement(B) -> OBJECT_POINT_TYPE_VALUE);
    ToUserSpace() -> c(pArray -> getElement(C) -> OBJECT_POINT_TYPE_VALUE);
    ToUserSpace() -> d(pArray -> getElement(D) -> OBJECT_POINT_TYPE_VALUE);
    ToUserSpace() -> tx(pArray -> getElement(TX) -> OBJECT_POINT_TYPE_VALUE);
    ToUserSpace() -> ty(pArray -> getElement(TY) -> OBJECT_POINT_TYPE_VALUE);

    return;
    }


    void graphicsState::currentMatrix() {
/*
    currentmatrix 
        matrix currentmatrix matrix

    replaces the value of matrix with the current transformation matrix (CTM) in the
    graphics state and pushes this modified matrix back on the operand stack (see
    Section 4.3.2, “Transformations”).
*/
    matrix *pMatrix = reinterpret_cast<matrix *>(pJob -> top());

    pMatrix -> a(ToUserSpace() -> a());
    pMatrix -> b(ToUserSpace() -> b());
    pMatrix -> c(ToUserSpace() -> c());
    pMatrix -> d(ToUserSpace() -> d());
    pMatrix -> tx(ToUserSpace() -> tx());
    pMatrix -> ty(ToUserSpace() -> ty());

    return;
    }

    void graphicsState::revertMatrix() {
    ToUserSpace() -> revertTransform();
    return;
    }


    void graphicsState::translate(POINT_TYPE x,POINT_TYPE y) {
    POINT_TYPE values[] = {1.0,0.0,0.0,1.0,x,y};
    concat(values);
    return;
    }


    void graphicsState::scale(POINT_TYPE scaleX,POINT_TYPE scaleY) {
    POINT_TYPE values[] = {scaleX,0.0,0.0,scaleY,0.0,0.0};
    concat(values);
    return;
    }


    void graphicsState::rotate(POINT_TYPE angle) {
    POINT_TYPE cosAngle = cos(-angle * graphicsState::degToRad);
    POINT_TYPE sinAngle = sin(-angle * graphicsState::degToRad);
    POINT_TYPE values[] = {cosAngle,sinAngle,-sinAngle,cosAngle,0.0,0.0};
    concat(values);
    totalRotation -= angle;
    return;
    }


    void graphicsState::transformPoint(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    *pX2 = pMatrix -> a() * x + pMatrix -> b() * y + pMatrix -> tx();
    *pY2 = pMatrix -> c() * x + pMatrix -> d() * y + pMatrix -> ty();
    return;
    }


    void graphicsState::transformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult = ToUserSpace() -> a() * x + ToUserSpace() -> b() * y + ToUserSpace() -> tx();
    POINT_TYPE yResult = ToUserSpace() -> c() * x + ToUserSpace() -> d() * y + ToUserSpace() -> ty();
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void graphicsState::transformPointInPlace(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    *pX2 = pMatrix -> a() * x + pMatrix -> b() * y;
    *pY2 = pMatrix -> c() * x + pMatrix -> d() * y;
    return;
    }


    void graphicsState::transformPointInPlace(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult = ToUserSpace() -> a() * x + ToUserSpace() -> b() * y;
    POINT_TYPE yResult = ToUserSpace() -> c() * x + ToUserSpace() -> d() * y;
    *pX2 = xResult;
    *pY2 = yResult;
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
    POINT_TYPE xResult = ToUserSpace() -> aInverse() * x + ToUserSpace() -> bInverse() * y + ToUserSpace() -> txInverse();
    POINT_TYPE yResult = ToUserSpace() -> cInverse() * x + ToUserSpace() -> dInverse() * y + ToUserSpace() -> tyInverse();
    *pX2 = xResult;
    *pY2 = yResult;
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
    POINT_TYPE xResult = ToUserSpace() -> aInverse() * x + ToUserSpace() -> bInverse() * y;
    POINT_TYPE yResult = ToUserSpace() -> cInverse() * x + ToUserSpace() -> dInverse() * y;
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void graphicsState::scalePoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    *pX2 = x * ToUserSpace() -> a();
    *pY2 = y * ToUserSpace() -> d();
    return;
    }


    void graphicsState::transform(GS_POINT *pPoints,uint16_t pointCount) {
    for ( long k = 0; k < pointCount; k++ ) {
        POINT_TYPE x = pPoints[k].x;
        POINT_TYPE y = pPoints[k].y;
        pPoints[k].x = ToUserSpace() -> a() * x + ToUserSpace() -> b() * y + ToUserSpace() -> tx();
        pPoints[k].y = ToUserSpace() -> c() * x + ToUserSpace() -> d() * y + ToUserSpace() -> ty();
    }
    return;
    }


    void graphicsState::transformInPlace(GS_POINT *pPoints,uint16_t pointCount) {
    for ( long k = 0; k < pointCount; k++ ) {
        POINT_TYPE x = pPoints[k].x;
        POINT_TYPE y = pPoints[k].y;
        pPoints[k].x = ToUserSpace() -> a() * x + ToUserSpace() -> b() * y;
        pPoints[k].y = ToUserSpace() -> c() * x + ToUserSpace() -> d() * y;
    }
    return;
    }


    void graphicsState::translate(GS_POINT *pPoints,uint16_t pointCount,GS_POINT *pToPoint) {
    for ( long k = 0; k < pointCount; k++ ) {
        pPoints[k].x += pToPoint -> x;
        pPoints[k].y += pToPoint -> y;
    }
    return;
    }


    void graphicsState::scale(GS_POINT *pPoints,uint16_t pointCount,POINT_TYPE scale) {
    for ( long k = 0; k < pointCount; k++ ) {
        pPoints[k].x = pPoints[k].x * scale;
        pPoints[k].y = pPoints[k].y * scale;
    }
    return;
    }


