
#include <windows.h>
#include <olectl.h>

#include "PostScript.h"
#include "utilities.h"

   long __stdcall PStoPDF::_IOleObject::QueryInterface(REFIID riid,void **ppv) {
 
   if ( IID_IOleObject == riid )
      *ppv = static_cast<IOleObject *>(this); 
   else
      return pParent -> QueryInterface(riid,ppv);

   AddRef();
  
   return S_OK; 
   }
 
   unsigned long __stdcall PStoPDF::_IOleObject::AddRef() {
   return pParent -> AddRef();
   }
 
   unsigned long __stdcall PStoPDF::_IOleObject::Release() {
   return pParent -> Release();
   }


   STDMETHODIMP PStoPDF::_IOleObject::SetClientSite(IOleClientSite *pcs) {

   if ( pParent -> pIOleClientSite )
      pParent -> pIOleClientSite -> Release();

   if ( ! pcs )
      return S_OK;
 
   pParent -> pIOleClientSite = pcs;
 
   pParent -> pIOleClientSite -> AddRef();
 
   if ( pParent -> pIOleInPlaceSite ) 
      pParent -> pIOleInPlaceSite -> Release();

   pParent -> pIOleInPlaceSite = NULL;
   pParent -> pIOleClientSite -> QueryInterface(IID_IOleInPlaceSite,(void **)&pParent -> pIOleInPlaceSite);

   pParent -> initWindows();

   return S_OK;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::GetClientSite(IOleClientSite **pcs) {
   *pcs = pParent -> pIOleClientSite;
   pParent -> pIOleClientSite -> AddRef();
   return S_OK;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::Advise(IAdviseSink *pAdvSink,DWORD *pdwConnection) {
   return S_OK;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::Unadvise(DWORD dwConnection) {
   return S_OK;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::EnumAdvise(IEnumSTATDATA **ppenum) {
   return S_OK;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::SetExtent(DWORD dwDrawAspect,SIZEL *pSizel) {
   if ( dwDrawAspect != DVASPECT_CONTENT ) 
      return S_OK;

   SIZEL tempSizel;
   RECT rect = {0,0,0,0};
   hiMetricToPixel(pSizel,&tempSizel);
   rect.right = tempSizel.cx;
   rect.bottom = tempSizel.cy;

   SetWindowPos(pParent -> hwndHost,HWND_TOP,rect.left,rect.top,rect.right - rect.left,rect.bottom - rect.top,SWP_SHOWWINDOW);

   return S_OK;
   }
 
 
 
   STDMETHODIMP PStoPDF::_IOleObject::GetExtent(DWORD dwDrawAspect,SIZEL *pSizel) {
   RECT rcParent;
   GetWindowRect(pParent -> hwndHost,&rcParent);
   pSizel -> cx = rcParent.right - rcParent.left;
   pSizel -> cy = rcParent.bottom - rcParent.top;
   pixelToHiMetric(pSizel,pSizel);
   return S_OK;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::DoVerb(LONG iVerb, LPMSG ,IOleClientSite *, LONG,HWND hwndParent,LPCRECT lprcPosRect) {

   switch ( iVerb ) {
 
   case OLEIVERB_PROPERTIES: {
      }
      break;
 
   case OLEIVERB_UIACTIVATE:
      break;
 
   case OLEIVERB_HIDE:
      ShowWindow(pParent -> hwndHost,SW_HIDE);
      break;
 
   case OLEIVERB_SHOW: 
 
      if ( ! pParent -> hwndHost )
         pParent -> initWindows();
 
      if ( lprcPosRect )
         pParent -> pIOleInPlaceObject -> SetObjectRects(lprcPosRect,NULL);
 
      break;
 
    case OLEIVERB_INPLACEACTIVATE:
       pParent -> pIOleInPlaceObject -> SetObjectRects(lprcPosRect,NULL);
       pParent -> pIOleInPlaceSite -> OnInPlaceActivate();
       break;
 
   default:
      break;
   }

   return S_OK;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::SetHostNames(LPCOLESTR szContainerApp,LPCOLESTR olestrContainerObject) {
   return E_NOTIMPL;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::Close(DWORD dwOptions) {
   return S_OK;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::SetMoniker(DWORD dwMonikerNo,IMoniker *pm) {
   return E_NOTIMPL;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::GetMoniker(DWORD dwAssign,DWORD dwMonikerNo,IMoniker **ppm) {
   if ( ! pParent -> pIOleClientSite ) 
      return E_NOTIMPL;
   return pParent -> pIOleClientSite -> GetMoniker(OLEGETMONIKER_FORCEASSIGN,OLEWHICHMK_OBJFULL,ppm);
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::InitFromData(IDataObject *pdo,BOOL fCreation,DWORD dwReserved) {
   return E_NOTIMPL;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::GetClipboardData(DWORD,IDataObject **ppdo) {
   return E_NOTIMPL;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::EnumVerbs(IEnumOLEVERB **ppEnumOleVerb) {
   return E_NOTIMPL;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::Update() {
   return S_OK;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::IsUpToDate() {
   return S_OK;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::GetUserClassID(CLSID *pclsid) {
  *pclsid = CLSID_PostScript;
   return S_OK;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::GetUserType(DWORD dwTypeOfType,LPOLESTR *pszTypeName) {
   return E_NOTIMPL;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::GetMiscStatus(DWORD dwAspect,DWORD *dwStatus) {
   return E_NOTIMPL;
   }
 
 
   STDMETHODIMP PStoPDF::_IOleObject::SetColorScheme(LOGPALETTE *) {
   return S_OK;
   }
