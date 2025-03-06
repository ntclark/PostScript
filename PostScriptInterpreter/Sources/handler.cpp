
#include "PostScript.h"

#include "job.h"

#define SCROLL_DELTA 16

    long windowTop = 0L;

    LRESULT CALLBACK PStoPDF::handler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    PStoPDF *p = (PStoPDF *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch ( msg ) {
    case WM_CREATE: {
        CREATESTRUCT *pc = reinterpret_cast<CREATESTRUCT *>(lParam);
        p = reinterpret_cast<PStoPDF *>(pc -> lpCreateParams);
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
        FillRect(ps.hdc,&ps.rcPaint,(HBRUSH)(COLOR_WINDOW + 1));

        long y = windowTop;
        for ( std::pair<size_t,HBITMAP> pPair : pPStoPDF -> pageBitmaps ) {
            SIZEL *pSizel = pPStoPDF -> pageSizes[pPair.first];
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
        RECT rcClient;
        GetClientRect(hwnd,&rcClient);
        if ( 0 == (rcClient.bottom - rcClient.top) ) 
            break;
        if ( -1L == PStoPDF::initialCYClient )
            PStoPDF::initialCYClient = rcClient.bottom - rcClient.top;
        SetWindowPos(hwndVScroll,HWND_TOP,rcClient.right - GetSystemMetrics(SM_CXVSCROLL),0,GetSystemMetrics(SM_CXVSCROLL),PStoPDF::initialCYClient,0L);
        SCROLLINFO scrollInfo{0};
        scrollInfo.cbSize = sizeof(SCROLLINFO);
        scrollInfo.fMask = SIF_PAGE | SIF_RANGE;
        scrollInfo.nPage = PStoPDF::initialCYClient;
        scrollInfo.nMin = 0;
        scrollInfo.nMax = rcClient.bottom - rcClient.top;
        SetScrollInfo(hwndVScroll,SB_CTL,&scrollInfo,TRUE);
        }
        break;

    /*
        Doesn't work at this point, probably the hosting frame get's the keystrokes
    */
    case WM_KEYDOWN: {
        switch ( (char)wParam ) {
        case VK_PRIOR:
            handler(hwnd,WM_VSCROLL,MAKEWPARAM(SB_PAGEUP,0L),0L);
            break;
        case VK_NEXT:
            handler(hwnd,WM_VSCROLL,MAKEWPARAM(SB_PAGEDOWN,0L),0L);
            break;
        case VK_UP:
            handler(hwnd,WM_VSCROLL,MAKEWPARAM(SB_LINEUP,0L),0L);
            break;
        case VK_DOWN:
            handler(hwnd,WM_VSCROLL,MAKEWPARAM(SB_LINEDOWN,0L),0L);
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

        if ( 0 == yDelta )
            break;

        if ( 0 > yDelta && scrollInfo.nMax <= (long)scrollInfo.nPage - windowTop ) 
            break;

        if ( 0 > yDelta && scrollInfo.nMax <= (long)scrollInfo.nPage - windowTop - yDelta ) 
            yDelta = scrollInfo.nPage - windowTop - scrollInfo.nMax;

        if ( 0 < (windowTop + yDelta ) )
            yDelta = -windowTop;

        windowTop += yDelta;

        if ( 0 < windowTop ) {
            windowTop = 0;
            break;
        } 

        ScrollWindowEx(hwnd,0L,yDelta,NULL,NULL,NULL,NULL,SW_ERASE);

        scrollInfo.fMask = SIF_POS;
        scrollInfo.nPos -= yDelta;

        SetScrollInfo(hwndVScroll,SB_CTL,&scrollInfo,TRUE);

        InvalidateRect(hwnd,NULL,TRUE);
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


    static POINT ptDragStart;
    static boolean isCaptured = false;
    static TRACKMOUSEEVENT trackMouseEvent;

    LRESULT CALLBACK PStoPDF::splitterHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    PStoPDF *p = (PStoPDF *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch ( msg ) {
    case WM_CREATE: {
        CREATESTRUCT *pc = reinterpret_cast<CREATESTRUCT *>(lParam);
        p = reinterpret_cast<PStoPDF *>(pc -> lpCreateParams);
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
            POINT ptNew{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
            MapWindowPoints(hwnd,hwndClient,&ptNew,1);
            long deltaX = ptNew.x - ptDragStart.x;
            ptDragStart = ptNew;
            pPStoPDF -> logPaneWidth -= deltaX;

            SET_PANES

            SetCursor(LoadCursor(NULL,IDC_SIZEWE));
        }
        }
        break;

    default:
        break;
    }

    return DefWindowProc(hwnd,msg,wParam,lParam);
    }


    LRESULT CALLBACK PStoPDF::hostFrameHandlerOveride(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    switch ( msg ) {

    case WM_SIZE:
    case WM_MOVE: {
        SET_PANES
        }
        break;

    default:
        break;

    }

    return PStoPDF::nativeHostFrameHandler(hwnd,msg,wParam,lParam);
    }