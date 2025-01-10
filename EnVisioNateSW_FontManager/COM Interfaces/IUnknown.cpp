
#include "EnVisioNateSW_FontManager.h"

    HRESULT FontManager::QueryInterface(REFIID refIID,void **pvResult) {

    if ( ! pvResult )
        return E_POINTER;

    *pvResult = NULL;

    if ( IID_IUnknown == refIID ) 
        *pvResult = static_cast<IUnknown *>(this);

    else if ( IID_IFontManager == refIID )
        *pvResult = static_cast<IFontManager *>(this);

    if ( * pvResult ) {
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
    }


    ULONG FontManager::AddRef() {
    return ++refCount;
    }


    ULONG FontManager::Release() {
    if ( 1 == refCount ) {
        delete this;
        return 0;
    }
    return --refCount;
    }