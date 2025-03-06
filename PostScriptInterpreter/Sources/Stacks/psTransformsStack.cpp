
#include "job.h"

    psTransformsStack::psTransformsStack() {
    return;
    }


    psTransformsStack::~psTransformsStack() {
    while ( 0 < size() ) {
        //delete top();
        pop();
    }
    return;
    }


    void psTransformsStack::initialize(job *pj) {
    pJob = pj;
    push(new (pJob -> CurrentObjectHeap()) matrix(pJob));
    return;
    }


    void psTransformsStack::gSave() {
    matrix *pMatrix = new (pJob -> CurrentObjectHeap()) matrix(pJob);
    pMatrix -> copyFrom(top());
    push(pMatrix);
    return;
    }


    void psTransformsStack::gRestore() {
    if ( 1 == size() )
{
Beep(2000,200);
        return;
}
    //delete top();
    pop();
    return;
    }