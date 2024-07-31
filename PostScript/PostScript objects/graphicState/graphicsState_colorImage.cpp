
#include <Windows.h>
#include <Gdiplus.h>
#include <gdiplusinit.h>

#include "job.h"

#include "PostScript objects/graphicsState.h"
#include "PostScript objects/filter.h"

#include <iostream>
#include <streambuf>

    Gdiplus::Bitmap *pPadBitmap = NULL;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput = 0L;

    void graphicsState::colorImage() {

    //width height bits/comp matrix datasrc0 ... datasrcncomp - 1 multi ncomp colorimage -

    object *pCountColorComponents = pJob -> pop();
    object *pIsMultiSource = pJob -> pop();
    object *pSource = pJob -> pop();

    matrix *pMatrix = reinterpret_cast<matrix *>(pJob -> pop());
    object *pBitsPerComponent = pJob -> pop();
    object *pHeight = pJob -> pop();
    object *pWidth = pJob -> pop();

    DWORD cbData;
    BYTE *pbImage = NULL;
    class filter *pFilter = NULL;

    if ( object::objectType::filter == pSource -> ObjectType() ) {
        pFilter = reinterpret_cast<class filter *>(pSource);
        pbImage = pFilter -> getBinaryData(&cbData,NULL);
    } else
        return;

    WCHAR szwTempName[MAX_PATH];
    wcscpy(szwTempName,_wtempnam(NULL,NULL));

    FILE *fbmp = _wfopen(szwTempName,L"wb");
    fwrite(pbImage,1,cbData,fbmp);
    fclose(fbmp);

    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    HBITMAP hbmResult = NULL;

    Gdiplus::Bitmap *pBitmap = Gdiplus::Bitmap::FromFile(szwTempName, false);

    pBitmap -> GetHBITMAP(RGB(255, 255, 255), &hbmResult);

    DeleteFileW(szwTempName);

    Gdiplus::GdiplusShutdown(gdiplusToken);

    POINT_TYPE wUserSpace;
    POINT_TYPE hUserSpace;

    scalePoint((POINT_TYPE)PICA_FACTOR,(POINT_TYPE)PICA_FACTOR,&wUserSpace,&hUserSpace);

    moveto(0.0,0.0);

    HDC hdcSource = CreateCompatibleDC(pPStoPDF -> GetDC());
    SelectObject(hdcSource,hbmResult);

    StretchBlt(pPStoPDF -> GetDC(),(long)((float)PICA_FACTOR * currentPointsPoint.x),(long)(((float)PICA_FACTOR * currentPointsPoint.y) + hUserSpace),(long)wUserSpace,-(long)hUserSpace,
                hdcSource,0,0,pWidth -> IntValue(),pHeight -> IntValue(),SRCCOPY);

    DeleteObject(hbmResult);
    DeleteDC(hdcSource);

    pFilter -> releaseData();

    return;
    }