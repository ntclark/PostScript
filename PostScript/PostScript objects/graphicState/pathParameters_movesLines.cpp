#include "job.h"

#include "pathParameters.h"

    matrix *pathParameters::pUserSpaceToDeviceSpace = NULL;

    void pathParameters::moveto(POINT_TYPE x,POINT_TYPE y) {
/*
    moveto 
        x y moveto –

    starts a new subpath of the current path (see Section 4.4, “Path Construction”) by
    setting the current point in the graphics state to the coordinates (x, y) in user
    space. No new line segments are added to the current path.
    If the previous path operation in the current path was moveto or rmoveto, that
    point is deleted from the current path and the new moveto point replaces it.
*/
    if ( ! isPathActive ) {
        pPStoPDF -> BeginPath();
        BeginPath(pPStoPDF -> GetDC());
        pathBeginPoint = {x,y};
        isPathActive = true;
    }

    HDC hdc = pPStoPDF -> GetDC();

    currentUserSpacePoint = {x,y};

    POINT_TYPE ptx2;
    POINT_TYPE pty2;
    transformPoint(x,y,&ptx2,&pty2);
    currentPointsPoint = {ptx2,pty2};

    if ( saveThePath )
        pSavedPath -> addMove(currentPointsPoint);

    MoveToEx(hdc,std::lround(currentPointsPoint.x),std::lround(currentPointsPoint.y),NULL);
    GetCurrentPositionEx(hdc,&currentGDIPoint);

    return;
    }


    void pathParameters::moveto(GS_POINT *pPt) {
    moveto(pPt -> x,pPt -> y);
    return;
    }


    void pathParameters::rmoveto(POINT_TYPE x,POINT_TYPE y) {
    moveto(currentUserSpacePoint.x + x,currentUserSpacePoint.y + y);
    return;
    }


    void pathParameters::rmoveto(GS_POINT pt) {
    rmoveto(pt.x,pt.y);
    return;
    }


    void pathParameters::lineto(POINT_TYPE x,POINT_TYPE y) {
    if ( ! isPathActive ) {
        char szMessage[1024];
        sprintf(szMessage,"operator: lineto. lineto called with no current point");
        throw nocurrentpoint(szMessage);
        return;
    }
    currentUserSpacePoint.x = x;
    currentUserSpacePoint.y = y;
    POINT_TYPE ptx2;
    POINT_TYPE pty2;
    transformPoint(x,y,&ptx2,&pty2);
    currentPointsPoint.x = ptx2;
    currentPointsPoint.y = pty2;

    if ( saveThePath )
        pSavedPath -> addLine(currentPointsPoint);

    LineTo(pPStoPDF -> GetDC(),std::lround(currentPointsPoint.x),std::lround(currentPointsPoint.y));
    GetCurrentPositionEx(pPStoPDF -> GetDC(),&currentGDIPoint);
    return;
    }


    void pathParameters::lineto(GS_POINT *pPt) {
    lineto(pPt -> x,pPt -> y);
    return;
    }


    void pathParameters::rlineto(POINT_TYPE x,POINT_TYPE y) {
    lineto(currentUserSpacePoint.x + x,currentUserSpacePoint.y + y);
    return;
    }


    void pathParameters::rlineto(GS_POINT pt) {
    rlineto(pt.x,pt.y);
    return;
    }


    void pathParameters::curveto(GS_POINT *pPoints) {

    POINT *pThePts = new POINT[3];

    GS_POINT pts[3]{*pPoints,*(pPoints + 1),*(pPoints + 2)};

    transform(pts,3);

    if ( saveThePath )
        pSavedPath -> addCurve(pts[0],pts[1],pts[2]);

    for ( long k = 0; k < 3; k++ ) {
        pThePts[k].x = std::lround((POINT_TYPE)pts[k].x);
        pThePts[k].y = std::lround((POINT_TYPE)pts[k].y);
    }

    PolyBezierTo(pPStoPDF -> GetDC(),pThePts,3);

    GetCurrentPositionEx(pPStoPDF -> GetDC(),&currentGDIPoint);

    currentUserSpacePoint = *(pPoints + 2);

    delete [] pThePts;

    return;
    }


    void pathParameters::arcto(POINT_TYPE xCenter,POINT_TYPE yCenter,POINT_TYPE radius,POINT_TYPE angle1,POINT_TYPE angle2) {

    if ( isnan(pathBeginPoint.x) ) 
        moveto(xCenter + radius * cos(graphicsState::degToRad * (angle1 )),yCenter + radius * sin(graphicsState::degToRad * (angle1 )));

    GS_POINT center{xCenter,yCenter};
    transform(&center,1);

    BOOL rv = AngleArc(pPStoPDF -> GetDC(),std::lround((POINT_TYPE)center.x),
                            std::lround((POINT_TYPE)center.y),
                            std::lround((POINT_TYPE)radius),(float)(angle1 + totalRotation),(float)(angle1 - angle2));

    GetCurrentPositionEx(pPStoPDF -> GetDC(),&currentGDIPoint);
    untransformPoint((POINT_TYPE)currentGDIPoint.x,
                        (POINT_TYPE)currentGDIPoint.y,
                            &currentUserSpacePoint.x,&currentUserSpacePoint.y);

    return;
    }


    void pathParameters::dot(GS_POINT at,POINT_TYPE radius) {

    POINT_TYPE ptx;
    POINT_TYPE pty;

    transformPoint(at.x,at.y,&ptx,&pty);

    BOOL rv = Arc(pPStoPDF -> GetDC(),
                            std::lround((POINT_TYPE)ptx - radius),
                            std::lround((POINT_TYPE)pty - radius),
                            std::lround((POINT_TYPE)ptx + radius),
                            std::lround((POINT_TYPE)pty + radius),
                            std::lround((POINT_TYPE)ptx + radius),
                            std::lround((POINT_TYPE)pty + radius / 2),
                            std::lround((POINT_TYPE)ptx + radius),
                            std::lround((POINT_TYPE)pty + radius / 2));

    return;
    }