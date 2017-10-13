
#include "PostScript.h"

   class comment {
   public:

      comment(char *ps,char *pe) : pStart(ps), pEnd(pe) {};
      ~comment() {};

   public:

      char *pStart,*pEnd;

   };
   