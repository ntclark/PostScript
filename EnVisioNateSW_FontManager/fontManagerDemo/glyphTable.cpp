
#include "fontManagerDemo.h"

    void generateGlyphTable() {

    POINTF startPoint,endPoint;

    RECT rc;
    GetClientRect(hwndGlyphTableHost,&rc);

    cxRenderPane = rc.right - rc.left;
    cyRenderPane = rc.bottom - rc.top;

    calcTransforms(hwndGlyphTableHost);

    HDC hdc = GetDC(hwndGlyphTableHost);

    FLOAT pointSize;
    pIFont -> get_PointSize(&pointSize);


    FLOAT extraWidthPoints = (FLOAT)GLYPH_INNERMARGIN_LEFT  / fabsf(gdiXForm.eM11);
    FLOAT extraHeightPoints = (FLOAT)GLYPH_INNERMARGIN_BOTTOM / fabsf(gdiXForm.eM22);

    FLOAT fontDeltaXPoints = pointSize + extraWidthPoints;
    FLOAT fontDeltaYPoints = pointSize + extraHeightPoints;

    FLOAT fontDeltaXPixels = fontDeltaXPoints * fabsf(gdiXForm.eM11);
    FLOAT fontDeltaYPixels = fontDeltaYPoints * fabsf(gdiXForm.eM22);;

    FLOAT deltaInPagePoints = (FLOAT)cyRenderPane / fabsf(gdiXForm.eM22) - fontDeltaYPoints;

    startPoint.x = 0.0f;
    startPoint.y = deltaInPagePoints;

    std::list<HBITMAP> pageBitmaps;

    RECT rcDrawing{0,0,cxRenderPane,cyRenderPane};
    HDC hdcBackground = CreateCompatibleDC(hdc);
    HBITMAP hbmPage = CreateCompatibleBitmap(hdcBackground,cxRenderPane,cyRenderPane);
    HGDIOBJ oldObj = SelectObject(hdcBackground,hbmPage);
    pIRenderer -> ClearRect(hdcBackground,&rcDrawing,WHITE);

    HFONT hGUIFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    HGDIOBJ oldFont = SelectObject(hdcBackground,hGUIFont);

    for ( unsigned short p = 0x21; p < 0x4FB; p++ ) {

        long x = (long)(startPoint.x * fabsf(gdiXForm.eM11));
        long y = (long)gdiXForm.eDy - (long)((startPoint.y + fontDeltaYPoints) * fabsf(gdiXForm.eM22));

        MoveToEx(hdcBackground,x,y,NULL);
        LineTo(hdcBackground,x + (long)fontDeltaXPixels,y);
        LineTo(hdcBackground,x + (long)fontDeltaXPixels,y + (long)fontDeltaYPixels);
        LineTo(hdcBackground,x,y + (long)fontDeltaYPixels);
        LineTo(hdcBackground,x,y);

        char szGlyph[16];
        sprintf_s<16>(szGlyph,"0x%04x",p);
        RECT rcText{x + 2,y + (long)fontDeltaYPixels - 16,x + (long)fontDeltaXPixels - 2,y + (long)fontDeltaYPixels};

        DrawTextEx(hdcBackground,szGlyph,-1,&rcText,DT_CENTER,0L);

        POINTF centerPoint{startPoint.x + extraWidthPoints,startPoint.y + extraHeightPoints};

        pIFont -> RenderGlyph((unsigned short)p,(UINT_PTR)&psXForm,(UINT_PTR)&gdiXForm,&centerPoint,&endPoint);

        endPoint.y = startPoint.y;
        endPoint.x = startPoint.x + fontDeltaXPoints;

        if ( 0 == ( ( p - 0x21 + 1 ) % GLYPH_TABLE_COLUMN_COUNT ) ) {
            endPoint.y -= fontDeltaYPoints;
            endPoint.x = 0.0f;
        }

        startPoint = endPoint;

        if ( 0.0f < startPoint.y )
            continue;

        pIRenderer -> Render(hdcBackground,&rcDrawing);

        SelectObject(hdcBackground,oldObj);

        pageBitmaps.push_back(hbmPage);

        hbmPage = CreateCompatibleBitmap(hdcBackground,cxRenderPane,cyRenderPane);
        oldObj = SelectObject(hdcBackground,hbmPage);
        pIRenderer -> ClearRect(hdcBackground,&rcDrawing,WHITE);

        startPoint.x = 0.0f;
        startPoint.y = deltaInPagePoints;

    }

    if ( 0.0f < startPoint.y ) {

        pIRenderer -> Render(hdcBackground,&rcDrawing);

        SelectObject(hdcBackground,oldObj);

        pageBitmaps.push_back(hbmPage);

        hbmPage = CreateCompatibleBitmap(hdcBackground,cxRenderPane,cyRenderPane);
        oldObj = SelectObject(hdcBackground,hbmPage);
        pIRenderer -> ClearRect(hdcBackground,&rcDrawing,WHITE);
    }

    long cyBitmap = cyRenderPane * (long)pageBitmaps.size();

    if ( ! ( NULL == hbmGlyphTable ) )
        DeleteObject(hbmGlyphTable);

    if ( ! ( NULL == hdcGlyphTable ) )
        DeleteDC(hdcGlyphTable);

    hbmGlyphTable = CreateCompatibleBitmap(hdcBackground,cxRenderPane,cyBitmap);
    hdcGlyphTable = CreateCompatibleDC(hdc);
    HGDIOBJ oldObj2 = SelectObject(hdcGlyphTable,hbmGlyphTable);

    long yLast = 0;

    for ( HBITMAP bmpPage : pageBitmaps ) {
        HGDIOBJ oldObj2 = SelectObject(hdcBackground,bmpPage);
        BOOL rc = BitBlt(hdcGlyphTable,0,yLast,cxRenderPane,cyRenderPane,hdcBackground,0,0,SRCCOPY);
        yLast += cyRenderPane;
    }

    DeleteDC(hdcBackground);

    RECT rcAdjust{0,0,0,0};

    scrollInfo.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
    scrollInfo.nPage = cyRenderPane;
    scrollInfo.nPos = 0;
    scrollInfo.nMin = 0;
    scrollInfo.nMax = cyRenderPane * (int)pageBitmaps.size();

    SetScrollInfo(hwndGlyphTableDialog,SBS_VERT,&scrollInfo,TRUE);

    InvalidateRect(hwndGlyphTableHost,NULL,TRUE);

    ReleaseDC(hwndGlyphTableHost,hdc);

    return;
    }