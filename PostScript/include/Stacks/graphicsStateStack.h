
#pragma once

#include <stack>

class graphicsState;
class job;

   class graphicsStateStack : public std::stack<graphicsState *> {

   public:
      graphicsStateStack(job *pJob);
      ~graphicsStateStack();

      void gSave();
      graphicsState *gRestore();
      graphicsState *current();

      job *pJob{NULL};

   private:

   };