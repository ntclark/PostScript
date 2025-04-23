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

#include "Renderer.h"

#define ALLOC_CONNECTIONS  16

    Renderer::_IConnectionPoint::_IConnectionPoint(Renderer *pp) : 
        pParent(pp), 

        nextCookie(400),
        countLiveConnections(0),
        countConnections(ALLOC_CONNECTIONS)
    { 
    connections = new CONNECTDATA[countConnections];
    memset(connections, 0, countConnections * sizeof(CONNECTDATA));
    return;
    };


    Renderer::_IConnectionPoint::~_IConnectionPoint() {
    for ( int k = 0; k < countConnections; k++ ) 
        if ( connections[k].pUnk ) connections[k].pUnk -> Release();
    delete [] connections;
    return;
    }


    HRESULT Renderer::_IConnectionPoint::QueryInterface(REFIID riid,void **ppv) {
    if ( riid == IID_IConnectionPoint ) {
        *ppv = static_cast<IConnectionPoint *>(this);
        AddRef();
        return S_OK;
    }
    if ( riid == IID_IUnknown ) {
        *ppv = static_cast<IUnknown *>(this);
        AddRef();
        return S_OK;
    }
    return pParent -> QueryInterface(riid,ppv);
    }


    STDMETHODIMP_(ULONG) Renderer::_IConnectionPoint::AddRef() {
    return ++refCount;
    }

    STDMETHODIMP_(ULONG) Renderer::_IConnectionPoint::Release() {
    if ( 1 == refCount ) {
        delete this;
        return 0;
    }
    return --refCount;
    }


    STDMETHODIMP Renderer::_IConnectionPoint::GetConnectionInterface(IID *pIID) {
    if ( 0 == pIID ) 
        return E_POINTER;
    return S_OK;
    }


    STDMETHODIMP Renderer::_IConnectionPoint::GetConnectionPointContainer(IConnectionPointContainer **ppCPC) {
    return pParent -> QueryInterface(IID_IConnectionPointContainer,(void **)ppCPC);
    }


    STDMETHODIMP Renderer::_IConnectionPoint::Advise(IUnknown *pUnkSink,DWORD *pdwCookie) {

    IUnknown *pISink = NULL;

    HRESULT hr = pUnkSink -> QueryInterface(MY_EVENT_INTERFACE,(void **)&pISink);

    if ( E_NOINTERFACE == hr ) 
        return CONNECT_E_NOCONNECTION;

    if ( ! SUCCEEDED(hr) ) 
        return hr;

    if ( NULL == pISink ) 
        return CONNECT_E_CANNOTCONNECT;

    int freeSlot = getSlot();

    pISink -> AddRef();

    connections[freeSlot].pUnk = pISink;
    connections[freeSlot].dwCookie = nextCookie;

    *pdwCookie = nextCookie++;

    countLiveConnections++;

    return S_OK;
    }


    STDMETHODIMP Renderer::_IConnectionPoint::Unadvise(DWORD dwCookie) {

    if ( 0 == dwCookie ) 
        return E_INVALIDARG;

    int slot;

    slot = findSlot(dwCookie);

    if ( -1 == slot ) 
        return CONNECT_E_NOCONNECTION;

    if ( ! ( NULL == connections[slot].pUnk ) ) {
        connections[slot].pUnk -> Release();
        connections[slot].pUnk = NULL;
    }

    connections[slot].dwCookie = 0;

    countLiveConnections--;

    return S_OK;
    }


    STDMETHODIMP Renderer::_IConnectionPoint::EnumConnections(IEnumConnections **ppEnum) {

    *ppEnum = NULL;

    if ( 0 == countLiveConnections )
        return OLE_E_NOCONNECTION;

    CONNECTDATA *pTempConnections = new CONNECTDATA[countLiveConnections];
    memset(pTempConnections,0,countLiveConnections * sizeof(CONNECTDATA));

    int j = 0;
    for ( int k = 0, j = 0; k < countConnections && j < countLiveConnections; k++) {
        if ( ! ( 0 == connections[k].dwCookie ) ) {
            pTempConnections[j].pUnk = (IUnknown *)connections[k].pUnk;
            pTempConnections[j].dwCookie = connections[k].dwCookie;
            j++;
        }
    }

    Renderer::_IEnumerateConnections *p = new Renderer::_IEnumerateConnections(this,countLiveConnections,pTempConnections,0);

    p -> QueryInterface(IID_IEnumConnections,(void **)ppEnum);

    delete [] pTempConnections;

    return S_OK;
    }


    int Renderer::_IConnectionPoint::getSlot() {
    CONNECTDATA* moreConnections;
    int i;
    i = findSlot(0);
    if ( i > -1 ) return i;
    moreConnections = new CONNECTDATA[countConnections + ALLOC_CONNECTIONS];
    memset( moreConnections, 0, sizeof(CONNECTDATA) * (countConnections + ALLOC_CONNECTIONS));
    memcpy( moreConnections, connections, sizeof(CONNECTDATA) * countConnections);
    delete [] connections;
    connections = moreConnections;
    countConnections += ALLOC_CONNECTIONS;
    return countConnections - ALLOC_CONNECTIONS;
    }


    int Renderer::_IConnectionPoint::findSlot(DWORD dwCookie) {
    for ( int k = 0; k < countConnections; k++ )
        if ( dwCookie == connections[k].dwCookie ) 
            return k;
    return -1;
    }