
#include "PostScript objects\resource.h"

   resource::resource(job *pJob,object *pc,object *pi,object *pn) : 
      object(pJob,object::objectType::resource,object::valueType::valueTypeUnspecified,object::valueClassification::composite),
      pCategory(pc),pInstance(pi),pName(pn) {
   return;
   }


   resource::~resource() {
   return;
   }


   char *resource::Name(char *pszNewName) {
   return pName -> Name();
   }
