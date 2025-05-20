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

#include "PostScriptInterpreter.h"

    HWND hwndPSLogLevel = NULL;
    HWND hwndRendererLogLevel = NULL;

    LRESULT CALLBACK PostScriptInterpreter::logPaneHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    PostScriptInterpreter *p = (PostScriptInterpreter *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch ( msg ) {
    case WM_INITDIALOG: {
        SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)lParam);
        HWND hwndCB = NULL;
        if ( ! ( NULL == GetDlgItem(hwnd,IDDI_POSTSCRIPT_LOG_CMD_PANE_LOG_LEVEL) ) ) {
            hwndPSLogLevel = GetDlgItem(hwnd,IDDI_POSTSCRIPT_LOG_CMD_PANE_LOG_LEVEL);
            hwndCB = GetDlgItem(hwnd,IDDI_POSTSCRIPT_LOG_CMD_PANE_LOG_LEVEL);
        }
        if ( ! ( NULL == GetDlgItem(hwnd,IDDI_RENDERER_LOG_CMD_PANE_LOG_LEVEL) ) ) {
            hwndRendererLogLevel = GetDlgItem(hwnd,IDDI_RENDERER_LOG_CMD_PANE_LOG_LEVEL);
            hwndCB = GetDlgItem(hwnd,IDDI_RENDERER_LOG_CMD_PANE_LOG_LEVEL);
        }
        SendMessage(hwndCB,CB_INSERTSTRING,(WPARAM)-1L,(LPARAM)"none");
        SendMessage(hwndCB,CB_INSERTSTRING,(WPARAM)-1L,(LPARAM)"Verbose");
        if ( ! ( NULL == GetDlgItem(hwnd,IDDI_POSTSCRIPT_LOG_CMD_PANE_LOG_LEVEL) ) )
            SendMessage(hwndCB,CB_SETCURSEL,(WPARAM)PostScriptInterpreter::theLogLevel,0L);
        if ( ! ( NULL == GetDlgItem(hwnd,IDDI_RENDERER_LOG_CMD_PANE_LOG_LEVEL) ) )
            SendMessage(hwndCB,CB_SETCURSEL,(WPARAM)PostScriptInterpreter::theRendererLogLevel,0L);
        }
        break;

    case WM_COMMAND: {
        switch ( LOWORD(wParam) ) {
        case IDDI_POSTSCRIPT_LOG_CMD_PANE_LOG_LEVEL:
            if ( CBN_SELCHANGE == HIWORD(wParam) ) {
                if ( logLevel::none == PostScriptInterpreter::theLogLevel )
                    SetWindowText(hwndPostScriptLogContent,"");
                PostScriptInterpreter::theLogLevel = (logLevel)SendMessage((HWND)lParam,CB_GETCURSEL,0L,0L);
            }
            break;
        case IDDI_RENDERER_LOG_CMD_PANE_LOG_LEVEL:
            if ( CBN_SELCHANGE == HIWORD(wParam) ) {
                if ( logLevel::none == PostScriptInterpreter::theRendererLogLevel )
                    SetWindowText(hwndRendererLogContent,"");
                PostScriptInterpreter::theRendererLogLevel = (logLevel)SendMessage((HWND)lParam,CB_GETCURSEL,0L,0L);
            }
            break;
        }
        }
        break;

    case WM_LBUTTONDOWN:
    case WM_MOUSEMOVE: {
        POINT ptMouse{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
        MapWindowPoints(hwnd,HWND_DESKTOP,&ptMouse,1);
        RECT rcLogLabel{0,0,0,0};
        GetWindowRect(GetDlgItem(hwnd,IDDI_CMD_PANE_RENDERER_LOG_SHOW),&rcLogLabel);
        if ( ptMouse.x > rcLogLabel.left && ptMouse.y > rcLogLabel.top && ptMouse.x < rcLogLabel.right && ptMouse.y < rcLogLabel.bottom ) {
            if ( msg == WM_MOUSEMOVE )
                SetCursor(LoadCursor(NULL,IDC_HAND));
            else
                p -> toggleLogVisibility(IDDI_CMD_PANE_RENDERER_LOG_SHOW);
            break;
        }
        }
        break;

    default:
        break;
    }

    return 0;
    }


    void PostScriptInterpreter::toggleLogVisibility(long itemId) {

    if ( IDDI_CMD_PANE_LOG_SHOW == itemId ) {

        logIsVisible = ! logIsVisible;

        if ( logIsVisible && logLevel::none == theLogLevel ) {
            SendMessage(hwndPostScriptLogContent,EM_SETTEXTMODE,(WPARAM)TM_PLAINTEXT,0L);
            SetWindowText(hwndPostScriptLogContent,"Note: The logLevel is set to none");
            SendMessage(hwndPostScriptLogContent,EM_SETTEXTMODE,(WPARAM)TM_RICHTEXT,0L);
        }

    } else if ( IDDI_CMD_PANE_RENDERER_LOG_SHOW == itemId ) {

        rendererLogIsVisible = ! rendererLogIsVisible;

        if ( rendererLogIsVisible && logLevel::none == theRendererLogLevel ) {
            SendMessage(hwndRendererLogContent,EM_SETTEXTMODE,(WPARAM)TM_PLAINTEXT,0L);
            SetWindowText(hwndRendererLogContent,"Note: The renderer logLevel is set to none");
            SendMessage(hwndRendererLogContent,EM_SETTEXTMODE,(WPARAM)TM_RICHTEXT,0L);
        }

    } else
        return;

    PostMessage(hwndClient,WM_REFRESH_CLIENT_WINDOW,0L,0L);

    return;
    }