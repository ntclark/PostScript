
#include "fontManagerDemo.h"

    boolean isInitialized = false;

    long resizeCount = 0L;

    WNDPROC defaultStaticHandler = NULL;

    LRESULT CALLBACK dialogHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    switch ( msg ) {
    case WM_INITDIALOG: {

        SetDlgItemText(hwnd,IDDI_SENTENCE,"The quick brown fox jumped over the lazy dog");
        SetWindowText(hwnd,"specify the sample string");
        SetDlgItemText(hwnd,IDDI_FONT_WEIGHT,"?");
        SetDlgItemText(hwnd,IDDI_FONT_SIZE,"?");
        EnableWindow(GetDlgItem(hwnd,IDDI_FONT_WEIGHT),FALSE);

        SetFocus(GetDlgItem(hwnd,IDDI_GET_FONT));

        pIFontManager -> get_CurrentFont(&pIFont);

        PostMessage(hwnd,WM_USER + 1,0L,0L);

        hwndDialog = hwnd;

        RECT rcFrame;
        GetWindowRect(hwndFrame,&rcFrame);
        SetWindowPos(hwnd,HWND_TOP,rcFrame.left + 32,rcFrame.top - 128,0,0,SWP_NOSIZE);

        return FALSE;
        }

    case WM_USER + 1: {

        char szFontName[64]{64 * '\0'};
        char szFontStyle[64]{64 * '\0'};
        char szFontScript[64]{64 * '\0'};
        long fontWeight;
        FLOAT fontSize = 0.0f;

        UINT_PTR pAvailableFonts = NULL;
        UINT_PTR pAvailableNames = NULL;
        UINT_PTR pAvailableStyles = NULL;
        UINT_PTR pAvailableScripts = NULL;

        pIFont -> GetCharacteristics(64,szFontName,64,szFontStyle,64,szFontScript,&fontWeight,&fontSize,
                                        &pAvailableFonts,&pAvailableNames,&pAvailableStyles,&pAvailableScripts);

        char szTemp[64];

        sprintf_s<64>(szTemp,"%d",fontWeight);
        SetDlgItemText(hwnd,IDDI_FONT_WEIGHT,szTemp);

        sprintf_s<64>(szTemp,"%d",(int)fontSize);
        SetDlgItemText(hwnd,IDDI_FONT_SIZE,szTemp);

        HWND hwndCB = GetDlgItem(hwnd,IDDI_FONT_NAMES);

        SendMessage(hwndCB,CB_RESETCONTENT,0L,0L);
        char *p = (char *)pAvailableNames;
        while ( *p ) {
            if ( CB_ERR == SendMessage(hwndCB,CB_FINDSTRINGEXACT,-1L,(LPARAM)p) ) {
                SendMessage(hwndCB,CB_INSERTSTRING,0L,(LPARAM)p);
                if ( 0 == strcmp(p,szFontName) && strlen(p) == strlen(szFontName) )
                    SendMessage(hwndCB,CB_SETCURSEL,SendMessage(hwndCB,CB_GETCOUNT,0L,0L) - 1,0L);
            }
            p += strlen(p) + 1;
        }

        hwndCB = GetDlgItem(hwnd,IDDI_FONT_SCRIPTS);

        SendMessage(hwndCB,CB_RESETCONTENT,0L,0L);
        p = (char *)pAvailableScripts;
        while ( *p ) {
            if ( CB_ERR == SendMessage(hwndCB,CB_FINDSTRINGEXACT,-1L,(LPARAM)p) ) {
                SendMessage(hwndCB,CB_INSERTSTRING,0L,(LPARAM)p);
                if ( 0 == strcmp(p,szFontScript) )
                    SendMessage(hwndCB,CB_SETCURSEL,SendMessage(hwndCB,CB_GETCOUNT,0L,0L) - 1,0L);
            }
            p += strlen(p) + 1;
        }

        CoTaskMemFree((void *)pAvailableFonts);
        CoTaskMemFree((void *)pAvailableNames);
        CoTaskMemFree((void *)pAvailableStyles);
        CoTaskMemFree((void *)pAvailableScripts);

        isInitialized = true;

        SendMessage(hwnd,WM_USER + 2,0L,0L);

        }
        return FALSE;

    case WM_USER + 2: {

        if ( ! isInitialized )
            break;

        calcTransforms(hwndFrame);

        static char szText[256];

        GetWindowText(GetDlgItem(hwnd,IDDI_SENTENCE),szText,255);

        HDC hdc = GetDC(hwndFrame);

        POINTF startPoint,endPoint;

        startPoint.x = 16.0f;
        startPoint.y = 16.0f;

        char *p = szText;

        RECT rcFrame;
        GetClientRect(hwndFrame,&rcFrame);

        pIRenderer -> ClearRect(hdc,&rcFrame,WHITE);

        while ( *p ) {
            pIFont -> RenderGlyph(hdc,(unsigned short)*p,(UINT_PTR)&psXForm,(UINT_PTR)&gdiXForm,&startPoint,&endPoint);
            startPoint = endPoint;
            p++;
        }

        FLOAT pointSize;
        pIFont -> get_PointSize(&pointSize);

        long cx = (long)(endPoint.x * gdiXForm.eM11);
        long cy = (long)((startPoint.y + pointSize) * fabs(gdiXForm.eM22));

        RECT rcAdjust{0,0,0,0};
        AdjustWindowRectEx(&rcAdjust,(DWORD)GetWindowLongPtr(hwndFrame,GWL_STYLE),FALSE,(DWORD)GetWindowLongPtr(hwndFrame,GWL_EXSTYLE));

        SetWindowPos(hwndFrame,HWND_TOP,0,0,cx + rcAdjust.right - rcAdjust.left + (long)(16.0f * gdiXForm.eM11) + 1,
                                                cy + rcAdjust.bottom - rcAdjust.top,SWP_NOMOVE);

        if ( 0 == resizeCount ) {
            pIRenderer -> Discard();
            resizeCount++;
            return dialogHandler(hwndDialog,WM_USER + 2,0L,0L);
        }

        resizeCount = 0;

        GetClientRect(hwndFrame,&rcAdjust);
        pIRenderer -> ClearRect(hdc,&rcAdjust,WHITE);

        pIRenderer -> Render(hdc,&rcAdjust);
        pIRenderer -> Reset();

        ReleaseDC(hwndFrame,hdc);
        }
        return FALSE;

    case WM_TIMER:
        KillTimer(hwndDialog,(UINT_PTR)wParam);
        setFont();
        break;

    case WM_COMMAND:

        if ( ! isInitialized )
            break;

        switch ( LOWORD(wParam) ) {
        case IDDI_GET_FONT: {
            HDC hdc = GetDC(hwndFrame);
            HRESULT rc = pIFontManager -> ChooseFont(hdc,&pIFont);
            ReleaseDC(hwndFrame,hdc);
            if ( ! ( S_OK == rc ) )
                break;
            SendMessage(hwndDialog,WM_USER + 1,0L,0L);
            }
            break;

        case IDDI_SENTENCE:
        case IDDI_FONT_SIZE:
            if ( HIWORD(wParam) == CBN_SELCHANGE || HIWORD(wParam) == EN_CHANGE )
                SetTimer(hwndDialog,LOWORD(wParam),750,NULL);
            break;

        case IDDI_FONT_NAMES:
        case IDDI_FONT_SCRIPTS:
            if ( HIWORD(wParam) == CBN_SELCHANGE )
                setFont();
            break;

        case IDDI_SHOW_TABLE: {

            DLGTEMPLATE *dt = (DLGTEMPLATE *)LoadResource(NULL,FindResource(NULL,MAKEINTRESOURCE(IDD_GLYPH_TABLE),RT_DIALOG));
            CreateDialogIndirect(NULL,dt,hwndFrame,glyphTableDialogHandler);

            if ( NULL == defaultStaticHandler )
                defaultStaticHandler = (WNDPROC)SetWindowLongPtr(hwndGlyphTableHost,GWLP_WNDPROC,(ULONG_PTR)glyphTableHostHandler);

            FLOAT pointSize;
            pIFont -> get_PointSize(&pointSize);

            long cx = GLYPH_TABLE_MARGIN_LEFT + (long)((FLOAT)GLYPH_TABLE_COLUMN_COUNT * (pointSize + 2) * gdiXForm.eM11) + GLYPH_TABLE_MARGIN_RIGHT;
            long cy = GLYPH_TABLE_MARGIN_TOP  + (long)((FLOAT)GLYPH_TABLE_ROW_COUNT * (pointSize + 2) * gdiXForm.eM11) + GLYPH_TABLE_MARGIN_BOTTOM;

            SetWindowPos(hwndGlyphTableDialog,HWND_TOP,128,128,cx,cy,0*SWP_NOMOVE);
            ShowWindow(hwndGlyphTableDialog,SW_SHOW);
            }
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }

    return 0;
    }


    LRESULT CALLBACK glyphTableHostHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {
    switch ( msg ) {

    case WM_PAINT: {
        LRESULT rc = defaultStaticHandler(hwnd,msg,wParam,lParam);
        if ( NULL == hdcGlyphTable ) 
            return rc;
        scrollInfo.fMask = SIF_POS;
        GetScrollInfo(hwndGlyphTableDialog,SBS_VERT,&scrollInfo);
        HDC hdc = GetDC(hwndGlyphTableHost);
        BitBlt(hdc,0,0,cxRenderPane,cyRenderPane,hdcGlyphTable,0,scrollInfo.nPos,SRCCOPY);
        ReleaseDC(hwndGlyphTableHost,hdc);
        return rc;
        }
        break;

    default:
        break;
    }
    return defaultStaticHandler(hwnd,msg,wParam,lParam);
    }


    LRESULT CALLBACK glyphTableDialogHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    switch ( msg ) {
    case WM_INITDIALOG:
        hwndGlyphTableDialog = hwnd;
        hwndGlyphTableHost = GetDlgItem(hwnd,IDDI_GLYPH_PANE_HOST);
        char szTitle[128];
        char szFontName[64];
        pIFont -> FontName(64,szFontName);
        sprintf_s<128>(szTitle,"Glyph table for font: %s",szFontName);
        SetWindowText(hwnd,szTitle);
        SetTimer(hwnd,1,500,NULL);
        return (LRESULT)0;

    case WM_TIMER: 
        KillTimer(hwnd,1);
        generateGlyphTable();
        break;

    case WM_MOUSEWHEEL:
        if ( 0 < (int16_t)HIWORD(wParam) )
            return SendMessage(hwnd,WM_VSCROLL,MAKEWPARAM(SB_LINEUP,0L),0L);
        else
            return SendMessage(hwnd,WM_VSCROLL,MAKEWPARAM(SB_LINEDOWN,0L),0L);
        break;

    case WM_SIZE: {
        long cx = LOWORD(lParam) - GLYPH_TABLE_MARGIN_LEFT - GLYPH_TABLE_MARGIN_RIGHT;
        long cy = HIWORD(lParam) - GLYPH_TABLE_MARGIN_TOP - GLYPH_TABLE_MARGIN_BOTTOM;
        SetWindowPos(hwndGlyphTableHost,HWND_TOP,
                          GLYPH_TABLE_MARGIN_LEFT,GLYPH_TABLE_MARGIN_TOP,cx,cy,0L);
        SetWindowPos(GetDlgItem(hwnd,IDOK),HWND_TOP,16,HIWORD(lParam) - 40,0,0,SWP_NOSIZE);
        }
        break;

    case WM_USER + 1: {

        generateGlyphTable();

        }
        break;

    case WM_VSCROLL: {

        scrollInfo.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;

        FLOAT pointSize;
        pIFont -> get_PointSize(&pointSize);

        int32_t deltaLine = (int32_t)(pointSize / fabs(gdiXForm.eM22));

        GetScrollInfo(hwnd,SBS_VERT,&scrollInfo);

        int32_t delta = 0;

        switch ( LOWORD(wParam) ) {

        case SB_PAGEDOWN:
            delta = scrollInfo.nPage;
            scrollInfo.nPos += scrollInfo.nPage;
            break;

        case SB_PAGEUP:
            delta = -(int32_t)scrollInfo.nPage;
            scrollInfo.nPos -= scrollInfo.nPage;
            break;

        case SB_LINEDOWN:
            delta = deltaLine;
            scrollInfo.nPos += deltaLine;
            break;
        
        case SB_LINEUP:
            delta = -deltaLine;
            scrollInfo.nPos -= deltaLine;
            break;

        case SB_THUMBTRACK:
        case SB_THUMBPOSITION:
            delta = HIWORD(wParam) - scrollInfo.nPos;
            
            scrollInfo.nPos = HIWORD(wParam);
            break;

        case SB_TOP:
            delta = -scrollInfo.nPos;
            scrollInfo.nPos = scrollInfo.nMin;
            break;

        case SB_BOTTOM:
            delta = scrollInfo.nMin - scrollInfo.nPos;
            scrollInfo.nPos = scrollInfo.nMax - scrollInfo.nPage;
            break;

        }

        if ( LOWORD(wParam) == SB_ENDSCROLL )
            break;

        scrollInfo.nPos = min(scrollInfo.nPos,scrollInfo.nMax);
        scrollInfo.nPos = max(scrollInfo.nPos,scrollInfo.nMin);

        SetScrollInfo(hwndGlyphTableDialog,SBS_VERT,&scrollInfo,TRUE);

        InvalidateRect(hwndGlyphTableHost,NULL,TRUE);

        }
        return (LRESULT)0;

    case WM_COMMAND:
        switch ( LOWORD(wParam) ) {
        case IDOK:
            if ( ! ( NULL == defaultStaticHandler ) ) {
                SetWindowLongPtr(hwndGlyphTableHost,GWLP_WNDPROC,(ULONG_PTR)defaultStaticHandler);
                defaultStaticHandler = NULL;
            }
            EndDialog(hwnd,0);

            break;

        default:
            break;
        }

    default:
        break;
    }

    return 0;
    }