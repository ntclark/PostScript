
#define DEFINE_DATA

#include "EnVisioNateSW_GlyphRenderer.h"

#include "GlyphRenderer_i.c"

    GlyphRenderer::GlyphRenderer(IUnknown *pUnkOuter) {
    pIGraphicElements = new GraphicElements(this);
    pIGraphicParameters = new GraphicParameters(this);
    return;
    }


    GlyphRenderer::~GlyphRenderer() {
    return;
    }


    void GlyphRenderer::setupRenderer() {

    if ( ! ( NULL == pID2D1Factory1 ) )
        return;

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

    FLOAT r = (FLOAT)GetRValue(GlyphRenderer::GraphicParameters::defaultRGBColor) / 255.0f;
    FLOAT g = (FLOAT)GetGValue(GlyphRenderer::GraphicParameters::defaultRGBColor) / 255.0f;
    FLOAT b = (FLOAT)GetBValue(GlyphRenderer::GraphicParameters::defaultRGBColor) / 255.0f;

    pID2D1DCRenderTarget -> CreateSolidColorBrush(D2D1::ColorF(r,g,b,0.0f),&pID2D1SolidColorBrush);

    pIGraphicParameters -> put_LineWidth(GlyphRenderer::GraphicParameters::defaultLineWidth);
    pIGraphicParameters -> put_LineJoin(GlyphRenderer::GraphicParameters::defaultLineJoin);
    pIGraphicParameters -> put_LineCap(GlyphRenderer::GraphicParameters::defaultLineCap);

    pID2D1DCRenderTarget -> SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    return;
    }
