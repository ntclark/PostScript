
#define DEFINE_DATA

#include "Renderer.h"

#include "Renderer_i.c"

    char Renderer::szStatusMessage[1024];
    char Renderer::szErrorMessage[1024];

    Renderer::Renderer(IUnknown *pUnkOuter) {
    pIGraphicElements = new GraphicElements(this);
    pIGraphicParameters = new GraphicParameters(this);
    D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED,&pID2D1Factory1);
    pIConnectionPointContainer = new _IConnectionPointContainer(this);
    pIConnectionPoint = new _IConnectionPoint(this);
    return;
    }


    Renderer::~Renderer() {
    pID2D1Factory1 -> Release();
    pID2D1Factory1 = NULL;
    delete pIConnectionPointContainer;
    delete pIConnectionPoint;
    return;
    }


    void Renderer::setupRenderer(HDC hdc,RECT *pDrawingRect) {

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_IGNORE),
            0,
            0,
            D2D1_RENDER_TARGET_USAGE_NONE,
            D2D1_FEATURE_LEVEL_DEFAULT);

    HRESULT hr = pID2D1Factory1 -> CreateDCRenderTarget(&props, &pID2D1DCRenderTarget);

    hr = pID2D1DCRenderTarget -> BindDC(hdc,pDrawingRect);

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

    if ( ! ( S_OK == hr ) ) {
        OutputDebugStringA("THAT ERROR HAPPENED\n");
        sprintf(Renderer::szErrorMessage,"Error: The Direct2D error happened at line %d in %s",__LINE__,__FUNCTION__);
        pIConnectionPointContainer -> fire_StatusNotification(Renderer::szErrorMessage);
        pIConnectionPointContainer -> fire_ErrorNotification(Renderer::szErrorMessage);
    }

    return S_OK;
    }


    HRESULT Renderer::strokeRender() {

    pID2D1DCRenderTarget -> BeginDraw();
    pID2D1DCRenderTarget -> DrawGeometry(pID2D1PathGeometry,(ID2D1Brush *)pID2D1SolidColorBrush,pIGraphicParameters -> valuesStack.top() -> lineWidth,pID2D1StrokeStyle1);
    HRESULT hr = pID2D1DCRenderTarget -> EndDraw(&tag1,&tag2);

    if ( ! ( S_OK == hr ) ) {
        OutputDebugStringA("THAT ERROR HAPPENED\n");
        sprintf(Renderer::szErrorMessage,"Error: The Direct2D error happened at line %d in %s",__LINE__,__FUNCTION__);
        pIConnectionPointContainer -> fire_StatusNotification(Renderer::szErrorMessage);
        pIConnectionPointContainer -> fire_ErrorNotification(Renderer::szErrorMessage);
    }

    return S_OK;
    }