#pragma once

#include "string.h"

   class constantString : public string {
   public:

      constantString(job *pJob,char *pStart,char *pEnd);

      virtual ~constantString();

   private:

   };