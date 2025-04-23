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

#include "job.h"
#include "PostScript objects/file.h"

    file::file(job *pj,char *pszName) :
        object(pj,pszName,object::objectType::file,object::valueType::container,object::valueClassification::composite)
    {
    memset(szFileName,0,sizeof(szFileName));
    strcpy(szFileName,pszName);
    }


    uint8_t *file::getBinaryData(DWORD *pcbData,char *pszEndDelimiter) {

    char *pStart = pJob -> currentInput();

    char *ppEnd = NULL;

    pJob -> parseBinary(pszEndDelimiter,pStart,&ppEnd);

    long cbData = (long)(ppEnd - pStart);

    uint8_t *pbTemp = ::new uint8_t[cbData];

    memcpy(pbTemp,pStart,cbData);

    ppEnd += strlen(pszEndDelimiter);
    pJob -> setCurrentInput(ppEnd);

    long k = 0;
    long cbResult = 0;
    while ( k < cbData ) {
        if ( 0x0D == pbTemp[k] || 0x0A == pbTemp[k] ) {
            k += 1;
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
        if ( 0x0D == pbTemp[k] || 0x0A == pbTemp[k] ) {
            k += 1;
            continue;
        }
        pbData[cbResult++] = pbTemp[k];
        k++;
    }

    delete [] pbTemp;

    return pbData;
    }
    