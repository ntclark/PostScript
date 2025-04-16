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

#define SCROLL_DELTA 16

    long windowTop = 0L;

    boolean awaitingClientPaint = false;

    LRESULT CALLBACK PostScriptInterpreter::clientWindowHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    PostScriptInterpreter *p = (PostScriptInterpreter *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch ( msg ) {
    case WM_CREATE: {
        CREATESTRUCT *pc = reinterpret_cast<CREATESTRUCT *>(lParam);
        p = reinterpret_cast<PostScriptInterpreter *>(pc -> lpCreateParams);
        SetWindowLongPtr(hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(p));
        windowTop = 0L;
        }
        break;

    case WM_REFRESH_CLIENT_WINDOW:
        hostFrameHandlerOveride(hwnd,WM_MOVE,0L,0L);
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd,&ps);
        ps.rcPaint.right -= GetSystemMetrics(SM_CXVSCROLL);
        //if ( 0 == pPostScriptInterpreter -> pageBitmaps.size() )
            FillRect(ps.hdc,&ps.rcPaint,(HBRUSH)(COLOR_APPWORKSPACE + 1));
        long y = windowTop;
        for ( std::pair<size_t,HBITMAP> pPair : pPostScriptInterpreter -> pageBitmaps ) {
            SIZEL *pSizel = pPostScriptInterpreter -> pageSizes[pPair.first];
            HDC hdcSource = CreateCompatibleDC(ps.hdc);
            HGDIOBJ oldObj = SelectObject(hdcSource,pPair.second);
            BitBlt(ps.hdc,0,y,pSizel -> cx,pSizel -> cy,hdcSource,0,0,SRCCOPY);
            SelectObject(hdcSource,oldObj);
            DeleteDC(hdcSource);
            y += pSizel -> cy;
        }
        EndPaint(hwnd,&ps);
        awaitingClientPaint = false;
        return (LRESULT)TRUE;
        }

    case WM_DROPFILES: {
        char szFiles[MAX_PATH];
        HDROP hDropInfo = reinterpret_cast<HDROP>(wParam);
        for ( long k = 0; k < (long)DragQueryFile(hDropInfo,0xFFFFFFFF,NULL,0L); k++ ) {
            DragQueryFile(hDropInfo,k,szFiles,MAX_PATH);
            p -> Parse(szFiles);
            break;
        }
        }
        break;

    case WM_SIZE: {
        if ( 0 == HIWORD(lParam) )
            break;
        if ( -1L == PostScriptInterpreter::cyClientWindow ) {
            PostScriptInterpreter::cxClientWindow = LOWORD(lParam);
            PostScriptInterpreter::cyClientWindow = HIWORD(lParam);
        }
        SetWindowPos(hwndVScroll,HWND_TOP,LOWORD(lParam) - GetSystemMetrics(SM_CXVSCROLL),0,GetSystemMetrics(SM_CXVSCROLL),PostScriptInterpreter::cyClientWindow,0L);
        SCROLLINFO scrollInfo{0};
        scrollInfo.cbSize = sizeof(SCROLLINFO);
        scrollInfo.fMask = SIF_PAGE | SIF_RANGE;
        scrollInfo.nPage = PostScriptInterpreter::cyClientWindow;
        scrollInfo.nMin = 0;
        if ( 0 == pPostScriptInterpreter -> pageBitmaps.size() )
            scrollInfo.nMax = scrollInfo.nPage;
        else
            scrollInfo.nMax = HIWORD(lParam);
        SetScrollInfo(hwndVScroll,SB_CTL,&scrollInfo,TRUE);
        }
        break;

    /*
        Doesn't work at this point, probably the hosting frame get's the keystrokes
    */
    case WM_KEYDOWN: {
        switch ( (char)wParam ) {
        case VK_PRIOR:
            clientWindowHandler(hwnd,WM_VSCROLL,MAKEWPARAM(SB_PAGEUP,0L),0L);
            break;
        case VK_NEXT:
            clientWindowHandler(hwnd,WM_VSCROLL,MAKEWPARAM(SB_PAGEDOWN,0L),0L);
            break;
        case VK_UP:
            clientWindowHandler(hwnd,WM_VSCROLL,MAKEWPARAM(SB_LINEUP,0L),0L);
            break;
        case VK_DOWN:
            clientWindowHandler(hwnd,WM_VSCROLL,MAKEWPARAM(SB_LINEDOWN,0L),0L);
            break;
        default:
            break;
        }
        }
        break;

    case WM_MOUSEWHEEL:
    case WM_VSCROLL: {

        SCROLLINFO scrollInfo{0};

        scrollInfo.cbSize = sizeof(SCROLLINFO);
        scrollInfo.fMask = SIF_ALL;
        GetScrollInfo(hwndVScroll,SB_CTL,&scrollInfo);

        long yDelta = 0L;

        if ( WM_MOUSEWHEEL == msg ) {

            yDelta = GET_WHEEL_DELTA_WPARAM(wParam);

        } else {

            switch ( LOWORD(wParam) ) {
            case SB_LINEDOWN:
                yDelta -= SCROLL_DELTA;
                break;

            case SB_LINEUP:
                yDelta += SCROLL_DELTA;
                break;

            case SB_PAGEDOWN:
                yDelta = -1 * scrollInfo.nPage;
                break;

            case SB_PAGEUP:
                yDelta = scrollInfo.nPage;
                break;

            case SB_THUMBTRACK:
                yDelta = scrollInfo.nPos - HIWORD(wParam);
                break;

            default:
                break;
            }

        }

        if ( 0 == yDelta ) {
            if ( ! ( INVALID_HANDLE_VALUE == hsemSized) ) 
                ReleaseSemaphore(hsemSized,1,NULL);
            break;
        }

        if ( 0 > yDelta && scrollInfo.nMax <= (long)scrollInfo.nPage - windowTop ) {
            if ( ! ( INVALID_HANDLE_VALUE == hsemSized) ) 
                ReleaseSemaphore(hsemSized,1,NULL);
            break;
        }

        if ( 0 > yDelta && scrollInfo.nMax <= (long)scrollInfo.nPage - windowTop - yDelta ) 
            yDelta = scrollInfo.nPage - windowTop - scrollInfo.nMax;

        if ( 0 < (windowTop + yDelta ) )
            yDelta = -windowTop;

        windowTop += yDelta;

        if ( 0 < windowTop ) {
            windowTop = 0;
            if ( ! ( INVALID_HANDLE_VALUE == hsemSized) ) 
                ReleaseSemaphore(hsemSized,1,NULL);
            break;
        } 

        ScrollWindowEx(hwnd,0L,yDelta,NULL,NULL,NULL,NULL,SW_ERASE);

        scrollInfo.fMask = SIF_POS;
        scrollInfo.nPos -= yDelta;

        SetScrollInfo(hwndVScroll,SB_CTL,&scrollInfo,TRUE);

        InvalidateRect(hwnd,NULL,TRUE);

        if ( ! ( INVALID_HANDLE_VALUE == hsemSized) ) {
            UpdateWindow(hwnd);
            ReleaseSemaphore(hsemSized,1,NULL);
        }
        }
        break;

    case WM_TIMER:
        awaitingClientPaint = false;
        KillTimer(hwnd,wParam);
        break;

    case WM_FLUSH_LOG: {
        return 0L;
        }
        break;

    default:
        break;
    }

    return DefWindowProc(hwnd,msg,wParam,lParam);
    }


    static POINT ptDragStart;
    static boolean isCaptured = false;
    static TRACKMOUSEEVENT trackMouseEvent;

    LRESULT CALLBACK PostScriptInterpreter::splitterHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    PostScriptInterpreter *p = (PostScriptInterpreter *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch ( msg ) {
    case WM_CREATE: {
        CREATESTRUCT *pc = reinterpret_cast<CREATESTRUCT *>(lParam);
        p = reinterpret_cast<PostScriptInterpreter *>(pc -> lpCreateParams);
        SetWindowLongPtr(hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(p));
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd,&ps);
        EndPaint(hwnd,&ps);
        return (LRESULT)TRUE;
        }

    case WM_MOUSELEAVE:
    case WM_LBUTTONUP:
        isCaptured = false;
        ReleaseCapture();
        SetCursor(LoadCursor(NULL,IDC_ARROW));
        break;

    case WM_LBUTTONDOWN: {
        POINT ptMouse{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
        ptDragStart = ptMouse;
        MapWindowPoints(hwnd,hwndClient,&ptDragStart,1);
        isCaptured = false;
        }

    case WM_MOUSEMOVE: {

        if ( msg == WM_MOUSEMOVE && 0 == ( MK_LBUTTON & wParam) ) {
            SetCursor(LoadCursor(NULL,IDC_SIZEWE));
            memset(&trackMouseEvent,0,sizeof(TRACKMOUSEEVENT));
            trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
            trackMouseEvent.dwFlags = TME_LEAVE;
            trackMouseEvent.hwndTrack = hwnd;
            TrackMouseEvent(&trackMouseEvent);
            break;
        }

        if ( msg == WM_MOUSEMOVE && ! ( 0 == ( MK_LBUTTON & wParam) ) ) {

            if ( ! isCaptured ) {
                SetCapture(hwnd);
                isCaptured = true;
            }

            if ( awaitingClientPaint )
                break;

            POINT ptNew{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
            MapWindowPoints(hwnd,hwndClient,&ptNew,1);

            long deltaX = ptNew.x - ptDragStart.x;
            ptDragStart = ptNew;

            if ( hwnd == hwndLogSplitter )
                pPostScriptInterpreter -> logPaneWidth -= deltaX;
            else if ( hwnd == hwndRendererLogSplitter )
                pPostScriptInterpreter -> rendererLogPaneWidth -= deltaX;

            awaitingClientPaint = true;

            SetTimer(hwndClient,1,500,NULL);

            setWindowPanes();

            SetCursor(LoadCursor(NULL,IDC_SIZEWE));
        }
        }
        break;

    default:
        break;
    }

    return DefWindowProc(hwnd,msg,wParam,lParam);
    }


    LRESULT CALLBACK PostScriptInterpreter::hostFrameHandlerOveride(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    switch ( msg ) {

    case WM_SIZE:
    case WM_MOVE: 
        setWindowPanes();
        break;

    default:
        break;

    }

    return PostScriptInterpreter::nativeHostFrameHandler(hwnd,msg,wParam,lParam);
    }


    void PostScriptInterpreter::setWindowPanes() {

    RECT rcHost;
    GetWindowRect(hwndHost,&rcHost);
    long cx = rcHost.right - rcHost.left;
    long cy = rcHost.bottom - rcHost.top;

    long cxClient = cx - (pPostScriptInterpreter -> logIsVisible ? pPostScriptInterpreter -> logPaneWidth : 0);
    cxClient -= (pPostScriptInterpreter -> rendererLogIsVisible ? pPostScriptInterpreter -> rendererLogPaneWidth : 0);
    cxClient -= 8;

    SetWindowPos(hwndClient,HWND_TOP,0,CMD_PANE_HEIGHT,cxClient,cy - CMD_PANE_HEIGHT,0L);
    SetWindowPos(hwndCmdPane,HWND_TOP,rcHost.left,rcHost.top,cxClient,CMD_PANE_HEIGHT,SWP_SHOWWINDOW);

    long cxLogWindow = 0;

    if ( 0 < pPostScriptInterpreter -> logPaneWidth && pPostScriptInterpreter -> logIsVisible ) {
        cxLogWindow = pPostScriptInterpreter -> logPaneWidth;
        SetWindowPos(hwndLogContent,HWND_TOP,cxClient,LOG_PANE_HEIGHT,cxLogWindow,cy - LOG_PANE_HEIGHT,SWP_SHOWWINDOW);
        SetWindowPos(hwndLogPane,HWND_TOP,rcHost.left + cxClient,rcHost.top,pPostScriptInterpreter -> logPaneWidth,LOG_PANE_HEIGHT,SWP_SHOWWINDOW);
        SetWindowPos(hwndLogSplitter,HWND_TOP,cxClient - SPLITTER_WIDTH / 2,0,SPLITTER_WIDTH,cy,SWP_SHOWWINDOW);
        SetWindowText(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_LOG_SHOW),"PS Log >>");
    } else {
        cxLogWindow = 0;
        ShowWindow(hwndLogContent,SW_HIDE);
        ShowWindow(hwndLogPane,SW_HIDE);
        ShowWindow(hwndLogSplitter,SW_HIDE);
        SetWindowText(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_LOG_SHOW),"<< PS Log");
    }

    if ( 0 < pPostScriptInterpreter -> rendererLogPaneWidth && pPostScriptInterpreter -> rendererLogIsVisible ) {
        SetWindowPos(hwndRendererLogContent,HWND_TOP,cxClient + cxLogWindow,0,pPostScriptInterpreter -> rendererLogPaneWidth,cy,SWP_SHOWWINDOW);
        SetWindowPos(hwndRendererLogSplitter,HWND_TOP,cxClient + cxLogWindow - SPLITTER_WIDTH / 2,0,SPLITTER_WIDTH,cy,SWP_SHOWWINDOW);
        SetWindowText(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_RENDERER_LOG_SHOW),"Renderer Log >>");
    } else {
        ShowWindow(hwndRendererLogContent,SW_HIDE);
        ShowWindow(hwndRendererLogSplitter,SW_HIDE);
        SetWindowText(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_RENDERER_LOG_SHOW),"<< Renderer Log");
    }


    return;
    }