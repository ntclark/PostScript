
#include "EnVisioNateSW_Renderer.h"

    HRESULT Renderer::GraphicElements::QueryInterface(REFIID refIID,void **pvResult) {

    if ( ! pvResult )
        return E_POINTER;

    *pvResult = NULL;

    if ( IID_IUnknown == refIID ) 
        *pvResult = static_cast<IUnknown *>(this);

    else if ( IID_IGraphicElements == refIID )
        *pvResult = static_cast<GraphicElements *>(this);

    if ( * pvResult ) {
        AddRef();
        return S_OK;
    }

    return pParent -> QueryInterface(refIID,pvResult);
    }


    HRESULT Renderer::GraphicElements::MoveTo(FLOAT x,FLOAT y) {
/*
    moveto 
        x y moveto –

    starts a new subpath of the current path (see Section 4.4, “Path Construction”) by
    setting the current point in the graphics state to the coordinates (x, y) in user
    space. No new line segments are added to the current path.
    If the previous path operation in the current path was moveto or rmoveto, that
    point is deleted from the current path and the new moveto point replaces it.
*/

    currentPageSpacePoint = {x,y};

    if ( NULL == pCurrentPath )
        NewPath();

    if ( ! ( NULL == pCurrentPath -> pLastPrimitive ) ) {
        if ( primitive::type::move == pCurrentPath -> pLastPrimitive -> theType ) {
            // There are two consequetive moves, update the prior move with the new position
            pCurrentPath -> pLastPrimitive -> vertices[0] = currentPageSpacePoint;
            return S_OK;
        }
    } 

    pCurrentPath -> addPrimitive(new movePrimitive(this,&currentPageSpacePoint));

    return S_OK;
    }


    HRESULT Renderer::GraphicElements::MoveToRelative(FLOAT x,FLOAT y) {
    return MoveTo(currentPageSpacePoint.x + x,currentPageSpacePoint.y + y);
    }


    HRESULT Renderer::GraphicElements::LineTo(FLOAT x,FLOAT y) {
    currentPageSpacePoint = {x,y};
    pCurrentPath -> addPrimitive(new linePrimitive(this,&currentPageSpacePoint));
    return S_OK;
    }


    HRESULT Renderer::GraphicElements::LineToRelative(FLOAT x,FLOAT y) {
    return LineTo(currentPageSpacePoint.x + x,currentPageSpacePoint.y + y);
    }


    HRESULT Renderer::GraphicElements::ArcTo(FLOAT xCenter,FLOAT yCenter,FLOAT radius,FLOAT angle1,FLOAT angle2) {

Beep(2000,200);
return E_NOTIMPL;
#if 0
    D2D1_ARC_SEGMENT arcSegment{0};

    POINTF endPoint;

    arcSegment.point.x = xCenter + radius * cos(degToRad * angle1);
    arcSegment.point.y = yCenter + radius * sin(degToRad * angle1);
    arcSegment.size = radius;
    arcSegment.rotationAngle = angle2 - angle1;

//    sweepDirection = 
//  D2D1_ARC_SIZE        arcSize;
//} D2D1_ARC_SEGMENT;
    return S_OK;
#endif
    }


    HRESULT Renderer::GraphicElements::CubicBezierTo(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2,FLOAT x3,FLOAT y3) {
    currentPageSpacePoint = {x3,y3};
    D2D1_BEZIER_SEGMENT theSegment{{x1,y1},{x2,y2},{x3,y3}};
    pCurrentPath -> addPrimitive(new bezierPrimitive(this,&theSegment));
    return S_OK;
    }


    HRESULT Renderer::GraphicElements::QuadraticBezierTo(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2) {
    POINTF priorCurrentPoint = currentPageSpacePoint;
    currentPageSpacePoint = {x2,y2};
    D2D1_QUADRATIC_BEZIER_SEGMENT theSegment{{x1,y1},{x2,y2}};
    pCurrentPath -> addPrimitive(new quadraticBezierPrimitive(this,&priorCurrentPoint,&theSegment));
    return E_NOTIMPL;
    }


    HRESULT Renderer::GraphicElements::Image(HBITMAP hbmImage,UINT_PTR pPSCurrentCTM,FLOAT width,FLOAT height) {

    FLOAT wUserSpace = 1.0f;
    FLOAT hUserSpace = 1.0f;

    XFORM *pXForm = (XFORM *)pPSCurrentCTM;

    FLOAT xResult = pXForm -> eM11 * wUserSpace + pXForm -> eM12 * hUserSpace;
    FLOAT yResult = pXForm -> eM21 * wUserSpace + pXForm -> eM22 * hUserSpace;

    wUserSpace = toDeviceSpace.eM11 * xResult + toDeviceSpace.eM12 * yResult;
    hUserSpace = toDeviceSpace.eM21 * xResult + toDeviceSpace.eM22 * yResult;

    //moveto(0.0,0.0);

    HDC hdcSource = CreateCompatibleDC(pParent -> hdc);
    SelectObject(hdcSource,hbmImage);

    StretchBlt(pParent -> hdc,0,0,(long)wUserSpace,-(long)hUserSpace,
                hdcSource,0,0,(long)width,(long)height,SRCCOPY);

    return S_OK;
    }


    void Renderer::GraphicElements::scalePoint(FLOAT *px,FLOAT *py) {
    FLOAT xResult = toDeviceSpace.eM11 * *px + toDeviceSpace.eM12 * *py;
    FLOAT yResult = toDeviceSpace.eM21 * *px + toDeviceSpace.eM22 * *py;
    *px = xResult;
    *py = yResult;
    return;
    }


    void Renderer::GraphicElements::transformPoint(FLOAT *px,FLOAT *py) {
    FLOAT xResult = toDeviceSpace.eM11 * *px + toDeviceSpace.eM12 * *py + toDeviceSpace.eDx;
    FLOAT yResult = toDeviceSpace.eM21 * *px + toDeviceSpace.eM22 * *py + toDeviceSpace.eDy;
    *px = xResult;
    *py = yResult;
    return;
    }


    void Renderer::GraphicElements::transformPoint(POINTF *ptIn,POINTF *ptOut) {
    FLOAT xResult = toDeviceSpace.eM11 * ptIn -> x + toDeviceSpace.eM12 * ptIn -> y + toDeviceSpace.eDx;
    FLOAT yResult = toDeviceSpace.eM21 * ptIn -> x + toDeviceSpace.eM22 * ptIn -> y + toDeviceSpace.eDy;
    ptOut -> x = xResult;
    ptOut -> y = yResult;
    return;
    }


    void Renderer::GraphicElements::transformPoint(D2D1_POINT_2F *ptIn,D2D1_POINT_2F *ptOut) {
    FLOAT xResult = toDeviceSpace.eM11 * ptIn -> x + toDeviceSpace.eM12 * ptIn -> y + toDeviceSpace.eDx;
    FLOAT yResult = toDeviceSpace.eM21 * ptIn -> x + toDeviceSpace.eM22 * ptIn -> y + toDeviceSpace.eDy;
    ptOut -> x = xResult;
    ptOut -> y = yResult;
    return;
    }
