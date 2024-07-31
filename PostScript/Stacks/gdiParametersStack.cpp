
#include "job.h"

#include "PostScript objects\graphicsState.h"

    gdiParametersStack::gdiParametersStack() {
    push(new gdiParameters());
    return;
    }


    gdiParametersStack::~gdiParametersStack() {
    while ( 0 < size() ) {
        delete top();
        pop();
    }
    return;
    }


    void gdiParametersStack::gSave() {
    push(new gdiParameters(top()));
    return;
    }


    void gdiParametersStack::gRestore() {
    if ( 1 == size() )
{
Beep(2000,200);
        return;
}
    delete top();
    pop();
    top() -> setupDC();
    return;
    }


    void gdiParametersStack::setupDC() {
    top() -> setupDC();
    }


    // gdiParameters


    void gdiParameters::setupDC() {
    setRGBColor(rgbColor);
    setLineWidth(lineWidth);
    setLineJoin(lineJoin);
    setLineCap(lineCap);
    setLineDash(NULL,0.0);
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
                pLineStyles[k] = (DWORD)(PICA_FACTOR * pArray -> getElement(k) -> DoubleValue());
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

    HPEN hPen = ExtCreatePen(penStyle,(int)(0*lineWidth * PICA_FACTOR),&logBrush,countDashSizes,pLineStyles);

    DeleteObject(SelectObject(pPStoPDF -> GetDC(),hPen));

    return;
    }