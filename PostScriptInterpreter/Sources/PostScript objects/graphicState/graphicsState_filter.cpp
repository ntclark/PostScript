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

    object *pFilterName = pJob -> pop();

    if ( 0 == strcmp(pFilterName -> Contents(),filter::pszKindSubFile) ) {
    
        /*
        (Page 86)
        SubFileDecode count, string

        Passes all data through, without any modification. This permits an
        arbitrary data source (procedure or string) to be treated as an input
        file. Optionally, this filter detects an end-of-data marker in the source
        data stream, treating the preceding data as a subfile. 

        ALSO

        PostScript Language Reference Third Edition page(151)

        SubFileDecode Filter
            source EODCount EODString /SubFileDecode filter
            source dictionary EODCount EODString /SubFileDecode filter
            source dictionary /SubFileDecode filter (LanguageLevel 3)

        More information in subFileFilter.cpp

        */

        object *pFilterString = pJob -> pop();
        object *pFilterCount = NULL;
        object *pFilterDataSource = NULL;
        object *pFilterDictionary = NULL;

        boolean isLanguageLevel3 = object::objectType::dictionaryObject == pFilterString -> ObjectType();

        if ( ! isLanguageLevel3 ) {
            pFilterCount = pJob -> pop();
            pFilterDataSource = pJob -> pop();
            if ( object::objectType::dictionaryObject == pFilterDataSource -> ObjectType() ) {
                pFilterDictionary = pFilterDataSource;
                pFilterDataSource = pJob -> pop();
            }
        } else {
            pFilterDictionary = pFilterString;
            pFilterDataSource = pJob -> pop();
        }

        subFileFilter *pFilter = new (pJob -> CurrentObjectHeap()) subFileFilter(pJob,pFilterDataSource,pFilterString,pFilterCount,pFilterDictionary,isLanguageLevel3);

        pJob -> push(pFilter);

        return;
    }

    object *pds = pJob -> pop();

    class filter *pDataSource = NULL;

    if ( object::objectType::file == pds -> ObjectType() ) 
        pDataSource = new (pJob -> CurrentObjectHeap()) fileFilter(pJob);
    else
        pDataSource = reinterpret_cast<class filter *>(pds);

    if ( 0 == strcmp(pFilterName -> Contents(),filter::pszKindDCT) ) {
        pJob -> push(new (pJob -> CurrentObjectHeap()) dctFilter(pJob,pDataSource));
        return;
    } 

    if ( 0 == strcmp(pFilterName -> Contents(),filter::pszKindAscii85) ) {
        pJob -> push(new (pJob -> CurrentObjectHeap()) ascii85Filter(pJob,pDataSource));
        return;
    } 

    if ( 0 == strcmp(pFilterName -> Contents(),filter::pszKindRunLength) ) {
        pJob -> push(new (pJob -> CurrentObjectHeap()) runLengthFilter(pJob,pDataSource));
        return;
    } 

    if ( 0 == strcmp(pFilterName -> Contents(),filter::pszKindLZW) ) {
        pJob -> push(new (pJob -> CurrentObjectHeap()) lzwFilter(pJob,pDataSource));
        return;
    }

    char szMessage[1024];
    sprintf_s<1024>(szMessage,"The standard filter %s is not implemented",pFilterName -> Contents());
    throw typecheck(szMessage);
    return;
    }