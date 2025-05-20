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

    void AdobeType1Fonts::executeEExec(file *pFileObject) {

    uint32_t cbData = 0;

    uint8_t *pbData = pFileObject -> getBinaryData(&cbData,"cleartomark");

    uint8_t *pbTemp = new uint8_t[cbData];NULL;
    memset(pbTemp,0,cbData * sizeof(uint8_t));

    uint16_t cntTemp = 0;
    uint8_t *p = pbData;
    while ( *p ) {

        if ( isspace(*p) ) {
            p++;
            continue;
        }

        char c[]{(char)*p,(char)*(p + 1)};

        for ( long k = 0; k < 2; k++ ) {
            if ( c[k] >= 'A' && c[k] <= 'F' )
                c[k] = c[k] - 'A' + 10;
            else if ( c[k] >= 'a' && c[k] <= 'f' )
                c[k] = c[k] - 'a' + 10;
            else if ( c[k] >= '0' && c[k] <= '9' )
                c[k] = c[k] - '0';
        }

        c[0] = (c[0] << 4) + c[1];
        pbTemp[cntTemp++] = c[0];

        p += 2;

    }

    uint8_t *pbTemp2 = NULL;

    cbData = decryptType1Data(55665,pbTemp,cntTemp,4,&pbTemp2);

    delete [] pbTemp;

    pFileObject -> releaseData();

/*
    When eexec begins operation, it performs a begin operation on
    systemdict to ensure that the operators that follow will be taken
    from systemdict.
*/
    pJob -> push(pJob -> SystemDict());
    pJob -> operatorBegin();

    char szEmbeddedFile[MAX_PATH];
    sprintf_s<MAX_PATH>(szEmbeddedFile,"%s:%ld",pJob -> CurrentSourceFileName(),pJob -> ExecutionStackSize());

    pJob -> execute((char *)pbTemp2,(char *)(pbTemp2 + cbData),szEmbeddedFile);

    delete [] pbTemp2;

/*
    When eexec terminates, it automatically performs an end operation to remove 
    the systemdict that it begins here.
*/
    pJob -> operatorEnd();

    return;
    }


    uint32_t AdobeType1Fonts::decryptType1CharString(uint8_t *pbData,uint32_t cbData,uint8_t countRandomBytes,type1Token *pTokens[]) {

    uint8_t *pbTemp = NULL;

    cbData = decryptType1Data(4330,pbData,cbData,countRandomBytes,&pbTemp);

/*
    6.2 Charstring Number Encoding

    A charstring byte containing the values from 32 through 255
    inclusive indicates an integer. These values are decoded in four
    ranges.
*/

    uint32_t countTokens = 0;

    int32_t val;
    uint32_t k = 0;
    while ( k < cbData ) {

        byte b = pbTemp[k];

        if ( 31 < b ) {
/*
    1. A charstring byte containing a value, v, between 32 and 246
        inclusive, indicates the integer v - 139. Thus, the integer values
        from -107 through 107 inclusive may be encoded in a single
        byte.
*/
            if ( b < 247 ) 
                val = b - 139;
/*
    2. A charstring byte containing a value, v, between 247 and 250
        inclusive, indicates an integer involving the next byte, w,
        according to the formula:
            [(v - 247) × 256] + w + 108
        Thus, the integer values between 108 and 1131 inclusive can
        be encoded in 2 bytes in this manner.
*/
            else if ( 246 < b && b < 251 ) {
                k++;
                val = ( b - 247 ) * 256 + pbTemp[k] + 108;
/*
    3. A charstring byte containing a value, v, between 251 and 254
        inclusive, indicates an integer involving the next byte, w,
        according to the formula:
            - [(v - 251) × 256] - w - 108
        Thus, the integer values between -1131 and -108 inclusive can
        be encoded in 2 bytes in this manner.
*/
            } else if ( 250 < b && b < 255 ) {
                k++;
                val = -(( b - 251 ) * 256) - pbTemp[k] - 108;
/*
    4. Finally, if the charstring byte contains the value 255, the next
        four bytes indicate a two’s complement signed integer. The
        first of these four bytes contains the highest order bits, the
        second byte contains the next higher order bits and the fourth
        byte contains the lowest order bits. Thus, any 32-bit signed
        integer may be encoded in 5 bytes in this manner (the 255 byte
        plus 4 more bytes).

    !!!!!!!!!!!!

    Note 3 The Type 2 interpretation of a number encoded in five-bytes (those
    with an initial byte value of 255) differs from how it is interpreted in
    the Type 1 format.
*/
            } else {
                uint32_t uval =  (uint32_t) pbTemp[k + 1] << 24;
                uval |= (uint32_t) pbTemp[k + 2] << 16;
                uval |= (uint32_t) pbTemp[k + 3] <<  8;
                uval |= (uint32_t) pbTemp[k + 4] <<  0;
                val = (int32_t) uval;
                k += 4;
            }

            k++;

            if ( ! ( NULL == pTokens ) )
                pTokens[countTokens] = new type1Token(type1Token::tokenKind::isNumber,val);
            countTokens++;

            continue;

        }

/*
    6.3 Charstring Command Encoding

    Charstring commands are encoded in 1 or 2 bytes.
    Single byte commands are encoded in 1 byte that contains a
    value between 0 and 31 inclusive. Not all possible command
    encoding values are listed. The command values that are omitted
    are special purpose commands that are not used in any downloadable
    Type 1 font program, or they are reserved.

    If a command byte contains the value 12, then the value in the
    next byte indicates a command. This "escape" mechanism allows
    many extra commands to be encoded. These 2-byte commands
    are not used as often as the 1-byte commands; this encoding
    technique helps to minimize the length of charstrings. Refer to
    Appendix 2 for a summary of the charstring commands and their
    encoding values.

    Appendix 2 is in the document "Adobe Type 1 Font Format"
        Copyright © 1990 Adobe Systems Incorporated.
        page 89
*/
        switch ( b ) {
        case 12: {
            k++;
            b = pbTemp[k];
            if ( ! ( NULL == pTokens ) ) {
                if ( type1CharStringSubCommands.end() == type1CharStringSubCommands.find(b) ) 
                    pTokens[countTokens] = new type1Token(type1Token::tokenKind::isCommand,type1CharStringSubCommands[99]);
                else
                    pTokens[countTokens] = new type1Token(type1Token::tokenKind::isCommand,type1CharStringSubCommands[b]);
            }
            countTokens++;
            }
            break;

        case 28: {
            k++;
            val = (pbTemp[k] & 0xff) << 8;
            k++;
            val |= (pbTemp[k] & 0xff);
            if ( val & 0x8000 )
                val |= ~0x7FFF;
            if ( ! ( NULL == pTokens ) )
                pTokens[countTokens] = new type1Token(type1Token::tokenKind::isNumber,val);
            countTokens++;
            }
            break;

        default:
            if ( ! ( NULL == pTokens ) ) {
                if ( type1CharStringCommands.end() == type1CharStringCommands.find(b) ) 
                    pTokens[countTokens] = new type1Token(type1Token::tokenKind::isCommand,type1CharStringCommands[99]);
                else
                    pTokens[countTokens] = new type1Token(type1Token::tokenKind::isCommand,type1CharStringCommands[b]);
            }
            countTokens++;
            break;
        }

        k++;

    }

    delete [] pbTemp;

    return countTokens;
    }


    uint32_t AdobeType1Fonts::decryptType1Data(uint16_t initialKey,uint8_t *pbData,uint32_t cbData,uint8_t countRandomBytes,uint8_t **ppbResults) {

    *ppbResults = new uint8_t[cbData];

    memset(*ppbResults,0,cbData * sizeof(uint8_t));

    /*
    To Decrypt a Sequence of Ciphertext Bytes to Produce the
    Original Sequence of Plaintext Bytes

        1. Initialize an unsigned 16-bit integer variable R to the encryption 
            key (the same key as used to encrypt).
    */

    uint16_t r = initialKey;
    uint16_t c1 = 52845;
    uint16_t c2 = 22719;
/*
        3. Discard the first n bytes of plaintext; these are the random
        bytes added during encryption. The remainder of the plaintext
        bytes are the original sequence.
*/
    int16_t index = -countRandomBytes - 1;

    for ( uint32_t k = 0; k < cbData; k++ ) {

        /*
        2. For each 8-bit byte, C, of ciphertext the following steps are executed:
            a. Assign the high order 8 bits of R to a temporary variable, T.
            b. Exclusive-OR C with T, producing a plaintext byte, P.
            c. Compute the next value of R by the formula ((C + R) × c1 + c2) mod 65536, 
                where c1 and c2 are the same constants that were used to encrypt.
        */

        unsigned char cipher = pbData[k];
        unsigned char plain = (cipher ^ (r >> 8));
        r = ((cipher + r) * c1 + c2) & 0xFFFF;

        index++;
        if ( -1 < index )
            (*ppbResults)[index] = plain;

    }

    return index + 1;
    }