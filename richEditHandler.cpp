
#include "PostScript.h"

   LRESULT CALLBACK PStoPDF::richEditHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

   PStoPDF *p = (PStoPDF *)GetWindowLong(hwnd,GWL_USERDATA);

   switch ( msg ) {
   case WM_PAINT: {

      LRESULT rc = CallWindowProc(PStoPDF::nativeRichEditHandler,hwnd,msg,wParam,lParam);

      if ( p -> logPaintPending ) {
         DWORD countPaintMessages = GetQueueStatus(QS_PAINT);
         if ( 0 == HIWORD(countPaintMessages) && 0 == (QS_PAINT & LOWORD(countPaintMessages)) ) {
            p -> logPaintPending = false;
            PostMessage(hwnd,WM_FLUSH_LOG,0L,0L);
         }
      } 

      return rc;
      }

   case WM_FLUSH_LOG: {
      long lineCount = SendMessage(hwnd,EM_GETLINECOUNT,0L,0L);
      SendMessage(hwnd,EM_LINESCROLL,0L,lineCount - 32);
      ReleaseSemaphore(PStoPDF::hRichEditSemaphore,1,NULL);
      }
      return 0L;
   }

   return CallWindowProc(PStoPDF::nativeRichEditHandler,hwnd,msg,wParam,lParam);
   }