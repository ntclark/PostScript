
#include "job.h"

#include "pathParameters.h"

    pathParametersStack::pathParametersStack() {
    return;
    }


    pathParametersStack::~pathParametersStack() {
    while ( 0 < size() ) {
        delete top();
        pop();
    }
    return;
    }


    void pathParametersStack::initialize() {
    push(new pathParameters());
    top() -> initialize();
    return;
    }


    void pathParametersStack::gSave() {
    push(new pathParameters(top()));
    return;
    }


    void pathParametersStack::gRestore() {
    if ( 1 == size() )
{
Beep(2000,200);
        return;
}
    delete top();
    pop();
    return;
    }
