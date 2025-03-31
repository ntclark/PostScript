
#define DEFINE_DATA

#include "fontManagerDemo.h"

/*
    fontManagerDemo is a very simple windows application to show how the 
    EnVisioNateSW FontManager and Renderer work so you can learn
    how to use these together easily in any of your own software.
*/

    // I'll break my own rule about commenting in code in order to provide guidance on how to hook up an events
    // interface (sink) to a COM Object (source) among other things.

    // This code is a tutorial, it is not production code

    // The following struct is an implementation of the FontManagerNotifications 
    // interface.
    // If this project were a COM object, there would be a lot more
    // to the IUnknown and other COM Implementation factors.
    // However, this project needs a simple Events Interface
    // to sink events (notifications) from the FontManager
    // COM object and this very simple technique suffices
    // for that purpose

    class _IFontManagerNotifications : public IFontManagerNotifications {
    public:

        //   IUnknown

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv) {
            if ( ! ppv )
                return E_POINTER;
            if ( IID_IUnknown == riid )
                *ppv = static_cast<IUnknown *>(this);
            else if ( IID_IFontManagerNotifications == riid )
                *ppv = static_cast<IFontManagerNotifications *>(this);
            else
                return E_NOINTERFACE;
            return S_OK;
        }
        STDMETHOD_ (ULONG, AddRef)() { return 1; }
        STDMETHOD_ (ULONG, Release)() { return 1; }

        // IFontManagerNotifications

        HRESULT __stdcall ErrorNotification(UINT_PTR theError) {
            if ( ! ( '\0' == ((char *)theError)[0] ) )
                MessageBox(hwndFrame,(char *)theError,"Notification",MB_OK);
            return S_OK;
        }

    } fontManagerNotifications;


    int __stdcall WinMain(HINSTANCE hInst,HINSTANCE hInstancePrevious,LPSTR lpCmdLine,int nCmdShow) {

    HRESULT rc = CoInitializeEx(NULL,0);

    rc = CoCreateInstance(CLSID_FontManager,NULL,CLSCTX_ALL,IID_IFontManager,reinterpret_cast<void **>(&pIFontManager));

    IConnectionPointContainer *pIConnectionPointContainer = NULL;

    // Ask the COM object for a ConnectionPointContainer, which can be thought of as the thing 
    // that will manage the connection points the COM Object might provide to other clients

    rc = pIFontManager -> QueryInterface(IID_IConnectionPointContainer,reinterpret_cast<void **>(&pIConnectionPointContainer));

    IConnectionPoint *pIConnectionPoint = NULL;

    // Ask the container whether it supports a connection to a particular interface
    rc = pIConnectionPointContainer -> FindConnectionPoint(IID_IFontManagerNotifications,&pIConnectionPoint);

    DWORD dwCookie = 0L;
    IUnknown *pIUnknownSink = NULL;

    // The container, in this case DOES support the event interface. This is known because we wrote it
    // If it did not, then the ConnectionPointContainer should be Released.

    fontManagerNotifications.QueryInterface(IID_IUnknown,reinterpret_cast<void **>(&pIUnknownSink));

    rc = pIConnectionPoint -> Advise(pIUnknownSink,&dwCookie);

    pIConnectionPointContainer -> Release();

    pIFontManager -> QueryInterface(IID_IRenderer,reinterpret_cast<void **>(&pIRenderer));

    pIRenderer -> QueryInterface(IID_IGraphicElements,reinterpret_cast<void **>(&pIGraphicElements));

    WNDCLASS gClass;

    memset(&gClass,0,sizeof(WNDCLASS));
    gClass.style = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
    gClass.lpfnWndProc = frameHandler;
    gClass.cbClsExtra = 32;
    gClass.cbWndExtra = 32;
    gClass.hInstance = hInst;
    gClass.hIcon = NULL;
    gClass.hCursor = NULL;
    gClass.hbrBackground = 0;
    gClass.lpszMenuName = NULL;
    gClass.lpszClassName = "fontManagerFrame";

    RegisterClass(&gClass);

    hwndFrame = CreateWindowEx(WS_EX_CONTROLPARENT,"fontManagerFrame","Font Manager Demo", WS_OVERLAPPEDWINDOW | WS_VISIBLE,64,256,0,0,NULL,(HMENU)NULL,hInst,NULL);

    SetWindowPos(hwndFrame,HWND_TOP,0,0,128,128,SWP_NOMOVE);

    pIFontManager -> LoadFont((char *)"Arial",0,&pIFont);

    pIFontManager -> ScaleFont(36.0);

    DLGTEMPLATE *dt = (DLGTEMPLATE *)LoadResource(hInst,FindResource(hInst,MAKEINTRESOURCE(IDD_GET_SENTENCE),RT_DIALOG));
    CreateDialogIndirect(hInst,dt,hwndFrame,dialogHandler);

    MSG qMessage;

    while ( GetMessage(&qMessage,(HWND)NULL,0L,0L) ) { 
        TranslateMessage(&qMessage);
        DispatchMessage(&qMessage);
    }

    pIConnectionPoint -> Unadvise(dwCookie);

    DestroyWindow(hwndFrame);

    pIGraphicElements -> Release();
    pIRenderer -> Release();
    pIFontManager -> Release();

    return 0;
    }


    LRESULT CALLBACK frameHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    switch ( msg ) {

    case WM_CREATE:
        hwndFrame = hwnd;
        scrollInfo.cbSize = sizeof(SCROLLINFO);
        return (LRESULT)0;

    case WM_SYSCOMMAND: {
        if ( wParam == SC_CLOSE ) {
            PostQuitMessage(0);
            return (LRESULT)0;
        }
        }
        break;

    default:
        break;
    }

    return DefWindowProc(hwnd,msg,wParam,lParam);
    }


    void setFont() {

    char szFontSpec[256];
    char szTemp[128];

    SendMessage(GetDlgItem(hwndDialog,IDDI_FONT_NAMES),CB_GETLBTEXT,SendMessage(GetDlgItem(hwndDialog,IDDI_FONT_NAMES),CB_GETCURSEL,0L,0L),(LPARAM)szTemp);
    sprintf_s<256>(szFontSpec,"%s",szTemp);

    SendMessage(GetDlgItem(hwndDialog,IDDI_FONT_SCRIPTS),CB_GETLBTEXT,SendMessage(GetDlgItem(hwndDialog,IDDI_FONT_SCRIPTS),CB_GETCURSEL,0L,0L),(LPARAM)szTemp);

    //strcat(szFontSpec,":");
    //strcat(szFontSpec,szTemp);

    pIFontManager -> LoadFont(szFontSpec,0,&pIFont);

    GetDlgItemText(hwndDialog,IDDI_FONT_SIZE,szTemp,128);

    pIFont -> Scale((FLOAT)atof(szTemp),(FLOAT)atof(szTemp));

    PostMessage(hwndDialog,WM_USER + 2,0L,0L);

    return;
    }


    void calcTransforms(HWND hwndHost) {

    RECT rc;
    GetClientRect(hwndHost,&rc);

    long cyWindow = rc.bottom - rc.top;

    HDC hdc = GetDC(hwndHost);

    long cyWindowInPoints = (long)((FLOAT)cyWindow * 72.0f / (FLOAT)GetDeviceCaps(hdc,LOGPIXELSX));

    scalePDF = (FLOAT)cyWindow / cyWindowInPoints;
    psXForm = {1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};
    gdiXForm = {scalePDF,0.0f,0.0f,-scalePDF,0.0f,(FLOAT)cyWindow};

    ReleaseDC(hwndHost,hdc);
    return;
    }

