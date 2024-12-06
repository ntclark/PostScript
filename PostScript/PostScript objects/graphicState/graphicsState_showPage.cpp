
#include "job.h"

    void graphicsState::showPage() {
/*
    showpage 
        – showpage –

    transmits the contents of the current page to the current output device, causing
    any marks painted on the page to be rendered on the output medium. showpage
    then erases the current page and reinitializes ..... !
*/

    pPStoPDF -> CommitCurrentPage(pageWidthPoints,pageHeightPoints);

    initialize();

    return;
    }
