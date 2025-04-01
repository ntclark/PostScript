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
#include "PostScript_Log.h"

#include "job.h"

   char *replaceChar(char *pszString,char theChar,char *pszReplacement);

   static std::queue<char *> theLog;

   long PStoPDF::clearLog() {

   if ( NULL == hwndLog )
      return 0;

   SETTEXTEX st;
   memset(&st,0,sizeof(SETTEXTEX));
   st.flags = ST_DEFAULT;
   st.codepage = CP_ACP;

   PostMessage(hwndLog,EM_SETTEXTEX,(WPARAM)&st,(LPARAM)L"");

#ifdef DO_RTF
   PostMessage(hwndLog,EM_SETTEXTMODE,(WPARAM)TM_RICHTEXT,(LPARAM)0L);
#endif

   PostMessage(hwndLog,EM_SETREADONLY,(WPARAM)TRUE,0L);

   return 0;
   }


   long PStoPDF::queueLog(char *pszOutput,char *pEnd,bool isError) {

   if ( none == theLogLevel )
      return 0L;

    if ( NULL == pszOutput )
        return 0L;

   EnterCriticalSection(&theQueueCriticalSection);

   static char szOperandStackSize[64];
   sprintf_s<64>(szOperandStackSize,"%ld",(long)pJob -> operandStack.size());
   PostMessage(hwndOperandStackSize,WM_SETTEXT,0L,(LPARAM)szOperandStackSize);

   static char szCurrentDictionary[64];
   sprintf_s<64>(szCurrentDictionary,"%s",pJob -> dictionaryStack.top() -> Name());
   PostMessage(hwndCurrentDictionary,WM_SETTEXT,0L,(LPARAM)szCurrentDictionary);

   long n = 0L;

   if ( pEnd )
      n = (long)(pEnd - pszOutput);
   else
      n = (long)strlen(pszOutput);

   if ( 0 == n ) {
      LeaveCriticalSection(&theQueueCriticalSection);
      return 0L;
   }

   if ( isError )
      n += 32;

   char *pStart = pszOutput;

   pEnd = pStart + n;

   while ( n > 4094 ) {
      char *p = new char[4095];
      memcpy((BYTE *)p,pStart,4094);
      p[4094] = '\0';
      theLog.push(p);
      pStart += 4094;
      n -= 4094;
   }

   char *p = new char[pEnd - pStart + 2];
   memcpy((BYTE *)p,pStart,pEnd - pStart);
   p[pEnd - pStart] = '\0';

   if ( NULL == hwndLog ) {
      OutputDebugStringA(p);
      LeaveCriticalSection(&theQueueCriticalSection);
      return 0;
   }

   theLog.push(p);

#ifdef DO_RTF
   PostMessage(hwndLog,EM_STREAMIN,(WPARAM)(SF_RTF | SFF_SELECTION),(LPARAM)&logStream);
#else
   PostMessage(hwndLog,EM_STREAMIN,(WPARAM)(SF_TEXT | SFF_SELECTION),(LPARAM)&logStream);
#endif

   LeaveCriticalSection(&theQueueCriticalSection);

   return 0;
   }


    long PStoPDF::settleLog() {
    PostMessage(hwndLog,WM_VSCROLL,SB_BOTTOM,0L);
    return 0;
    }


    DWORD __stdcall PStoPDF::processLog(DWORD_PTR dwCookie,BYTE *pBuffer,LONG bufferSize,LONG *pBytesReturned) {

    EnterCriticalSection(&theQueueCriticalSection);

    if ( theLog.empty() ) {
        *pBytesReturned = 0;
        LeaveCriticalSection(&theQueueCriticalSection);
        return 1;
    }

    char *p = theLog.front();

    theLog.pop();

#ifdef DO_RTF

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

#if 1
    CHARRANGE chr;
    nativeRichEditHandler(hwndLog,EM_EXGETSEL,(WPARAM)0,(LPARAM)&chr);

    long charFirst = (long)nativeRichEditHandler(hwndLog,EM_LINEINDEX,(WPARAM)-1L,0L);

    if ( chr.cpMax > charFirst )
        *pBytesReturned = sprintf((char *)pBuffer,RESET_RTF" \\par%s",pNew);
    else
        *pBytesReturned = sprintf((char *)pBuffer,RESET_RTF"%s",pNew);
#endif

    //*pBytesReturned = sprintf((char *)pBuffer,"%s",pNew);
    //*pBytesReturned = n;

    //memcpy(pBuffer,p,n);

    if ( ! ( pNew == p ) )
        delete [] pNew;
#else

    long n = (long)strlen(p);

    if ( n > bufferSize ) {
        p[bufferSize - 1] = '\0';
        n = bufferSize;
    }

    *pBytesReturned = n;

    memcpy(pBuffer,p,n);

#endif

    delete [] p;

    settleLog();

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
