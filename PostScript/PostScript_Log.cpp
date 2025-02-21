
#include "PostScript.h"
#include "PostScript_Log.h"

#include "job.h"

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
   //PostMessage(hwndLog,EM_STREAMIN,(WPARAM)(SF_TEXT | SFF_SELECTION),(LPARAM)&logStream);
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

    long n = (long)strlen(p);

    if ( n > bufferSize ) {
        p[bufferSize - 1] = '\0';
        n = bufferSize;
    }

#ifdef DO_RTF
    CHARRANGE chr;
    nativeRichEditHandler(hwndLog,EM_EXGETSEL,(WPARAM)0,(LPARAM)&chr);

    long charFirst = nativeRichEditHandler(hwndLog,EM_LINEINDEX,(WPARAM)-1L,0L);

    if ( chr.cpMax > charFirst )
        *pBytesReturned = sprintf((char *)pBuffer,RESET_RTF" \\par%s",p);
    else
        *pBytesReturned = sprintf((char *)pBuffer,RESET_RTF"%s",p);
    
    //*pBytesReturned = sprintf((char *)pBuffer,"%s",p);
    //*pBytesReturned = n;
    //memcpy(pBuffer,p,n);

#else

    *pBytesReturned = n;

    memcpy(pBuffer,p,n);

#endif

    delete [] p;

    settleLog();

    LeaveCriticalSection(&theQueueCriticalSection);

    return 0;
    }