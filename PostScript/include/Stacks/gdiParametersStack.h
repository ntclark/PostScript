#pragma once

#include <stack>

class gdiParameters;

    class gdiParametersStack : public std::stack<gdiParameters *> {
    public:

        gdiParametersStack();
        ~gdiParametersStack();

        void initialize();

        void setupDC();

        void gSave();
        void gRestore();

        boolean isInitialized() { return 0 < size(); }

    private:

    };