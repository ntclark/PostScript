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
//
//NTC: 02-19-2012: Inline images start with the BI operator, and end with the EI operator, with an intervening
// ID "operator" that contains the image data after the operator.
// Until inline images are implemented completely and correctly, I am going to parse from any ID operator, through to the EI operator
// and ignore the inline image image data.
//
        if ( ! ( NULL == po -> Contents() ) && 2 == strlen(po -> Contents()) && 0 == strncmp(po -> Contents(),"ID",2) ) {

            while ( ! ( 'E' == pNext[0] && 'I' == pNext[1] && ( 0x0A == pNext[2] || 0x0D == pNext[2] || ' ' == pNext[2] ) ) )
                pNext++;

            pNext += 3;

        }

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
        } else {
            char *pObjectName = parseObject(p,&p);
            po = resolve(pObjectName);
            if ( NULL == po )
                po = new (CurrentObjectHeap()) object(this,pObjectName);
        }

        pProcedure -> insert(po);

    } while ( ! ( '\0' == p ) );

    if ( ! ( NULL == ppEnd ) )
        *ppEnd = NULL;

    return;
    }
