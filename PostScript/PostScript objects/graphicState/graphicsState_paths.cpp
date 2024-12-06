#include "job.h"

#include "pathParameters.h"

    void graphicsState::newpath(POINT_TYPE x,POINT_TYPE y) {
    pathParametersStack.top() -> newpath(x,y);
    return;
    }


    void graphicsState::stroke() {
    pathParametersStack.top() -> stroke();
    return;
    }


    void graphicsState::closepath() {
    pathParametersStack.top() -> closepath();
    return;
    }


    void graphicsState::fillpath(boolean doRasterization) {
    pathParametersStack.top() -> fillpath(doRasterization);
    return;
    }


    void graphicsState::eofillpath(boolean doRasterization) {
    pathParametersStack.top() -> eofillpath(doRasterization);
    return;
    }


    boolean graphicsState::savePath(boolean doSave) {
    return pathParametersStack.top() -> savePath(doSave);
    }


    boolean graphicsState::setDefaultToRasterize(boolean doRasterization) {
    return pathParametersStack.top() -> setDefaultToRasterize(doRasterization);
    }