
#include "PostScript objects\literal.h"

   literal::literal(job *pJob,char *pValue) :
      literal(pJob,pValue,NULL) {}

   literal::literal(job *pJob,char *pStart,char *pEnd) :
      pszStringRepresentation(NULL),
      object(pJob,pStart,pEnd,object::objectType::literal,object::valueType::string,object::valueClassification::simple)
   {
   long n = (DWORD)strlen(Contents()) + 2;
   pszStringRepresentation = new char[n];
   memset(pszStringRepresentation,0,n * sizeof(char));
   pszStringRepresentation[0] = '/';
   strcpy(pszStringRepresentation + 1,Contents());
   return;
   }

   literal::~literal() {
   if ( pszStringRepresentation )
      delete []pszStringRepresentation;   
   return;
   }

   char *literal::ToString() {
   if ( pszStringRepresentation )
      delete [] pszStringRepresentation;
   long n = (DWORD)strlen(Contents()) + 4;
   pszStringRepresentation = new char[n];
   memset(pszStringRepresentation,0,n * sizeof(char));
   pszStringRepresentation[0] = '/';
   strcpy(pszStringRepresentation + 1,Contents());
   return pszStringRepresentation;
   }
   