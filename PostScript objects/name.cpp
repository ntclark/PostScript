
#include "PostScript objects\name.h"

   name::name(char *pszValue) :
      object(pszValue,object::name)
   {
   }

   name::~name() {
   return;
   }

   