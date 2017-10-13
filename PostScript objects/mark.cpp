
#include "PostScript objects\mark.h"

   mark::mark(markType t) :
      theType(t),
      object("",object::mark)
   {
   return;
   }

   mark::~mark() {
   return;
   }