#include "rendererDemo.h"

#include <math.h>

double degToRad = 4.0 * atan(1.0) / 180.0;

RECT rcDrawing;
RECT rcEntire;
RECT rcStatus;
boolean renderLive = false;

#define MARGIN_LEFT_PIXELS 96
#define MARGIN_RIGHT_PIXELS 32
#define MARGIN_BOTTOM_PIXELS 32
#define MARGIN_TOP_PIXELS 32

HBITMAP getSampleImage(HDC hdc);

    LRESULT frameHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {
    switch ( msg ) {
    case WM_CREATE:
        hwndFrame = hwnd;
        break;

    case WM_SYSCOMMAND: {
        if ( wParam == SC_CLOSE ) {
            PostQuitMessage(0);
            return (LRESULT)0;
        }
        }
        break;

    case WM_SIZE: {
        RECT rcStatus;
        GetClientRect(hwndStatus,&rcStatus);
        SetWindowPos(hwndStatus,HWND_TOP,0,HIWORD(lParam) - (rcStatus.bottom - rcStatus.top),LOWORD(lParam),(rcStatus.bottom - rcStatus.top),0L);
        InvalidateRect(hwnd,NULL,TRUE);
        }
        break;

    case WM_MOUSEMOVE: {
        char szLocation[64];
        FLOAT x,y;
        pIRenderer -> WhereAmI(LOWORD(lParam),HIWORD(lParam),&x,&y);
        sprintf_s<64>(szLocation,"x = %5.2f, y = %5.2f",x,y);
        SetWindowText(hwndStatus,szLocation);
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd,&ps);
        FillRect(ps.hdc,&ps.rcPaint,(HBRUSH)(COLOR_WINDOW + 1));
        crossHairs(ps.hdc);
        EndPaint(hwnd,&ps);
        }
        break;

    default:
        break;
    }


    return DefWindowProc(hwnd,msg,wParam,lParam);
    }


    void crossHairs(HDC hdc) {

    GetClientRect(hwndFrame,&rcEntire);
    GetClientRect(hwndStatus,&rcStatus);

    long cyStatus = rcStatus.bottom - rcStatus.top;

    rcDrawing = rcEntire;

    renderLive = (BST_CHECKED == SendMessage(GetDlgItem(hwndDialog,IDDI_RENDER_LIVE),BM_GETCHECK,0L,0L));

    if ( renderLive )
        pIRenderer -> SetRenderLive(hdc,&rcEntire);
    else
        pIRenderer -> UnSetRenderLive();

    // The domain should be (0->100) x by (0->100) y
    // The device coordinates are {rcDrawing.left,rcDrawing.top} by {rcDrawing.right,rcDrawing.bottom} pixels
    // I want to draw in {rcDrawing.left + MARGIN_LEFT,rcDrawing.top + MARGIN_TOP} by 
    //          {rcDrawing.right - MARGIN_RIGHT,rcDrawing.bottom - MARGIN'BOTTOM - statusBarHeight}
    // Therefore, the SCALING should be calculated from the dimensions of THAT box (that I'm drawing in)

    rcDrawing.left += MARGIN_LEFT_PIXELS;
    rcDrawing.right -= MARGIN_RIGHT_PIXELS;
    rcDrawing.top += MARGIN_TOP_PIXELS;
    rcDrawing.bottom -= (MARGIN_BOTTOM_PIXELS + cyStatus);

    long shaveHeight = 0;
    long shaveWidth = 0;

    if ( BST_CHECKED == SendMessage(GetDlgItem(hwndDialog,IDDI_KEEP_SQUARE),BM_GETCHECK,0L,0L) ) {
        if ( (rcDrawing.right - rcDrawing.left) < (rcDrawing.bottom - rcDrawing.top) )
            shaveHeight = (rcDrawing.bottom - rcDrawing.top) - (rcDrawing.right - rcDrawing.left);
        else
            shaveWidth = (rcDrawing.right - rcDrawing.left) - (rcDrawing.bottom - rcDrawing.top);
        rcDrawing.left += shaveWidth / 2;
        rcDrawing.right -= shaveWidth / 2;
        rcDrawing.top += shaveHeight / 2;
        rcDrawing.bottom -= shaveHeight / 2;
    }

    pIRenderer -> ClearRect(hdc,&rcDrawing,graphBackgroundColor);

    FLOAT cxDrawingF = (FLOAT)(rcDrawing.right - rcDrawing.left);
    FLOAT cyDrawingF = (FLOAT)(rcDrawing.bottom - rcDrawing.top);

    xFormToPixels.eM11 = cxDrawingF / 100.0f;
    xFormToPixels.eM22 = -cyDrawingF / 100.0f;

    // HOWEVER, the eDy component of the transformation should be 
    // from the TOP of the actual window, to the BOTTOM of the
    // desired rendering box, therefore, put TOP margin
    // back in the height value.

    xFormToPixels.eDx = (FLOAT)MARGIN_LEFT_PIXELS;
    if ( 0 < shaveWidth )
        xFormToPixels.eDx += shaveWidth / 2.0f;

    xFormToPixels.eDy = cyDrawingF + (FLOAT)MARGIN_TOP_PIXELS;
    if ( 0 < shaveHeight )
        xFormToPixels.eDy += shaveHeight/ 2.0f;

    pIRenderer -> put_TransformMatrix((UINT_PTR)&xFormToPixels);

    pIGraphicParameters -> put_RGBColor(RGB(0,0,0));

    FLOAT x = 0.0f;
    FLOAT y = 0.0f;

    FLOAT deltaX = 100.0f / (ticCount - 1.0f);
    FLOAT deltaY = 100.0f / (ticCount - 1.0f);
    FLOAT xTicSize = deltaX / 4.0f;
    FLOAT yTicSize = deltaY / 4.0f;

    boolean useGrid = (BST_CHECKED == SendMessage(GetDlgItem(hwndDialog,IDDI_SHOW_GRID),BM_GETCHECK,0L,0L));

    pIGraphicElements -> NewPath();

    pIGraphicElements -> MoveTo(0.0f,0.0f);
    pIGraphicElements -> LineToRelative(100.0f,0.0f);
    pIGraphicElements -> MoveTo(0.0f,0.0f);
    pIGraphicElements -> LineToRelative(0.0f,100.0f);

    char szTic[16];

    do {

        sprintf_s<16>(szTic,"%4.1f",x);

        pIGraphicElements -> MoveTo(x,xTicSize / 2.0f);
        pIGraphicElements -> LineToRelative(0.0f,-xTicSize);
        if ( useGrid )
            pIGraphicElements -> LineToRelative(0.0f,100.0f + xTicSize / 2.0f);

        pIGraphicElements -> StrokePath();
        drawText(hdc,(char *)szTic,x,-1.25f * xTicSize,DT_CENTER | DT_TOP);

        pIGraphicElements -> MoveTo(-yTicSize / 2,y);
        pIGraphicElements -> LineToRelative(yTicSize,0.0f);
        if ( useGrid )
            pIGraphicElements -> LineToRelative(100.0f - yTicSize / 2.0f,0.0f);

        pIGraphicElements -> StrokePath();
        drawText(hdc,(char *)szTic,-yTicSize,y,DT_RIGHT | DT_VCENTER);

        x += deltaX;
        y += deltaY;

    } while ( x <= 100.0f );

    pIGraphicElements -> Circle(60.0f,20.0f,1.0f);
    pIGraphicElements -> StrokePath();

    pIGraphicElements -> Ellipse(60.0f,20.0f,20.0f,10.0f);
    pIGraphicElements -> StrokePath();

    char szText[256];
    sprintf_s<256>(szText,"Ellipse at %4.0f,%4.0f (center), x-radius %4.0f, y-radius %4.0f",60.0f,20.0f,20.0f,10.0f);
    drawText(hdc,szText,65.0f,18.0f,DT_CENTER | DT_TOP);

    pIGraphicElements -> Arc(15.0f,-5.0f,20.0f,30.0f,125.0f);
    pIGraphicElements -> StrokePath();

    sprintf_s<256>(szText,"Arc at %4.0f,%4.0f (center), radius %4.0f, start angle %4.0f, end angle %4.0f",15.0f,-5.0f,20.0f,30.0f,125.0f);

    FLOAT x2 = 15.0f + 20.0f * (FLOAT)cos(30.0 * degToRad);
    FLOAT y2 = -5.0f + 20.0f * (FLOAT)sin(30.0 * degToRad);
    drawText(hdc,szText,x2,y2,DT_TOP);

    pIGraphicElements -> Circle(80.0f,50.0f,15.0f);
    pIGraphicElements -> StrokePath();

    sprintf_s<256>(szText,"Circle at %4.0f,%4.0f (center), radius %4.0f",80.0f,50.0f,15.0f);
    drawText(hdc,szText,80.0f,50.0f,DT_CENTER);

    pIGraphicElements -> MoveTo(65.0f,65.0f);
    pIGraphicElements -> LineTo(85.0f,80.0f);
    pIGraphicElements -> LineTo(75.0f,85.0f);

    pIGraphicElements -> ClosePath();

    sprintf_s<256>(szText,"ClosePath at 65, 65");
    drawText(hdc,szText,65.0f,62.50f,DT_CENTER);

    pIGraphicParameters -> put_RGBColor(RGB(255,0,0));

    POINTF bezierPoints[]{{5.0f,25.0f},{40.0f,30.0f},{20.0f,50.0f},{45.0f,55.0f}};

    pIGraphicElements -> Circle(bezierPoints[0].x,bezierPoints[0].y,0.5);
    pIGraphicElements -> FillPath();

    pIGraphicElements -> Circle(bezierPoints[1].x,bezierPoints[1].y,0.5);
    pIGraphicElements -> FillPath();

    pIGraphicElements -> Circle(bezierPoints[2].x,bezierPoints[2].y,0.5);
    pIGraphicElements -> FillPath();

    pIGraphicElements -> Circle(bezierPoints[3].x,bezierPoints[3].y,0.5);
    pIGraphicElements -> FillPath();

    pIGraphicElements -> CubicBezier(bezierPoints[0].x,bezierPoints[0].y,
                                        bezierPoints[1].x,bezierPoints[1].y,
                                            bezierPoints[2].x,bezierPoints[2].y,
                                                bezierPoints[3].x,bezierPoints[3].y);

    pIGraphicElements -> StrokePath();

    sprintf_s<256>(szText,"Cubic Bezier Curve at %4.0f, %4.0f",bezierPoints[0].x,bezierPoints[0].y);
    drawText(hdc,szText,bezierPoints[0].x,bezierPoints[0].y - 2.5f,0);

    pIGraphicParameters -> put_RGBColor(RGB(0,0,255));

    bezierPoints[0].y += 15.0f;
    bezierPoints[1].x = 5.0f;
    bezierPoints[1].y = 65.0f;
    bezierPoints[2].x = 35.0f;
    bezierPoints[2].y += 15.0f;

    pIGraphicElements -> Circle(bezierPoints[0].x,bezierPoints[0].y,0.5);
    pIGraphicElements -> FillPath();

    pIGraphicElements -> Circle(bezierPoints[1].x,bezierPoints[1].y,0.5);
    pIGraphicElements -> FillPath();

    pIGraphicElements -> Circle(bezierPoints[2].x,bezierPoints[2].y,0.5);
    pIGraphicElements -> FillPath();

    // Note that with a QuadraticBezier curve, it starts where the last primitive
    // left off. Therefore, you need to call MoveTo if you want the curve to start
    // elsewhere
    pIGraphicElements -> MoveTo(bezierPoints[0].x,bezierPoints[0].y);
    pIGraphicElements -> QuadraticBezier(bezierPoints[1].x,bezierPoints[1].y,bezierPoints[2].x,bezierPoints[2].y);

    pIGraphicElements -> StrokePath();

    sprintf_s<256>(szText,"Quadratic Bezier Curve at %4.0f, %4.0f",bezierPoints[0].x,bezierPoints[0].y);
    drawText(hdc,szText,bezierPoints[0].x,bezierPoints[0].y - 2.5f,0);

    HBITMAP hbmSample = getSampleImage(hdc);

    // Pass 0 the displayWidth value,to render in native size
    pIGraphicElements -> NonPostScriptImage(hdc,hbmSample,2.0f,90.0f,30.0f,30.0f);

    // Note carefully. Direct2D "binds" the device context to the RECT passed in.
    // If you calculated your user space to device space transformation
    // from a "sub" rectangle of the target window, then, when you 
    // call Render, you must use the "full" rectangle of the target window.
    // Otherwise, your scaling will be off because Direct2D rescales based
    // on the relationship been the sub and full rectangles (which is an 
    // effect you may want anyway).

    if ( ! renderLive )
        pIRenderer -> Render(hdc,&rcEntire);

    return;
    }


    void drawText(HDC hdc,char *pszText,FLOAT x,FLOAT y,uint8_t flag) {

    FLOAT pointSize;
    pIFont -> get_PointSize(&pointSize);

    xFormToPoints.eM11 = pointSize / 96.0f;
    xFormToPoints.eM22 = pointSize / 96.0f;

    POINTF startPoint,endPoint;

    startPoint.x = x;
    startPoint.y = y;

    char *p = pszText;

    while ( ' ' == *p && *p )
        p++;

    if ( flag & DT_CENTER )
        startPoint.x -= (DWORD)strlen(p) * pointSize * xFormToPoints.eM11 / 4.0f;

    if ( flag & DT_TOP )
        startPoint.y -= pointSize * xFormToPoints.eM22;

    if ( flag & DT_RIGHT )
        startPoint.x -= (DWORD)strlen(p) * pointSize * xFormToPoints.eM11 / 2.0f;

    if ( flag & DT_VCENTER )
        startPoint.y -= pointSize * xFormToPoints.eM22 / 2.0f;

    while ( *p ) {
        pIFont -> RenderGlyph(hdc,(unsigned short)*p,(UINT_PTR)&xFormToPoints,(UINT_PTR)&xFormToPixels,&startPoint,&endPoint);
        startPoint = endPoint;
        p++;
    }

    return;
    }


    HBITMAP getSampleImage(HDC hdc) {

    HRSRC hrsrc = FindResourceA(NULL,MAKEINTRESOURCE(1),"#256");
    HGLOBAL hResource = LoadResource(NULL,hrsrc);
    SIZE_T sizeData = SizeofResource(NULL,hrsrc);

    BYTE *pBitmap = new BYTE[sizeData + 1];

    BYTE *pBitmapStart = pBitmap;

    memcpy(pBitmap,(BYTE *)LockResource(hResource),sizeData);

    BITMAPFILEHEADER bitmapFileHeader{0};

    memcpy(&bitmapFileHeader,pBitmap,sizeof(BITMAPFILEHEADER));

    pBitmap += sizeof(BITMAPFILEHEADER);

    BITMAPINFOHEADER bitmapInfoHeader{0};

    memcpy(&bitmapInfoHeader,pBitmap,sizeof(BITMAPINFOHEADER));

    pBitmap += sizeof(BITMAPINFOHEADER);

    BITMAPINFO *pBitmapInfo = (BITMAPINFO *)&bitmapInfoHeader;

    HBITMAP hBitmap = CreateCompatibleBitmap(hdc,bitmapInfoHeader.biWidth,bitmapInfoHeader.biHeight);

    SetDIBits(hdc,hBitmap,0,bitmapInfoHeader.biHeight,pBitmap,pBitmapInfo,DIB_RGB_COLORS);

    delete [] pBitmapStart;

    return hBitmap;
    }