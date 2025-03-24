
#include "PostScript.h"

    LRESULT CALLBACK PStoPDF::cmdPaneHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    PStoPDF *p = (PStoPDF *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

    switch ( msg ) {
    case WM_INITDIALOG: {
        SetWindowLongPtr(hwnd,GWLP_USERDATA,(LONG_PTR)lParam);
        }
        break;

    case WM_SIZE: {
        long cx = LOWORD(lParam);
        RECT rcLabel;
        GetWindowRect(GetDlgItem(hwnd,IDDI_CMD_PANE_LOG_SHOW),&rcLabel);
        SetWindowPos(GetDlgItem(hwnd,IDDI_CMD_PANE_LOG_SHOW),HWND_TOP,cx - (rcLabel.right - rcLabel.left),8,0,0,SWP_NOSIZE);
        }
        break;

    case WM_LBUTTONDOWN:
    case WM_MOUSEMOVE: {
        POINT ptMouse{GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
        MapWindowPoints(hwnd,HWND_DESKTOP,&ptMouse,1);
        RECT rcLogLabel{0,0,0,0};
        GetWindowRect(GetDlgItem(hwnd,IDDI_CMD_PANE_LOG_SHOW),&rcLogLabel);
        if ( ptMouse.x < rcLogLabel.left || ptMouse.y < rcLogLabel.top || ptMouse.x > rcLogLabel.right || ptMouse.y > rcLogLabel.bottom )
            break;
        if ( msg == WM_MOUSEMOVE )
            SetCursor(LoadCursor(NULL,IDC_HAND));
        else 
            p -> toggleLogVisibility();
        }
        break;


    case WM_COMMAND: {
        switch( LOWORD(wParam) ) {
        case IDDI_CMD_PANE_ACTIVE_FILE: {
            char szTemp[MAX_PATH];
            GetWindowText((HWND)lParam,szTemp,MAX_PATH);
            EnableWindow(GetDlgItem(hwnd,IDDI_CMD_PANE_EXECUTE), ! ( '\0' == szTemp[0] ) ? TRUE : FALSE);
            }
            break;

        case IDDI_CMD_PANE_ACTIVE_FILE_GET: {
            OPENFILENAME openFileName;
            char szFilter[MAX_PATH],szFile[MAX_PATH];

            memset(szFilter,0,sizeof(szFilter));
            memset(szFile,0,sizeof(szFile));
            memset(&openFileName,0,sizeof(OPENFILENAME));

            sprintf(szFilter,"PostScript Files");
            long k = (DWORD)strlen(szFilter) + (DWORD)sprintf(szFilter + (DWORD)strlen(szFilter) + 1,"*.ps");
            k = k + (DWORD)sprintf(szFilter + k + 2,"All Files");
            sprintf(szFilter + k + 3,"*.*");

            openFileName.lStructSize = sizeof(OPENFILENAME);
            openFileName.hwndOwner = hwnd;
            openFileName.Flags = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
            openFileName.lpstrFilter = szFilter;
            openFileName.lpstrFile = szFile;
            openFileName.lpstrDefExt = "pdf";
            openFileName.nFilterIndex = 1;
            openFileName.nMaxFile = MAX_PATH;
            openFileName.lpstrTitle = "Select the existing PostScript file";

            openFileName.lpstrInitialDir = szUserDirectory;

            if ( ! GetOpenFileName(&openFileName) ) 
                break;

            p -> SetSource(openFileName.lpstrFile);

            }
            break;

        case IDDI_CMD_PANE_EXECUTE:
            p -> Parse();
            break;

        case IDDI_CMD_PANE_CONVERT_TO_PDF: {
#if 0
            if ( NULL == p -> pICVPostscriptConverter )
                CoCreateInstance(CLSID_CVPostscriptConverter,NULL,CLSCTX_ALL,IID_ICVPostscriptConverter,reinterpret_cast<void **>(&p -> pICVPostscriptConverter));

            char szTemp[MAX_PATH];
            GetDlgItemText(hwnd,IDDI_CMD_PANE_ACTIVE_FILE,szTemp,MAX_PATH);
            p -> pICVPostscriptConverter -> ConvertToPDF(szTemp);
#endif
            }
            break;

        default:
            break;
        }

        }
        break;

    default:
        break;
    }

    return 0;
    }


    void PStoPDF::toggleLogVisibility() {
    logIsVisible = ! logIsVisible;
    PostMessage(hwndClient,WM_REFRESH_CLIENT_WINDOW,0L,0L);
    return;
    }