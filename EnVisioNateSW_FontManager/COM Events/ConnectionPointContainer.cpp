
#include "FontManager.h"

    HRESULT FontManager::_IConnectionPointContainer::QueryInterface(REFIID riid,void **ppv) {

    if ( IID_IConnectionPointContainer == riid ) {
        *ppv = static_cast<IConnectionPointContainer *>(this);
        pParent -> AddRef();
        return S_OK;
    }
    return pParent -> QueryInterface(riid,ppv);
    }

    STDMETHODIMP_(ULONG) FontManager::_IConnectionPointContainer::AddRef() {
    return pParent -> AddRef();
    }
    STDMETHODIMP_(ULONG) FontManager::_IConnectionPointContainer::Release() {
    return pParent -> Release();
    }


    STDMETHODIMP FontManager::_IConnectionPointContainer::EnumConnectionPoints(IEnumConnectionPoints **ppEnum) {

    _IConnectionPoint *pConnectionPoints[1];

    *ppEnum = NULL;

    if ( pParent -> pIEnumConnectionPoints ) 
        delete pParent -> pIEnumConnectionPoints;

    pConnectionPoints[0] = pParent -> pIConnectionPoint;
    pParent -> pIEnumConnectionPoints = new _IEnumConnectionPoints(pParent,pConnectionPoints,1);

    return pParent -> pIEnumConnectionPoints -> QueryInterface(IID_IEnumConnectionPoints,(void **)ppEnum);
    }


    STDMETHODIMP FontManager::_IConnectionPointContainer::FindConnectionPoint(REFIID riid,IConnectionPoint **ppCP) {
    *ppCP = NULL;
    if ( IID_IFontManagerNotifications == riid )
        return pParent -> pIConnectionPoint -> QueryInterface(IID_IConnectionPoint,(void **)ppCP);
    return CONNECT_E_NOCONNECTION;
    }


    void FontManager::_IConnectionPointContainer::fire_ErrorNotification(char *pszError) {
    IEnumConnections* pIEnum;
    CONNECTDATA connectData;
    pParent -> pIConnectionPoint -> EnumConnections(&pIEnum);
    if ( ! pIEnum ) return;
    while ( 1 ) {
        if ( pIEnum -> Next(1, &connectData, NULL) ) 
            break;
        IFontManagerNotifications * pClient = reinterpret_cast<IFontManagerNotifications *>(connectData.pUnk);
        pClient -> ErrorNotification((UINT_PTR)pszError);
    }
    static_cast<IUnknown*>(pIEnum) -> Release();
    return;
    }