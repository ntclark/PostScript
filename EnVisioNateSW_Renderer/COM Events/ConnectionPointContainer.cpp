
#include "Renderer.h"

    HRESULT Renderer::_IConnectionPointContainer::QueryInterface(REFIID riid,void **ppv) {

    if ( IID_IConnectionPointContainer == riid ) {
        *ppv = static_cast<IConnectionPointContainer *>(this);
        pParent -> AddRef();
        return S_OK;
    }
    return pParent -> QueryInterface(riid,ppv);
    }

    STDMETHODIMP_(ULONG) Renderer::_IConnectionPointContainer::AddRef() {
    return pParent -> AddRef();
    }
    STDMETHODIMP_(ULONG) Renderer::_IConnectionPointContainer::Release() {
    return pParent -> Release();
    }


    STDMETHODIMP Renderer::_IConnectionPointContainer::EnumConnectionPoints(IEnumConnectionPoints **ppEnum) {

    _IConnectionPoint *pConnectionPoints[1];

    *ppEnum = NULL;

    if ( pParent -> pIEnumConnectionPoints ) 
        delete pParent -> pIEnumConnectionPoints;

    pConnectionPoints[0] = pParent -> pIConnectionPoint;
    pParent -> pIEnumConnectionPoints = new _IEnumConnectionPoints(pParent,pConnectionPoints,1);

    return pParent -> pIEnumConnectionPoints -> QueryInterface(IID_IEnumConnectionPoints,(void **)ppEnum);
    }


    STDMETHODIMP Renderer::_IConnectionPointContainer::FindConnectionPoint(REFIID riid,IConnectionPoint **ppCP) {
    *ppCP = NULL;
    if ( MY_EVENT_INTERFACE == riid )
        return pParent -> pIConnectionPoint -> QueryInterface(IID_IConnectionPoint,(void **)ppCP);
    return CONNECT_E_NOCONNECTION;
    }


    void Renderer::_IConnectionPointContainer::fire_ErrorNotification(char *pszError) {
    IEnumConnections* pIEnum;
    CONNECTDATA connectData;
    pParent -> pIConnectionPoint -> EnumConnections(&pIEnum);
    if ( ! pIEnum ) return;
    while ( 1 ) {
        if ( pIEnum -> Next(1, &connectData, NULL) ) 
            break;
        MY_EVENT_CLASS * pClient = reinterpret_cast<MY_EVENT_CLASS *>(connectData.pUnk);
        pClient -> ErrorNotification((UINT_PTR)pszError);
    }
    static_cast<IUnknown*>(pIEnum) -> Release();
    return;
    }


    void Renderer::_IConnectionPointContainer::fire_StatusNotification(char *pszError) {
    IEnumConnections* pIEnum;
    CONNECTDATA connectData;
    pParent -> pIConnectionPoint -> EnumConnections(&pIEnum);
    if ( ! pIEnum ) return;
    while ( 1 ) {
        if ( pIEnum -> Next(1, &connectData, NULL) ) 
            break;
        MY_EVENT_CLASS * pClient = reinterpret_cast<MY_EVENT_CLASS *>(connectData.pUnk);
        pClient -> StatusNotification((UINT_PTR)pszError);
    }
    static_cast<IUnknown*>(pIEnum) -> Release();
    return;
    }


    void Renderer::_IConnectionPointContainer::fire_Clear() {
    IEnumConnections* pIEnum;
    CONNECTDATA connectData;
    pParent -> pIConnectionPoint -> EnumConnections(&pIEnum);
    if ( ! pIEnum ) return;
    while ( 1 ) {
        if ( pIEnum -> Next(1, &connectData, NULL) ) 
            break;
        MY_EVENT_CLASS * pClient = reinterpret_cast<MY_EVENT_CLASS *>(connectData.pUnk);
        pClient -> Clear();
    }
    static_cast<IUnknown*>(pIEnum) -> Release();
    return;
    }