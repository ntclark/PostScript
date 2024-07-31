
#include "PostScript objects/procedure.h"

#include "job.h"

    procedure::procedure(job *pj,char *pStart,char *pEnd,char **ppEnd) :
        isBound(false),pszStringRepresentation(NULL),
        object(pj,NULL,object::objectType::procedure,object::valueType::executableProcedure,object::valueClassification::composite,object::executableAttribute::executable)
    {

    pJob -> parseProcedure(this,pStart,ppEnd);

    long n = 0L;
    if ( NULL == ppEnd ) {
        if ( NULL == pEnd )
            n = (DWORD)strlen(pStart);
        else
            n = (long)(pEnd - pStart);
    } else
        n = (long)(*ppEnd - pStart);

    pszStringRepresentation = new char[n];
    memset(pszStringRepresentation,0,n * sizeof(char));

    strncpy(pszStringRepresentation,pStart,n - 1);

    return;
    }

    procedure::procedure(job *pj,char *pStart,char *pEnd) : procedure(pj,pStart,pEnd,NULL) {}

    procedure::procedure(job *pj) : procedure(pj,NULL,NULL,NULL) {}

    procedure::procedure(job *pj,char *pStart,char **ppEnd) : procedure(pj,pStart,NULL,ppEnd) {}


    procedure::~procedure() {
    entries.clear();
    if ( ! ( NULL == pszStringRepresentation ) )
        delete pszStringRepresentation;
    return;
    }


    void procedure::insert(object *p) {
    entries.insert(entries.end(),p);
    return;
    }


    void procedure::clear() {
    entries.clear();
    return;
    }


    void procedure::execute() {

    try {

    pJob -> executeProcedure(this);

    } catch ( syntaxerror *pse ) {
        char szMessage[2048];
        sprintf(szMessage,"\n\n%s\n\nThere was a %s exception while executing the procedure(%s):\n%s",pse -> Message(),pse -> ExceptionName(),Name(),Contents());
        pPStoPDF -> queueLog(szMessage);
        throw;
    }

    return;
    }


    void procedure::bind() {

    if ( isBound )
        return;

    try {

    pJob -> bindProcedure(this);

    isBound = true;

    } catch ( syntaxerror *pse ) {
        char szMessage[2048];
        sprintf(szMessage,"\n\n%s\n\nThere was a %s while binding the procedure(%s):\n%s",pse -> Message(),pse -> ExceptionName(),Name(),Contents());
        pPStoPDF -> queueLog(szMessage);
        throw;
    }

    return;
    }


    char *procedure::Contents(char *pszContents) {
    if ( pszContents )
        return object::Contents(pszContents);
    return ToString();
    }


    void procedure::putElement(long index,object *pObject) {
    entries[index] = pObject;
    return;
    }


    char *procedure::ToString() {

    if ( pszStringRepresentation )
        delete [] pszStringRepresentation;

    long n = 0L;
    for ( object *pObj : entries )
        n += (long)strlen(pObj -> ToString());

    n += 4 * (long)entries.size();
    n += 4;

    pszStringRepresentation = new char[n];

    memset(pszStringRepresentation,0,n * sizeof(char));

    n = sprintf(pszStringRepresentation,"{");

    for ( object *pObj : entries )
        n += sprintf(pszStringRepresentation + n," %s ",pObj -> ToString());

    n += sprintf(pszStringRepresentation + n,"}");

    return pszStringRepresentation;
    }