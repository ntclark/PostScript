
#include "PostScript.h"

    LRESULT CALLBACK PStoPDF::logPaneHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    PStoPDF *p = (PStoPDF *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch ( msg ) {
    case WM_INITDIALOG: {
        SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)lParam);
        }
        break;

#if 0
    case WM_SIZE: {
        long cx = LOWORD(lParam);
        RECT rcLabel;
        GetWindowRect(GetDlgItem(hwnd,IDDI_CMD_PANE_LOG_SHOW),&rcLabel);
        SetWindowPos(GetDlgItem(hwnd,IDDI_CMD_PANE_LOG_SHOW),HWND_TOP,cx - (rcLabel.right - rcLabel.left),8,0,0,SWP_NOSIZE);
        }
        break;

    case WM_LBUTTONDOWN:
    case WM_MOUSEMOVE: {
        POINT ptMouse{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
        MapWindowPoints(hwnd,HWND_DESKTOP,&ptMouse,1);
        RECT rcLogLabel{0,0,0,0};
        GetWindowRect(GetDlgItem(hwnd,IDDI_CMD_PANE_LOG_SHOW),&rcLogLabel);
        if ( ptMouse.x < rcLogLabel.left || ptMouse.y < rcLogLabel.top || ptMouse.x > rcLogLabel.right || ptMouse.y > rcLogLabel.bottom )
            break;
        if ( msg == WM_MOUSEMOVE )
            SetCursor(LoadCursor(NULL,IDC_HAND));
        else 
            p -> toggleLogVisibility();
        }
        break;
#endif

    case WM_COMMAND: {
        switch( LOWORD(wParam) ) {
        default:
            break;
        }

        }
        break;

    default:
        break;
    }

    return 0;
    }