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
#include "job.h"

    LRESULT CALLBACK PostScriptInterpreter::cmdPaneHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    PostScriptInterpreter *p = (PostScriptInterpreter *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch ( msg ) {
    case WM_INITDIALOG: {
        SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)lParam);
        }
        break;

    case WM_SIZE: {
        long cx = LOWORD(lParam);
        RECT rcLabel;
        GetWindowRect(GetDlgItem(hwnd,IDDI_CMD_PANE_LOG_SHOW),&rcLabel);
        SetWindowPos(GetDlgItem(hwnd,IDDI_CMD_PANE_LOG_SHOW),HWND_TOP,cx - (rcLabel.right - rcLabel.left),8,0,0,SWP_NOSIZE);
        GetWindowRect(GetDlgItem(hwnd,IDDI_CMD_PANE_RENDERER_LOG_SHOW),&rcLabel);
        SetWindowPos(GetDlgItem(hwnd,IDDI_CMD_PANE_RENDERER_LOG_SHOW),HWND_TOP,cx - (rcLabel.right - rcLabel.left),32,0,0,SWP_NOSIZE);
        }
        break;

    case WM_LBUTTONDOWN:
    case WM_MOUSEMOVE: {
        POINT ptMouse{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
        MapWindowPoints(hwnd,HWND_DESKTOP,&ptMouse,1);
        RECT rcLogLabel{0,0,0,0};
        long ids[]{IDDI_CMD_PANE_LOG_SHOW,IDDI_CMD_PANE_RENDERER_LOG_SHOW};
        for ( long k = 0; k < sizeof(ids)/sizeof(long); k++ ) {
            GetWindowRect(GetDlgItem(hwnd,ids[k]),&rcLogLabel);
            if ( ptMouse.x > rcLogLabel.left && ptMouse.y > rcLogLabel.top && ptMouse.x < rcLogLabel.right && ptMouse.y < rcLogLabel.bottom ) {
                if ( msg == WM_MOUSEMOVE )
                    SetCursor(LoadCursor(NULL,IDC_HAND));
                else
                    p -> toggleLogVisibility(ids[k]);
                break;
            }
        }
        }
        break;


    case WM_COMMAND: {
        switch( LOWORD(wParam) ) {
        case IDDI_CMD_PANE_ACTIVE_FILE: {
            char szTemp[MAX_PATH];
            GetWindowText((HWND)lParam,szTemp,MAX_PATH);
            EnableWindow(GetDlgItem(hwnd,IDDI_CMD_PANE_RUN), ! ( '\0' == szTemp[0] ) ? TRUE : FALSE);
            }
            break;

        case IDDI_CMD_PANE_ACTIVE_FILE_GET: {
            OPENFILENAME openFileName;
            char szFilter[MAX_PATH],szFile[MAX_PATH];

            memset(szFilter,0,sizeof(szFilter));
            memset(szFile,0,sizeof(szFile));
            memset(&openFileName,0,sizeof(OPENFILENAME));

            sprintf(szFilter,"PostScript Files");
            long k = (DWORD)strlen(szFilter) + (DWORD)sprintf(szFilter + (DWORD)strlen(szFilter) + 1,"*.ps");
            k = k + (DWORD)sprintf(szFilter + k + 2,"All Files");
            sprintf(szFilter + k + 3,"*.*");

            openFileName.lStructSize = sizeof(OPENFILENAME);
            openFileName.hwndOwner = hwnd;
            openFileName.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
            openFileName.lpstrFilter = szFilter;
            openFileName.lpstrFile = szFile;
            openFileName.lpstrDefExt = "pdf";
            openFileName.nFilterIndex = 1;
            openFileName.nMaxFile = MAX_PATH;
            openFileName.lpstrTitle = "Select the existing PostScript file";

            openFileName.lpstrInitialDir = szUserDirectory;

            if ( ! GetOpenFileName(&openFileName) ) 
                break;

            p -> SetSource(openFileName.lpstrFile);

            }
            break;

        case IDDI_CMD_PANE_RUN:
            PostScriptInterpreter::endRunActions.push_back( [=] {
                EnableWindow(GetDlgItem(hwnd,IDDI_CMD_PANE_RUN),TRUE);
                EnableWindow(GetDlgItem(hwnd,IDDI_CMD_PANE_STOP),FALSE);
            });
            p -> Parse();
            EnableWindow(GetDlgItem(hwnd,IDDI_CMD_PANE_RUN),FALSE);
            EnableWindow(GetDlgItem(hwnd,IDDI_CMD_PANE_STOP),TRUE);
            break;

        case IDDI_CMD_PANE_STOP:
            p -> currentJob() -> RequestQuit();
            EnableWindow(GetDlgItem(hwnd,IDDI_CMD_PANE_RUN),TRUE);
            EnableWindow(GetDlgItem(hwnd,IDDI_CMD_PANE_STOP),FALSE);
            break;

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