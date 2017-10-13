
#include "PostScript objects\matrix.h"

#include "job.h"

#include "PostScript.h"

   LRESULT CALLBACK PStoPDF::handler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

   PStoPDF *p = (PStoPDF *)GetWindowLongPtr(hwnd,GWLP_USERDATA);

   switch ( msg ) {
   case WM_CREATE: {
      CREATESTRUCT *pc = reinterpret_cast<CREATESTRUCT *>(lParam);
      p = reinterpret_cast<PStoPDF *>(pc -> lpCreateParams);
      SetWindowLongPtr(hwnd,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(p));
      }
      break;

   case WM_TIMER: 
      break;

   case WM_COMMAND: {
      switch ( LOWORD(wParam) ) {
      }
      
      }
      break;

   case WM_PAINT: {

      PAINTSTRUCT psPaint;
      BeginPaint(hwnd,&psPaint);
//      FillRect(psPaint.hdc,&psPaint.rcPaint,(HBRUSH)(COLOR_WINDOW + 1));
      EndPaint(hwnd,&psPaint);

      return (LRESULT)TRUE;
      }

   case WM_SIZE: {

      if ( SIZE_MINIMIZED == wParam )
         break;

      if ( ! p )
         break;

      long cx = LOWORD(lParam);
      long cy = HIWORD(lParam);

      SetWindowPos(p -> hwndLog,HWND_TOP,0,0,3 * cx / 4,cy,SWP_NOMOVE);
      SetWindowPos(p -> hwndStack,HWND_TOP,3 * cx / 4,0,cx / 4,cy,0L);

      }
      break;

   case WM_MOVE: 
      break;

   case WM_DESTROY:
      break;

   case WM_INITMENUPOPUP: 
      break;

   case WM_DROPFILES: {
      wchar_t szFiles[MAX_PATH];
      HDROP hDropInfo = reinterpret_cast<HDROP>(wParam);
      for ( long k = 0; k < (long)DragQueryFile(hDropInfo,0xFFFFFFFF,NULL,0L); k++ ) {
         DragQueryFileW(hDropInfo,k,szFiles,MAX_PATH);
         p -> Convert(szFiles);
         break;
      }
      }
      break;

   case WM_FLUSH_LOG: {
      return 0L;
      }
      break;

   default:
      break;
   }

   return DefWindowProc(hwnd,msg,wParam,lParam);
   }