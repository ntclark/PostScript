
#include "PostScript.h"

    HRESULT PStoPDF::_IConnectionPointContainer::QueryInterface(REFIID riid,void **ppv) {

    if ( IID_IConnectionPointContainer == riid ) {
        *ppv = static_cast<IConnectionPointContainer *>(this);
        pParent -> AddRef();
        return S_OK;
    }
    return pParent -> QueryInterface(riid,ppv);
    }

    STDMETHODIMP_(ULONG) PStoPDF::_IConnectionPointContainer::AddRef() {
    return pParent -> AddRef();
    }
    STDMETHODIMP_(ULONG) PStoPDF::_IConnectionPointContainer::Release() {
    return pParent -> Release();
    }


    STDMETHODIMP PStoPDF::_IConnectionPointContainer::EnumConnectionPoints(IEnumConnectionPoints **ppEnum) {

    _IConnectionPoint *pConnectionPoints[1];

    *ppEnum = NULL;

    if ( pParent -> pIEnumConnectionPoints ) 
        delete pParent -> pIEnumConnectionPoints;

    pConnectionPoints[0] = pParent -> pIConnectionPoint;
    pParent -> pIEnumConnectionPoints = new _IEnumConnectionPoints(pParent,pConnectionPoints,1);

    return pParent -> pIEnumConnectionPoints -> QueryInterface(IID_IEnumConnectionPoints,(void **)ppEnum);
    }


    STDMETHODIMP PStoPDF::_IConnectionPointContainer::FindConnectionPoint(REFIID riid,IConnectionPoint **ppCP) {
    *ppCP = NULL;
    if ( IID_IPostScriptEvents == riid )
        return pParent -> pIConnectionPoint -> QueryInterface(IID_IConnectionPoint,(void **)ppCP);
    return CONNECT_E_NOCONNECTION;
    }


    void PStoPDF::_IConnectionPointContainer::fire_RenderChar(POINT *pPoint,char theChar) {
    IEnumConnections* pIEnum;
    CONNECTDATA connectData;
    pParent -> pIConnectionPoint -> EnumConnections(&pIEnum);
    if ( ! pIEnum ) return;
    while ( 1 ) {
        if ( pIEnum -> Next(1, &connectData, NULL) ) break;
        IPostScriptEvents * pClient = reinterpret_cast<IPostScriptEvents *>(connectData.pUnk);
        pClient -> RenderChar(pPoint,theChar);
    }
    static_cast<IUnknown*>(pIEnum) -> Release();
    return;
    }


    void PStoPDF::_IConnectionPointContainer::fire_RenderString(POINT *pPoint,char *pszString) {
    IEnumConnections* pIEnum;
    CONNECTDATA connectData;
    pParent -> pIConnectionPoint -> EnumConnections(&pIEnum);
    if ( ! pIEnum ) return;
    while ( 1 ) {
        if ( pIEnum -> Next(1, &connectData, NULL) ) break;
        IPostScriptEvents * pClient = reinterpret_cast<IPostScriptEvents *>(connectData.pUnk);
        pClient -> RenderString(pPoint,pszString);
    }
    static_cast<IUnknown*>(pIEnum) -> Release();
    return;
    }
