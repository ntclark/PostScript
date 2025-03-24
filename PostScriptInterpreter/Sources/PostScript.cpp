
#include "PostScript.h"
#include "error.h"

#include "Properties_i.c"
#include "PostScriptInterpreter_i.c"
//#include "CVPostscriptConverter_i.c"

    CRITICAL_SECTION PStoPDF::theQueueCriticalSection;

    LRESULT (__stdcall *PStoPDF::nativeHostFrameHandler)(HWND,UINT,WPARAM,LPARAM) = NULL;
    LRESULT (__stdcall *PStoPDF::nativeRichEditHandler)(HWND,UINT,WPARAM,LPARAM) = NULL;

    HWND PStoPDF::hwndHost = NULL;
    HWND PStoPDF::hwndClient = NULL;
    HWND PStoPDF::hwndCmdPane = NULL;
    HWND PStoPDF::hwndLogPane = NULL;
    HWND PStoPDF::hwndLog = NULL;
    HWND PStoPDF::hwndLogSplitter = NULL;
    HWND PStoPDF::hwndOperandStackSize = NULL;
    HWND PStoPDF::hwndCurrentDictionary = NULL;
    HWND PStoPDF::hwndVScroll = NULL;

    HDC PStoPDF::hdcSurface = NULL;

    job *PStoPDF::pJob = NULL;
    enum logLevel PStoPDF::theLogLevel = none;

    long PStoPDF::initialCYClient = -1L;

    char PStoPDF::szErrorMessage[1024];

    char PStoPDFException::szMessage[2048];


    PStoPDF::PStoPDF(IUnknown *pUnknownOuter) {

    pPStoPDF = this;

    //theLogLevel = none;

    pIOleObject = new _IOleObject(this);
    pIOleInPlaceObject = new _IOleInPlaceObject(this);

    pIConnectionPointContainer = new _IConnectionPointContainer(this);
    pIConnectionPoint = new _IConnectionPoint(this);

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_TAB_CLASSES | ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS;
    InitCommonControlsEx(&icex);
    LoadLibrary("Msftedit.dll");

    InitializeCriticalSection(&theQueueCriticalSection);

    memset(szCurrentPostScriptFile,0,sizeof(szCurrentPostScriptFile));

    HRESULT rc = CoInitialize(NULL);

    CoCreateInstance(CLSID_InnoVisioNateProperties,NULL,CLSCTX_ALL,IID_IGProperties,reinterpret_cast<void **>(&pIGProperties));

    pIGProperties -> Add(L"log visible",NULL);
    pIGProperties -> DirectAccess(L"log visible",TYPE_BINARY,&logIsVisible,sizeof(logIsVisible));

    pIGProperties -> Add(L"log pane width",NULL);
    pIGProperties -> DirectAccess(L"log pane width",TYPE_BINARY,&logPaneWidth,sizeof(logPaneWidth));

    WCHAR szwDataFile[MAX_PATH];
    MultiByteToWideChar(CP_ACP,0,szApplicationDataDirectory,-1,szwDataFile,MAX_PATH);
    wcscat(szwDataFile,L"\\PStoPDF.settings");

    pIGProperties -> put_FileName(szwDataFile);

    VARIANT_BOOL bSuccess;

    pIGProperties -> LoadFile(&bSuccess);

    return;
    }


    PStoPDF::~PStoPDF() {

    delete pIOleObject;
    delete pIOleInPlaceObject;
    delete pIConnectionPointContainer;
    delete pIConnectionPoint;

    if ( pIOleInPlaceSite )
        pIOleInPlaceSite -> Release();

    if ( pIOleClientSite )
        pIOleClientSite -> Release();

    if ( pIEnumConnectionPoints )
        pIEnumConnectionPoints -> Release();

    if ( pIEnumerateConnections )
        pIEnumerateConnections -> Release();

    pIGProperties -> Save();
    pIGProperties -> Release();

#if 0
    if ( ! ( NULL == pICVPostscriptConverter ) ) 
        pICVPostscriptConverter -> Release();
#endif

    if ( ! ( NULL == PStoPDF::nativeHostFrameHandler ) )
        SetWindowLongPtr(GetParent(hwndHost),GWLP_WNDPROC,(ULONG_PTR)PStoPDF::nativeHostFrameHandler);

    DestroyWindow(hwndClient);
    DestroyWindow(hwndCmdPane);
    DestroyWindow(hwndLog);
    DestroyWindow(hwndLogPane);
    DestroyWindow(hwndLogSplitter);

    return;
    }
