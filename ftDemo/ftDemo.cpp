
#include <Windows.h>

#include "GlyphRenderer_i.h"
#include "GlyphRenderer_i.c"
#include "FontManager_i.h"
#include "FontManager_i.c"

    HWND hwndFrame = NULL;

    long cyWindow = 1864;

    LRESULT CALLBACK handler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

    IGlyphRenderer *pIGlyphRenderer = NULL;

    IFontManager *pIFontManager = NULL;

    int __stdcall WinMain(HINSTANCE hInst,HINSTANCE hInstancePrevious,LPSTR lpCmdLine,int nCmdShow) {

    HRESULT rc = CoInitializeEx(NULL,0);

    rc = CoCreateInstance(CLSID_GlyphRenderer,NULL,CLSCTX_ALL,IID_IGlyphRenderer,reinterpret_cast<void **>(&pIGlyphRenderer));
    rc = CoCreateInstance(CLSID_FontManager,NULL,CLSCTX_ALL,IID_IFontManager,reinterpret_cast<void **>(&pIFontManager));

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

    hwndFrame = CreateWindowEx(0L,"ftDemo","Free Type Demo",WS_OVERLAPPEDWINDOW | WS_VISIBLE,32,32,2048,cyWindow,NULL,(HMENU)NULL,hInst,NULL);

    IFont_EVNSW *pIFont = NULL;

    pIFontManager -> LoadFont((char *)"Tahoma",0,&pIFont);
    pIFontManager -> ScaleFont(72.0);

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