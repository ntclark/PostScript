
#include "PostScript objects/graphicsState.h"
#include <vector>

static long pathDepth = 0L;

    void graphicsState::newPath() {
    memset(&lastUserSpacePoint,0xFF,sizeof(GS_POINT));
    memset(&lastUserSpaceMovedToPoint,0xFF,sizeof(GS_POINT));
    memset(&lastPointsPoint,0xFF,sizeof(GS_POINT));
    if ( 0 == pathDepth ) {
        BeginPath(activeDC());
        pathDepth = 1L;
    }
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


    static boolean dontRecurse = false;


    void graphicsState::moveto(POINT_TYPE x,POINT_TYPE y) {

    if ( ! ( NULL == hdcAlternate ) ) {
        MoveToEx(hdcAlternate,std::lround(scaleGlyphSpacetoPixels * x),std::lround(scaleGlyphSpacetoPixels * y),NULL);
char szX[128];
sprintf_s<128>(szX,"move glyph %6ld, %6ld\n",std::lround(scaleGlyphSpacetoPixels * x),std::lround(scaleGlyphSpacetoPixels * y));
OutputDebugStringA(szX);
        return;
    }

    lastUserSpacePoint.x = x;
    lastUserSpacePoint.y = y;
    if ( isnan(lastUserSpacePoint.x) )
        lastUserSpaceMovedToPoint = lastUserSpacePoint;

    POINT_TYPE ptx2;
    POINT_TYPE pty2;
    transformPoint(x,y,&ptx2,&pty2);
    lastPointsPoint.x = ptx2;
    lastPointsPoint.y = pty2;
    if ( ! ( NULL == hdcSurface ) ) 
        MoveToEx(hdcSurface,std::lround(PICA_FACTOR * lastPointsPoint.x),std::lround(PICA_FACTOR * lastPointsPoint.y),NULL);

char szX[128];
sprintf_s<128>(szX,"move %6ld, %6ld\n",std::lround(PICA_FACTOR * lastPointsPoint.x),std::lround(PICA_FACTOR * lastPointsPoint.y));
OutputDebugStringA(szX);

    if ( ! pPStoPDF -> drawGlyphRenderingPoints() ) 
        return;

    if ( dontRecurse )
        return;

    dontRecurse = true;

    MoveToEx(hdcSurface,(long)lastPointsPoint.x + 2,(long)lastPointsPoint.y + 2,NULL);

    LineTo(hdcSurface,(long)lastPointsPoint.x - 2,(long)lastPointsPoint.y + 2);
    LineTo(hdcSurface,(long)lastPointsPoint.x - 2,(long)lastPointsPoint.y - 2);
    LineTo(hdcSurface,(long)lastPointsPoint.x + 2,(long)lastPointsPoint.y - 2);
    LineTo(hdcSurface,(long)lastPointsPoint.x + 2,(long)lastPointsPoint.y + 2);

    MoveToEx(hdcSurface,(long)lastPointsPoint.x,(long)lastPointsPoint.y,NULL);

    dontRecurse = false;

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
    moveto(lastUserSpacePoint.x + x,lastUserSpacePoint.y + y);
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
    if ( ! ( NULL == hdcAlternate ) ) {
        LineTo(hdcAlternate,std::lround(scaleGlyphSpacetoPixels * x),std::lround(scaleGlyphSpacetoPixels * y));
char szX[128];
sprintf_s<128>(szX,"line glyph %6ld, %6ld\n",std::lround(scaleGlyphSpacetoPixels * x),std::lround(scaleGlyphSpacetoPixels * y));
OutputDebugStringA(szX);
        return;
    }
    lastUserSpacePoint.x = x;
    lastUserSpacePoint.y = y;
    POINT_TYPE ptx2;
    POINT_TYPE pty2;
    transformPoint(x,y,&ptx2,&pty2);
    lastPointsPoint.x = ptx2;
    lastPointsPoint.y = pty2;
    if ( ! ( NULL == hdcSurface ) ) 
        LineTo(hdcSurface,std::lround(PICA_FACTOR * lastPointsPoint.x),std::lround(PICA_FACTOR * lastPointsPoint.y));

char szX[128];
sprintf_s<128>(szX,"line %6ld, %6ld\n",std::lround(PICA_FACTOR * lastPointsPoint.x),std::lround(PICA_FACTOR * lastPointsPoint.y));
OutputDebugStringA(szX);

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
    lineto(lastUserSpacePoint.x + x,lastUserSpacePoint.y + y);
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

    POINT_TYPE ptx2 = pX1 -> DoubleValue();
    POINT_TYPE pty2 = pY1 -> DoubleValue();
    POINT_TYPE ptx3 = pX2 -> DoubleValue();
    POINT_TYPE pty3 = pY2 -> DoubleValue();
    POINT_TYPE ptx4 = pX3 -> DoubleValue();
    POINT_TYPE pty4 = pY3 -> DoubleValue();

    transformPoint(ptx2,pty2,&ptx2,&pty2);
    transformPoint(ptx3,pty3,&ptx3,&pty3);
    transformPoint(ptx4,pty4,&ptx4,&pty4);

    POINT pts[3];

    POINT_TYPE factor = PICA_FACTOR;
    if ( ! ( NULL == hdcAlternate ) )
        factor = scaleGlyphSpacetoPixels;

    pts[0].x = std::lround(factor * ptx2);
    pts[0].y = std::lround(factor * pty2);
    pts[1].x = std::lround(factor * ptx3);
    pts[1].y = std::lround(factor * pty3);
    pts[2].x = std::lround(factor * ptx4);
    pts[2].y = std::lround(factor * pty4);

    PolyBezierTo(activeDC(),pts,3);

    return;
    }


    void graphicsState::closepath() {

if ( 1L == pathDepth ) {
    EndPath(activeDC());
    StrokePath(activeDC());
    pathDepth = 0L;
}
//    lineto(lastUserSpaceMovedToPoint.x,lastUserSpaceMovedToPoint.y);
    return;
    }


    void graphicsState::fillpath() {
//    lineto(lastUserSpaceMovedToPoint.x,lastUserSpaceMovedToPoint.y);
if ( 1L == pathDepth ) {
    EndPath(activeDC());
    //StrokePath(hdcSurface);
    FillPath(activeDC());
    pathDepth = 0L;
}
    return;
    }