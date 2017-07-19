
#include "structureSpec.h"

   structureSpec::structureSpec(char *pStart,char *pEnd) :
      object(pStart,pEnd,object::structureSpec,object::string)
   {
   }

   structureSpec::~structureSpec() {
   }   