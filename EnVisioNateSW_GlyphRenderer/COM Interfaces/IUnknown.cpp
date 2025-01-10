
#include "EnVisioNateSW_GlyphRenderer.h"

    HRESULT GlyphRenderer::QueryInterface(REFIID refIID,void **pvResult) {

    if ( ! pvResult )
        return E_POINTER;

    *pvResult = NULL;

    if ( IID_IUnknown == refIID ) 
        *pvResult = static_cast<IUnknown *>(this);

    else if ( IID_IGlyphRenderer == refIID )
        *pvResult = static_cast<IGlyphRenderer *>(this);

    else if ( IID_IGraphicElements == refIID )
        return pIGraphicElements -> QueryInterface(refIID,pvResult);

    else if ( IID_IGraphicParameters == refIID )
        return pIGraphicParameters -> QueryInterface(refIID,pvResult);

    if ( * pvResult ) {
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
    }


    ULONG GlyphRenderer::AddRef() {
    return ++refCount;
    }


    ULONG GlyphRenderer::Release() {
    if ( 1 == refCount ) {
        delete this;
        return 0;
    }
    return --refCount;
    }

