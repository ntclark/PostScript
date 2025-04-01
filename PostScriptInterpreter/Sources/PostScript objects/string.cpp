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

#include <stdio.h>

#include "PostScript objects\string.h"
#include "PostScript objects\binaryString.h"

#include "job.h"

#include "error.h"

    string::string(job *pJob,char *pszValue) : string(pJob,pszValue,NULL) { }

    string::string(job *pJob,char *pszValue,object::valueType vt) : string(pJob,pszValue,NULL,vt) {}

    string::string(job *pJob,char *pStart,char *pEnd) : string(pJob,pStart,pEnd,object::valueType::string) { }

    string::string(job *pJob,object::valueType vt) : string(pJob,NULL,NULL,vt) {}

    string::string(job *pJob,char *pStart,char *pEnd,object::valueType vt) :
        object(pJob,pStart,pEnd,object::objectType::atom,vt,object::valueClassification::composite)
    { 

    if ( NULL == strchr(Contents(),'\\') )
        return;

    DWORD n = (DWORD)strlen(Contents()) + 1;

    pszUnescapedString  = new char[n];

    char *pLast = pszUnescapedString + n;
    *pLast = '\0';

    strcpy(pszUnescapedString,Contents());

    char *pSlash = strchr(pszUnescapedString,'\\');

    while ( ! ( NULL == pSlash ) ) {

        if ( isdigit(*(pSlash + 1)) ) {

            char *pDigit = pSlash + 1;
            while ( isdigit(*pDigit) && pDigit < pLast ) pDigit++;
            char c = *pDigit;
            *pDigit = '\0';

            char szOctal[32];
            strcpy(szOctal,pSlash + 1);
            *pDigit = c;

            long value;
            sscanf(szOctal,"%o",&value);

            char *pszRemainder = new char[pLast - pDigit];
            strcpy(pszRemainder,pDigit);
            *pSlash = '#';
            strcpy(pSlash + 1,pszRemainder);

            escapedValues[(uint16_t)(pSlash - pszUnescapedString)] = (BYTE)value;

            delete [] pszRemainder;
            pSlash = strchr(pSlash + 1,'\\');
            continue;
        }

        char *pszRemainder = new char[pLast - pSlash];
        strcpy(pszRemainder,pSlash + 1);
        strcpy(pSlash,pszRemainder);
        delete [] pszRemainder;
        pSlash = strchr(pSlash,'\\');
    }

    return;
    }


    string::~string() {
    return;
    }


    void string::put(long index,BYTE v) {

    switch ( theValueType ) {
    default:
        break;

    case valueType::binaryString:
        return reinterpret_cast<class binaryString *>(this) -> put(index,v);

    }

    if ( index >= (long)strlen(Contents()) ) {
        char *pszNew = new char[index + 2];
        memset(pszNew,' ',(index + 1) * sizeof(char));
        pszNew[index + 1] = '\0';
        memcpy(pszNew,Contents(),strlen(Contents()));
        Contents(pszNew);
    }

    Contents()[index] = (char)v;
    return;
    }


    BYTE string::get(long index) {

    switch ( theValueType ) {
    default:
        break;

    case valueType::binaryString:
        return reinterpret_cast<class binaryString *>(this) -> get(index);

    }

    if ( index >= length() ) {
        char szMessage[128];
        sprintf(szMessage,"rangecheck: attempting to retrieve character %ld past the end of string %s",index,Contents());
        throw rangecheck(szMessage);
    }

    if ( NULL == pszUnescapedString ) 
        return (BYTE)Contents()[index];

    if ( 0 == escapedValues.size() || ( escapedValues.end() == escapedValues.find((uint16_t)index) ) )
        return (BYTE)pszUnescapedString[index];

    return escapedValues[(uint16_t)index];
    }