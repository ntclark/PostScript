#include "job.h"

#include "pathParameters.h"


    void pathParameters::forwardToRenderer() {
    pIGraphicElements = job::pIGraphicElements_External;
    return;
    }


    void pathParameters::revertToGDI() {
    pIGraphicElements = pIGraphicElements_GDI;
    return;
    }


    void pathParameters::newpath() {
    pIGraphicElements -> NewPath();
    }


    void pathParameters::stroke() {
    pIGraphicElements -> StrokePath();
    return;
    }


    void pathParameters::fillpath() {
    pIGraphicElements -> FillPath();
    return;
    }

    void pathParameters::moveto(GS_POINT *pPt) {
    pIGraphicElements -> MoveTo(pPt -> x,pPt -> y);
    return;
    }


    void pathParameters::moveto(POINT_TYPE x,POINT_TYPE y) {
    pIGraphicElements -> MoveTo(x,y);
    return;
    }


    void pathParameters::rmoveto(GS_POINT *pPt) {
    pIGraphicElements -> MoveToRelative(pPt -> x,pPt -> y);
    return;
    }


    void pathParameters::rmoveto(POINT_TYPE x,POINT_TYPE y) {
    pIGraphicElements -> MoveToRelative(x,y);
    return;
    }


    void pathParameters::lineto(GS_POINT *pPt) {
    pIGraphicElements -> LineTo(pPt -> x,pPt -> y);
    return;
    }


    void pathParameters::lineto(POINT_TYPE x,POINT_TYPE y) {
    pIGraphicElements -> LineTo(x,y);
    return;
    }


    void pathParameters::rlineto(GS_POINT *pPt) {
    pIGraphicElements -> LineToRelative(pPt -> x,pPt -> y);
    return;
    }


    void pathParameters::rlineto(POINT_TYPE x,POINT_TYPE y) {
    pIGraphicElements -> LineToRelative(x,y);
    return;
    }


    void pathParameters::arcto(POINT_TYPE xCenter,POINT_TYPE yCenter,POINT_TYPE radius,POINT_TYPE angle1,POINT_TYPE angle2) {
    pIGraphicElements -> ArcTo(xCenter,yCenter,radius,angle1,angle2);
    return;
    }


    void pathParameters::curveto(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2,FLOAT x3,FLOAT y3) {
    pIGraphicElements -> CubicBezierTo(x1,y1,x2,y2,x3,y3);
    return;
    }
