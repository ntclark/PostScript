
#include "PostScript.h"
#include "PostScript objects/graphicsState.h"

    HDC PStoPDF::GetDC() {

    if ( ! ( NULL == hdcSurface ) )
        return hdcSurface;

    hdcSurface = ::GetDC(hwndClient);

    if ( 0 < activePageOrigin.y ) {
        RECT rcClient;
        GetWindowRect(hwndClient,&rcClient);
        SetWindowPos(hwndClient,HWND_TOP,0,0,rcClient.right - rcClient.left,activePageOrigin.y + initialCYClient,SWP_NOMOVE);
        SendMessage(hwndClient,WM_VSCROLL,MAKEWPARAM(SB_PAGEDOWN,0L),0L);
        GetClientRect(hwndClient,&rcClient);
        rcClient.right -= GetSystemMetrics(SM_CXVSCROLL);
        FillRect(hdcSurface,&rcClient,(HBRUSH)(COLOR_WINDOW + 1));
    }

    graphicsState::SetSurface(hwndClient,(long)(pageBitmaps.size() + 1));

//pJob -> currentGS() -> outlinePage();

    return hdcSurface;
    }


    void PStoPDF::CommitCurrentPage(long pageWidthPoints,long pageHeightPoints) {

    if ( NULL == hdcSurface )
        return;

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