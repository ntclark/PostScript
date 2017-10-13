
#pragma once

#include "object.h"

   class mark : public object {
   public:

      enum markType {
         array = 1,
         dictionary = 2,
         procedure = 3,
         unspecified = 4
      };

      mark(markType);
      ~mark();

   private:

      markType theType;
   
   };
