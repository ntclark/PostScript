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
#include "PostScript objects/subFileFilter.h"

    subFileFilter::subFileFilter(job *pj,object *,object *pfs,object *pfc,object *pfd,boolean isLL3) :
        isLanguageLevel3(isLL3),
        filter(pj,pszKindSubFile,NULL) 
    { 

        if ( ! ( NULL == pfd ) ) {
            dictionary *p = reinterpret_cast<dictionary *>(pfd);
            pFilterString = p -> retrieve("EODString");
            byteCount = p -> retrieve("EODCount") -> IntValue();
        } else {
            pFilterString = pfs;
            byteCount = pfc -> IntValue();
        }

        getBinaryData(NULL);
    }


    void subFileFilter::operatorReadstring(object *ps) {

    class string *pString = reinterpret_cast<class string *>(ps);

    if ( byteCount <= currentByte ) {
        pString -> Contents("");
        pJob -> push(pString);
        pJob -> push(pJob -> pFalseConstant);
        return;
    }

    uint32_t requestedSize = (uint32_t)pString -> length();
    uint32_t availableSize = byteCount - currentByte;
    uint32_t fillSize = min(requestedSize,availableSize);

    pString -> Contents(&pbData[currentByte],fillSize);

    pJob -> push(pString);

    currentByte += fillSize;

    if ( fillSize <= requestedSize )
        pJob -> push(pJob -> pTrueConstant);
    else
        pJob -> push(pJob -> pFalseConstant);

    return;
    }

extern "C" int LZWDecodeFile(FILE *fpIn, FILE *fpOut);

    uint8_t *subFileFilter::getBinaryData(uint32_t *pcbData) {

    // THIS is how software needs to be commented. Do it this way, or not at all

    // Even so, a  ghostscript generated test PS file completely violates this documentation
    // It seems to be passing the # of bytes in the subfile as EODCount, and "" as EODString

    /*

    PostScript Language Reference Third Edition page(151)

    SubFileDecode Filter
        source EODCount EODString /SubFileDecode filter
        source dictionary EODCount EODString /SubFileDecode filter
        source dictionary /SubFileDecode filter (LanguageLevel 3)

    The SubFileDecode filter does not perform data transformation, but it can detect
    an EOD condition. Its output is always identical to its input, up to the point
    where EOD occurs. The data preceding the EOD is called a subfile of the underlying data source. 

    The SubFileDecode filter can be used in a variety of ways:

        • A subfile can contain data that should be read or executed conditionally, depending
        on information that is not known until execution. If a program decides to ignore 
        the information in a subfile, it can easily skip to the end of the subfile by 
        invoking flushfile on the filter file.

        • Subfiles can help recover from errors that occur in encapsulated programs. If
        the encapsulated program is treated as a subfile, the enclosing program can 
        regain control if an error occurs, flush to the end of the subfile, and 
        resume execution from the underlying data source. The application, 
        not the PostScript interpreter, must provide such error handling; it is 
        not the default error handling provided by the PostScript interpreter.

        • The SubFileDecode filter enables an arbitrary data source (procedure or string)
        to be treated as an input file. This use of subfiles does not require detection of
        an EOD marker.

    The SubFileDecode filter requires two parameters, EODCount and EODString,
    which specify the condition under which the filter is to recognize EOD. The filter
    will allow data to pass through the filter until it has encountered exactly
    EODCount instances of the EODString; then it will reach EOD.

    In LanguageLevel 2, EODCount and EODString are specified as operands on the
    stack. In LanguageLevel 3, they may alternatively be specified in the
    SubFileDecode parameter dictionary (as shown in Table 3.23). They must be
    specified in the parameter dictionary if the SubFileDecode filter is used as one of
    the filters in a ReusableStreamDecode filter (described in the next section). 

    TABLE 3.23 Entries in a SubFileDecode parameter dictionary (LanguageLevel 3)

        KEY         TYPE            VALUE
        EODCount    integer         (Required) The number of occurrences of EODString 
                                    that will be passed through the filter and made 
                                    available for reading.

        EODString   string          (Required) The end-of-data string.

        CloseSource boolean         (Optional) A flag specifying whether closing the 
                                    filter should  also close its data source. 
                                    Default value: false.

    EODCount must be a nonnegative integer. If it is greater than 0, all input data up
    to and including that many occurrences of EODString will be passed through the
    filter and made available for reading. If EODCount is 0, the first occurrence of 
    EODString will be consumed by the filter, but it will not be passed through the filter.

    EODString is ordinarily a string of nonzero length. It is compared with successive
    subsequences of the data read from the data source. This comparison is based on
    equality of 8-bit character codes, so matching is case-sensitive. Each occurrence
    of EODString in the data is counted once. Overlapping instances of EODString will
    not be recognized. For example, an EODString of eee will be recognized only once
    in the input XeeeeX.

    EODString may also be of length 0, in which case the SubFileDecode filter will
    simply pass EODCount bytes of arbitrary data. This is dependable only for binary
    data, when suitable precautions have been taken to protect the data from any
    modification by communication channels or operating systems. Ordinary ASCII
    text is subject to modifications such as translation between different end-of-line
    conventions, which can change the byte count in unpredictable ways.

    A recommended value for EODString is a document structuring comment, such as
    %%EndBinary. Including newline characters in EODString is not recommended;
    translating the data between different end-of-line conventions could subvert the
    string comparisons.

    If EODCount is 0 and EODString is of length 0, detection of EOD markers is disabled; 
    the filter will not reach EOD. This is useful primarily when using procedures or 
    strings as data sources. EODCount is not allowed to be negative.

    */

    // NTC: 03-02-2026
    // !!!! My test postscript source used to develop this class/method includes 
    // a large # as EODCount and an empty string as EODString
    // That test was created with ghostscript by converting Test Page.pdf to Test Page.ps
    // I assume that the large # is the number of bytes in the subfile

    // But I sure would like to find an example that does it properly (i.e., as documented)

    pJob -> currentInputFlushSpace();

    char *p = pJob -> currentInput();
    while ( 0x0A == *p || 0x0D == *p )
        p++;

    char *pEnd = NULL;

    if ( 0 == pJob -> inputLineNumberSize ) {
        pEnd = p + byteCount;
        if ( pEnd > pJob -> pStorageEnd ) {
            char szMessage[128];
            sprintf_s<128>(szMessage,"readstring: Input line %ld: A subFileFilter specifies more data than is available",pJob -> inputLineNumber);
            throw new rangecheck(szMessage);
            return NULL;
        }
        pbData = new uint8_t[byteCount];
        memcpy(pbData,p,byteCount);
        pJob -> setCurrentInput(pEnd);
        return pbData;
    }

    p += pJob -> inputLineNumberSize;

    pbData = new uint8_t[byteCount];

    uint32_t countRead = 0;

    while ( countRead < byteCount ) {
        if ( p > pJob -> pStorageEnd ) {
            delete [] pbData;
            pbData = NULL;
            char szMessage[128];
            sprintf_s<128>(szMessage,"readstring: Input line %ld: A subFileFilter specifies more data than is available",pJob -> inputLineNumber);
            throw new rangecheck(szMessage);
            return NULL;
        }
        if ( 0x0A == *p || 0x0D == *p ) {
            while ( 0x0A == *p || 0x0D == *p )
                pbData[countRead] = *p++;
            p += pJob -> inputLineNumberSize;
            countRead++;
            continue;
        }
        pbData[countRead++] = *p++;
    }

    pJob -> setCurrentInput(p);

int *pbx = new int[byteCount];

for ( long k = 0; k < byteCount; k++ )
pbx[k] = pbData[k];

void decoding(int *pbx,uint32_t cbData);

decoding(pbx,byteCount);

    return pbData;
    }