
#include "PostScript objects/graphicsState.h"

    void graphicsState::outlinePage() {

    POINT currentGDIPoint;

    BeginPath(pPStoPDF -> GetDC());

    MoveToEx(pPStoPDF -> GetDC(),std::lround(PICA_FACTOR * 72),std::lround(PICA_FACTOR * 72),NULL);
    GetCurrentPositionEx(pPStoPDF -> GetDC(),&currentGDIPoint);
    LineTo(pPStoPDF -> GetDC(),std::lround(PICA_FACTOR * (pageWidthPoints - 72)),std::lround(PICA_FACTOR * (pageHeightPoints - 72)));
    GetCurrentPositionEx(pPStoPDF -> GetDC(),&currentGDIPoint);
    LineTo(pPStoPDF -> GetDC(),std::lround(PICA_FACTOR * 72),std::lround(PICA_FACTOR * (pageHeightPoints - 72)));

    EndPath(pPStoPDF -> GetDC());
    StrokePath(pPStoPDF -> GetDC());

    GetCurrentPositionEx(pPStoPDF -> GetDC(),&currentGDIPoint);
    return;
    }

    void graphicsState::moveto() {
    object *pY = pJob -> pop();
    object *pX = pJob -> pop();
    moveto(pX,pY);
    return;
    }


    void graphicsState::moveto(object *pX,object *pY) {
    moveto(pX -> OBJECT_POINT_TYPE_VALUE,pY -> OBJECT_POINT_TYPE_VALUE);
    return;
    }


    void graphicsState::moveto(POINT_TYPE x,POINT_TYPE y) {
/*
    moveto 
        x y moveto –

    starts a new subpath of the current path (see Section 4.4, “Path Construction”) by
    setting the current point in the graphics state to the coordinates (x, y) in user
    space. No new line segments are added to the current path.
    If the previous path operation in the current path was moveto or rmoveto, that
    point is deleted from the current path and the new moveto point replaces it.
*/
    if ( ! gdiParametersStack.top() -> isPathActive ) {
SetLastError(0);
        BeginPath(pPStoPDF -> GetDC());
        pathBeginPoint = {x,y};
        gdiParametersStack.top() -> isPathActive = true;
    }

    currentUserSpacePoint = {x,y};

    POINT_TYPE ptx2;
    POINT_TYPE pty2;
    transformPoint(x,y,&ptx2,&pty2);
    currentPointsPoint = {ptx2,pty2};
    MoveToEx(pPStoPDF -> GetDC(),std::lround(PICA_FACTOR * currentPointsPoint.x),std::lround(PICA_FACTOR * currentPointsPoint.y),NULL);
    GetCurrentPositionEx(pPStoPDF -> GetDC(),&currentGDIPoint);

    return;
    }


    void graphicsState::moveto(GS_POINT *pPt) {
    moveto(pPt -> x,pPt -> y);
    return;
    }


    void graphicsState::moveto(POINTD *pPointd) {
    moveto(pPointd -> X(),pPointd -> Y());
    return;
    }


    void graphicsState::rmoveto() {
    object *pY = pJob -> pop();
    object *pX = pJob -> pop();
    rmoveto(pX -> OBJECT_POINT_TYPE_VALUE,pY -> OBJECT_POINT_TYPE_VALUE);
    return;
    }


    void graphicsState::rmoveto(POINT_TYPE x,POINT_TYPE y) {
    moveto(currentUserSpacePoint.x + x,currentUserSpacePoint.y + y);
    return;
    }


    void graphicsState::rmoveto(GS_POINT pt) {
    rmoveto(pt.x,pt.y);
    return;
    }


    void graphicsState::rmoveto(POINTD *pPointd) {
    rmoveto(pPointd -> X(),pPointd -> Y());
    return;
    }


    void graphicsState::lineto() {
    object *pY = pJob -> pop();
    object *pX = pJob -> pop();
    lineto(pX,pY);
    return;
    }


    void graphicsState::lineto(object *pX,object *pY) {
    lineto(pX -> OBJECT_POINT_TYPE_VALUE,pY -> OBJECT_POINT_TYPE_VALUE);
    return;
    }


    void graphicsState::lineto(POINT_TYPE x,POINT_TYPE y) {
    if ( ! gdiParametersStack.top() -> isPathActive ) {
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
    LineTo(pPStoPDF -> GetDC(),std::lround(PICA_FACTOR * currentPointsPoint.x),std::lround(PICA_FACTOR * currentPointsPoint.y));
    GetCurrentPositionEx(pPStoPDF -> GetDC(),&currentGDIPoint);
    return;
    }


    void graphicsState::lineto(GS_POINT *pPt) {
    lineto(pPt -> x,pPt -> y);
    return;
    }


    void graphicsState::lineto(POINTD *pPointd) {
    lineto(pPointd -> X(),pPointd -> Y());
    return;
    }


    void graphicsState::rlineto() {
    object *pY = pJob -> pop();
    object *pX = pJob -> pop();
    rlineto(pX -> OBJECT_POINT_TYPE_VALUE,pY -> OBJECT_POINT_TYPE_VALUE);
    return;
    }


    void graphicsState::rlineto(POINT_TYPE x,POINT_TYPE y) {
    lineto(currentUserSpacePoint.x + x,currentUserSpacePoint.y + y);
    return;
    }


    void graphicsState::rlineto(GS_POINT pt) {
    rlineto(pt.x,pt.y);
    return;
    }


    void graphicsState::rlineto(POINTD *pPointd) {
    rlineto(pPointd -> X(),pPointd -> Y());
    return;
    }


    void graphicsState::curveto() {

    object *pY3 = pJob -> pop();
    object *pX3 = pJob -> pop();
    object *pY2 = pJob -> pop();
    object *pX2 = pJob -> pop();
    object *pY1 = pJob -> pop();
    object *pX1 = pJob -> pop();

    GS_POINT points[3] { {pX1 -> DoubleValue(),pY1 -> DoubleValue()},
                         {pX2 -> DoubleValue(),pY2 -> DoubleValue()},
                         {pX3 -> DoubleValue(),pY3 -> DoubleValue()} };

    curveto(points);

    return;
    }


    void graphicsState::curveto(GS_POINT *pPoints) {

    POINT *pThePts = new POINT[3];

    GS_POINT pts[3]{*pPoints,*(pPoints + 1),*(pPoints + 2)};

    transform(pts,3);

    for ( long k = 0; k < 3; k++ ) {
        pThePts[k].x = std::lround((POINT_TYPE)PICA_FACTOR * pts[k].x);
        pThePts[k].y = std::lround((POINT_TYPE)PICA_FACTOR * pts[k].y);
    }

    PolyBezierTo(pPStoPDF -> GetDC(),pThePts,3);

    GetCurrentPositionEx(pPStoPDF -> GetDC(),&currentGDIPoint);

    untransformPoint((POINT_TYPE)pThePts[2].x / (POINT_TYPE)PICA_FACTOR,
                        (POINT_TYPE)pThePts[2].y / (POINT_TYPE)PICA_FACTOR,
                            &currentUserSpacePoint.x,&currentUserSpacePoint.y);

    delete [] pThePts;

    return;
    }


    void graphicsState::arcto(POINT_TYPE xCenter,POINT_TYPE yCenter,POINT_TYPE radius,POINT_TYPE angle1,POINT_TYPE angle2) {

    if ( isnan(pathBeginPoint.x) ) 
        moveto(xCenter + radius * cos(degToRad * (angle1 )),yCenter + radius * sin(degToRad * (angle1 )));

    GS_POINT center{xCenter,yCenter};
    transform(&center,1);

    BOOL rv = AngleArc(pPStoPDF -> GetDC(),std::lround((POINT_TYPE)PICA_FACTOR * center.x),
                            std::lround((POINT_TYPE)PICA_FACTOR * center.y),
                            std::lround((POINT_TYPE)PICA_FACTOR * radius),(float)(angle1 + totalRotation),(float)(angle1 - angle2));

    GetCurrentPositionEx(pPStoPDF -> GetDC(),&currentGDIPoint);
    untransformPoint((POINT_TYPE)currentGDIPoint.x / (POINT_TYPE)PICA_FACTOR,
                        (POINT_TYPE)currentGDIPoint.y / (POINT_TYPE)PICA_FACTOR,
                            &currentUserSpacePoint.x,&currentUserSpacePoint.y);

    return;
    }


    void graphicsState::gdiMoveTo(POINT *pGDIPoint) {
    MoveToEx(pPStoPDF -> GetDC(),pGDIPoint -> x,pGDIPoint -> y,NULL);
    return;
    }