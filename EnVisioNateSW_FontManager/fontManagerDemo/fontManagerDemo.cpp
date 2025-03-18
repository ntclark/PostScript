
#include <Windows.h>

#include "resource.h"

#include "Renderer_i.h"
#include "Renderer_i.c"

#include "FontManager_i.h"
#include "FontManager_i.c"

/*
    fontManagerDemo is a very simple windows application to show how the 
    EnVisioNateSW FontManager and Renderer work so you can learn
    how to use these together easily in any of your own software.
*/

    HWND hwndFrame = NULL;
    HWND hwndDialog = NULL;

    long cyWindow = 1024;

    LRESULT CALLBACK handler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
    LRESULT CALLBACK dialogHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

    IRenderer *pIGlyphRenderer = NULL;
    IFontManager *pIFontManager = NULL;
    IFont_EVNSW *pIFont = NULL;


    int __stdcall WinMain(HINSTANCE hInst,HINSTANCE hInstancePrevious,LPSTR lpCmdLine,int nCmdShow) {

    HRESULT rc = CoInitializeEx(NULL,0);

    rc = CoCreateInstance(CLSID_FontManager,NULL,CLSCTX_ALL,IID_IFontManager,reinterpret_cast<void **>(&pIFontManager));

    //pIFontManager -> LoadFont((char *)"@Microsoft JhengHei UI",0,&pIFont);
    pIFontManager -> LoadFont((char *)"Arial",0,&pIFont);
    pIFontManager -> ScaleFont(36.0);

    pIFontManager -> QueryInterface(IID_IRenderer,reinterpret_cast<void **>(&pIGlyphRenderer));

    WNDCLASS gClass;

    memset(&gClass,0,sizeof(WNDCLASS));
    gClass.style = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
    gClass.lpfnWndProc = handler;
    gClass.cbClsExtra = 32;
    gClass.cbWndExtra = 32;
    gClass.hInstance = hInst;
    gClass.hIcon = NULL;
    gClass.hCursor = NULL;
    gClass.hbrBackground = 0;
    gClass.lpszMenuName = NULL;
    gClass.lpszClassName = "fontManagerDemo";

    RegisterClass(&gClass);

    hwndFrame = CreateWindowEx(0L,"fontManagerDemo","Font Manager Demo",WS_OVERLAPPEDWINDOW | WS_VISIBLE,32,32,2 * cyWindow,cyWindow,NULL,(HMENU)NULL,hInst,NULL);

    DLGTEMPLATE *dt = (DLGTEMPLATE *)LoadResource(hInst,FindResource(hInst,MAKEINTRESOURCE(IDD_GET_SENTENCE),RT_DIALOG));
    CreateDialogIndirect(hInst,dt,hwndFrame,dialogHandler);

    MSG qMessage;

    while ( GetMessage(&qMessage,(HWND)NULL,0L,0L) ) { 
        TranslateMessage(&qMessage);
        DispatchMessage(&qMessage);
    }

    DestroyWindow(hwndFrame);

    pIGlyphRenderer -> Release();
    pIFontManager -> Release();

    return 0;
    }