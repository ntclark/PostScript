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


    long job::execute(char *pBegin,char *pEnd,char *pszFileName) {

    executionStack.push(new executionLevel(pBegin,pEnd,pszFileName));

    if (NULL == pRootExecutionLevel )
        pRootExecutionLevel = executionStack.top();

    char *p = pBegin;
    char *pLogStart = NULL;

    executionLevel *pExecutionLevel = executionStack.top();

    static _se_translator_function defaultExTranslator = NULL;

    if ( 1 == executionStack.size() )
        defaultExTranslator = _set_se_translator(trans_func);

    if ( 0 < inputLineNumberSize ) {
        sprintf_s<16>(szLNFormat,"%%0%ldld",inputLineNumberSize);
        sscanf(p,szLNFormat,&inputLineNumber);
        p += inputLineNumberSize;
    }

    do {

        if ( '\0' == *p )
            break;

if ( 412 == inputLineNumber )
printf("hello world");
        if ( 0x0A == p[0] || 0x0D == p[0] ) {
            pExecutionLevel-> pNext = p;
            while ( 0x0A == *pExecutionLevel -> pNext || 0x0D == *pExecutionLevel -> pNext )
                pExecutionLevel -> pNext++;
            pPostScriptInterpreter -> queueLog(true,p,pExecutionLevel -> pNext,false);
            p = pExecutionLevel -> pNext;
            if ( 0 < inputLineNumberSize ) {
                sscanf(p,szLNFormat,&inputLineNumber);
                p += inputLineNumberSize;
            }
            continue;
        }

        if ( 0 == strncmp(pExecutionLevel -> pNext,"terminate",9) )
            break;

        char *pCollectionDelimiter = collectionDelimiterPeek(p,&pExecutionLevel -> pNext);
        char *pDelimiter = NULL;

        if ( NULL == pCollectionDelimiter ) {

            pDelimiter = (char *)delimiterPeek(p,&pExecutionLevel -> pNext);

            if ( ! ( NULL == pDelimiter ) ) {

                pLogStart = pExecutionLevel -> pNext;

                if ( DSC_DELIMITER[0] == pDelimiter[0] && DSC_DELIMITER[1] == pDelimiter[1] ) {
                    parseDSC(p,&pExecutionLevel -> pNext);
                    pLogStart -= 2;
                } else {
                    (this ->* tokenProcedures[std::hash<std::string>()((char *)pDelimiter)])(pExecutionLevel -> pNext,
                            &pExecutionLevel -> pNext);
                    pLogStart--;
                    if ( RESOLVE_NOW_DELIMITER[0] == pDelimiter[0] && RESOLVE_NOW_DELIMITER[1] == pDelimiter[1] )
                        push(resolve(pop() -> Name()));
                }

                pPostScriptInterpreter -> queueLog(true,pLogStart,pExecutionLevel -> pNext);

                ADVANCE_THRU_WHITE_SPACE(pExecutionLevel -> pNext)

                p = pExecutionLevel -> pNext;

                continue;

            }

        }

        if ( ! ( NULL == pCollectionDelimiter ) && PROC_DELIMITER_BEGIN[0] == pCollectionDelimiter[0] ) {

            pLogStart = pExecutionLevel -> pNext - 1;

            char *pProcedureEnd = NULL;
            parseProcedureString(pExecutionLevel -> pNext,&pProcedureEnd);
            pPostScriptInterpreter -> queueLog(true,pLogStart,pProcedureEnd);

            try {

            push(new (CurrentObjectHeap()) procedure(this,pExecutionLevel -> pNext,pProcedureEnd));

            object *pName = peekPrior();

            if ( ! ( NULL == pName ) && object::objectType::literal == pName -> ObjectType() )
                top() -> Name(pName -> Contents());

            } catch ( nonPostscriptException *ex ) {
                pPostScriptInterpreter -> queueLog(true,ex -> Message(),NULL,true);
            } catch ( PStoPDFException *pe ) {
                char szMessage[1024];
                sprintf(szMessage,"\n\nA %s exception occurred: %s\n",pe -> ExceptionName(),pe -> Message());
                pPostScriptInterpreter -> queueLog(true,szMessage,NULL,true);
            }

            ADVANCE_THRU_WHITE_SPACE(pProcedureEnd)

            pExecutionLevel -> pNext = pProcedureEnd;
            p = pProcedureEnd;
            continue;
        }

        pLogStart = pExecutionLevel -> pNext;

        object *po = NULL;

        try {

        if ( ! ( NULL == pCollectionDelimiter ) ) {
            pPostScriptInterpreter -> queueLog(true,pCollectionDelimiter,NULL,false);
            po = new (CurrentObjectHeap()) object(this,pCollectionDelimiter);
            push(po);
            resolve();
            po = top();
        } else {
            char *pObjectName = parseObject(pExecutionLevel -> pNext,&pExecutionLevel -> pNext);
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
                pExecutionLevel -> quitRequested = true;
            }
        }

        ADVANCE_THRU_WHITE_SPACE(pExecutionLevel -> pNext)

        p = pExecutionLevel -> pNext;

        if ( 0 == strncmp(pExecutionLevel -> pNext,"terminate",9) )
            break;

        pPostScriptInterpreter -> queueLog(true,pLogStart,pExecutionLevel -> pNext);

    } while ( p && ! ( pExecutionLevel -> quitRequested || pExecutionLevel -> fileClosed ) );

    pPostScriptInterpreter -> settleLog(PostScriptInterpreter::hwndPostScriptLogContent);

    if ( 1 == executionStack.size() ) {
        _set_se_translator(defaultExTranslator);
        defaultExTranslator = NULL;
    }

    bool isTerminating = pExecutionLevel -> quitRequested;

    delete pExecutionLevel;

    executionStack.pop();

    if ( isTerminating && 0 < executionStack.size() )
        executionStack.top() -> quitRequested = true;

    return 0;
    }


    void job::parseProcedure(procedure *pProcedure,char *pStart,char **ppEnd) {

    char *p = pStart;
    char *pNext = p;

    do {

        if ( '\0' == *p )
            break;

        ADVANCE_THRU_WHITE_SPACE(p)

        if ( 0x0A == p[0] || 0x0D == p[0] ) {
            while ( 0x0A == *p || 0x0D == *p ) 
                p++;
            if ( 0 < inputLineNumberSize ) {
                sscanf(p,szLNFormat,&inputLineNumber);
                p += inputLineNumberSize;
            }
            continue;
        }

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

        po -> HandyIdentifier((long)pProcedure -> entries.size());

        pProcedure -> insert(po);

    } while ( ! ( '\0' == p ) );

    if ( ! ( NULL == ppEnd ) )
        *ppEnd = NULL;

    return;
    }
