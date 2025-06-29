/*
Copyright 2025 EnVisioNate LLC

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the �Software�), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is the MIT License
*/

#include "PostScriptInterpreter.h"

#define ALLOC_CONNECTIONS  16

    PostScriptInterpreter::_IConnectionPoint::_IConnectionPoint(PostScriptInterpreter *pp,REFIID eventInterfaceId) : 
        pParent(pp), 
        myEventInterface(eventInterfaceId),
        adviseSink(0),
        nextCookie(400),
        countLiveConnections(0),
        countConnections(ALLOC_CONNECTIONS)
    { 
    connections = new CONNECTDATA[countConnections];
    memset(connections, 0, countConnections * sizeof(CONNECTDATA));
    return;
    };


    PostScriptInterpreter::_IConnectionPoint::~_IConnectionPoint() {
    for ( int k = 0; k < countConnections; k++ ) 
        if ( connections[k].pUnk ) connections[k].pUnk -> Release();
    delete [] connections;
    return;
    }


    HRESULT PostScriptInterpreter::_IConnectionPoint::QueryInterface(REFIID riid,void **ppv) {
    if ( riid == IID_IConnectionPoint ) {
        *ppv = static_cast<IConnectionPoint *>(this);
        AddRef();
        return S_OK;
    }
    return pParent -> QueryInterface(riid,ppv);
    }


    STDMETHODIMP_(ULONG) PostScriptInterpreter::_IConnectionPoint::AddRef() {
    return pParent -> AddRef();
    }

    STDMETHODIMP_(ULONG) PostScriptInterpreter::_IConnectionPoint::Release() {
    return pParent -> Release();
    }


    STDMETHODIMP PostScriptInterpreter::_IConnectionPoint::GetConnectionInterface(IID *pIID) {
    if ( pIID == 0 ) return E_POINTER;
    return S_OK;
    }


    STDMETHODIMP PostScriptInterpreter::_IConnectionPoint::GetConnectionPointContainer(IConnectionPointContainer **ppCPC) {
    return pParent -> QueryInterface(IID_IConnectionPointContainer,(void **)ppCPC);
    }


    STDMETHODIMP PostScriptInterpreter::_IConnectionPoint::Advise(IUnknown *pUnkSink,DWORD *pdwCookie) {

    HRESULT hr;
    IUnknown* pISink = 0;

    hr = pUnkSink -> QueryInterface(IID_IPostScriptInterpreterEvents,(void **)&pISink);

    if ( hr == E_NOINTERFACE ) 
        return CONNECT_E_NOCONNECTION;

    if ( ! SUCCEEDED(hr) ) 
        return hr;

    if ( ! pISink ) 
        return CONNECT_E_CANNOTCONNECT;

    int freeSlot;
    *pdwCookie = 0;

    freeSlot = getSlot();

    pISink -> AddRef();

    connections[freeSlot].pUnk = pISink;
    connections[freeSlot].dwCookie = nextCookie;

    *pdwCookie = nextCookie++;

    countLiveConnections++;

    return S_OK;
    }


    STDMETHODIMP PostScriptInterpreter::_IConnectionPoint::Unadvise(DWORD dwCookie) {

    if ( 0 == dwCookie ) return E_INVALIDARG;

    int slot;

    slot = findSlot(dwCookie);

    if ( slot == -1 ) 
        return CONNECT_E_NOCONNECTION;

    if ( connections[slot].pUnk ) 
        connections[slot].pUnk -> Release();

    connections[slot].dwCookie = 0;

    countLiveConnections--;

    return S_OK;
    }

    STDMETHODIMP PostScriptInterpreter::_IConnectionPoint::EnumConnections(IEnumConnections **ppEnum) {
    CONNECTDATA *tempConnections;
    int i,j;

    *ppEnum = NULL;

    if ( countLiveConnections == 0 ) return OLE_E_NOCONNECTION;

    tempConnections = new CONNECTDATA[countLiveConnections];

    for ( i = 0, j = 0; i < countConnections && j < countLiveConnections; i++) {
        if ( 0 != connections[i].dwCookie ) {
            tempConnections[j].pUnk = (IUnknown *)connections[i].pUnk;
            tempConnections[j].dwCookie = connections[i].dwCookie;
            j++;
        }
    }

    PostScriptInterpreter::_IEnumerateConnections *p = new PostScriptInterpreter::_IEnumerateConnections(this,countLiveConnections,tempConnections,0);

    p -> QueryInterface(IID_IEnumConnections,(void **)ppEnum);

    delete [] tempConnections;

    return S_OK;
    }


    int PostScriptInterpreter::_IConnectionPoint::getSlot() {
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


    int PostScriptInterpreter::_IConnectionPoint::findSlot(DWORD dwCookie) {
    for ( int i = 0; i < countConnections; i++ )
        if ( dwCookie == connections[i].dwCookie ) return i;
    return -1;
    }