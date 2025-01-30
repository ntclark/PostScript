
#include "EnVisioNateSW_Renderer.h"

#include <list>
#include <map>
#include <algorithm>

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
    return S_OK;
    }


    HRESULT Renderer::Render() {

    if ( NULL == pID2D1Factory1 )
        return E_UNEXPECTED;

    if ( NULL == pIGraphicElements -> pFirstPath )
        return E_UNEXPECTED;

    std::map<long,std::list<GraphicElements::path *> *> strokePathsMap;
    std::map<long,std::list<GraphicElements::path *> *> fillPathsMap;

    GraphicElements::path *p = pIGraphicElements -> pFirstPath;

    long fileOccurance = 0L;

    std::vector<std::pair<long,long>> fillPathSorter;
    std::vector<std::pair<long,long>> strokePathSorter;

    while ( ! ( p == NULL ) ) {

        if ( p -> isFillPath ) {
            if ( fillPathsMap.find(p -> hashCode) == fillPathsMap.end() ) {
                fillPathsMap[p -> hashCode] = new std::list<GraphicElements::path *>();
                fileOccurance++;
                fillPathSorter.push_back(std::pair<long,long>(p -> hashCode,fileOccurance));
            }
            fillPathsMap[p -> hashCode] -> push_back(p);
        } else {
            if ( strokePathsMap.find(p -> hashCode) == strokePathsMap.end() ) {
                strokePathsMap[p -> hashCode] = new std::list<GraphicElements::path *>();
                fileOccurance++;
                strokePathSorter.push_back(std::pair<long,long>(p -> hashCode,fileOccurance));
            }
            strokePathsMap[p -> hashCode] -> push_back(p);
        }

        p = p -> pNext;
    }

    std::sort(fillPathSorter.begin(), fillPathSorter.end(), [=](std::pair<long,long> &a, std::pair<long,long > &b) { return a.second < b.second; });

    for ( std::pair<long,long> sortedPair : fillPathSorter ) {

        std::list<GraphicElements::path *> *pList = fillPathsMap[sortedPair.first];

        pIGraphicParameters -> resetParameters(pList -> front() -> szLineSettings);

        for ( GraphicElements::path *p : *pList ) {

if ( GraphicElements::primitive::type::strokePathMarker == p -> pLastPrimitive -> theType ) {
MessageBox(NULL,"stroke path in fill collection","Error",MB_OK);
continue;
}
            if ( GraphicElements::primitive::type::fillPathMarker == p -> pLastPrimitive -> theType  )
                GraphicElements::path::pathAction pa = p -> apply(true,NULL);
        }

        fillRender();

    }

    for ( std::pair<long,std::list<GraphicElements::path *>*> pPair : fillPathsMap )
        delete pPair.second;

    fillPathsMap.clear();

    std::sort(strokePathSorter.begin(), strokePathSorter.end(), [=](std::pair<long,long> &a, std::pair<long, long > &b) { return a.second < b.second; });

    for ( std::pair<long,long> sortedPair : strokePathSorter ) {

        std::list<GraphicElements::path *> *pList = strokePathsMap[sortedPair.first];

        pIGraphicParameters -> resetParameters(pList -> front() -> szLineSettings);

        for ( GraphicElements::path *p : *pList ) {

if ( GraphicElements::primitive::type::fillPathMarker == p -> pLastPrimitive -> theType ) {
MessageBox(NULL,"fill path in stroke collection","Error",MB_OK);
continue;
}
            if ( GraphicElements::primitive::type::strokePathMarker == p -> pLastPrimitive -> theType  )
                GraphicElements::path::pathAction pa = p -> apply(false,NULL);
        }

        strokeRender();

    }

    for ( std::pair<long,std::list<GraphicElements::path *>*> pPair : strokePathsMap )
        delete pPair.second;

    strokePathsMap.clear();

    pIGraphicElements -> pFirstPath = NULL;
    pIGraphicElements -> pCurrentPath = NULL;

    return S_OK;
    }


    HRESULT Renderer::GetParametersBundle(UINT_PTR **pptrBundleSpace) {
    *pptrBundleSpace = (UINT_PTR *)CoTaskMemAlloc(sizeof(GraphicParameters::values));
    memcpy((void *)*pptrBundleSpace,pIGraphicParameters -> valuesStack.top(),sizeof(GraphicParameters::values));
    return S_OK;
    }


    HRESULT Renderer::SetParametersBundle(UINT_PTR *ptrBundleSpace) {
    memcpy(pIGraphicParameters -> valuesStack.top(),(void *)ptrBundleSpace,sizeof(GraphicParameters::values));
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