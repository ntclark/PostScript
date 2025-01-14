
#include "job.h"

    void font::setFontMatrix(XFORM *pXForm) {
    pIFont -> put_Matrix((UINT_PTR)pXForm);
char szX[128];
sprintf_s<128>(szX,"setFontMatrix: eM11 %f, eM12 %f, eM21 %f, eM22 %f\n",pXForm -> eM11,pXForm -> eM12,
                                                                        pXForm -> eM21,pXForm -> eM22);
OutputDebugStringA(szX);
    return;
    }


    XFORM *font::getFontMatrix() {
    static XFORM xForm;
    pIFont -> get_Matrix((LPVOID)&xForm);
char szX[128];
sprintf_s<128>(szX,"getFontMatrix: eM11 %f, eM12 %f, eM21 %f, eM22 %f\n",xForm.eM11,xForm.eM12,xForm.eM21,xForm.eM22);
OutputDebugStringA(szX);
    return &xForm;
    }