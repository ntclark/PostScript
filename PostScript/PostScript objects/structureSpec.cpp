
#include "PostScript objects\structureSpec.h"

   structureSpec::structureSpec(job *pJob,char *pStart,char *pEnd) :
      object(pJob,pStart,pEnd,object::structureSpec,object::string)
   {
   }

   structureSpec::~structureSpec() {
   }   