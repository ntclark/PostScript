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

#include "PostScript objects/object.h"

   class comment : public object {
   public:

      comment(job *pj,char *ps,char *pe) : 
            object(pj,ps,pe,object::objectType::comment,
                object::valueType::valueTypeUnspecified,object::valueClassification::simple) {
            pszValue = (char *)allocate((pe - ps + 1) * sizeof(char));
            char *p = pszValue + (pe - ps);
            *p = '\0';
            strncpy(pszValue,ps,(pe - ps));
            p--;
            while ( 0x0A == *p || 0x0D == *p )
                p--;
            *(p + 1) = '\0';
        }

      ~comment() {};

    private:

        char *pszValue{NULL};
   };
   