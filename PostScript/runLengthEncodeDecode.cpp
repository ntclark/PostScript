
#include <stdint.h>
#include <memory.h>
#include <Windows.h>

    int32_t runLengthDecode(uint8_t *pbInput, int32_t cbInput, uint8_t **ppbOutput) {

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

    uint8_t *pbNext = pbInput;
    uint8_t *pbEnd = pbInput + cbInput;

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

    *ppbOutput = new uint8_t[cbTotal];

    memset(*ppbOutput,0,cbTotal);

    pbNext = pbInput;

    uint8_t *pbTarget = *ppbOutput;

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

    return cbTotal;
    }
