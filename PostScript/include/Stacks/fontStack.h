#pragma once

#include <stack>

class font;

   class fontStack : public std::stack<font *> {
   public:

      fontStack();
      ~fontStack();

      void initialize();

      void gSave();
      void gRestore();

   private:

   };