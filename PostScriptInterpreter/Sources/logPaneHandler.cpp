/*
Copyright 2025 EnVisioNate LLC

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the “Software”), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is the MIT License
*/

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

    default:
        break;
    }

    return 0;
    }