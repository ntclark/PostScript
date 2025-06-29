/*
Copyright 2025 EnVisioNate LLC

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the �Software�), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is the MIT License
*/

#pragma once

#include <stdio.h>
#include <stdint.h>

#include "PostScript objects/object.h"

    class file : public object {
    public:

        file(job *pJob,char *pszName);

        virtual uint8_t *getBinaryData(uint32_t *pcbSize,char *pszEndDelimiter);

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