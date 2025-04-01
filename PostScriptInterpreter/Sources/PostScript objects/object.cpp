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

#include "PostScript objects\object.h"
#include "job.h"

#include <float.h>

    void *object::pHeap = NULL;
    void *object::pCurrentHeap = NULL;
    void *object::pNextHeap = NULL;
    size_t object::currentlyAllocatedHeap = 0L;

    void object::initializeStorage() {
    pHeap = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,OBJECT_HEAP_SIZE);
    pCurrentHeap = (BYTE *)pHeap;
    pNextHeap = (BYTE *)pCurrentHeap;
    currentlyAllocatedHeap = OBJECT_HEAP_SIZE;
    }

    void object::releaseStorage() {
    GlobalFree(pHeap);
    pHeap = NULL;
    pCurrentHeap = NULL;
    pNextHeap = NULL;
    currentlyAllocatedHeap = 0L;
    }

    void *object::operator new(size_t theSize) {
    return ::new BYTE[theSize];
    }

    void *object::operator new(size_t theSize,void *pPtr) {

    if ( ((BYTE *)pNextHeap - (BYTE *)pHeap + theSize) > currentlyAllocatedHeap ) {
MessageBox(NULL,"Big Problem","Out of object space",MB_OK | MB_TOPMOST);
    }

    pCurrentHeap = pNextHeap;
    pNextHeap = (BYTE *)pCurrentHeap + theSize;
    return pCurrentHeap;
    }


    object::object(job *pj,char charVal,long longVal,double realVal,char *pszVal,char *pszEnd,objectType ot,valueType vt,valueClassification vcf,executableAttribute ea,accessAttribute aa) :

        pJob(pj),
        theObjectType(ot),
        theValueType(vt),
        theExecutableAttribute(ea),
        theAccessAttribute(aa)

    {

    pJob -> incrementCount();

    if ( ! ( '\0' == charVal ) ) {

        pszContents = new char[2];
        pszContents[0] = charVal;
        pszContents[1] = '\0';

        pszName = new char[2];
        pszName[0] = charVal;
        pszName[1] = '\0';

        if ( objectTypeUnspecified == theObjectType )
            theObjectType = atom;

        if ( valueTypeUnspecified == theValueType )
            theValueType = character;

    } else if ( ! ( NULL == pszVal ) ) {

        long n = 0L;
        if ( ! ( NULL == pszEnd ) )
            n = (long)(pszEnd - pszVal);
        else
            n = (long)strlen(pszVal);

        pszContents = new char[n + 1];
        pszContents[n] = '\0';
        if ( ! ( NULL == pszEnd ) )
            strncpy(pszContents,pszVal,n);
        else
            strcpy(pszContents,pszVal);

        pszName = new char[n + 1];
        pszName[n] = '\0';
        if ( ! ( NULL == pszEnd ) )
            strncpy(pszName,pszVal,n);
        else
            strcpy(pszName,pszVal);

        if ( objectTypeUnspecified == theObjectType )
            theObjectType = atom;

        if ( valueTypeUnspecified == theValueType )
            theValueType = string;

    } else if ( ! ( LONG_MAX == longVal ) ) {

        pszContents = new char[64];
        memset(pszContents,0,64 * sizeof(char));
        sprintf(pszContents,"%ld",longVal);

        pszName = new char[64];
        memset(pszName,0,64 * sizeof(char));
        sprintf(pszName,"%ld",longVal);

        if ( objectTypeUnspecified == theObjectType )
            theObjectType = number;

        if ( valueTypeUnspecified == theValueType )
            theValueType = integer;

    } else if ( ! ( DBL_MAX == realVal ) ) {

        pszContents = new char[64];
        memset(pszContents,0,64 * sizeof(char));
        sprintf(pszContents,"%lf",realVal);

        pszName = new char[64];
        memset(pszName,0,64 * sizeof(char));
        sprintf(pszName,"%lf",realVal);

        if ( objectTypeUnspecified == theObjectType )
            theObjectType = number;

        if ( valueTypeUnspecified == theValueType )
            theValueType = real;
    }

    parseValue(ot,vt);

    return;
    }


    object::object(job *pj,objectType ot,valueType vt,valueClassification vcf) :
        object(pj,'\0',LONG_MAX,DBL_MAX,NULL,NULL,ot,vt,vcf,nonExecutable,unlimited) {}

    object::object(job *pj,char *pszContents,objectType ot,valueType vt,valueClassification vcf) :
        object(pj,'\0',LONG_MAX,DBL_MAX,pszContents,NULL,ot,vt,vcf,nonExecutable,unlimited) {}

    object::object(job *pj,char *pszContents,objectType ot,valueType vt,valueClassification vcf,executableAttribute ea) :
        object(pj,'\0',LONG_MAX,DBL_MAX,pszContents,NULL,ot,vt,vcf,ea,unlimited) {}

    object::object(job *pj,char *pStart,char *pEnd) :
        object(pj,'\0',LONG_MAX,DBL_MAX,pStart,pEnd,objectTypeUnspecified,valueTypeUnspecified,simple,nonExecutable,unlimited) {}

    object::object(job *pj,char *pStart,char *pEnd,objectType ot,valueType vt,valueClassification vcf) :
        object(pj,'\0',LONG_MAX,DBL_MAX,pStart,pEnd,ot,vt,vcf,nonExecutable,unlimited) {}

    object::object(job *pj,char c) :
        object(pj,   c,LONG_MAX,DBL_MAX,NULL,NULL,objectTypeUnspecified,character,simple,nonExecutable,unlimited) {}

    object::object(job *pj,char *pszContents) :
        object(pj,'\0',LONG_MAX,DBL_MAX,pszContents,NULL,objectTypeUnspecified,valueTypeUnspecified,simple,nonExecutable,unlimited) {}

    object::object(job *pj,BYTE bValue) :
        object(pj,'\0',(long)bValue,DBL_MAX,NULL,NULL,number,integer,simple,nonExecutable,unlimited) {}

    object::object(job *pj,unsigned short bValue) :
        object(pj,'\0',(long)bValue,DBL_MAX,NULL,NULL,number,integer,simple,nonExecutable,unlimited) {}

    object::object(job *pj,long value) :
        object(pj,'\0',value,DBL_MAX,NULL,NULL,number,integer,simple,nonExecutable,unlimited) {}

    object::object(job *pj,double value) :
        object(pj,'\0',LONG_MAX,value,NULL,NULL,number,real,simple,nonExecutable,unlimited) {}


    object::~object() {

    pJob -> decrementCount();

    if ( ! ( NULL == pszContents ) )
        delete [] pszContents;
    if ( ! ( NULL == pszName ) )
        delete [] pszName;
    return;
    }


    char *object::Contents(char *pszNewContents) { 
    if ( ! ( NULL == pszNewContents ) ) {
        if ( ! ( NULL == pszContents ) )
            delete [] pszContents;
        pszContents = new char[strlen(pszNewContents) + 1];
        strcpy(pszContents,pszNewContents);
    }
    return pszContents;
    }


    char *object::Name(char *pszNewName) {

    if ( NULL == pszNewName )
        return pszName;

    if ( ! ( NULL == pszName ) )
        delete [] pszName;

    pszName = new char[strlen(pszNewName) + 1];
    pszName[strlen(pszNewName)] = '\0';
    strcpy(pszName,pszNewName);

    return pszName;
    }


    void object::parseValue(objectType ot,valueType vt) {

    if ( NULL == pszContents )
        return;

    char *p = pszContents;

    if ( '\0' == *p )
        return;

    // radix number is xx#anychars.. xx digits, min 2, max 36, 

    bool isReal = false;
    bool radixFound = false;
    long digitCount = 0L;

    long preRadixDigitIndex = -1L;
    char radixBase[2]{'\0','\0'};

    while ( *p ) {

        if ( ( 'a' <= *p && *p <= 'z' ) || ( 'A' <= *p && *p <= 'Z' ) ) {

            if ( ! radixFound )
                return;

        } else if ( '0' <= *p && *p <= '9'  ) {

            digitCount++;

            if ( ! radixFound ) {
                preRadixDigitIndex++;
                if ( 2 > preRadixDigitIndex ) 
                    radixBase[preRadixDigitIndex] = *p;
            }

        } else if ( '.' == *p ) {

            isReal = true;

        } else if ( '#' == *p ) {

            if ( radixFound || -1L == preRadixDigitIndex ) 
                return;

            if ( 2 > atol(radixBase) || 36 < atol(radixBase) )
                return;

            radixFound = true;

        } else 

            if ( ! ( '-' == *p || '+' == *p || 0x0A == *p || 0x0D == *p ) ) 
                return;

        p++;

    }

    if ( 0L == digitCount )
        return;

    if ( objectTypeUnspecified == ot )
        theObjectType = number;

    if ( isReal ) {
        doubleValue = atof(pszContents);
        if ( valueTypeUnspecified == vt )
            theValueType = object::real;
    } else if ( radixFound ) {
        char *p = strchr(pszContents,'#');
        *p = '\0';
        long base = atol(pszContents);
        *p = '#';
        p = p + 1;
        intValue = strtoul(p,NULL,base);
        if ( valueTypeUnspecified == vt )
            theValueType = object::radix;
    } else {
        intValue = atol(pszContents);
        if ( valueTypeUnspecified == vt )
            theValueType = object::integer;
    } 

    return;
    }


    long object::IntValue(long value) { 
    if ( -LONG_MAX == value ) {
        if ( object::integer == theValueType )
            return intValue;
        return atol(pszContents);
    }
    intValue = value; 
    char szInt[32];
    sprintf_s<32>(szInt,"%ld",value);
    Contents(szInt);
    theObjectType = object::number;
    theValueType = object::integer;
    return intValue;
    }


    double object::DoubleValue(double value) {

    if ( -DBL_MAX == value ) {
        if ( object::real == theValueType )
            return doubleValue; 
        return atof(pszContents);
    }

    doubleValue = value; 
    char szDouble[64];
    sprintf_s<64>(szDouble,"%f",value);
    Contents(szDouble);
    theObjectType = object::number;
    theValueType = object::real;
    return doubleValue;
    }


    float object::FloatValue(float value) {
    if ( -FLT_MAX == value ) 
        return (float)DoubleValue();
    return (float)DoubleValue((double)value);
    }


    POINT_TYPE object::PointValue() {
    return (POINT_TYPE)DoubleValue();
    }


    double object::Value() {
    if ( object::number == theObjectType ) {
        if ( object::integer == theValueType )
            return (double)intValue;
        return doubleValue;
    }
    return atof(pszContents);
    }

    char *object::TypeName() {

    switch ( theObjectType ) {
    case objectTypeUnspecified:
        return "unspecified";
    case atom:
        return "atom";
    case procedure:
        return "procedure";
    case dictionary:
        return "dictionary";
    case structureSpec:
        return "structureSpec";
    case directExecutable:
        return "operand";
    case objTypeMatrix:
        return "matrix";
    case objTypeArray:
        return "array";
    case packedarray:
        return "packedarray";
    case number:
        return "number";
    case logical:
        return "logical";
    case mark:
        return "mark";
    case null:
        return "null";
    case objTypeSave:
        return "save";
    case pattern:
        return "pattern";
    case colorSpace:
        return "colorSpace";
    case font:
        return "font";
    case resource:
        return "resource";
    }
    return "";
    }

    char *object::ValueTypeName() {
    switch ( theValueType ) {
    case valueTypeUnspecified:
        return "unspecified";
    case container:
        return "container";
    case string:
        return "string";
    case character:
        return "character";
    case integer:
        return "integer";
    case real:
        return "real";
    case radix:
        return "radix";
    }
    return "";
    }


    void object::put(long,BYTE) {
    throw new notimplemented("put is not implemented on a subclass of object. This is an error condition, further parsing is suspect");
    _endthread();
    return;
    }


    BYTE object::get(long index) {
    throw new notimplemented("get is not implemented on a subclass of object. This is an error condition, further parsing is suspect");
    _endthread();
    return NULL;
    }

    void object::putElement(long index,object *) {
    throw new notimplemented("putElement is not implemented on a subclass of object. This is an error condition, further parsing is suspect");
    _endthread();
    return;
    }


    object *object::getElement(long index) {
    throw new notimplemented("getElement is not implemented on a subclass of object. This is an error condition, further parsing is suspect");
    _endthread();
    return NULL;
    }


    void object::execute() {
    throw new notimplemented("execute is not implemented on a subclass of object. This is an error condition, further parsing is suspect");
    _endthread();
    return;
    }


    char *object::ToString() {
    return Contents();
    }