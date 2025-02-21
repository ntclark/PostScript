
#include <Windows.h>

#include "Renderer_i.h"
#include "Renderer_i.c"

#include "FontManager_i.h"
#include "FontManager_i.c"

    HWND hwndFrame = NULL;

    long cyWindow = 1864;

    LRESULT CALLBACK handler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

    IRenderer *pIGlyphRenderer = NULL;

    IFontManager *pIFontManager = NULL;

    int __stdcall WinMain(HINSTANCE hInst,HINSTANCE hInstancePrevious,LPSTR lpCmdLine,int nCmdShow) {

    HRESULT rc = CoInitializeEx(NULL,0);

    //rc = CoCreateInstance(CLSID_Renderer,NULL,CLSCTX_ALL,IID_IRenderer,reinterpret_cast<void **>(&pIGlyphRenderer));

    rc = CoCreateInstance(CLSID_FontManager,NULL,CLSCTX_ALL,IID_IFontManager,reinterpret_cast<void **>(&pIFontManager));
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
    gClass.lpszClassName = "ftDemo";

    RegisterClass(&gClass);

    hwndFrame = CreateWindowEx(0L,"ftDemo","Free Type Demo",WS_OVERLAPPEDWINDOW | WS_VISIBLE,32,32,2048 + 1024,cyWindow,NULL,(HMENU)NULL,hInst,NULL);

    IFont_EVNSW *pIFont = NULL;

    pIFontManager -> LoadFont((char *)"Gabriola",0,&pIFont);
    pIFontManager -> ScaleFont(36.0);

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