
#include "PostScript objects/graphicsState.h"

    void graphicsState::moveto() {

    if ( NULL == hdcSurface ) 
        return;

    object *pY = pJob -> pop();
    object *pX = pJob -> pop();

    moveto(pX -> IntValue(),pY -> IntValue());

    return;
    }


    void graphicsState::moveto(POINT_TYPE x,POINT_TYPE y) {
    moveto((long)x,(long)y);
    return;
    }


    void graphicsState::moveto(long x,long y) {

    lastPoint.x = x;
    lastPoint.y = y;

    if ( ! ( NULL == hdcSurface ) )
        MoveToEx(hdcSurface,lastPoint.x,lastPoint.y,NULL);

    return;
    }


    void graphicsState::moveto(POINTL ptl) {
    moveto(ptl.x,ptl.y);
    return;
    }


    void graphicsState::moveto(POINTD *pPointd) {
    moveto(pPointd -> X(),pPointd -> Y());
    return;
    }


    void graphicsState::rmoveto() {

    object *pY = pJob -> pop();
    object *pX = pJob -> pop();

    rmoveto(pX -> IntValue(),pY -> IntValue());

    return;
    }


    void graphicsState::rmoveto(POINT_TYPE x,POINT_TYPE y) {
    rmoveto((long)x,(long)y);
    return;
    }


    void graphicsState::rmoveto(long x,long y) {

    lastPoint.x += x;
    lastPoint.y += y;

    if ( ! ( NULL == hdcSurface ) )
        MoveToEx(hdcSurface,lastPoint.x,lastPoint.y,NULL);

    return;
    }


    void graphicsState::rmoveto(POINTL ptl) {
    moveto(ptl.x,ptl.y);
    return;
    }


    void graphicsState::lineto() {
    object *pY = pJob -> pop();
    object *pX = pJob -> pop();
    lineto(pX -> IntValue(),pY -> IntValue());
    return;
    }


    void graphicsState::lineto(POINT_TYPE x,POINT_TYPE y) {
    lineto((long)x,(long)y);
    return;
    }


    void graphicsState::lineto(long x,long y) {

    lastPoint.x = x;
    lastPoint.y = y;

    if ( ! ( NULL == hdcSurface ) )
        LineTo(hdcSurface,lastPoint.x,lastPoint.y);

    return;
    }


    void graphicsState::lineto(POINTL ptl) {
    lineto(ptl.x,ptl.y);
    return;
    }


    void graphicsState::lineto(POINTD *pPointd) {
    lineto(pPointd -> X(),pPointd -> Y());
    return;
    }


    void graphicsState::rlineto() {
    object *pY = pJob -> pop();
    object *pX = pJob -> pop();
    if ( ! ( NULL == hdcSurface ) )
        lineto(lastPoint.x + pX -> IntValue(),lastPoint.y + pY -> IntValue());
    return;
    }


    void graphicsState::rlineto(POINT_TYPE x,POINT_TYPE y) {
    rlineto((long)x,(long)y);
    return;
    }


    void graphicsState::rlineto(long x,long y) {
    lastPoint.x += x;
    lastPoint.y += y;
    LineTo(hdcSurface,lastPoint.x,lastPoint.y);
    return;
    }


    void graphicsState::rlineto(POINTL ptl) {
    rlineto(ptl.x,ptl.y);
    return;
    }

    void graphicsState::closepath() {
    //if ( ! ( NULL == hdcSurface ) )
    //    LineTo(hdcSurface,lastMove.x,lastMove.y);
Beep(2000,200);
    return;
    }


