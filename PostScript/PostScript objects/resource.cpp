
#include "PostScript objects\resource.h"

   resource::resource(job *pJob,object *pc,object *pi,object *pn) : 
      object(pJob,objectType::resource),
      pCategory(pc),pInstance(pi),pName(pn) {
   return;
   }


   resource::~resource() {
   return;
   }


   char *resource::Name(char *pszNewName) {
   return pName -> Name();
   }
