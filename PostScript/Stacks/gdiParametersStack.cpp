
#include "job.h"

#include "gdiParameters.h"

    gdiParametersStack::gdiParametersStack() {
    push(new gdiParameters());
    return;
    }


    gdiParametersStack::~gdiParametersStack() {
    while ( 0 < size() ) {
        delete top();
        pop();
    }
    return;
    }


    void gdiParametersStack::initialize() {
    top() -> initialize();
    return;
    }


    void gdiParametersStack::gSave() {
    push(new gdiParameters(top()));
    return;
    }


    void gdiParametersStack::gRestore() {
    if ( 1 == size() )
{
Beep(2000,200);
        return;
}
    delete top();
    pop();
    top() -> setupDC();
    return;
    }


    void gdiParametersStack::setupDC() {
    top() -> setupDC();
    }
