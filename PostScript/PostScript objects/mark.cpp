
#include "PostScript objects\mark.h"

   mark::mark(job *pJob,object::valueType vt) :
      object(pJob,object::objectType::mark,vt,object::valueClassification::simple) { }

   mark::~mark() { }