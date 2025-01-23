#include "job.h"

#include "pathParameters.h"

#ifdef USE_RENDERER
#else

    pathParameters::GraphicElements::GraphicElements(pathParameters *pp) : pParent(pp) {
    return;
    }


    HRESULT pathParameters::GraphicElements::NewPath() {
    pParent -> currentUserPoint = POINT_TYPE_NAN_POINT;
    pParent -> currentDevicePoint = POINT_TYPE_NAN_POINT;
    pParent -> currentGDIPoint = {0,0};
    isPathActive = false;
    return S_OK;
    }


    HRESULT pathParameters::GraphicElements::ClosePath() {
    if ( isPathActive ) 
        ::EndPath(pPStoPDF -> GetDC());
    isPathActive = false;
    return S_OK;
    }


    HRESULT pathParameters::GraphicElements::StrokePath() {
    if ( isPathActive ) {
        ::EndPath(pPStoPDF -> GetDC());
        ::StrokePath(pPStoPDF -> GetDC());
    }
    isPathActive = false;
    return S_OK;
    }


    HRESULT pathParameters::GraphicElements::FillPath() {
    if ( isPathActive )
        pParent -> closepath();
    ::StrokeAndFillPath(pPStoPDF -> GetDC());
    return S_OK;
    }


    HRESULT pathParameters::GraphicElements::MoveTo(POINT_TYPE x,POINT_TYPE y) {
/*
    moveto 
        x y moveto –

    starts a new subpath of the current path (see Section 4.4, “Path Construction”) by
    setting the current point in the graphics state to the coordinates (x, y) in user
    space. No new line segments are added to the current path.
    If the previous path operation in the current path was moveto or rmoveto, that
    point is deleted from the current path and the new moveto point replaces it.
*/
    HDC hdc = pPStoPDF -> GetDC();

    if ( ! isPathActive ) {
        pPStoPDF -> BeginPath();
        isPathActive = true;
        BeginPath(hdc);
    }

    POINT_TYPE ptx2;
    POINT_TYPE pty2;

    pParent -> currentUserPoint = {x,y};

    pParent -> transformPoint(x,y,&ptx2,&pty2);

    pParent -> currentDevicePoint = {ptx2,pty2};

    MoveToEx(hdc,std::lround(pParent -> currentDevicePoint.x),std::lround(pParent -> currentDevicePoint.y),NULL);

    GetCurrentPositionEx(hdc,&pParent -> currentGDIPoint);

    return S_OK;
    }


    HRESULT pathParameters::GraphicElements::MoveToRelative(POINT_TYPE x,POINT_TYPE y) {
    pParent -> currentUserPoint.x += x;
    pParent -> currentUserPoint.y += y;
    return MoveTo(pParent -> currentUserPoint.x,pParent -> currentUserPoint.y);
    }


    HRESULT pathParameters::GraphicElements::LineTo(POINT_TYPE x,POINT_TYPE y) {

    if ( ! isPathActive ) {
        char szMessage[1024];
        sprintf(szMessage,"operator: lineto. lineto called with no current point");
        MessageBox(NULL,szMessage,__FILE__,MB_OK);
        return E_FAIL;
    }

    pParent -> currentUserPoint = {x,y};

    POINT_TYPE ptx2;
    POINT_TYPE pty2;

    pParent -> transformPoint(x,y,&ptx2,&pty2);

    pParent -> currentDevicePoint.x = ptx2;
    pParent -> currentDevicePoint.y = pty2;

    ::LineTo(pPStoPDF -> GetDC(),std::lround(pParent -> currentDevicePoint.x),std::lround(pParent -> currentDevicePoint.y));

    GetCurrentPositionEx(pPStoPDF -> GetDC(),&pParent -> currentGDIPoint);

    return S_OK;
    }


    HRESULT pathParameters::GraphicElements::LineToRelative(POINT_TYPE x,POINT_TYPE y) {
    pParent -> currentUserPoint.x += x;
    pParent -> currentUserPoint.y += y;
    return LineTo(pParent -> currentUserPoint.x,pParent -> currentUserPoint.y);
    }


    HRESULT pathParameters::GraphicElements::ArcTo(POINT_TYPE xCenter,POINT_TYPE yCenter,POINT_TYPE radius,POINT_TYPE angle1,POINT_TYPE angle2) {

    POINT_TYPE theXCenter = xCenter;
    POINT_TYPE theYCenter = yCenter;
    POINT_TYPE theRadius = radius;

    if ( ! isPathActive )
        MoveTo(theXCenter + theRadius * cos(graphicsState::degToRad * (angle1 )),
                    theYCenter + theRadius * sin(graphicsState::degToRad * (angle1 )));

    GS_POINT center{theXCenter,theYCenter};

    pParent -> transform(&center,1);

    BOOL rv = AngleArc(pPStoPDF -> GetDC(),std::lround((POINT_TYPE)center.x),
                            std::lround((POINT_TYPE)center.y),
                            std::lround((POINT_TYPE)theRadius),(float)(angle1 + pParent -> totalRotation),(float)(angle1 - angle2));

    GetCurrentPositionEx(pPStoPDF -> GetDC(),&pParent -> currentGDIPoint);

    return S_OK;
    }


    HRESULT pathParameters::GraphicElements::CubicBezierTo(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2,FLOAT x3,FLOAT y3) {

    POINT *pThePts = new POINT[3];

    GS_POINT pts[3]{{x1,y1},{x2,y2},{x3,y3}};

    pParent -> currentUserPoint = {x3,y3};

    pParent -> transform(pts,3);

    pParent -> currentDevicePoint.x = pts[2].x;
    pParent -> currentDevicePoint.y = pts[2].y;

    for ( long k = 0; k < 3; k++ ) {
        pThePts[k].x = std::lround((POINT_TYPE)pts[k].x);
        pThePts[k].y = std::lround((POINT_TYPE)pts[k].y);
    }

    PolyBezierTo(pPStoPDF -> GetDC(),pThePts,3);

    GetCurrentPositionEx(pPStoPDF -> GetDC(),&pParent -> currentGDIPoint);

    delete [] pThePts;

    return S_OK;
    }


    HRESULT pathParameters::GraphicElements::QuadraticBezierTo(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2) {
Beep(2000,200);
    return E_NOTIMPL;
    }

#endif