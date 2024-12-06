#include "job.h"

    void graphicsState::initialize() {
    gdiParametersStack.initialize();
    pathParametersStack.initialize();
    fontStack.initialize();
    return;
    }