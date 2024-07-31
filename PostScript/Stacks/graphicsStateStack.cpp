
#include "Stacks\graphicsStateStack.h"

#include "PostScript objects\graphicsState.h"
#include "PostScript objects\font.h"

    graphicsStateStack::graphicsStateStack(job *pj) : pJob(pj) {
    return;
    }

    graphicsStateStack::~graphicsStateStack() {
    return;
    }


    void graphicsStateStack::gSave() {

    if ( current() -> gdiParametersStack.top() -> isPathActive ) {
BOOL rv = EndPath(pPStoPDF -> GetDC());
rv = FlattenPath(pPStoPDF -> GetDC());
long cntPoints = GetPath(pPStoPDF -> GetDC(),NULL,NULL,0);
printf("hello world");
    }

    graphicsState *pgs = new graphicsState(pJob,NULL);
    if ( ! ( NULL == current() -> pCurrentFont ) ) 
        current() -> pCurrentFont -> gSave();
    pgs -> copyFrom(current());
    std::stack<graphicsState *>::push(pgs);
    return;
    }


    graphicsState *graphicsStateStack::gRestore() {

    //if ( 0 == size() ) 
    //    throw stackunderflow("There was an underflow in the PostScript graphics state stack.");

    if ( 1 == size() )
        return current();

    if ( ! ( NULL == current() -> pCurrentFont ) ) 
        current() -> pCurrentFont -> gRestore();

    delete current();

    std::stack<graphicsState *>::pop();

    //current() -> resetDC();

    return current();
    }


    graphicsState *graphicsStateStack::current() {
    //if ( 0 == size() ) 
    //    throw stackunderflow("There was an underflow in the PostScript graphics state stack.");
    return std::stack<graphicsState *>::top();
    }
