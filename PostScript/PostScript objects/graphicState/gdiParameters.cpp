
#include "job.h"

#include "gdiParameters.h"

    POINT_TYPE gdiParameters::defaultLineWidth = 1.0f;
    long gdiParameters::defaultLineCap = 0;
    long gdiParameters::defaultLineJoin = 0;
    COLORREF gdiParameters::defaultRGBColor = RGB(0,0,0);


    gdiParameters::gdiParameters() {
#ifdef USE_RENDERER
    pIGraphicParameters = job::pIGraphicParameters_External;
#else
    pIGraphicParameters_Local = new GraphicParameters(this);
    revertToGDI();
#endif
    return;
    }


    gdiParameters::gdiParameters(gdiParameters *pRHS) {

    lineWidth = pRHS -> lineWidth;
    lineCap = pRHS -> lineCap;
    lineJoin = pRHS -> lineJoin;
    rgbColor = pRHS -> rgbColor;

    if ( ! ( NULL == pRHS -> pLineStyles ) ) {
        pLineStyles = new DWORD[pRHS -> countDashSizes];
        memcpy(pLineStyles,pRHS -> pLineStyles,pRHS -> countDashSizes * sizeof(DWORD));
    }

    pColorSpace = pRHS -> pColorSpace;

#ifdef USE_RENDERER
    pIGraphicParameters = job::pIGraphicParameters_External;
#else
    pIGraphicParameters_Local = new GraphicParameters(this);

    if ( pRHS -> pIGraphicParameters == job::pIGraphicParameters_External )
        pIGraphicParameters = job::pIGraphicParameters_External;
    else
        pIGraphicParameters = pIGraphicParameters_Local;
#endif
    return;
    }


    gdiParameters::~gdiParameters() {
    if ( ! ( NULL == pLineStyles ) )
        delete [] pLineStyles;
#ifdef USE_RENDERER
#else
    delete pIGraphicParameters_Local;
#endif
    }


    // gdiParameters

    void gdiParameters::setupDC() {
    setRGBColor(rgbColor);
    setLineWidth(lineWidth);
    setLineJoin(lineJoin);
    setLineCap(lineCap);
    setLineDash(NULL,0,0.0f);
    return;
    }


    void gdiParameters::initialize() {
    setRGBColor(defaultRGBColor);
    setLineWidth(defaultLineWidth);
    setLineJoin(defaultLineJoin);
    setLineCap(defaultLineCap);
    setLineDash(NULL,0,0.0f);
    return;
    }


#ifdef USE_RENDERER
#else
    void gdiParameters::forwardToRenderer() {
    pIGraphicParameters = job::pIGraphicParameters_External;
    setRGBColor(rgbColor);
    setLineWidth(lineWidth);
    setLineJoin(lineJoin);
    setLineCap(lineCap);
    setLineDash(NULL,0,0.0f);
    return;
    }
#endif

    void gdiParameters::setColorSpace(colorSpace *pcs) {
    pColorSpace = pcs;
    return;
    }


    colorSpace *gdiParameters::getColorSpace() {
    return pColorSpace;
    }


    void gdiParameters::setColor(colorSpace *pColorSpace) {

    if ( 0 == strcmp(pColorSpace -> Name(),"DeviceGray") ) {
        setRGBColor(RGB(pColorSpace -> getElement(1) -> IntValue() * 255,pColorSpace -> getElement(1) -> IntValue() * 255,pColorSpace -> getElement(1) -> IntValue() * 255));
        return;
    }

    if ( 0 == strcmp(pColorSpace -> Name(),"DeviceRGB") )  {
        setRGBColor(RGB(pColorSpace -> getElement(1) -> IntValue() * 255,pColorSpace -> getElement(2) -> IntValue() * 255,pColorSpace -> getElement(3) -> IntValue() * 255));
        return;
    }

    char szMessage[1024];
    sprintf_s<1024>(szMessage,"Error: Setting the color for ColorSpace family %s is not implemented",pColorSpace -> Name());
    throw notimplemented(szMessage);
    return;
    }


    void gdiParameters::setRGBColor(POINT_TYPE r,POINT_TYPE g,POINT_TYPE b) {
    setRGBColor(RGB(r * 255,g * 255,b * 255));
    return;
    }


    void gdiParameters::setRGBColor(COLORREF rgb) {
    pIGraphicParameters -> put_RGBColor(rgb);
    }


    void gdiParameters::setLineWidth(POINT_TYPE v) { 
    pIGraphicParameters -> put_LineWidth(v);
    return;
    }


    void gdiParameters::setLineJoin(long lj) {
    pIGraphicParameters -> put_LineJoin(lj);
    return;
    }


    void gdiParameters::setLineCap(long lc) {
    pIGraphicParameters -> put_LineCap(lc);
    return;
    }


    void gdiParameters::setLineDash(FLOAT *pValues,long countValues,POINT_TYPE offset) {
    pIGraphicParameters -> put_LineDash(pValues,countValues,offset);
    return;
    }