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


    long job::execute(char *pBegin,char *pszFileName) {

    executionStack.push(new executionLevel(pBegin,pszFileName));

    char *p = pBegin;
    char *pLogStart = NULL;
    long lineNumber = 0L;

    _se_translator_function defaultExTranslator = _set_se_translator(trans_func);

    do {

        if ( '\0' == *p )
            break;

        if ( 0x0A == p[0] || 0x0D == p[0] ) {
            executionStack.top() -> pNext = p;
            long count[]{0,0};
            while ( 0x0A == *executionStack.top() -> pNext || 0x0D == *executionStack.top() -> pNext ) {
                count[0x0A == *executionStack.top() -> pNext ? 0 : 1]++;
                executionStack.top() -> pNext++;
            }
            pPostScriptInterpreter -> queueLog(true,p,executionStack.top() -> pNext,false);
            p = executionStack.top() -> pNext;
            lineNumber += max(count[0],count[1]);
            continue;
        }

        char *pCollectionDelimiter = collectionDelimiterPeek(p,&executionStack.top() -> pNext);

        char *pDelimiter = NULL;

        if ( NULL == pCollectionDelimiter ) {
            pDelimiter = (char *)delimiterPeek(p,&executionStack.top() -> pNext);
            if ( ! ( NULL == pDelimiter ) ) {
                if ( DSC_DELIMITER[0] == pDelimiter[0] && DSC_DELIMITER[1] == pDelimiter[1] ) {
                    parseDSC(p,&executionStack.top() -> pNext,&lineNumber);
                    pPostScriptInterpreter -> queueLog(true,p,executionStack.top() -> pNext);
                    p = executionStack.top() -> pNext;
                    continue;
                } else if ( COMMENT_DELIMITER[0] == pDelimiter[0] ) {
                    parseComment(executionStack.top() -> pNext,&executionStack.top() -> pNext,&lineNumber);
                    pPostScriptInterpreter -> queueLog(true,p,executionStack.top() -> pNext);
                    p = executionStack.top() -> pNext;
                    continue;
                }
            }
        }

        pPostScriptInterpreter -> queueLog(true,p,executionStack.top() -> pNext);

        pLogStart = executionStack.top() -> pNext;

        if ( ! ( NULL == pDelimiter ) ) {
            (this ->* tokenProcedures[std::hash<std::string>()((char *)pDelimiter)])(executionStack.top() -> pNext,&executionStack.top() -> pNext,&lineNumber);
            pPostScriptInterpreter -> queueLog(true,pLogStart,executionStack.top() -> pNext);
            ADVANCE_THRU_WHITE_SPACE(executionStack.top() -> pNext)
            p = executionStack.top() -> pNext;
            continue;
        }

        if ( ! ( NULL == pCollectionDelimiter ) && PROC_DELIMITER_BEGIN[0] == pCollectionDelimiter[0] ) {
            char *pProcedureEnd = NULL;
            parseProcedureString(executionStack.top() -> pNext,&pProcedureEnd,&lineNumber);
            pPostScriptInterpreter -> queueLog(true,pLogStart,pProcedureEnd);

            try {
            push(new (CurrentObjectHeap()) procedure(this,executionStack.top() -> pNext,pProcedureEnd,&lineNumber));
            } catch ( PStoPDFException *pe ) {
                char szMessage[1024];
                sprintf(szMessage,"\n\nA %s exception occurred: %s\n",pe -> ExceptionName(),pe -> Message());
                pPostScriptInterpreter -> queueLog(true,szMessage,NULL,true);
            }

            ADVANCE_THRU_WHITE_SPACE(pProcedureEnd)
            executionStack.top() -> pNext = pProcedureEnd;
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
            char *pObjectName = parseObject(executionStack.top() -> pNext,&executionStack.top() -> pNext);
            po = resolve(pObjectName);
            if ( NULL == po ) {
                push(new (CurrentObjectHeap()) object(this,pObjectName));
                po = top();
            } else {
                push(po);
            }
        }

        if ( object::objectType::procedure == po -> ObjectType() ) {
            pop();
            executeProcedure(reinterpret_cast<procedure *>(po));
        } else
            executeObject();

        } catch ( nonPostscriptException *ex ) {
            pPostScriptInterpreter -> queueLog(true,ex -> Message(),NULL,true);
        } catch ( PStoPDFException *pe ) {
            char szMessage[1024];
            sprintf(szMessage,"\n\nA %s exception occurred: %s\n",pe -> ExceptionName(),pe -> Message());
            pPostScriptInterpreter -> queueLog(true,szMessage,NULL,true);
            if ( ! ( logLevel::none == PostScriptInterpreter::theLogLevel ) ) {
                executionStack.top() -> quitRequested = true;
            }
            //pPostScriptInterpreter -> queueLog("\nStack Trace:\n");
            //operatorPstack();
        }

        ADVANCE_THRU_WHITE_SPACE(executionStack.top() -> pNext)

        p = executionStack.top() -> pNext;

        if ( 0 == strncmp(executionStack.top() -> pNext,"terminate",9) )
            break;

        pPostScriptInterpreter -> queueLog(true,pLogStart,executionStack.top() -> pNext);

    } while ( p && ! executionStack.top() -> quitRequested );

    pPostScriptInterpreter -> settleLog(PostScriptInterpreter::hwndLogContent);

    _set_se_translator(defaultExTranslator);

    bool isTerminating = executionStack.top() -> quitRequested;

    delete executionStack.top();

    executionStack.pop();

    if ( isTerminating && 0 < executionStack.size() )
        executionStack.top() -> quitRequested = true;

    return 0;
    }


    void job::parseProcedure(procedure *pProcedure,char *pStart,char **ppEnd,long *pLineNumber) {

    char *p = pStart;
    char *pNext = p;

    do {

        {
        long count[]{0,0,0};
        while ( *p && strchr(PSZ_WHITE_SPACE_AND_EOL,*p) ) {
            count[0x0A == *p ? 0 : 0x0D == *p ? 1 : 2]++;
            p++;
        }
        *pLineNumber += max(count[0],count[1]);
        }

        if ( '\0' == *p )
            break;

        char *pCollectionDelimiter = collectionDelimiterPeek(p,&pNext);

        char *pDelimiter = NULL;

        if ( NULL == pCollectionDelimiter )
            pDelimiter = (char *)delimiterPeek(p,&pNext);

        if ( ! ( NULL == pDelimiter ) ) {
            (this ->* tokenProcedures[std::hash<std::string>()((char *)pDelimiter)])(pNext,&pNext,pLineNumber);
            if ( ! ( DSC_DELIMITER[0] == *pDelimiter ) && ! ( COMMENT_DELIMITER[0] == *pDelimiter ) )
                pProcedure -> insert(pop());
            p = pNext;
            continue;
        }

        if ( ! ( NULL == pCollectionDelimiter ) && PROC_DELIMITER_BEGIN[0] == *pCollectionDelimiter ) {
            char *pProcedureEnd = NULL;
            parseProcedureString(pNext,&pProcedureEnd,pLineNumber);
            procedure *pInnerProcedure = new (CurrentObjectHeap()) procedure(this,pNext,pProcedureEnd,pLineNumber);
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
