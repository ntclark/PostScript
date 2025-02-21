#pragma once

#include <list>

#include "Renderer_i.h"
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

        // FontManager

        STDMETHOD(SeekFont)(char *pszFontFamily,long dupCount,IFont_EVNSW **ppIFont);
        STDMETHOD(LoadFont)(char *pszFontFamily,UINT_PTR cookie,IFont_EVNSW **ppIFont);
        STDMETHOD(DuplicateFont)(IFont_EVNSW *pIFont,UINT_PTR cookie,IFont_EVNSW **ppIFont);

        STDMETHOD(RenderGlyph)(HDC hdc,unsigned short bGlyph,
                                UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,
                                POINTF *pStartPoint,POINTF *pEndPoint);

        STDMETHOD(ScaleFont)(FLOAT scaleFactor);

        STDMETHOD(put_PointSize)(FLOAT pointSize);

        STDMETHOD(get_CurrentFont)(IFont_EVNSW **ppIFont);
        STDMETHOD(put_CurrentFont)(IFont_EVNSW *pIFont);

    private:

        HRESULT drawType3Glyph(HDC,unsigned short,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,POINTF *pStartPoint,POINTF *pEndPoint);
        HRESULT drawType42Glyph(HDC,unsigned short,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,POINTF *pStartPoint,POINTF *pEndPoint);

        ULONG refCount{0};

        POINT currentPoint{0,0};

        static IFont_EVNSW *pIFont_Current;
        static IRenderer *pIRenderer;
        static IGraphicElements *pIGraphicElements;

        static std::list<font *> managedFonts;

        static char *translateFamily(char *pszFamilyName,char *pszReturnedQualifier = NULL);

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
