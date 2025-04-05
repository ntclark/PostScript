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

#include <d2d1.h>
#include <d2d1helper.h>
#include <wincodec.h>

    int Mx3Inverse(double *,double *);

    FLOAT pathParameters::scalePointsToPixels = 1.0f;
    FLOAT pathParameters::renderingHeight = 0.0f;

    XFORM pathParameters::toDeviceSpace{1.0,0.0,0.0,1.0,0.0,0.0};
    XFORM pathParameters::toDeviceSpaceInverse{6 * 0.0};

    long pathParameters::displayResolution = 0L;
    long pathParameters::cxClient = 0L;
    long pathParameters::cyClient = 0L;
    long pathParameters::cyWindow = 0L;


    pathParameters::pathParameters(pathParameters *pRHS) {
    return;
    }


    pathParameters::~pathParameters() {
    return;
    }


    void pathParameters::initialize() {
    memset(&toDeviceSpace,0,sizeof(XFORM));
    toDeviceSpace.eM11 = 1.0;
    toDeviceSpace.eM22 = 1.0;
    displayResolution = GetDeviceCaps(pPostScriptInterpreter -> GetDC(),LOGPIXELSX);
    scalePointsToPixels = 1.0;

#if 0
    RECT rcDrawing;
    GetClientRect(pPostScriptInterpreter -> HwndClient(),&rcDrawing);
    job::pIRenderer -> SetRenderLive(pPostScriptInterpreter -> GetDC(),&rcDrawing);
#endif

    return;
    }


    void pathParameters::initMatrix(HWND hwndClient,long pageNumber,long pageHeightPoints) {

    if ( ! ( NULL == hwndClient ) ) {

        RECT rcClient;
        GetClientRect(hwndClient,&rcClient);

        cxClient = (rcClient.right - rcClient.left);
        cyClient = (rcClient.bottom - rcClient.top);

        cyWindow = (long)((double)cyClient / ( 0 == pageNumber ? 1.0 : (double)pageNumber) );

        //SetWindowExtEx(pPostScriptInterpreter -> GetDC(),cxClient,cyWindow,NULL);
        //SetViewportExtEx(pPostScriptInterpreter -> GetDC(),cxClient,cyWindow,NULL);

        /*
        Note: At this point, pageHeightPoints was not set by interpreting PostScript,
        it is a static value and PS is not being parsed yet. Move this portion
        of transform setup until after PS specifies the page dimensions
        */
        scalePointsToPixels = (FLOAT)cyWindow / (FLOAT)pageHeightPoints;

    }

    renderingHeight = (FLOAT)cyWindow;

    toDeviceSpace.eM11 = scalePointsToPixels;
    toDeviceSpace.eM12 = 0.0;
    toDeviceSpace.eM21 = 0.0;
    toDeviceSpace.eM22 = -scalePointsToPixels;
    toDeviceSpace.eDx = 0.0;
    toDeviceSpace.eDy = (FLOAT)cyWindow;

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

    PostScriptInterpreter::pIRenderer -> put_TransformMatrix((UINT_PTR)&toDeviceSpace);

    return;
    }


    void pathParameters::RenderGeometry() {
    RECT rcDrawing;
    GetClientRect(pPostScriptInterpreter -> HwndClient(),&rcDrawing);
    PostScriptInterpreter::pIRenderer -> Render(pPostScriptInterpreter -> GetDC(),&rcDrawing);
    return;
    }