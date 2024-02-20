
#include "PostScript objects\mark.h"

   mark::mark(job *pJob,markType t) :
      theType(t),
      object(pJob,"",object::mark)
   {
   return;
   }

   mark::~mark() {
   return;
   }