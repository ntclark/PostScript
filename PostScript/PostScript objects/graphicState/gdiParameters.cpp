
#include "job.h"

#include "gdiParameters.h"

    POINT_TYPE gdiParameters::defaultLineWidth = 0.0;
    long gdiParameters::defaultLineCap = 0;
    long gdiParameters::defaultLineJoin = 0;
    COLORREF gdiParameters::defaultRGBColor = RGB(0,0,0);

    // gdiParameters

    void gdiParameters::setupDC() {
    setRGBColor(rgbColor);
    setLineWidth(lineWidth);
    setLineJoin(lineJoin);
    setLineCap(lineCap);
    setLineDash(NULL,0.0);
    return;
    }


    void gdiParameters::initialize() {
    setRGBColor(defaultRGBColor);
    setLineWidth(defaultLineWidth);
    setLineJoin(defaultLineJoin);
    setLineCap(defaultLineCap);
    setLineDash(NULL,0.0);
    return;
    }


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


    void gdiParameters::setRGBColor(COLORREF rgb) {
    rgbColor = rgb;
    DeleteObject(SelectObject(pPStoPDF -> GetDC(),CreateSolidBrush(rgbColor)));
    setLineDash(NULL,0.0);
    return;
    }


    void gdiParameters::setRGBColor(POINT_TYPE r,POINT_TYPE g,POINT_TYPE b) {
    setRGBColor(RGB(r * 255,g * 255,b * 255));
    return;
    }


    void gdiParameters::setLineWidth(POINT_TYPE v) { 
    lineWidth = v; 
    setLineDash(NULL,0.0);
    return;
    }


    void gdiParameters::setLineJoin(long lj) {
    lineJoin = lj;
    setLineDash(NULL,0.0);
    return;
    }


    void gdiParameters::setLineCap(long lc) {
    lineCap = lc;
    setLineDash(NULL,0.0);
    return;
    }


    void gdiParameters::setLineDash(class array *pArray,POINT_TYPE offset) {

    DWORD countItems = 0;

    if ( ! ( NULL == pArray ) ) {

        countItems = pArray -> size();

        if ( ! ( NULL == pLineStyles ) ) {
            delete [] pLineStyles;
            countDashSizes = 0;
        }

        pLineStyles = NULL;

        if ( 0 < countItems ) {
            countItems = min(16,countItems);
            pLineStyles = new DWORD[countItems];
            for ( long k = 0; k < countItems; k++ )
                pLineStyles[k] = (DWORD)(pArray -> getElement(k) -> DoubleValue());
        }

        countDashSizes = countItems;

    }

/*
      0 Butt cap. The stroke is squared off at the endpoint of the path. There is no
      projection beyond the end of the path.

      1 Round cap. A semicircular arc with a diameter equal to the line width is
      drawn around the endpoint and filled in.

      2 Projecting square cap. The stroke continues beyond the endpoint of the
      path for a distance equal to half the line width and is then squared off.
*/

    LOGBRUSH logBrush{0};
    logBrush.lbColor = rgbColor;
    logBrush.lbStyle = BS_SOLID;

    long penStyle = PS_GEOMETRIC;

    if ( ! ( NULL == pLineStyles ) )
        penStyle |= PS_USERSTYLE;

    switch ( lineCap ) {
    case 0:
        penStyle |= PS_ENDCAP_FLAT;
        break;
    case 1:
        penStyle |= PS_ENDCAP_ROUND;
        break;
    case 2:
        penStyle |= PS_ENDCAP_SQUARE;
        break;
    }

    HPEN hPen = ExtCreatePen(penStyle,(int)(0*lineWidth),&logBrush,countDashSizes,pLineStyles);

    DeleteObject(SelectObject(pPStoPDF -> GetDC(),hPen));

    return;
    }