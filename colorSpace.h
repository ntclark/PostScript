
#pragma once

#include "array.h"

   class job;

   class colorSpace : public array {
   public:

      colorSpace(job *,object *);
      colorSpace(job *,char *pszName);
      ~colorSpace();

      long ParameterCount();

   private:

      long parameterCount;

   };
