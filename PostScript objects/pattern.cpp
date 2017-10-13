
#include "PostScript objects\pattern.h"

   pattern::pattern(job *pj) :
      dictionary(pj,object::pattern)
   {
   return;
   }

   pattern::~pattern() {
   return;
   }