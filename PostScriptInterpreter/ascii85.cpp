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

#include "PostScript.h"

    uint32_t decodeASCII85(uint8_t *pbInput,uint32_t cbSource,uint8_t **ppbResult) {

    uint8_t *pbStart = new uint8_t[cbSource + 4];

    memset(pbStart,0,(cbSource + 4) * sizeof(uint8_t));

    std::vector<uint8_t *> toDelete;

    toDelete.push_back(pbStart);

    memcpy(pbStart,pbInput,cbSource * sizeof(uint8_t));

    if ( '<' == pbStart[0] && '~' == pbStart[1] ) {
        pbStart += 2;
        cbSource -= 2;
    }

    uint8_t *pbEnd = pbStart + cbSource;

    if ( '~' == pbEnd[-2] && '>' == pbEnd[-1] ) {
        pbEnd[-2] = '\0';
        pbEnd -= 2;
        cbSource -= 2;
    }

    while ( pbStart < pbEnd ) {
        if ( ( ' ' < *(pbEnd - 1) && *(pbEnd - 1) < 'v' ) || 'z' == *(pbEnd - 1) )
            break;
        pbEnd--;
    }

    if ( pbStart == pbEnd ) {
        delete [] toDelete[0];
        return 0;
    }

/*
    The ASCII base-85 encoding uses the characters ! through u and the character z,
    with the 2-character sequence ~> as its EOD marker. The ASCII85Decode filter
    ignores all white-space characters (see Section 3.1, “Lexical Conventions”). Any
    other characters, and any character sequences that represent impossible combinations 
    in the ASCII base-85 encoding, will cause an error. 
*/

    uint8_t *p = pbStart;

    while ( p < pbEnd ) {

        if ( ' ' < *p && *p < 'v' ) {
            *p -= '!';
            p++;
            continue;
        }

        uint8_t *pbZ = p;

        if ( ! ( 'z' == *pbZ ) ) {
            for ( int32_t k = 0; k < toDelete.size(); k++ )
                delete [] toDelete[k];
            return 0;
        }

        *pbZ = 0;

        uint32_t cbFirstHalf = (uint32_t)(pbZ - pbStart);
        uint32_t cbNextHalf = (uint32_t)(pbEnd - pbZ - 1);

        uint8_t *pbReplacement = new uint8_t[cbFirstHalf + cbNextHalf + 5 + 4];

        toDelete.push_back(pbReplacement);

        memcpy(pbReplacement,pbStart,cbFirstHalf);

        pbReplacement[cbFirstHalf + 0] = 0;
        pbReplacement[cbFirstHalf + 1] = 0;
        pbReplacement[cbFirstHalf + 2] = 0;
        pbReplacement[cbFirstHalf + 3] = 0;
        pbReplacement[cbFirstHalf + 4] = 0;

        memcpy(pbReplacement + cbFirstHalf + 5,pbZ + 1,cbNextHalf);

        pbEnd = pbReplacement + cbFirstHalf + cbNextHalf + 5;
        p = pbReplacement + cbFirstHalf + 5;

        pbStart = pbReplacement;

    }

    cbSource = (uint32_t)(pbEnd - pbStart);

    uint32_t padding = ( ( 5 - cbSource % 5 ) % 5 );

    for ( long k = 0; k < padding; k++ )
        pbEnd[k] = 'u' - '!';

    pbEnd += padding;
    cbSource += padding;

    p = pbStart;

    uint8_t *pbTemp = new uint8_t[cbSource];

    uint32_t outIndex = 0;

    while ( p < pbEnd ) {

        uint32_t groupValue = p[0];
        groupValue *= 85u;

        groupValue += p[1];
        groupValue *= 85u;

        groupValue += p[2];
        groupValue *= 85u;

        groupValue += p[3];
        groupValue *= 85u;

        groupValue += p[4];

        pbTemp[outIndex + 3] = groupValue % 256u;
        groupValue /= 256u;

        pbTemp[outIndex + 2] = groupValue % 256u;
        groupValue /= 256u;

        pbTemp[outIndex + 1] = groupValue % 256u;
        groupValue /= 256u;

        pbTemp[outIndex + 0] = (uint8_t )groupValue;

        outIndex += 4;
        p += 5;

    }

    *ppbResult = new uint8_t[outIndex];

    memcpy(*ppbResult,pbTemp,outIndex * sizeof(uint8_t));

    for ( int32_t k = 0; k < toDelete.size(); k++ )
        delete [] toDelete[k];

    toDelete.clear();

    delete [] pbTemp;

    return outIndex - padding;
    }