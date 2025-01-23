#include "job.h"

#include "pathParameters.h"

    void graphicsState::newpath() {
    pathParametersStack.top() -> newpath();
    currentUserSpacePoint = POINT_TYPE_NAN_POINT;
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


    void graphicsState::fillpath() {
    pathParametersStack.top() -> fillpath();
    return;
    }


    void graphicsState::eofillpath() {
    pathParametersStack.top() -> eofillpath();
    return;
    }