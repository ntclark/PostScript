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
            // There are two consequetive moves, update the prior move with the new position
            pCurrentPath -> pLastPrimitive -> vertex = currentPageSpacePoint;
            return S_OK;
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
    if ( NULL == pCurrentPath )
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


    HRESULT Renderer::GraphicElements::Image(HDC hdc,HBITMAP hbmImage,UINT_PTR pPSCurrentCTM,FLOAT width,FLOAT height) {

    FLOAT widthUserSpace = 1.0f;
    FLOAT heightUserSpace = 1.0f;

    XFORM *pXForm = (XFORM *)pPSCurrentCTM;

    FLOAT xResult = pXForm -> eM11 * widthUserSpace + pXForm -> eM12 * heightUserSpace;
    FLOAT yResult = pXForm -> eM21 * widthUserSpace + pXForm -> eM22 * heightUserSpace;

    widthUserSpace = toDeviceSpace.eM11 * xResult + toDeviceSpace.eM12 * yResult;
    heightUserSpace = toDeviceSpace.eM21 * xResult + toDeviceSpace.eM22 * yResult;

    xResult = pXForm -> eDx;
    yResult = pXForm -> eDy;

    FLOAT xDeviceSpace = toDeviceSpace.eM11 * xResult + toDeviceSpace.eM12 * yResult + toDeviceSpace.eDx;
    FLOAT yDeviceSpace = toDeviceSpace.eM21 * xResult + toDeviceSpace.eM22 * yResult + toDeviceSpace.eDy;

    HDC hdcSource = CreateCompatibleDC(hdc);
    SelectObject(hdcSource,hbmImage);

    StretchBlt(hdc,(long)xDeviceSpace,(long)yDeviceSpace,(long)widthUserSpace,(long)heightUserSpace,
                hdcSource,0,0,(long)width,(long)height,SRCCOPY);

    return S_OK;
    }


    HRESULT Renderer::GraphicElements::NonPostScriptImage(HDC hdc,HBITMAP hBitmap,FLOAT x0,FLOAT y0,FLOAT width,FLOAT height) {

    POINTF ptDevice{x0,y0};
    transformPoint(&ptDevice,&ptDevice);

    POINTF ptSize{width,height};
    scalePoint(&ptSize.x,&ptSize.y);

    HDC hdcSource = CreateCompatibleDC(hdc);
    SelectObject(hdcSource,hBitmap);

    long cb = GetObject(hBitmap,0,NULL);

    BYTE *pBitmap = new BYTE[cb];
    GetObject(hBitmap,cb,pBitmap);

    BITMAPINFOHEADER *pHeader = (BITMAPINFOHEADER *)pBitmap;

    SIZE sizeBM{pHeader -> biWidth,pHeader -> biHeight};

    delete [] pBitmap;

    if ( 0.0f == width || 0.0f == height ) {
        ptSize.x = (FLOAT)sizeBM.cx;
        ptSize.y = (FLOAT)sizeBM.cy;
        BitBlt(hdc,(long)ptDevice.x,(long)ptDevice.y,(long)ptSize.x,(long)ptSize.y,hdcSource,0,0,SRCCOPY);
    } else
        StretchBlt(hdc,(long)ptDevice.x,(long)ptDevice.y,(long)ptSize.x,(long)ptSize.y,hdcSource,0,0,sizeBM.cx,sizeBM.cy,SRCCOPY);

    DeleteDC(hdcSource);

    return S_OK;
    }


    void Renderer::GraphicElements::calcInverseTransform() {

    double theMatrix[3][3];
    double theMatrixInverted[3][3];

    theMatrix[0][0] = (double)toDeviceSpace.eM11;
    theMatrix[0][1] = (double)toDeviceSpace.eM12;
    theMatrix[0][2] = (double)toDeviceSpace.eDx;
    theMatrix[1][0] = (double)toDeviceSpace.eM21;
    theMatrix[1][1] = (double)toDeviceSpace.eM22;
    theMatrix[1][2] = (double)toDeviceSpace.eDy;

    theMatrix[2][0] = 0.0;
    theMatrix[2][1] = 0.0;
    theMatrix[2][2] = 1.0f;

    Mx3Inverse(&theMatrix[0][0],&theMatrixInverted[0][0]);

    toUserSpace.eM11 = (FLOAT)theMatrixInverted[0][0];
    toUserSpace.eM12 = (FLOAT)theMatrixInverted[0][1];
    toUserSpace.eM21 = (FLOAT)theMatrixInverted[1][0];
    toUserSpace.eM22 = (FLOAT)theMatrixInverted[1][1];

    toUserSpace.eDx = (FLOAT)theMatrixInverted[0][2];
    toUserSpace.eDy = (FLOAT)theMatrixInverted[1][2];

    return;
    }


    void Renderer::GraphicElements::scalePoint(FLOAT *px,FLOAT *py) {
    FLOAT xResult = toDeviceSpace.eM11 * *px + (FLOAT)fabs(toDeviceSpace.eM12) * *py;
    FLOAT yResult = toDeviceSpace.eM21 * *px + (FLOAT)fabs(toDeviceSpace.eM22) * *py;
    *px = xResult;
    *py = yResult;
    return;
    }


    void Renderer::GraphicElements::transformPoint(FLOAT *px,FLOAT *py) {
    FLOAT xResult = toDeviceSpace.eM11 * *px + toDeviceSpace.eM12 * *py + toDeviceSpace.eDx;
    FLOAT yResult = toDeviceSpace.eM21 * *px + toDeviceSpace.eM22 * *py + toDeviceSpace.eDy;
    *px = xResult;
    *py = yResult;
    return;
    }


    void Renderer::GraphicElements::unTransformPoint(FLOAT *px,FLOAT *py) {
    FLOAT xResult = toUserSpace.eM11 * *px + toUserSpace.eM12 * *py + toUserSpace.eDx;
    FLOAT yResult = toUserSpace.eM21 * *px + toUserSpace.eM22 * *py + toUserSpace.eDy;
    *px = xResult;
    *py = yResult;
    return;
    }


    void Renderer::GraphicElements::transformPoint(POINTF *ptIn,POINTF *ptOut) {
    FLOAT xResult = toDeviceSpace.eM11 * ptIn -> x + toDeviceSpace.eM12 * ptIn -> y + toDeviceSpace.eDx;
    FLOAT yResult = toDeviceSpace.eM21 * ptIn -> x + toDeviceSpace.eM22 * ptIn -> y + toDeviceSpace.eDy;
    ptOut -> x = xResult;
    ptOut -> y = yResult;
    return;
    }


    void Renderer::GraphicElements::transformPoint(D2D1_POINT_2F *ptIn,D2D1_POINT_2F *ptOut) {
    FLOAT xResult = toDeviceSpace.eM11 * ptIn -> x + toDeviceSpace.eM12 * ptIn -> y + toDeviceSpace.eDx;
    FLOAT yResult = toDeviceSpace.eM21 * ptIn -> x + toDeviceSpace.eM22 * ptIn -> y + toDeviceSpace.eDy;
    ptOut -> x = xResult;
    ptOut -> y = yResult;
    return;
    }
