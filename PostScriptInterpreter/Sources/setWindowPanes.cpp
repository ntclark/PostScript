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

    void PostScriptInterpreter::setWindowPanes(RECT *prcClient) {

    RECT rcDialog{0,0,0,0};

    RECT rcHost{0,0,0,0};
    if ( NULL == prcClient ) 
        GetClientRect(hwndHost,&rcHost);
    else
        rcHost = {prcClient -> left,prcClient -> top,prcClient-> right,prcClient -> bottom};

    long cxHost = rcHost.right - rcHost.left;
    long cyHost = rcHost.bottom - rcHost.top;

    SetWindowPos(hwndCmdPane,HWND_TOP,0,0,cxHost,CMD_PANE_HEIGHT,SWP_SHOWWINDOW);
    GetWindowRect(hwndCmdPane,&rcDialog);

    long cxClient = cxHost - (pPostScriptInterpreter -> logIsVisible ? pPostScriptInterpreter -> postScriptLogPaneWidth + SPLITTER_WIDTH : 0);
    cxClient -= (pPostScriptInterpreter -> rendererLogIsVisible ? pPostScriptInterpreter -> rendererLogPaneWidth + SPLITTER_WIDTH : 0);

    long cxPostScriptLogWindow = 0;
    long cxRendererLogWindow = 0;

    long cxRightEdge = 0;

    if ( pPostScriptInterpreter -> logIsVisible ) {

        cxPostScriptLogWindow = pPostScriptInterpreter -> postScriptLogPaneWidth + SPLITTER_WIDTH;

        if ( ! pPostScriptInterpreter -> rendererLogIsVisible )
            cxRightEdge = 8;

        SetWindowPos(hwndPostScriptLogContent,HWND_TOP,cxClient + SPLITTER_WIDTH,CMD_PANE_HEIGHT,
                        cxPostScriptLogWindow - cxRightEdge - SPLITTER_WIDTH,cyHost - CMD_PANE_HEIGHT,SWP_SHOWWINDOW);

        SetWindowPos(hwndPostScriptLogCmdPane,HWND_TOP,cxClient,0,
                        cxPostScriptLogWindow - cxRightEdge,CMD_PANE_HEIGHT,SWP_SHOWWINDOW);

        SetWindowPos(hwndPostScriptLogSplitter,HWND_TOP,cxClient,CMD_PANE_HEIGHT,SPLITTER_WIDTH,cyHost - CMD_PANE_HEIGHT,SWP_SHOWWINDOW);

        SetWindowText(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_LOG_SHOW),"PostScript Log >>");
        ShowWindow(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_RENDERER_LOG_SHOW),SW_HIDE);

        if ( pPostScriptInterpreter -> rendererLogIsVisible )
            SetWindowText(GetDlgItem(hwndPostScriptLogCmdPane,IDDI_CMD_PANE_RENDERER_LOG_SHOW),"Renderer Log >>");
        else
            SetWindowText(GetDlgItem(hwndPostScriptLogCmdPane,IDDI_CMD_PANE_RENDERER_LOG_SHOW),"<< Renderer Log");

    } else {
        cxPostScriptLogWindow = 0;
        ShowWindow(hwndPostScriptLogContent,SW_HIDE);
        ShowWindow(hwndPostScriptLogCmdPane,SW_HIDE);
        ShowWindow(hwndPostScriptLogSplitter,SW_HIDE);
        SetWindowText(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_LOG_SHOW),"<< PostScript Log");
    }

    if ( pPostScriptInterpreter -> rendererLogIsVisible ) {

        cxRendererLogWindow = pPostScriptInterpreter -> rendererLogPaneWidth + SPLITTER_WIDTH;

        if ( 0 == cxRightEdge )
            cxRightEdge = 8;

        SetWindowPos(hwndRendererLogContent,HWND_TOP,cxClient + cxPostScriptLogWindow + SPLITTER_WIDTH,CMD_PANE_HEIGHT,
                        cxRendererLogWindow - cxRightEdge - SPLITTER_WIDTH,cyHost - CMD_PANE_HEIGHT,SWP_SHOWWINDOW);

        SetWindowPos(hwndRendererLogCmdPane,HWND_TOP,cxClient + cxPostScriptLogWindow,0,
                        cxRendererLogWindow - SPLITTER_WIDTH - cxRightEdge,cyHost - CMD_PANE_HEIGHT,SWP_SHOWWINDOW);

        SetWindowPos(hwndRendererLogSplitter,HWND_TOP,cxClient + cxPostScriptLogWindow,CMD_PANE_HEIGHT,SPLITTER_WIDTH,cyHost - CMD_PANE_HEIGHT,SWP_SHOWWINDOW);

        if ( pPostScriptInterpreter -> logIsVisible )
            ShowWindow(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_RENDERER_LOG_SHOW),SW_HIDE);
        else {
            ShowWindow(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_RENDERER_LOG_SHOW),SW_SHOW);
            SetWindowText(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_RENDERER_LOG_SHOW),"Renderer Log >>");
        }

    } else {
        cxRendererLogWindow = 0;
        ShowWindow(hwndRendererLogContent,SW_HIDE);
        ShowWindow(hwndRendererLogCmdPane,SW_HIDE);
        ShowWindow(hwndRendererLogSplitter,SW_HIDE);
        if ( ! pPostScriptInterpreter -> logIsVisible ) {
            SetWindowText(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_RENDERER_LOG_SHOW),"<< Renderer Log");
            ShowWindow(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_RENDERER_LOG_SHOW),SW_SHOW);
        }
    }

    cxClient = rcHost.right - rcHost.left - cxPostScriptLogWindow - cxRendererLogWindow;

    cxClientWindow = cxClient;
    cyClientWindow = cyHost - CMD_PANE_HEIGHT;

    SetWindowPos(hwndClient,HWND_TOP,0,CMD_PANE_HEIGHT,cxClientWindow,cyClientWindow,SWP_SHOWWINDOW);

    RECT rcText{0,0,0,0};
    GetWindowRect(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_LOG_SHOW),&rcText);
    SetWindowPos(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_LOG_SHOW),HWND_TOP,cxClient - (rcText.right - rcText.left),12,0,0,SWP_NOSIZE);

    if ( pPostScriptInterpreter -> logIsVisible ) {
        GetWindowRect(GetDlgItem(hwndPostScriptLogCmdPane,IDDI_CMD_PANE_RENDERER_LOG_SHOW),&rcText);
        SetWindowPos(GetDlgItem(hwndPostScriptLogCmdPane,IDDI_CMD_PANE_RENDERER_LOG_SHOW),HWND_TOP,cxPostScriptLogWindow - cxRightEdge - (rcText.right - rcText.left),36,0,0,SWP_NOSIZE);
    } else {
        GetWindowRect(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_RENDERER_LOG_SHOW),&rcText);
        SetWindowPos(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_RENDERER_LOG_SHOW),HWND_TOP,cxClient - (rcText.right - rcText.left),36,0,0,SWP_NOSIZE);
    }

    RedrawWindow(hwndHost,NULL,NULL,RDW_ERASE | RDW_ERASENOW | RDW_UPDATENOW | RDW_ALLCHILDREN);

    RECT rcRightEdge{cxHost - cxRightEdge,CMD_PANE_HEIGHT,cxHost,cyHost};

    HDC hdcFrame = ::GetDC(hwndHost);
    FillRect(hdcFrame,&rcRightEdge,(HBRUSH)(COLOR_3DFACE + 1));
    ::ReleaseDC(hwndHost,hdcFrame);

    graphicsState::SetSurface(hwndClient,1);

    return;
    }