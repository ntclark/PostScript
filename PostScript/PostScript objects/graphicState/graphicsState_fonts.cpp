
#include "job.h"

#include "PostScript objects\graphicsState.h"
#include "PostScript objects\font.h"

    void graphicsState::setFont(class font *pFont) {

    pCurrentFont = pFont;

    pJob -> push(pCurrentFont);

    pJob -> operatorBegin();

    pJob -> push(pJob -> pSfntsLiteral);

    pJob -> operatorWhere();

    if ( pJob -> pTrueConstant == pJob -> pop() ) {

        pJob -> push(pJob -> pSfntsLiteral);
        pJob -> operatorGet();

        pCurrentFont -> pSfntsArray = reinterpret_cast<class array *>(pJob -> pop());

        pJob -> push(pCurrentFont);
        pJob -> push(pJob -> pCharStringsLiteral);
        pJob -> operatorGet();
        pJob -> operatorLength();

        pCurrentFont -> load(pJob -> pop() -> IntValue());

    } else
        pCurrentFont -> pSfntsArray = NULL;

    pJob -> operatorEnd();

    return;
    }