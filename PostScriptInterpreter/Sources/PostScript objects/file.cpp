
#include "job.h"
#include "PostScript objects/file.h"

    file::file(job *pj,char *pszName) :
        object(pj,pszName,object::objectType::file,object::valueType::container,object::valueClassification::composite)
    {
    memset(szFileName,0,sizeof(szFileName));
    strcpy(szFileName,pszName);
    }

    file::~file() {}


    uint8_t *file::getBinaryData(DWORD *pcbData,char *pszEndDelimiter) {

#if 0
    FILE *psInputFile = fopen(szFileName,"rb");

    fseek(psInputFile,pJob -> currentInputFileOffset(),SEEK_SET);

BYTE pbTest[1024];
fread(pbTest,1,1024,psInputFile);
#endif

    char *pStart = pJob -> currentInput();

    char *ppEnd = NULL;

    pJob -> parseBinary(pszEndDelimiter,pStart,&ppEnd);

    long cbData = (long)(ppEnd - pStart);

    uint8_t *pbTemp = new uint8_t[cbData];

    memcpy(pbTemp,pStart,cbData);

    ppEnd += strlen(pszEndDelimiter);
    pJob -> setCurrentInput(ppEnd);

    long k = 0;
    long cbResult = 0;
    while ( k < cbData ) {
        if ( 0x0D == pbTemp[k] && 0x0A == pbTemp[k + 1] ) {
            k += 2;
            continue;
        }
        cbResult++;
        k++;
    }

    *pcbData = cbResult;

    pbData = new uint8_t[cbResult];

    k = 0;
    cbResult = 0;
    while ( k < cbData ) {
        if ( 0x0D == pbTemp[k] && 0x0A == pbTemp[k + 1] ) {
            k += 2;
            continue;
        }
        pbData[cbResult++] = pbTemp[k];
        k++;
    }

    delete [] pbTemp;

    return pbData;
    }
    