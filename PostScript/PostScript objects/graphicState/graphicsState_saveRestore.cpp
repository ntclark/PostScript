
#include "PostScript objects/graphicsState.h"

    void graphicsState::gSave() {
    pJob -> graphicsStateStack.gSave();
    return;
    }


    void graphicsState::gRestore() {
    pJob -> graphicsStateStack.gRestore();
    return;
    }