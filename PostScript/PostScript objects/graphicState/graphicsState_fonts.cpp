
#include "job.h"

#include "PostScript objects\graphicsState.h"
#include "PostScript objects\font.h"

    void graphicsState::setFont(class font *pFont) {

    pCurrentFont = pFont;

    pCurrentFont -> pSfntsArray = NULL;

    pJob -> push(pCurrentFont);

    pJob -> operatorBegin();

    pJob -> push(pJob -> pSfntsLiteral);

    pJob -> operatorWhere();

    pJob -> operatorEnd();

    long countGlyphs = 0;

    if ( pJob -> pTrueConstant == pJob -> pop() ) {

        pJob -> push(pJob -> pSfntsLiteral);
        pJob -> operatorGet();

        pCurrentFont -> pSfntsArray = reinterpret_cast<class array *>(pJob -> pop());

        pJob -> push(pCurrentFont);
        pJob -> push(pJob -> pCharStringsLiteral);
        pJob -> operatorGet();
        pJob -> operatorLength();

        countGlyphs = pJob -> pop() -> IntValue();

    }

    pJob -> push(pCurrentFont);
    pJob -> push(pJob -> pFontTypeLiteral);
    pJob -> operatorGet();

    object *pFontType = pJob -> pop();

    switch ( pFontType -> IntValue() ) {
    case 3:
        pCurrentFont -> fontType = font::ftype3;
        break;
    case 42:
        pCurrentFont -> fontType = font::ftype42;
        break;
    }

    pCurrentFont -> load(countGlyphs);

    return;
    }


    font *graphicsState::findFont(char *pszFontName) {

    /*font *pFont = */
    return font::findFont(pJob,pszFontName);
#if 0
    if ( NULL == pFont )
        return NULL;

    pCurrentFont = pFont;

    return pCurrentFont;
#endif
    }


    font *graphicsState::makeFont(matrix *pTransform,font *pCopyFrom) {
    font *pFont = new (pJob -> CurrentObjectHeap()) font(pJob,pCopyFrom);
    pFont -> setFontMatrix(pTransform);
    return pFont;
    }


    font *graphicsState::scaleFont(POINT_TYPE scaleFactor,font *pCopyFrom) {
    font *pFont = new (pJob -> CurrentObjectHeap()) font(pJob,pCopyFrom);
    pFont -> scalefont(scaleFactor);
    return pFont;
    }