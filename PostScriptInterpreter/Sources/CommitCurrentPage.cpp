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
#include "PostScript objects/graphicsState.h"

    HDC PostScriptInterpreter::GetDC() {

    if ( ! ( NULL == hdcSurface ) )
        return hdcSurface;

    hdcSurface = ::GetDC(hwndClient);

    if ( 0 < activePageOrigin.y ) {

        beginPathAction = [this]() { 

            beginPathAction = NULL;

            RECT rcClient;
            GetWindowRect(hwndClient,&rcClient);
            SetWindowPos(hwndClient,HWND_TOP,0,0,rcClient.right - rcClient.left,activePageOrigin.y + initialCYClient,SWP_NOMOVE);
            SendMessage(hwndClient,WM_VSCROLL,MAKEWPARAM(SB_PAGEDOWN,0L),0L);
            GetClientRect(hwndClient,&rcClient);
            rcClient.right -= GetSystemMetrics(SM_CXVSCROLL);
            FillRect(hdcSurface,&rcClient,(HBRUSH)(COLOR_WINDOW + 1));

            graphicsState::SetSurface(hwndClient,(long)(pageBitmaps.size() + 1));
        };

    }

    return hdcSurface;
    }


    void PostScriptInterpreter::BeginPath() {
    if ( ! ( NULL == beginPathAction ) )
        beginPathAction();
    return;
    }


    void PostScriptInterpreter::PotentialNewPage() {
    if ( ! ( NULL == beginPathAction ) )
        beginPathAction();
    return;
    }


    void PostScriptInterpreter::CommitCurrentPage(long pageWidthPoints,long pageHeightPoints) {

    if ( NULL == hdcSurface )
        return;

    graphicsState::RenderGeometry();

    ReleaseDC(hwndClient,hdcSurface);

    hdcSurface = ::GetDC(hwndClient);

    HDC hdcTarget = CreateCompatibleDC(hdcSurface);

    long cxClient = (long)((double)(pageWidthPoints * initialCYClient) / (double)pageHeightPoints);

    SIZEL *pSizel = new SIZEL;
    pSizel -> cx = cxClient;
    pSizel -> cy = initialCYClient;

    HBITMAP hbmPage = CreateCompatibleBitmap(hdcSurface,cxClient,initialCYClient);

    SelectObject(hdcTarget,hbmPage);

    BitBlt(hdcTarget,0,0,cxClient,initialCYClient,hdcSurface,0,0,SRCCOPY);

    pageBitmaps[pageBitmaps.size() + 1] = hbmPage;
    pageSizes[pageBitmaps.size()] = pSizel;

    DeleteDC(hdcTarget);

    activePageOrigin.y += initialCYClient;

    ReleaseDC(hwndClient,hdcSurface);

    hdcSurface = NULL;

    return;
    }