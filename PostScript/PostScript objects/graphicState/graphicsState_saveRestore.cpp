
#include "job.h"

#include "pathParameters.h"
#include "gdiParameters.h"

    void graphicsState::gSave() {
    gdiParametersStack.gSave();
    pathParametersStack.gSave();
    fontStack.gSave();
    return;
    }


    void graphicsState::gRestore() {
    gdiParametersStack.gRestore();
    pathParametersStack.gRestore();
    fontStack.gRestore();
    return;
    }


    void graphicsState::save() {
    pJob -> push(new (pJob -> CurrentObjectHeap()) class save(pJob));
    gSave();
    return;
    }


    void graphicsState::restore(class save *pSave) {
    gRestore();
    return;
    }