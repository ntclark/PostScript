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

#include "job.h"
#include "PostScript objects\binaryString.h"
#include "error.h"


    binaryString::binaryString(job *pJob,char *pszContents,BYTE *pBinary,long theLength) : 
        dwLength(theLength),
        string(pJob,pszContents,object::valueType::binaryString) { 
    pbData = new BYTE[dwLength];
    memcpy(pbData,pBinary,dwLength);
    return;
    }

    binaryString::~binaryString() {
    if ( ! ( NULL == pbData ) )
        delete [] pbData;
    return;
    }


    long binaryString::length() {
    return dwLength;
    }


    void binaryString::put(long index,BYTE v) {
    pbData[index] = (char)v;
    return;
    }


    BYTE binaryString::get(long index) {
    return pbData[index];
    }


    BYTE *binaryString::getData() {
    return pbData;
    }