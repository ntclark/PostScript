
#include "FontManager.h"

    static HDC hdcCompat = NULL;

    HRESULT FontManager::GetGlyphGeometry(HDC hdc,BYTE bGlyph,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,
                                            POINTF *pStartPoint,POINTF *pEndPoint,LONG *pcbPoints,BYTE *pbVertexTypes,POINT *pVertices) {

    if ( ! pcbPoints )
        return E_POINTER;

    if ( NULL == hdcCompat )
        hdcCompat = CreateCompatibleDC(hdc);

    if ( ! ( NULL == pbVertexTypes ) ) {
        if ( ! pVertices)
            return E_POINTER;
        *pcbPoints = GetPath(hdcCompat,pVertices,pbVertexTypes,*pcbPoints);
        DeleteDC(hdcCompat);
        hdcCompat = NULL;
        return S_OK;
    }

    BeginPath(hdcCompat);

    drawType42Glyph(hdcCompat,bGlyph,pPSXform,pXformToDeviceSpace,NULL,pStartPoint,pEndPoint);

    EndPath(hdcCompat);

    *pcbPoints = GetPath(hdcCompat,NULL,NULL,0L);

    return S_OK;
    }