
#include "job.h"

#include "pathParameters.h"
#include "gdiParameters.h"

    long graphicsState::cyPageGutter = 32L;
    long graphicsState::pageCount = 0L;
    long graphicsState::displayResolution = 0L;
    long graphicsState::cxClient = 0L;
    long graphicsState::cyClient = 0L;
    long graphicsState::cyWindow = 0L;

    long graphicsState::pageHeightPoints = 792;
    long graphicsState::pageWidthPoints = 612;
    POINT_TYPE graphicsState::scalePointsToPixels = 0.0f;

    POINT_TYPE graphicsState::piOver2 = 2.0 * atan(1.0);
    POINT_TYPE graphicsState::degToRad = piOver2 / 90.0;

    gdiParametersStack graphicsState::gdiParametersStack;
    pathParametersStack graphicsState::pathParametersStack;
    fontStack graphicsState::fontStack;

    GS_POINT::GS_POINT(GS_POINT *pPointd) {
        x = pPointd -> x;
        y = pPointd -> y;
    }


    graphicsState::graphicsState(job *pj) : pJob(pj) {
    setFont(findFont("Courier New"));
    pathParametersStack.top() -> pToUserSpace = new (pJob -> CurrentObjectHeap()) matrix(pJob);
    pathParametersStack.top() -> pUserSpaceToDeviceSpace = new (pJob -> CurrentObjectHeap()) matrix(pJob);
    return;
    }


    graphicsState::~graphicsState() {
    return;
    }


    void graphicsState::SetSurface(HWND hwndSurface,long pageNumber) {

    pathParametersStack.top() -> currentUserSpacePoint = {POINT_TYPE_NAN,POINT_TYPE_NAN};
    pathParametersStack.top() -> pathBeginPoint = {POINT_TYPE_NAN,POINT_TYPE_NAN};
    pathParametersStack.top() -> currentPointsPoint = {POINT_TYPE_NAN,POINT_TYPE_NAN};
    pathParametersStack.top() -> userSpaceDomain = {POINT_TYPE_NAN,POINT_TYPE_NAN};

    memset(&pathParametersStack.top() -> currentGDIPoint,0,sizeof(POINT));

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
    SetWorldTransform(pPStoPDF -> GetDC(),pathParameters::pTransform());
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


    void graphicsState::setColorSpace(colorSpace *pcs) {
    gdiParametersStack.top() -> setColorSpace(pcs);
    return;
    }


    colorSpace *graphicsState::getColorSpace() {
    return gdiParametersStack.top() -> getColorSpace();
    }


    void graphicsState::setColor(colorSpace *pColorSpace) {
    return gdiParametersStack.top() -> setColor(pColorSpace);
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