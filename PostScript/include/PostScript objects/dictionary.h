
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
