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
#include "pageParameters.h"

    int Mx3Inverse(double *,double *);

    FLOAT pageParameters::scalePointsToPixels = 1.0f;

    XFORM pageParameters::toDeviceSpace{1.0,0.0,0.0,1.0,0.0,0.0};
    XFORM pageParameters::toDeviceSpaceInverse{6 * 0.0};

    long pageParameters::displayResolution = 0L;
    long pageParameters::cxClient = 0L;
    long pageParameters::cyClient = 0L;

    RECT pageParameters::rcPage{-1L,-1L,-1L,-1L};
    RECT pageParameters::rcClient{-1L,-1L,-1L,-1L};

    void pageParameters::initialize() {
    memset(&toDeviceSpace,0,sizeof(XFORM));
    toDeviceSpace.eM11 = 1.0;
    toDeviceSpace.eM22 = 1.0;
    displayResolution = GetDeviceCaps(pPostScriptInterpreter -> GetDC(),LOGPIXELSX);
    scalePointsToPixels = 1.0;

#if 0
    RECT rcDrawing;
    GetClientRect(pPostScriptInterpreter -> HwndClient(),&rcDrawing);
    PostScriptInterpreter::pIRenderer -> SetRenderLive(pPostScriptInterpreter -> GetDC(),&rcDrawing);
#endif

    return;
    }


    void pageParameters::initMatrix(HWND hwndSurface,long pageNumber,long pageHeightPoints,long pageWidthPoints) {

    if ( ! ( NULL == hwndSurface ) ) {

        scalePointsToPixels = (FLOAT)(PostScriptInterpreter::ClientWindowHeight() - 
                                        2 * PostScriptInterpreter::TopGutter() ) / (FLOAT)pageHeightPoints;

        FLOAT maxWidth = (FLOAT)(PostScriptInterpreter::ClientWindowWidth() - 2 * PostScriptInterpreter::SideGutter());

        if ( maxWidth < pageWidthPoints * scalePointsToPixels ) {
            scalePointsToPixels = (FLOAT)(PostScriptInterpreter::ClientWindowWidth() - 
                                            2 * PostScriptInterpreter::SideGutter() ) / (FLOAT)pageWidthPoints;

            PostScriptInterpreter::TopGutter(((FLOAT)PostScriptInterpreter::ClientWindowHeight() - 
                                                scalePointsToPixels * (FLOAT)pageHeightPoints ) / 2.0f);

        }

        if ( -1L == rcPage.left ) {

            rcPage.left = PostScriptInterpreter::SideGutter();
            rcPage.top = PostScriptInterpreter::TopGutter();

            rcPage.right = rcPage.left + (long)(pageWidthPoints * scalePointsToPixels);
            rcPage.bottom = rcPage.top + (long)(pageHeightPoints * scalePointsToPixels);

            rcClient.left = 0;
            rcClient.top = 0;
            rcClient.right = rcPage.right - rcPage.left;
            rcClient.bottom = (rcPage.bottom - rcPage.top);

            HDC hdc = GetDC(hwndSurface);
            PostScriptInterpreter::pIRenderer -> ClearRect(hdc,&pageParameters::rcPage,RGB(255,255,255));
            ReleaseDC(hwndSurface,hdc);
        }

    }

    toDeviceSpace.eM11 = scalePointsToPixels;
    toDeviceSpace.eM12 = 0.0;
    toDeviceSpace.eM21 = 0.0;
    toDeviceSpace.eM22 = -scalePointsToPixels;
    toDeviceSpace.eDx = (FLOAT)rcPage.left;
    toDeviceSpace.eDy = (FLOAT)(PostScriptInterpreter::ClientWindowHeight() - rcPage.top);

    double inverse[3][3]{9 * 0.0};
    double theMatrix[3][3]{9 * 0.0};

    theMatrix[0][0] = (double)toDeviceSpace.eM11;
    theMatrix[0][1] = 0.0;
    theMatrix[0][2] = 0.0;

    theMatrix[1][0] = 0.0;
    theMatrix[1][1] = (double)toDeviceSpace.eM22;
    theMatrix[1][2] = 0.0;

    theMatrix[2][0] = 0.0;
    theMatrix[2][1] = (double)toDeviceSpace.eDx;
    theMatrix[2][2] = (double)toDeviceSpace.eDy;

    Mx3Inverse(&theMatrix[0][0],&inverse[0][0]);

    toDeviceSpaceInverse.eM11 = (FLOAT)inverse[0][0];
    toDeviceSpaceInverse.eM12 = (FLOAT)inverse[0][1];
    toDeviceSpaceInverse.eM21 = (FLOAT)inverse[1][0];
    toDeviceSpaceInverse.eM22 = (FLOAT)inverse[1][1];
    toDeviceSpaceInverse.eDx = (FLOAT)inverse[2][1];
    toDeviceSpaceInverse.eDy = (FLOAT)inverse[2][2];

    PostScriptInterpreter::pIRenderer -> put_ToDeviceTransform((UINT_PTR)&toDeviceSpace);

    return;
    }


    void pageParameters::RenderGeometry() {
    RECT rcDrawing;
    GetClientRect(pPostScriptInterpreter -> HwndClient(),&rcDrawing);
    PostScriptInterpreter::pIRenderer -> Render(pPostScriptInterpreter -> GetDC(),&rcDrawing);
    return;
    }