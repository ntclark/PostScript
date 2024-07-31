
#include "PostScript objects/graphicsState.h"

    void graphicsState::newpath(POINT_TYPE x,POINT_TYPE y) {
    currentUserSpacePoint = POINT_TYPE_NAN_POINT;
    currentPointsPoint = POINT_TYPE_NAN_POINT;
    gdiParametersStack.top() -> isPathActive = false;
    return;
    }


    void graphicsState::stroke() {
    if ( gdiParametersStack.top() -> isPathActive ) {
        EndPath(pPStoPDF -> GetDC());
if ( 1003 == GetLastError() )
printf("hello world");
        StrokePath(pPStoPDF -> GetDC());
    }
    gdiParametersStack.top() -> isPathActive = false;
if ( 1003 == GetLastError() )
printf("hello world");
    return;
    }


    void graphicsState::closepath() {
    if ( gdiParametersStack.top() -> isPathActive )
        EndPath(pPStoPDF -> GetDC());
if ( 1003 == GetLastError() )
printf("hello world");
    newpath();
    return;
    }


    void graphicsState::fillpath() {
    if ( gdiParametersStack.top() -> isPathActive )
        closepath();
    StrokeAndFillPath(pPStoPDF -> GetDC());
if ( 1003 == GetLastError() )
printf("hello world");
    return;
    }