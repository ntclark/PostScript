#include "job.h"

#include "pathParameters.h"

#if USE_RENDERER
#else
    void pathParameters::dot(GS_POINT at,POINT_TYPE radius) {

    POINT_TYPE ptx;
    POINT_TYPE pty;

    transformPoint(at.x,at.y,&ptx,&pty);

    BOOL rv = Arc(pPStoPDF -> GetDC(),
                            std::lround((POINT_TYPE)ptx - radius),
                            std::lround((POINT_TYPE)pty - radius),
                            std::lround((POINT_TYPE)ptx + radius),
                            std::lround((POINT_TYPE)pty + radius),
                            std::lround((POINT_TYPE)ptx + radius),
                            std::lround((POINT_TYPE)pty + radius / 2),
                            std::lround((POINT_TYPE)ptx + radius),
                            std::lround((POINT_TYPE)pty + radius / 2));
    return;
    }



    void pathParameters::outlinePage(long pageWidthPoints,long pageHeightPoints) {

return;

    BeginPath(pPStoPDF -> GetDC());

    moveto(72.0,72.0);

    lineto(pageWidthPoints - 72.0f,72.0f);
    lineto(pageWidthPoints - 72.0f,pageHeightPoints - 72.0f);
    lineto(72.0,pageHeightPoints - 72.0f);

    lineto(72.0f,72.0f);

    EndPath(pPStoPDF -> GetDC());
    StrokePath(pPStoPDF -> GetDC());

    return;
    }
#endif