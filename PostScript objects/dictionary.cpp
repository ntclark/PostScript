
#include "utilities.h"

#include "PostScript objects\dictionary.h"
#include "PostScript objects\directExec.h"

   dictionary::dictionary(job *pj,char *pszContents) :
      pJob(pj),
      object(pszContents,object::dictionary)
   {
   return;
   }

   dictionary::dictionary(job *pj,char *pszContents,object::objectType t) :
      pJob(pj),
      object(pszContents,t)
   {
   return;
   }

   dictionary::dictionary(job *pj,object::objectType t) :
      pJob(pj),
      object(t)
   {
   return;
   }


   dictionary::~dictionary() {

//NTC: This loop is suspect of never exiting
//
//NTC: 12-17-2011: Commenting this out for now. 
//
#if 0
   while ( entries.size() ) {
      std::map<long,object *>::iterator it = entries.begin();
      delete (*it).second;
      entries.erase(it);
   }
#endif

   operators.clear();

   for ( std::list<char *>::iterator it = keys.begin(); it != keys.end(); it++ ) 
      delete [] (*it);
   
   keys.clear();

   return;
   }


   void dictionary::insert(char *pszName,void (job::*theProcedure)()) {

   long hc = HashCode(pszName);

   operators[hc] = theProcedure;   

   bool didExist = false;

   if ( entries.find(hc) != entries.end() )
      didExist = true;

   entries[hc] = new directExec(pszName,theProcedure);

   if ( didExist )
      return;

   char *p = new char[strlen(pszName) + 1];
   p[strlen(pszName)] = '\0';
   strcpy(p,pszName);
   keys.insert(keys.end(),p);

   return;
   }


   void dictionary::insert(char *pszKey,object *pValue) {

   long hc = HashCode(pszKey);

   bool didExist = false;

   if ( entries.find(hc) != entries.end() )
      didExist = true;

   entries[hc] = pValue;

   if ( didExist )
      return;

   char *p = new char[strlen(pszKey) + 1];
   p[strlen(pszKey)] = '\0';
   strcpy(p,pszKey);
   keys.insert(keys.end(),p);

   return;
   }

   void dictionary::insert(char *pszKey,char *pszValue) {

   object *pNewObject = new object(pszValue);
   switch ( pNewObject -> ObjectType() ) {
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
   if ( it != entries.end() )
      return it -> second;
   return NULL;
   }


   boolean dictionary::exists(char *pszName) {
   if ( entries.find(HashCode(pszName)) != entries.end() )
      return 1L;
   return 0L;
   }


#if 0
   object *dictionary::get(long index) {
   std::map<long,object *>::iterator it = entries.begin();
   long k = 0;
   while ( k < index ) {
      k++;
      it++;
      if ( it == entries.end() ) {
_asm {
int 3;
}
         return NULL;
      }
   }
   return (*it).second;
   }
#endif

   char *dictionary::getKey(long index) {
   std::list<char *>::iterator it = keys.begin();
   long k = 0;
   while ( k < index ) {
      k++;
      it++;
      if ( it == keys.end() ) {
         __debugbreak();
         return NULL;
      }
   }
   return (*it);
   }

   void dictionary::remove(char *pszKey) {

   for ( std::list<char *>::iterator it = keys.begin(); it != keys.end(); it++ ) {

      if ( strcmp((*it),pszKey) || strlen((*it)) != strlen(pszKey) )
         continue;

      keys.remove((*it));

//NTC: This may not be the right thing to do here.
      object *pEntry = entries[HashCode(pszKey)];
      delete pEntry;

      entries.erase(HashCode(pszKey));

      return;

   }

   return;
   }
   