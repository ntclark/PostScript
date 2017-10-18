// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
   return CONNECT_E_NOCONNECTION;
   }


#if 0
   void PStoPDF::_IConnectionPointContainer::fire_DataEvent(long status) {
   IEnumConnections* pIEnum;
   CONNECTDATA connectData;
   pParent -> connectionPoint.EnumConnections(&pIEnum);
   if ( ! pIEnum ) return;
   while ( 1 ) {
      if ( pIEnum -> Next(1, &connectData, NULL) ) break;
      _IOPOSSigCapEvents * pClient = reinterpret_cast<_IOPOSSigCapEvents *>(connectData.pUnk);
      pClient -> DataEvent(status);
   }
   static_cast<IUnknown*>(pIEnum) -> Release();
   return;
   }
#endif
