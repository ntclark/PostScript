
#include "FontManager.h"

    HRESULT FontManager::RenderGlyph(HDC hdc,unsigned short bGlyph,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,POINTF *pStartPoint,POINTF *pEndPoint) {
    font *pFont = static_cast<font *>(pIFont_Current);
    return pFont -> RenderGlyph(hdc,bGlyph,pPSXform,pXformToDeviceSpace,pStartPoint,pEndPoint);
    }