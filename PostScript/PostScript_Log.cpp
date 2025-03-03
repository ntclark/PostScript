// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PostScript.h"
#include "PostScript_Log.h"

   static std::queue<char *> theLog;

   long PStoPDF::clearLog() {   

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

   EnterCriticalSection(&theQueueCriticalSection);

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
      return 0;
   }

   theLog.push(p);

   hRichEditSemaphore = CreateSemaphore(NULL,0,1,NULL);

#ifdef DO_RTF

   PostMessage(hwndLog,EM_STREAMIN,(WPARAM)(SF_RTF | SFF_SELECTION),(LPARAM)&logStream);

#else

   PostMessage(hwndLog,EM_STREAMIN,(WPARAM)(SF_TEXT | SFF_SELECTION),(LPARAM)&logStream);

#endif

   LeaveCriticalSection(&theQueueCriticalSection);

   while ( WaitForSingleObject(hRichEditSemaphore,500) ) {
//      if ( ! logPaintPending )
         break;
   }//;INFINITE);

   CloseHandle(hRichEditSemaphore);

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

   long n = (long)strlen((char *)p);

   if ( n > bufferSize ) {
      p[bufferSize - 1] = '\0';
      n = bufferSize;
   }

#ifdef DO_RTF
   CHARRANGE chr;
   SendMessage(((PStoPDF *)dwCookie) -> hwndLog,EM_EXGETSEL,(WPARAM)0,(LPARAM)&chr);

   long charFirst = SendMessage(((PStoPDF *)dwCookie) -> hwndLog,EM_LINEINDEX,(WPARAM)-1L,0L);

   if ( chr.cpMax > charFirst )
      *pBytesReturned = sprintf((char *)pBuffer,RESET_RTF" \\par%s",p);
   else
      *pBytesReturned = sprintf((char *)pBuffer,RESET_RTF"%s",p);

#else

   *pBytesReturned = n;
   memcpy(pBuffer,p,n);

#endif

   delete [] p;

   logPaintPending = true;

   LeaveCriticalSection(&theQueueCriticalSection);

   return 0;
   }