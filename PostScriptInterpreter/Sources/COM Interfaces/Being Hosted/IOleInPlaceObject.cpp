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

   long __stdcall PostScriptInterpreter::_IOleInPlaceObject::QueryInterface(REFIID riid,void **ppv) {
 
   if ( IID_IOleInPlaceObject == riid )
      *ppv = static_cast<IOleInPlaceObject *>(this); 
   else
      return pParent -> QueryInterface(riid,ppv);

   AddRef();
  
   return S_OK; 
   }
 
   unsigned long __stdcall PostScriptInterpreter::_IOleInPlaceObject::AddRef() {
   return pParent -> AddRef();
   }
 
   unsigned long __stdcall PostScriptInterpreter::_IOleInPlaceObject::Release() {
   return pParent -> Release();
   }


   STDMETHODIMP PostScriptInterpreter::_IOleInPlaceObject::GetWindow(HWND *pHwnd) {
   *pHwnd = pParent -> hwndClient;
   return S_OK;
   }
 
   STDMETHODIMP PostScriptInterpreter::_IOleInPlaceObject::ContextSensitiveHelp(BOOL) {
   return E_NOTIMPL;
   }
 
   STDMETHODIMP PostScriptInterpreter::_IOleInPlaceObject::InPlaceActivate() {
   return S_OK;
   }

   STDMETHODIMP PostScriptInterpreter::_IOleInPlaceObject::InPlaceDeactivate() {
   return S_OK;
   }
 
   STDMETHODIMP PostScriptInterpreter::_IOleInPlaceObject::UIDeactivate() {
   return S_OK;
   }
 
   STDMETHODIMP PostScriptInterpreter::_IOleInPlaceObject::SetObjectRects(LPCRECT pRectPos,LPCRECT pRectClip) {
   SetWindowPos(pParent -> hwndClient,HWND_TOP,pRectPos -> left,pRectPos -> top,
                               pRectPos -> right - pRectPos -> left,pRectPos -> bottom - pRectPos -> top,SWP_SHOWWINDOW);
   return S_OK;
   }
 
   STDMETHODIMP PostScriptInterpreter::_IOleInPlaceObject::ReactivateAndUndo() {
   return S_OK;
   }