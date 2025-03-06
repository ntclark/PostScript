#include "job.h"

    void graphicsState::initialize() {
    pathParametersStack.initialize();
    psXformsStack.initialize(pJob);
    return;
    }