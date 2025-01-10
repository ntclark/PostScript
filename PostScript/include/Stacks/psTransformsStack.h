#pragma once

#include <stack>

class matrix;

    class psTransformsStack : public std::stack<matrix *> {
    public:

        psTransformsStack();
        ~psTransformsStack();

        void initialize(job *pJob);

        void gSave();
        void gRestore();

        boolean isInitialized() { return 0 < size(); }

    private:

        job *pJob{NULL};
    };
