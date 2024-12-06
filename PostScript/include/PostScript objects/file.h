#pragma once

#include <stdio.h>
#include <stdint.h>

#include "PostScript objects/object.h"

    class file : public object {
    public:

        file(job *pJob,char *pszName);

        virtual ~file();

        virtual uint8_t *getBinaryData(DWORD *pcbSize,char *pszEndDelimiter);

        virtual void releaseData() { 
            if ( ! ( NULL == pbData ) ) 
                delete [] pbData; 
        }

    protected:

        uint8_t *pbData{NULL};

    private:

        FILE *fileHandle{NULL};
        char szFileName[MAX_PATH];
        long currentSeek{0L};

        friend class job;
    };