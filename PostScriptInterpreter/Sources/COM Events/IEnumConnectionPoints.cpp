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

    PostScriptInterpreter::_IEnumConnectionPoints::_IEnumConnectionPoints(PostScriptInterpreter *pp,_IConnectionPoint **cp,int connectionPointCount) : 
   	    cpCount(connectionPointCount),
        pParent(pp) {

    //connectionPoints = new _IConnectionPoint *[cpCount];

    //for ( int k = 0; k < cpCount; k++ ) {
    //    connectionPoints[k] = cp[k];
    //    connectionPoints[k] -> AddRef();
    //}

    Reset();

    return;
    };


    PostScriptInterpreter::_IEnumConnectionPoints::~_IEnumConnectionPoints() { 
    for ( int k = 0; k < cpCount; k++ )
        connectionPoints[k] -> Release();
    delete [] connectionPoints;
    return;
    };


    HRESULT PostScriptInterpreter::_IEnumConnectionPoints::QueryInterface(REFIID riid,void **ppv) {
    return pParent -> QueryInterface(riid,ppv);
    }
    unsigned long __stdcall PostScriptInterpreter::_IEnumConnectionPoints::AddRef() {
    return pParent -> AddRef();
    }
    unsigned long __stdcall PostScriptInterpreter::_IEnumConnectionPoints::Release() {
    return pParent -> Release();
    }


    HRESULT PostScriptInterpreter::_IEnumConnectionPoints::Next(ULONG countRequested,IConnectionPoint **rgpcn,ULONG *pcFetched) {

    ULONG found;

    if ( ! rgpcn ) 
        return E_POINTER;

    if ( *rgpcn && enumeratorIndex < cpCount ) {
        if ( pcFetched ) 
            *pcFetched = 0L;
        else
            if ( countRequested != 1L ) 
                return E_POINTER;
    } else 
        return S_FALSE;

    for ( found = 0; enumeratorIndex < cpCount && countRequested > 0; enumeratorIndex++, rgpcn++, found++, countRequested-- ) {
        *rgpcn = connectionPoints[enumeratorIndex];
        if ( *rgpcn ) 
            (*rgpcn) -> AddRef();
    }

    if ( pcFetched ) 
        *pcFetched = found;

    return S_OK;
    }


    HRESULT PostScriptInterpreter::_IEnumConnectionPoints::Skip(ULONG cSkip) {
    if ( cpCount < 1 || ((enumeratorIndex + (int)cSkip) < cpCount) ) 
        return S_FALSE;
    enumeratorIndex += cSkip;
    return S_OK;
    }


    HRESULT PostScriptInterpreter::_IEnumConnectionPoints::Reset() {
    enumeratorIndex = 0;
    return S_OK;
    }


    HRESULT PostScriptInterpreter::_IEnumConnectionPoints::Clone(IEnumConnectionPoints **ppEnum) {
    *ppEnum = NULL;
    _IEnumConnectionPoints*  p = new _IEnumConnectionPoints(pParent,connectionPoints,cpCount);
    p -> enumeratorIndex = enumeratorIndex;
    p -> QueryInterface(IID_IEnumConnectionPoints,(void **)ppEnum);
    return S_OK;
    }

