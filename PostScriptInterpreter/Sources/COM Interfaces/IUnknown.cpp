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

   // IUnknown

   long __stdcall PStoPDF::QueryInterface(REFIID riid,void **ppv) {

   *ppv = NULL;

   if ( riid == IID_IUnknown )
      *ppv = static_cast<IUnknown *>(this);
   else 

   if ( riid == IID_IPostScript )
      *ppv = static_cast<IPostScript *>(this);
   else

   if ( riid == IID_IOleObject )
      return pIOleObject -> QueryInterface(riid,ppv);
   else 

   if ( IID_IConnectionPointContainer == riid ) 
      return pIConnectionPointContainer -> QueryInterface(riid,ppv);
   else

   if ( IID_IConnectionPoint == riid ) 
      return pIConnectionPoint -> QueryInterface(riid,ppv);
   else

   if ( IID_IOleObject == riid )
      return pIOleObject -> QueryInterface(riid,ppv);
   else

   if ( IID_IOleInPlaceObject == riid )
      return pIOleInPlaceObject -> QueryInterface(riid,ppv);
   else

      return E_NOINTERFACE;

   AddRef();

   return S_OK;
   }

   unsigned long __stdcall PStoPDF::AddRef() {
   refCount++;
   return refCount;
   }

   unsigned long __stdcall PStoPDF::Release() { 
   refCount--;
   if ( ! refCount ) {
      delete this;
      return 0;
   }
   return refCount;
   }