
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