/*
Copyright 2025 EnVisioNate LLC

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the “Software”), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is the MIT License
*/

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


    void PStoPDF::_IConnectionPointContainer::fire_ErrorNotification(char *pszString) {
    IEnumConnections* pIEnum;
    CONNECTDATA connectData;
    pParent -> pIConnectionPoint -> EnumConnections(&pIEnum);
    if ( ! pIEnum ) return;
    while ( 1 ) {
        if ( pIEnum -> Next(1, &connectData, NULL) ) break;
        IPostScriptEvents * pClient = reinterpret_cast<IPostScriptEvents *>(connectData.pUnk);
        pClient -> ErrorNotification((UINT_PTR)pszString);
    }
    static_cast<IUnknown*>(pIEnum) -> Release();
    return;
    }