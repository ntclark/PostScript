#pragma once

#include <stack>

struct gdiParameters;

   class gdiParametersStack : public std::stack<gdiParameters *> {
   public:

      gdiParametersStack();
      ~gdiParametersStack();

      void initialize();

      void setupDC();

      void gSave();
      void gRestore();

   private:

   };