
#include "job.h"
#include "PostScript objects\constantString.h"

    constantString::constantString(job *pJob,char *pStart,char *pEnd) : 
        string(pJob,pStart,pEnd,object::valueType::constantString) { }

    constantString::~constantString() {
    return;
    }

