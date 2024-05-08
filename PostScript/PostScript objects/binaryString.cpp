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