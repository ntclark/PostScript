
#include "EnVisioNateSW_GlyphRenderer.h"

    HRESULT GlyphRenderer::Prepare(HDC theHDC) {

    hdc = theHDC;

    setupRenderer();

    pID2D1PathGeometry = NULL;

    HRESULT hr = pID2D1Factory1 -> CreatePathGeometry(&pID2D1PathGeometry);
    if ( ! ( S_OK == hr ) )
{
MessageBox(NULL,"BAD","BAD",MB_OK);
        return hr;
}

    hr = pID2D1PathGeometry -> Open(&pID2D1GeometrySink);
    if ( ! ( S_OK == hr ) )
{
MessageBox(NULL,"BAD","BAD",MB_OK);
        return hr;
}
    //pIGraphicElements -> isFigureStarted = false;

    return S_OK;
    }


    HRESULT GlyphRenderer::put_TransformMatrix(UINT_PTR pXFormToDeviceSpace) {
    memcpy(&pIGraphicElements -> toDeviceSpace,(void *)pXFormToDeviceSpace,sizeof(XFORM));
    return S_OK;
    }


    HRESULT GlyphRenderer::get_IsPrepared(BOOL *pIsPrepared) {
    if ( ! pIsPrepared )
        return E_POINTER;
    *pIsPrepared = ! ( NULL == pID2D1Factory1 );
    return S_OK;
    }


    HRESULT GlyphRenderer::Render() {

    if ( NULL == pID2D1Factory1 )
        return E_UNEXPECTED;

    if ( ! ( NULL == pID2D1GeometrySink ) ) {

        HRESULT rc = pID2D1GeometrySink -> Close();

        pID2D1GeometrySink -> Release();
        pID2D1GeometrySink = NULL;

    }

    pID2D1DCRenderTarget -> BeginDraw();

    //if ( doFill )
        pID2D1DCRenderTarget -> FillGeometry(pID2D1PathGeometry,(ID2D1Brush *)pID2D1SolidColorBrush);
    //else
        //pID2D1DCRenderTarget -> DrawGeometry(pID2D1PathGeometry,(ID2D1Brush *)pID2D1SolidColorBrush,pIGraphicParameters -> lineWidth,pIGraphicParameters -> pID2D1StrokeStyle1);

    D2D1_TAG tag1;
    D2D1_TAG tag2;

    HRESULT hr = pID2D1DCRenderTarget -> EndDraw(&tag1,&tag2);
{
if ( ! ( S_OK == hr ) )
MessageBox(NULL,"BAD","BAD",MB_OK);
}
    pID2D1PathGeometry -> Release();

    pID2D1PathGeometry = NULL;

    hr = pID2D1Factory1 -> CreatePathGeometry(&pID2D1PathGeometry);
    if ( ! ( S_OK == hr ) )
{
MessageBox(NULL,"BAD","BAD",MB_OK);
        return hr;
}

    hr = pID2D1PathGeometry -> Open(&pID2D1GeometrySink);
    if ( ! ( S_OK == hr ) )
{
MessageBox(NULL,"BAD","BAD",MB_OK);
        return hr;
}
    return S_OK;
    }

    HBITMAP createTemporaryBitmap(long cx,long cy,long pitch,long bitsPerComponent,BYTE *pBits) {

    BITMAPINFO bitMapInfo;

    memset(&bitMapInfo,0,sizeof(bitMapInfo));

    bitMapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitMapInfo.bmiHeader.biWidth = cx;
    bitMapInfo.bmiHeader.biHeight = cy;
    bitMapInfo.bmiHeader.biPlanes = 1;
    bitMapInfo.bmiHeader.biBitCount = 3 * (unsigned short)bitsPerComponent;
    bitMapInfo.bmiHeader.biCompression = BI_RGB;

    long widthBytes = 3 * cx;
    long stride = ((((cx * 24) + 31) & ~31) >> 3);
    long padding = stride - widthBytes;

    BYTE *pRGBBits = new BYTE[(widthBytes + padding) * abs(cy)];

    memset(pRGBBits,0x0,(widthBytes + padding) * abs(cy));

    long n = 0;
    for ( long j = cy - 1; j > -1; j-- ) {
        for ( long k = 0; k < cx; k++ ) {
            pRGBBits[n] = 0xFF - pBits[j * cx + k];
            pRGBBits[n + 1] = pRGBBits[n];
            pRGBBits[n + 2] = pRGBBits[n];
            n += 3;
        }
        n += padding;
    }

    HBITMAP hBitmap = CreateDIBSection(NULL,&bitMapInfo,DIB_RGB_COLORS,NULL,NULL,0L);

    SetDIBits(NULL,hBitmap,0,cy,pRGBBits,&bitMapInfo,DIB_RGB_COLORS);

//SetBitmapBits(hBitmap,cx * cy * 3 * bitsPerComponent,pBits);

    return hBitmap;
    }