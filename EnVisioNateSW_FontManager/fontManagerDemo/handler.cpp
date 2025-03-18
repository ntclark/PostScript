#include <Windows.h>
#include <stdio.h>
#include <functional>

#include "resource.h"

#include "Renderer_i.h"
#include "FontManager_i.h"

#define ON_CURVE_POINT  0x01

    extern HWND hwndFrame;
    extern HWND hwndDialog;
    extern long cyWindow;

    extern IRenderer *pIGlyphRenderer;
    extern IFontManager *pIFontManager;
    extern IFont_EVNSW *pIFont;

    boolean isInitialized = false;

    FLOAT scalePDF = 1.0f;
    FLOAT cyWindowInPoints = 0.0f;

    FLOAT dialogHeight = 0.0f;

    XFORM psXForm{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};
    XFORM gdiXForm{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};

    void setFont();

    LRESULT CALLBACK handler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    switch ( msg ) {

    case WM_CREATE: {
        CREATESTRUCT *pc = reinterpret_cast<CREATESTRUCT *>(lParam);
        hwndFrame = hwnd;
        }
        return (LRESULT)0;

    case WM_SYSCOMMAND: {
        if ( wParam == SC_CLOSE ) {
            PostQuitMessage(0);
            return (LRESULT)0;
        }
        }
        break;

    case WM_SIZE: {
        cyWindow = HIWORD(lParam);
        HDC hdc = GetDC(hwndFrame);

LOGFONT theFont{0};
strcpy(theFont.lfFaceName,"Arial");
theFont.lfHeight = -72;
HFONT hFontDate = CreateFontIndirect(&theFont);

SelectObject(hdc,hFontDate);
RECT rc{100,100,512,512};

//long wtf = DrawTextExA(hdc,(char *)"M",1,&rc,DT_CALCRECT,NULL);

FLOAT pixelsPerInch = DrawTextExA(hdc,(char *)"M",1,&rc,DT_CENTER,NULL);

pixelsPerInch = GetDeviceCaps(hdc,LOGPIXELSY);
char szX[128];
sprintf_s<128>(szX,"Height in inches %f\n",(FLOAT)cyWindow / (FLOAT)pixelsPerInch);
OutputDebugStringA(szX);

GetClientRect(hwndFrame,&rc);

SetGraphicsMode(hdc, GM_ADVANCED);
int oldMode = SetMapMode(hdc,MM_HIENGLISH);
GetClientRect(hwndFrame,&rc);

//MoveToEx(hdc,0,0,NULL);
//POINT ptStart,ptEnd;
//GetCurrentPositionEx(hdc,&ptStart);
//LineTo(hdc,0,1000);
//SetMapMode(hdc,oldMode);
//GetCurrentPositionEx(hdc,&ptEnd);

        cyWindowInPoints = (FLOAT)cyWindow * 72.0f / (FLOAT)pixelsPerInch;//GetDeviceCaps(hdc,LOGPIXELSX);
        scalePDF = (FLOAT)cyWindow / cyWindowInPoints;
        psXForm = {1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};
        gdiXForm = {scalePDF,0.0f,0.0f,-scalePDF,0.0f,(FLOAT)cyWindow};
        ReleaseDC(hwndFrame,hdc);
        }
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps{0};
        BeginPaint(hwnd,&ps); 
        SendMessage(hwndDialog,WM_USER + 2,0L,0L);
        EndPaint(hwnd,&ps);
        }
        break;

    default:
        break;
    }

    return DefWindowProc(hwnd,msg,wParam,lParam);
    }


    LRESULT CALLBACK dialogHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    switch ( msg ) {
    case WM_INITDIALOG: {

        SetDlgItemText(hwnd,IDDI_SENTENCE,"The quick brown fox jumped over the lazy dog");
        SetWindowText(hwnd,"specify the sample string");
        SetDlgItemText(hwnd,IDDI_FONT_STYLES,"?");
        SetDlgItemText(hwnd,IDDI_FONT_WEIGHT,"?");
        SetDlgItemText(hwnd,IDDI_FONT_SIZE,"?");
        EnableWindow(GetDlgItem(hwnd,IDDI_FONT_WEIGHT),FALSE);

        SetFocus(GetDlgItem(hwnd,IDDI_GET_FONT));

        pIFontManager -> get_CurrentFont(&pIFont);

        if ( ! ( NULL == pIFont ) )
            PostMessage(hwnd,WM_USER + 1,0L,0L);

        hwndDialog = hwnd;

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

        hwndCB = GetDlgItem(hwnd,IDDI_FONT_STYLES);

        SendMessage(hwndCB,CB_RESETCONTENT,0L,0L);
        p = (char *)pAvailableStyles;
        while ( *p ) {
            if ( CB_ERR == SendMessage(hwndCB,CB_FINDSTRINGEXACT,-1L,(LPARAM)p) ) {
                SendMessage(hwndCB,CB_INSERTSTRING,0L,(LPARAM)p);
                if ( 0 == strcmp(p,szFontStyle) )
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

        SendMessage(hwnd,WM_USER + 2,0L,0L);

        isInitialized = true;

        }
        return FALSE;

    case WM_USER + 2: {

        if ( ! isInitialized )
            break;

        long countChars = GetWindowTextLength(GetDlgItem(hwnd,IDDI_SENTENCE));

        char *pszText = new char[countChars + 1];
        pszText[countChars + 1] = '\0';
        GetWindowText(GetDlgItem(hwnd,IDDI_SENTENCE),pszText,countChars + 1);

        POINTF startPoint,endPoint;

        startPoint.x = 36.0f;
        startPoint.y = cyWindowInPoints / 2.0f;

        char *p = pszText;

        HDC hdc = GetDC(hwndFrame);

        RECT rc;
        GetClientRect(hwndFrame,&rc);

        FillRect(hdc,&rc,(HBRUSH) (COLOR_WINDOW + 1));

        while ( *p ) {
            pIFont -> RenderGlyph(hdc,(BYTE)*p,(UINT_PTR)&psXForm,(UINT_PTR)&gdiXForm,&startPoint,&endPoint);
            startPoint = endPoint;
            p++;
        }

        pIGlyphRenderer -> Render();

        pIGlyphRenderer -> Reset();

        ReleaseDC(hwndFrame,hdc);

        delete [] pszText;
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
        case IDDI_FONT_STYLES:
        case IDDI_FONT_SCRIPTS:
            if ( HIWORD(wParam) == CBN_SELCHANGE )
                setFont();
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


    void setFont() {

    long curSel[]{
        SendMessage(GetDlgItem(hwndDialog,IDDI_FONT_NAMES),CB_GETCURSEL,0L,0L),
        SendMessage(GetDlgItem(hwndDialog,IDDI_FONT_STYLES),CB_GETCURSEL,0L,0L),
        SendMessage(GetDlgItem(hwndDialog,IDDI_FONT_SCRIPTS),CB_GETCURSEL,0L,0L)};

    char szFontSpec[256];
    char szTemp[128];

    SendMessage(GetDlgItem(hwndDialog,IDDI_FONT_NAMES),CB_GETLBTEXT,curSel[0],(LPARAM)szTemp);
    sprintf_s<256>(szFontSpec,"%s",szTemp);

    //SendMessage(GetDlgItem(hwndDialog,IDDI_FONT_STYLES),CB_GETLBTEXT,curSel[1],(LPARAM)szTemp);
    //strcat(szFontSpec,szTemp);

    SendMessage(GetDlgItem(hwndDialog,IDDI_FONT_SCRIPTS),CB_GETLBTEXT,curSel[2],(LPARAM)szTemp);

    //strcat(szFontSpec,":");
    //strcat(szFontSpec,szTemp);

    pIFontManager -> LoadFont(szFontSpec,0,&pIFont);

    GetDlgItemText(hwndDialog,IDDI_FONT_SIZE,szTemp,128);

    pIFont ->Scale(atof(szTemp),atof(szTemp));// -> ScaleFont(atof(szTemp));

    InvalidateRect(hwndFrame,NULL,TRUE);
    UpdateWindow(hwndFrame);

    return;
    }