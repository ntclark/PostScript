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

#include "Renderer.h"

    HRESULT Renderer::GraphicElements::QueryInterface(REFIID refIID,void **pvResult) {

    if ( ! pvResult )
        return E_POINTER;

    *pvResult = NULL;

    if ( IID_IUnknown == refIID ) 
        *pvResult = static_cast<IUnknown *>(this);

    else if ( IID_IGraphicElements == refIID )
        *pvResult = static_cast<GraphicElements *>(this);

    if ( * pvResult ) {
        AddRef();
        return S_OK;
    }

    return pParent -> QueryInterface(refIID,pvResult);
    }


    HRESULT Renderer::GraphicElements::MoveTo(FLOAT x,FLOAT y) {
/*
    moveto 
        x y moveto –

    starts a new subpath of the current path (see Section 4.4, “Path Construction”) by
    setting the current point in the graphics state to the coordinates (x, y) in user
    space. No new line segments are added to the current path.
    If the previous path operation in the current path was moveto or rmoveto, that
    point is deleted from the current path and the new moveto point replaces it.
*/

    currentPageSpacePoint = {x,y};

    if ( NULL == pCurrentPath )
        NewPath();

    if ( ! ( NULL == pCurrentPath -> pLastPrimitive ) ) {
        if ( primitive::type::move == pCurrentPath -> pLastPrimitive -> theType ) {
            // There are two consequetive moves, delete the existing one
            primitive *pPrior = pCurrentPath -> pLastPrimitive -> pPriorPrimitive;
            if ( ! ( NULL == pPrior ) ) 
                pPrior -> pNextPrimitive = NULL;
            delete pCurrentPath -> pLastPrimitive;
            pCurrentPath -> pLastPrimitive = pPrior;
        }
    } 

    pCurrentPath -> addPrimitive(new movePrimitive(this,&currentPageSpacePoint));

    return S_OK;
    }


    HRESULT Renderer::GraphicElements::MoveToRelative(FLOAT x,FLOAT y) {
    return MoveTo(currentPageSpacePoint.x + x,currentPageSpacePoint.y + y);
    }


    HRESULT Renderer::GraphicElements::LineTo(FLOAT x,FLOAT y) {
    currentPageSpacePoint = {x,y};
    if ( NULL == pCurrentPath )
        NewPath();
    pCurrentPath -> addPrimitive(new linePrimitive(this,&currentPageSpacePoint));
    return S_OK;
    }


    HRESULT Renderer::GraphicElements::LineToRelative(FLOAT x,FLOAT y) {
    return LineTo(currentPageSpacePoint.x + x,currentPageSpacePoint.y + y);
    }


    HRESULT Renderer::GraphicElements::Arc(FLOAT xCenter,FLOAT yCenter,FLOAT radius,FLOAT startAngle,FLOAT endAngle) {
    FLOAT xStart = xCenter + radius * (FLOAT)cos(startAngle * degToRad);
    FLOAT yStart = yCenter + radius * (FLOAT)sin(startAngle * degToRad);
    FLOAT xEnd = xCenter + radius * (FLOAT)cos(endAngle * degToRad);
    FLOAT yEnd = yCenter + radius * (FLOAT)sin(endAngle * degToRad);
    currentPageSpacePoint = {xEnd,yEnd};
    D2D1_ARC_SEGMENT theSegment{{xEnd,yEnd},{radius,radius},startAngle,D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,D2D1_ARC_SIZE_SMALL};
    if ( NULL == pCurrentPath )
        NewPath();
    MoveTo(xStart,yStart);
    pCurrentPath -> addPrimitive(new arcPrimitive(this,&theSegment));
    return S_OK;
    }


    HRESULT Renderer::GraphicElements::Ellipse(FLOAT xCenter,FLOAT yCenter,FLOAT xRadius,FLOAT yRadius) {
    D2D1_ELLIPSE theSegment{{xCenter,yCenter},xRadius,yRadius};
    //if ( NULL == pCurrentPath )
        NewPath();
    pCurrentPath -> addPrimitive(new ellipsePrimitive(this,&theSegment));
    return S_OK;
    }


    HRESULT Renderer::GraphicElements::Circle(FLOAT xCenter,FLOAT yCenter,FLOAT radius) {
    Arc(xCenter,yCenter,radius,0.0f,180.0f);
    Arc(xCenter,yCenter,radius,180.0f,360.0f);
    return S_OK;
    }


    HRESULT Renderer::GraphicElements::CubicBezier(FLOAT x0,FLOAT y0,FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2,FLOAT x3,FLOAT y3) {
    currentPageSpacePoint = {x3,y3};
    D2D1_BEZIER_SEGMENT theSegment{{x1,y1},{x2,y2},{x3,y3}};
    POINTF movePoint{x0,y0};
    if ( NULL == pCurrentPath )
        NewPath();
    pCurrentPath -> addPrimitive(new bezierPrimitive(this,&movePoint,&theSegment));
    return S_OK;
    }


    HRESULT Renderer::GraphicElements::QuadraticBezier(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2) {
    POINTF priorCurrentPoint = currentPageSpacePoint;
    currentPageSpacePoint = {x2,y2};
    D2D1_QUADRATIC_BEZIER_SEGMENT theSegment{{x1,y1},{x2,y2}};
    if ( NULL == pCurrentPath )
        NewPath();
    pCurrentPath -> addPrimitive(new quadraticBezierPrimitive(this,&priorCurrentPoint,&theSegment));
    return E_NOTIMPL;
    }


    HRESULT Renderer::GraphicElements::GetCurrentPoint(FLOAT *pX,FLOAT *pY) {
    if ( NULL == pX || NULL == pY )
        return E_POINTER;
    *pX = currentPageSpacePoint.x;
    *pY = currentPageSpacePoint.y;
    return S_OK;
    }


    HRESULT Renderer::GraphicElements::SetCurrentPoint(FLOAT *pX,FLOAT *pY) {
    if ( NULL == pX || NULL == pY )
        return E_INVALIDARG;
    currentPageSpacePoint.x = *pX;
    currentPageSpacePoint.y = *pY;
    return S_OK;
    }