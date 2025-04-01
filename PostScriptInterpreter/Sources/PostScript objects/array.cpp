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

#include "PostScript objects/array.h"

#include "job.h"

    array::array(job *pj,char *pszName) :
        array(pj,pszName,0,NULL) 
    {
    return;
    }

    array::array(job *pj,char *pszName,char *pszValues) :
        array(pj,pszName,0,pszValues) 
    {
    return;
    }

    array::array(job *pj,char *pszName,long initialSize) :
        array(pj,pszName,initialSize,NULL)
    {
    return;
    }

    array::array(job *pj,long initialSize) :
        array(pj,"",initialSize,NULL)
    {
    return;
    }

    array::array(job *pj,char *pszName,long initialSize,char *pszValues) :
        object(pj,pszName,object::objectType::objTypeArray,object::valueType::container,object::valueClassification::composite)
    {

    for ( long k = 0; k < initialSize; k++ )
        entries[k] = new (pJob -> CurrentObjectHeap()) object(pJob,0L);

    if ( ! ( NULL == pszValues ) ) {
        long countValues = sizeFromString(pszValues);
        for ( long k = 0; k < countValues; k++ )
            insert(new (pJob -> CurrentObjectHeap()) object(pJob,stringValueFromArray(pszValues,k + 1)));
    }
    return;
    }

    array::~array() {
    for ( std::pair<long,object *> pPair : entries )
        pJob -> deleteObject(pPair.second);
    entries.clear();
    return;
    }

    void array::insert(object *p) {
    entries[(long)entries.size()] = p;
    return;
    }

    void array::putElement(long index,object *p) {
    entries[index] = p;
    return;
    }

    object *array::getElement(long index) {
    if ( ! find(index) )
        return NULL;
    return entries[index];
    }

    void array::setValue(long index,POINT_TYPE v) {
    object *pElement = getElement(index);
    if ( NULL == pElement )
        return;
    pElement -> OBJECT_SET_POINT_TYPE_VALUE(v);
    return;
    }


    void array::clear() {
    entries.clear();
    }


    long array::sizeFromString(char *pszValue) {

    char *pValue = new char[strlen(pszValue) + 1];

    memset(pValue,0,(strlen(pszValue) + 1) * sizeof(char));

    strcpy(pValue,pszValue);   

    char *p = strtok(pValue,ARRAY_START_DELIMITERS);

    if ( ! p ) {
        delete [] pValue;
        return 0;
    }

    long countItems = 0;

    while ( p ) {

        if ( ']' == *p )
            break;

        countItems++;

        p = strtok(NULL,ARRAY_ITEM_DELIMITERS);

        if ( ! p || ']' == *p )
            break;

    }

    delete [] pValue;

    return countItems;
    }


    char *array::stringValueFromArray(char *pszValue,long oneBasedIndex) {

    static char szValue[4096];

    memset(szValue,0,sizeof(szValue));

    strcpy(szValue,pszValue);

    char *p = strtok(szValue,ARRAY_START_DELIMITERS);
    if ( ! p )
        return NULL;

    for ( long k = 0; k < oneBasedIndex - 1; k++ ) {
        p = strtok(NULL,ARRAY_ITEM_DELIMITERS);
        if ( ! p )
            return NULL;
    }

    if ( ! *p )
        return p;

    if ( ']' == p[strlen(p) - 1] )
        p[strlen(p) - 1] = '\0';

    return p;
    }
