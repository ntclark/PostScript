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

//
// On entry, pStart is positioned just past the beginning delimiter
//
// However, on return *ppEnd points TO the delimiter. This means the 
// caller needs to advance ppEnd after this call depending on the
// delimiter passed in (1 or 2 characters)

    void job::parse(char *pszBeginDelimiter,char *pszEndDelimiter,char *pStart,char **ppEnd) {

    long n = 0;
    char *p = pStart;
    char *pBegin = pStart;
    bool twoCharDelimiter = ! ('\0' == pszEndDelimiter[1]);
    char *pClear[64];
    memset(pClear,0,64 * sizeof(char *));
    long clearCount = 0L;

    if ( twoCharDelimiter ) {

        while ( *p && ( 0 < n || ! ( *p == pszEndDelimiter[0] ) || ! ( *(p + 1) == pszEndDelimiter[1] ) ) ) {

            if ( 0x0A == *p || 0x0D == *p ) {
                while ( *p && ( 0x0A == *p || 0x0D == *p ) )
                    p++;
                if ( '\0' == *p )
                    break;
                if ( 0 < inputLineNumberSize ) {
                    sscanf(p,szLNFormat,&inputLineNumber);
                    p += inputLineNumberSize;
                }
                if ( DSC_DELIMITER[0] == *p && DSC_DELIMITER[1] == *(p + 1) ) {
                    pClear[clearCount] = p;
                    parseDSC(p + 2,&p);
                    memset(pClear[clearCount],' ',p - pClear[clearCount]);
                    clearCount++;
                    continue;
                }
                if ( COMMENT_DELIMITER[0] == *p ) {
                    pClear[clearCount] = p;
                    parseComment(p + 1,&p);
                    memset(pClear[clearCount],' ',p - pClear[clearCount]);
                    clearCount++;
                    continue;
                }
                p--;
            }

            if ( *p == pszBeginDelimiter[0] && *(p + 1) == pszBeginDelimiter[1] )
                n++;
            else if ( *p == pszEndDelimiter[0] && *(p + 1) == pszEndDelimiter[1] )
                n--;

            p++;

        }

    } else {

        while ( *p && ( 0 < n || ! ( *p == pszEndDelimiter[0] ) ) ) {

            if ( 0x0A == *p || 0x0D == *p ) {
                while ( *p && ( 0x0A == *p || 0x0D == *p ) )
                    p++;
                if ( '\0' == *p )
                    break;
                if ( 0 < inputLineNumberSize ) {
                    sscanf(p,szLNFormat,&inputLineNumber);
                    p += inputLineNumberSize;
                }
                if ( DSC_DELIMITER[0] == *p && DSC_DELIMITER[1] == *(p + 1) ) {
                    pClear[clearCount] = p;
                    parseDSC(p + 2,&p);
                    memset(pClear[clearCount],' ',p - pClear[clearCount]);
                    clearCount++;
                    continue;
                }
                if ( COMMENT_DELIMITER[0] == *p ) {
                    pClear[clearCount] = p;
                    parseComment(p + 1,&p);
                    memset(pClear[clearCount],' ',p - pClear[clearCount]);
                    clearCount++;
                    continue;
                }
                p--;
            }

            if ( *p == pszBeginDelimiter[0] )
                n++;
            else if ( *p == pszEndDelimiter[0] )
                n--;

            if ( *p == '\\' ) 
                p++;

            p++;

        }

    }

    *ppEnd = p;

    long totalMotion = 0;

    for ( long k = 0; k < clearCount; k++ ) {

        pClear[k] -= totalMotion;

        char *pSource = pClear[k];

        long cntSpaces = 0L;
        while ( ' ' == *pSource ) {
            pSource++;
            cntSpaces++;
        }

        long n = (long)((char *)(*ppEnd + 1) - pSource);

        char *pNew = new char[n];

        memcpy(pNew,pSource,n);

        memcpy(pClear[k],pNew,n);

        delete [] pNew;

        pNew = pClear[k] + n;
        while ( pNew <= *ppEnd ) {
            *pNew = ' ';
            pNew++;
        }

        totalMotion += cntSpaces;

    }

    *ppEnd -= totalMotion;

    return;
    }


    void job::parseBinary(char *pszEndDelimiter,char *pStart,char **ppEnd) {

    char *p = pStart;
    bool twoCharDelimiter = false;

    if ( ! ( NULL == pszEndDelimiter ) ) 
        twoCharDelimiter = ! ('\0' == pszEndDelimiter[1]);

    if ( twoCharDelimiter )

        while ( *p && ( ! ( *p == pszEndDelimiter[0] ) || ! ( *(p + 1) == pszEndDelimiter[1] ) ) ) 
            p++;

    else if ( 0 == strcmp(pszEndDelimiter,"cleartomark") ) {

        while ( *p ) { 
            if ( 0 == strcmp(p,"cleartomark") )
                break;
            p++;
        }

    } else

        while ( *p && ( ! ( *p == pszEndDelimiter[0] ) ) ) 
            p++;

    *ppEnd = p;

    return;
    }


    void job::parseComment(char *pStart,char **ppEnd) {
    char *p = pStart;
    ADVANCE_THRU_END_OF_LINE(p)
    *ppEnd = p;
    pComments -> push_back(new (CurrentObjectHeap()) comment(this,pStart,*ppEnd));
    return;
    }


    void job::parseDSC(char *pStart,char **ppEnd) {
    char *p = pStart;
    ADVANCE_THRU_END_OF_LINE(p)
    *ppEnd = p;
    pDSCItems -> push_back(new (CurrentObjectHeap()) dscItem(this,pStart,*ppEnd));
    return;
    }


    void job::parseHexString(char *pStart,char **ppEnd) {

    *ppEnd = NULL;

    parse(HEX_STRING_DELIMITER_BEGIN,HEX_STRING_DELIMITER_END,pStart,ppEnd);

    if ( ! *ppEnd ) 
        return;

    char *p = pStart;
    char c = **ppEnd;
    **ppEnd = '\0';

    long n = (DWORD)strlen((char *)p) + 1;
    long k = 0; 
    long cbString = 0;

    while ( k < n ) {
        if ( 0x0D == p[k] || 0x0A == p[k] ) {
            while ( 0x0D == p[k] || 0x0A == p[k] )
                k++;
            if ( 0 < inputLineNumberSize )
                k += inputLineNumberSize;
            continue;
        }
        cbString++;
        k++;
    }

    n = cbString;
    char *pszNew = new char[n];
    memset(pszNew,0,n * sizeof(char));
    k = 0; 
    cbString = 0;

    while ( k < n ) {
        if ( 0x0D == p[k] || 0x0A == p[k] ) {
            while ( 0x0D == p[k] || 0x0A == p[k] )
                k++;
            if ( 0 < inputLineNumberSize )
                k += inputLineNumberSize;
            continue;
        }
        pszNew[cbString++] = p[k];
        k++;
    }

    cbString--;

    DWORD dwFlags = 0L;
    DWORD dwCount = 0L;

    CryptStringToBinaryA(pszNew,cbString,CRYPT_STRING_HEX,NULL,&dwCount,NULL,&dwFlags);

    BYTE *pbDecoded = new BYTE[dwCount];

    memset(pbDecoded,0,dwCount * sizeof(BYTE));

    CryptStringToBinaryA(pszNew,cbString,CRYPT_STRING_HEX,pbDecoded,&dwCount,NULL,&dwFlags);

    push(new (CurrentObjectHeap()) binaryString(this,"<binary>",pbDecoded,dwCount));

    delete [] pbDecoded;
    delete [] pszNew;

    **ppEnd = c;

    *ppEnd += 1;

    return;
    }


    void job::parseHex85String(char *pStart,char **pEnd) {
MessageBox(NULL,"Not implemented","Not implemented",MB_OK);
    return;
    }


    void job::parseLiteralName(char *pStart,char **ppEnd) {
    char *p = pStart;
    ADVANCE_THRU_WHITE_SPACE(p)
    char *pBegin = p;
    ADVANCE_TO_END(p)
    push(new (CurrentObjectHeap()) literal(this,pBegin,p));
    ADVANCE_THRU_WHITE_SPACE(p)
    *ppEnd = p;
    return;
    }


    char * job::parseObject(char *pStart,char **pEnd) {
    static long wheelIndex = -1L;
    static char result[32][MAX_PATH];

    wheelIndex++;
    if ( 32 == wheelIndex )
        wheelIndex = 0;

    memset(result[wheelIndex],0,MAX_PATH * sizeof(char));
    char *p = pStart;
    ADVANCE_THRU_WHITE_SPACE(p)
    char *pBegin = p;
    p++;
    ADVANCE_TO_END(p)
    *pEnd = p;
    strncpy(result[wheelIndex],(char *)pBegin,p - pBegin);
    return result[wheelIndex];
    }


    void job::parseResolveNowString(char *pStart,char **ppEnd) {
    *ppEnd = NULL;
    parseObject(pStart,ppEnd);
    push(new (CurrentObjectHeap()) resolveNowString(this,pStart,*ppEnd));
    push(resolve(pop() -> Name()));
    return;
    }


    void job::parseString(char *pStart,char **ppEnd) {

    char *p = pStart;
    *ppEnd = NULL;

    parse(STRING_DELIMITER_BEGIN,STRING_DELIMITER_END,p,ppEnd);

    if ( *ppEnd ) {
        push(new (CurrentObjectHeap()) constantString(this,p,*ppEnd));
        *ppEnd = *ppEnd + 1;
    }

    return;
    }