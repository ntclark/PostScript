
#include "booleanObject.h"

   booleanObject::booleanObject(char *psz) :
      object(psz,object::logical)
   {
   if ( 0 == stricmp(psz,"true") )
      value = true;
   else
      value = false;
   return;
   }


   booleanObject::~booleanObject() {
   return;
   }