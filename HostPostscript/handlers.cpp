
#include "HostPostscript.h"

    LRESULT CALLBACK ParsePSHost::frameHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    ParsePSHost *p = reinterpret_cast<ParsePSHost *>(GetWindowLongPtr(hwnd,GWLP_USERDATA));

    switch ( msg ) {

    case WM_CREATE: {
        CREATESTRUCT *pc = reinterpret_cast<CREATESTRUCT *>(lParam);
        SetWindowLongPtr(hwnd,GWLP_USERDATA,(ULONG_PTR)pc -> lpCreateParams);
        }
        return (LRESULT)FALSE;

    case WM_MOVE: {
        if ( NULL == p -> pIOleObject_HTML )
            return (LRESULT)FALSE;
        RECT rcFrame;
        GetClientRect(hwnd,&rcFrame);
        p -> pIOleObject_HTML -> DoVerb(OLEIVERB_INPLACEACTIVATE,NULL,NULL,0L,NULL,&rcFrame);
        }
        return (LRESULT)FALSE;

    case WM_SIZE: {
        if ( NULL == p -> pIOleObject_HTML )
            return (LRESULT)FALSE;
        SIZEL sizel{LOWORD(lParam),HIWORD(lParam)};
        SIZEL tempSizel;
        pixelsToHiMetric(&sizel,&tempSizel);
        p -> pIOleObject_HTML -> SetExtent(DVASPECT_CONTENT,&tempSizel);
        }
        return (LRESULT)FALSE;


    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd,&ps);
        EndPaint(hwnd,&ps);
        }
        return (LRESULT)TRUE;

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