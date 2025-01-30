
#define DEFINE_DATA

#include "EnVisioNateSW_FontManager.h"

#include "FontManager_i.c"

#include "Renderer_i.c"

    IRenderer *FontManager::pIRenderer = NULL;
    //IRenderer *FontManager::TextRenderer::pIRenderer_text = NULL;

    //IRenderer *FontManager::Type3GraphicElements::pIRenderer_type3 = NULL;

    IGraphicElements *FontManager::pIGraphicElements_Type42 = NULL;

    //IGraphicElements *FontManager::Type3GraphicElements::pIGraphicElements_type3 = NULL;
    //IGraphicElements *FontManager::TextRenderer::pIGraphicElements_text = NULL;

    IFont_EVNSW *FontManager::pIFont_Current = NULL;
    std::list<font *> FontManager::managedFonts;

    FontManager::FontManager(IUnknown *pUnkOuter) {
    if ( NULL == pIRenderer ) {
        CoCreateInstance(CLSID_Renderer,NULL,CLSCTX_ALL,IID_IRenderer,reinterpret_cast<void **>(&pIRenderer));
        pIRenderer -> QueryInterface(IID_IGraphicElements,reinterpret_cast<void **>(&pIGraphicElements_Type42));
    }
    return;
    }


    FontManager::~FontManager() {
    for ( font *pFont : managedFonts)
        delete pFont;
    return;
    }
