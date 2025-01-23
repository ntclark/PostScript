#include "EnVisioNateSW_Renderer.h"

    FLOAT Renderer::GraphicParameters::defaultLineWidth = 0.0f;
    D2D1_CAP_STYLE Renderer::GraphicParameters::defaultLineCap = D2D1_CAP_STYLE_ROUND;//D2D1_CAP_STYLE_FLAT;
    D2D1_LINE_JOIN Renderer::GraphicParameters::defaultLineJoin = D2D1_LINE_JOIN_MITER;
    D2D1_DASH_STYLE Renderer::GraphicParameters::defaultDashStyle = D2D1_DASH_STYLE_SOLID;
    COLORREF Renderer::GraphicParameters::defaultRGBColor = RGB(0,0,0);

    ID2D1StrokeStyle1 *Renderer::GraphicParameters::pID2D1StrokeStyle1 = NULL;


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
    if ( lineWidth == lw )
        return S_OK;
    lineWidth = lw; 
    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_LineJoin(long lj) {

    if ( lineJoin == (D2D1_LINE_JOIN) lj )
        return S_OK;

    switch ( lj ) {
    default:
    case 0:
        lineJoin = D2D1_LINE_JOIN_MITER;
        break;

    case 1:
        lineJoin = D2D1_LINE_JOIN_BEVEL;
        break;

    case 2:
        lineJoin = D2D1_LINE_JOIN_ROUND;
        break;

    case 3:
        lineJoin = D2D1_LINE_JOIN_MITER_OR_BEVEL;
        break;

    }

    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_LineCap(long lc) {

    if ( lineCap == (D2D1_CAP_STYLE) lc )
        return S_OK;

    switch ( lc ) {
    default:
    case 0:
        lineCap = D2D1_CAP_STYLE_FLAT;
        break;
    case 1:
        lineCap = D2D1_CAP_STYLE_SQUARE;
        break;
    case 2:
        lineCap = D2D1_CAP_STYLE_ROUND;
        break;
    }

    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_LineDash(FLOAT *pValues,long countValues,FLOAT offset) {

    FLOAT *pLineStyles = NULL;

    countDashSizes = 0;

    if ( ! ( -1 == countValues ) ) {

        countDashSizes = countValues;

        if ( ! ( NULL == pValues ) ) {

            if ( 0 < countDashSizes ) {
                countDashSizes = min(16,countDashSizes);
                pLineStyles = new FLOAT[countDashSizes];
                memcpy(pLineStyles,pValues,countDashSizes * sizeof(FLOAT));
for ( long k = 0; k < countDashSizes; k++ )
pLineStyles[k] *= 8.0f;
            }

        }  else

            countDashSizes = 0;

    }

    lineDashStyle = D2D1_DASH_STYLE_SOLID;
    if ( 0 < countDashSizes )
        lineDashStyle = D2D1_DASH_STYLE_CUSTOM;

    if ( ! ( NULL == pID2D1StrokeStyle1 ) )
        pID2D1StrokeStyle1 -> Release();

    pParent -> pID2D1Factory1 -> CreateStrokeStyle(
            D2D1::StrokeStyleProperties1(
                lineCap,
                lineCap,
                lineCap,
                lineJoin,
                10.0f,
                lineDashStyle,
                offset,
                D2D1_STROKE_TRANSFORM_TYPE_FIXED),
                (FLOAT *)pLineStyles,
                countDashSizes,
                    &pID2D1StrokeStyle1);

    if ( 0 < countDashSizes )
        delete [] pLineStyles;

    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_RGBColor(COLORREF color) {
    rgbColor = color;

if ( NULL == pParent -> pID2D1DCRenderTarget )
return S_OK;

    pParent -> pID2D1SolidColorBrush -> Release();
    FLOAT r = (FLOAT)GetRValue(rgbColor) / 255.0f;
    FLOAT g = (FLOAT)GetGValue(rgbColor) / 255.0f;
    FLOAT b = (FLOAT)GetBValue(rgbColor) / 255.0f;
    pParent -> pID2D1DCRenderTarget -> CreateSolidColorBrush(D2D1::ColorF(r,g,b, 1.0f),&pParent -> pID2D1SolidColorBrush);

    return S_OK;
    }


    void Renderer::GraphicParameters::resetParameters(char *pszLineSettings) {

    FLOAT f[5];
    sscanf_s(pszLineSettings,"%f-%f-%f-%f-%f-%ld",f,f + 1,f + 2,f + 3,f + 4,&rgbColor);

    lineWidth = f[0];
    lineCap = (D2D1_CAP_STYLE)f[1];
    lineJoin = (D2D1_LINE_JOIN)f[2];
    lineDashStyle = (D2D1_DASH_STYLE)f[3];
    countDashSizes = (long)f[4];

    put_LineDash(NULL,-1L,0.0f);

    put_RGBColor(rgbColor);

    return;
    }


    long Renderer::GraphicParameters::hashCode(char *szLineSettings) {
    sprintf_s(szLineSettings,128,"%f-%f-%f-%f-%f-%ld",
            (FLOAT)lineWidth,(FLOAT)lineCap,(FLOAT)lineJoin,(FLOAT)lineDashStyle,(FLOAT)countDashSizes,rgbColor);
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