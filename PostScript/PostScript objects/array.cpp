
#include "PostScript objects\array.h"

#include "job.h"

    array::array(job *pj,char *pn,objectType t) :
        object(pj,pn,t,object::container)
    {
    return;
    }

    array::array(job *pj,char *pszName) :
        object(pj,pszName,object::array,object::container)
    {
    return;
    }

    array::array(job *pj,char *pszName,char *pszValues) :
        object(pj,pszName,object::array,object::container)
    {
    PdfUtility *pUtility = pJob -> GetPdfUtility();
    long countValues = pUtility -> ArraySize(pszValues);
    for ( long k = 0; k < countValues; k++ )
        insert(new (pJob -> CurrentObjectHeap()) object(pJob,pUtility -> StringValueFromArray(pszValues,k + 1)));
    return;
    }

    array::array(job *pj,char *pszName,long initialSize) :
        object(pj,pszName,object::array,object::container)
    {
    for ( long k = 0; k < initialSize; k++ )
        entries[k] = new (pJob -> CurrentObjectHeap()) object(pJob,0L);
    return;
    }

    array::array(job *pj,long initialSize) :
        object(pj,"",object::array,object::container)
    {
    for ( long k = 0; k < initialSize; k++ )
        entries[k] = new (pJob -> CurrentObjectHeap()) object(pJob,0L);
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

    void array::put(long index,object *p) {
    entries[index] = p;
    return;
    }

    object *array::get(long index) {
    if ( ! find(index) )
        return NULL;
    return entries[index];
    }

    void array::clear() {
    entries.clear();
    }