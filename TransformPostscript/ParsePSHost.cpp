
#include "ParsePostscript.h"

    ParsePSHost::ParsePSHost() {

    }

    ParsePSHost::~ParsePSHost() {

    if ( ! ( NULL == pIOleObject_HTML ) ) {
        pIOleObject_HTML -> SetClientSite(NULL);
        pIOleObject_HTML -> Release();
    }

    if ( ! ( NULL == pIOleInPlaceObject_HTML ) )
        pIOleInPlaceObject_HTML -> Release();

    if ( ! ( NULL == pIPostScript ) )
        pIPostScript -> Release();
    }


   long __stdcall ParsePSHost::QueryInterface(REFIID riid,void **ppv) {

   if ( ! ppv )
      return E_POINTER;

   *ppv = NULL;

   if ( IID_IUnknown == riid )
      *ppv = static_cast<IUnknown *>(this);
   else

   if ( IID_IOleInPlaceFrame == riid ) 
      *ppv = static_cast<IOleInPlaceFrame *>(pIOleInPlaceFrame_HTML_Host);
   else

   if ( IID_IOleInPlaceSite == riid ) 
      *ppv = static_cast<IOleInPlaceSite *>(pIOleInPlaceSite_HTML_Host);
   else

   if ( IID_IOleInPlaceSiteEx == riid ) 
      *ppv = static_cast<IOleInPlaceSiteEx *>(pIOleInPlaceSite_HTML_Host);
   else

   if ( IID_IOleDocumentSite == riid ) 
      *ppv = static_cast<IOleDocumentSite *>(pIOleDocumentSite_HTML_Host);
   else

      return E_NOINTERFACE;

   AddRef();
   return S_OK;
   }
   unsigned long __stdcall ParsePSHost::AddRef() {
   return ++refCount;
   }
   unsigned long __stdcall ParsePSHost::Release() { 
   return --refCount;
   }