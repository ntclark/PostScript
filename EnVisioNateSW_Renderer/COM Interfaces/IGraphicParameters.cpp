#include "EnVisioNateSW_Renderer.h"

    FLOAT Renderer::GraphicParameters::defaultLineWidth = 1.0f;
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


    HRESULT Renderer::GraphicParameters::SaveState() {
    valuesStack.push(new values(*valuesStack.top()));
    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::RestoreState() {
    if ( 1 == valuesStack.size() )
        return E_UNEXPECTED;
    values *pCurrent = valuesStack.top();
    valuesStack.pop();
    delete pCurrent;
    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_LineWidth(FLOAT lw) {
    if ( valuesStack.top() -> lineWidth == lw )
        return S_OK;
    valuesStack.top() -> lineWidth = lw; 
    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_LineJoin(long lj) {

    if ( valuesStack.top() -> lineJoin == (D2D1_LINE_JOIN)lj )
        return S_OK;

    switch ( lj ) {
    default:
    case 0:
        valuesStack.top() -> lineJoin = D2D1_LINE_JOIN_MITER;
        break;

    case 1:
        valuesStack.top() -> lineJoin = D2D1_LINE_JOIN_BEVEL;
        break;

    case 2:
        valuesStack.top() -> lineJoin = D2D1_LINE_JOIN_ROUND;
        break;

    case 3:
        valuesStack.top() -> lineJoin = D2D1_LINE_JOIN_MITER_OR_BEVEL;
        break;

    }

    put_LineDash(NULL,-1L,0.0f);

    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_LineCap(long lc) {

    if ( valuesStack.top() -> lineCap == (D2D1_CAP_STYLE) lc )
        return S_OK;

    switch ( lc ) {
    default:
    case 0:
        valuesStack.top() -> lineCap = D2D1_CAP_STYLE_FLAT;
        break;
    case 1:
        valuesStack.top() -> lineCap = D2D1_CAP_STYLE_SQUARE;
        break;
    case 2:
        valuesStack.top() -> lineCap = D2D1_CAP_STYLE_ROUND;
        break;
    }

    put_LineDash(NULL,-1L,0.0f);

    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_LineDash(FLOAT *pValues,long countValues,FLOAT passedOffset) {

    if ( ! ( -1L == countValues ) ) {

        valuesStack.top() -> countDashSizes = 0;

        valuesStack.top() -> offset = passedOffset;

        if ( ! ( -1 == countValues ) ) {

            valuesStack.top() -> countDashSizes = countValues;

            if ( ! ( NULL == pValues ) ) {

                if ( 0 < valuesStack.top() -> countDashSizes ) {
                    valuesStack.top() -> countDashSizes = min(16,valuesStack.top() -> countDashSizes);
                    memcpy(valuesStack.top() -> lineStyles,pValues,valuesStack.top() -> countDashSizes * sizeof(FLOAT));
                }

            }  else

                valuesStack.top() -> countDashSizes = 0;

        }

        valuesStack.top() -> lineDashStyle = D2D1_DASH_STYLE_SOLID;
        if ( 0 < valuesStack.top() -> countDashSizes )
            valuesStack.top() -> lineDashStyle = D2D1_DASH_STYLE_CUSTOM;

    }

    addLineStylePrimitive();

    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_RGBColor(COLORREF color) {

    //if ( rgbColor == color )
    //    return S_OK;

    valuesStack.top() -> rgbColor = color;

    addColorPrimitive();

    return S_OK;
    }


    void Renderer::GraphicParameters::resetParameters(char *pszLineSettings) {

    FLOAT f[5];
    boolean doFill;
    sscanf_s(pszLineSettings,"%f-%f-%f-%f-%f-%ld-%d",f,f + 1,f + 2,f + 3,f + 4,&valuesStack.top() -> rgbColor,&doFill);

    valuesStack.top() -> lineWidth = f[0];
    valuesStack.top() -> lineCap = (D2D1_CAP_STYLE)f[1];
    valuesStack.top() -> lineJoin = (D2D1_LINE_JOIN)f[2];
    valuesStack.top() -> lineDashStyle = (D2D1_DASH_STYLE)f[3];
    valuesStack.top() -> countDashSizes = (long)f[4];

    put_LineDash(NULL,-1L,0.0f);

    put_RGBColor(valuesStack.top() -> rgbColor);

    return;
    }


    long Renderer::GraphicParameters::hashCode(char *szLineSettings,boolean doFill) {
    sprintf_s(szLineSettings,128,"%f-%f-%f-%f-%f-%ld-%d",
            (FLOAT)valuesStack.top() -> lineWidth,(FLOAT)valuesStack.top() -> lineCap,(FLOAT)valuesStack.top() -> lineJoin,
                        (FLOAT)valuesStack.top() -> lineDashStyle,(FLOAT)valuesStack.top() -> countDashSizes,valuesStack.top() -> rgbColor,doFill);
    long hashCode = 0L;
    long part = 0L;
    long n = (DWORD)strlen(szLineSettings);
    char *psz = new char[n + 4];
    memset(psz,0,(n + 4) * sizeof(char));
    strcpy(psz,szLineSettings);
    char *p = psz;
    for ( long k = 0; k < n; k += 4 ) {
        memcpy(&part,p,4 * sizeof(char));
        hashCode ^= part;
        p += 4;
    }
    delete [] psz;
    return hashCode;
    }