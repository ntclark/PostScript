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

#pragma once

#include <windows.h>
#include <windowsx.h>

#include <olectl.h>
#include <ShlObj.h>
#include <stdio.h>
#include <float.h>
#include <RichEdit.h>
#include <process.h>

#include <map>
#include <list>
#include <queue>

#include "utilities.h"

#include "PostScriptInterpreter_i.h"
#include "FontManager_i.h"
#include "Renderer_i.h"

#undef ADVANCE_THRU_EOL

#include "resourceIdentifiers.h"

#define PS_VERSION   3000L
#define PRODUCT_NAME "EnVisioNateSW PostScript Interpreter"

#define POINTS_TO_PIXELS    2
#define CMD_PANE_HEIGHT     96
#define SPLITTER_WIDTH      6
#define NOMINAL_GUTTER      32

class job;

    class PostScriptInterpreter : public IPostScriptInterpreter {

    public:

        PostScriptInterpreter(IUnknown *pUnknownOuter);
        ~PostScriptInterpreter();

        static long queueLog(boolean isPostScriptLog,char *pszOutput,char *pEnd = NULL,bool isError = false);
        static long clearLog(HWND);
        static long settleLog(HWND);

        //   IUnknown

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
        STDMETHOD_ (ULONG, AddRef)();
        STDMETHOD_ (ULONG, Release)();

        // IPostScript

        STDMETHOD(get_LineNumberGeneration)(BOOL *pIsDoingLineNumberGeneration);
        STDMETHOD(put_LineNumberGeneration)(BOOL doLineNumberGeneration);

        STDMETHOD(SetSource)(char *pszFileName);
        STDMETHOD(Parse)(char *pszFileName = NULL,BOOL autoStart = TRUE);
        STDMETHOD(ParseText)(char *pszPDFText);
        STDMETHOD(GetLastError)(char **ppszError);
        STDMETHOD(LogLevel)(logLevel theLogLevel);
        STDMETHOD(RendererLogLevel)(logLevel theLogLevel);

        STDMETHOD(GetPeristableProperties)(UINT_PTR *pProperties,long *pSize);
        STDMETHOD(SetPeristableProperties)(UINT_PTR pProperties);

        STDMETHOD(SetNoGraphicsRendering)();

        // IOleObject 

        class _IOleObject : public IOleObject {
        public:

            _IOleObject(PostScriptInterpreter *pp) : pParent(pp) {};
            ~_IOleObject() {};

            STDMETHOD(QueryInterface)(REFIID riid,void **ppv);
            STDMETHOD_ (ULONG, AddRef)();
            STDMETHOD_ (ULONG, Release)();

        private:

            STDMETHOD(SetClientSite)(IOleClientSite *pClientSite);
            STDMETHOD(GetClientSite)(IOleClientSite **ppCLientSite);
            STDMETHOD(SetHostNames)(LPCOLESTR szContainerApp,LPCOLESTR szContainerObj);
            STDMETHOD(Close)(DWORD dwSaveOption);
            STDMETHOD(SetMoniker)(DWORD dwWhichMoniker, IMoniker *pmk);
            STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker,IMoniker **ppmk);
            STDMETHOD(InitFromData)(IDataObject *pDataObject,BOOL fCreation,DWORD dwReserved);
            STDMETHOD(GetClipboardData)(DWORD dwReserved,IDataObject **ppDataObject);
            STDMETHOD(DoVerb)(LONG iVerb,LPMSG lpmsg,IOleClientSite *pActiveSite,LONG lindex,HWND hwndParent,LPCRECT lprcPosRect);
            STDMETHOD(EnumVerbs)(IEnumOLEVERB **ppenumOleVerb);
            STDMETHOD(Update)();
            STDMETHOD(IsUpToDate)();
            STDMETHOD(GetUserClassID)(CLSID * pClsid);
            STDMETHOD(GetUserType)(DWORD dwFormOfType, LPOLESTR *pszUserType);
            STDMETHOD(SetExtent)(DWORD dwDrawAspect, LPSIZEL lpsizel);
            STDMETHOD(GetExtent)(DWORD dwDrawAspect, LPSIZEL lpsizel);
        
            STDMETHOD(Advise)(IAdviseSink *pAdvSink, DWORD * pdwConnection);
            STDMETHOD(Unadvise)(DWORD dwConnection);
            STDMETHOD(EnumAdvise)(IEnumSTATDATA **ppenumAdvise);
            STDMETHOD(GetMiscStatus)(DWORD dwAspect, DWORD * pdwStatus);        
        
            STDMETHOD(SetColorScheme)(LPLOGPALETTE lpLogpal);
   
            PostScriptInterpreter *pParent{NULL};

        } *pIOleObject{NULL};

        // IOleInPlaceObject

        class _IOleInPlaceObject : public IOleInPlaceObject {
        public:

            _IOleInPlaceObject(PostScriptInterpreter *pp) : pParent(pp) {};
            ~_IOleInPlaceObject() {};

            STDMETHOD(QueryInterface)(REFIID riid,void **ppv);
            STDMETHOD_ (ULONG, AddRef)();
            STDMETHOD_ (ULONG, Release)();

            // IOleWindow
   
            STDMETHOD (GetWindow)(HWND *);
            STDMETHOD (ContextSensitiveHelp)(BOOL);

            // IOleInPlaceObject

            STDMETHOD (InPlaceActivate)();
            STDMETHOD (InPlaceDeactivate)();
            STDMETHOD (UIDeactivate)();
            STDMETHOD (SetObjectRects)(LPCRECT,LPCRECT);
            STDMETHOD (ReactivateAndUndo)();

        private:

            PostScriptInterpreter *pParent{NULL};

        } *pIOleInPlaceObject{NULL};

        // IConnectionPointContainer

        struct _IConnectionPointContainer : public IConnectionPointContainer {
        public:

            _IConnectionPointContainer(PostScriptInterpreter *pp) : pParent(pp) {};
            ~_IConnectionPointContainer() {};

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
        STDMETHOD_ (ULONG, AddRef)();
        STDMETHOD_ (ULONG, Release)();

        STDMETHOD(FindConnectionPoint)(REFIID riid,IConnectionPoint **);
        STDMETHOD(EnumConnectionPoints)(IEnumConnectionPoints **);

        void fire_RenderChar(POINT *pPointStartPDF,POINT *pPointEndPDF,char theCar);
        void fire_RenderString(POINT *pPointStartPDF,POINT *pPointEndPDF,char *pszString);
        void fire_PageShown(POINT *pPageDimensionsPDF);
        void fire_ErrorNotification(char *pszErrorString);

        private:

            PostScriptInterpreter *pParent{NULL};

        } *pIConnectionPointContainer{NULL};

        struct _IConnectionPoint : IConnectionPoint {
        public:

            _IConnectionPoint(PostScriptInterpreter *pp,REFIID eventInterfaceId);
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

            REFIID myEventInterface{IID_NULL};
            IUnknown *adviseSink{NULL};
            PostScriptInterpreter *pParent{NULL};
            DWORD nextCookie{0L};
            int countConnections{0};
            int countLiveConnections{0};

            CONNECTDATA *connections{NULL};

        } *pIConnectionPoints[2]{NULL,NULL};

        struct _IEnumConnectionPoints : IEnumConnectionPoints {
        public:

            _IEnumConnectionPoints(PostScriptInterpreter *pp,_IConnectionPoint **cp,int connectionPointCount);
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
        PostScriptInterpreter *pParent{NULL};
        _IConnectionPoint **connectionPoints{NULL};

        } *pIEnumConnectionPoints{NULL};

        struct _IEnumerateConnections : public IEnumConnections {
        public:

        _IEnumerateConnections(IUnknown* pParentUnknown,ULONG cConnections,CONNECTDATA* paConnections,ULONG initialIndex);
        ~_IEnumerateConnections();

            STDMETHODIMP         QueryInterface(REFIID, void **);
            STDMETHODIMP_(ULONG) AddRef();
            STDMETHODIMP_(ULONG) Release();
            STDMETHODIMP         Next(ULONG, CONNECTDATA*, ULONG*);
            STDMETHODIMP         Skip(ULONG);
            STDMETHODIMP         Reset();
            STDMETHODIMP         Clone(IEnumConnections**);

        private:

        ULONG refCount{0L};
        IUnknown *pParentUnknown{NULL};
        ULONG enumeratorIndex{0L};
        ULONG countConnections{0L};
        CONNECTDATA *connections{NULL};

        } *pIEnumerateConnections{NULL};

        struct _IRendererNotifications : IRendererNotifications {
        public:

            _IRendererNotifications(PostScriptInterpreter *pp,IRenderer *pIRenderer) : pParent(pp) {
                HRESULT rc = pIRenderer -> QueryInterface(IID_IConnectionPointContainer,
                                    reinterpret_cast<void **>(&pIConnectionPointContainer));
                rc = pIConnectionPointContainer -> FindConnectionPoint(IID_IRendererNotifications,&pIConnectionPoint);
                rc = pIConnectionPoint -> Advise(static_cast<IUnknown *>(this),&dwConnectionCookie);
            }

            ~_IRendererNotifications() {
                pIConnectionPoint -> Unadvise(dwConnectionCookie);
                pIConnectionPoint -> Release();
                pIConnectionPointContainer -> Release();
            }

            //   IUnknown

            STDMETHOD (QueryInterface)(REFIID riid,void **ppv) {
                if ( ! ppv )
                    return E_POINTER;
                if ( IID_IUnknown == riid )
                    *ppv = static_cast<IUnknown *>(this);
                else if ( IID_IRendererNotifications == riid )
                    *ppv = static_cast<IRendererNotifications *>(this);
                else
                    return E_NOINTERFACE;
                return S_OK;
            }
            STDMETHOD_ (ULONG, AddRef)() { return 1; }
            STDMETHOD_ (ULONG, Release)() { return 1; }

            // IRendererNotifications

            HRESULT __stdcall ErrorNotification(UINT_PTR theError) {
                pParent -> queueLog(false,(char *)theError,NULL,true);
                return S_OK;
            }

            HRESULT __stdcall StatusNotification(UINT_PTR theStatus) {
                //OutputDebugStringA("Renderer Status\n");
                //OutputDebugStringA((char *)theStatus);
                //OutputDebugStringA("\n");
                return S_OK;
            }

            HRESULT __stdcall LogNotification(UINT_PTR theLogEntry) {
                pParent -> queueLog(false,(char *)theLogEntry,NULL,false);
                return S_OK;
            }

            HRESULT __stdcall Clear() {
                return S_OK;
            }

            IConnectionPointContainer *pIConnectionPointContainer{NULL};
            IConnectionPoint *pIConnectionPoint{NULL};
            DWORD dwConnectionCookie{0L};

        private:

            PostScriptInterpreter *pParent{NULL};

        } *pIRendererNotifications{NULL};

        // IDropTarget

        class _IDropTarget : IDropTarget {
        public:

            _IDropTarget(PostScriptInterpreter *pp) : pParent(pp) {}

            // IUnknown 

            STDMETHOD(QueryInterface)(REFIID riid,void **ppv);
            STDMETHOD_ (ULONG, AddRef)() { return 1UL; }
            STDMETHOD_ (ULONG, Release)() { return 1UL; }

            // IDropTarget

            STDMETHOD(DragEnter)(IDataObject *pDataObj,DWORD  grfKeyState,POINTL pt,DWORD *pdwEffect);
            STDMETHOD(DragLeave)();
            STDMETHOD(DragOver)(DWORD  grfKeyState,POINTL pt,DWORD  *pdwEffect);
            STDMETHOD(Drop)(IDataObject *pDataObj,DWORD grfKeyState,POINTL pt,DWORD *pdwEffect);

        private:
    
            PostScriptInterpreter *pParent{NULL};

        } *pIDropTarget{NULL};;

        static CRITICAL_SECTION theQueueCriticalSection;

        HWND HwndClient() { return hwndClient; }

        boolean IsNoGraphicsRendering() { return noGraphicsRendering; }

        HDC GetDC();
        void ReleaseDC();

        void CommitCurrentPage(long pageWidthPoints,long pageHeightPoints);

        static long ClientHeight() { return cyClientHeight; }
        static long ClientWidth() { return cxClientWidth; }

        job *currentJob() { return pJob; }

        POINTL activePageOrigin{0L,0L};

        static long SideGutter(long v = -LONG_MAX) { 
            if ( -LONG_MAX == v ) 
                return sideGutter; 
            sideGutter = v; 
            return v; 
        }

        static long TopGutter(long v = -LONG_MAX) { 
            if ( -LONG_MAX == v ) 
                return topGutter; 
            topGutter = v;
            return v; 
        }

        std::map<size_t,HBITMAP> pageBitmaps;

        static char szErrorMessage[1024];

        static IFontManager *pIFontManager;
        static IRenderer *pIRenderer;
        static IGraphicElements *pIGraphicElements;
        static IGraphicParameters *pIGraphicParameters;

        static std::function<void(void)> beginPathAction;
        static std::list<std::function<void(void)>> endRunActions;

        static HWND hwndPostScriptLogContent;
        static HWND hwndRendererLogContent;

        static enum logLevel theLogLevel;
        static enum logLevel theRendererLogLevel;

    private:

        int initWindows();

        void toggleLogVisibility(long itemId);

        void cycle();

        IOleClientSite *pIOleClientSite{NULL};
        IOleInPlaceSite *pIOleInPlaceSite{NULL};

        static job *pJob;

        long refCount{0L};

        BYTE persistablePropertiesStart{0x00};

        long savedPropertiesSize{0L};
        long postScriptLogPaneWidth{256L};
        long rendererLogPaneWidth{256L};
        boolean logIsVisible{false};
        boolean rendererLogIsVisible{false};
        boolean noGraphicsRendering{false};
        boolean generateLineNumbers{false};

        char szCurrentPostScriptFile[MAX_PATH];
        char szWorkingFile[MAX_PATH];

        long persistLogLevel{0};
        long persistRendererLogLevel{0};

        BYTE persistablePropertiesEnd{0x00};

        long pageNumber{0};

        std::map<size_t,SIZEL *> pageSizes;

        static HWND hwndHost;
        static HWND hwndClient;
        static HWND hwndCmdPane;

        static HWND hwndPostScriptLogCmdPane;
        static HWND hwndPostScriptLogSplitter;

        static HWND hwndRendererLogCmdPane;
        static HWND hwndRendererLogSplitter;

        static HWND hwndVScroll;

        static HANDLE hsemSized;

        static long sideGutter;
        static long topGutter;

        static HDC hdcSurface;

        static long cyClientHeight;
        static long cxClientWidth;

        static void setWindowPanes(RECT *prcClient);

        static LRESULT CALLBACK clientWindowHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
        static LRESULT CALLBACK cmdPaneHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
        static LRESULT CALLBACK logPaneHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
        static LRESULT CALLBACK splitterHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

        static DWORD __stdcall processLog(DWORD_PTR dwCookie,BYTE *pBuffer,LONG bufferSize,LONG *pBytesReturned);

    };

#define WHITE_SPACE "[]<> ()\t\x0A\x0D/"

#define ARRAY_START_DELIMITERS "[ "
#define ARRAY_ITEM_DELIMITERS "\x0A\x0D "
#define ARRAY_END_DELIMITERS "\x0A\x0D ]"

#define PIXELS_TO_HIMETRIC(x,ppli)  ( (2540*(x) + ((ppli) >> 1)) / (ppli) )
#define HIMETRIC_TO_PIXELS(x,ppli)  ( ((ppli)*(x) + 1270) / 2540 )

#define EOL "\x0A\x0D"

#ifdef DEFINE_DATA

    PostScriptInterpreter *pPostScriptInterpreter = NULL;

    HMODULE hModule = NULL;
    char szModuleName[MAX_PATH];
    char szApplicationDataDirectory[MAX_PATH];
    char szUserDirectory[MAX_PATH];

    EDITSTREAM logStream;
    EDITSTREAM rendererLogStream;

    char psCollectionDelimiters[][8] = { "[","]","<<",">>","{","}",0};
    long psCollectionDelimiterLength[] = { 1,  1,   2,   2,  1,  1,0};

    char psDelimiters[][8] =   {"%%","%","(","//","/","<","<~",0};
    long psDelimiterLength[] = {  2,  1,  1,   2,  1,  1,   2,0};

    char psOtherDelimiters[][8] = {"]",0};
    char pszDelimiters[] = {"%%%(/<~{"};

#else

    extern PostScriptInterpreter *pPostScriptInterpreter;

    extern HMODULE hModule;
    extern char szModuleName[];
    extern char szApplicationDataDirectory[];
    extern char szUserDirectory[];

    extern EDITSTREAM logStream;
    extern EDITSTREAM rendererLogStream;

    extern char psCollectionDelimiters[][8];
    extern long psCollectionDelimiterLength[];
    extern char psDelimiters[][8];
    extern long psDelimiterLength[];
    extern char psOtherDelimiters[][8];
    extern char pszDelimiters[];

#endif
