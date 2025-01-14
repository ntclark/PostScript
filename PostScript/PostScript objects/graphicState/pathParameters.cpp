
#include "job.h"

#include "pathParameters.h"

#include "GlyphRenderer_i.c"

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

    boolean pathParameters::isPathActive = false;

    pathParameters::pathParameters() {
    pIGraphicElements_Local = static_cast<IGraphicElements *>(new GraphicElements(this));
    revertToLocal();
    return;
    }


    pathParameters::pathParameters(pathParameters *pRHS) {
    pIGraphicElements_Local = static_cast<IGraphicElements *>(new GraphicElements(this));
    if ( pRHS -> pIGraphicElements == pRHS -> pIGraphicElements_Local )
        revertToLocal();
    else
       forwardToRenderer();
    return;
    }


    pathParameters::~pathParameters() {
    delete pIGraphicElements_Local;
    return;
    }


    void pathParameters::initialize() {
    closepath();
    memset(&toDeviceSpace,0,sizeof(XFORM));
    toDeviceSpace.eM11 = 1.0;
    toDeviceSpace.eM22 = 1.0;
    displayResolution = GetDeviceCaps(pPStoPDF -> GetDC(),LOGPIXELSX);
    scalePointsToPixels = 1.0;
    currentUserPoint = {POINT_TYPE_NAN,POINT_TYPE_NAN};
    currentDevicePoint = {POINT_TYPE_NAN,POINT_TYPE_NAN};
    userSpaceDomain = {POINT_TYPE_NAN,POINT_TYPE_NAN};
    currentGDIPoint.x = 0;
    currentGDIPoint.y = 0;
    return;
    }


    void pathParameters::initMatrix(HWND hwndClient,long pageNumber,long pageHeightPoints) {

    if ( ! ( NULL == hwndClient ) ) {

        RECT rcClient;
        GetClientRect(hwndClient,&rcClient);

        cxClient = (rcClient.right - rcClient.left);
        cyClient = (rcClient.bottom - rcClient.top);

        cyWindow = (long)((double)cyClient / ( 0 == pageNumber ? 1.0 : (double)pageNumber) );

        //SetWindowExtEx(pPStoPDF -> GetDC(),cxClient,cyWindow,NULL);
        //SetViewportExtEx(pPStoPDF -> GetDC(),cxClient,cyWindow,NULL);

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

/*
double v[]{0.5,0.75,1.0};
double vAnswer[]{0.0,0.0,0.0};
double prod[][3]{ {0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};

MxV(&theMatrix[0][0],v,vAnswer);
MxV(&untransformGDIMatrix[0][0],vAnswer,v);

MxM(&theMatrix[0][0],&untransformGDIMatrix[0][0],&prod[0][0]);
*/

    job::pIGlyphRenderer -> put_TransformMatrix((UINT_PTR)&toDeviceSpace);

    return;
    }