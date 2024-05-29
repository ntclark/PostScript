
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
        object(pj,pszName,object::objectType::array,object::valueType::container,object::valueClassification::composite)
    {

    for ( long k = 0; k < initialSize; k++ )
        entries[k] = new (pJob -> CurrentObjectHeap()) object(pJob,0L);

    if ( ! ( NULL == pszValues ) ) {
        PdfUtility *pUtility = pJob -> GetPdfUtility();
        long countValues = pUtility -> ArraySize(pszValues);
        for ( long k = 0; k < countValues; k++ )
            insert(new (pJob -> CurrentObjectHeap()) object(pJob,pUtility -> StringValueFromArray(pszValues,k + 1)));
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
