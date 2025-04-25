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
    void job::parse(char *pszBeginDelimiter,char *pszEndDelimiter,char *pStart,char **ppEnd,long *pLineNumber) {

    long n = 0;
    char *p = pStart;
    char *pBegin = pStart;
    bool twoCharDelimiter = ! ('\0' == pszEndDelimiter[1]);
    char *pClear[64];
    memset(pClear,0,64 * sizeof(char *));
    long clearCount = 0L;

    if ( twoCharDelimiter ) {

        while ( *p && ( n || ! ( *p == pszEndDelimiter[0] ) || ! ( *(p + 1) == pszEndDelimiter[1] ) ) ) {

            if ( 0x0A == *p || 0x0D == *p ) {
                while ( *p && ( 0x0A == *p || 0x0D == *p ) )
                    p++;
                if ( '\0' == *p )
                    break;
                if ( DSC_DELIMITER[0] == *p && DSC_DELIMITER[1] == *(p + 1) ) {
                    pClear[clearCount] = p;
                    parseDSC(p + 2,&p,pLineNumber);
                    memset(pClear[clearCount],' ',p - pClear[clearCount]);
                    clearCount++;
                    continue;
                }
                if ( COMMENT_DELIMITER[0] == *p ) {
                    pClear[clearCount] = p;
                    parseComment(p + 1,&p,pLineNumber);
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

        while ( *p && ( n || ! ( *p == pszEndDelimiter[0] ) ) ) {

            if ( 0x0A == *p || 0x0D == *p ) {
                long count[]{0,0};
                while ( *p && ( 0x0A == *p || 0x0D == *p ) ) {
                    count[0x0A == *p ? 0 : 1]++;
                    p++;
                }
                *pLineNumber += max(count[0],count[1]);
                if ( '\0' == *p )
                    break;
                if ( DSC_DELIMITER[0] == *p && DSC_DELIMITER[1] == *(p + 1) ) {
                    pClear[clearCount] = p;
                    parseDSC(p + 2,&p,pLineNumber);
                    memset(pClear[clearCount],' ',p - pClear[clearCount]);
                    clearCount++;
                    continue;
                }
                if ( COMMENT_DELIMITER[0] == *p ) {
                    pClear[clearCount] = p;
                    parseComment(p + 1,&p,pLineNumber);
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