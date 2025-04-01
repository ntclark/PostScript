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

    long dictionary::countAutoCreated = 0;

    entry::entry(job *pJob,char *pszName,object *pObj) : pValue(pObj) {
        nName = (long)strlen(strcpy(szName,pszName));
        pKey = new (pJob -> CurrentObjectHeap()) string(pJob,szName);
    }

    dictionary::dictionary(job *pj,char *pszName,long initialSize) :
        entryCount(0L),
        object(pj,pszName,object::objectType::dictionary,object::valueType::container,object::valueClassification::composite)
    {
    entries.reserve(initialSize);
    return;
    }

    dictionary::dictionary(job *pj,char *pszName) :
        dictionary(pj,pszName,DEFAULT_DICTIONARY_SIZE) {}

    dictionary::dictionary(job *pj,long initialSize) :
        dictionary(pj,NULL,initialSize) {}

    dictionary::~dictionary() {

    for ( entry *pEntry : entries ) {
        object *pValue = pEntry -> pValue;
        if ( this == pValue )
            continue;
        if ( ! ( NULL == pValue -> pContainingDictionary ) && ! ( pValue -> pContainingDictionary == this ) ) 
            continue;
        pJob -> deleteNonContainedObject(pValue);
    }

    entries.clear();

    pJob -> dictionaryStack.remove(this);

    return;
    }


    void dictionary::put(object *pValue) {
    put(pValue -> Name(),pValue);
    return;
    }


    void dictionary::put(char *pszKey,object *pValue) {

    if ( exists(pszKey) )
        remove(pszKey);

    entry *pEntry = new entry(pJob,pszKey,pValue);

    entries.push_back(pEntry);
    pValue -> pContainingDictionary = this;

    return;
    }


    void dictionary::put(char *pszKey,char *pszValue) {
    put(pszKey,new (pJob -> CurrentObjectHeap()) object(pJob,pszValue));
    return;
    }


    void dictionary::put(char *pszName,void (job::*theProcedure)()) {
    put(pszName,new (pJob -> CurrentObjectHeap()) directExec(pJob,pszName,theProcedure));
    return;
    }


    object *dictionary::retrieve(long index) {
    if ( entries.size() < index )
        return NULL;
    return entries[index] -> pValue;
    }


    char *dictionary::retrieveKey(long index) {
    if ( entries.size() < index )
        return NULL;
    return entries[index] -> szName;
    }


    object *dictionary::retrieve(char *pszName) {

    long nName = (long)strlen(pszName);
    for ( entry *pEntry : entries ) {
        if ( ! ( nName == pEntry -> nName ) )
            continue;
        if ( 0 == strcmp(pEntry -> szName,pszName) )
            return pEntry -> pValue;
    }
    return NULL;
    }


    object *dictionary::retrieveKey(char *pszName) {
    long nName = (long)strlen(pszName);
    for ( entry *pEntry : entries ) {
        if ( ! ( nName == pEntry -> nName ) )
            continue;
        if ( 0 == strcmp(pEntry -> szName,pszName) )
            return pEntry -> pKey;
    }
    return NULL;
    }


    char *dictionary::ToString() {
    if ( ! ( NULL == pszContents ) )
        return pszContents;
    if ( ! ( NULL == pszName ) )
        return pszName;
    return "";
    }


    boolean dictionary::exists(object *pValue) {
    return ! ( NULL == retrieve(pValue -> Name()) );
    }


    boolean dictionary::exists(char *pszName) {
    return ! ( NULL == retrieve(pszName) );
    }


    boolean dictionary::hasSameEntries(dictionary *pOtherDict) {
/*
    eq 
        any1 any2 eq bool

    ...

    ... Other composite objects (arrays and dictionaries) are equal only if they 
    share the same value. Separate values are considered unequal, even if all the 
    components of those values are the same.
*/
    for ( entry *pEntry : entries ) {
        object *pOtherVal = pOtherDict -> retrieve(pEntry -> szName);
        if ( NULL == pOtherVal )
            return false;
        if ( ! ( pOtherVal == pEntry -> pValue) )
            return false;
    }
    return true;
    }


    void dictionary::remove(char *pszKey) {
    for ( std::vector<entry *>::iterator it = entries.begin(); it != entries.end(); it++ ) {
        if ( 0 == strcmp((*it) -> szName,pszKey) ) {
            entries.erase(it);
            return;
        }
    }
    return;
    }


    void dictionary::remove(object *pObjRemove) {
    remove(pObjRemove -> Name());
    return;
    }


    void dictionary::copyFrom(dictionary *pSource) {
    for ( entry *pEntry : pSource -> entries )
        put(pEntry -> szName,pEntry -> pValue);
    return;
    }


    void dictionary::forAll(class procedure *pProc) {

    for ( entry *pEntry : entries ) {
        pJob -> push(pEntry -> pKey);
        pJob -> push(pEntry -> pValue);
        pJob -> executeProcedure(pProc);
    }

    return;
    }