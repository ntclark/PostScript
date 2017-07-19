
#include "PostScript.h"

   long __stdcall PStoPDF::_IOleInPlaceObject::QueryInterface(REFIID riid,void **ppv) {
 
   if ( IID_IOleInPlaceObject == riid )
      *ppv = static_cast<IOleInPlaceObject *>(this); 
   else
      return pParent -> QueryInterface(riid,ppv);

   AddRef();
  
   return S_OK; 
   }
 
   unsigned long __stdcall PStoPDF::_IOleInPlaceObject::AddRef() {
   return pParent -> AddRef();
   }
 
   unsigned long __stdcall PStoPDF::_IOleInPlaceObject::Release() {
   return pParent -> Release();
   }


   STDMETHODIMP PStoPDF::_IOleInPlaceObject::GetWindow(HWND *pHwnd) {
   *pHwnd = pParent -> hwndClient;
   return S_OK;
   }
 
   STDMETHODIMP PStoPDF::_IOleInPlaceObject::ContextSensitiveHelp(BOOL) {
   return E_NOTIMPL;
   }
 
   STDMETHODIMP PStoPDF::_IOleInPlaceObject::InPlaceActivate() {
   return S_OK;
   }

   STDMETHODIMP PStoPDF::_IOleInPlaceObject::InPlaceDeactivate() {
   return S_OK;
   }
 
   STDMETHODIMP PStoPDF::_IOleInPlaceObject::UIDeactivate() {
   return S_OK;
   }
 
   STDMETHODIMP PStoPDF::_IOleInPlaceObject::SetObjectRects(LPCRECT pRectPos,LPCRECT pRectClip) {
   SetWindowPos(pParent -> hwndClient,HWND_TOP,pRectPos -> left,pRectPos -> top,
                               pRectPos -> right - pRectPos -> left,pRectPos -> bottom - pRectPos -> top,SWP_SHOWWINDOW);
   return S_OK;
   }
 
   STDMETHODIMP PStoPDF::_IOleInPlaceObject::ReactivateAndUndo() {
   return S_OK;
   }