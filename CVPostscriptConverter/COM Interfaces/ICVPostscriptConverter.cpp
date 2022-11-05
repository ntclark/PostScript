
#include "CVPostscriptConverter.h"

    void replacePSCode(char *pbPS,long *pSize);

    BOOL CVPostscriptConverter::ConvertToPDF(char *pszPostscriptFileName) {

#if 0

    char szBrokenFile[256];
    char szPattern[256];

    sprintf(szPattern,"%s\\*.ps","C:\\Users\\NateC\\Desktop\\Needs Fixed");

    WIN32_FIND_DATA findData{0};

    HANDLE hFiles = FindFirstFile(szPattern,&findData);

    do {

        if ( '.' == findData.cFileName[0] )
            continue;

        sprintf(szBrokenFile,"C:\\Users\\NateC\\Desktop\\Needs Fixed\\%s",findData.cFileName);

        FILE *fX = fopen(szBrokenFile,"rb");

        fseek(fX,0,SEEK_END);

        long fileSize = ftell(fX);

        fseek(fX,0,SEEK_SET);

        char *pszReplacement = new char[fileSize + 2];
    
        memset(pszReplacement,0,(fileSize + 2) * sizeof(char));

        fread(pszReplacement,1,fileSize,fX);

        fclose(fX);

        long pSize = fileSize;

        replacePSCode2(pszReplacement,&pSize);

        fX = fopen("C:\\temp\\x.ps","wb");
        fwrite(pszReplacement,1,pSize,fX);
        fclose(fX);
        
break;

    } while ( 0 < FindNextFile(hFiles,&findData) );

    Initialize(gsInstance,(char *)"C:\\temp\\x.ps");

    gsapi_exit(gsInstance);
    gsapi_delete_instance(gsInstance);

#else

    FILE *fX = fopen(pszPostscriptFileName,"rb");

    fseek(fX,0,SEEK_END);

    long fileSize = ftell(fX);

    char *pbFile = new char[fileSize + 1];

    pbFile[fileSize] = 0x00;

    fseek(fX,0,SEEK_SET);

    fread(pbFile,1,fileSize,fX);

    fclose(fX);

    long idIndex = 0;

    long pSize = fileSize;

    replacePSCode(pbFile,&pSize);

    if ( ! ( pSize == fileSize ) ) {
        fX = fopen(pszPostscriptFileName,"wb");
        fwrite(pbFile,1,pSize,fX);
        fclose(fX);
    }

    Initialize(gsInstance,pszPostscriptFileName);

    gsapi_exit(gsInstance);
    gsapi_delete_instance(gsInstance);

    delete [] pbFile;

#endif

    return TRUE;
    }


    void replacePSCode(char *pbCode,long *pSize) {

    char *pszCurrent = pbCode;

    while ( true ) {

        char *pszStart = strstr(pszCurrent,"1 1 1 1 scol O ;");

        if ( NULL == pszStart )
            break;

        if ( ! ( pszStart[-1] == 0x0A ) && ! ( pszStart[-1] == 0x0D ) ) {
            pszCurrent = pszStart + 16;
            continue;
        }

        char *pszEnd = strstr(pszStart,"~> Z");

        char *pLF = pszEnd;

        while ( ! ( *pLF == 0x0A ) && ! ( *pLF == 0x0D ) )
            pLF++;

        while ( *pLF == 0x0A || *pLF == 0x0D )
            pLF++;

        strcpy(pszStart,pLF);

        *pSize -= pLF - pszStart;

        pszCurrent += 16;

    }

    return;
    }
