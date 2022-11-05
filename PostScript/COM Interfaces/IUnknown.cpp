// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

#if 0
   if ( IID_IObjectSafety == riid )
      return pIObjectSafety -> QueryInterface(riid,ppv);
   else
#endif

   if ( IID_IOleObject == riid )
      return pIOleObject -> QueryInterface(riid,ppv);
   else
#if 0
   if ( IID_IOleControl == riid ) 
      return pIOleControl -> QueryInterface(riid,ppv);
   else
#endif

   if ( IID_IOleInPlaceObject == riid )
      return pIOleInPlaceObject -> QueryInterface(riid,ppv);
   else

#if 0
   if ( IID_IOleInPlaceActiveObject == riid )
      return pIOleInPlaceActiveObject -> QueryInterface(riid,ppv);
   else
#endif

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