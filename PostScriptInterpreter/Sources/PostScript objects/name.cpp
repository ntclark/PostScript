
#include "job.h"
#include "PostScript objects\name.h"

   name::name(job *pJob,char *pszValue) :
      object(pJob,pszValue,object::objectType::name,object::valueType::string,object::valueClassification::simple)
   {
   pJob -> validNames[std::hash<std::string>()(pszValue)] = this;
   }

   name::~name() {
   return;
   }

   