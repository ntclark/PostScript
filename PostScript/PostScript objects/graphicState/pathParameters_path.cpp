#include "job.h"

#include "pathParameters.h"

#ifdef USE_RENDERER
#else
    void pathParameters::openGeometry() {
    isPathActive = false;
    forwardToRenderer();
    return;
    }


    void pathParameters::closeGeometry() {
    revertToLocal();
    return;
    }


    void pathParameters::renderGeometry() {
    job::pIGlyphRenderer -> Render();
    revertToLocal();
    return;
    }
#endif

    void pathParameters::RenderGeometry() {
    job::pIGlyphRenderer -> Render();
    return;
    }
