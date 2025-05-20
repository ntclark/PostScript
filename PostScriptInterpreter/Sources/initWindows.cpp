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
#include "logHandling.h"

    int PostScriptInterpreter::initWindows() {

    //
    // Note that hwndHost is *external*, our client window will be embedded in the
    // windows application that is hosting this object.
    // 

    pIOleInPlaceSite -> GetWindow(&hwndHost);

    WNDCLASS gClass;

    memset(&gClass,0,sizeof(WNDCLASS));

    gClass.style = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
    gClass.lpfnWndProc = PostScriptInterpreter::clientWindowHandler;
    gClass.cbClsExtra = 32;
    gClass.cbWndExtra = 32;
    gClass.hInstance = hModule;
    gClass.hIcon = NULL;
    gClass.hCursor = NULL;
    gClass.hbrBackground = 0;
    gClass.lpszMenuName = NULL;
    gClass.lpszClassName = "psInterpreterCanvas";

    RegisterClass(&gClass);

    gClass.lpfnWndProc = PostScriptInterpreter::splitterHandler;
    gClass.lpszClassName = "splitter";

    RegisterClass(&gClass);

    hwndClient = CreateWindowEx(WS_EX_CLIENTEDGE,"psInterpreterCanvas","",WS_CHILD,0,0,0,0,hwndHost,
                                    NULL,NULL,reinterpret_cast<void *>(this));

    hwndVScroll = CreateWindowEx(0L,"SCROLLBAR","",WS_CHILD | WS_VISIBLE | SBS_VERT,0,0,
                                    GetSystemMetrics(SM_CXVSCROLL),CW_USEDEFAULT,hwndClient,NULL,NULL,NULL);

    DLGTEMPLATE *dt = (DLGTEMPLATE *)LoadResource(hModule,FindResource(hModule,MAKEINTRESOURCE(IDD_CMD_PANE),RT_DIALOG));
    hwndCmdPane = CreateDialogIndirectParam(hModule,dt,(HWND)hwndHost,(DLGPROC)PostScriptInterpreter::cmdPaneHandler,(ULONG_PTR)(void *)this);

    SetWindowText(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_ACTIVE_FILE),szCurrentPostScriptFile);

    dt = (DLGTEMPLATE *)LoadResource(hModule,FindResource(hModule,MAKEINTRESOURCE(IDD_POSTSCRIPT_LOG_CMD_PANE),RT_DIALOG));
    hwndPostScriptLogCmdPane = CreateDialogIndirectParam(hModule,dt,(HWND)hwndCmdPane,(DLGPROC)PostScriptInterpreter::logPaneHandler,(ULONG_PTR)(void *)this);

    dt = (DLGTEMPLATE *)LoadResource(hModule,FindResource(hModule,MAKEINTRESOURCE(IDD_RENDERER_LOG_CMD_PANE),RT_DIALOG));
    hwndRendererLogCmdPane = CreateDialogIndirectParam(hModule,dt,(HWND)hwndCmdPane,(DLGPROC)PostScriptInterpreter::logPaneHandler,(ULONG_PTR)(void *)this);


    ShowWindow(hwndPostScriptLogCmdPane,SW_HIDE);

    hwndPostScriptLogContent = CreateWindowEx(WS_EX_CLIENTEDGE,RICHEDIT_CLASS,"",WS_CHILD | ES_MULTILINE | WS_VSCROLL,
                                       0,0,0,0,hwndHost,NULL,NULL,NULL);

    hwndPostScriptLogSplitter = CreateWindowEx(0,"splitter","",WS_CHILD,0,0,0,0,hwndHost,NULL,NULL,reinterpret_cast<void *>(this));

    ShowWindow(hwndRendererLogCmdPane,SW_HIDE);

    hwndRendererLogContent = CreateWindowEx(WS_EX_CLIENTEDGE,RICHEDIT_CLASS,"",WS_CHILD | ES_MULTILINE | WS_VSCROLL,
                                                    0,0,0,0,hwndHost,NULL,NULL,NULL);

    hwndRendererLogSplitter = CreateWindowEx(0,"splitter","",WS_CHILD,0,0,0,0,hwndHost,NULL,NULL,reinterpret_cast<void *>(this));

    activePageOrigin.x = 0L;
    activePageOrigin.y = 0L;

    SetWindowLongPtr(hwndPostScriptLogContent,GWLP_USERDATA,(UINT_PTR)this);
    SetWindowLongPtr(hwndRendererLogContent,GWLP_USERDATA,(UINT_PTR)this);

    SendMessage(hwndPostScriptLogContent,EM_SETREADONLY,(WPARAM)TRUE,0L);
    SendMessage(hwndRendererLogContent,EM_SETREADONLY,(WPARAM)TRUE,0L);

    CHARFORMAT charFormat;
    memset(&charFormat,0,sizeof(CHARFORMAT));

    charFormat.cbSize = sizeof(CHARFORMAT);
    charFormat.dwMask = CFM_FACE | CFM_SIZE;

    strcpy(charFormat.szFaceName,"Courier New");
    charFormat.yHeight = 10 * 20;

    SendMessage(hwndPostScriptLogContent,EM_SETCHARFORMAT,(WPARAM)SCF_ALL,(LPARAM)&charFormat);
    SendMessage(hwndPostScriptLogContent,EM_SHOWSCROLLBAR,(WPARAM)SB_VERT,(LPARAM)TRUE);
    SendMessage(hwndPostScriptLogContent,EM_SHOWSCROLLBAR,(WPARAM)SB_HORZ,(LPARAM)TRUE);

    SendMessage(hwndRendererLogContent,EM_SETCHARFORMAT,(WPARAM)SCF_ALL,(LPARAM)&charFormat);
    SendMessage(hwndRendererLogContent,EM_SHOWSCROLLBAR,(WPARAM)SB_VERT,(LPARAM)TRUE);
    SendMessage(hwndRendererLogContent,EM_SHOWSCROLLBAR,(WPARAM)SB_HORZ,(LPARAM)TRUE);

    SETTEXTEX st;
    memset(&st,0,sizeof(SETTEXTEX));
    st.flags = ST_DEFAULT;
    st.codepage = CP_ACP;

    SendMessage(hwndPostScriptLogContent,EM_SETTEXTMODE,(WPARAM)TM_RICHTEXT,(LPARAM)0L);
    SendMessage(hwndRendererLogContent,EM_SETTEXTMODE,(WPARAM)TM_RICHTEXT,(LPARAM)0L);

    SendMessage(hwndPostScriptLogContent,EM_SETTEXTEX,(WPARAM)&st,(LPARAM)L"");
    SendMessage(hwndPostScriptLogContent,EM_EXLIMITTEXT,(WPARAM)0L,(LPARAM)(32768 * 65535));

    SendMessage(hwndRendererLogContent,EM_SETTEXTEX,(WPARAM)&st,(LPARAM)L"");
    SendMessage(hwndRendererLogContent,EM_EXLIMITTEXT,(WPARAM)0L,(LPARAM)(32768 * 65535));

    memset(&logStream,0,sizeof(EDITSTREAM));
    logStream.dwCookie = (DWORD_PTR)hwndPostScriptLogContent;
    logStream.pfnCallback = processLog;

    memset(&rendererLogStream,0,sizeof(EDITSTREAM));
    rendererLogStream.dwCookie = (DWORD_PTR)hwndRendererLogContent;
    rendererLogStream.pfnCallback = processLog;

    IDropTarget *pIDropTarget = NULL;
    QueryInterface(IID_IDropTarget,reinterpret_cast<void **>(&pIDropTarget));
    HRESULT rcdd = RegisterDragDrop(hwndClient,pIDropTarget);

    return 0;
    }