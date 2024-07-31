
#include "ParsePostscript.h"

    LRESULT CALLBACK ParsePSHost::frameHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    ParsePSHost *p = reinterpret_cast<ParsePSHost *>(GetWindowLongPtr(hwnd,GWLP_USERDATA));

    switch ( msg ) {

    case WM_CREATE: {
        CREATESTRUCT *pc = reinterpret_cast<CREATESTRUCT *>(lParam);
        SetWindowLongPtr(hwnd,GWLP_USERDATA,(ULONG_PTR)pc -> lpCreateParams);
        }
        return (LRESULT)FALSE;

    case WM_SIZE:
        if ( NULL == p -> hwndPSHost )
            return (LRESULT)FALSE;
        SetWindowPos(p -> hwndPSHost,HWND_TOP,0,0,LOWORD(lParam),HIWORD(lParam),SWP_NOMOVE);
        return (LRESULT)FALSE;

    case WM_USER + 1: {
        SetWindowPos(p -> hwndPSHost,HWND_TOP,0,0,LOWORD(lParam),HIWORD(lParam),0L);
        }
        return (LRESULT)0L;

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


    LRESULT CALLBACK ParsePSHost::clientHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    ParsePSHost *p = reinterpret_cast<ParsePSHost *>(GetWindowLongPtr(hwnd,GWLP_USERDATA));

    switch ( msg ) {

    case WM_CREATE: {
        CREATESTRUCT *pc = reinterpret_cast<CREATESTRUCT *>(lParam);
        SetWindowLongPtr(hwnd,GWLP_USERDATA,(ULONG_PTR)pc -> lpCreateParams);
        }
        return (LRESULT)FALSE;

    case WM_SIZE: {
        if ( NULL == p -> pIOleObject_HTML )
            break;
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

    case WM_PARSE_PS:
        p -> pIPostScript -> Parse((char *)lParam);
        return (LRESULT)FALSE;

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
