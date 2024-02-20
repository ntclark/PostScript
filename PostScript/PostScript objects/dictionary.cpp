#include "utilities.h"

#include "PostScript objects\dictionary.h"
#include "PostScript objects\directExec.h"

    dictionary::dictionary(job *pj,char *pszContents,bool iso) :
        object(pj,pszContents,object::dictionary)
    {
    isSystemObject = iso;
    pJob -> dictionaryStack.push(this);
    return;
    }

    dictionary::dictionary(job *pj,char *pszContents) :
        object(pj,pszContents,object::dictionary)
    {
    pJob -> dictionaryStack.push(this);
    return;
    }

    dictionary::dictionary(job *pj,char *pszContents,object::objectType t) :
        object(pj,pszContents,t)
    {
    pJob -> dictionaryStack.push(this);
    return;
    }

    dictionary::dictionary(job *pj,object::objectType t) :
        object(pj,t)
    {
    pJob -> dictionaryStack.push(this);
    return;
    }


    dictionary::~dictionary() {

    long index = -1L;

    for ( std::pair<long,object *> pPair : entries ) {

        index++;

        object *pObject = pPair.second;

        if ( this == pObject )
            continue;

        if ( ! ( NULL == pObject -> pContainingDictionary ) && ! ( pObject -> pContainingDictionary == this ) ) 
            continue;

        pJob -> deleteNonContainedObject(pObject);

    }

    for ( char *pszKey : keys )
        delete [] pszKey;

    keys.clear();
    entries.clear();
    operators.clear();

    pJob -> dictionaryStack.remove(this);
    }


   void dictionary::insert(char *pszName,void (job::*theProcedure)()) {

   long hc = HashCode(pszName);

   operators[hc] = theProcedure;

   bool didExist = ! ( entries.find(hc) == entries.end() );

   entries[hc] = new (pJob -> CurrentObjectHeap()) directExec(pJob,pszName,theProcedure);

   entries[hc] -> pContainingDictionary = this;

   if ( didExist )
      return;

   char *p = new char[strlen(pszName) + 1];
   p[strlen(pszName)] = '\0';
   strcpy(p,pszName);
   keys.insert(keys.end(),p);

   return;
   }


   void dictionary::insert(char *pszKey,object *pValue) {

if ( strstr(pszName,"awidthshow") )
printf("hello world");

   long hc = HashCode(pszKey);

   bool didExist = ! ( entries.find(hc) == entries.end() );

   entries[hc] = pValue;

   pValue -> pContainingDictionary = this;

   if ( didExist )
      return;

   char *p = new char[strlen(pszKey) + 1];
   p[strlen(pszKey)] = '\0';
   strcpy(p,pszKey);
   keys.insert(keys.end(),p);

   return;
   }


    void dictionary::insert(char *pszKey,char *pszValue) {

    object *pNewObject = new (pJob -> CurrentObjectHeap()) object(pJob,pszValue);

    switch ( pNewObject -> ObjectType() ) {
    // Why aren't numbers put in the dictionary, also, this orphans pNewObject I think (memory leadk)
    case objectType::number:
        return;
    default:
        break;
    }

    insert(pszKey,pNewObject);

    return;
    }


    void dictionary::retrieve(char *pszName,void (job::**ppProcedure)()) {
    *ppProcedure = NULL;
    if ( operators.find(HashCode(pszName)) != operators.end() ) 
        *ppProcedure = operators[HashCode(pszName)];
    return;
    }


    object *dictionary::retrieve(char *pszName) {
    std::map<long,object *>::iterator it = entries.find(HashCode(pszName));
    if ( ! ( entries.end() == it ) )
        return it -> second;
    return NULL;
    }


    boolean dictionary::exists(object *pObject) {
    for ( std::pair<long,object *> pPair : entries )
        if ( pPair.second == pObject )
            return true;
    return false;
    }


    boolean dictionary::exists(char *pszName) {
    if ( ! ( entries.find(HashCode(pszName)) == entries.end() ) )
        return true;
    return false;
    }


    char *dictionary::getKey(long index) {
    return keys[index];
    }


    void dictionary::remove(char *pszKey) {

    long index = -1L;
    for ( char *pKey : keys ) {
        index++;
        if ( strcmp(pKey,pszKey) || ! ( strlen(pKey) == strlen(pszKey) ) )
            continue;
        delete [] keys[index];
        keys.erase(keys.begin() + index);
        long hc = HashCode(pszKey);
        entries.erase(hc);
        return;
    }

    return;
    }


    void dictionary::remove(object *pObjRemove) {

    long index = -1L;
    for ( std::pair<long,object *> pPair : entries ) {
        index++;
        if ( ! ( pPair.second == pObjRemove ) )
            continue;
        delete [] keys[index];
        keys.erase(keys.begin() + index);
        entries.erase(pPair.first);
        return;
    }

    return;
    }