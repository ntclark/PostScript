
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

    currentUserPoint = {x,y};

    transformPoint(&x,&y);

    currentGDIPoint = {x,y};

    if ( NULL == pCurrentPath )
        NewPath();

    if ( ! ( NULL == pCurrentPath -> pCurrentPrimitive ) ) {
        if ( primitive::type::move == pCurrentPath -> pCurrentPrimitive -> theType ) {
            // There are two consequetive moves, update the prior move with the new position
            pCurrentPath -> pCurrentPrimitive -> vertices[0] = currentGDIPoint;
            return S_OK;
        }
    } 

    pCurrentPath -> addPrimitive(new primitive(this,primitive::type::move,&currentGDIPoint));

    return S_OK;
    }


    HRESULT Renderer::GraphicElements::MoveToRelative(FLOAT x,FLOAT y) {
    return MoveTo(currentUserPoint.x + x,currentUserPoint.y + y);
    }


    HRESULT Renderer::GraphicElements::LineTo(FLOAT x,FLOAT y) {
    currentUserPoint = {x,y};
    transformPoint(&x,&y);
    currentGDIPoint = {x,y};
    pCurrentPath -> addPrimitive(new primitive(this,primitive::type::line,&currentGDIPoint));
    return S_OK;
    }


    HRESULT Renderer::GraphicElements::LineToRelative(FLOAT x,FLOAT y) {
    return LineTo(currentUserPoint.x + x,currentUserPoint.y + y);
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

    currentUserPoint = {x3,y3};

    transformPoint(&x1,&y1);
    transformPoint(&x2,&y2);
    transformPoint(&x3,&y3);

    currentGDIPoint = {x3,y3};

    D2D1_BEZIER_SEGMENT theSegment{{x1,y1},{x2,y2},{x3,y3}};
    pCurrentPath -> addPrimitive(new primitive(this,&theSegment));

    return S_OK;
    }


    HRESULT Renderer::GraphicElements::QuadraticBezierTo(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2) {

    POINTF priorCurrentPoint = currentGDIPoint;

    currentUserPoint = {x2,y2};

    transformPoint(&x1,&y1);
    transformPoint(&x2,&y2);

    currentGDIPoint = {x2,y2};

    D2D1_QUADRATIC_BEZIER_SEGMENT theSegment{{x1,y1},{x2,y2}};

    pCurrentPath -> addPrimitive(new primitive(this,&priorCurrentPoint,&theSegment));

    return E_NOTIMPL;
    }


    void Renderer::GraphicElements::transformPoint(FLOAT *px,FLOAT *py) {
    FLOAT xResult = toDeviceSpace.eM11 * *px + toDeviceSpace.eM12 * *py + toDeviceSpace.eDx;
    FLOAT yResult = toDeviceSpace.eM21 * *px + toDeviceSpace.eM22 * *py + toDeviceSpace.eDy;
    *px = xResult;
    *py = yResult;
    return;
    }