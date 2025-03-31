
#include "job.h"

#include "pathParameters.h"
#include "gdiParameters.h"


    POINTF *graphicsState::CurrentPoint() {
    static POINTF currentPoint;
    currentPoint.x = currentUserSpacePoint.x;
    currentPoint.y = currentUserSpacePoint.y;
    return &currentPoint;
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


    void graphicsState::moveto(FLOAT x,FLOAT y) {
    currentUserSpacePoint = {x,y};
    transformPoint(&currentUserSpacePoint,&currentPageSpacePoint);
    pathParametersStack.top() -> moveto(&currentPageSpacePoint);
    return;
    }


    void graphicsState::moveto(GS_POINT *pPt) {
    moveto(pPt -> x,pPt -> y);
    return;
    }


    void graphicsState::moveto(POINTF *pPt) {
    moveto(pPt -> x,pPt -> y);
    return;
    }


    void graphicsState::rmoveto() {
    object *pY = pJob -> pop();
    object *pX = pJob -> pop();
    rmoveto(pX -> OBJECT_POINT_TYPE_VALUE,pY -> OBJECT_POINT_TYPE_VALUE);
    return;
    }


    void graphicsState::rmoveto(FLOAT x,FLOAT y) {
    currentUserSpacePoint.x += x;
    currentUserSpacePoint.y += y;
    transformPoint(&currentUserSpacePoint,&currentPageSpacePoint);
    pathParametersStack.top() -> moveto(&currentPageSpacePoint);
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


    void graphicsState::lineto(FLOAT x,FLOAT y) {
    currentUserSpacePoint = {x,y};
    transformPoint(&currentUserSpacePoint,&currentPageSpacePoint);
    pathParametersStack.top() -> lineto(&currentPageSpacePoint);
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


    void graphicsState::rlineto(FLOAT x,FLOAT y) {
    currentUserSpacePoint.x += x;
    currentUserSpacePoint.y += y;
    transformPoint(&currentUserSpacePoint,&currentPageSpacePoint);
    pathParametersStack.top() -> lineto(&currentPageSpacePoint);
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

    GS_POINT points[] { currentUserSpacePoint,
                         {pX1 -> OBJECT_POINT_TYPE_VALUE,pY1 -> OBJECT_POINT_TYPE_VALUE},
                         {pX2 -> OBJECT_POINT_TYPE_VALUE,pY2 -> OBJECT_POINT_TYPE_VALUE},
                         {pX3 -> OBJECT_POINT_TYPE_VALUE,pY3 -> OBJECT_POINT_TYPE_VALUE} };

    currentUserSpacePoint = points[3];

    transformPoint(points[0].x,points[0].y,&points[0].x,&points[0].y);
    transformPoint(points[1].x,points[1].y,&points[1].x,&points[1].y);
    transformPoint(points[2].x,points[2].y,&points[2].x,&points[2].y);
    transformPoint(points[3].x,points[3].y,&points[3].x,&points[3].y);

    currentPageSpacePoint = points[3];

    pathParametersStack.top() -> curveto(points[0].x,points[0].y,points[1].x,points[1].y,points[2].x,points[2].y,points[3].x,points[3].y);

    return;
    }


    void graphicsState::quadcurveto() {

    object *pY2 = pJob -> pop();
    object *pX2 = pJob -> pop();
    object *pY1 = pJob -> pop();
    object *pX1 = pJob -> pop();

    GS_POINT points[2] { {pX1 -> OBJECT_POINT_TYPE_VALUE,pY1 -> OBJECT_POINT_TYPE_VALUE},
                         {pX2 -> OBJECT_POINT_TYPE_VALUE,pY2 -> OBJECT_POINT_TYPE_VALUE} };

    currentUserSpacePoint = points[1];

    transformPoint(points[0].x,points[0].y,&points[0].x,&points[0].y);
    transformPoint(points[1].x,points[1].y,&points[1].x,&points[1].y);

    currentPageSpacePoint = points[1];

    pathParametersStack.top() -> quadcurveto(points[0].x,points[0].y,points[1].x,points[1].y);

    return;
    }


    void graphicsState::arcto(FLOAT xCenter,FLOAT yCenter,FLOAT radius,FLOAT angle1,FLOAT angle2) {
    transformPoint(xCenter,yCenter,&xCenter,&yCenter);
    GS_POINT r{0.0,radius};
    transformPoint(0.0f,radius,&r.x,&r.y);
    radius = sqrt(r.x*r.x + r.y*r.y);
    pathParametersStack.top() -> arcto(xCenter,yCenter,radius,angle1,angle2);
    return;
    }


    void graphicsState::dot(GS_POINT at,FLOAT radius) {
Beep(2000,200);
    return;
    }