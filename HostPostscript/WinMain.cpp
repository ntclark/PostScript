
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "HostPostscript.h"

#include "PostScriptInterpreter_i.c"

#if USE_GS_PROPERTIES
#include "Properties_i.c"
IGProperties *pIGProperties = NULL;
#endif

void adjustToMonitorSize(RECT *);

char szApplicationDataDirectory[MAX_PATH];

RECT rcFrame{512,512,1024,768};

BYTE postScriptPropertiesBlob[512];
long cbPostScriptProperties = 512;

boolean useGSProperties = true;

    int __stdcall WinMain(HINSTANCE hModule,HINSTANCE hInstancePrevious,LPSTR lpCmdLine,int nCmdShow) {

    char *pCommandLine = GetCommandLine();
    char **argv;
    int argc = 0;

    long n = (DWORD)strlen(pCommandLine) + 1;
    BSTR bstrCommandLine = SysAllocStringLen(GetCommandLineW(),n);
    BSTR *pArgs = CommandLineToArgvW(bstrCommandLine,&argc);
    argv = new char *[argc];

    for ( long k = 0; k < argc; k++ ) {
        argv[k] = new char[wcslen(pArgs[k]) + 1];
        memset(argv[k],0,(wcslen(pArgs[k]) + 1) * sizeof(char));
        WideCharToMultiByte(CP_ACP,0,pArgs[k],-1,argv[k],(DWORD)wcslen(pArgs[k]),0,0);
    }

    // 
    // This is apparantly an undocumented hack to allow
    // Drag/Drop in a multi-threaded COM environment
    //
    if ( S_OK == OleInitialize(0) )
        CoUninitialize();

    HRESULT rc = CoInitializeEx(NULL, COINIT_MULTITHREADED);

#if USE_GS_PROPERTIES

    char szTemp[MAX_PATH];

    GetLocation(NULL,CSIDL_COMMON_APPDATA,szTemp);

    sprintf(szApplicationDataDirectory,"%s\\EnVisioNateSW",szTemp);

    rc = CoCreateInstance(CLSID_InnoVisioNateProperties,NULL,CLSCTX_ALL,IID_IGProperties,reinterpret_cast<void **>(&pIGProperties));

    VARIANT_BOOL bSuccess = FALSE;

    if ( ! ( S_OK == rc ) ) {

        char szMessage[1024];
        char szEnv[64];

        if ( ! ( NULL == getenv("GSYSTEM_HOME") ) )
            strcpy(szEnv,getenv("GSYSTEM_HOME"));
        else
            sprintf_s<64>(szEnv,"%cGSYSTEM_HOME%c",'%','%');

        strcat(szEnv,"\\Common\\Artifacts\\");

#if _DEBUG
        strcat(szEnv,"Debug\\");
#else
        strcat(szEnv,"Release\\");
#endif

#ifdef _WIN64
        strcat(szEnv,"x64\\");
#else
        strcat(szEnv,"Win32\\");
#endif

        sprintf_s<1024>(szMessage,
            "Note that this project uses the Properties "
            "component developed by EnVisioNateSW.\n\n"
            "The component is open source, however, to\n"
            "simplify your use of this project the binary\n"
            "for that component is included in the \n"
            "Common Repository.\n\n"
            "The component, however, needs to be registered.\n\n"
            "To register it, in a DOS  prompt with\n"
            "administrative privileges, CD to the folder \n"
            "\"%s\\\"\n"
            "and issue the command \"regsvr32 Properties.ocx\"\n\n"
            "Alternatively, build this project with USE_GS_PROPERTIES= 0\n"
            "to remove this warning",szEnv);

        if ( IsUserAnAdmin() ) {

            strcat(szMessage,"\n\nYou are running as administrator. Would you like "
                                "to register the component from here?");

            HRESULT idAsk = MessageBox(NULL,szMessage,"Note!",MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON1);

            if ( IDYES == idAsk ) {
                char szDLL[MAX_PATH];
                sprintf_s<MAX_PATH>(szDLL,"%sProperties.ocx",szEnv);
                HMODULE hModule = LoadLibrary(szDLL);
                HRESULT (__stdcall *dllRegisterServer)() = NULL;
                HRESULT rc = E_FAIL;

                if ( ! ( NULL == hModule )) {
                    dllRegisterServer = (HRESULT (__stdcall *)())GetProcAddress(hModule,"DllRegisterServer");
                    if ( ! ( NULL == dllRegisterServer ) )
                        rc = dllRegisterServer();
                }
                if ( ! ( S_OK == rc ) || NULL == hModule || NULL == dllRegisterServer ) {
                    sprintf_s<1024>(szMessage,"Self registration of the Properties component failed.\n"
                                            "Please ensure the file Properties.ocx is in\n\n"
                                            "%s\n\nOr perhaps reclone the Common repository",szEnv);
                    useGSProperties = false;
                } else {

                    if ( 1 < argc )
                        sprintf_s<1024>(szMessage,"\"%s\"",argv[1]);
                    else 
                        szMessage[0] = '\0';
                    switch ( argc ) {
                    case 1:
                        _spawnl(_P_NOWAIT,argv[0],szMessage,NULL);
                        break;
                    default:
                        _spawnl(_P_NOWAIT,argv[0],szMessage,szMessage,NULL);
                        break;
                    }
                    exit(0);
                }
            }
            useGSProperties = false;
        } else {
            MessageBox(NULL,szMessage,"Note!",MB_ICONEXCLAMATION);
            useGSProperties = false;
        }

    } else {

        pIGProperties -> Add((BSTR)L"window position",NULL);
        pIGProperties -> DirectAccess((BSTR)L"window position",TYPE_BINARY,&rcFrame,sizeof(RECT));

        pIGProperties -> Add((BSTR)L"postscript properties size",NULL);
        pIGProperties -> DirectAccess((BSTR)L"postscript properties size",TYPE_BINARY,&cbPostScriptProperties,sizeof(long));

        pIGProperties -> Add((BSTR)L"postscript properties",NULL);
        pIGProperties -> DirectAccess((BSTR)L"postscript properties",TYPE_BINARY,(void *)postScriptPropertiesBlob,512);

        WCHAR szwDataFile[MAX_PATH];
        MultiByteToWideChar(CP_ACP,0,szApplicationDataDirectory,-1,szwDataFile,MAX_PATH);
        wcscat(szwDataFile,L"\\HostPostscript.settings");

        pIGProperties -> put_FileName(szwDataFile);

        pIGProperties -> LoadFile(&bSuccess);

    }

    if ( FALSE == bSuccess )
        adjustToMonitorSize(&rcFrame);

#else

    adjustToMonitorSize(&rcFrame);

#endif

    ParsePSHost *pParsePSHost = new ParsePSHost();

    WNDCLASS gClass;

    memset(&gClass,0,sizeof(WNDCLASS));
    gClass.style = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
    gClass.lpfnWndProc = ParsePSHost::frameHandler;
    gClass.cbClsExtra = 32;
    gClass.cbWndExtra = 32;
    gClass.hInstance = hModule;
    gClass.hIcon = NULL;
    gClass.hCursor = NULL;
    gClass.hbrBackground = 0;
    gClass.lpszMenuName = NULL;
    gClass.lpszClassName = "psHostFrame";

    RegisterClass(&gClass);

    gClass.lpfnWndProc = ParsePSHost::clientHandler;
    gClass.lpszClassName = "psHostClient";

    RegisterClass(&gClass);

    pParsePSHost -> hwndFrame = CreateWindowEx(0L,"psHostFrame","PostScript Interpreter",WS_THICKFRAME | WS_VISIBLE | WS_SYSMENU,rcFrame.left,rcFrame.top,rcFrame.right - rcFrame.left,rcFrame.bottom - rcFrame.top,HWND_DESKTOP,(HMENU)NULL,hModule,(LPVOID)pParsePSHost);

    RECT rcAdj{0,0,0,0};
    AdjustWindowRectEx(&rcAdj,(DWORD)GetWindowLongPtr(pParsePSHost -> hwndFrame,GWL_STYLE),FALSE,(DWORD)GetWindowLongPtr(pParsePSHost -> hwndFrame,GWL_EXSTYLE));

    pParsePSHost -> hwndPSHost = CreateWindowEx(0L,"psHostClient","",WS_CHILD | WS_VISIBLE,0,0,rcFrame.right - rcFrame.left - (rcAdj.right - rcAdj.left),
                                                                        rcFrame.bottom - rcFrame.top - (rcAdj.bottom - rcAdj.top),pParsePSHost -> hwndFrame,(HMENU)NULL,hModule,(LPVOID)pParsePSHost);

    pParsePSHost -> pIOleInPlaceFrame_HTML_Host = new ParsePSHost::_IOleInPlaceFrame(pParsePSHost,pParsePSHost -> hwndPSHost);
    pParsePSHost -> pIOleInPlaceSite_HTML_Host = new ParsePSHost::_IOleInPlaceSite(pParsePSHost,pParsePSHost -> pIOleInPlaceFrame_HTML_Host);
    pParsePSHost -> pIOleClientSite_HTML_Host = new ParsePSHost::_IOleClientSite(pParsePSHost,pParsePSHost -> pIOleInPlaceSite_HTML_Host,pParsePSHost -> pIOleInPlaceFrame_HTML_Host);
    pParsePSHost -> pIOleDocumentSite_HTML_Host = new ParsePSHost::_IOleDocumentSite(pParsePSHost,pParsePSHost -> pIOleClientSite_HTML_Host);

    rc = CoCreateInstance(CLSID_PostScriptInterpreter,NULL,CLSCTX_ALL,IID_IPostScriptInterpreter,reinterpret_cast<void **>(&pParsePSHost -> pIPostScript));

    pParsePSHost -> pIPostScript -> QueryInterface(IID_IOleObject,reinterpret_cast<void **>(&pParsePSHost -> pIOleObject_HTML));
    rc = pParsePSHost -> pIOleObject_HTML -> QueryInterface(IID_IOleInPlaceObject,reinterpret_cast<void **>(&pParsePSHost -> pIOleInPlaceObject_HTML));
    pParsePSHost -> pIOleObject_HTML -> SetClientSite(pParsePSHost -> pIOleClientSite_HTML_Host);

    if ( 1 < argc )
        pParsePSHost -> pIPostScript -> SetSource((char *)argv[1]);

    //pParsePSHost -> pIPostScript -> LogLevel(logLevel::verbose);
    //pParsePSHost -> pIPostScript -> LogLevel(logLevel::none);

    //pParsePSHost -> pIPostScript -> RendererLogLevel(logLevel::verbose);
    //pParsePSHost -> pIPostScript -> RendererLogLevel(logLevel::none);

    if ( 0 < cbPostScriptProperties )
        pParsePSHost -> pIPostScript -> SetPeristableProperties((UINT_PTR)postScriptPropertiesBlob);

    MSG qMessage;

    while ( GetMessage(&qMessage,(HWND)NULL,0L,0L) ) { 
        TranslateMessage(&qMessage);
        DispatchMessage(&qMessage);
    }

#if USE_GS_PROPERTIES
    if ( useGSProperties ) {
        GetWindowRect(pParsePSHost -> hwndFrame,&rcFrame);
        UINT_PTR pPostScriptProperties = NULL;
        pParsePSHost -> pIPostScript -> GetPeristableProperties(&pPostScriptProperties,&cbPostScriptProperties);
        memcpy(postScriptPropertiesBlob,(BYTE *)pPostScriptProperties,cbPostScriptProperties);
        pIGProperties -> Save();
        CoTaskMemFree((void *)pPostScriptProperties);
    }
#endif

    delete pParsePSHost;

_CrtDumpMemoryLeaks(  );
     return 0;
    }


#if USE_GS_PROPERTIES
    int GetLocation(HWND hwnd,long key,char *szFolderLocation) {

    ITEMIDLIST *ppItemIDList;
    IShellFolder *pIShellFolder;
    LPCITEMIDLIST pcParentIDList;

    HRESULT wasInitialized = CoInitialize(NULL);

    szFolderLocation[0] = '\0';

    HRESULT rc = SHGetFolderLocation(hwnd,key,NULL,0,&ppItemIDList);

    if ( S_OK != rc ) {
        char szMessage[256];
        sprintf(szMessage,"SHGetFolderLocation returned %ld",rc);
        MessageBoxA(NULL,szMessage,"Error",MB_OK);
        szFolderLocation[0] = '\0';
        return 0;
    }

    rc = SHBindToParent(ppItemIDList, IID_IShellFolder, (void **) &pIShellFolder, &pcParentIDList);

    if ( S_OK == rc ) {

        STRRET strRet;
        rc = pIShellFolder -> GetDisplayNameOf(pcParentIDList,SHGDN_FORPARSING,&strRet);
        pIShellFolder -> Release();
        if ( S_OK == rc ) {
            WideCharToMultiByte(CP_ACP,0,strRet.pOleStr,-1,szFolderLocation,MAX_PATH,0,0);
        } else {
            char szMessage[256];
            sprintf(szMessage,"GetDisplayNameOf returned %ld",rc);
            MessageBoxA(NULL,szMessage,"Error",MB_OK);
            szFolderLocation[0] = '\0';
            return 0;
        }
    } else {
        char szMessage[256];
        sprintf(szMessage,"SHBindToParent returned %ld",rc);
        MessageBoxA(NULL,szMessage,"Error",MB_OK);
        szFolderLocation[0] = '\0';
        return 0;
    }

    if ( S_FALSE == wasInitialized )
        CoUninitialize();

    return 0;
    }
#endif

    int pixelsToHiMetric(SIZEL *pPixels,SIZEL *phiMetric) {
    HDC hdc = GetDC(0);
    int pxlsX,pxlsY;
 
    pxlsX = GetDeviceCaps(hdc,LOGPIXELSX);
    pxlsY = GetDeviceCaps(hdc,LOGPIXELSY);
    ReleaseDC(0,hdc);
 
    phiMetric -> cx = PIXELS_TO_HIMETRIC(pPixels -> cx,pxlsX);
    phiMetric -> cy = PIXELS_TO_HIMETRIC(pPixels -> cy,pxlsY);

    return TRUE;
    }


    static long monitorIndex = -1L;;
    static HMONITOR hFoundMonitor = NULL;
    static long workingAreaHeight = 0L;


    BOOL CALLBACK enumFindMonitor(HMONITOR hm,HDC,RECT *,LPARAM lp) {
    monitorIndex++;
    if ( -1L == lp ) {
        MONITORINFO mi{0};
        mi.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(hm,&mi);
        if ( mi.dwFlags & MONITORINFOF_PRIMARY ) {
            hFoundMonitor = hm;
            workingAreaHeight = mi.rcWork.bottom - mi.rcWork.top;
            return FALSE;
        }
    }
    if ( monitorIndex == lp ) {
        hFoundMonitor = hm;
        MONITORINFO mi{0};
        mi.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(hm,&mi);
        workingAreaHeight = mi.rcWork.bottom - mi.rcWork.top;
        return FALSE;
    }
    return TRUE;
    }


    void adjustToMonitorSize(RECT *prcFrame) {

    long monitorCount = GetSystemMetrics(SM_CMONITORS);

    long monitorIndex = -1L;
    long activeMonitor = -1L;

    EnumDisplayMonitors(NULL,NULL,enumFindMonitor,(LPARAM)activeMonitor);

    RECT rcScreen = {0};

    if ( hFoundMonitor ) {
        MONITORINFO monitorInfo = {0};
        monitorInfo.cbSize = sizeof(MONITORINFO);
        GetMonitorInfo(hFoundMonitor,&monitorInfo);
        rcScreen = monitorInfo.rcWork;
        activeMonitor = monitorIndex;
    } else
        SystemParametersInfo(SPI_GETWORKAREA,0,&rcScreen,0);

    RECT rectFrame{0,0,0,0};

    rectFrame.top = 16;
    rectFrame.bottom = rcScreen.bottom - rcScreen.top - rectFrame.top - 16;
    rectFrame.right = rectFrame.left + (long)(8.5 * (double)(rectFrame.bottom - rectFrame.top) / 11.0);

    long cxScreen = rcScreen.right - rcScreen.left;
    long cyScreen = rcScreen.bottom - rcScreen.top;
    double aspectRatio = (double)cxScreen / (double)cyScreen;

    long cx = rectFrame.right - rectFrame.left;
    long cy = rectFrame.bottom - rectFrame.top;

    if ( rectFrame.top < rcScreen.top )
        rectFrame.top = rcScreen.top + 8;

    if ( rectFrame.left < rcScreen.left )
        rectFrame.left = rcScreen.left + 8;

    if ( rectFrame.left - rcScreen.left > cxScreen )
        rectFrame.left = cxScreen + rcScreen.left;

    double cxChange = (double)cx;
    while ( (rectFrame.top - rcScreen.top) + cy > cyScreen || (rectFrame.left - rcScreen.left) + cx > cxScreen ) {
        cy -= 1;
        cx = (long)((double)cx - aspectRatio);
    }

    cx = (long)cxChange;

    rectFrame.right = rectFrame.left + cx;
    rectFrame.bottom = rectFrame.top + cy;

    *prcFrame = rectFrame;
    //SetWindowPos(hwndFrame,HWND_TOP,rectFrame.left,rectFrame.top,rectFrame.right - rectFrame.left,rectFrame.bottom - rectFrame.top,SWP_SHOWWINDOW);

    return;
    }