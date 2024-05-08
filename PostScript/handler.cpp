// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PostScript.h"

    LRESULT CALLBACK PStoPDF::handler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    PStoPDF *p = (PStoPDF *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch ( msg ) {
    case WM_CREATE: {
        CREATESTRUCT *pc = reinterpret_cast<CREATESTRUCT *>(lParam);
        p = reinterpret_cast<PStoPDF *>(pc -> lpCreateParams);
        SetWindowLongPtr(hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(p));
        }
        break;

    case WM_REFRESH_CLIENT_WINDOW:
        hostFrameHandlerOveride(hwnd,WM_MOVE,0L,0L);
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd,&ps);
        FillRect(ps.hdc,&ps.rcPaint,(HBRUSH)(COLOR_WINDOW + 1));
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
#if 0
        RECT rcHost;
        HWND hwndHost;
        pPStoPDF -> pIOleInPlaceSite -> GetWindow(&hwndHost);
        GetWindowRect(hwndHost,&rcHost);
        long cx = rcHost.right - rcHost.left - SPLITTER_WIDTH;
        long cy = rcHost.bottom - rcHost.top;
        long cxClient = cx - (pPStoPDF -> logIsVisible ? pPStoPDF -> logPaneWidth : 0);
        SetWindowPos(hwndClient,HWND_TOP,0,CMD_PANEL_HEIGHT,cxClient,cy - CMD_PANEL_HEIGHT,0L);
        SetWindowPos(hwndCmdPane,HWND_TOP,rcHost.left,rcHost.top ,cxClient,CMD_PANEL_HEIGHT,0L);
        if ( 0 < pPStoPDF -> logPaneWidth && pPStoPDF -> logIsVisible ) {
            SetWindowPos(hwndLog,HWND_TOP,cxClient,0,pPStoPDF -> logPaneWidth,cy,SWP_SHOWWINDOW);
            SetWindowPos(hwndLogSplitter,HWND_TOP,cxClient,0,32,cy,SWP_SHOWWINDOW);
            SetWindowText(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_LOG_SHOW),"Log >>");
        } else {
            ShowWindow(hwndLog,SW_HIDE);
            ShowWindow(hwndLogSplitter,SW_HIDE);
            SetWindowText(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_LOG_SHOW),"<< Log");
        }
#endif
        }
        break;

    default:
        break;

    }

    return PStoPDF::nativeHostFrameHandler(hwnd,msg,wParam,lParam);
    }