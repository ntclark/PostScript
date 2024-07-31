#include "job.h"

    long job::execute(char *pBegin) {

    pStart = pBegin;
    char *p = pStart;
    pNext = p;
    char *pLogStart = NULL;

    try {

    do {

        ADVANCE_THRU_WHITE_SPACE(p)

        if ( '\0' == *p )
            break;

        char *pCollectionDelimiter = collectionDelimiterPeek(p,&pNext);

        char *pDelimiter = NULL;

        if ( NULL == pCollectionDelimiter )
            pDelimiter = (char *)delimiterPeek(p,&pNext);

        pPStoPDF -> queueLog(p,pNext);

        pLogStart = pNext;

        if ( ! ( NULL == pDelimiter ) ) {
            (this ->* tokenProcedures[std::hash<std::string>()((char *)pDelimiter)])(pNext,&pNext);
            pPStoPDF -> queueLog(pLogStart,pNext);
            ADVANCE_THRU_WHITE_SPACE(pNext)
            p = pNext;
            continue;
        }

        if ( ! ( NULL == pCollectionDelimiter ) && PROC_DELIMITER_BEGIN[0] == *pCollectionDelimiter ) {
            char *pProcedureEnd = NULL;
            parseProcedureString(pNext,&pProcedureEnd);
            pPStoPDF -> queueLog(pLogStart,pProcedureEnd);
            push(new (CurrentObjectHeap()) procedure(this,pNext,pProcedureEnd));
            ADVANCE_THRU_WHITE_SPACE(pProcedureEnd)
            pNext = pProcedureEnd;
            p = pProcedureEnd;
            continue;
        }

        object *po = NULL;

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

        ADVANCE_THRU_WHITE_SPACE(pNext)

        p = pNext;

        pPStoPDF -> queueLog(pLogStart,pNext);

    } while ( p );

    pPStoPDF -> queueLog(pLogStart,pNext);

    } catch ( syntaxerror se ) {
        throw;
    }

    return 0;
    }


    void job::parseProcedure(procedure *pProcedure,char *pStart,char **ppEnd) {

    char *p = pStart;
    char *pNext = p;

    do {

        ADVANCE_THRU_WHITE_SPACE(p)

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
