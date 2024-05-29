
#include "job.h"

#include "PostScript objects/graphicsState.h"
#include "PostScript objects/filter.h"

    void graphicsState::filter() {

    //datasrc dict param1 … paramn filtername 
    //datatgt dict param1 … paramn filtername 

    object *pFilterName = pJob -> pop();

    if ( 0 == strcmp(pFilterName -> Contents(),"DCTDecode") ) {
/*
        (Page 85)
        DCTDecode (none) Decompresses DCT-encoded data, producing image sample data that
        approximate the original data. 
*/
        object *pDataSource = pJob -> pop();

        class filter *pFilter = new (pJob -> CurrentObjectHeap()) class filter(pJob,pFilterName -> Contents(),pDataSource,NULL);

        pJob -> push(pFilter);

    } else if ( 0 == strcmp(pFilterName -> Contents(),"ASCII85Decode") ) {
/*
        (Page 85)
        ASCII85Decode (none) Decodes ASCII base-85 data, producing the original binary data.
*/
        object *pDataSource = pJob -> pop();

        class filter *pFilter = new (pJob -> CurrentObjectHeap()) class filter(pJob,pFilterName -> Contents(),pDataSource,"~>");

        pJob -> push(pFilter);
#if 0
        if ( object::objectType::file == pDataSource -> ObjectType() ) {

        } else {
            char szMessage[1024];
            sprintf_s<1024>(szMessage,"The standard filter %s is not implemented",pFilterName -> Contents());
            throw typecheck(szMessage);
            return;
        }
#endif
    } else {
        char szMessage[1024];
        sprintf_s<1024>(szMessage,"The standard filter %s is not implemented",pFilterName -> Contents());
        throw typecheck(szMessage);
        return;
    }

    return;
    }