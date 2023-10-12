// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PostScript.h"
#include "job.h"

   HRESULT PStoPDF::Parse(wchar_t *wsFileName) {

   if ( pJob )
      delete pJob;

   long n = (DWORD)wcslen(wsFileName) + 1;
   char *pszFileName = new char[n];
   memset(pszFileName,0,n * sizeof(char));

   WideCharToMultiByte(CP_ACP,0,wsFileName,-1,pszFileName,n,0,0);

#if 1
   pJob = new job(pszFileName,NULL,NULL,NULL,NULL);
#else
   pJob = new job("D:\\CursiVision\\PStoPDF\\Test Page.ps",NULL,NULL,NULL,NULL);
#endif

   pJob -> parseJob(false);

   delete [] pszFileName;

   return S_OK;
   }


   HRESULT PStoPDF::Convert(wchar_t *wsFileName) {

   if ( pJob )
      delete pJob;

   long n = (DWORD)wcslen(wsFileName) + 1;
   char *pszFileName = new char[n];
   memset(pszFileName,0,n * sizeof(char));

   WideCharToMultiByte(CP_ACP,0,wsFileName,-1,pszFileName,n,0,0);

#if 1
   pJob = new job(pszFileName,NULL,NULL,NULL,NULL);
#else
   pJob = new job("D:\\CursiVision\\PStoPDF\\Test Page.ps",NULL,NULL,NULL,NULL);
#endif

   pJob -> parseJob();

   delete [] pszFileName;

   return S_OK;
   }


   HRESULT PStoPDF::ParseText(char *pszStream,long length,void *pv_pipage,void *pvIPostScriptTakeText,HDC hdcTarget,RECT *prcWindowsClip) {

   if ( pJob )
      delete pJob;   

   loggingOff = 32768L;

   PdfPage *pPdfPage = NULL;

   reinterpret_cast<IPdfPage *>(pv_pipage) -> GetNativePdfPage(reinterpret_cast<void **>(&pPdfPage));

   pJob = new job(NULL,pPdfPage,hdcTarget,prcWindowsClip,reinterpret_cast<IPostScriptTakeText *>(pvIPostScriptTakeText));

   try { 

   pJob -> execute(pszStream,NULL,length);

   if ( hdcTarget )
      ModifyWorldTransform(hdcTarget,NULL,MWT_IDENTITY);

   } catch ( PStoPDFException pe ) {

      strcpy(szErrorMessage,pe.Message());

      return E_FAIL;

   } catch ( std::exception e ) {

      strcpy(szErrorMessage,e.what());

      return E_FAIL;

   }

   return S_OK;
   }


   HRESULT PStoPDF::GetLastError(char **ppszError) {
   if ( ! ppszError )
      return E_POINTER;
   *ppszError = szErrorMessage;
   return S_OK;
   }