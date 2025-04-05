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

#include "PostScriptInterpreter.h"

    PostScriptInterpreter::_IEnumerateConnections::_IEnumerateConnections(IUnknown* pHostObj,ULONG cConnections,CONNECTDATA* paConnections,ULONG initialIndex) :
        refCount(0), 
        pParentUnknown(pHostObj),
        enumeratorIndex(initialIndex),
        countConnections(cConnections)
    {

    connections = new CONNECTDATA[countConnections];

    for ( UINT k = 0; k < countConnections; k++ ) 
        connections[k] = paConnections[k];

    return;
    }


    PostScriptInterpreter::_IEnumerateConnections::~_IEnumerateConnections() {
    delete [] connections;
    return;
    }


    STDMETHODIMP PostScriptInterpreter::_IEnumerateConnections::QueryInterface(REFIID riid,void **ppv) {
    *ppv = NULL;
    if ( IID_IUnknown != riid && IID_IEnumConnections != riid) 
        return E_NOINTERFACE;
    *ppv = (LPVOID)this;
    AddRef();
    return S_OK;
    }


    STDMETHODIMP_(ULONG) PostScriptInterpreter::_IEnumerateConnections::AddRef() {
    pParentUnknown -> AddRef();
    return ++refCount;
    }



    STDMETHODIMP_(ULONG) PostScriptInterpreter::_IEnumerateConnections::Release() {
    pParentUnknown -> Release();
    if ( 0 == --refCount ) {
        refCount++;
        delete this;
        return 0;
    }
    return refCount;
    }



    STDMETHODIMP PostScriptInterpreter::_IEnumerateConnections::Next(ULONG cReq,CONNECTDATA* paConnections,ULONG* pcEnumerated) {

    ULONG cRet;

    if ( NULL == paConnections ) 
        return E_POINTER;

    for ( cRet = 0; enumeratorIndex < countConnections && cReq > 0; paConnections++, enumeratorIndex++, cRet++, cReq-- ) 
        *paConnections = connections[enumeratorIndex];

    if ( NULL != pcEnumerated )
        *pcEnumerated = cRet;

    return 0 == cRet ? S_FALSE : S_OK;
    }


    STDMETHODIMP PostScriptInterpreter::_IEnumerateConnections::Skip(ULONG cSkip) {
    if ( (enumeratorIndex + cSkip) < countConnections ) 
        return S_FALSE;
    enumeratorIndex += cSkip;
    return S_OK;
    }



    STDMETHODIMP PostScriptInterpreter::_IEnumerateConnections::Reset() {
    enumeratorIndex = 0;
    return S_OK;
    }



    STDMETHODIMP PostScriptInterpreter::_IEnumerateConnections::Clone(IEnumConnections** ppIEnum) {
    _IEnumerateConnections* p = new _IEnumerateConnections(pParentUnknown,countConnections,connections,enumeratorIndex);
    return p -> QueryInterface(IID_IEnumConnections,(void **)ppIEnum);
    }
