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

#include "job.h"
#include "PostScript objects/lzwFilter.h"

    /*
    LZWDecode Filter (page 131)
        source /LZWDecode filter
        source dictionary /LZWDecode filter
    
    The LZWDecode filter decodes data that is encoded in a Lempel-Ziv-Welch compressed format. 
    See the description of the LZWEncode filter for details of the format and a 
    description of the filter parameters. 
    */

    lzwFilter::lzwFilter(job *pj,filter *pDataSource) :
        filter(pj,pszKindLZW,pDataSource) 
    { 
    }

    uint8_t *lzwFilter::getBinaryData(uint32_t *pcbSize) {

    if ( NULL == pDataSource )
        return NULL;

    uint32_t cbSource;

    uint8_t *pbInput = pDataSource -> getBinaryData(&cbSource);

    }