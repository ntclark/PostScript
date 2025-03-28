
#include "Renderer.h"

    HRESULT Renderer::QueryInterface(REFIID refIID,void **pvResult) {

    if ( ! pvResult )
        return E_POINTER;

    *pvResult = NULL;

    if ( IID_IUnknown == refIID ) 
        *pvResult = static_cast<IUnknown *>(this);

    else if ( IID_IRenderer == refIID )
        *pvResult = static_cast<IRenderer *>(this);

    else if ( IID_IGraphicElements == refIID )
        return pIGraphicElements -> QueryInterface(refIID,pvResult);

    else if ( IID_IGraphicParameters == refIID )
        return pIGraphicParameters -> QueryInterface(refIID,pvResult);

    else if ( IID_IConnectionPointContainer == refIID ) 
      return pIConnectionPointContainer -> QueryInterface(refIID,pvResult);

    else if ( IID_IConnectionPoint == refIID ) 
        return pIConnectionPoint -> QueryInterface(refIID,pvResult);

    if ( * pvResult ) {
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
    }


    ULONG Renderer::AddRef() {
    return ++refCount;
    }


    ULONG Renderer::Release() {
    if ( 1 == refCount ) {
        delete this;
        return 0;
    }
    return --refCount;
    }

