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

    void pageParameters::initialize() {
    memset(&toDeviceSpace,0,sizeof(XFORM));
    toDeviceSpace.eM11 = 1.0;
    toDeviceSpace.eM22 = 1.0;
    displayResolution = GetDeviceCaps(pPostScriptInterpreter -> GetDC(),LOGPIXELSX);
    scalePointsToPixels = 1.0;
    return;
    }


    void pageParameters::initMatrix(HWND hwndSurface,long pageNumber,long pageHeightPoints,long pageWidthPoints) {

    if ( ! ( NULL == hwndSurface ) ) {

        scalePointsToPixels = (FLOAT)(PostScriptInterpreter::ClientHeight() - 
                                        2 * PostScriptInterpreter::TopGutter() ) / (FLOAT)pageHeightPoints;

        long cxCurrent = 2 * PostScriptInterpreter::SideGutter() + 
                                    (long)(pageWidthPoints * scalePointsToPixels);

        long cxMargin = PostScriptInterpreter::ClientWidth() - (long)((scalePointsToPixels * (FLOAT)pageWidthPoints) / 2.0f);

        if ( cxCurrent > PostScriptInterpreter::ClientWidth() || cxMargin > 2 * NOMINAL_GUTTER ) {
            long cxDelta = cxCurrent - PostScriptInterpreter::ClientWidth() + 2 * NOMINAL_GUTTER;
            scalePointsToPixels = (FLOAT)(cxCurrent - cxDelta) / (FLOAT)pageWidthPoints;
            cxMargin = PostScriptInterpreter::ClientWidth() - (long)(scalePointsToPixels * (FLOAT)pageWidthPoints);
            PostScriptInterpreter::SideGutter(cxMargin / 2);
            PostScriptInterpreter::TopGutter((long)(((FLOAT)PostScriptInterpreter::ClientHeight() - 
                                                    scalePointsToPixels * (FLOAT)pageHeightPoints ) / 2.0f));
            if ( 0 > PostScriptInterpreter::TopGutter() ) {
                scalePointsToPixels = (FLOAT)(PostScriptInterpreter::ClientHeight() - 2 * NOMINAL_GUTTER) / (FLOAT)pageHeightPoints;
                PostScriptInterpreter::TopGutter(NOMINAL_GUTTER);
                cxMargin = PostScriptInterpreter::ClientWidth() - (long)(scalePointsToPixels * (FLOAT)pageWidthPoints);
                PostScriptInterpreter::SideGutter(cxMargin / 2);
            }

        }

        RECT rcClient{0,0,PostScriptInterpreter::ClientWidth() - GetSystemMetrics(SM_CXVSCROLL),PostScriptInterpreter::ClientHeight()};
        HDC hdc = GetDC(hwndSurface);
        FillRect(hdc,&rcClient,(HBRUSH)(COLOR_APPWORKSPACE + 1));
        RECT rcPage{PostScriptInterpreter::SideGutter(),PostScriptInterpreter::TopGutter(),0,0};
        rcPage.right = rcPage.left + (long)(scalePointsToPixels * (FLOAT)pageWidthPoints);
        rcPage.bottom = rcPage.top + (long)(scalePointsToPixels * (FLOAT)pageHeightPoints);
        PostScriptInterpreter::pIRenderer -> ClearRect(hdc,&rcPage,RGB(255,255,255));
        ReleaseDC(hwndSurface,hdc);

    }

    toDeviceSpace.eM11 = scalePointsToPixels;
    toDeviceSpace.eM12 = 0.0;
    toDeviceSpace.eM21 = 0.0;
    toDeviceSpace.eM22 = -scalePointsToPixels;
    toDeviceSpace.eDx = (FLOAT)PostScriptInterpreter::SideGutter();
    toDeviceSpace.eDy = (FLOAT)(PostScriptInterpreter::ClientHeight() - PostScriptInterpreter::TopGutter());

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