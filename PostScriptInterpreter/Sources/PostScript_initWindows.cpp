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

#include "PostScript.h"

    int PStoPDF::initWindows() {

    pIOleInPlaceSite -> GetWindow(&hwndHost);

    WNDCLASS gClass;

    memset(&gClass,0,sizeof(WNDCLASS));

    gClass.style = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
    gClass.lpfnWndProc = PStoPDF::handler;
    gClass.cbClsExtra = 32;
    gClass.cbWndExtra = 32;
    gClass.hInstance = hModule;
    gClass.hIcon = NULL;
    gClass.hCursor = NULL;
    gClass.hbrBackground = 0;
    gClass.lpszMenuName = NULL;
    gClass.lpszClassName = "PStoPDF";

    RegisterClass(&gClass);

    gClass.lpfnWndProc = PStoPDF::splitterHandler;
    gClass.lpszClassName = "PStoPDFSplitter";

    RegisterClass(&gClass);

    if ( ! ( NULL == PStoPDF::nativeHostFrameHandler ) )
        SetWindowLongPtr(GetParent(hwndHost),GWLP_WNDPROC,(ULONG_PTR)PStoPDF::nativeHostFrameHandler);

    PStoPDF::nativeHostFrameHandler = (WNDPROC)SetWindowLongPtr(GetParent(hwndHost),GWLP_WNDPROC,(ULONG_PTR)PStoPDF::hostFrameHandlerOveride);

    hwndClient = CreateWindowEx(WS_EX_CLIENTEDGE,"PStoPDF","",WS_CHILD | WS_VISIBLE,0,0,0,0,hwndHost,NULL,NULL,reinterpret_cast<void *>(this));

    hwndVScroll = CreateWindowEx(0L,"SCROLLBAR","",WS_CHILD | WS_VISIBLE | SBS_VERT,0,0,GetSystemMetrics(SM_CXVSCROLL),CW_USEDEFAULT,hwndClient,NULL,NULL,NULL);

    DLGTEMPLATE *dt = (DLGTEMPLATE *)LoadResource(hModule,FindResource(hModule,MAKEINTRESOURCE(IDD_CMD_PANE),RT_DIALOG));
    hwndCmdPane = CreateDialogIndirectParam(hModule,dt,(HWND)hwndClient,(DLGPROC)PStoPDF::cmdPaneHandler,(ULONG_PTR)(void *)this);

    LoadLibrary("Riched20.dll");

    hwndLog = CreateWindowEx(WS_EX_CLIENTEDGE,RICHEDIT_CLASS,"",WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL,
                                       0,0,0,0,hwndHost,NULL,NULL,reinterpret_cast<void *>(this));

    nativeRichEditHandler = (WNDPROC)GetWindowLongPtr(hwndLog,GWLP_WNDPROC);

    dt = (DLGTEMPLATE *)LoadResource(hModule,FindResource(hModule,MAKEINTRESOURCE(IDD_LOG_PANE),RT_DIALOG));
    hwndLogPane = CreateDialogIndirectParam(hModule,dt,(HWND)hwndClient,(DLGPROC)PStoPDF::logPaneHandler,(ULONG_PTR)(void *)this);

    hwndOperandStackSize = GetDlgItem(hwndLogPane,IDDI_LOG_PANE_OPERAND_STACK_SIZE);
    hwndCurrentDictionary = GetDlgItem(hwndLogPane,IDDI_LOG_PANE_CURRENT_DICTIONARY);

    hwndLogSplitter = CreateWindowEx(WS_EX_TRANSPARENT,"PStoPDFSplitter","",WS_CHILD | WS_VISIBLE,0,0,0,0,hwndHost,NULL,NULL,reinterpret_cast<void *>(this));

   //hwndStack = CreateWindowEx(WS_EX_CLIENTEDGE,"RICHEDIT50W","",WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL,
   //                                       0,0,0,0,hwndClient,NULL,NULL,reinterpret_cast<void *>(this));

    SET_PANES

    activePageOrigin.x = 0L;
    activePageOrigin.y = 0L;

    SetWindowLongPtr(hwndLog,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(this));

    CHARFORMAT charFormat;
    memset(&charFormat,0,sizeof(CHARFORMAT));

    charFormat.cbSize = sizeof(CHARFORMAT);
    charFormat.dwMask = CFM_FACE | CFM_SIZE;

    strcpy(charFormat.szFaceName,"Courier New");
    charFormat.yHeight = 10 * 20;

    SendMessage(hwndLog,EM_SETCHARFORMAT,(WPARAM)SCF_ALL,(LPARAM)&charFormat);
    SendMessage(hwndLog,EM_SHOWSCROLLBAR,(WPARAM)SB_VERT,(LPARAM)TRUE);
    SendMessage(hwndLog,EM_SHOWSCROLLBAR,(WPARAM)SB_HORZ,(LPARAM)TRUE);

    SETTEXTEX st;
    memset(&st,0,sizeof(SETTEXTEX));
    st.flags = ST_DEFAULT;
    st.codepage = CP_ACP;

//#ifdef DO_RTF
//    SendMessage(hwndLog,EM_SETTEXTMODE,(WPARAM)TM_RICHTEXT,(LPARAM)0L);
//#else
    SendMessage(hwndLog,EM_SETTEXTMODE,(WPARAM)TM_PLAINTEXT,(LPARAM)0L);
//#endif

    SendMessage(hwndLog,EM_SETTEXTEX,(WPARAM)&st,(LPARAM)L"");
    SendMessage(hwndLog,EM_EXLIMITTEXT,(WPARAM)0L,(LPARAM)(32768 * 65535));

    memset(&logStream,0,sizeof(EDITSTREAM));

    logStream.dwCookie = (DWORD_PTR)this;
    logStream.pfnCallback = processLog;

    return 0;
    }