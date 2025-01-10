#include "EnVisioNateSW_GlyphRenderer.h"

    FLOAT GlyphRenderer::GraphicParameters::defaultLineWidth = 0.0f;
    long GlyphRenderer::GraphicParameters::defaultLineCap = 0;
    long GlyphRenderer::GraphicParameters::defaultLineJoin = 0;
    COLORREF GlyphRenderer::GraphicParameters::defaultRGBColor = RGB(0,0,0);

    ID2D1StrokeStyle1 *GlyphRenderer::GraphicParameters::pID2D1StrokeStyle1 = NULL;


    HRESULT GlyphRenderer::GraphicParameters::QueryInterface(REFIID refIID,void **pvResult) {

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


    HRESULT GlyphRenderer::GraphicParameters::put_LineWidth(FLOAT lw) {
    lineWidth = lw; 
    put_LineDash(NULL,-1,0.0f);
    return S_OK;
    }


    HRESULT GlyphRenderer::GraphicParameters::put_LineJoin(long lj) {

    switch ( lj ) {
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

    default:
        lineJoin = D2D1_LINE_JOIN_MITER;

    }
    put_LineDash(NULL,-1,0.0f);
    return S_OK;
    }


    HRESULT GlyphRenderer::GraphicParameters::put_LineCap(long lc) {
    switch ( lc ) {
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
    put_LineDash(NULL,-1,0.0f);
    return S_OK;
    }


    HRESULT GlyphRenderer::GraphicParameters::put_LineDash(FLOAT *pValues,long countValues,FLOAT offset) {

    if ( ! -1 == countValues ) {

        if ( ! ( NULL == pValues ) ) {

            if ( ! ( NULL == pLineStyles ) ) {
                delete [] pLineStyles;
                countDashSizes = 0;
            }

            pLineStyles = NULL;

            if ( 0 < countValues ) {
                countValues = min(16,countValues);
                pLineStyles = new FLOAT[countValues];
                memcpy(pLineStyles,pValues,countValues * sizeof(FLOAT));
            }

            countDashSizes = countValues;

        } else {

            if ( ! ( NULL == pLineStyles ) ) {
                delete [] pLineStyles;
                pLineStyles = NULL;
            }

            countDashSizes = 0;

        }

    }

    if ( ! ( NULL == pID2D1StrokeStyle1 ) )
        pID2D1StrokeStyle1 -> Release();

    D2D1_DASH_STYLE dashStyle = D2D1_DASH_STYLE_SOLID;

    if ( ! ( NULL == pLineStyles ) ) 
        dashStyle = D2D1_DASH_STYLE_CUSTOM;

    pParent -> pID2D1Factory1 -> CreateStrokeStyle(
        D2D1::StrokeStyleProperties1(
            lineCap,
            lineCap,
            lineCap,
            lineJoin,
            10.0f,
            dashStyle,
            offset,
            D2D1_STROKE_TRANSFORM_TYPE_FIXED),
        pLineStyles,
        countDashSizes,
        &pID2D1StrokeStyle1);

    FLOAT r = (FLOAT)GetRValue(rgbColor) / 255.0f;
    FLOAT g = (FLOAT)GetGValue(rgbColor) / 255.0f;
    FLOAT b = (FLOAT)GetBValue(rgbColor) / 255.0f;

    if ( ! ( NULL == pParent -> pID2D1SolidColorBrush ) )
        pParent -> pID2D1SolidColorBrush-> Release();

    pParent -> pID2D1DCRenderTarget -> CreateSolidColorBrush(D2D1::ColorF(r,g,b, 1.0f),&pParent -> pID2D1SolidColorBrush);

    return S_OK;
    }


    HRESULT GlyphRenderer::GraphicParameters::put_RGBColor(COLORREF color) {

    if ( color == rgbColor )
        return S_OK;

    pParent -> Render();

    rgbColor = color;
    FLOAT r = (FLOAT)GetRValue(color) / 255.0f;
    FLOAT g = (FLOAT)GetGValue(color) / 255.0f;
    FLOAT b = (FLOAT)GetBValue(color) / 255.0f;
    pParent -> pID2D1SolidColorBrush -> Release();
    pParent -> pID2D1DCRenderTarget -> CreateSolidColorBrush(D2D1::ColorF(r,g,b, 1.0f),&pParent -> pID2D1SolidColorBrush);

    return S_OK;
    }
