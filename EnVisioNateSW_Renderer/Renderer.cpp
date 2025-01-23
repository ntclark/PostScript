
#define DEFINE_DATA

#include "EnVisioNateSW_Renderer.h"

#include "Renderer_i.c"

    Renderer::Renderer(IUnknown *pUnkOuter) {
    pIGraphicElements = new GraphicElements(this);
    pIGraphicParameters = new GraphicParameters(this);
    return;
    }


    Renderer::~Renderer() {
    return;
    }


    void Renderer::setupRenderer() {

    if ( NULL == pID2D1Factory1 ) 
        D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED,&pID2D1Factory1);

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_IGNORE),
            0,
            0,
            D2D1_RENDER_TARGET_USAGE_NONE,
            D2D1_FEATURE_LEVEL_DEFAULT);

    HRESULT hr = pID2D1Factory1 -> CreateDCRenderTarget(&props, &pID2D1DCRenderTarget);

    RECT rcImage{0,0,0,0};
    GetClientRect(WindowFromDC(hdc),&rcImage);

    hr = pID2D1DCRenderTarget -> BindDC(hdc,&rcImage);

    FLOAT r = (FLOAT)GetRValue(Renderer::GraphicParameters::defaultRGBColor) / 255.0f;
    FLOAT g = (FLOAT)GetGValue(Renderer::GraphicParameters::defaultRGBColor) / 255.0f;
    FLOAT b = (FLOAT)GetBValue(Renderer::GraphicParameters::defaultRGBColor) / 255.0f;

    pID2D1DCRenderTarget -> CreateSolidColorBrush(D2D1::ColorF(r,g,b,0.0f),&pID2D1SolidColorBrush);

    pIGraphicParameters -> put_LineWidth(Renderer::GraphicParameters::defaultLineWidth);
    pIGraphicParameters -> put_LineJoin(Renderer::GraphicParameters::defaultLineJoin);
    pIGraphicParameters -> put_LineCap(Renderer::GraphicParameters::defaultLineCap);

    pID2D1DCRenderTarget -> SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    return;
    }


    HRESULT Renderer::internalRender() {

    if ( ! ( NULL == pID2D1GeometrySink ) ) {
        pID2D1GeometrySink -> Close();
        pID2D1GeometrySink -> Release();
        pID2D1GeometrySink = NULL;
    }

    pID2D1DCRenderTarget -> BeginDraw();

    if ( doRasterize )
        pID2D1DCRenderTarget -> FillGeometry(pID2D1PathGeometry,(ID2D1Brush *)pID2D1SolidColorBrush);
    else
        pID2D1DCRenderTarget -> DrawGeometry(pID2D1PathGeometry,(ID2D1Brush *)pID2D1SolidColorBrush,pIGraphicParameters -> lineWidth,pIGraphicParameters -> pID2D1StrokeStyle1);

    lastRenderWasFilled = doRasterize;

    doRasterize = false;

    renderCount++;

    D2D1_TAG tag1;
    D2D1_TAG tag2;

    HRESULT hr = pID2D1DCRenderTarget -> EndDraw(&tag1,&tag2);
{
if ( ! ( S_OK == hr ) )
MessageBox(NULL,"BAD","BAD",MB_OK);
}
    pID2D1PathGeometry -> Release();

    pID2D1PathGeometry = NULL;

    hr = pID2D1Factory1 -> CreatePathGeometry(&pID2D1PathGeometry);
    if ( ! ( S_OK == hr ) )
{
MessageBox(NULL,"BAD","BAD",MB_OK);
        return hr;
}

    hr = pID2D1PathGeometry -> Open(&pID2D1GeometrySink);
    if ( ! ( S_OK == hr ) )
{
MessageBox(NULL,"BAD","BAD",MB_OK);
        return hr;
}
    return S_OK;
    }