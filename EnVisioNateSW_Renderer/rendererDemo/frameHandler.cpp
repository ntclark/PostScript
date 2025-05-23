#include "rendererDemo.h"


    LRESULT CALLBACK frameHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {
    switch ( msg ) {
    case WM_CREATE:
        hwndFrame = hwnd;
        break;

    case WM_SYSCOMMAND: {
        if ( wParam == SC_CLOSE ) {
            PostQuitMessage(0);
            return (LRESULT)0;
        }
        }
        break;

    case WM_SIZE: {
        RECT rcStatus;
        GetClientRect(hwndStatus,&rcStatus);
        SetWindowPos(hwndStatus,HWND_TOP,0,HIWORD(lParam) - (rcStatus.bottom - rcStatus.top),LOWORD(lParam),(rcStatus.bottom - rcStatus.top),0L);
        InvalidateRect(hwnd,NULL,TRUE);
        }
        break;

    case WM_MOUSEMOVE: {
        char szLocation[64];
        FLOAT x,y;
        pIRenderer -> WhereAmI(LOWORD(lParam),HIWORD(lParam),&x,&y);
        sprintf_s<64>(szLocation,"x = %5.2f, y = %5.2f",x,y);
        SetWindowText(hwndStatus,szLocation);
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd,&ps);
        FillRect(ps.hdc,&ps.rcPaint,(HBRUSH)(COLOR_WINDOW + 1));
        drawGraphics(ps.hdc);
        EndPaint(hwnd,&ps);
        }
        break;

    default:
        break;
    }


    return DefWindowProc(hwnd,msg,wParam,lParam);
    }
