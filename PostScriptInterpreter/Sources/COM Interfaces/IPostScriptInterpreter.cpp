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
#include "job.h"

    HRESULT PostScriptInterpreter::SetSource(char *pszFileName) {

    struct _stat fstat;
    if ( ! ( 0 == _stat(pszFileName,&fstat) ) ) {
        if ( ! ( NULL == hwndCmdPane ) ) {
            char *p = strrchr(pszFileName,'/');
            if ( ! p )
                p = strrchr(pszFileName,'\\');
            if ( ! p )
                p = pszFileName - 1;
            sprintf_s<1024>(szErrorMessage,"Error: The file \"%s\" does not exist",p + 1);
            SetWindowText(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_ERROR),szErrorMessage);
        }
        return E_FAIL;
    }

    strcpy(szCurrentPostScriptFile,pszFileName);

    if ( ! ( NULL == hwndCmdPane ) )
        SetWindowText(GetDlgItem(hwndCmdPane,IDDI_CMD_PANE_ACTIVE_FILE),szCurrentPostScriptFile);

    return S_OK;
    }


    HRESULT PostScriptInterpreter::Parse(char *pszFileName,BOOL autoStart) {

    if ( pJob )
        delete pJob;

    HRESULT rc = S_OK;
    if ( ! ( NULL == pszFileName ) ) 
        rc = SetSource(pszFileName);

    if ( ! ( S_OK == rc ) )
        return E_FAIL;

    if ( FALSE == autoStart )
        return S_OK;

//_CrtSetBreakAlloc(281);

    pJob = new job(szCurrentPostScriptFile,hwndClient);

    pJob -> parseJob(NULL == hwndClient ? false : true);

    return S_OK;
    }


    HRESULT PostScriptInterpreter::ParseText(char *pszStream) {
    if ( pJob )
        delete pJob;
    pJob = new job(NULL,NULL);
    WaitForSingleObject(pJob -> hsemIsInitialized,INFINITE);
    pJob -> execute(pszStream,pszStream + strlen(pszStream),"<unnamed>");
    return S_OK;
   }


    HRESULT PostScriptInterpreter::GetLastError(char **ppszError) {
    if ( ! ppszError )
        return E_POINTER;
    *ppszError = szErrorMessage;
    return S_OK;
    }


    HRESULT PostScriptInterpreter::LogLevel(enum logLevel logLevel) {
    theLogLevel = logLevel;
    return S_OK;
    }


    HRESULT PostScriptInterpreter::RendererLogLevel(enum logLevel logLevel) {
    theRendererLogLevel = logLevel;
    return S_OK;
    }


    HRESULT PostScriptInterpreter::GetPeristableProperties(UINT_PTR *pProperties,long *pSize) {

    long cbProperties = (long)(offsetof(PostScriptInterpreter,persistablePropertiesEnd) - 
                                offsetof(PostScriptInterpreter,persistablePropertiesStart));

    uint8_t *pProps = (uint8_t *)CoTaskMemAlloc(cbProperties);
    *pProperties = (UINT_PTR)pProps;
    *pSize = cbProperties;

    RECT rc;

    GetWindowRect(hwndLogContent,&rc);

    logPaneWidth = rc.right - rc.left;

    memcpy(pProps,&logPaneWidth,sizeof(logPaneWidth));
    pProps += sizeof(logPaneWidth);

    GetWindowRect(hwndRendererLogContent,&rc);

    rendererLogPaneWidth = rc.right - rc.left;

    memcpy(pProps,&rendererLogPaneWidth,sizeof(rendererLogPaneWidth));
    pProps += sizeof(rendererLogPaneWidth);

    memcpy(pProps,&logIsVisible,sizeof(logIsVisible));
    pProps += sizeof(logIsVisible);

    memcpy(pProps,&rendererLogIsVisible,sizeof(rendererLogIsVisible));
    pProps += sizeof(rendererLogIsVisible);

    memcpy(pProps,szCurrentPostScriptFile,sizeof(szCurrentPostScriptFile));

    return S_OK;
    }


    HRESULT PostScriptInterpreter::SetPeristableProperties(UINT_PTR pProperties) {

    uint8_t *pProps = (uint8_t *)pProperties;

    long x,y;
    memcpy(&x,pProps,sizeof(logPaneWidth));
    pProps += sizeof(logPaneWidth);

    memcpy(&y,pProps,sizeof(rendererLogPaneWidth));
    pProps += sizeof(rendererLogPaneWidth);

    memcpy(&logIsVisible,pProps,sizeof(logIsVisible));
    pProps += sizeof(logIsVisible);
    if ( logIsVisible ) {
        logIsVisible = false;
        toggleLogVisibility(IDDI_CMD_PANE_LOG_SHOW);
        logPaneWidth = x;
    }

    memcpy(&rendererLogIsVisible,pProps,sizeof(rendererLogIsVisible));
    pProps += sizeof(rendererLogIsVisible);

    if ( rendererLogIsVisible ) {
        rendererLogIsVisible = false;
        toggleLogVisibility(IDDI_CMD_PANE_RENDERER_LOG_SHOW);
        rendererLogPaneWidth = y;
    }

    if ( logIsVisible || rendererLogIsVisible )
        setWindowPanes();

    if ( '\0' == szCurrentPostScriptFile[0] ) {
        memcpy(szCurrentPostScriptFile,pProps,sizeof(szCurrentPostScriptFile));
        if ( ! ( '\0' == szCurrentPostScriptFile[0] ) )
            if ( ! ( S_OK == SetSource(szCurrentPostScriptFile) ) )
                memset(szCurrentPostScriptFile,0,sizeof(szCurrentPostScriptFile));
    }

    return S_OK;
    }