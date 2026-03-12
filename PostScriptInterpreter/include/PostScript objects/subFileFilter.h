/*
Copyright 2025 EnVisioNate LLC

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the “Software”), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is the MIT License
*/

#pragma once

#include <stdint.h>
#include "PostScript Objects/filter.h"

    class subFileFilter : public filter {
    public:

        // SubFileFilter may not have the same file based input (I think)
        subFileFilter(job *pj,object *pDataSource,object *pString,object *pCount,object *pDictionary,boolean isLanguageLevel3);

        virtual uint8_t *getBinaryData(uint32_t *pcbSize);
        virtual void operatorReadstring(object *pString);

    private:

        boolean isLanguageLevel3{false};

        uint32_t byteCount{0};
        uint32_t currentByte{0};
        object *pFilterString{NULL};

    };
