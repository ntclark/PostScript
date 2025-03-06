
#pragma once

#include "array.h"

    class job;

    class colorSpace : public array {
    public:

        colorSpace(job *,object *);
        colorSpace(job *,char *pszName);
        ~colorSpace();

        void setColor();

        long ParameterCount();

        uint16_t HiVal() { return hiVal; }
        object *Lookup() { return pLookup; }

    private:

        object *pFamilyName{NULL};
        object *pBaseName{NULL};
        uint16_t hiVal{0L};
        object *pLookup{NULL};

        long parameterCount;

    };
