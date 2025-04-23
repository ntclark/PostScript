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

    HRESULT PostScriptInterpreter::_IDropTarget::QueryInterface(REFIID riid,void **ppv) {

    if ( IID_IDropTarget == riid )
        *ppv = static_cast<IDropTarget *>(this); 
    else
        if ( IID_IUnknown == riid )
            *ppv = static_cast<IUnknown *>(this);
        else
            return pParent -> QueryInterface(riid,ppv);

    AddRef();
  
    return S_OK;
    }


    HRESULT PostScriptInterpreter::_IDropTarget::DragEnter(IDataObject *pIDataObject,DWORD,POINTL,DWORD *pdwEffect) {
    IEnumFORMATETC *pIEnumFORMATETC = NULL;
    pIDataObject -> EnumFormatEtc(DATADIR_GET,&pIEnumFORMATETC);
    FORMATETC format;
    while ( S_OK == pIEnumFORMATETC -> Next(1,&format,NULL) ) {
        if ( CF_HDROP == format.cfFormat ) {
            *pdwEffect = DROPEFFECT_COPY;
            pIEnumFORMATETC -> Release();
            return S_OK;
        }
    }
    pIEnumFORMATETC -> Release();
    *pdwEffect = DROPEFFECT_NONE;
    return S_OK;
    }


    HRESULT PostScriptInterpreter::_IDropTarget::DragLeave() {
    return S_OK;
    }


    HRESULT PostScriptInterpreter::_IDropTarget::DragOver(DWORD,POINTL,DWORD  *) {
    return S_OK;
    }


    HRESULT PostScriptInterpreter::_IDropTarget::Drop(IDataObject *pIDataObject,DWORD grfKeyState,POINTL pt,DWORD *pdwEffect) {

    FORMATETC format{0};
    STGMEDIUM stgMedium{0};

    format.cfFormat = CF_HDROP;
    format.dwAspect = DVASPECT_CONTENT;
    format.lindex = -1;
    format.tymed = TYMED_HGLOBAL;

    stgMedium.tymed = TYMED_HGLOBAL;

    pIDataObject -> GetData(&format,&stgMedium);

    HDROP handleDrop = (HDROP)stgMedium.hGlobal;

    long cb = DragQueryFile(handleDrop,0,NULL,0) + 1;

    char *pszFileName = new char[cb + 1];

    pszFileName[cb] = '\0';

    DragQueryFile(handleDrop,0,pszFileName,cb);

    pParent -> Parse(pszFileName,FALSE);

    delete [] pszFileName;

    return S_OK;
    }
