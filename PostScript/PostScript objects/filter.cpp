
#include "PostScript objects/filter.h"

#include "ascii85.h""

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

    BYTE *filter::getBinaryData(DWORD *pcbSize,char *pszEndDelimiter) {

    if ( NULL == pDataSource )
        return NULL;

    BYTE *pbSource = NULL;
    *pcbSize = 0L;

    if ( object::objectType::file == pDataSource -> ObjectType() ) 
        pbSource = reinterpret_cast<file *>(pDataSource) -> getBinaryData(pcbSize,szSourceEndDelimiter);

    else if ( object::objectType::filter == pDataSource -> ObjectType() )
        pbSource = reinterpret_cast<filter *>(pDataSource) -> getBinaryData(pcbSize,szSourceEndDelimiter);

    else 
        return NULL;

    if ( 0 == strcmp(szFilterName,"ASCII85Decode") ) {
        pbData = new BYTE[*pcbSize * 4];
        int32_t rv = decode_ascii85(pbSource,(int32_t)*pcbSize, pbData, (int32_t)*pcbSize * 4);
        reinterpret_cast<file *>(pDataSource) -> releaseData();
        return pbData;
    }

    if ( 0 == strcmp(szFilterName,"DCTDecode") ) {
        return pbSource;
    }

    }
