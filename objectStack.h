
#pragma once

#include <stack>
#include "object.h"

   class objectStack : public std::stack<object *> {
   public:

      object *top();
      object *pop();
      void push(object *);
  
   private:

   };