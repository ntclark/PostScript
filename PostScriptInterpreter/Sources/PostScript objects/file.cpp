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

    char file::szEndDelimiter[]{"~>"};

    file::file(job *pj,char *pszName) :
        object(pj,pszName,object::objectType::file,object::valueType::container,object::valueClassification::composite)
    {
    memset(szFileName,0,sizeof(szFileName));
    strcpy(szFileName,pszName);
    }


    void file::operatorReadstring(object *pString) {

    /*

    NTC: 03-02-2026
    This (base class) version of readstring was presumably developed to read data 
    for Adobe type 1 fonts.

    */

    uint32_t strSize = (uint32_t)reinterpret_cast<class string *>(pString) -> length();

    uint8_t *pbContents = new uint8_t[strSize + 1];
    pbContents[strSize] = '\0';

/*
    Each use of RD is followed by exactly one blank
    character followed by a sequence of binary bytes that are the
    charstring contents.
*/

    pJob -> currentInputFlushSpace();

    uint32_t countRead = 0;

    while ( countRead < strSize ) {
        if ( NULL == pJob -> currentInput() )
            break;
        pbContents[countRead] = (uint8_t)*(pJob -> currentInput());
        pJob -> setCurrentInput(pJob -> currentInput() + 1);
        countRead++;
    }

    /*

    page 65 of Adobe Type 1 Font Format states:

        When this encoded and encrypted charstring is expressed in
        binary form, it is ready for inclusion in a Type 1 font program.
        The charstring would be inserted in the CharStrings dictionary as
        follows:

        /C 41 RD ~41~binary~bytes~ ND

        SOMEWHERE I saw a reference to the first, or first few bytes, in the 
        string indicating it is binary and should be decrypted.
        Can't find that again. In any case, I will assume that if
        executionStack.size is > 0, it means that an Adobe type1 font
        program is being read, and that charstrings are encrypted
        but still keep them encrypted until point of use
    */

    object *pTarget = NULL;

    if ( 0 == pJob -> executionStack.size() )
        pTarget = new (pJob -> CurrentObjectHeap()) class string(pJob,(char *)pbContents);
    else
        pTarget = new (pJob -> CurrentObjectHeap()) class binaryString(pJob,NULL,pbContents,countRead);

    pJob -> push(pTarget);

    delete [] pbContents;

    if ( countRead <= strSize )
        pJob -> push(pJob -> pTrueConstant);
    else
        // Need a test for this which is an eexec or readstring at end of the input
        pJob -> push(pJob -> pFalseConstant);

    return;
    }


    uint8_t *file::getBinaryData(uint32_t *pcbData) {

    pJob -> currentInputFlushSpace();

    char *pStart = pJob -> currentInput();
    while ( 0x0A == *pStart || 0x0D == *pStart )
        pStart++;

    if ( 0 == pJob -> inputLineNumberSize ) {

        char *pEnd = NULL;

        pJob -> parseBinary(file::szEndDelimiter,pStart,&pEnd);

        long cbData = (long)(pEnd - pStart);

        //if ( strcmp(file::szEndDelimiter,"cleartomark") )
        //    pEnd += strlen(pszEndDelimiter);

        pJob -> setCurrentInput(pEnd);

        long cbValid = 0;
        char *p = pStart;
        while ( p < pEnd ) {
            if ( 0x0D == *p || 0x0A == *p ) {
                p++;
                continue;
            }
            cbValid++;
            p++;
        }

        *pcbData = cbValid;

        pbData = new uint8_t[cbValid];

        p = pStart;
        cbValid = 0;
        while ( cbValid < *pcbData ) {
            if ( 0x0D == *p || 0x0A == *p ) {
                p++;
                continue;
            }
            pbData[cbValid++] = *p;
            p++;
        }

        return pbData;
    }

    pStart += pJob -> inputLineNumberSize;

    char *pEnd = NULL;

    pJob -> parseBinary(file::szEndDelimiter,pStart,&pEnd);

    if ( NULL == pEnd ) {
        char szMessage[128];
        sprintf_s<128>(szMessage,"file: Input line %ld: A file reached EOF before finding the end-delimiter",pJob -> inputLineNumber);
        throw new rangecheck(szMessage);
        return NULL;
    }

    long cbData = (long)(pEnd - pStart);

    uint32_t countValid = 0;
    char *p = pStart;

    while ( p < pEnd ) {
        if ( 0x0A == *p || 0x0D == *p ) {
            while ( 0x0A == *p || 0x0D == *p )
                p++;
            p += pJob -> inputLineNumberSize;
        }
        p++;
        countValid++;
    }

    *pcbData = countValid;

    pbData = ::new uint8_t[countValid];

    p = pStart;
    countValid = 0;

    while ( p < pEnd ) {
        if ( 0x0A == *p || 0x0D == *p ) {
            while ( 0x0A == *p || 0x0D == *p )
                p++;
            p += pJob -> inputLineNumberSize;
        }
        pbData[countValid++] = *p++;
    }

    pJob -> setCurrentInput(pEnd);

    return pbData;
    }