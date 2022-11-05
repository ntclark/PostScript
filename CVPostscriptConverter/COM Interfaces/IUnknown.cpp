
#include "CVPostscriptConverter.h"

    // IUnknown

    long __stdcall CVPostscriptConverter::QueryInterface(REFIID riid,void **ppv) {

    if ( ! ppv )
        return E_POINTER;

    *ppv = NULL;

    if ( IID_IUnknown == riid )
        *ppv = static_cast<IUnknown *>(this);
    else

    if ( IID_ICVPostscriptConverter == riid )
        *ppv = static_cast<ICVPostscriptConverter *>(this);
    
#if 0
   else

   if ( IID_IConnectionPointContainer == riid ) 
      *ppv = static_cast<IConnectionPointContainer *>(&connectionPointContainer);
   else

   if ( IID_IConnectionPoint == riid ) 
      *ppv = static_cast<IConnectionPoint *>(&connectionPoint);
   else

   if ( IID_IGPropertyPageClient == riid )
      return pIGPropertyPageClient -> QueryInterface(riid,ppv);
   else

   if ( IID_IGPropertiesClient == riid )
      *ppv = static_cast<IGPropertiesClient *>(this);
   else
#endif
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
    }

    unsigned long __stdcall CVPostscriptConverter::AddRef() {
    return ++refCount;
    }

    unsigned long __stdcall CVPostscriptConverter::Release() { 
    if ( 1 == refCount ) {
        delete this;
        return 0;
    }    
    return --refCount;
    }
