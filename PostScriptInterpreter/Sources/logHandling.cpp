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

#include "job.h"

    char *replaceChar(char *pszString,char theChar,char *pszReplacement);

    static std::queue<char *> theLogQueue;
    static std::queue<char *> theRendererLogQueue;

    long PostScriptInterpreter::clearLog(HWND hwndLog) {

    if ( NULL == hwndLog )
        return 0;

    SETTEXTEX st;
    memset(&st,0,sizeof(SETTEXTEX));
    st.flags = ST_DEFAULT;
    st.codepage = CP_ACP;

    PostMessage(hwndLog,EM_SETTEXTEX,(WPARAM)&st,(LPARAM)L"");

    //PostMessage(hwndLog,EM_SETTEXTMODE,(WPARAM)TM_RICHTEXT,(LPARAM)0L);

    return 0;
    }


    long PostScriptInterpreter::queueLog(boolean isPostScriptLog,char *pszOutput,char *pEnd,bool isError) {

    if ( isPostScriptLog && none == theLogLevel )
        return 0L;

    if ( isPostScriptLog ) {
        if ( none == theLogLevel )
            return 0L;
    } else
        if ( none == theRendererLogLevel )
            return 0L;

    if ( NULL == pszOutput )
        return 0L;

    EnterCriticalSection(&theQueueCriticalSection);

    std::queue<char *> *pLog = NULL;
    HWND hwndLog = NULL;
    EDITSTREAM *pLogStream = NULL;

    if ( isPostScriptLog ) {
        pLog = &theLogQueue;
        hwndLog = hwndLogContent;
        pLogStream = &logStream;
        static char szOperandStackSize[64];
        sprintf_s<64>(szOperandStackSize,"%ld",(long)pJob -> pOperandStack -> size());
        PostMessage(hwndOperandStackSize,WM_SETTEXT,0L,(LPARAM)szOperandStackSize);
        static char szCurrentDictionary[64];
        sprintf_s<64>(szCurrentDictionary,"%s",pJob -> pDictionaryStack -> top() -> Name());
        PostMessage(hwndCurrentDictionary,WM_SETTEXT,0L,(LPARAM)szCurrentDictionary);
    } else {
        pLog = &theRendererLogQueue;
        hwndLog = hwndRendererLogContent;
        pLogStream = &rendererLogStream;
    }

    long n = 0L;

    if ( pEnd )
        n = (long)(pEnd - pszOutput);
    else
        n = (long)strlen(pszOutput);

    if ( 0 == n ) {
        LeaveCriticalSection(&theQueueCriticalSection);
        return 0L;
    }

    long cbError = 0;

    if ( isError ) {
        char szError[128];
        cbError = sprintf_s<128>(szError,"%sError: ",COLOR_RED);
        n += cbError;
    }

    char *pStart = pszOutput;

    pEnd = pStart + n;

    while ( n > 4094 ) {
        char *p = new char[4095];
        memcpy((BYTE *)p,pStart,4094);
        p[4094] = '\0';
        pLog -> push(p);
        pStart += 4094;
        n -= 4094;
    }

    char *p = new char[pEnd - pStart + 2];
    memset(p,0,(pEnd - pStart + 2) * sizeof(char));

    if ( isError ) {
        sprintf_s(p,pEnd - pStart + 2,"%sError: ",COLOR_RED);
        memcpy((BYTE *)(p + cbError),pStart,pEnd - pStart - cbError);
    } else
        memcpy((BYTE *)p,pStart,pEnd - pStart);
        
    p[pEnd - pStart] = '\0';

    if ( NULL == hwndLog ) {
        OutputDebugStringA(p);
        LeaveCriticalSection(&theQueueCriticalSection);
        return 0;
    }

    pLog -> push(p);

    PostMessage(hwndLog,EM_STREAMIN,(WPARAM)(SF_RTF | SFF_SELECTION),(LPARAM)pLogStream);

    LeaveCriticalSection(&theQueueCriticalSection);

    return 0;
    }


    long PostScriptInterpreter::settleLog(HWND hwndLog) {
    PostMessage(hwndLog,WM_VSCROLL,SB_BOTTOM,0L);
    return 0;
    }


    DWORD __stdcall PostScriptInterpreter::processLog(DWORD_PTR dwCookie,BYTE *pBuffer,LONG bufferSize,LONG *pBytesReturned) {

    HWND hwndLog = (HWND)dwCookie;

    std::queue<char *> *pLog = &theLogQueue;
    if ( PostScriptInterpreter::hwndRendererLogContent == hwndLog )
        pLog = &theRendererLogQueue;

    EnterCriticalSection(&theQueueCriticalSection);

    if ( pLog -> empty() ) {
        *pBytesReturned = 0;
        LeaveCriticalSection(&theQueueCriticalSection);
        return 1;
    }

    char *p = pLog -> front();

    pLog -> pop();

    char *pNew = replaceChar(p,'{',"\\{");

    if ( ! ( p == pNew ) ) {
        char *pNew2 = replaceChar(pNew,'/',"\\/");
        if ( ! ( pNew2 == pNew ) ) {
            delete [] pNew;
            pNew = pNew2;
        }
    }

    long n = (long)strlen(pNew);

    if ( n > bufferSize ) {
        pNew[bufferSize - 1] = '\0';
        n = bufferSize;
    }

    //CHARRANGE chr;
    //nativeRichEditHandler(hwndLog,EM_EXGETSEL,(WPARAM)0,(LPARAM)&chr);
    //long charFirst = (long)nativeRichEditHandler(hwndLog,EM_LINEINDEX,(WPARAM)-1L,0L);

    *pBytesReturned = sprintf((char *)pBuffer,RESET_RTF"%s\\line\r",pNew);

    if ( ! ( pNew == p ) )
        delete [] pNew;

    delete [] p;

    settleLog(hwndLog);

    LeaveCriticalSection(&theQueueCriticalSection);

    return 0;
    }


    char *replaceChar(char *pszString,char theChar,char *pszRepString) {

    char *p = strchr(pszString,theChar);

    if ( ! p )
        return pszString;

    long countReplacement = (DWORD)strlen(pszRepString);

    long countOriginal = (DWORD)strlen(pszString);

    char *pszReplacement = new char[countOriginal + 1];
    strcpy(pszReplacement,pszString);

    char *pStart = pszReplacement;
    p = pStart + (p - pszString);

    while ( p ) {

        long countBefore = (long)(p - pStart);
        *p = '\0';
        long countAfter = (DWORD)strlen(p + 1);

        char *pszRemainder = new char[countAfter + 1];
        strcpy(pszRemainder,p + 1);

        char *pszOldReplacement = pszReplacement;

        countReplacement = countBefore + countReplacement + countAfter;

        pszReplacement = new char[countReplacement + 1];
        memset(pszReplacement,0,(countReplacement + 1) * sizeof(char));

        strcat(pszReplacement,pStart);
        strcat(pszReplacement,pszRepString);
        strcat(pszReplacement,pszRemainder);

        pStart = pszReplacement;

        delete [] pszOldReplacement;

        p = strrchr(pStart + countBefore + 2,theChar);

    }

    return pszReplacement;
    }
