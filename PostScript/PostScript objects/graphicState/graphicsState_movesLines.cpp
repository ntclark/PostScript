
#include "job.h"

#include "pathParameters.h"
#include "gdiParameters.h"

    void graphicsState::outlinePage() {
    pathParametersStack.top() -> outlinePage(pageWidthPoints,pageHeightPoints);
    return;
    }


    void graphicsState::openGeometry() {
    pathParametersStack.top() -> openGeometry();
    gdiParametersStack.top() -> forwardToRenderer();
    return;
    }


    void graphicsState::closeGeometry() {
    pathParametersStack.top() -> closeGeometry();
    gdiParametersStack.top() -> revertToGDI();
    return;
    }


    void graphicsState::renderGeometry() {
    pathParametersStack.top() -> renderGeometry();
    return;
    }


    void graphicsState::RenderGeometry() {
    pathParameters::RenderGeometry();
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
    currentUserSpacePoint = {x,y};
    POINT_TYPE ptx;
    POINT_TYPE pty;
    transformPoint(x,y,&ptx,&pty);
    currentPageSpacePoint = {ptx,pty};
    pathParametersStack.top() -> moveto(ptx,pty);
    return;
    }


    void graphicsState::moveto(GS_POINT *pPt) {
    moveto(pPt -> x,pPt -> y);
    return;
    }


    void graphicsState::rmoveto() {
    object *pY = pJob -> pop();
    object *pX = pJob -> pop();
    rmoveto(pX -> OBJECT_POINT_TYPE_VALUE,pY -> OBJECT_POINT_TYPE_VALUE);
    return;
    }


    void graphicsState::rmoveto(POINT_TYPE x,POINT_TYPE y) {
    currentUserSpacePoint.x += x;
    currentUserSpacePoint.y += y;
    POINT_TYPE ptx;
    POINT_TYPE pty;
    transformPoint(x,y,&ptx,&pty);
    currentPageSpacePoint = {ptx,pty};
    pathParametersStack.top() -> rmoveto(ptx,pty);
    return;
    }


    void graphicsState::rmoveto(GS_POINT pt) {
    rmoveto(pt.x,pt.y);
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
    currentUserSpacePoint = {x,y};
    transformPoint(x,y,&x,&y);
    currentPageSpacePoint = {x,y};
    pathParametersStack.top() -> lineto(x,y);
    return;
    }


    void graphicsState::lineto(GS_POINT *pPt) {
    lineto(pPt -> x,pPt -> y);
    return;
    }


    void graphicsState::rlineto() {
    object *pY = pJob -> pop();
    object *pX = pJob -> pop();
    rlineto(pX -> OBJECT_POINT_TYPE_VALUE,pY -> OBJECT_POINT_TYPE_VALUE);
    return;
    }


    void graphicsState::rlineto(POINT_TYPE x,POINT_TYPE y) {
    currentUserSpacePoint.x += x;
    currentUserSpacePoint.y += y;
    transformPoint(x,y,&x,&y);
    currentPageSpacePoint = {x,y};
    pathParametersStack.top() -> rlineto(x,y);
    return;
    }


    void graphicsState::rlineto(GS_POINT pt) {
    rlineto(pt.x,pt.y);
    return;
    }


    void graphicsState::curveto() {

    object *pY3 = pJob -> pop();
    object *pX3 = pJob -> pop();
    object *pY2 = pJob -> pop();
    object *pX2 = pJob -> pop();
    object *pY1 = pJob -> pop();
    object *pX1 = pJob -> pop();

    GS_POINT points[3] { {pX1 -> OBJECT_POINT_TYPE_VALUE,pY1 -> OBJECT_POINT_TYPE_VALUE},
                         {pX2 -> OBJECT_POINT_TYPE_VALUE,pY2 -> OBJECT_POINT_TYPE_VALUE},
                         {pX3 -> OBJECT_POINT_TYPE_VALUE,pY3 -> OBJECT_POINT_TYPE_VALUE} };

    //currentUserSpacePoint.x = points[2].x;
    //currentUserSpacePoint.y = points[2].y;
    currentUserSpacePoint = points[2];

    transformPoint(points[0].x,points[0].y,&points[0].x,&points[0].y);
    transformPoint(points[1].x,points[1].y,&points[1].x,&points[1].y);
    transformPoint(points[2].x,points[2].y,&points[2].x,&points[2].y);

    currentPageSpacePoint = points[2];

    pathParametersStack.top() -> curveto(points[0].x,points[0].y,points[1].x,points[1].y,points[2].x,points[2].y);

    return;
    }


    void graphicsState::arcto(POINT_TYPE xCenter,POINT_TYPE yCenter,POINT_TYPE radius,POINT_TYPE angle1,POINT_TYPE angle2) {
    transformPoint(xCenter,yCenter,&xCenter,&yCenter);
    GS_POINT r{0.0,radius};
    transformPoint(0.0f,radius,&r.x,&r.y);
    radius = sqrt(r.x*r.x + r.y*r.y);
    pathParametersStack.top() -> arcto(xCenter,yCenter,radius,angle1,angle2);
    return;
    }


    void graphicsState::dot(GS_POINT at,POINT_TYPE radius) {
    pathParametersStack.top() -> dot(at,radius);
    }