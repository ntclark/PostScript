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

    private:

        // FontManager

        STDMETHOD(SeekFont)(char *pszFontFamily,long dupCount,IFont_EVNSW **ppIFont);
        STDMETHOD(LoadFont)(char *pszFontFamily,UINT_PTR cookie,IFont_EVNSW **ppIFont);
        STDMETHOD(DuplicateFont)(IFont_EVNSW *pIFont,UINT_PTR cookie,IFont_EVNSW **ppIFont);

        STDMETHOD(RenderGlyph)(HDC hdc,BYTE bGlyph,
                                UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,
                                POINTF *pStartPoint,POINTF *pEndPoint);

        STDMETHOD(ScaleFont)(FLOAT scaleFactor);

        STDMETHOD(put_PointSize)(FLOAT pointSize);

        STDMETHOD(get_CurrentFont)(IFont_EVNSW **ppIFont);
        STDMETHOD(put_CurrentFont)(IFont_EVNSW *pIFont);

#if 0
        class TextRenderer : public IUnknown {
        public:

            TextRenderer(FontManager *pp) : pParent(pp) {
                if ( NULL == pIRenderer_text )
                    CoCreateInstance(CLSID_Renderer,NULL,CLSCTX_ALL,IID_IRenderer,reinterpret_cast<void **>(&pIRenderer_text));
            }
            //   IUnknown

            STDMETHOD (QueryInterface)(REFIID refIID,void **ppv) {
                if ( ! ppv )
                    return E_POINTER;
                if ( IID_IUnknown == refIID ) {
                    *ppv = static_cast<IUnknown *>(this);
                    return S_OK;
                }
                HRESULT rc = pIRenderer_text -> QueryInterface(refIID,ppv);
                if ( S_OK == rc )
                    return S_OK;
                return pParent -> QueryInterface(refIID,ppv);

            }
            STDMETHOD_ (ULONG, AddRef)() { return 0; } 
            STDMETHOD_ (ULONG, Release)() { return 0; }

        private:

            FontManager *pParent{NULL};

            static IRenderer *pIRenderer_text;

        } *pTextRenderer = NULL;
#endif

#if 0
        class Type3GraphicElements : public IGraphicElements {
        public:

            Type3GraphicElements(FontManager *pp) : pParent(pp) {
                if ( NULL == pIRenderer_type3 ) {
                    CoCreateInstance(CLSID_Renderer,NULL,CLSCTX_ALL,IID_IRenderer,reinterpret_cast<void **>(&pIRenderer_type3));
                    pIRenderer -> QueryInterface(IID_IGraphicElements,reinterpret_cast<void **>(&pIGraphicElements_type3));
                }
            }

            //   IUnknown

            STDMETHOD (QueryInterface)(REFIID refIID,void **ppv) {
                if ( ! ppv )
                    return E_POINTER;
                if ( IID_IUnknown == refIID ) 
                    *ppv = static_cast<IUnknown *>(this);
                else if ( refIID == IID_IGraphicElements ) 
                    *ppv = static_cast<IGraphicElements *>(this);
                if ( NULL == *ppv )
                    return pParent -> QueryInterface(refIID,ppv);
                return S_OK;
            }
            STDMETHOD_ (ULONG, AddRef)() { return 0; } 
            STDMETHOD_ (ULONG, Release)() { return 0; }

            STDMETHOD(NewPath)() { return pIGraphicElements_type3 -> NewPath(); }
            STDMETHOD(ClosePath)() { return pIGraphicElements_type3 -> ClosePath(); }
            STDMETHOD(StrokePath)() { return pIGraphicElements_type3 -> StrokePath(); }
            STDMETHOD(FillPath)() { return pIGraphicElements_type3 -> FillPath(); }

            STDMETHOD(MoveTo)(FLOAT x,FLOAT y) { return pIGraphicElements_type3 -> MoveTo(x,y); }
            STDMETHOD(MoveToRelative)(FLOAT x,FLOAT y) { return pIGraphicElements_type3 -> MoveToRelative(x,y); }
            STDMETHOD(LineTo)(FLOAT x,FLOAT y) { return pIGraphicElements_type3 -> LineTo(x,y); }
            STDMETHOD(LineToRelative)(FLOAT x,FLOAT y) { return pIGraphicElements_type3 -> LineToRelative(x,y); }
            STDMETHOD(ArcTo)(FLOAT xCenter,FLOAT yCenter,FLOAT radius,FLOAT angle1,FLOAT angle2){ return pIGraphicElements_type3 -> ArcTo(xCenter,yCenter,radius,angle1,angle2); }
            STDMETHOD(CubicBezierTo)(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2,FLOAT x3,FLOAT y3) { return pIGraphicElements_type3 -> CubicBezierTo(x1,y1,x2,y2,x3,y3); }
            STDMETHOD(QuadraticBezierTo)(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2) { return pIGraphicElements_type3 -> QuadraticBezierTo(x1,y1,x2,y2); }

            STDMETHOD(Image)(HBITMAP hbmImage,UINT_PTR /*xForm*/ pPSCurrentCTM,FLOAT width,FLOAT height) { return pIGraphicElements_type3 -> Image(hbmImage,pPSCurrentCTM,width,height); }

            FontManager *pParent{NULL};

            static IRenderer *pIRenderer_type3;
            static IGraphicElements *pIGraphicElements_type3;

        } *pIGraphicElements_Type3{NULL};
#endif

        HRESULT drawType3Glyph(HDC,BYTE,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,POINTF *pStartPoint,POINTF *pEndPoint);
        HRESULT drawType42Glyph(HDC,BYTE,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,POINTF *pStartPoint,POINTF *pEndPoint);

        ULONG refCount{0};

        POINT currentPoint{0,0};

        static IFont_EVNSW *pIFont_Current;
        static IRenderer *pIRenderer;
        static IGraphicElements *pIGraphicElements_Type42;

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
