
#include "job.h"

    void font::setFontMatrix(XFORM *pXForm) {
    pIFont -> put_Matrix((UINT_PTR)pXForm);
    return;
    }


    XFORM *font::getFontMatrix() {
    static XFORM xForm;
    pIFont -> get_Matrix((LPVOID)&xForm);
    return &xForm;
    }