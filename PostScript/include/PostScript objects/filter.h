#pragma once

#include <stdint.h>
#include "PostScript Objects/file.h"

    class filter : public file {
    public:

        filter(job *pj,char *pszFilterName,object *pDataSource,char *pszEndDelimiter);
        virtual ~filter();

        void setSource(object *pFilterSource);

        virtual uint8_t *getBinaryData(DWORD *pcbSize,char *pszEndDelimiter);

    private:

        object *pDataSource{NULL};
        char szSourceEndDelimiter[8];
        char szFilterName[64];
    };
