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


    void trans_func( unsigned int u, EXCEPTION_POINTERS* ) {
    static char szException[1024];
    sprintf_s<1024>(szException,"A non postscript exception has occurred. Code: %04x",u);
    throw new nonPostscriptException(szException);
    return;
    }


    long job::execute(char *pBegin) {

    pStart = pBegin;
    char *p = pStart;
    pNext = p;
    char *pLogStart = NULL;

    _se_translator_function defaultExtranslator = _set_se_translator(trans_func);

    quitRequested = false;

    do {

        //ADVANCE_THRU_WHITE_SPACE(p)

        if ( '\0' == *p )
            break;

        if ( 0x0A == p[0] || 0x0D == p[0] ) {
            pNext = p;
            while ( 0x0A == *pNext || 0x0D == *pNext ) 
                pNext++;
            pPStoPDF -> queueLog(p,pNext,false);
            p = pNext;
            continue;
        }

        char *pCollectionDelimiter = collectionDelimiterPeek(p,&pNext);

        char *pDelimiter = NULL;

        if ( NULL == pCollectionDelimiter ) {
            pDelimiter = (char *)delimiterPeek(p,&pNext);
            if ( ! ( NULL == pDelimiter ) && COMMENT_DELIMITER[0] == pDelimiter[0] ) {
                parseComment(pNext,&pNext);
                pPStoPDF -> queueLog(p,pNext);
                p = pNext;
                continue;
            }
        }

        pPStoPDF -> queueLog(p,pNext);

        pLogStart = pNext;

        if ( ! ( NULL == pDelimiter ) ) {
            (this ->* tokenProcedures[std::hash<std::string>()((char *)pDelimiter)])(pNext,&pNext);
            pPStoPDF -> queueLog(pLogStart,pNext);
            ADVANCE_THRU_WHITE_SPACE(pNext)
            p = pNext;
            continue;
        }

        if ( ! ( NULL == pCollectionDelimiter ) && PROC_DELIMITER_BEGIN[0] == pCollectionDelimiter[0] ) {
            char *pProcedureEnd = NULL;
            parseProcedureString(pNext,&pProcedureEnd);
            pPStoPDF -> queueLog(pLogStart,pProcedureEnd);

            try {
            push(new (CurrentObjectHeap()) procedure(this,pNext,pProcedureEnd));
            } catch ( PStoPDFException *pe ) {
                char szMessage[1024];
                sprintf(szMessage,"\n\nA %s exception occurred: %s\n",pe -> ExceptionName(),pe -> Message());
                pPStoPDF -> queueLog(szMessage,NULL,true);
            }

            ADVANCE_THRU_WHITE_SPACE(pProcedureEnd)
            pNext = pProcedureEnd;
            p = pProcedureEnd;
            continue;
        }

        object *po = NULL;

        try {

        if ( ! ( NULL == pCollectionDelimiter ) ) {
            po = new (CurrentObjectHeap()) object(this,pCollectionDelimiter);
            push(po);
            resolve();
            po = top();
        } else {
            char *pObjectName = parseObject(pNext,&pNext);
            po = resolve(pObjectName);
            if ( NULL == po ) {
                push(new (CurrentObjectHeap()) object(this,pObjectName));
                po = top();
            } else {
                push(po);
            }
        }

        if ( object::procedure == po -> ObjectType() ) {
            pop();
            executeProcedure(reinterpret_cast<procedure *>(po));
        } else
            executeObject();

        } catch ( nonPostscriptException *ex ) {
            pPStoPDF -> queueLog(ex -> Message(),NULL,true);
        } catch ( PStoPDFException *pe ) {
            char szMessage[1024];
            sprintf(szMessage,"\n\nA %s exception occurred: %s\n",pe -> ExceptionName(),pe -> Message());
            //pPStoPDF -> queueLog(szMessage,NULL,true);
            //pPStoPDF -> queueLog("\nStack Trace:\n");
            //operatorPstack();
        }

        ADVANCE_THRU_WHITE_SPACE(pNext)

        p = pNext;

        if ( 0 == strncmp(pNext,"terminate",9) )
            break;

        pPStoPDF -> queueLog(pLogStart,pNext);

    } while ( p && ! quitRequested );

    pPStoPDF -> settleLog();

    _set_se_translator(defaultExtranslator);

    return 0;
    }


    void job::parseProcedure(procedure *pProcedure,char *pStart,char **ppEnd) {

    char *p = pStart;
    char *pNext = p;

    do {

        ADVANCE_THRU_WHITE_SPACE_AND_EOL(p)

        if ( '\0' == *p )
            break;

        char *pCollectionDelimiter = collectionDelimiterPeek(p,&pNext);

        char *pDelimiter = NULL;

        if ( NULL == pCollectionDelimiter )
            pDelimiter = (char *)delimiterPeek(p,&pNext);

        if ( ! ( NULL == pDelimiter ) ) {
            (this ->* tokenProcedures[std::hash<std::string>()((char *)pDelimiter)])(pNext,&pNext);
            if ( ! ( DSC_DELIMITER[0] == *pDelimiter ) && ! ( COMMENT_DELIMITER[0] == *pDelimiter ) )
                pProcedure -> insert(pop());
            p = pNext;
            continue;
        }

        if ( ! ( NULL == pCollectionDelimiter ) && PROC_DELIMITER_BEGIN[0] == *pCollectionDelimiter ) {
            char *pProcedureEnd = NULL;
            parseProcedureString(pNext,&pProcedureEnd);
            procedure *pInnerProcedure = new (CurrentObjectHeap()) procedure(this,pNext,pProcedureEnd);
            pInnerProcedure -> pContainingProcedure = pProcedure;
            pProcedure -> insert(pInnerProcedure);
            pNext = pProcedureEnd;
            p = pProcedureEnd;
            continue;
        }

        if ( ! ( NULL == pCollectionDelimiter ) && PROC_DELIMITER_END[0] == *pCollectionDelimiter ) {
            if ( ! ( NULL == ppEnd ) )
                *ppEnd = pNext;
            return;
        }

        object *po = NULL;

        if ( ! ( NULL == pCollectionDelimiter ) ) {
            po = new (CurrentObjectHeap()) object(this,pCollectionDelimiter);
            push(po);
            resolve();
            po = pop();
            p = pNext;
        } else 
            po = new (CurrentObjectHeap()) object(this,parseObject(p,&p));

        pProcedure -> insert(po);

    } while ( ! ( '\0' == p ) );

    if ( ! ( NULL == ppEnd ) )
        *ppEnd = NULL;

    return;
    }
