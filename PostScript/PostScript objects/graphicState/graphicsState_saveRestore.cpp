
#include "job.h"

#include "pathParameters.h"
#include "gdiParameters.h"

static long xx = 0L;

    void graphicsState::gSave() {
    gdiParametersStack.gSave();
    pathParametersStack.gSave();
    psXformsStack.gSave();
    CurrentFont() -> gSave();
xx++;
    return;
    }


    void graphicsState::gRestore() {
    gdiParametersStack.gRestore();
    pathParametersStack.gRestore();
    psXformsStack.gRestore();
    CurrentFont() -> gRestore();
--xx;
    return;
    }


    void graphicsState::save() {
    pJob -> push(new (pJob -> CurrentObjectHeap()) class save(pJob));
    //gSave();
    return;
    }


    void graphicsState::restore(class save *pSave) {
    //gRestore();
    return;
    }