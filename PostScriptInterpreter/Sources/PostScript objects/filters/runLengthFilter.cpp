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
#include "PostScript objects/runLengthFilter.h"

    /*
    (Page 86)
    RunLengthDecode (none) Decompresses data encoded in the run-length encoding format, producing the original data. 
    */

    runLengthFilter::runLengthFilter(job *pj,filter *pDataSource) :
        filter(pj,pszKindRunLength,pDataSource) 
    {}


    uint8_t *runLengthFilter::getBinaryData(uint32_t *pcbSize) {

    if ( NULL == pDataSource )
        return NULL;

    uint32_t cbSource;

    uint8_t *pbSource = pDataSource -> getBinaryData(&cbSource);

    /*
    3.3.4 RunLengthDecode Filter

    The RunLengthDecode filter decodes data that has been encoded in a simple
    byte-oriented format based on run length. The encoded data is a sequence of
    runs, where each run consists of a length byte followed by 1 to 128 bytes of data.

    If the length byte is in the range 0 to 127, the following length + 1 (1 to 128) bytes
    are copied literally during decompression. 

    If length is in the range 129 to 255, the following single byte is to be copied 
    257 - length (2 to 128) times during decompression. 

    A length value of 128 denotes EOD

    The compression achieved by run-length encoding depends on the input data. In
    the best case (all zeros), a compression of approximately 64:1 is achieved for long
    files. The worst case (the hexadecimal sequence 00 alternating with FF) results in
    an expansion of 127:128.

    */

    long cbTotal = 0;

    uint8_t *pbNext = pbSource;
    uint8_t *pbEnd = pbSource + cbSource;

    do {

        uint8_t runLength = *pbNext;

        if ( 128 > runLength ) {
            cbTotal += runLength + 1;
            pbNext += runLength + 2;
        } else if ( 128 < runLength ) {
            cbTotal += 257 - runLength;
            pbNext += 2;
        } else if ( 128 == runLength )
            break;

    } while ( pbNext < pbEnd );

    *pcbSize = cbTotal;

    pbData = new uint8_t[cbTotal];

    memset(pbData,0,cbTotal);

    pbNext = pbSource;

    uint8_t *pbTarget = pbData;

    do {

        uint8_t runLength = *pbNext;

        if ( 128 > runLength ) {
            memcpy(pbTarget,pbNext + 1,runLength + 1);
            pbNext += runLength + 2;
            pbTarget += runLength + 1;
        } else if ( 128 < runLength ) {
            runLength = 257 - runLength;
            for ( long k = 0; k < runLength; k++ )
                pbTarget[k] = *(pbNext + 1);
            pbNext += 2;
            pbTarget += runLength;
        } else if ( 128 == runLength )
            break;

    } while ( pbNext < pbEnd );

    pDataSource -> releaseData();

    return pbData;
    }