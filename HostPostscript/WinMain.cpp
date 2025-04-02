
#include "HostPostscript.h"

#include "Properties_i.c"
#include "PostScriptInterpreter_i.c"

IGProperties *pIGProperties = NULL;

char szApplicationDataDirectory[MAX_PATH];

RECT rcFrame{512,512,1024,768};

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

    HRESULT rc = CoInitializeEx(NULL,0);

    char szTemp[MAX_PATH];

    GetLocation(NULL,CSIDL_COMMON_APPDATA,szTemp);

    sprintf(szApplicationDataDirectory,"%s\\CursiVision",szTemp);

    CoCreateInstance(CLSID_InnoVisioNateProperties,NULL,CLSCTX_ALL,IID_IGProperties,reinterpret_cast<void **>(&pIGProperties));

    pIGProperties -> Add((BSTR)L"window position",NULL);
    pIGProperties -> DirectAccess((BSTR)L"window position",TYPE_BINARY,&rcFrame,sizeof(RECT));

    WCHAR szwDataFile[MAX_PATH];
    MultiByteToWideChar(CP_ACP,0,szApplicationDataDirectory,-1,szwDataFile,MAX_PATH);
    wcscat(szwDataFile,L"\\HostPostscript.settings");

    pIGProperties -> put_FileName(szwDataFile);

    VARIANT_BOOL bSuccess;

    pIGProperties -> LoadFile(&bSuccess);

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

    rc = CoCreateInstance(CLSID_PostScriptInterpreter,NULL,CLSCTX_ALL,IID_IPostScript,reinterpret_cast<void **>(&pParsePSHost -> pIPostScript));

    pParsePSHost -> pIPostScript -> QueryInterface(IID_IOleObject,reinterpret_cast<void **>(&pParsePSHost -> pIOleObject_HTML));

    rc = pParsePSHost -> pIOleObject_HTML -> QueryInterface(IID_IOleInPlaceObject,reinterpret_cast<void **>(&pParsePSHost -> pIOleInPlaceObject_HTML));

    pParsePSHost -> pIOleObject_HTML -> SetClientSite(pParsePSHost -> pIOleClientSite_HTML_Host);

    if ( 1 < argc )
        pParsePSHost -> pIPostScript -> SetSource((char *)argv[1]);

    //pParsePSHost -> pIPostScript -> LogLevel(logLevel::verbose);
    pParsePSHost -> pIPostScript -> LogLevel(logLevel::none);

    MSG qMessage;

    while ( GetMessage(&qMessage,(HWND)NULL,0L,0L) ) { 
        TranslateMessage(&qMessage);
        DispatchMessage(&qMessage);
    }

    delete pParsePSHost;

    GetWindowRect(pParsePSHost -> hwndFrame,&rcFrame);

    pIGProperties -> Save();

    return 0;
    }


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


    int hiMetricToPixels(SIZEL *phiMetric,SIZEL *pPixels) {
    HDC hdc = GetDC(0);
    int pxlsX,pxlsY;

    pxlsX = GetDeviceCaps(hdc,LOGPIXELSX);
    pxlsY = GetDeviceCaps(hdc,LOGPIXELSY);

    ReleaseDC(0,hdc);

    pPixels -> cx = HIMETRIC_TO_PIXELS(phiMetric -> cx,pxlsX);
    pPixels -> cy = HIMETRIC_TO_PIXELS(phiMetric -> cy,pxlsY);

    return TRUE;
    }