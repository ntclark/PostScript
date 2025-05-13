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

    long graphicsState::cyPageGutter = 32L;
    long graphicsState::pageCount = 0L;

    long graphicsState::pageHeightPoints = 792;
    long graphicsState::pageWidthPoints = 612;

    FLOAT graphicsState::piOver2 = 2.0f * atan(1.0f);
    FLOAT graphicsState::degToRad = piOver2 / 90.0f;

    gdiParameters graphicsState::theGDIParameters;
    pageParameters graphicsState::thePageParameters;
    psTransformsStack *graphicsState::pPSXformsStack = NULL;
    long graphicsState::instanceCount = 0;

    GS_POINT::GS_POINT(GS_POINT *pPointd) {
        x = pPointd -> x;
        y = pPointd -> y;
    }


    graphicsState::graphicsState(job *pj) : pJob(pj) {

    instanceCount++;

    if ( 1 == instanceCount )
        pPSXformsStack = new psTransformsStack();

    setFont(findFont("Courier"));

    pPSXformsStack -> initialize(pJob);

    SetSurface(pJob -> hwndSurface,0);

    return;
    }


    graphicsState::~graphicsState() {
    PSTransforms() -> Clear();
    instanceCount--;
    if ( 0 == instanceCount ) 
        delete pPSXformsStack;
    return;
    }


    void graphicsState::initialize() {
    pPSXformsStack -> initialize(pJob);
    return;
    }


    void graphicsState::SetSurface(HWND hwndSurface,long pageNumber) {
    initMatrix(hwndSurface,pageNumber);
    return;
    }


    void graphicsState::setPageDevice(class dictionary *pDict) {

    if ( NULL == pPostScriptInterpreter -> GetDC() )
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
    initMatrix(pPostScriptInterpreter -> HwndClient(),1);

    return;
    }


    void graphicsState::setCacheDevice() {
    object *pUpperRightY = pJob -> pop();
    object *pUpperRightX = pJob -> pop();
    object *pLowerLeftY = pJob -> pop();
    object *pLowerLeftX = pJob -> pop();
    object *pWidthY = pJob -> pop();
    object *pWidthX = pJob -> pop();
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
    

    void graphicsState::setRGBColor(FLOAT r,FLOAT g,FLOAT b) {
    theGDIParameters.setRGBColor(r,g,b);
    return;
    }


    void graphicsState::setLineWidth(FLOAT lw) { 
    theGDIParameters.setLineWidth(abs(lw));
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


    void graphicsState::setLineDash(array *pArray,FLOAT offset) {

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

    for ( long k = 0; k < countItems; k++ )
        pValues[k] = pArray -> getElement(k) -> FloatValue();

    theGDIParameters.setLineDash(pValues,countItems,offset);

    delete [] pValues;

    return;
    }


    void graphicsState::setStrokeAdjustmentParameter(object *pBool) {
    strokeAdjustmentParameter = (pBool == pBool -> Job() -> pTrueConstant);
    return;
    }