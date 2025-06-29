/*
Copyright 2025 EnVisioNate LLC

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the �Software�), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is the MIT License
*/

#include "Renderer.h"


    FLOAT Renderer::GraphicParameters::defaultLineWidth = 0.0f;
    FLOAT Renderer::GraphicParameters::defaultMiterLimit = 10.0f;
    D2D1_CAP_STYLE Renderer::GraphicParameters::defaultLineCap = D2D1_CAP_STYLE_ROUND;//D2D1_CAP_STYLE_FLAT;
    D2D1_LINE_JOIN Renderer::GraphicParameters::defaultLineJoin = D2D1_LINE_JOIN_MITER;
    D2D1_DASH_STYLE Renderer::GraphicParameters::defaultDashStyle = D2D1_DASH_STYLE_SOLID;
    COLORREF Renderer::GraphicParameters::defaultRGBColor = RGB(0,0,0);


    HRESULT Renderer::GraphicParameters::QueryInterface(REFIID refIID,void **pvResult) {

    if ( ! pvResult )
        return E_POINTER;

    *pvResult = NULL;

    if ( IID_IUnknown == refIID ) 
        *pvResult = static_cast<IUnknown *>(this);

    else if ( IID_IGraphicParameters == refIID )
        *pvResult = static_cast<GraphicParameters *>(this);

    if ( * pvResult ) {
        AddRef();
        return S_OK;
    }

    return QueryInterface(refIID,pvResult);
    }


    HRESULT Renderer::GraphicParameters::put_LineWidth(FLOAT lw) {
    pParent -> pGraphicsStateManager -> parametersStack.top() -> lineWidth = lw; 
    //if ( 0 < pParent -> pIConnectionPoint -> CountConnections() ) {
    //    sprintf_s<1024>(Renderer::szLogMessage,"Set LineWidth: %5.2f",lw);
    //    pParent -> pIConnectionPointContainer -> fire_LogNotification(Renderer::szLogMessage);
    //}
    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::get_LineWidth(FLOAT *plw) {
    if ( ! ( plw ) )
        return E_POINTER;
    *plw = pParent -> pGraphicsStateManager -> parametersStack.top() -> lineWidth;
    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_LineJoin(long lj) {

    switch ( lj ) {
    default:
    case 0:
        pParent -> pGraphicsStateManager -> parametersStack.top() -> lineJoin = D2D1_LINE_JOIN_MITER;
        break;

    case 1:
        pParent -> pGraphicsStateManager -> parametersStack.top() -> lineJoin = D2D1_LINE_JOIN_ROUND;
        break;

    case 2:
        pParent -> pGraphicsStateManager -> parametersStack.top() -> lineJoin = D2D1_LINE_JOIN_BEVEL;
        break;

    case 3:
        pParent -> pGraphicsStateManager -> parametersStack.top() -> lineJoin = D2D1_LINE_JOIN_MITER_OR_BEVEL;
        break;

    }

    //if ( 0 < pParent -> pIConnectionPoint -> CountConnections() ) {
    //    sprintf_s<1024>(Renderer::szLogMessage,"Set LineJoin : %d",lj);
    //    pParent -> pIConnectionPointContainer -> fire_LogNotification(Renderer::szLogMessage);
    //}

    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::get_LineJoin(long *plj) {
    if ( ! ( plj ) )
        return E_POINTER;
    *plj = pParent -> pGraphicsStateManager -> parametersStack.top() -> lineJoin;
    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_LineCap(long lc) {

    switch ( lc ) {
    default:
    case 0:
        pParent -> pGraphicsStateManager -> parametersStack.top() -> lineCap = D2D1_CAP_STYLE_FLAT;
        break;
    case 1:
        pParent -> pGraphicsStateManager -> parametersStack.top() -> lineCap = D2D1_CAP_STYLE_ROUND;
        break;
    case 2:
        pParent -> pGraphicsStateManager -> parametersStack.top() -> lineCap = D2D1_CAP_STYLE_SQUARE;
        break;
    }

    //if ( 0 < pParent -> pIConnectionPoint -> CountConnections() ) {
    //    sprintf_s<1024>(Renderer::szLogMessage,"Set LineCap : %d",lc);
    //    pParent -> pIConnectionPointContainer -> fire_LogNotification(Renderer::szLogMessage);
    //}

    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::get_LineCap(long *plc) {
    if ( ! ( plc ) )
        return E_POINTER;
    *plc = pParent -> pGraphicsStateManager -> parametersStack.top() -> lineCap;
    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_MiterLimit(FLOAT ml) {
    pParent -> pGraphicsStateManager -> parametersStack.top() -> miterLimit = ml;
    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::get_MiterLimit(FLOAT *pml) {
    if ( ! ( pml ) )
        return E_POINTER;
    *pml = pParent -> pGraphicsStateManager -> parametersStack.top() -> miterLimit;
    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_LineDash(FLOAT *pDashValues,long countValues,FLOAT passedOffset) {

    values *pValues = pParent -> pGraphicsStateManager -> parametersStack.top();

    if ( ! ( NULL == pDashValues ) ) {

        pValues -> countDashSizes = (int)min(16,countValues);
        pValues -> lineDashOffset = passedOffset;
        for ( long k = 0; k < pValues -> countDashSizes; k++ ) {
            POINTF ptValue{30.0f * 5.0f * pDashValues[k] / 9.0f,0.0f};
            //
            // Need to check whether the current toDeviceSpace transform is the right one here
            // or if it should be the transform saved into the primitives, maybe at rendering time ?
            //
            pParent -> pIGraphicElements -> transformPoint(&pParent -> pIGraphicElements -> toDeviceSpace,&ptValue,&ptValue);
            pValues -> lineDashSizes[k] = ptValue.x;
        }
        pValues -> lineDashStyle = D2D1_DASH_STYLE_CUSTOM;

    }  else {

        pValues -> countDashSizes = 0;
        pValues -> lineDashOffset = 0.0f;
        pValues -> lineDashStyle = D2D1_DASH_STYLE_SOLID;

    }

    if ( 0 < pParent -> pIConnectionPoint -> CountConnections() ) {
        sprintf_s<1024>(Renderer::szLogMessage,"Set LineDash: ");
        if ( 0 == countValues )
            strcat(Renderer::szLogMessage," solid line");
        else {
            char szVal[32];
            for ( int32_t k = 0; k < countValues - 1; k++ ) {
                sprintf_s<32>(szVal,"%5.1f,",pDashValues[k]);
                strcat(Renderer::szLogMessage,szVal);
            }
            sprintf_s<32>(szVal,"%5.1f",pDashValues[countValues - 1]);
            strcat(Renderer::szLogMessage,szVal);
        }
        pParent -> pIConnectionPointContainer -> fire_LogNotification(Renderer::szLogMessage);
    }

    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_RGBColor(COLORREF color) {
    pParent -> pGraphicsStateManager -> parametersStack.top() -> rgbColor = color;
    //if ( 0 < pParent -> pIConnectionPoint -> CountConnections() ) {
    //    FLOAT r = (FLOAT)GetRValue(color) / 255.0f;
    //    FLOAT g = (FLOAT)GetGValue(color) / 255.0f;
    //    FLOAT b = (FLOAT)GetBValue(color) / 255.0f;
    //    sprintf_s<1024>(Renderer::szLogMessage,"Set RGBColor : %5.1f, %5.1f, %5.1f",r,g,b);
    //    pParent -> pIConnectionPointContainer -> fire_LogNotification(Renderer::szLogMessage);
    //}
    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::get_RGBColor(COLORREF *pColor) {
    if ( ! pColor )
        return E_POINTER;
    *pColor = pParent -> pGraphicsStateManager -> parametersStack.top() -> rgbColor;
    return S_OK;
    }


    void Renderer::GraphicParameters::setParameters(char *pszLineSettings) {

    values *pValues = pParent -> pGraphicsStateManager -> parametersStack.top();
    char szLDashes[128]{128 * '\0'};
    long cntPrint = 0;
    for ( int k = 0; k < 16; k++ )
        cntPrint += sprintf(szLDashes + cntPrint,"%05.2f,",pValues -> lineDashSizes[k]);
    szLDashes[cntPrint - 1] = '\0';

    wchar_t szwGUID[64]{0};
    char szGUID[64];
    StringFromGUID2(pParent -> pGraphicsStateManager -> parametersStack.top() -> valuesId,szwGUID,64);
    WideCharToMultiByte(CP_ACP,0,szwGUID,-1,szGUID,64,NULL,NULL);

    sprintf_s(pszLineSettings,256,"%s:%05.2f:%05.2f:%01d:%01d:%01d:%01d:%08ld:%s",
                    szGUID,pValues -> lineWidth,pValues -> miterLimit,pValues -> lineCap,pValues -> lineJoin,
                            pValues -> lineDashStyle,pValues -> countDashSizes,pValues -> rgbColor,szLDashes);

    return;
    }


    void Renderer::GraphicParameters::resetParameters(char *pszLineSettings) {

    values *pValues = pParent -> pGraphicsStateManager -> parametersStack.top();

    char szLDashes[128]{128 * '\0'};

    char *pStart = pszLineSettings + GUID_PRINT_LENGTH;

    sscanf(pStart,"%05f:%05f:%01d:%01d:%01d:%01d:%08ld:%s",&pValues -> lineWidth ,&pValues -> miterLimit,&pValues -> lineCap,&pValues -> lineJoin,
                        &pValues -> lineDashStyle,&pValues -> countDashSizes,&pValues -> rgbColor,szLDashes);

    char *pszDash = strtok(szLDashes,",");

    for ( int k = 0; k < 16; k++ ) {
        pValues -> lineDashSizes[k] = (FLOAT)atof(pszDash);
        pszDash = strtok(NULL,",");
    }

    if ( ! ( NULL == pParent -> pID2D1StrokeStyle1 ) )
        pParent -> pID2D1StrokeStyle1 -> Release();

    //
    // line width is in units of PS user space
    //
    if ( 0.0f < pValues -> lineWidth ) {
        XFORM *pToPageSpace = &pParent -> pGraphicsStateManager -> parametersStack.top() -> toPageSpace;
        POINTF lw{pValues -> lineWidth,0.0f};
        FLOAT dx = pToPageSpace -> eM11 * lw.x + pToPageSpace -> eM12 * lw.y;
        FLOAT dy = pToPageSpace -> eM21 * lw.x + pToPageSpace -> eM22 * lw.y;
        XFORM *pToDeviceSpace = &pParent -> pIGraphicElements -> toDeviceSpace;
        FLOAT dxps = pToDeviceSpace -> eM11 * dx + pToDeviceSpace -> eM12 * dy;
        FLOAT dyps = pToDeviceSpace -> eM21 * dx + pToDeviceSpace -> eM22 * dy;
        pValues -> calculatedLineWidth = (FLOAT)sqrt(dxps*dxps + dyps*dyps);
    } else
        pValues -> calculatedLineWidth = 0.0f;

    HRESULT rc = pParent -> pID2D1Factory1 -> CreateStrokeStyle(
            D2D1::StrokeStyleProperties1(
                pValues -> lineCap,
                pValues -> lineCap,
                D2D1_CAP_STYLE_FLAT,
                pValues -> lineJoin,
                pValues -> miterLimit,
                pValues -> lineDashStyle,
                pValues -> lineDashOffset,
                D2D1_STROKE_TRANSFORM_TYPE_NORMAL ),//D2D1_STROKE_TRANSFORM_TYPE_FIXED),
                D2D1_DASH_STYLE_CUSTOM == pValues -> lineDashStyle ? pValues -> lineDashSizes : NULL,
                pValues -> countDashSizes,
                    &pParent -> pID2D1StrokeStyle1);

    if ( ! ( NULL == pParent-> pID2D1SolidColorBrush ) )
        pParent-> pID2D1SolidColorBrush -> Release();

    FLOAT r = (FLOAT)GetRValue(pValues -> rgbColor) / 255.0f;
    FLOAT g = (FLOAT)GetGValue(pValues -> rgbColor) / 255.0f;
    FLOAT b = (FLOAT)GetBValue(pValues -> rgbColor) / 255.0f;

    pParent -> pID2D1DCRenderTarget -> CreateSolidColorBrush(D2D1::ColorF(r,g,b, 1.0f),&pParent -> pID2D1SolidColorBrush);

    return;
    }


    long Renderer::GraphicParameters::hashCode(char *pszLineSettings) {

    setParameters(pszLineSettings);

    long hashCode = 0L;
    long part = 0L;
    long n = (DWORD)strlen(pszLineSettings) - 40;
    char *psz = new char[n + 4];
    memset(psz,0,(n + 4) * sizeof(char));
    strcpy(psz,pszLineSettings + 39);
    char *p = psz;
    for ( long k = 0; k < n; k += 4 ) {
        memcpy(&part,p,4 * sizeof(char));
        hashCode ^= part;
        p += 4;
    }
    delete [] psz;

    return hashCode;
    }