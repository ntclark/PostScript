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

    static POINT ptDragStart;
    static boolean isCaptured = false;
    static TRACKMOUSEEVENT trackMouseEvent;

    long windowTop = 0L;

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
        p -> setWindowPanes(NULL);
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
        return (LRESULT)TRUE;
        }

    case WM_SIZE: {
        if ( 0 == HIWORD(lParam) )
            break;
        if ( -1L == PostScriptInterpreter::cyClientWindow ) {
            PostScriptInterpreter::cxClientWindow = LOWORD(lParam);
            PostScriptInterpreter::cyClientWindow = HIWORD(lParam);
        }
        SetWindowPos(hwndVScroll,HWND_TOP,LOWORD(lParam) - GetSystemMetrics(SM_CXVSCROLL),0,
                                GetSystemMetrics(SM_CXVSCROLL),PostScriptInterpreter::cyClientWindow,0L);
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

    case WM_FLUSH_LOG: {
        return 0L;
        }
        break;

    default:
        break;
    }

    return DefWindowProc(hwnd,msg,wParam,lParam);
    }


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
        FillRect(ps.hdc,&ps.rcPaint,(HBRUSH)(COLOR_3DFACE + 1));
        EndPaint(hwnd,&ps);
        return (LRESULT)TRUE;
        }

    case WM_MOUSELEAVE:
    case WM_LBUTTONUP:
        if ( isCaptured )
            setWindowPanes(NULL);
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

            POINT ptNew{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
            MapWindowPoints(hwnd,hwndClient,&ptNew,1);

            long deltaX = ptNew.x - ptDragStart.x;
            ptDragStart = ptNew;

            if ( hwnd == hwndPostScriptLogSplitter ) {
                pPostScriptInterpreter -> postScriptLogPaneWidth -= deltaX;
            } else if ( hwnd == hwndRendererLogSplitter ) {
                pPostScriptInterpreter -> rendererLogPaneWidth -= deltaX;
            }

            setWindowPanes(NULL);

            SetCursor(LoadCursor(NULL,IDC_SIZEWE));
        }
        }
        break;

    default:
        break;
    }

    return DefWindowProc(hwnd,msg,wParam,lParam);
    }