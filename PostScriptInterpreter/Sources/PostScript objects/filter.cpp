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

#include "PostScript objects/filter.h"

#include "ascii85.h"

    int32_t runLengthDecode(uint8_t *pbInput,int32_t cbInput,uint8_t **ppbOutput);

    filter::filter(job *pJob,char *pszFilterName,object *pSource,char *pszEOD) :
        file(pJob,pszFilterName),
        pDataSource(pSource)
    {
    theObjectType = object::objectType::filter;
    memset(szSourceEndDelimiter,0,sizeof(szSourceEndDelimiter));
    if ( ! ( NULL == pszEOD ) )
        strcpy(szSourceEndDelimiter,pszEOD);
    strcpy(szFilterName,pszFilterName);
    return;
    }

    filter::~filter() {}

    uint8_t *filter::getBinaryData(DWORD *pcbSize,char *pszEndDelimiter) {

    if ( NULL == pDataSource )
        return NULL;

    uint8_t *pbSource = NULL;

    *pcbSize = 0L;

    if ( object::objectType::file == pDataSource -> ObjectType() ) 
        pbSource = reinterpret_cast<file *>(pDataSource) -> getBinaryData(pcbSize,szSourceEndDelimiter);

    else if ( object::objectType::filter == pDataSource -> ObjectType() )
        pbSource = reinterpret_cast<filter *>(pDataSource) -> getBinaryData(pcbSize,szSourceEndDelimiter);

    else 
        return NULL;

    if ( 0 == _stricmp(szFilterName,"ASCII85Decode") ) {
        pbData = NULL;
        *pcbSize = decodeASCII85(pbSource,(int32_t)*pcbSize,&pbData);
        reinterpret_cast<file *>(pDataSource) -> releaseData();
        if ( 0 == *pcbSize ) {
            sprintf_s(PostScriptInterpreter::szErrorMessage,1024,"There was an invalid block of data for ASCII85Decode");
            pPostScriptInterpreter -> pIConnectionPointContainer -> fire_ErrorNotification(PostScriptInterpreter::szErrorMessage);
        }
        return pbData;
    }

    if ( 0 == _stricmp(szFilterName,"DCTDecode") ) {
        return pbSource;
    }

    if ( 0 == _stricmp(szFilterName,"RunLengthDecode") ) {
        pbData = NULL;
        *pcbSize = runLengthDecode(pbSource,(int32_t)*pcbSize, &pbData);
        reinterpret_cast<file *>(pDataSource) -> releaseData();
        return pbData;
    }

    return NULL;
    }
