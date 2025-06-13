// Copyright 2017 EnVisioNate LLC. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PostScript.h"


  PStoPDF::_IEnumConnectionPoints::_IEnumConnectionPoints(PStoPDF *pp,_IConnectionPoint **cp,int connectionPointCount) : 
   	  cpCount(connectionPointCount),
        pParent(pp) {
 
  connectionPoints = new _IConnectionPoint *[cpCount];
  for ( int k = 0; k < cpCount; k++ ) {
     connectionPoints[k] = cp[k];
     connectionPoints[k] -> AddRef();
  }

  Reset();

  return;
  };


  PStoPDF::_IEnumConnectionPoints::~_IEnumConnectionPoints() { 
  for ( int k = 0; k < cpCount; k++ )
     connectionPoints[k] -> Release();
  delete [] connectionPoints;
  return;
  };


  HRESULT PStoPDF::_IEnumConnectionPoints::QueryInterface(REFIID riid,void **ppv) {
  return pParent -> QueryInterface(riid,ppv);
  }
  unsigned long __stdcall PStoPDF::_IEnumConnectionPoints::AddRef() {
  return pParent -> AddRef();
  }
  unsigned long __stdcall PStoPDF::_IEnumConnectionPoints::Release() {
  return pParent -> Release();
  }


  HRESULT PStoPDF::_IEnumConnectionPoints::Next(ULONG countRequested,IConnectionPoint **rgpcn,ULONG *pcFetched) {
  ULONG found;

  if ( !rgpcn ) return E_POINTER;

  if ( *rgpcn && enumeratorIndex < cpCount ) {
     if ( pcFetched ) 
       *pcFetched = 0L;
     else
       if ( countRequested != 1L ) return E_POINTER;
  }
  else return S_FALSE;

  for ( found = 0; enumeratorIndex < cpCount && countRequested > 0; enumeratorIndex++, rgpcn++, found++, countRequested-- ) {
    *rgpcn = connectionPoints[enumeratorIndex];
    if ( *rgpcn ) (*rgpcn) -> AddRef();
  }

  if ( pcFetched ) *pcFetched = found;

  return S_OK;
  }


  HRESULT PStoPDF::_IEnumConnectionPoints::Skip(ULONG cSkip) {
  if ( cpCount < 1 || ((enumeratorIndex + (int)cSkip) < cpCount) ) return S_FALSE;
  enumeratorIndex += cSkip;
  return S_OK;
  }


  HRESULT PStoPDF::_IEnumConnectionPoints::Reset() {
  enumeratorIndex = 0;
  return S_OK;
  }

  HRESULT PStoPDF::_IEnumConnectionPoints::Clone(IEnumConnectionPoints **ppEnum) {
  _IEnumConnectionPoints* p;

  *ppEnum = NULL;

  p = new _IEnumConnectionPoints(pParent,connectionPoints,cpCount);
  p -> enumeratorIndex = enumeratorIndex;
  p -> QueryInterface(IID_IEnumConnectionPoints,(void **)ppEnum);

  return S_OK;
  }

