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
    // Please note that hwndHost is *external*, our client window will be embedded in the
    // windows application that is hosting this object.
    // 
    // It is also true in this case, that this host is NOT the client of the main frame window.
    // It is an additional simple window that is created to mimic the client *area* of the frame
    // that is, under the caption, and inside the size-borders, etc.
    // 
    // In 30 years of windows development, I have never been able to actually get the handle
    // of this area *as a window handle*, which is to me an obvious thing to need
    // on so many occassions.
    // 
    // This object also subclasses that host window so that these windows stay consistent
    // with it on move or size. Something in the IOleInPlaceObject and that flavor of
    // interfaces might provide the proper notification of when to synchronize position,
    // but I believe I gave up looking for it.
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

    if ( ! ( NULL == PostScriptInterpreter::nativeHostFrameHandler ) )
        SetWindowLongPtr(GetParent(hwndHost),GWLP_WNDPROC,(ULONG_PTR)PostScriptInterpreter::nativeHostFrameHandler);

    PostScriptInterpreter::nativeHostFrameHandler = (WNDPROC)SetWindowLongPtr(GetParent(hwndHost),GWLP_WNDPROC,(ULONG_PTR)PostScriptInterpreter::hostFrameHandlerOveride);

    hwndClient = CreateWindowEx(0L,"psInterpreterCanvas","",WS_CHILD | WS_VISIBLE,0,0,0,0,hwndHost,NULL,NULL,reinterpret_cast<void *>(this));

    hwndVScroll = CreateWindowEx(0L,"SCROLLBAR","",WS_CHILD | WS_VISIBLE | SBS_VERT,0,0,GetSystemMetrics(SM_CXVSCROLL),CW_USEDEFAULT,hwndClient,NULL,NULL,NULL);

    DLGTEMPLATE *dt = (DLGTEMPLATE *)LoadResource(hModule,FindResource(hModule,MAKEINTRESOURCE(IDD_CMD_PANE),RT_DIALOG));
    hwndCmdPane = CreateDialogIndirectParam(hModule,dt,(HWND)hwndClient,(DLGPROC)PostScriptInterpreter::cmdPaneHandler,(ULONG_PTR)(void *)this);

    LoadLibrary("Riched20.dll");

    hwndLogContent = CreateWindowEx(WS_EX_CLIENTEDGE,RICHEDIT_CLASS,"",WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL,
                                       0,0,0,0,hwndHost,NULL,NULL,NULL);

    nativeRichEditHandler = (WNDPROC)GetWindowLongPtr(hwndLogContent,GWLP_WNDPROC);

    dt = (DLGTEMPLATE *)LoadResource(hModule,FindResource(hModule,MAKEINTRESOURCE(IDD_LOG_PANE),RT_DIALOG));
    hwndLogPane = CreateDialogIndirectParam(hModule,dt,(HWND)hwndClient,(DLGPROC)PostScriptInterpreter::logPaneHandler,(ULONG_PTR)(void *)this);

    hwndOperandStackSize = GetDlgItem(hwndLogPane,IDDI_LOG_PANE_OPERAND_STACK_SIZE);
    hwndCurrentDictionary = GetDlgItem(hwndLogPane,IDDI_LOG_PANE_CURRENT_DICTIONARY);

    hwndLogSplitter = CreateWindowEx(WS_EX_TRANSPARENT,"splitter","",WS_CHILD | WS_VISIBLE,0,0,0,0,hwndHost,NULL,NULL,reinterpret_cast<void *>(this));

    hwndRendererLogContent = CreateWindowEx(WS_EX_CLIENTEDGE,RICHEDIT_CLASS,"",WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL,
                                                    0,0,0,0,hwndHost,NULL,NULL,NULL);

    hwndRendererLogSplitter = CreateWindowEx(WS_EX_TRANSPARENT,"splitter","",WS_CHILD | WS_VISIBLE,0,0,0,0,hwndHost,NULL,NULL,reinterpret_cast<void *>(this));

    setWindowPanes();

    activePageOrigin.x = 0L;
    activePageOrigin.y = 0L;

    SetWindowLongPtr(hwndLogContent,GWLP_USERDATA,(UINT_PTR)this);
    SetWindowLongPtr(hwndRendererLogContent,GWLP_USERDATA,(UINT_PTR)this);

    SendMessage(hwndLogContent,EM_SETREADONLY,(WPARAM)TRUE,0L);
    SendMessage(hwndRendererLogContent,EM_SETREADONLY,(WPARAM)TRUE,0L);

    CHARFORMAT charFormat;
    memset(&charFormat,0,sizeof(CHARFORMAT));

    charFormat.cbSize = sizeof(CHARFORMAT);
    charFormat.dwMask = CFM_FACE | CFM_SIZE;

    strcpy(charFormat.szFaceName,"Courier New");
    charFormat.yHeight = 10 * 20;

    SendMessage(hwndLogContent,EM_SETCHARFORMAT,(WPARAM)SCF_ALL,(LPARAM)&charFormat);
    SendMessage(hwndLogContent,EM_SHOWSCROLLBAR,(WPARAM)SB_VERT,(LPARAM)TRUE);
    SendMessage(hwndLogContent,EM_SHOWSCROLLBAR,(WPARAM)SB_HORZ,(LPARAM)TRUE);

    SendMessage(hwndRendererLogContent,EM_SETCHARFORMAT,(WPARAM)SCF_ALL,(LPARAM)&charFormat);
    SendMessage(hwndRendererLogContent,EM_SHOWSCROLLBAR,(WPARAM)SB_VERT,(LPARAM)TRUE);
    SendMessage(hwndRendererLogContent,EM_SHOWSCROLLBAR,(WPARAM)SB_HORZ,(LPARAM)TRUE);

    SETTEXTEX st;
    memset(&st,0,sizeof(SETTEXTEX));
    st.flags = ST_DEFAULT;
    st.codepage = CP_ACP;

#ifdef DO_RTF
    SendMessage(hwndLogContent,EM_SETTEXTMODE,(WPARAM)TM_RICHTEXT,(LPARAM)0L);
    SendMessage(hwndRendererLogContent,EM_SETTEXTMODE,(WPARAM)TM_RICHTEXT,(LPARAM)0L);
#else
    SendMessage(hwndLogContent,EM_SETTEXTMODE,(WPARAM)TM_PLAINTEXT,(LPARAM)0L);
    SendMessage(hwndLogContent,EM_SETTEXTMODE,(WPARAM)TM_PLAINTEXT,(LPARAM)0L);
#endif

    SendMessage(hwndLogContent,EM_SETTEXTEX,(WPARAM)&st,(LPARAM)L"");
    SendMessage(hwndLogContent,EM_EXLIMITTEXT,(WPARAM)0L,(LPARAM)(32768 * 65535));

    SendMessage(hwndRendererLogContent,EM_SETTEXTEX,(WPARAM)&st,(LPARAM)L"");
    SendMessage(hwndRendererLogContent,EM_EXLIMITTEXT,(WPARAM)0L,(LPARAM)(32768 * 65535));

    memset(&logStream,0,sizeof(EDITSTREAM));
    logStream.dwCookie = (DWORD_PTR)hwndLogContent;
    logStream.pfnCallback = processLog;

    memset(&rendererLogStream,0,sizeof(EDITSTREAM));
    rendererLogStream.dwCookie = (DWORD_PTR)hwndRendererLogContent;
    rendererLogStream.pfnCallback = processLog;

    return 0;
    }