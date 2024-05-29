
#include "PostScript objects/graphicsState.h"
#include "PostScript objects/font.h"

#include <math.h>
#include "utilities.h"

    long graphicsState::displayResolution = 0L;
    long graphicsState::cxClient = 0L;
    long graphicsState::cyClient = 0L;
    HWND graphicsState::hwndSurface = NULL;
    HDC graphicsState::hdcSurface = NULL;
    HDC graphicsState::hdcAlternate = NULL;
    HBITMAP graphicsState::hbmGlyph = NULL;

    matrix *graphicsState::pFromUserSpaceToDeviceSpace = NULL;

    GS_POINT::GS_POINT(POINTD *pPointd) {
        x = pPointd -> X();
        y = pPointd -> Y();
    }


    graphicsState::graphicsState(job *pj,HWND hwndSur) :
        pJob(pj)
    {

    pToUserSpace = new (pJob -> CurrentObjectHeap()) matrix(pJob);

    memset(&lastUserSpacePoint,0,sizeof(GS_POINT));
    memset(&lastUserSpaceMovedToPoint,0,sizeof(GS_POINT));
    memset(&lastPointsPoint,0,sizeof(GS_POINT));

    if ( ! ( NULL == hwndSur ) ) {
        pFromUserSpaceToDeviceSpace = new (pJob -> CurrentObjectHeap()) matrix(pJob);
        hwndSurface = hwndSur;
        hdcSurface = GetDC(hwndSurface);
        SetGraphicsMode(hdcSurface,GM_ADVANCED);
        SetMapMode(hdcSurface,MM_ANISOTROPIC);
        SetWorldTransform(hdcSurface,pTransform());
        SetPolyFillMode(hdcSurface,ALTERNATE);
        HPEN hPen = CreatePen(PS_SOLID,0,RGB(0,0,0));
        SelectObject(hdcSurface,hPen);
        HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
        SelectObject(hdcSurface,hBrush);
        displayResolution = GetDeviceCaps(hdcSurface,LOGPIXELSX);
    }

    return;
    }


    // I don't think I can use an actual copy constructor with positional new
    // Probably for the same or similar reason as mentioned in this object
    // constructor.

    void graphicsState::copyFrom(graphicsState *pOther) {

    pFromUserSpaceToDeviceSpace -> copyFrom(pOther -> pFromUserSpaceToDeviceSpace);

    pToUserSpace -> copyFrom(pOther -> pToUserSpace);

    lastUserSpacePoint = pOther -> lastUserSpacePoint;
    lastUserSpaceMovedToPoint = pOther -> lastUserSpaceMovedToPoint;
    lastPointsPoint = pOther -> lastPointsPoint;

    lineCap = pOther -> lineCap;
    lineJoin = pOther -> lineJoin;

    pageHeightPoints = pOther -> pageHeightPoints;
    pageWidthPoints = pOther -> pageWidthPoints;

    scalePointsToPixels = pOther -> scalePointsToPixels;

    return;
    }


    graphicsState::~graphicsState() {
    return;
    }


    XFORM *graphicsState::pTransform() {
    static XFORM theXForm;
    theXForm.eM11 = (float)pFromUserSpaceToDeviceSpace -> a();
    theXForm.eM12 = (float)pFromUserSpaceToDeviceSpace -> b();
    theXForm.eM21 = (float)pFromUserSpaceToDeviceSpace -> c();
    theXForm.eM22 = (float)pFromUserSpaceToDeviceSpace -> d();
    theXForm.eDx = (float)pFromUserSpaceToDeviceSpace -> tx();
    theXForm.eDy = (float)pFromUserSpaceToDeviceSpace -> ty();
    return &theXForm;
    }


    void graphicsState::concat(matrix *pSource) {
    pToUserSpace -> concat(pSource);
    return;
    }


    void graphicsState::concat(array *pSource) {
    pToUserSpace -> concat(pSource);
    return;
    }


    void graphicsState::concat(XFORM &winXForm) {
    pToUserSpace -> concat(winXForm);
    return;
    }


    void graphicsState::concat(POINT_TYPE *pValues) {
    pToUserSpace -> concat(pValues);
    return;
    }


    void graphicsState::restored() {
    SetWorldTransform(hdcSurface,pTransform());
    return;
    }


    void graphicsState::setPageDevice(class dictionary *pDict) {

    if ( NULL == hdcSurface )
        return;

    class array *pPageSizeArray = NULL;

    pJob -> push(pDict);

    pJob -> operatorBegin();

    pJob -> push(pJob -> pPageSizeLiteral);

    pJob -> operatorWhere();

    pJob -> operatorEnd();

    if ( pJob -> pFalseConstant == pJob -> pop() ) 
        return;

    pJob -> push(pJob -> pPageSizeLiteral);

    pJob -> operatorGet();

    pPageSizeArray = reinterpret_cast<array *>(pJob -> pop());

    pageWidthPoints = pPageSizeArray -> getElement(0) -> IntValue();
    pageHeightPoints = pPageSizeArray -> getElement(1) -> IntValue();

    initMatrix();

    return;
    }


    void graphicsState::initMatrix() {

    RECT rcClient;
    GetClientRect(pPStoPDF -> HwndClient(),&rcClient);

    cxClient = (rcClient.right - rcClient.left);
    cyClient = (rcClient.bottom - rcClient.top);

    SIZEL oldExtent;

    SetWindowExtEx(hdcSurface,PICA_FACTOR * cxClient,PICA_FACTOR * cyClient,&oldExtent);
    SetViewportExtEx(hdcSurface,cxClient,cyClient,&oldExtent);

    scalePointsToPixels = (POINT_TYPE)(cyClient) / (POINT_TYPE)pageHeightPoints;

    pFromUserSpaceToDeviceSpace -> a(scalePointsToPixels);
    pFromUserSpaceToDeviceSpace -> d(-scalePointsToPixels);
    pFromUserSpaceToDeviceSpace -> ty((POINT_TYPE)(PICA_FACTOR * cyClient));

    SetWorldTransform(hdcSurface,pTransform());

//MoveToEx(hdcSurface,72,72,NULL);
//LineTo(hdcSurface,pageWidthPoints / 2,pageHeightPoints);
//LineTo(hdcSurface,pageWidthPoints - 72,72);

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

    pToUserSpace -> a(pArray -> getElement(A) -> OBJECT_POINT_TYPE_VALUE);
    pToUserSpace -> b(pArray -> getElement(B) -> OBJECT_POINT_TYPE_VALUE);
    pToUserSpace -> c(pArray -> getElement(C) -> OBJECT_POINT_TYPE_VALUE);
    pToUserSpace -> d(pArray -> getElement(D) -> OBJECT_POINT_TYPE_VALUE);
    pToUserSpace -> tx(pArray -> getElement(TX) -> OBJECT_POINT_TYPE_VALUE);
    pToUserSpace -> ty(pArray -> getElement(TY) -> OBJECT_POINT_TYPE_VALUE);

    return;
    }


    void graphicsState::setGraphicsStateDict(char *pszDictName) {
Beep(2000,200);
    return;
    }


    void graphicsState::revertMatrix() {
    pToUserSpace -> revertTransform();
    return;
    }


    void graphicsState::transformPoint(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    *pX2 = pMatrix -> a() * x + pMatrix -> b() * y + pMatrix -> tx();
    *pY2 = pMatrix -> c() * x + pMatrix -> d() * y + pMatrix -> ty();
    return;
    }

    void graphicsState::transformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult;
    POINT_TYPE yResult;
    xResult = pToUserSpace -> a() * x + pToUserSpace -> b() * y + pToUserSpace -> tx();
    yResult = pToUserSpace -> c() * x + pToUserSpace -> d() * y + pToUserSpace -> ty();
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void graphicsState::untransformPoint(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult;
    POINT_TYPE yResult;
    xResult = pMatrix -> aInverse() * x + pMatrix -> bInverse() * y + pMatrix -> txInverse();
    yResult = pMatrix -> cInverse() * x + pMatrix -> dInverse() * y + pMatrix -> tyInverse();
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void graphicsState::untransformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult;
    POINT_TYPE yResult;
    xResult = pToUserSpace -> aInverse() * x + pToUserSpace -> bInverse() * y + pToUserSpace -> txInverse();
    yResult = pToUserSpace -> cInverse() * x + pToUserSpace -> dInverse() * y + pToUserSpace -> tyInverse();
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void graphicsState::scalePoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    *pX2 = x * pToUserSpace -> a();
    *pY2 = y * pToUserSpace -> d();
    return;
    }


    void graphicsState::transform(POINTD *pPoints,uint16_t pointCount) {
    for ( long k = 0; k < pointCount; k++ ) {
        POINT_TYPE x = pPoints[k].X();
        POINT_TYPE y = pPoints[k].Y();
        *pPoints[k].px = pToUserSpace -> a() * x + pToUserSpace -> b() * y + pToUserSpace -> tx();
        *pPoints[k].py = pToUserSpace -> c() * x + pToUserSpace -> d() * y + pToUserSpace -> ty();
    }
    return;
    }


    void graphicsState::transformInPlace(POINTD *pPoints,uint16_t pointCount) {
    for ( long k = 0; k < pointCount; k++ ) {
        POINT_TYPE x = pPoints[k].X();
        POINT_TYPE y = pPoints[k].Y();
        *pPoints[k].px = pToUserSpace -> a() * x + pToUserSpace -> b() * y;
        *pPoints[k].py = pToUserSpace -> c() * x + pToUserSpace -> d() * y;
    }
    return;
    }


    void graphicsState::translate(POINTD *pPoints,uint16_t pointCount,POINTD *pToPoint) {
    for ( long k = 0; k < pointCount; k++ ) {
        *pPoints[k].px += pToPoint -> X();
        *pPoints[k].py += pToPoint -> Y();
    }
    return;
    }


    void graphicsState::scale(POINTD *pPoints,uint16_t pointCount,POINT_TYPE scale) {
    for ( long k = 0; k < pointCount; k++ ) {
        *pPoints[k].px = *pPoints[k].px * scale;
        *pPoints[k].py = *pPoints[k].py * scale;
    }
    return;
    }


    void graphicsState::lerp(GS_POINT *pFrom,GS_POINT *pTo,POINT_TYPE ratio,GS_POINT *pResult) {
    pResult -> x = pFrom -> x + ratio * (pTo -> x - pFrom -> x);
    pResult -> y = pFrom -> y + ratio * (pTo -> y - pFrom -> y);
    return;
    }


    POINTD *graphicsState::lerp(POINTD *pFrom,POINTD *pTo,POINT_TYPE ratio) {
    return new POINTD(pFrom -> X() + ratio * (pTo -> X() - pFrom -> X()),pFrom -> Y() + ratio * (pTo -> Y() - pFrom -> Y()));
    }