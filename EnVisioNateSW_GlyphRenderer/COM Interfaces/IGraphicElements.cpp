
#include "EnVisioNateSW_GlyphRenderer.h"

    HRESULT GlyphRenderer::GraphicElements::QueryInterface(REFIID refIID,void **pvResult) {

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


    HRESULT GlyphRenderer::GraphicElements::MoveTo(FLOAT x,FLOAT y) {

    currentUserPoint = {x,y};

    transformPoint(&x,&y);

    currentGDIPoint = {x,y};

    if ( savePath )
        thePath.push_back(new primitive(primitive::type::move,&currentGDIPoint));
    else {
MessageBox(NULL,"Outside of path","Error",MB_OK);
        if ( isFigureStarted )
            pParent -> pID2D1GeometrySink -> EndFigure(D2D1_FIGURE_END_CLOSED);
        pParent -> pID2D1GeometrySink -> BeginFigure(D2D1::Point2F(x,y),/*doFill ? */D2D1_FIGURE_BEGIN_FILLED);// : D2D1_FIGURE_BEGIN_HOLLOW );
        isFigureStarted = true;
    }
    return S_OK;
    }


    HRESULT GlyphRenderer::GraphicElements::MoveToRelative(FLOAT x,FLOAT y) {
    return MoveTo(currentUserPoint.x + x,currentUserPoint.y + y);
    }


    HRESULT GlyphRenderer::GraphicElements::LineTo(FLOAT x,FLOAT y) {

    currentUserPoint = {x,y};

    transformPoint(&x,&y);

    currentGDIPoint = {x,y};

    if ( savePath )
        thePath.push_back(new primitive(primitive::type::line,&currentGDIPoint));
    else
{
MessageBox(NULL,"Outside of path","Error",MB_OK);
        pParent -> pID2D1GeometrySink -> AddLine(D2D1::Point2F(x,y));
}
    return S_OK;
    }


    HRESULT GlyphRenderer::GraphicElements::LineToRelative(FLOAT x,FLOAT y) {
    return LineTo(currentUserPoint.x + x,currentUserPoint.y + y);
    }


    HRESULT GlyphRenderer::GraphicElements::ArcTo(FLOAT xCenter,FLOAT yCenter,FLOAT radius,FLOAT angle1,FLOAT angle2) {

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


    HRESULT GlyphRenderer::GraphicElements::CubicBezierTo(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2,FLOAT x3,FLOAT y3) {

    currentUserPoint = {x3,y3};

    transformPoint(&x1,&y1);
    transformPoint(&x2,&y2);
    transformPoint(&x3,&y3);

    currentGDIPoint = {x3,y3};

    D2D1_BEZIER_SEGMENT theSegment{{x1,y1},{x2,y2},{x3,y3}};

    if ( savePath )
        thePath.push_back(new primitive(&theSegment));
    else
        pParent -> pID2D1GeometrySink -> AddBezier(&theSegment);

    return S_OK;
    }


    HRESULT GlyphRenderer::GraphicElements::QuadraticBezierTo(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2) {

    currentUserPoint = {x2,y2};

    transformPoint(&x1,&y1);
    transformPoint(&x2,&y2);

    currentGDIPoint = {x2,y2};

    D2D1_QUADRATIC_BEZIER_SEGMENT theSegment{{x1,y1},{x2,y2}};

    if ( savePath )
        thePath.push_back(new primitive(&theSegment));
    else
        pParent -> pID2D1GeometrySink -> AddQuadraticBezier(&theSegment);

    return E_NOTIMPL;
    }


    void GlyphRenderer::GraphicElements::transformPoint(FLOAT *px,FLOAT *py) {
    FLOAT xResult = toDeviceSpace.eM11 * *px + toDeviceSpace.eM12 * *py + toDeviceSpace.eDx;
    FLOAT yResult = toDeviceSpace.eM21 * *px + toDeviceSpace.eM22 * *py + toDeviceSpace.eDy;
    *px = xResult;
    *py = yResult;
    return;
    }