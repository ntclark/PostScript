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

        STDMETHOD(ChooseFont)(HDC hdc,IFont_EVNSW **ppIFont);

        STDMETHOD(RenderGlyph)(HDC hdc,unsigned short bGlyph,
                                UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,
                                POINTF *pStartPoint,POINTF *pEndPoint);

        STDMETHOD(ScaleFont)(FLOAT scaleFactor);

        STDMETHOD(put_PointSize)(FLOAT pointSize);

        STDMETHOD(get_CurrentFont)(IFont_EVNSW **ppIFont);
        STDMETHOD(put_CurrentFont)(IFont_EVNSW *pIFont);

        static char szFailureMessage[1024];

        static void FireErrorNotification(char *pszNotification) {
            pIConnectionPointContainer -> fire_ErrorNotification(pszNotification);
            return;
        }

    private:

        // IConnectionPointContainer

        struct _IConnectionPointContainer : public IConnectionPointContainer {
        public:

            _IConnectionPointContainer(FontManager *pp) : pParent(pp) {};
            ~_IConnectionPointContainer() {};

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
        STDMETHOD_ (ULONG, AddRef)();
        STDMETHOD_ (ULONG, Release)();

        STDMETHOD(FindConnectionPoint)(REFIID riid,IConnectionPoint **);
        STDMETHOD(EnumConnectionPoints)(IEnumConnectionPoints **);

        void fire_ErrorNotification(char *pszError);

        private:

            FontManager *pParent{NULL};

        };

        static _IConnectionPointContainer *pIConnectionPointContainer;

        // IConnectionPoint

        struct _IConnectionPoint : IConnectionPoint {
        public:

            _IConnectionPoint(FontManager *pp);
            ~_IConnectionPoint();

            STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
            STDMETHOD_ (ULONG, AddRef)();
            STDMETHOD_ (ULONG, Release)();

            STDMETHOD (GetConnectionInterface)(IID *);
            STDMETHOD (GetConnectionPointContainer)(IConnectionPointContainer **ppCPC);
            STDMETHOD (Advise)(IUnknown *pUnk,DWORD *pdwCookie);
            STDMETHOD (Unadvise)(DWORD);
            STDMETHOD (EnumConnections)(IEnumConnections **ppEnum);

            IUnknown *AdviseSink() { return adviseSink; };

        private:

            int getSlot();
            int findSlot(DWORD dwCookie);

            IUnknown *adviseSink{NULL};
            FontManager *pParent{NULL};
            DWORD nextCookie{0L};
            int countConnections{0};
            int countLiveConnections{0};

            CONNECTDATA *connections{NULL};

        } *pIConnectionPoint{NULL};

        // IEnumConnectionPoints

        struct _IEnumConnectionPoints : IEnumConnectionPoints {
        public:

            _IEnumConnectionPoints(FontManager *pp,_IConnectionPoint **cp,int connectionPointCount);
            ~_IEnumConnectionPoints();

            STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
            STDMETHOD_ (ULONG, AddRef)();
            STDMETHOD_ (ULONG, Release)();

            STDMETHOD (Next)(ULONG cConnections,IConnectionPoint **rgpcn,ULONG *pcFetched);
            STDMETHOD (Skip)(ULONG cConnections);
            STDMETHOD (Reset)();
            STDMETHOD (Clone)(IEnumConnectionPoints **);

        private:

        int cpCount{0};
        int enumeratorIndex{0};
        FontManager *pParent{NULL};
        _IConnectionPoint **connectionPoints{NULL};

        } *pIEnumConnectionPoints{NULL};

        // IEnumerateConnections

        struct _IEnumerateConnections : public IEnumConnections {
        public:

        _IEnumerateConnections(IUnknown* pParentUnknown,ULONG cConnections,CONNECTDATA* paConnections,ULONG initialIndex);
        ~_IEnumerateConnections();

            STDMETHODIMP QueryInterface(REFIID, void **);
            STDMETHODIMP_(ULONG) AddRef();
            STDMETHODIMP_(ULONG) Release();
            STDMETHODIMP Next(ULONG, CONNECTDATA*, ULONG*);
            STDMETHODIMP Skip(ULONG);
            STDMETHODIMP Reset();
            STDMETHODIMP Clone(IEnumConnections**);

        private:

        ULONG refCount{0L};
        IUnknown *pParentUnknown{NULL};
        ULONG enumeratorIndex{0L};
        ULONG countConnections{0L};
        CONNECTDATA *connections{NULL};

        } *pIEnumerateConnections{NULL};

        ULONG refCount{0};

        static IFont_EVNSW *pIFont_Current;
        static IRenderer *pIRenderer;
        static IGraphicElements *pIGraphicElements;

        static std::list<font *> managedFonts;

        static int CALLBACK enumFonts(const LOGFONT *pLogFont,const TEXTMETRIC *pTextMetric,DWORD FontType,LPARAM lParam);

        static char *translateFamily(char *pszFamilyName,char *pszReturnedQualifier = NULL);

        friend class font;

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
