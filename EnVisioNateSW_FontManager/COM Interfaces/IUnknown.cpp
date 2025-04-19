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

#include "EnVisioNateSW_FontManager.h"

    HRESULT FontManager::QueryInterface(REFIID refIID,void **pvResult) {

    if ( ! pvResult )
        return E_POINTER;

    *pvResult = NULL;

    if ( IID_IUnknown == refIID ) 
        *pvResult = static_cast<IUnknown *>(this);

    else if ( IID_IFontManager == refIID )
        *pvResult = static_cast<IFontManager *>(this);

    else if ( IID_IRenderer == refIID )
        return pIRenderer -> QueryInterface(refIID,pvResult);

    else if ( IID_IConnectionPointContainer == refIID ) 
        return pIConnectionPointContainer -> QueryInterface(refIID,pvResult);

    else if ( IID_IConnectionPoint == refIID ) 
        return pIConnectionPoint -> QueryInterface(refIID,pvResult);

    if ( ! ( NULL == *pvResult ) ) {
        AddRef();
        return S_OK;
    }

    HRESULT rc = pIRenderer -> QueryInterface(refIID,pvResult);

    if ( S_OK == rc )
        return S_OK;

    return E_NOINTERFACE;
    }


    ULONG FontManager::AddRef() {
    return ++refCount;
    }


    ULONG FontManager::Release() {
    if ( 1 == refCount ) {
        delete this;
        return 0;
    }
    return --refCount;
    }