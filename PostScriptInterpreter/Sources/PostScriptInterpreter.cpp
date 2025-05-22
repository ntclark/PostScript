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

#include "PostScriptInterpreter.h"
#include "job.h"

    IFontManager *PostScriptInterpreter::pIFontManager = NULL;
    IRenderer *PostScriptInterpreter::pIRenderer = NULL;
    IGraphicElements *PostScriptInterpreter::pIGraphicElements = NULL;
    IGraphicParameters *PostScriptInterpreter::pIGraphicParameters = NULL;

    std::function<void(void)> PostScriptInterpreter::beginPathAction;
    std::list<std::function<void(void)>> PostScriptInterpreter::endRunActions;

    CRITICAL_SECTION PostScriptInterpreter::theQueueCriticalSection;

    HWND PostScriptInterpreter::hwndHost = NULL;
    HWND PostScriptInterpreter::hwndClient = NULL;
    HWND PostScriptInterpreter::hwndCmdPane = NULL;
    HWND PostScriptInterpreter::hwndVScroll = NULL;

    HWND PostScriptInterpreter::hwndPostScriptLogCmdPane = NULL;
    HWND PostScriptInterpreter::hwndPostScriptLogContent = NULL;
    HWND PostScriptInterpreter::hwndPostScriptLogSplitter = NULL;

    HWND PostScriptInterpreter::hwndRendererLogCmdPane = NULL;
    HWND PostScriptInterpreter::hwndRendererLogContent = NULL;
    HWND PostScriptInterpreter::hwndRendererLogSplitter = NULL;

    HANDLE PostScriptInterpreter::hsemSized = INVALID_HANDLE_VALUE;

    long PostScriptInterpreter::sideGutter = NOMINAL_GUTTER;
    long PostScriptInterpreter::topGutter = NOMINAL_GUTTER;

    HDC PostScriptInterpreter::hdcSurface = NULL;

    job *PostScriptInterpreter::pJob = NULL;
    enum logLevel PostScriptInterpreter::theLogLevel = none;
    enum logLevel PostScriptInterpreter::theRendererLogLevel = none;

    long PostScriptInterpreter::cyClientWindow = -1L;
    long PostScriptInterpreter::cxClientWindow = -1L;

    char PostScriptInterpreter::szErrorMessage[1024];

    char PStoPDFException::szMessage[2048];


    PostScriptInterpreter::PostScriptInterpreter(IUnknown *pUnknownOuter) {

    pPostScriptInterpreter = this;

    pIOleObject = new _IOleObject(this);
    pIOleInPlaceObject = new _IOleInPlaceObject(this);

    pIConnectionPointContainer = new _IConnectionPointContainer(this);
    pIConnectionPoints[0] = new _IConnectionPoint(this,IID_IPostScriptInterpreterEvents);
    pIConnectionPoints[1] = new _IConnectionPoint(this,IID_IRendererNotifications);

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_TAB_CLASSES | ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS;
    InitCommonControlsEx(&icex);
    LoadLibrary("Msftedit.dll");

    InitializeCriticalSection(&theQueueCriticalSection);

    memset(szCurrentPostScriptFile,0,sizeof(szCurrentPostScriptFile));

    HRESULT rc = CoInitialize(NULL);

    CoCreateInstance(CLSID_FontManager,NULL,CLSCTX_ALL,IID_IFontManager,reinterpret_cast<void **>(&pIFontManager));
    pIFontManager -> QueryInterface(IID_IRenderer,reinterpret_cast<void **>(&pIRenderer));
    pIRenderer -> QueryInterface(IID_IGraphicElements,reinterpret_cast<void **>(&pIGraphicElements));
    pIRenderer -> QueryInterface(IID_IGraphicParameters,reinterpret_cast<void **>(&pIGraphicParameters));

    pIRendererNotifications = new _IRendererNotifications(this,pIRenderer);

    return;
    }


    PostScriptInterpreter::~PostScriptInterpreter() {

    if ( pJob )
        delete pJob;

    cycle();

    clearLog(hwndRendererLogContent);

    if ( ! ( NULL == pIFontManager ) ) {
        pIFontManager -> Release();
        pIRenderer -> Release();
        pIGraphicElements -> Release();
        pIGraphicParameters -> Release();
    }

    pIFontManager = NULL;
    pIRenderer = NULL;
    pIGraphicElements = NULL;
    pIGraphicParameters = NULL;

    if ( ! ( NULL == pIRendererNotifications ) )
        delete pIRendererNotifications;

    pIRendererNotifications = NULL;

    delete pIOleObject;
    delete pIOleInPlaceObject;
    delete pIConnectionPointContainer;
    delete pIConnectionPoints[0];
    delete pIConnectionPoints[1];

    if ( pIOleInPlaceSite )
        pIOleInPlaceSite -> Release();

    if ( pIOleClientSite )
        pIOleClientSite -> Release();

    if ( pIEnumConnectionPoints )
        pIEnumConnectionPoints -> Release();

    if ( pIEnumerateConnections )
        pIEnumerateConnections -> Release();

    if ( ! ( NULL == pIDropTarget ) )
        delete pIDropTarget;

    DestroyWindow(hwndClient);
    DestroyWindow(hwndCmdPane);

    DestroyWindow(hwndPostScriptLogContent);
    DestroyWindow(hwndPostScriptLogCmdPane);
    DestroyWindow(hwndPostScriptLogSplitter);

    DestroyWindow(hwndRendererLogContent);
    DestroyWindow(hwndRendererLogCmdPane);
    DestroyWindow(hwndRendererLogSplitter);

    return;
    }


    void PostScriptInterpreter::cycle() {

    ReleaseDC();

    for ( std::pair<size_t,HBITMAP> pPair : pageBitmaps )
        DeleteObject(pPair.second);

    pageBitmaps.clear();

    for ( std::pair<size_t,SIZEL *> pPair : pageSizes )
        delete [] pPair.second;

    pageSizes.clear();

    pIFontManager -> Reset();

    return;
    }