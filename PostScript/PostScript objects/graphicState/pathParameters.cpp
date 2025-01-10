
#include "job.h"

#include "pathParameters.h"

#include "GlyphRenderer_i.c"

#include <d2d1.h>
#include <d2d1helper.h>
#include <wincodec.h>

    int Mx3Inverse(double *,double *);

    long pathParameters::displayResolution = 0L;
    long pathParameters::cxClient = 0L;
    long pathParameters::cyClient = 0L;
    long pathParameters::cyWindow = 0L;

    boolean pathParameters::isPathActive = false;

    pathParameters::pathParameters() {

    HRESULT rc = S_OK;

    if ( pIGraphicElements_GDI )
        pIGraphicElements_GDI -> Release();

    pIGraphicElements_GDI = static_cast<IGraphicElements *>(new GraphicElements(this));

    revertToGDI();

    return;
    }


    pathParameters::pathParameters(pathParameters &rhs) {

    memcpy(toDeviceSpace,rhs.toDeviceSpace,sizeof(toDeviceSpace));
    memcpy(toDeviceSpaceInverse,rhs.toDeviceSpaceInverse,sizeof(toDeviceSpaceInverse));
    scalePointsToPixels = rhs.scalePointsToPixels;
    renderingHeight = rhs.renderingHeight;

    pIGraphicElements_GDI = rhs.pIGraphicElements_GDI;

    pIGraphicElements = rhs.pIGraphicElements;

    return;
    }


    pathParameters::~pathParameters() {
    if ( pIGraphicElements_GDI )
        pIGraphicElements_GDI -> Release();
    return;
    }


    void pathParameters::initialize() {
    closepath();
    memset(toDeviceSpace,0,sizeof(toDeviceSpace));
    toDeviceSpace[A] = 1.0;
    toDeviceSpace[D] = 1.0;
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
        scalePointsToPixels = (POINT_TYPE)cyWindow / (POINT_TYPE)pageHeightPoints;

    }

    renderingHeight = (POINT_TYPE)cyWindow;

    toDeviceSpace[A] = scalePointsToPixels;
    toDeviceSpace[B] = 0.0;
    toDeviceSpace[C] = 0.0;
    toDeviceSpace[D] = -scalePointsToPixels;
    toDeviceSpace[TX] = 0.0;
    toDeviceSpace[TY] = (POINT_TYPE)cyWindow;

    double inverse[3][3]{9 * 0.0};
    double theMatrix[3][3]{9 * 0.0};

    theMatrix[0][0] = (double)toDeviceSpace[A];
    theMatrix[0][1] = 0.0;
    theMatrix[0][2] = 0.0;

    theMatrix[1][0] = 0.0;
    theMatrix[1][1] = (double)toDeviceSpace[D];
    theMatrix[1][2] = 0.0;

    theMatrix[2][0] = 0.0;
    theMatrix[2][1] = (double)toDeviceSpace[TX];
    theMatrix[2][2] = (double)toDeviceSpace[TY];

    Mx3Inverse(&theMatrix[0][0],&inverse[0][0]);

    toDeviceSpaceInverse[A] = (POINT_TYPE)inverse[0][0];
    toDeviceSpaceInverse[B] = (POINT_TYPE)inverse[0][1];
    toDeviceSpaceInverse[C] = (POINT_TYPE)inverse[1][0];
    toDeviceSpaceInverse[D] = (POINT_TYPE)inverse[1][1];
    toDeviceSpaceInverse[TX] = (POINT_TYPE)inverse[2][1];
    toDeviceSpaceInverse[TY] = (POINT_TYPE)inverse[2][2];

/*
double v[]{0.5,0.75,1.0};
double vAnswer[]{0.0,0.0,0.0};
double prod[][3]{ {0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};

MxV(&theMatrix[0][0],v,vAnswer);
MxV(&untransformGDIMatrix[0][0],vAnswer,v);

MxM(&theMatrix[0][0],&untransformGDIMatrix[0][0],&prod[0][0]);
*/

    job::pIGlyphRenderer -> put_TransformMatrix((UINT_PTR)toDeviceSpace);

    return;
    }