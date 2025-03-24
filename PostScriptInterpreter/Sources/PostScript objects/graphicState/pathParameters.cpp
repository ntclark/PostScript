
#include "job.h"

#include "pathParameters.h"

#include <d2d1.h>
#include <d2d1helper.h>
#include <wincodec.h>

    int Mx3Inverse(double *,double *);

    IRenderer *pathParameters::pIRenderer_text = NULL;
    IGraphicElements *pathParameters::pIGraphicElements_text = NULL;

    FLOAT pathParameters::scalePointsToPixels = 1.0f;
    FLOAT pathParameters::renderingHeight = 0.0f;

    XFORM pathParameters::toDeviceSpace{1.0,0.0,0.0,1.0,0.0,0.0};
    XFORM pathParameters::toDeviceSpaceInverse{6 * 0.0};

    long pathParameters::displayResolution = 0L;
    long pathParameters::cxClient = 0L;
    long pathParameters::cyClient = 0L;
    long pathParameters::cyWindow = 0L;


    pathParameters::pathParameters() {
    pIGraphicElements = job::pIGraphicElements_External;
    if ( NULL == pIRenderer_text ) {
        font::pIFontManager -> QueryInterface(IID_IRenderer,reinterpret_cast<void **>(&pIRenderer_text));
        font::pIFontManager -> QueryInterface(IID_IGraphicElements,reinterpret_cast<void **>(&pIGraphicElements_text));
    }
    return;
    }


    pathParameters::pathParameters(pathParameters *pRHS) {
    pIGraphicElements = job::pIGraphicElements_External;
    return;
    }


    pathParameters::~pathParameters() {
    return;
    }


    void pathParameters::initialize() {
    memset(&toDeviceSpace,0,sizeof(XFORM));
    toDeviceSpace.eM11 = 1.0;
    toDeviceSpace.eM22 = 1.0;
    displayResolution = GetDeviceCaps(pPStoPDF -> GetDC(),LOGPIXELSX);
    scalePointsToPixels = 1.0;
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

    job::pIRenderer -> put_TransformMatrix((UINT_PTR)&toDeviceSpace);
    pIRenderer_text -> put_TransformMatrix((UINT_PTR)&toDeviceSpace);

    return;
    }


    void pathParameters::RenderGeometry() {
    job::pIRenderer -> Render(pPStoPDF -> GetDC());
    pIRenderer_text -> Render(pPStoPDF -> GetDC());

    //job::pIRenderer -> Reset();
    //pIRenderer_text -> Reset();

    return;
    }