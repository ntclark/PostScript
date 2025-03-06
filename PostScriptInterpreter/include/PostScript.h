
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

#include "PdfEnabler_i.h"
#include "pdfEnabler\Page.h"
#include "CVPostscriptConverter_i.h"

#ifdef GetWindowID
#undef GetWindowID
#endif

#include "Properties_i.h"
#include "PostScriptInterpreter_i.h"

#undef ADVANCE_THRU_EOL

#include "resourceIdentifiers.h"

#define DO_RTF

#define PS_VERSION   3000L
#define PRODUCT_NAME "EnVisioNateSW PostScript interpreter"

#define POINTS_TO_PIXELS    2
#define CMD_PANE_HEIGHT     96
#define LOG_PANE_HEIGHT     72
#define SPLITTER_WIDTH      12

class job;

// Implementation rules:
//
// When parsing - the parsing procedure will be called with the first byte following the delimiter, which could be white space
//
    class PStoPDF : public IPostScript {

    public:

        PStoPDF(IUnknown *pUnknownOuter);
        ~PStoPDF();

        static long queueLog(char *pszOutput,char *pEnd = NULL,bool isError = false);
        static long clearLog();
        static long settleLog();

        long displayStack();

        //   IUnknown

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
        STDMETHOD_ (ULONG, AddRef)();
        STDMETHOD_ (ULONG, Release)();

        // IPStoPDF

        STDMETHOD(SetSource)(char *pszFileName);

        STDMETHOD(Parse)(char *pszFileName = NULL);

        STDMETHOD(ParseText)(char *pszPDFText,long length,void *pvIPdfPage,void *pvIPostScriptTakeText,HDC hdc,RECT *prcWindowsClip);

        STDMETHOD(GetLastError)(char **ppszError);

        STDMETHOD(LogLevel)(logLevel theLogLevel);

        // IOleObject 

        class _IOleObject : public IOleObject {
        public:

            _IOleObject(PStoPDF *pp) : pParent(pp) {};
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
   
            PStoPDF *pParent{NULL};

        } *pIOleObject{NULL};

        // IOleInPlaceObject

        class _IOleInPlaceObject : public IOleInPlaceObject {
        public:

            _IOleInPlaceObject(PStoPDF *pp) : pParent(pp) {};
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

            PStoPDF *pParent{NULL};

        } *pIOleInPlaceObject{NULL};

        //      IConnectionPointContainer

        struct _IConnectionPointContainer : public IConnectionPointContainer {
        public:

            _IConnectionPointContainer(PStoPDF *pp) : pParent(pp) {};
            ~_IConnectionPointContainer() {};

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
        STDMETHOD_ (ULONG, AddRef)();
        STDMETHOD_ (ULONG, Release)();

        STDMETHOD(FindConnectionPoint)(REFIID riid,IConnectionPoint **);
        STDMETHOD(EnumConnectionPoints)(IEnumConnectionPoints **);

        void fire_RenderChar(POINT *pPoint,char theCar);
        void fire_RenderString(POINT *pPoint,char *pszString);

        private:

            PStoPDF *pParent{NULL};

        } *pIConnectionPointContainer{NULL};

        struct _IConnectionPoint : IConnectionPoint {
        public:

            _IConnectionPoint(PStoPDF *pp);
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
            PStoPDF *pParent{NULL};
            DWORD nextCookie{0L};
            int countConnections{0};
            int countLiveConnections{0};

            CONNECTDATA *connections{NULL};

        } *pIConnectionPoint{NULL};

        struct _IEnumConnectionPoints : IEnumConnectionPoints {
        public:

            _IEnumConnectionPoints(PStoPDF *pp,_IConnectionPoint **cp,int connectionPointCount);
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
        PStoPDF *pParent{NULL};
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

        static CRITICAL_SECTION theQueueCriticalSection;

        HWND HwndClient() { return hwndClient; }

        HDC GetDC();

        void CommitCurrentPage(long pageWidthPoints,long pageHeightPoints);

        void BeginPath();

        void PotentialNewPage();

        long InitialCYClient() { return initialCYClient; }

        job *currentJob() { return pJob; }

        POINTL activePageOrigin{0L,0L};

    private:

        int initWindows();

        void toggleLogVisibility();

        IGProperties *pIGProperties{NULL};
        IOleClientSite *pIOleClientSite{NULL};
        IOleInPlaceSite *pIOleInPlaceSite{NULL};

        ICVPostscriptConverter *pICVPostscriptConverter{NULL};

        static job *pJob;

        long refCount{0L};
        long logPaneWidth{256L};
        static enum logLevel theLogLevel;
        boolean logIsVisible{false};

        char szCurrentPostScriptFile[MAX_PATH];

        long pageNumber{0};

        std::map<size_t,HBITMAP> pageBitmaps;
        std::map<size_t,SIZEL *> pageSizes;

        std::function<void(void)> beginPathAction{NULL};

        static char szErrorMessage[1024];

        static HWND hwndHost;
        static HWND hwndClient;
        static HWND hwndCmdPane;
        static HWND hwndLogPane;
        static HWND hwndLog;
        static HWND hwndLogSplitter;
        static HWND hwndOperandStackSize;
        static HWND hwndCurrentDictionary;
        static HWND hwndVScroll;

        static HDC hdcSurface;

        static long initialCYClient;

        static LRESULT (__stdcall *nativeHostFrameHandler)(HWND,UINT,WPARAM,LPARAM);

        static LRESULT (__stdcall *nativeRichEditHandler)(HWND,UINT,WPARAM,LPARAM);

        static LRESULT CALLBACK handler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
        static LRESULT CALLBACK cmdPaneHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
        static LRESULT CALLBACK logPaneHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
        static LRESULT CALLBACK splitterHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

        static LRESULT CALLBACK hostFrameHandlerOveride(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

        static DWORD __stdcall processLog(DWORD_PTR dwCookie,BYTE *pBuffer,LONG bufferSize,LONG *pBytesReturned);

    };

#define PIXELS_TO_HIMETRIC(x,ppli)  ( (2540*(x) + ((ppli) >> 1)) / (ppli) )
#define HIMETRIC_TO_PIXELS(x,ppli)  ( ((ppli)*(x) + 1270) / 2540 )

#define EOL "\x0A\x0D"

#define SYNCHRONOUS_CALL(theWindow,MSG,ARGUMENT) {                      \
   HANDLE hSemaphore = CreateSemaphore(NULL,0,1,NULL);                  \
   PostMessage((theWindow),(MSG),(WPARAM)(ARGUMENT),(LPARAM)hSemaphore);\
   WaitForSingleObject(hSemaphore,INFINITE);                            \
   CloseHandle(hSemaphore);                                             \
   }

#define SET_PANES   {                       \
    RECT rcHost;                            \
    GetWindowRect(hwndHost,&rcHost);        \
    long cx = rcHost.right - rcHost.left;   \
    long cy = rcHost.bottom - rcHost.top;   \
    long cxClient = cx - (pPStoPDF -> logIsVisible ? pPStoPDF -> logPaneWidth : 0);                     \
    SetWindowPos(hwndClient,HWND_TOP,0,CMD_PANE_HEIGHT,cxClient,cy - CMD_PANE_HEIGHT,0L);               \
    SetWindowPos(hwndCmdPane,HWND_TOP,rcHost.left,rcHost.top,cxClient,CMD_PANE_HEIGHT,SWP_SHOWWINDOW);  \
    if ( 0 < pPStoPDF -> logPaneWidth && pPStoPDF -> logIsVisible ) {                                   \
        SetWindowPos(hwndLog,HWND_TOP,cxClient,LOG_PANE_HEIGHT,pPStoPDF -> logPaneWidth,cy - LOG_PANE_HEIGHT,SWP_SHOWWINDOW);           \
        SetWindowPos(hwndLogPane,HWND_TOP,rcHost.left + cxClient,rcHost.top,pPStoPDF -> logPaneWidth,LOG_PANE_HEIGHT,SWP_SHOWWINDOW);   \
        SetWindowPos(hwndLogSplitter,HWND_TOP,cxClient - SPLITTER_WIDTH / 2,0,SPLITTER_WIDTH,cy,SWP_SHOWWINDOW);\
        SetWindowText(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_LOG_SHOW),"Log >>");                         \
    } else {                                                                                            \
        ShowWindow(hwndLog,SW_HIDE);                                                                    \
        ShowWindow(hwndLogPane,SW_HIDE);                                                                \
        ShowWindow(hwndLogSplitter,SW_HIDE);                                                            \
        SetWindowText(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_LOG_SHOW),"<< Log");                         \
    }   \
    }

#ifdef DEFINE_DATA

   PStoPDF *pPStoPDF = NULL;

   HMODULE hModule = NULL;
   char szModuleName[MAX_PATH];
   char szApplicationDataDirectory[MAX_PATH];
   char szUserDirectory[MAX_PATH];

   EDITSTREAM logStream;

   char psCollectionDelimiters[][8] = { "[","]","<<",">>","{","}",0};
   long psCollectionDelimiterLength[] = { 1,  1,   2,   2,  1,  1,0};

   char psDelimiters[][8] =   {"%%","%","(","/","<","<~",0};
   long psDelimiterLength[] = {  2,  1,  1,  1,  1,   2,0};

   char psOtherDelimiters[][8] = {"]",0};
   char pszDelimiters[] = {"%%%(/<~{"};

#else

   extern PStoPDF *pPStoPDF;

   extern HMODULE hModule;
   extern char szModuleName[];
   extern char szApplicationDataDirectory[];
   extern char szUserDirectory[];

   extern EDITSTREAM logStream;

   extern char psCollectionDelimiters[][8];
   extern long psCollectionDelimiterLength[];
   extern char psDelimiters[][8];
   extern long psDelimiterLength[];
   extern char psOtherDelimiters[][8];
   extern char pszDelimiters[];

#endif
