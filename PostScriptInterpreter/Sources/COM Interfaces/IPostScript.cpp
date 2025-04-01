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
#include "job.h"


    HRESULT PStoPDF::SetSource(char *pszFileName) {

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


    HRESULT PStoPDF::Parse(char *pszFileName) {

    if ( pJob )
        delete pJob;

    HRESULT rc = S_OK;
    if ( ! ( NULL == pszFileName ) ) 
        rc = SetSource(pszFileName);

    if ( ! ( S_OK == rc ) )
        return E_FAIL;

    pJob = new job(szCurrentPostScriptFile,hwndClient,NULL);

    pJob -> parseJob(NULL == hwndClient ? false : true);

    return S_OK;
    }


    HRESULT PStoPDF::ParseText(char *pszStream,long length,void *pv_pipage,void *pvIPostScriptTakeText,HDC hdcTarget,RECT *prcWindowsClip) {

#if 1
    if ( pJob )
        delete pJob;

    pJob = new job();

    WaitForSingleObject(pJob -> hsemIsInitialized,INFINITE);

    pJob -> execute(pszStream);

#else
    if ( pJob )
        delete pJob;

    PdfPage *pPdfPage = NULL;

    reinterpret_cast<IPdfPage *>(pv_pipage) -> GetNativePdfPage(reinterpret_cast<void **>(&pPdfPage));

    pJob = new job(NULL,pPdfPage,hdcTarget,prcWindowsClip,reinterpret_cast<IPostScriptTakeText *>(pvIPostScriptTakeText));

    try { 

    pJob -> execute(pszStream);

    if ( hdcTarget )
        ModifyWorldTransform(hdcTarget,NULL,MWT_IDENTITY);

    } catch ( PStoPDFException pe ) {

        strcpy(szErrorMessage,pe.Message());

        return E_FAIL;

    } catch ( std::exception e ) {

        strcpy(szErrorMessage,e.what());

        return E_FAIL;

    }

    delete pJob;

    pJob = NULL;
#endif

    return S_OK;
   }


    HRESULT PStoPDF::GetLastError(char **ppszError) {
    if ( ! ppszError )
        return E_POINTER;
    *ppszError = szErrorMessage;
    return S_OK;
    }


    HRESULT PStoPDF::LogLevel(enum logLevel logLevel) {
    theLogLevel = logLevel;
    return S_OK;
    }