
#pragma once

#include "object.h"

   class string : public object {
   public:

      string(char *pszValue); 
      string(char *pStart,char *pEnd);
      ~string();

      virtual object *get(long index);
      virtual void put(long index,object *);

   private:

   };