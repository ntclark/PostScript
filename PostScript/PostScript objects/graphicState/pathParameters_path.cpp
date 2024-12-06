#include "job.h"

#include "pathParameters.h"

    void pathParameters::newpath(POINT_TYPE x,POINT_TYPE y) {
    currentUserSpacePoint = POINT_TYPE_NAN_POINT;
    currentPointsPoint = POINT_TYPE_NAN_POINT;
    isPathActive = false;
    return;
    }


    void pathParameters::stroke() {
    if ( isPathActive ) {
        EndPath(pPStoPDF -> GetDC());
        StrokePath(pPStoPDF -> GetDC());
    }
    isPathActive = false;
    return;
    }


    void pathParameters::closepath() {
    if ( isPathActive )
        EndPath(pPStoPDF -> GetDC());
    newpath();
    return;
    }


    void pathParameters::fillpath(boolean doRasterization) {
    if ( isPathActive )
        closepath();
    if ( doRasterization || defaultDoRasterization )
        rasterizePath();
    else
        StrokeAndFillPath(pPStoPDF -> GetDC());
    return;
    }


    void pathParameters::eofillpath(boolean doRasterization) {
    if ( isPathActive )
        closepath();
    int oldMode = SetPolyFillMode(pPStoPDF -> GetDC(),ALTERNATE);
    fillpath(doRasterization || defaultDoRasterization);//StrokeAndFillPath(pPStoPDF -> GetDC());
    SetPolyFillMode(pPStoPDF -> GetDC(),oldMode);
    return;
    }


    boolean pathParameters::savePath(boolean doSave) {
    boolean didSave = saveThePath;
    saveThePath = doSave;
    if ( ! ( NULL == pSavedPath ) )
        delete pSavedPath;
    pSavedPath = NULL;
    if ( saveThePath ) 
        pSavedPath = new savedPath();
    return didSave;
    }