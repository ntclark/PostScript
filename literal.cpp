
#include "literal.h"

   literal::literal(char *pStart,char *pEnd) :
      pszStringRepresentation(NULL),
      object(pStart,pEnd,object::literal,object::string)
   {
   long n = strlen(Contents()) + 2;
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
   long n = strlen(Contents()) + 4;
   pszStringRepresentation = new char[n];
   memset(pszStringRepresentation,0,n * sizeof(char));
   pszStringRepresentation[0] = '/';
   strcpy(pszStringRepresentation + 1,Contents());
   return pszStringRepresentation;
   }
   