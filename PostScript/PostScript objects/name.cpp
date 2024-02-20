
#include "PostScript objects\name.h"

   name::name(job *pJob,char *pszValue) :
      object(pJob,pszValue,object::name)
   {
   }

   name::~name() {
   return;
   }

   