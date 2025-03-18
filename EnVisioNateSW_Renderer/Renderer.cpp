
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

    pID2D1DCRenderTarget -> SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    return;
    }


    void Renderer::setupPathAndSink() {

    HRESULT hr = pID2D1Factory1 -> CreatePathGeometry(&pID2D1PathGeometry);
    if ( ! ( S_OK == hr ) )
{
MessageBox(NULL,"BAD","BAD",MB_OK);
        return;
}

    hr = pID2D1PathGeometry -> Open(&pID2D1GeometrySink);
    if ( ! ( S_OK == hr ) )
{
MessageBox(NULL,"BAD","BAD",MB_OK);
        return;
}

    return;
    }


    void Renderer::closeSink() {
    if ( NULL == pID2D1GeometrySink ) 
        return;
    pID2D1GeometrySink -> Close();
    pID2D1GeometrySink -> Release();
    pID2D1GeometrySink = NULL;
    return;
    }


    void Renderer::shutdownPathAndSink() {
    closeSink();
    if ( ! ( NULL == pID2D1PathGeometry ) ) {
        pID2D1PathGeometry -> Release();
        pID2D1PathGeometry = NULL;
    }
    return;
    }


    void Renderer::shutdownRenderer() {

    if ( ! ( NULL == pID2D1SolidColorBrush ) ) {
        pID2D1SolidColorBrush -> Release();
        pID2D1SolidColorBrush = NULL;
    }

    if ( ! ( NULL == pID2D1StrokeStyle1 ) ) {
        pID2D1StrokeStyle1 -> Release();
        pID2D1StrokeStyle1 = NULL;
    }

    pID2D1DCRenderTarget -> Release();
    pID2D1DCRenderTarget = NULL;

    return;
    }


    HRESULT Renderer::fillRender() {

    pID2D1DCRenderTarget -> BeginDraw();
    pID2D1DCRenderTarget -> FillGeometry(pID2D1PathGeometry,(ID2D1Brush *)pID2D1SolidColorBrush);
    HRESULT hr = pID2D1DCRenderTarget -> EndDraw(&tag1,&tag2);

    if ( ! ( S_OK == hr ) ) 
        OutputDebugStringA("THAT ERROR HAPPENED\n");

    return S_OK;
    }


    HRESULT Renderer::strokeRender() {

    pID2D1DCRenderTarget -> BeginDraw();
    pID2D1DCRenderTarget -> DrawGeometry(pID2D1PathGeometry,(ID2D1Brush *)pID2D1SolidColorBrush,pIGraphicParameters -> valuesStack.top() -> lineWidth,pID2D1StrokeStyle1);
    HRESULT hr = pID2D1DCRenderTarget -> EndDraw(&tag1,&tag2);

    if ( ! ( S_OK == hr ) ) 
        OutputDebugStringA("THAT ERROR HAPPENED\n");

    return S_OK;
    }