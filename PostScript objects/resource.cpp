
#include "PostScript objects\resource.h"

   resource::resource(object *pc,object *pi,object *pn) : 
      object(objectType::resource),
      pCategory(pc),pInstance(pi),pName(pn) {
   return;
   }


   resource::~resource() {
   return;
   }


   char *resource::Name(char *pszNewName) {
   return pName -> Name();
   }
