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

#include <windows.h>
#include <olectl.h>

#include "PostScriptInterpreter.h"
#include "utilities.h"

   long __stdcall PostScriptInterpreter::_IOleObject::QueryInterface(REFIID riid,void **ppv) {
 
   if ( IID_IOleObject == riid )
      *ppv = static_cast<IOleObject *>(this); 
   else
      return pParent -> QueryInterface(riid,ppv);

   AddRef();
  
   return S_OK; 
   }
 
   unsigned long __stdcall PostScriptInterpreter::_IOleObject::AddRef() {
   return pParent -> AddRef();
   }
 
   unsigned long __stdcall PostScriptInterpreter::_IOleObject::Release() {
   return pParent -> Release();
   }


   STDMETHODIMP PostScriptInterpreter::_IOleObject::SetClientSite(IOleClientSite *pcs) {

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
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::GetClientSite(IOleClientSite **pcs) {
   *pcs = pParent -> pIOleClientSite;
   pParent -> pIOleClientSite -> AddRef();
   return S_OK;
   }
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::Advise(IAdviseSink *pAdvSink,DWORD *pdwConnection) {
   return S_OK;
   }
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::Unadvise(DWORD dwConnection) {
   return S_OK;
   }
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::EnumAdvise(IEnumSTATDATA **ppenum) {
   return S_OK;
   }
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::SetExtent(DWORD dwDrawAspect,SIZEL *pSizel) {
   if ( dwDrawAspect != DVASPECT_CONTENT ) 
      return S_OK;

   SIZEL tempSizel;
   RECT rect = {0,0,0,0};

   hiMetricToPixels(pSizel,&tempSizel);

   rect.right = tempSizel.cx;
   rect.bottom = tempSizel.cy;

   SetWindowPos(pParent -> hwndClient,HWND_TOP,rect.left,rect.top,rect.right - rect.left,rect.bottom - rect.top,SWP_SHOWWINDOW);

   return S_OK;
   }
 
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::GetExtent(DWORD dwDrawAspect,SIZEL *pSizel) {
   RECT rcParent;
   GetWindowRect(pParent -> hwndHost,&rcParent);
   pSizel -> cx = rcParent.right - rcParent.left;
   pSizel -> cy = rcParent.bottom - rcParent.top;
   pixelsToHiMetric(pSizel,pSizel);
   return S_OK;
   }
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::DoVerb(LONG iVerb, LPMSG ,IOleClientSite *, LONG,HWND hwndParent,LPCRECT lprcPosRect) {

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
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::SetHostNames(LPCOLESTR szContainerApp,LPCOLESTR olestrContainerObject) {
   return E_NOTIMPL;
   }
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::Close(DWORD dwOptions) {
   return S_OK;
   }
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::SetMoniker(DWORD dwMonikerNo,IMoniker *pm) {
   return E_NOTIMPL;
   }
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::GetMoniker(DWORD dwAssign,DWORD dwMonikerNo,IMoniker **ppm) {
   if ( ! pParent -> pIOleClientSite ) 
      return E_NOTIMPL;
   return pParent -> pIOleClientSite -> GetMoniker(OLEGETMONIKER_FORCEASSIGN,OLEWHICHMK_OBJFULL,ppm);
   }
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::InitFromData(IDataObject *pdo,BOOL fCreation,DWORD dwReserved) {
   return E_NOTIMPL;
   }
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::GetClipboardData(DWORD,IDataObject **ppdo) {
   return E_NOTIMPL;
   }
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::EnumVerbs(IEnumOLEVERB **ppEnumOleVerb) {
   return E_NOTIMPL;
   }
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::Update() {
   return S_OK;
   }
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::IsUpToDate() {
   return S_OK;
   }
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::GetUserClassID(CLSID *pclsid) {
  *pclsid = CLSID_PostScriptInterpreter;
   return S_OK;
   }
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::GetUserType(DWORD dwTypeOfType,LPOLESTR *pszTypeName) {
   return E_NOTIMPL;
   }
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::GetMiscStatus(DWORD dwAspect,DWORD *dwStatus) {
   return E_NOTIMPL;
   }
 
 
   STDMETHODIMP PostScriptInterpreter::_IOleObject::SetColorScheme(LOGPALETTE *) {
   return S_OK;
   }
