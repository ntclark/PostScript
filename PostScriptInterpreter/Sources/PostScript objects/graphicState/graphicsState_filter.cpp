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

        return;

    } 

    if ( 0 == strcmp(pFilterName -> Contents(),"ASCII85Decode") ) {

        /*
        (Page 85)
        ASCII85Decode (none) Decodes ASCII base-85 data, producing the original binary data.
        */
        object *pDataSource = pJob -> pop();

        class filter *pFilter = new (pJob -> CurrentObjectHeap()) class filter(pJob,pFilterName -> Contents(),pDataSource,"~>");

        pJob -> push(pFilter);

        return;

    } 

    if ( 0 == strcmp(pFilterName -> Contents(),"RunLengthDecode") ) {

        /*
        (Page 86)
        RunLengthDecode (none) Decompresses data encoded in the run-length encoding format, producing the original data. 
        */

        object *pDataSource = pJob -> pop();

        class filter *pFilter = new (pJob -> CurrentObjectHeap()) class filter(pJob,pFilterName -> Contents(),pDataSource,"~>");

        pJob -> push(pFilter);

        return;

    } 

    char szMessage[1024];
    sprintf_s<1024>(szMessage,"The standard filter %s is not implemented",pFilterName -> Contents());
    throw typecheck(szMessage);
    return;
    }