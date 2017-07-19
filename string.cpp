
#include <stdio.h>

#include "string.h"
#include "error.h"

   string::string(char *pszValue) :
      object(pszValue,object::atom,object::string)
   {
   }

   string::string(char *pStart,char *pEnd) :
      object(pStart,pEnd,object::atom,object::string)
   {
   }
   
   string::~string() {
   return;
   }

   void string::put(long index,object *v) {
   if ( index >= (long)strlen(Contents()) ) {
      char *pszNew = new char[index + 2];
      memset(pszNew,' ',(index + 1) * sizeof(char));
      pszNew[index + 1] = '\0';
      memcpy(pszNew,Contents(),strlen(Contents()));
      Contents(pszNew);
   }
   Contents()[index] = v -> Contents()[0];
   return;
   }

   object *string::get(long index) {
   if ( index >= (long)strlen(Contents()) ) {
      char szMessage[128];
      sprintf(szMessage,"rangecheck: attempting to retrieve character %ld past the end of string %s",index,Contents());
      throw rangecheck(szMessage);
   }
   return new object(Contents()[index]);
   }

   