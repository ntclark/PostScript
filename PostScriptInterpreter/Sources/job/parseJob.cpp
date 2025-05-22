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

    long job::parseJob(bool useThread) {

    pPostScriptInterpreter -> clearLog(PostScriptInterpreter::hwndPostScriptLogContent);

    if ( useThread ) {
        unsigned int threadAddr;
        HANDLE hJobThread = (HANDLE)_beginthreadex(NULL,65536,job::executeThread,(void *)this,CREATE_SUSPENDED,&threadAddr);
        ResumeThread(hJobThread);
    } else
        executeThread((void *)this);

    return 0L;
    }


    unsigned int __stdcall job::executeInitialization(void *pvThis) {

    job *pThis = reinterpret_cast<job *>(pvThis);

#if 0
    long gsResources[] = {ID_GS_PREP,ID_GS_INIT,ID_GS_RESOURCES,0};

    for ( long k = 0; 1; k++ ) {

        if ( 0 == gsResources[k] )
            break;

        HRSRC hrsrc = FindResourceA(hModule,MAKEINTRESOURCE(gsResources[k]),"#256");

        HGLOBAL hResource = LoadResource(hModule,hrsrc);

        SIZE_T sizeData = SizeofResource(hModule,hrsrc);

        pThis -> pStorage = new char[sizeData + 1];
        memcpy(pThis -> pStorage,(char *)LockResource(hResource),sizeData);
        pThis -> pEnd = pThis -> pStorage + sizeData;
        *pThis -> pEnd = '\0';

        pThis -> execute((char *)(pThis -> pStorage));

        delete [] pThis -> pStorage;

        pThis -> pStorage = NULL;

    }
#endif

#if 1
    ReleaseSemaphore(pThis -> hsemIsInitialized,1,NULL);
    CloseHandle(pThis -> hsemIsInitialized);
    pThis -> hsemIsInitialized = INVALID_HANDLE_VALUE;
#endif
    return 0L;
    }


    unsigned int __stdcall job::executeThread(void *pvThis) {
    job *pThis = reinterpret_cast<job *>(pvThis);

    if ( ! ( INVALID_HANDLE_VALUE == pThis -> hsemIsInitialized ) )
        WaitForSingleObject(pThis -> hsemIsInitialized,INFINITE);

    pThis -> execute(pThis -> pStorage,pThis -> pStorageEnd,pThis -> szPostScriptSourceFile);

    for ( std::function<void(void)> fn : PostScriptInterpreter::endRunActions )
        fn();

    PostScriptInterpreter::endRunActions.clear();

    return 0L;
    }


    long job::executeObject() {

    object *pObject = top();

    if ( ! ( object::executableAttribute::executable == pObject -> theExecutableAttribute ) ) {

        if ( object::objectType::atom == pObject -> ObjectType() && object::valueType::string == pObject -> ValueType() ) {

            pObject = resolve(pObject -> Name());

            if ( NULL == pObject || ! ( object::executableAttribute::executable == pObject -> theExecutableAttribute ) ) {
                if ( ! ( NULL == pObject ) ) {
                    pop();
                    push(pObject);
                }
                return 0;
            }

        } else
            return 0;
    }

    // seekDefinition will replace the object at the top of the 
    // stack with it's defined object if it is found
    // and will leave it alone if it is not found

    // (I am not sure this needs to be here ?!?!?)

    seekDefinition();

    switch ( pObject -> ObjectType() ) {

    // A directExecutable is an operator

    case object::objectType::directExecutable: {
        directExec *pDirectExec = reinterpret_cast<directExec *>(pop());
        void (__thiscall job::*pOperator)() = pDirectExec -> Operator();
        (this ->* pOperator)();
        return 1L;
        }

    case object::objectType::procedure:
/*
      3.5.3 Deferred Execution

        The first line of Example 3.2 defines a procedure named average that computes
        the average of two numbers. The second line applies that procedure to the inte-
        gers 40 and 60, producing the same result as Example 3.1.

            Example 3.2
            /average {add 2 div} def
            40 60 average

        The interpreter first encounters the literal name average. Recall from Section 3.2,
        “Syntax,” that / introduces a literal name. The interpreter pushes this object on
        the operand stack, as it would any object having the literal attribute.

        Why did the interpreter treat the procedure as data in the first line of the example
        but execute it in the second, despite the procedure having the executable attribute
        in both cases? There is a special rule that determines this behavior: An executable
        array or packed array encountered directly by the interpreter is treated as data
        (pushed on the operand stack), but an executable array or packed array encountered
        indirectly—as a result of executing some other object, such as a name or an
        operator—is invoked as a procedure.
*/
        reinterpret_cast<procedure *>(pop()) -> execute();
        return 1L;

    default:
        break;

    }

    return 0L;
    }


    void job::executeProcedure(procedure *pProcedure) {

    long stepNumber = 0;

    for ( object *pObj : pProcedure -> entries ) {

        stepNumber++;

        push(pObj);

        /*
        Per the comment above from section 3.5.3: if this is a procedure, it is 
        at this point "directly encountered" by the interpreter. Therefore, 
        it is treated as data and is simply placed (actually left) on the stack.
        */

        if ( object::objectType::procedure == pObj -> ObjectType() )
            continue;

        executeObject();

        if ( executionStack.top() -> quitRequested )
            break;
    }

    return;
    }


    bool job::seekDefinition() {

    // seekDefinition will replace the object at the top of the 
    // stack with it's defined object if it is found
    // and will leave it alone if it is not found

    object *pKey = top();

    operatorWhere();

    if ( top() == pFalseConstant ) {
        pop();
        push(pKey);
        return false;
    }

    pop();

    dictionary *pDictionary = reinterpret_cast<dictionary *>(pop());

    push(pDictionary -> retrieve(pKey -> Name()));

    return true;
    }


    dictionary * job::containingDictionary(object *pObj) {
    if ( NULL == pObj -> Name() )
        return NULL;
    return pDictionaryStack -> find(pObj -> Name());
    }


    void job::bindProcedure(procedure *pProcedure) {

    long k = -1L;

    for ( object *pObject : pProcedure -> entries ) {

        k++;

        switch ( pObject -> ObjectType() ) {
        case object::objectType::procedure:
            reinterpret_cast<procedure *>(pObject) -> bind();
            continue;

        case object::objectType::literal:
        case object::objectType::number:
            continue;

        case object::objectType::atom: {
            switch ( pObject -> ValueType() ) {
            case object::valueType::string: {
                push(pObject);
                if ( seekDefinition() ) {
                    object *pResolvedObject = pop();
                    if ( object::objectType::directExecutable == pResolvedObject -> ObjectType() ) 
                        pProcedure -> entries[k] = pResolvedObject;
                    continue;
                }
                pop();
                }
                break;
            default:
                break;
            }
            }
            break;

        default:
            break;

        }

        if ( ! ( NULL == pObject -> Name() ) ) {
            dictionary *pDict = pDictionaryStack -> find(pObject -> Name());
            if ( ! ( NULL == pDict ) ) {
                pProcedure -> entries[k] = pDict -> retrieve(pObject -> Name());
                continue;
            }
        }

        if ( ! ( NULL == containingDictionary(pObject) ) ) {
            push(pObject);
            resolve();
            pProcedure -> entries[k] = pop();
        }

    }

    return;
    }


    char *job::collectionDelimiterPeek(char *pStart,char **ppEnd) {
    static char result[32];
    memset(result,0,sizeof(result));
    ADVANCE_THRU_WHITE_SPACE(pStart)
    if ( ! pStart )
        return NULL;
    *ppEnd = pStart;
    for ( long k = 0; 1; k++ ) {
        if ( ! psCollectionDelimiters[k][0] )
            return NULL;
        if ( *pStart == psCollectionDelimiters[k][0] ) {
            if ( 2 == psCollectionDelimiterLength[k] ) {
                if ( psCollectionDelimiters[k][1] == *(pStart + 1) ) {
                    *ppEnd = pStart + 2;
                    result[0] = psCollectionDelimiters[k][0];
                    result[1] = psCollectionDelimiters[k][1];
                    return result;
                }
            } else {
                *ppEnd = pStart + 1;
                result[0] = psCollectionDelimiters[k][0];
                return result;
            }
        }
    }
    return NULL;
    }


    char *job::delimiterPeek(char *pStart,char **ppEnd) {
    static char result[32];
    memset(result,0,sizeof(result));
    ADVANCE_THRU_WHITE_SPACE(pStart)
    if ( ! pStart )
        return NULL;
    for ( long k = 0; 1; k++ ) {
        if ( '\0' == psDelimiters[k][0] )
            return NULL;
        if ( *pStart == psDelimiters[k][0] ) {
            if ( 2 == psDelimiterLength[k] ) {
                if ( psDelimiters[k][1] == *(pStart + 1) ) {
                    *ppEnd = pStart + 2;
                    result[0] = psDelimiters[k][0];
                    result[1] = psDelimiters[k][1];
                    return result;
                }
            } else {
                *ppEnd = pStart + 1;
                result[0] = psDelimiters[k][0];
                return result;
            }
        }
    }
    return NULL;
    }


    void job::parseLiteralName(char *pStart,char **pEnd,long *pLineNumber) {
    char *p = pStart;
    ADVANCE_THRU_WHITE_SPACE(p)
    char *pBegin = p;
    ADVANCE_TO_END(p)
    push(new (CurrentObjectHeap()) literal(this,pBegin,p));
    ADVANCE_THRU_WHITE_SPACE(p)
    *pEnd = p;
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


    void job::parseProcedureString(char *pStart,char **ppEnd,long *pLineNumber) {

    char *p = pStart;
    char *pNext = p;
    long depth = 1L;

    do {

        ADVANCE_THRU_WHITE_SPACE(p)

        if ( '\0' == *p )
            break;

        char *pCollectionDelimiter = collectionDelimiterPeek(p,&pNext);

        if ( ! ( NULL == pCollectionDelimiter ) && PROC_DELIMITER_BEGIN[0] == *pCollectionDelimiter ) {
            depth++;
            p = pNext;
            continue;
        }

        if ( ! ( NULL == pCollectionDelimiter ) && PROC_DELIMITER_END[0] == *pCollectionDelimiter ) {
            depth--;
            if ( 0 == depth ) {
                *ppEnd = pNext;
                return;
            }
            p = pNext;
            continue;
        }

        p++;

    } while ( ! ( '\0' == p ) );

    *ppEnd = NULL;

    return;
    }


    void job::parseDSC(char *pStart,char **ppEnd,long *pLineNumber) {
    char *p = pStart;
    ADVANCE_THRU_END_OF_LINE(p,pLineNumber)
    *ppEnd = p;
    pDSCItems -> push_back(new (CurrentObjectHeap()) dscItem(this,pStart,*ppEnd));
    return;
    }


    void job::parseComment(char *pStart,char **ppEnd,long *pLineNumber) {
    char *p = pStart;
    ADVANCE_THRU_END_OF_LINE(p,pLineNumber)
    *ppEnd = p;
    pComments -> push_back(new (CurrentObjectHeap()) comment(this,pStart,*ppEnd));
    return;
    }


    void job::parseString(char *pStart,char **ppEnd,long *pLineNumber) {

    char *p = pStart;
    *ppEnd = NULL;

    parse(STRING_DELIMITER_BEGIN,STRING_DELIMITER_END,p,ppEnd,pLineNumber);

    if ( *ppEnd ) {
        push(new (CurrentObjectHeap()) constantString(this,p,*ppEnd));
        *ppEnd = *ppEnd + 1;
    }

    return;
    }


    void job::parseHexString(char *pStart,char **ppEnd,long *pLineNumber) {
    char *p = pStart;
    *ppEnd = NULL;

    parse(HEX_STRING_DELIMITER_BEGIN,HEX_STRING_DELIMITER_END,p,ppEnd,pLineNumber);

    if ( ! *ppEnd ) 
        return;

    char c = **ppEnd;
    **ppEnd = '\0';

    long n = (DWORD)strlen((char *)p) + 1;
    char *pszNew = new char[n];
    memset(pszNew,0,n * sizeof(char));
    long k = 0; 
    long cbString = 0;
    while ( k < n ) {
        if ( 0x0D == p[k] || 0x0A == p[k] ) {
            k++;
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
    *ppEnd = *ppEnd + 1;

    return;
    }


    void job::parseHex85String(char *pStart,char **pEnd,long *pLineNumber) {
MessageBox(NULL,"Not implemented","Not implemented",MB_OK);
    return;
    }
