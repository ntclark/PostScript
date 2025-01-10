#pragma once

#include <list>

#include "GlyphRenderer_i.h"
#include "FontManager_i.h"

#include "font.h"

class font;

    class FontManager : public IFontManager {
    public:

        FontManager(IUnknown *pIUnkOuter);
        ~FontManager();

        //   IUnknown

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
        STDMETHOD_ (ULONG, AddRef)();
        STDMETHOD_ (ULONG, Release)();

    private:

        // FontManager

        STDMETHOD(SeekFont)(char *pszFontFamily,long dupCount,IFont_EVNSW **ppIFont);
        STDMETHOD(LoadFont)(char *pszFontFamily,UINT_PTR cookie,IFont_EVNSW **ppIFont);
        STDMETHOD(DuplicateFont)(IFont_EVNSW *pIFont,UINT_PTR cookie,IFont_EVNSW **ppIFont);

        STDMETHOD(RenderGlyph)(HDC hdc,BYTE bGlyph,
                                UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,
                                IGlyphRenderer *pIGlyphRenderer,
                                POINTF *pStartPoint,POINTF *pEndPoint);

        STDMETHOD(ScaleFont)(FLOAT scaleFactor);

        STDMETHOD(put_PointSize)(FLOAT pointSize);

        STDMETHOD(get_CurrentFont)(IFont_EVNSW **ppIFont);
        STDMETHOD(put_CurrentFont)(IFont_EVNSW *pIFont);

        HRESULT drawType3Glyph(HDC,BYTE,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,IGlyphRenderer *pIGlyphRenderer,POINTF *pStartPoint,POINTF *pEndPoint);

        HRESULT drawType42Glyph(HDC,BYTE,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,IGlyphRenderer *pIGlyphRenderer,POINTF *pStartPoint,POINTF *pEndPoint);

        ULONG refCount{0};

        POINT currentPoint{0,0};

        static IFont_EVNSW *pIFont_Current;
        static IGraphicElements *pIGraphicElements_External;

        static std::list<font *> managedFonts;

    };

#ifdef DEFINE_DATA

    HMODULE hModule;
    wchar_t wstrModuleName[1024];
    char szModuleName[1024];

#else

    extern HMODULE hModule;
    extern wchar_t wstrModuleName[];
    extern char szModuleName[];

#endif
