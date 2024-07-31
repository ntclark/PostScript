
#include "PostScript objects/graphicsState.h"
#include "PostScript objects/font.h"

#include <math.h>
#include "utilities.h"

    long graphicsState::cyPageGutter = 32L;
    long graphicsState::pageCount = 0L;
    long graphicsState::displayResolution = 0L;
    long graphicsState::cxClient = 0L;
    long graphicsState::cyClient = 0L;

    long graphicsState::pageHeightPoints = 792;
    long graphicsState::pageWidthPoints = 612;
    POINT_TYPE graphicsState::scalePointsToPixels = 0.0f;

    POINT_TYPE graphicsState::piOver2 = 2.0 * atan(1.0);
    POINT_TYPE graphicsState::degToRad = piOver2 / 90.0;

    matrix *graphicsState::pUserSpaceToDeviceSpace = NULL;

    gdiParametersStack graphicsState::gdiParametersStack;

    GS_POINT::GS_POINT(GS_POINT *pPointd) {
        x = pPointd -> x;
        y = pPointd -> y;
    }


    graphicsState::graphicsState(job *pj,HWND hwndSur) :
        pJob(pj)
    {

    pToUserSpace = new (pJob -> CurrentObjectHeap()) matrix(pJob);

    memset(&currentUserSpacePoint,0,sizeof(GS_POINT));

    pathBeginPoint.x = POINT_TYPE_NAN;
    pathBeginPoint.y = POINT_TYPE_NAN;

    memset(&currentPointsPoint,0,sizeof(GS_POINT));
    memset(&currentGDIPoint,0,sizeof(POINT));
    memset(&userSpaceDomain,0,sizeof(GS_POINT));

    pUserSpaceToDeviceSpace = new (pJob -> CurrentObjectHeap()) matrix(pJob);

    return;
    }


    // I don't think I can use an actual copy constructor with positional new
    // Probably for the same or similar reason as mentioned in this object
    // constructor.

    void graphicsState::copyFrom(graphicsState *pOther) {

    pUserSpaceToDeviceSpace -> copyFrom(pOther -> pUserSpaceToDeviceSpace);

    pToUserSpace -> copyFrom(pOther -> pToUserSpace);

    currentUserSpacePoint = pOther -> currentUserSpacePoint;
    pathBeginPoint = pOther -> pathBeginPoint;
    currentPointsPoint = pOther -> currentPointsPoint;

    gdiParametersStack.top() -> lineCap = pOther -> gdiParametersStack.top() -> lineCap;
    gdiParametersStack.top() -> lineJoin = pOther -> gdiParametersStack.top() -> lineJoin;

    pageHeightPoints = pOther -> pageHeightPoints;
    pageWidthPoints = pOther -> pageWidthPoints;

    scalePointsToPixels = pOther -> scalePointsToPixels;

    return;
    }


    graphicsState::~graphicsState() {
    return;
    }


    void graphicsState::SetSurface(HWND hwndSurface,long pageNumber) {

    SetGraphicsMode(pPStoPDF -> GetDC(),GM_ADVANCED);
    SetMapMode(pPStoPDF -> GetDC(),MM_ANISOTROPIC);
    SetPolyFillMode(pPStoPDF -> GetDC(),ALTERNATE);
    SetArcDirection(pPStoPDF -> GetDC(),AD_COUNTERCLOCKWISE);

    displayResolution = GetDeviceCaps(pPStoPDF -> GetDC(),LOGPIXELSX);

    initMatrix(hwndSurface,pageNumber);

    gdiParametersStack.setupDC();

    return;
    }


    void graphicsState::restored() {
    SetWorldTransform(pPStoPDF -> GetDC(),pTransform());
    return;
    }


    void graphicsState::setPageDevice(class dictionary *pDict) {

    if ( NULL == pPStoPDF -> GetDC() )
        return;

    class array *pPageSizeArray = NULL;

    pJob -> push(pDict);

    pJob -> operatorBegin();

    pJob -> push(pJob -> pPageSizeLiteral);

    pJob -> operatorWhere();

    pJob -> operatorEnd();

    if ( pJob -> pFalseConstant == pJob -> pop() ) 
        return;

    pJob -> push(pJob -> pPageSizeLiteral);

    pJob -> operatorGet();

    pPageSizeArray = reinterpret_cast<array *>(pJob -> pop());

Beep(2000,200);
return;

    pageWidthPoints = pPageSizeArray -> getElement(0) -> IntValue();
    pageHeightPoints = pPageSizeArray -> getElement(1) -> IntValue();

    // Get page number !!!
    initMatrix(pPStoPDF -> HwndClient(),1);

    return;
    }


    void graphicsState::setGraphicsStateDict(char *pszDictName) {
Beep(2000,200);
    return;
    }


    void graphicsState::setRGBColor(COLORREF rgb) {
    gdiParametersStack.top() -> setRGBColor(rgb);
    return;
    }
    

    void graphicsState::setRGBColor(POINT_TYPE r,POINT_TYPE g,POINT_TYPE b) {
    gdiParametersStack.top() -> setRGBColor(r,g,b);
    return;
    }


    void graphicsState::setLineWidth(POINT_TYPE lw) { 
    gdiParametersStack.top() -> setLineWidth(lw);
    return;
    }


    void graphicsState::setLineJoin(long lj) {
    gdiParametersStack.top() -> setLineJoin(lj);
    return;
    }


    void graphicsState::setLineCap(long lc) {
    gdiParametersStack.top() -> setLineCap(lc);
    return;
    }


    void graphicsState::setLineDash(class array *pArray,POINT_TYPE offset) {
    gdiParametersStack.top() -> setLineDash(pArray,offset);
    return;
    }