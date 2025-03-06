
#include "job.h"

#include "pathParameters.h"
#include "gdiParameters.h"


    void graphicsState::gSave() {
    theGDIParameters.SaveState();
    pathParametersStack.gSave();
    psXformsStack.gSave();
    CurrentFont() -> gSave();
    return;
    }


    void graphicsState::gRestore() {
    theGDIParameters.RestoreState();
    pathParametersStack.gRestore();
    psXformsStack.gRestore();
    CurrentFont() -> gRestore();
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