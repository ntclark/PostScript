// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PostScript.h"

  PStoPDF::_IEnumerateConnections::_IEnumerateConnections(
            IUnknown* pHostObj,
            ULONG cConnections,
            CONNECTDATA* paConnections,
            ULONG initialIndex) :
    refCount(0), 
    pParentUnknown(pHostObj),
    enumeratorIndex(initialIndex),
    countConnections(cConnections)
  {

  connections = new CONNECTDATA[countConnections];

  for ( UINT i = 0; i < countConnections; i++ ) {
     connections[i] = paConnections[i];
  }

  return;
  }



  PStoPDF::_IEnumerateConnections::~_IEnumerateConnections() {
  delete [] connections;
  return;
  }


  STDMETHODIMP PStoPDF::_IEnumerateConnections::QueryInterface(REFIID riid,void **ppv) {
  *ppv = NULL;
  if ( IID_IUnknown != riid && IID_IEnumConnections != riid) return E_NOINTERFACE;
  *ppv = (LPVOID)this;
  AddRef();
  return S_OK;
  }


  STDMETHODIMP_(ULONG) PStoPDF::_IEnumerateConnections::AddRef() {
  pParentUnknown -> AddRef();
  return ++refCount;
  }



  STDMETHODIMP_(ULONG) PStoPDF::_IEnumerateConnections::Release() {
  pParentUnknown -> Release();
  if ( 0 == --refCount ) {
    refCount++;
    delete this;
    return 0;
  }
  return refCount;
  }



  STDMETHODIMP PStoPDF::_IEnumerateConnections::Next(ULONG cReq,CONNECTDATA* paConnections,ULONG* pcEnumerated) {

  ULONG cRet;

  if ( NULL == paConnections ) return E_POINTER;

  for ( cRet = 0; enumeratorIndex < countConnections && cReq > 0; paConnections++, enumeratorIndex++, cRet++, cReq-- ) {

    *paConnections = connections[enumeratorIndex];

//    if ( NULL != paConnections -> pUnk ) paConnections -> pUnk -> AddRef();

  }

  if ( NULL != pcEnumerated )
    *pcEnumerated = cRet;

  return 0 == cRet ? S_FALSE : S_OK;
  }


  STDMETHODIMP PStoPDF::_IEnumerateConnections::Skip(ULONG cSkip) {
  if ( (enumeratorIndex + cSkip) < countConnections ) return S_FALSE;
  enumeratorIndex += cSkip;
  return S_OK;
  }



  STDMETHODIMP PStoPDF::_IEnumerateConnections::Reset() {
  enumeratorIndex = 0;
  return S_OK;
  }



  STDMETHODIMP PStoPDF::_IEnumerateConnections::Clone(IEnumConnections** ppIEnum) {
  _IEnumerateConnections* p = new _IEnumerateConnections(pParentUnknown,countConnections,connections,enumeratorIndex);
  return p -> QueryInterface(IID_IEnumConnections,(void **)ppIEnum);
  }
