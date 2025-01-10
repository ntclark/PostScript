#include "job.h"

#include "pathParameters.h"

    void pathParameters::openGeometry() {
    isPathActive = false;
    forwardToRenderer();
    return;
    }

#if 0
    void pathParameters::getGeometry(long *pCountVertices,BYTE **ppVertexTypes,POINT **ppVertexPoints) {

    EndPath(pPStoPDF -> GetDC());

    *pCountVertices = ::GetPath(pPStoPDF -> GetDC(),NULL,NULL,0);

    if ( 1 > *pCountVertices )
        return;

    *ppVertexPoints = new POINT[*pCountVertices];
    *ppVertexTypes = new BYTE[*pCountVertices];

    GetPath(pPStoPDF -> GetDC(),*ppVertexPoints,*ppVertexTypes,*pCountVertices);

    return;
    }
#endif

    void pathParameters::closeGeometry() {
    revertToGDI();
    return;
    }


    void pathParameters::renderGeometry() {
    job::pIGlyphRenderer -> Render();
    revertToGDI();
    return;
    }


    void pathParameters::RenderGeometry() {
    job::pIGlyphRenderer -> Render();
    return;
    }


    void pathParameters::closepath() {
    if ( isPathActive )
        EndPath(pPStoPDF -> GetDC());
    newpath();
    return;
    }


    void pathParameters::eofillpath() {
    if ( isPathActive )
        closepath();
    int oldMode = SetPolyFillMode(pPStoPDF -> GetDC(),ALTERNATE);
    fillpath();
    SetPolyFillMode(pPStoPDF -> GetDC(),oldMode);
    return;
    }