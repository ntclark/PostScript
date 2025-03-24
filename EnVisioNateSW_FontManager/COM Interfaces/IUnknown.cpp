
#include "EnVisioNateSW_FontManager.h"

    HRESULT FontManager::QueryInterface(REFIID refIID,void **pvResult) {

    if ( ! pvResult )
        return E_POINTER;

    *pvResult = NULL;

    if ( IID_IUnknown == refIID ) 
        *pvResult = static_cast<IUnknown *>(this);

    else if ( IID_IFontManager == refIID )
        *pvResult = static_cast<IFontManager *>(this);

    else if ( IID_IRenderer == refIID )
        return pIRenderer -> QueryInterface(refIID,pvResult);

    else if ( IID_IConnectionPointContainer == refIID ) 
      return pIConnectionPointContainer -> QueryInterface(refIID,pvResult);

    else if ( IID_IConnectionPoint == refIID ) 
        return pIConnectionPoint -> QueryInterface(refIID,pvResult);

    if ( ! ( NULL == *pvResult ) ) {
        AddRef();
        return S_OK;
    }

    HRESULT rc = pIRenderer -> QueryInterface(refIID,pvResult);

    if ( S_OK == rc )
        return S_OK;

    return E_NOINTERFACE;
    }


    ULONG FontManager::AddRef() {
    return ++refCount;
    }


    ULONG FontManager::Release() {
    if ( 1 == refCount ) {
        //if ( ! ( NULL == pIGraphicElements_Type3 ) )
        //    delete pIGraphicElements_Type3;
        //if ( ! ( NULL == pTextRenderer ) )
        //    delete pTextRenderer;
        delete this;
        return 0;
    }
    return --refCount;
    }