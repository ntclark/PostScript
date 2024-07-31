
#include "PostScript objects/graphicsState.h"
#include "PostScript objects/save.h"

    void graphicsState::gSave() {
    pJob -> graphicsStateStack.gSave();
    gdiParametersStack.gSave();
    return;
    }


    void graphicsState::gRestore() {
    pJob -> graphicsStateStack.gRestore();
    gdiParametersStack.gRestore();
    return;
    }


    void graphicsState::save() {
    pJob -> push(new (pJob -> CurrentObjectHeap()) class save(pJob));
    gdiParametersStack.gSave();
    return;
    }


    void graphicsState::restore(class save *pSave) {
    gRestore();
/*
    07/25/2024: It is not entirely clear what all of the entities that may be
    restored are, but at least the main transform,
*/
    pToUserSpace -> identity();
    return;
    }