
#pragma once

#include "object.h"

   class resource : public object {
   public:

      resource(object *pCategory,object *pInstance,object *pName);
      ~resource();

      virtual char *Name(char *pszNewName = NULL);

   private:

      object *pCategory;
      object *pInstance;
      object *pName;

   };
