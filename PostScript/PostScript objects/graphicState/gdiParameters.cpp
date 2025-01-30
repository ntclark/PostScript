
#include "job.h"

#include "gdiParameters.h"

    // gdiParameters

    HRESULT gdiParameters::SaveState() {
    return job::pIGraphicParameters_External -> SaveState();
    }


    HRESULT gdiParameters::RestoreState() {
    return job::pIGraphicParameters_External -> RestoreState();
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


    void gdiParameters::setRGBColor(POINT_TYPE r,POINT_TYPE g,POINT_TYPE b) {
    setRGBColor(RGB(r * 255,g * 255,b * 255));
    return;
    }


    void gdiParameters::setRGBColor(COLORREF rgb) {
    job::pIGraphicParameters_External -> put_RGBColor(rgb);
    }


    void gdiParameters::setLineWidth(POINT_TYPE v) { 
    job::pIGraphicParameters_External -> put_LineWidth(v);
    return;
    }


    void gdiParameters::setLineJoin(long lj) {
    job::pIGraphicParameters_External -> put_LineJoin(lj);
    return;
    }


    void gdiParameters::setLineCap(long lc) {
    job::pIGraphicParameters_External -> put_LineCap(lc);
    return;
    }


    void gdiParameters::setLineDash(FLOAT *pValues,long countValues,POINT_TYPE offset) {
    job::pIGraphicParameters_External -> put_LineDash(pValues,countValues,offset);
    return;
    }