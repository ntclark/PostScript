#pragma once

#include <stack>

struct pathParameters;

   class pathParametersStack : public std::stack<pathParameters *> {
   public:

      pathParametersStack();
      ~pathParametersStack();

      void initialize();

      void gSave();
      void gRestore();

      boolean isInitialized() { return 0 < size(); }

   private:

   };