
#define DEFINE_DATA

#include "EnVisioNateSW_FontManager.h"

#include "FontManager_i.c"
#include "GlyphRenderer_i.c"

    IGraphicElements *FontManager::pIGraphicElements_External = NULL;
    IFont_EVNSW *FontManager::pIFont_Current = NULL;
    std::list<font *> FontManager::managedFonts;

    FontManager::FontManager(IUnknown *pUnkOuter) {
    return;
    }


    FontManager::~FontManager() {
    for ( font *pFont : managedFonts)
        delete pFont;
    return;
    }
