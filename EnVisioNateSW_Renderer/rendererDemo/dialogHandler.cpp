
#include "rendererDemo.h"

HBITMAP hbmColorSwatch;

    void setBitmap();

    LRESULT dialogHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    switch(msg) {
    case WM_INITDIALOG:
        hwndDialog = hwnd;
        SetWindowText(hwnd,"Renderer Demo Options");
        setBitmap();
        SendMessage(GetDlgItem(hwnd,IDDI_KEEP_SQUARE),BM_SETCHECK,(WPARAM)BST_CHECKED,0L);
        SendMessage(GetDlgItem(hwnd,IDDI_SHOW_GRID),BM_SETCHECK,(WPARAM)BST_CHECKED,0L);
        return (LRESULT)0L;

    case WM_COMMAND: {
        switch ( LOWORD(wParam) ) {
        case IDDI_BACKGROUND_COLOR: {
            COLORREF customColors[16];
            memset(customColors,0xFFFF,16 * sizeof(COLORREF));
            CHOOSECOLOR colorSpec{0};
            colorSpec.lStructSize = sizeof(CHOOSECOLOR);
            colorSpec.hwndOwner = hwnd;
            colorSpec.rgbResult = graphBackgroundColor;
            colorSpec.lpCustColors = customColors;
            colorSpec.Flags = CC_RGBINIT | CC_FULLOPEN;
            if ( ! ::ChooseColor(&colorSpec) )
                break;
            if ( graphBackgroundColor == colorSpec.rgbResult )
                break;
            graphBackgroundColor = colorSpec.rgbResult;
            setBitmap();
            }
            break;

        default:
            break;
        }
        }
        InvalidateRect(hwndFrame,NULL,TRUE);
        break;

    default:
        break;

    }

    return 0;
    }


    void setBitmap() {
    HDC hdc = GetDC(hwndDialog);
    HDC hdcTarget = CreateCompatibleDC(hdc);
    hbmColorSwatch = CreateCompatibleBitmap(hdc,44, 20);
    SelectObject(hdcTarget,hbmColorSwatch);
    RECT rc{0,0,44,20};
    FillRect(hdcTarget,&rc,CreateSolidBrush(graphBackgroundColor));
    FrameRect(hdcTarget,&rc,CreateSolidBrush(RGB(128,128,128)));
    DeleteDC(hdcTarget);
    ReleaseDC(hwndDialog,hdc);
    SendMessage(GetDlgItem(hwndDialog,IDDI_BACKGROUND_COLOR),BM_SETIMAGE,IMAGE_BITMAP,(LPARAM)hbmColorSwatch);
    return;
    }