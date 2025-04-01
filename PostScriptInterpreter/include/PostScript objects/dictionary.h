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

#pragma once

#include <list>
#include <vector>
#include <unordered_map>

#define DEFAULT_DICTIONARY_SIZE 32

class job;

#include "PostScript objects/string.h"
#include "PostScript objects/procedure.h"

    struct entry {
        entry(job *pJob,char *pszName,object *pObj);
        char szName[64];
        long nName{0L};
        string *pKey{NULL};
        object *pValue{NULL};
    };


    class dictionary : public object {
    public:

        dictionary(job *,long initialSize);
        dictionary(job *,char *pszName);
        dictionary(job *,char *pszName,long initialSize);

        virtual ~dictionary();

        void put(object *);
        void put(char *pszKey,char *);
        void put(char *pszKey,object *);
        void put(char *pszKey,void (job::*theProcedure)());

        object *retrieve(long index);
        char *retrieveKey(long index);

        object *retrieve(char *);
        object *retrieveKey(char *);

        virtual char *ToString();

        boolean exists(char *);
        boolean exists(object *);
        boolean hasSameEntries(dictionary *pOtherDict);

        void remove(char *pszKey);
        void remove(object *pObj);

        long size() { return (long)entries.size(); };

        void copyFrom(dictionary *pSource);

        void forAll(class procedure *pOperator);

    private:

        std::vector<entry *> entries;

        long entryCount{0};

        static long countAutoCreated;

    friend class dictionaryStack;
    friend class job;

    };
