
#include "PostScript objects\pattern.h"

   pattern::pattern(job *pj) :
      dictionary(pj,DEFAULT_DICTIONARY_SIZE)
   {
   theObjectType = object::objectType::pattern;
   return;
   }

   pattern::~pattern() {
   return;
   }