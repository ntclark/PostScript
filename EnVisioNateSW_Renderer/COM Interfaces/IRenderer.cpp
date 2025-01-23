
#include "EnVisioNateSW_Renderer.h"

#include <list>
#include <map>


    HRESULT Renderer::Prepare(HDC theHDC) {

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

    return S_OK;
    }


    HRESULT Renderer::put_TransformMatrix(UINT_PTR pXFormToDeviceSpace) {
    memcpy(&pIGraphicElements -> toDeviceSpace,(void *)pXFormToDeviceSpace,sizeof(XFORM));
    return S_OK;
    }


    HRESULT Renderer::get_IsPrepared(BOOL *pIsPrepared) {
    if ( ! pIsPrepared )
        return E_POINTER;
    *pIsPrepared = ! ( NULL == pID2D1Factory1 );
    return S_OK;
    }


    HRESULT Renderer::put_DownScale(FLOAT ds) {
    downScale = ds; 
    return S_OK;
    }


    HRESULT Renderer::put_Origin(POINTF ptOrigin) { 
    origin = ptOrigin;
    pIGraphicElements -> transformPoint(&origin.x,&origin.y);
    return S_OK;
    }


    HRESULT Renderer::Render() {

    if ( NULL == pID2D1Factory1 )
        return E_UNEXPECTED;

    if ( NULL == pIGraphicElements -> pFirstPath )
        return E_UNEXPECTED;

#if 1
SetLastError(0);

FILE *fDebug = fopen("C:\\temp\\x.ps","a+");

    GraphicElements::path *p = pIGraphicElements -> pFirstPath;

    doRasterize = pIGraphicElements -> pCurrentPath -> isFillPath;

    boolean firstPass = true;
    while ( ! ( p == NULL ) ) {
        p -> apply(true,firstPass,fDebug);
        p -> clear();
        if ( NULL == p -> pNext ) {
            delete p;
            break;
        }
        p = p -> pNext;
        delete p -> pPrior;
        firstPass = false;
    }

    internalRender();

    pIGraphicElements -> pFirstPath = NULL;
    pIGraphicElements -> pCurrentPath = NULL;

fclose(fDebug);

#else
    std::map<long,std::list<GraphicElements::path *>> paths;

    GraphicElements::path *p = pIGraphicElements -> pFirstPath;

    while ( ! ( p == NULL ) ) {
        if ( p -> isFillPath )
            paths[p -> hashCode].push_back(p);
        p = p -> pNext;
    }

    doRasterize = true;

    for ( std::pair<long,std::list<GraphicElements::path *>> pPair : paths ) {
        boolean firstPass = true;
        for ( GraphicElements::path *p : pPair.second ) {
            p -> apply(true,firstPass);
            p -> clear();
            firstPass = false;
        }
        internalRender();
    }

    paths.clear();

    p = pIGraphicElements -> pFirstPath;

    while ( ! ( p == NULL ) ) {
        if ( ! p -> isFillPath )
            paths[p -> hashCode].push_back(p);
        p = p -> pNext;
    }

    doRasterize = false;

    for ( std::pair<long,std::list<GraphicElements::path *>> pPair : paths ) {
        boolean firstPass = true;
        for ( GraphicElements::path *p : pPair.second ) {
            p -> apply(false,firstPass);
            p -> clear();
            firstPass = false;
        }
        internalRender();
    }
#endif
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