#include "job.h"

    void graphicsState::setFont(class font *pFont) {
    font::setFont(pFont);
    return;
    }


    font *graphicsState::findFont(char *pszFontName) {
    return font::findFont(pJob,pszFontName);
    }


    font *graphicsState::makeFont(array *pTransform,font *pCopyFrom) {
    return font::makeFont(pTransform,pCopyFrom);
    }


    font *graphicsState::makeFont(matrix *pTransform,font *pCopyFrom) {
    return font::makeFont(pTransform,pCopyFrom);
    }


    font *graphicsState::scaleFont(POINT_TYPE scaleFactor,font *pCopyFrom) {
    return font::scaleFont(scaleFactor,pCopyFrom);
    }