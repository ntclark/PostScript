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

#define DEFINE_DATA

#include "Renderer.h"

#include "Renderer_i.c"

    char Renderer::szLogMessage[1024];
    char Renderer::szStatusMessage[1024];
    char Renderer::szErrorMessage[1024];

    Renderer::Renderer(IUnknown *pUnkOuter) {

    pIGraphicElements = new GraphicElements(this);
    pIGraphicParameters = new GraphicParameters(this);

    D2D1_FACTORY_OPTIONS factoryOptions{D2D1_DEBUG_LEVEL_INFORMATION};

    D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED,factoryOptions,&pID2D1Factory1);

    pIConnectionPointContainer = new _IConnectionPointContainer(this);
    pIConnectionPoint = new _IConnectionPoint(this);

    pIConnectionPointContainer -> AddRef();
    pIConnectionPoint -> AddRef();

    return;
    }


    Renderer::~Renderer() {
    pID2D1Factory1 -> Release();
    pID2D1Factory1 = NULL;
    pIConnectionPointContainer -> Release();
    pIConnectionPoint -> Release();
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
    if ( ! ( S_OK == hr ) ) {
        sprintf_s<1024>(Renderer::szErrorMessage,"ID2D1Factory1::CreatePathGeometry() returned HR 0x%x on Line %d in Function %s\n",hr,__LINE__,__FUNCTION__);
        pIConnectionPointContainer -> fire_ErrorNotification(Renderer::szErrorMessage);
        pIConnectionPointContainer -> fire_ErrorNotification(Renderer::szErrorMessage);
    }
    hr = pID2D1PathGeometry -> Open(&pID2D1GeometrySink);
    if ( ! ( S_OK == hr ) ) {

        sprintf_s<1024>(Renderer::szErrorMessage,"ID2D1PathGeometry::Open() returned HR 0x%x on Line %d in Function %s\n",hr,__LINE__,__FUNCTION__);
        pIConnectionPointContainer -> fire_ErrorNotification(Renderer::szErrorMessage);
        pIConnectionPointContainer -> fire_ErrorNotification(Renderer::szErrorMessage);
    }
    return;
    }


    void Renderer::closeSink() {
    if ( NULL == pID2D1GeometrySink ) 
        return;
    HRESULT hr = pID2D1GeometrySink -> Close();
    if ( ! ( S_OK == hr ) ) {
        sprintf_s<1024>(Renderer::szErrorMessage,"pID2D1GeometrySink::Close() returned HR 0x%x on Line %d in Function %s\n",hr,__LINE__,__FUNCTION__);
        pIConnectionPointContainer -> fire_ErrorNotification(Renderer::szErrorMessage);
        pIConnectionPointContainer -> fire_ErrorNotification(Renderer::szErrorMessage);
    }
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

    if ( 0 < pIConnectionPoint -> CountConnections() ) {
        sprintf_s<1024>(Renderer::szLogMessage,"FILL Rendering the content.\n");
        pIConnectionPointContainer -> fire_LogNotification(Renderer::szLogMessage);
    }

    pID2D1DCRenderTarget -> BeginDraw();
    pID2D1DCRenderTarget -> FillGeometry(pID2D1PathGeometry,(ID2D1Brush *)pID2D1SolidColorBrush);
    HRESULT hr = pID2D1DCRenderTarget -> EndDraw(&tag1,&tag2);

    if ( ! ( S_OK == hr ) ) {
        OutputDebugStringA("THAT ERROR HAPPENED\n");
        sprintf(Renderer::szErrorMessage,"Error: The Direct2D error happened at line %d in %s\n",__LINE__,__FUNCTION__);
        pIConnectionPointContainer -> fire_StatusNotification(Renderer::szErrorMessage);
        pIConnectionPointContainer -> fire_ErrorNotification(Renderer::szErrorMessage);
    }

    return S_OK;
    }


    HRESULT Renderer::strokeRender() {

    if ( 0 < pIConnectionPoint -> CountConnections() ) {
        sprintf_s<1024>(Renderer::szLogMessage,"STROKE Rendering the content.");
        pIConnectionPointContainer -> fire_LogNotification(Renderer::szLogMessage);
    }

    pID2D1DCRenderTarget -> BeginDraw();
    pID2D1DCRenderTarget -> DrawGeometry(pID2D1PathGeometry,(ID2D1Brush *)pID2D1SolidColorBrush,pIGraphicParameters -> valuesStack.top() -> lineWidth,pID2D1StrokeStyle1);
    HRESULT hr = pID2D1DCRenderTarget -> EndDraw(&tag1,&tag2);

    if ( ! ( S_OK == hr ) ) {
        OutputDebugStringA("THAT ERROR HAPPENED\n");
        sprintf(Renderer::szErrorMessage,"Error: The Direct2D error happened at line %d in %s\n",__LINE__,__FUNCTION__);
        pIConnectionPointContainer -> fire_StatusNotification(Renderer::szErrorMessage);
        pIConnectionPointContainer -> fire_ErrorNotification(Renderer::szErrorMessage);
    }

    return S_OK;
    }