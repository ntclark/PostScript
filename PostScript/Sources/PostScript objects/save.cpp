
#include "PostScript objects\save.h"

   save::save(job *pJob) :
      object(pJob,NULL,object::objectType::objTypeSave,object::valueType::valueTypeUnspecified,object::valueClassification::composite)
   {
   return;
   }

   save::~save() {
   return;
   }
