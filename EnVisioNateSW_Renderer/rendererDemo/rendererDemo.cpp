
#define DEFINE_DATA

#include "rendererDemo.h"

/*
    rendererDemo is a very simple windows application to show how the 
    EnVisioNateSW Renderer works so you can see how to use it 
    easily in any of your own software.
*/

    // I'll break my own rule about commenting in code in order to provide guidance on how to hook up an events
    // interface (sink) to a COM Object (source) among other things.

    // This code is a tutorial, it is not production code

    // The following struct is an implementation of the RendererNotifications 
    // interface.
    // If this project were a COM object, there would be a lot more
    // to the IUnknown and other COM Implementation factors.
    // However, this project needs a simple Events Interface
    // to sink events (notifications) from the RWenderer
    // COM object and this very simple technique suffices
    // for that purpose

    class _IRendererNotifications : public IRendererNotifications {
    public:

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
            SetWindowText(hwndStatus,(char *)theError);
            return S_OK;
        }

        HRESULT __stdcall StatusNotification(UINT_PTR theStatus) {
            SetWindowText(hwndStatus,(char *)theStatus);
            return S_OK;
        }

        HRESULT __stdcall LogNotification(UINT_PTR theStatus) {

            //
            // Note that outputing these debug strings really 
            // takes a major hit on performance. 
            // It's not the event itself, but writing to the VS output
            // window. Doing other things here is probably okay

            OutputDebugStringA((char *)theStatus);
            OutputDebugStringA("\n");
            return S_OK;
        }

        HRESULT __stdcall Clear() {
            SetWindowText(hwndStatus,(char *)"");
            return S_OK;
        }

    } rendererNotifications;


    int __stdcall WinMain(HINSTANCE hInst,HINSTANCE hInstancePrevious,LPSTR lpCmdLine,int nCmdShow) {

    HRESULT rc = CoInitializeEx(NULL,0);

    /*

    NOTE: THE FONT MANAGER HAS IT'S OWN BUILT IN RENDERER BECAUSE WHEN YOU WANT TO RENDER FONTS 
    WITHOUT OTHER GRAPHICS, THE CLIENT WOULD NOT THEN NEED TO CREATE A RENDERER.

    THIS MEANS THAT IF YOU HAVE FONTS MIXED WITH OTHER GRAPHICS, YOU WANT TO QUERY FOR YOUR RENDERER
    INTERFACE FROM THE FONT MANAGER.

    YOU CAN, IF YOU WANT, CREATE THE RENDERER SEPARATE FROM THE FONT MANAGER, BUT YOU WILL HAVE TO
    CALL RENDER ON THE FONT MANAGER'S RENDERER ANYWAY AT SOME POINT AND YOU WOULD HAVE TO
    GET THE IRENDERER INTRFACE FROM THE FONT MANAGER THEN ANYWAY.

    (sorry about all the yelling)
    */

    rc = CoCreateInstance(CLSID_FontManager,NULL,CLSCTX_ALL,IID_IFontManager,reinterpret_cast<void **>(&pIFontManager));

    // Don't do this
    //rc = CoCreateInstance(CLSID_Renderer,NULL,CLSCTX_ALL,IID_IRenderer,reinterpret_cast<void **>(&pIRenderer));

    // Do this instead
    rc = pIFontManager -> QueryInterface(IID_IRenderer,reinterpret_cast<void **>(&pIRenderer));

    pIRenderer -> QueryInterface(IID_IGraphicElements,reinterpret_cast<void **>(&pIGraphicElements));

    pIRenderer -> QueryInterface(IID_IGraphicParameters,reinterpret_cast<void **>(&pIGraphicParameters));

    IConnectionPointContainer *pIConnectionPointContainer = NULL;

    // Ask the COM object for a ConnectionPointContainer, which can be thought of as the thing 
    // that will manage the connection points the COM Object might provide to other clients

    rc = pIRenderer -> QueryInterface(IID_IConnectionPointContainer,reinterpret_cast<void **>(&pIConnectionPointContainer));

    IConnectionPoint *pIConnectionPoint = NULL;

    // Ask the container whether it supports a connection to a particular interface
    rc = pIConnectionPointContainer -> FindConnectionPoint(IID_IRendererNotifications,&pIConnectionPoint);

    DWORD dwCookie = 0L;
    IUnknown *pIUnknownSink = NULL;

    // The container, in this case DOES support the event interface. This is known because we wrote it
    // If it did not, then the ConnectionPointContainer should be Released.

    rendererNotifications.QueryInterface(IID_IUnknown,reinterpret_cast<void **>(&pIUnknownSink));

    rc = pIConnectionPoint -> Advise(pIUnknownSink,&dwCookie);

    pIConnectionPointContainer -> Release();

    pIFontManager -> LoadFont((char *)"Microsoft Sans Serif",0L,&pIFont);

    pIFontManager -> ScaleFont(2.0);

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
    gClass.lpszClassName = "rendererFrame";

    RegisterClass(&gClass);

    hwndFrame = CreateWindowEx(0L,"rendererFrame","Renderer Demo", WS_OVERLAPPEDWINDOW | WS_VISIBLE,256,256,768,768,NULL,(HMENU)NULL,hInst,NULL);

    hwndStatus = CreateWindowEx(0,STATUSCLASSNAME,(PCTSTR) NULL,SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE,0, 0, 0, 0,hwndFrame,(HMENU)NULL,hInst,NULL);

    DLGTEMPLATE *dt = (DLGTEMPLATE *)LoadResource(NULL,FindResource(NULL,MAKEINTRESOURCE(IDD_OPTIONS),RT_DIALOG));
    CreateDialogIndirect(NULL,dt,hwndFrame,(DLGPROC)dialogHandler);

    RECT rcDialog;
    GetWindowRect(hwndDialog,&rcDialog);

    SetWindowPos(hwndDialog,HWND_TOP,256 + 32,256 - (rcDialog.bottom - rcDialog.top) - 32,0,0,SWP_NOSIZE);

    MSG qMessage;

    while ( GetMessage(&qMessage,(HWND)NULL,0L,0L) ) { 
        TranslateMessage(&qMessage);
        DispatchMessage(&qMessage);
    }

    pIConnectionPoint -> Unadvise(dwCookie);

    DestroyWindow(hwndFrame);

    pIFontManager -> Release();

    pIGraphicParameters -> Release();
    pIGraphicElements -> Release();
    pIRenderer -> Release();

    return 0;
    }