
#include "PostScript.h"

#include "PostScript objects/graphicsState.h"

    void graphicsState::showPage() {
    pPStoPDF -> CommitCurrentPage(pageWidthPoints,pageHeightPoints);
    return;
    }
