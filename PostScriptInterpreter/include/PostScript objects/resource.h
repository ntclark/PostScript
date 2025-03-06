#pragma once

#include "PostScript objects\object.h"

    class resource : public object {
    public:

        resource(job *pJob,object *pCategory,object *pInstance,object *pName);
        ~resource();

        virtual char *Name(char *pszNewName = NULL);

        char *Category() { return pCategory -> Contents(); }
        object *Instance() { return pInstance; }

    private:

        object *pCategory;
        object *pInstance;
        object *pName;

    };
