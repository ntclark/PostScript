
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
        object(pJob,pStart,pEnd,object::objectType::atom,vt,object::valueClassification::composite) { }
   
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

    if ( index >= (long)strlen(Contents()) ) {
        char szMessage[128];
        sprintf(szMessage,"rangecheck: attempting to retrieve character %ld past the end of string %s",index,Contents());
        throw rangecheck(szMessage);
    }

    return (BYTE)Contents()[index];
    }

   