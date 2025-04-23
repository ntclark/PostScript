/*
Copyright 2025 EnVisioNate LLC

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the “Software”), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is the MIT License
*/

#include "Renderer.h"

    Renderer::GraphicElements::GraphicElements(Renderer *pp) : pParent(pp) {}


    Renderer::GraphicElements::~GraphicElements() {
    clearPaths();
    return;
    }


    void Renderer::GraphicElements::clearPaths() {
    path *p = pFirstPath;
    while ( ! ( NULL == p ) ) {
        path *pNextPath = p -> pNextPath;
        p -> clear();
        delete p;
        p = pNextPath;
    }
    pFirstPath = NULL;
    pCurrentPath = NULL;
    return;
    }


    uint8_t *Renderer::GraphicElements::getPixelsFromJpeg(uint8_t *pJPEGData,long dataSize,long width,long height) {

    IWICImagingFactory *pIWICImagingFactory = NULL;
    IWICBitmapDecoder *pIWICBitmapDecoder = NULL;
    IWICStream *pIWICStream = NULL;

    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory,NULL,CLSCTX_INPROC_SERVER,IID_IWICImagingFactory,(LPVOID*) &pIWICImagingFactory);

    hr = pIWICImagingFactory -> CreateStream(&pIWICStream);

    hr = pIWICStream -> InitializeFromMemory((WICInProcPointer)pJPEGData,(DWORD)dataSize);

    hr = pIWICImagingFactory -> CreateDecoder(GUID_ContainerFormatJpeg, NULL, &pIWICBitmapDecoder);

    hr = pIWICBitmapDecoder -> Initialize(pIWICStream,WICDecodeMetadataCacheOnDemand);

    IWICBitmapFrameDecode *pIWICBitmapFrameDecode = NULL;

    hr = pIWICBitmapDecoder -> GetFrame(0,&pIWICBitmapFrameDecode);

    IWICBitmap *pIBitmap = NULL;
    IWICBitmapLock *pILock = NULL;

    WICRect rcLock = { 0, 0, (int)width, (int)height};

    hr = pIWICImagingFactory -> CreateBitmapFromSource(pIWICBitmapFrameDecode,WICBitmapCacheOnDemand,&pIBitmap);

    hr = pIBitmap -> Lock(&rcLock, WICBitmapLockRead, &pILock);

    UINT cbBufferSize = 0;
    uint8_t *pv = NULL;

    hr = pILock -> GetDataPointer(&cbBufferSize, &pv);

    uint8_t *pbPixels = new uint8_t[cbBufferSize];

    memcpy(pbPixels,pv,cbBufferSize);

    pILock -> Release();
    pIBitmap -> Release();
    pIWICBitmapDecoder -> Release();
    pIWICBitmapFrameDecode -> Release();
    pIWICImagingFactory -> Release();

    return pbPixels;
    }


    void Renderer::GraphicElements::calcInverseTransform() {

    double theMatrix[3][3];
    double theMatrixInverted[3][3];

    theMatrix[0][0] = (double)toDeviceSpace.eM11;
    theMatrix[0][1] = (double)toDeviceSpace.eM12;
    theMatrix[0][2] = (double)toDeviceSpace.eDx;
    theMatrix[1][0] = (double)toDeviceSpace.eM21;
    theMatrix[1][1] = (double)toDeviceSpace.eM22;
    theMatrix[1][2] = (double)toDeviceSpace.eDy;

    theMatrix[2][0] = 0.0;
    theMatrix[2][1] = 0.0;
    theMatrix[2][2] = 1.0f;

    Mx3Inverse(&theMatrix[0][0],&theMatrixInverted[0][0]);

    toUserSpace.eM11 = (FLOAT)theMatrixInverted[0][0];
    toUserSpace.eM12 = (FLOAT)theMatrixInverted[0][1];
    toUserSpace.eM21 = (FLOAT)theMatrixInverted[1][0];
    toUserSpace.eM22 = (FLOAT)theMatrixInverted[1][1];

    toUserSpace.eDx = (FLOAT)theMatrixInverted[0][2];
    toUserSpace.eDy = (FLOAT)theMatrixInverted[1][2];

    return;
    }


    void Renderer::GraphicElements::scalePoint(FLOAT *px,FLOAT *py) {
    FLOAT xResult = toDeviceSpace.eM11 * *px + (FLOAT)fabs(toDeviceSpace.eM12) * *py;
    FLOAT yResult = toDeviceSpace.eM21 * *px + (FLOAT)fabs(toDeviceSpace.eM22) * *py;
    *px = xResult;
    *py = yResult;
    return;
    }


    void Renderer::GraphicElements::transformPoint(FLOAT *px,FLOAT *py) {
    FLOAT xResult = toDeviceSpace.eM11 * *px + toDeviceSpace.eM12 * *py + toDeviceSpace.eDx;
    FLOAT yResult = toDeviceSpace.eM21 * *px + toDeviceSpace.eM22 * *py + toDeviceSpace.eDy;
    *px = xResult;
    *py = yResult;
    return;
    }


    void Renderer::GraphicElements::unTransformPoint(FLOAT *px,FLOAT *py) {
    FLOAT xResult = toUserSpace.eM11 * *px + toUserSpace.eM12 * *py + toUserSpace.eDx;
    FLOAT yResult = toUserSpace.eM21 * *px + toUserSpace.eM22 * *py + toUserSpace.eDy;
    *px = xResult;
    *py = yResult;
    return;
    }


    void Renderer::GraphicElements::transformPoint(POINTF *ptIn,POINTF *ptOut) {
    FLOAT xResult = toDeviceSpace.eM11 * ptIn -> x + toDeviceSpace.eM12 * ptIn -> y + toDeviceSpace.eDx;
    FLOAT yResult = toDeviceSpace.eM21 * ptIn -> x + toDeviceSpace.eM22 * ptIn -> y + toDeviceSpace.eDy;
    ptOut -> x = xResult;
    ptOut -> y = yResult;
    return;
    }


    void Renderer::GraphicElements::transformPoint(D2D1_POINT_2F *ptIn,D2D1_POINT_2F *ptOut) {
    FLOAT xResult = toDeviceSpace.eM11 * ptIn -> x + toDeviceSpace.eM12 * ptIn -> y + toDeviceSpace.eDx;
    FLOAT yResult = toDeviceSpace.eM21 * ptIn -> x + toDeviceSpace.eM22 * ptIn -> y + toDeviceSpace.eDy;
    ptOut -> x = xResult;
    ptOut -> y = yResult;
    return;
    }

