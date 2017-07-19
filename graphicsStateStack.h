
#pragma once

#include <stack>

class graphicsState;
class job;

   class graphicsStateStack : public std::stack<graphicsState *> {
   public:
      graphicsStateStack(job *pJob);
      ~graphicsStateStack();

      void save();
      graphicsState *restore();
      graphicsState *current();
  
   private:

   };