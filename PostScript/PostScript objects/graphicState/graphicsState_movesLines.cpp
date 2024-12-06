
#include "job.h"

#include "pathParameters.h"

    void graphicsState::outlinePage() {

    POINT currentGDIPoint;

    BeginPath(pPStoPDF -> GetDC());

    MoveToEx(pPStoPDF -> GetDC(),std::lround(72),std::lround(72),NULL);
    GetCurrentPositionEx(pPStoPDF -> GetDC(),&currentGDIPoint);
    LineTo(pPStoPDF -> GetDC(),std::lround((pageWidthPoints - 72)),std::lround((pageHeightPoints - 72)));
    GetCurrentPositionEx(pPStoPDF -> GetDC(),&currentGDIPoint);
    LineTo(pPStoPDF -> GetDC(),std::lround(72),std::lround((pageHeightPoints - 72)));

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
    pathParametersStack.top() -> moveto(pX -> OBJECT_POINT_TYPE_VALUE,pY -> OBJECT_POINT_TYPE_VALUE);
    return;
    }


    void graphicsState::moveto(POINT_TYPE x,POINT_TYPE y) {
    pathParametersStack.top() -> moveto(x,y);
    return;
    }


    void graphicsState::moveto(GS_POINT *pPt) {
    pathParametersStack.top() -> moveto(pPt -> x,pPt -> y);
    return;
    }


    void graphicsState::rmoveto() {
    object *pY = pJob -> pop();
    object *pX = pJob -> pop();
    pathParametersStack.top() -> rmoveto(pX -> OBJECT_POINT_TYPE_VALUE,pY -> OBJECT_POINT_TYPE_VALUE);
    return;
    }


    void graphicsState::rmoveto(POINT_TYPE x,POINT_TYPE y) {
    pathParametersStack.top() -> rmoveto(x,y);
    return;
    }


    void graphicsState::rmoveto(GS_POINT pt) {
    pathParametersStack.top() -> rmoveto(pt.x,pt.y);
    return;
    }


    void graphicsState::lineto() {
    object *pY = pJob -> pop();
    object *pX = pJob -> pop();
    lineto(pX,pY);
    return;
    }


    void graphicsState::lineto(object *pX,object *pY) {
    pathParametersStack.top() -> lineto(pX -> DoubleValue(),pY -> DoubleValue());
    return;
    }


    void graphicsState::lineto(POINT_TYPE x,POINT_TYPE y) {
    pathParametersStack.top() -> lineto(x,y);
    return;
    }


    void graphicsState::lineto(GS_POINT *pPt) {
    pathParametersStack.top() -> lineto(pPt);
    return;
    }


    void graphicsState::rlineto() {
    object *pY = pJob -> pop();
    object *pX = pJob -> pop();
    rlineto(pX -> OBJECT_POINT_TYPE_VALUE,pY -> OBJECT_POINT_TYPE_VALUE);
    return;
    }


    void graphicsState::rlineto(POINT_TYPE x,POINT_TYPE y) {
    pathParametersStack.top() -> rlineto(x,y);
    return;
    }


    void graphicsState::rlineto(GS_POINT pt) {
    pathParametersStack.top() -> rlineto(pt);
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

    pathParametersStack.top() -> curveto(points);

    return;
    }


    void graphicsState::curveto(GS_POINT *pPoints) {
    pathParametersStack.top() -> curveto(pPoints);
    return;
    }


    void graphicsState::arcto(POINT_TYPE xCenter,POINT_TYPE yCenter,POINT_TYPE radius,POINT_TYPE angle1,POINT_TYPE angle2) {
    pathParametersStack.top() -> arcto(xCenter,yCenter,radius,angle1,angle2);
    return;
    }


    void graphicsState::dot(GS_POINT at,POINT_TYPE radius) {
    pathParametersStack.top() -> dot(at,radius);
    }


    void graphicsState::gdiMoveTo(POINT *pGDIPoint) {
    MoveToEx(pPStoPDF -> GetDC(),pGDIPoint -> x,pGDIPoint -> y,NULL);
    return;
    }