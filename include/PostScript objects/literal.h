
#pragma once

#include "object.h"

   class literal : public object {
   public:

      literal(char *pStart,char *pEnd);
      ~literal();

      char *ToString();

   private:

      char *pszStringRepresentation;

   };