
#include "job.h"

    void font::setFontMatrix(class matrix *pMatrix) {
    pIFont -> put_Matrix((UINT_PTR)pMatrix -> Values());
    return;
    }


    XFORM *font::getMatrix() {
    static XFORM xForm;
    pIFont -> get_Matrix((LPVOID)&xForm);
    return &xForm;
    }