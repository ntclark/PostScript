
#pragma once

#include <stack>

#include "PostScript objects\dictionary.h"

   class dictionaryStack : public std::stack<dictionary *> {
   public:

      dictionary *top();
      dictionary *pop();

      dictionary *find(char *pszName);

      void push(dictionary *);
  
   private:

   };