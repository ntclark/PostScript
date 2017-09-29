
#include "array.h"

#include "job.h"

   array::array(job *pj,char *pn,objectType t) :
      pJob(pj),
      object(pn,t,object::container)
   {
   return;
   }

   array::array(job *pj,char *pszName) :
      pJob(pj),
      object(pszName,object::array,object::container)
   {
   return;
   }

   array::array(job *pj,char *pszName,char *pszValues) :
      pJob(pj),
      object(pszName,object::array,object::container)
   {
   PdfUtility *pUtility = pJob -> GetPdfUtility();
   long countValues = pUtility -> ArraySize(pszValues);
   for ( long k = 0; k < countValues; k++ )
      insert(new object(pUtility -> StringValueFromArray(pszValues,k + 1)));
   return;
   }

   array::array(job *pj,char *pszName,long initialSize) :
      pJob(pj),
      object(pszName,object::array,object::container)
   {
   for ( long k = 0; k < initialSize; k++ )
      entries[k] = new object(0L);
   return;
   }

   array::array(job *pj,long initialSize) :
      pJob(pj),
      object("",object::array,object::container)
   {
   for ( long k = 0; k < initialSize; k++ )
      entries[k] = new object(0L);
   return;
   }

   array::~array() {
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
//   if ( index > (long)entries.size() ) {
//      return NULL;
//   }
   if ( ! find(index) )
      return NULL;
   return entries[index];
   }

   void array::clear() {
   entries.clear();
   }