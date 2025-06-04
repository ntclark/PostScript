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

#include "gdiParameters.h"

    // gdiParameters

    void gdiParameters::setColorSpace(colorSpace *pcs) {
    pColorSpace = pcs;
    return;
    }


    colorSpace *gdiParameters::getColorSpace() {
    return pColorSpace;
    }


    void gdiParameters::setColor(colorSpace *pColorSpace) {

    if ( 0 == strcmp(pColorSpace -> pFamilyName -> Contents(),"DeviceGray") ) {
        setRGBColor(RGB((uint8_t)(pColorSpace -> getElement(1) -> FloatValue() * 255.0f),
                        (uint8_t)(pColorSpace -> getElement(1) -> FloatValue() * 255.0f),
                        (uint8_t)(pColorSpace -> getElement(1) -> FloatValue() * 255.0f)));
        return;
    }

    if ( 0 == strcmp(pColorSpace -> pFamilyName -> Name(),"DeviceRGB") )  {
        setRGBColor(RGB((uint8_t)(pColorSpace -> getElement(1) -> FloatValue() * 255.0f),
                        (uint8_t)(pColorSpace -> getElement(2) -> FloatValue() * 255.0f),
                        (uint8_t)(pColorSpace -> getElement(3) -> FloatValue() * 255.0f)));
        return;
    }

    if ( 0 == strcmp(pColorSpace -> pFamilyName -> Name(),"Indexed") )  {
        if ( 0 == strcmp(pColorSpace -> pBaseName -> Contents(),"DeviceGray") ) {
            setRGBColor(RGB((uint8_t)(pColorSpace -> getElement(1) -> FloatValue() * 255.0f),
                            (uint8_t)(pColorSpace -> getElement(1) -> FloatValue() * 255.0f),
                            (uint8_t)(pColorSpace -> getElement(1) -> FloatValue() * 255.0f)));
            return;
        }
        if ( 0 == strcmp(pColorSpace -> pBaseName -> Name(),"DeviceRGB") ) {
            setRGBColor(RGB((uint8_t)(pColorSpace -> getElement(1) -> FloatValue() * 255.0f),
                            (uint8_t)(pColorSpace -> getElement(2) -> FloatValue() * 255.0f),
                            (uint8_t)(pColorSpace -> getElement(3) -> FloatValue() * 255.0f)));
            return;
        }
    }

    char szMessage[1024];
    sprintf_s<1024>(szMessage,"Error: Setting the color for ColorSpace family %s is not implemented",pColorSpace -> pFamilyName -> Contents());
    throw notimplemented(szMessage);
    return;
    }


    void gdiParameters::setRGBColor(POINT_TYPE r,POINT_TYPE g,POINT_TYPE b) {
    setRGBColor(RGB(r * 255,g * 255,b * 255));
    return;
    }


    void gdiParameters::getRGBColor(FLOAT *pR,FLOAT *pG,FLOAT *pB) {
    COLORREF rgb;
    PostScriptInterpreter::pIGraphicParameters -> get_RGBColor(&rgb);
    FLOAT r = (FLOAT)GetRValue(rgb) / 255.0f;
    FLOAT g = (FLOAT)GetGValue(rgb) / 255.0f;
    FLOAT b = (FLOAT)GetBValue(rgb) / 255.0f;
    return;
    }


    void gdiParameters::setRGBColor(COLORREF rgb) {
    PostScriptInterpreter::pIGraphicParameters -> put_RGBColor(rgb);
    return;
    }


    void gdiParameters::getRGBColor(COLORREF *pRGB) {
    PostScriptInterpreter::pIGraphicParameters -> get_RGBColor(pRGB);
    return;
    }


    void gdiParameters::setLineWidth(FLOAT v) { 
    PostScriptInterpreter::pIGraphicParameters -> put_LineWidth(v);
    return;
    }


    void gdiParameters::getLineWidth(FLOAT *pv) { 
    PostScriptInterpreter::pIGraphicParameters -> get_LineWidth(pv);
    return;
    }


    void gdiParameters::setLineJoin(long lj) {
    PostScriptInterpreter::pIGraphicParameters -> put_LineJoin(lj);
    return;
    }


    void gdiParameters::getLineJoin(long *plj) {
    PostScriptInterpreter::pIGraphicParameters -> get_LineJoin(plj);
    return;
    }


    void gdiParameters::setLineCap(long lc) {
    PostScriptInterpreter::pIGraphicParameters -> put_LineCap(lc);
    return;
    }


    void gdiParameters::getLineCap(long *plc) {
    PostScriptInterpreter::pIGraphicParameters -> get_LineCap(plc);
    return;
    }


    void gdiParameters::setMiterLimit(FLOAT v) { 
    PostScriptInterpreter::pIGraphicParameters -> put_MiterLimit(v);
    return;
    }


    void gdiParameters::getMiterLimit(FLOAT *pv) { 
    PostScriptInterpreter::pIGraphicParameters -> get_MiterLimit(pv);
    return;
    }


    void gdiParameters::setLineDash(FLOAT *pValues,long countValues,POINT_TYPE offset) {
    PostScriptInterpreter::pIGraphicParameters -> put_LineDash(pValues,countValues,offset);
    return;
    }