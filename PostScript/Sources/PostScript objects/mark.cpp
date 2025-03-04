
#include "PostScript objects\mark.h"

   mark::mark(job *pJob,object::valueType vt) :
      object(pJob,object::objectType::mark,vt,object::valueClassification::simple) {
    switch ( vt ) {
    case object::valueType::arrayMark:
        Name("[");
        Contents("[");
        break;
    default:
        Name("?");
        Contents("?");
    }
    return;
    }

   mark::~mark() { }