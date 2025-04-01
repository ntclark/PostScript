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

    long graphicsState::cyPageGutter = 32L;
    long graphicsState::pageCount = 0L;

    long graphicsState::pageHeightPoints = 792;
    long graphicsState::pageWidthPoints = 612;

    POINT_TYPE graphicsState::piOver2 = 2.0f * atan(1.0f);
    POINT_TYPE graphicsState::degToRad = piOver2 / 90.0f;

    gdiParameters graphicsState::theGDIParameters;
    pathParametersStack graphicsState::pathParametersStack;
    psTransformsStack graphicsState::psXformsStack;

    GS_POINT::GS_POINT(GS_POINT *pPointd) {
        x = pPointd -> x;
        y = pPointd -> y;
    }


    graphicsState::graphicsState(job *pj) : pJob(pj) {

    setFont(findFont("Courier"));

    if ( ! psXformsStack.isInitialized() )
        psXformsStack.initialize(pJob);

    if ( ! pathParametersStack.isInitialized() )
        pathParametersStack.initialize();

    SetSurface(pJob -> hwndSurface,0);

    return;
    }


    graphicsState::~graphicsState() {
    return;
    }


    void graphicsState::SetSurface(HWND hwndSurface,long pageNumber) {
    pathParametersStack.top() -> initialize();
    initMatrix(hwndSurface,pageNumber);
    return;
    }


    void graphicsState::restored() {
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

    pageWidthPoints = pPageSizeArray -> getElement(0) -> IntValue();
    pageHeightPoints = pPageSizeArray -> getElement(1) -> IntValue();

    // Get page number !!!
    initMatrix(pPStoPDF -> HwndClient(),1);

    return;
    }


    void graphicsState::setCacheDevice() {

    object *pUpperRightY = pJob -> pop();
    object *pUpperRightX = pJob -> pop();
    object *pLowerLeftY = pJob -> pop();
    object *pLowerLeftX = pJob -> pop();
    object *pWidthY = pJob -> pop();
    object *pWidthX = pJob -> pop();

#if 0
    fontStack.top() -> type3GlyphBoundingBox.lowerLeft.x = pLowerLeftX -> OBJECT_POINT_TYPE_VALUE;
    fontStack.top() -> type3GlyphBoundingBox.lowerLeft.y = pLowerLeftY -> OBJECT_POINT_TYPE_VALUE;
    fontStack.top() -> type3GlyphBoundingBox.upperRight.x = pUpperRightX -> OBJECT_POINT_TYPE_VALUE;
    fontStack.top() -> type3GlyphBoundingBox.upperRight.y = pUpperRightY -> OBJECT_POINT_TYPE_VALUE;
    fontStack.top() -> type3GlyphBoundingBox.advance.x = pWidthX -> OBJECT_POINT_TYPE_VALUE;
    fontStack.top() -> type3GlyphBoundingBox.advance.y = pWidthY -> OBJECT_POINT_TYPE_VALUE;
#endif
    return;
    }


    void graphicsState::setGraphicsStateDict(char *pszDictName) {
Beep(2000,200);
    return;
    }


    void graphicsState::setColorSpace(colorSpace *pcs) {
    theGDIParameters.setColorSpace(pcs);
    return;
    }


    colorSpace *graphicsState::getColorSpace() {
    return theGDIParameters.getColorSpace();
    }


    void graphicsState::setColor(colorSpace *pColorSpace) {
    return theGDIParameters.setColor(pColorSpace);
    }


    void graphicsState::setRGBColor(COLORREF rgb) {
    theGDIParameters.setRGBColor(rgb);
    return;
    }
    

    void graphicsState::setRGBColor(POINT_TYPE r,POINT_TYPE g,POINT_TYPE b) {
    theGDIParameters.setRGBColor(r,g,b);
    return;
    }


    void graphicsState::setLineWidth(POINT_TYPE lw) { 

    /*
    When stroking a path, stroke paints all points whose perpendicular 
    distance from the path in user space is less than or equal to half 
    the absolute value of num.
    (therefore, lineWidth is given in user space coordinates and it is ALSO 
    in the Y direction !)

    The effect produced in device space depends on the current transformation 
    matrix (CTM) in effect at the time the path is stroked.
    (therefore, the value must be converted to page space here because the renderer deals only
    with conversion between page space (pageWidth x pageHeight in points) TO device space)
    */

    POINTF ptLW{0.0f,lw};
    transformPointInPlace(0.0f,lw,&ptLW.x,&ptLW.y);
    theGDIParameters.setLineWidth(abs(ptLW.y));
    return;
    }


    void graphicsState::setLineJoin(long lj) {
    theGDIParameters.setLineJoin(lj);
    return;
    }


    void graphicsState::setLineCap(long lc) {
    theGDIParameters.setLineCap(lc);
    return;
    }


    void graphicsState::setLineDash(array *pArray,POINT_TYPE offset) {

    if ( NULL == pArray ) {
        theGDIParameters.setLineDash(NULL,0,offset);
        return;
    }

    long countItems = pArray -> size();

    if ( 0 == countItems ) {
        theGDIParameters.setLineDash(NULL,0,offset);
        return;
    }

    FLOAT *pValues = new FLOAT[countItems];

    for ( long k = 0; k < countItems; k++ ) {
        GS_POINT ptDash{pArray -> getElement(k) -> FloatValue(),0.0f};
        transformPoint(&ptDash,&ptDash);
        pValues[k] = ptDash.x;
    }

    theGDIParameters.setLineDash(pValues,countItems,offset);

    delete [] pValues;

    return;
    }


    void graphicsState::setStrokeAdjustmentParameter(object *pBool) {
    strokeAdjustmentParameter = pBool == pBool -> Job() -> pTrueConstant;
    return;
    }