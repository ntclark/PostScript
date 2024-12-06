
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
        pbData = new BYTE[*pcbSize * 4];
        *pcbSize = decode_ascii85(pbSource,(int32_t)*pcbSize, pbData, (int32_t)*pcbSize * 4);
        reinterpret_cast<file *>(pDataSource) -> releaseData();
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
