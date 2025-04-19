/*
Copyright 2025 EnVisioNate LLC

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the “Software”), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is the MIT License
*/

#include "job.h"

#include "pathParameters.h"


    void pathParameters::newpath() {
    PostScriptInterpreter::pIGraphicElements -> NewPath();
    }


    void pathParameters::stroke() {
    PostScriptInterpreter::pIGraphicElements -> StrokePath();
    return;
    }


    void pathParameters::fillpath() {
    PostScriptInterpreter::pIGraphicElements -> FillPath();
    return;
    }


    void pathParameters::closepath() {
    PostScriptInterpreter::pIGraphicElements -> ClosePath();
    return;
    }


    void pathParameters::eofillpath() {
PostScriptInterpreter::pIGraphicElements -> FillPath();
    return;
    }


    void pathParameters::moveto(GS_POINT *pPt) {
    PostScriptInterpreter::pIGraphicElements -> MoveTo(pPt -> x,pPt -> y);
    return;
    }


    void pathParameters::moveto(POINT_TYPE x,POINT_TYPE y) {
    PostScriptInterpreter::pIGraphicElements -> MoveTo(x,y);
    return;
    }


    void pathParameters::rmoveto(GS_POINT *pPt) {
    PostScriptInterpreter::pIGraphicElements -> MoveToRelative(pPt -> x,pPt -> y);
    return;
    }


    void pathParameters::rmoveto(POINT_TYPE x,POINT_TYPE y) {
    PostScriptInterpreter::pIGraphicElements -> MoveToRelative(x,y);
    return;
    }


    void pathParameters::lineto(GS_POINT *pPt) {
    PostScriptInterpreter::pIGraphicElements -> LineTo(pPt -> x,pPt -> y);
    return;
    }


    void pathParameters::lineto(POINT_TYPE x,POINT_TYPE y) {
    PostScriptInterpreter::pIGraphicElements -> LineTo(x,y);
    return;
    }


    void pathParameters::rlineto(GS_POINT *pPt) {
    PostScriptInterpreter::pIGraphicElements -> LineToRelative(pPt -> x,pPt -> y);
    return;
    }


    void pathParameters::rlineto(POINT_TYPE x,POINT_TYPE y) {
    PostScriptInterpreter::pIGraphicElements -> LineToRelative(x,y);
    return;
    }


    void pathParameters::arcto(POINT_TYPE xCenter,POINT_TYPE yCenter,POINT_TYPE radius,POINT_TYPE angle1,POINT_TYPE angle2) {
    PostScriptInterpreter::pIGraphicElements -> Arc(xCenter,yCenter,radius,angle1,angle2);
    return;
    }


    void pathParameters::curveto(FLOAT x0,FLOAT y0,FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2,FLOAT x3,FLOAT y3) {
    PostScriptInterpreter::pIGraphicElements -> CubicBezier(x0,y0,x1,y1,x2,y2,x3,y3);
    return;
    }


    void pathParameters::quadcurveto(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2) {
    PostScriptInterpreter::pIGraphicElements -> QuadraticBezier(x1,y1,x2,y2);
    return;
    }
