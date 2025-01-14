#include "job.h"

    void graphicsState::initialize() {
    gdiParametersStack.initialize();
    pathParametersStack.initialize();
    psXformsStack.initialize(pJob);
    //fontStack.initialize();
    return;
    }