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

    HBITMAP getHBITMAPFromPixels(long cx,long cy,long bitsPerComponent,uint8_t *pBits);

    HRESULT Renderer::GraphicElements::PostScriptImage(HDC hdc,UINT_PTR pbBits,UINT_PTR /*xForm*/ pvImageXForm,UINT_PTR /*xForm*/ pvPSCurrentCTM,
                                                            long width,long height,long bitsPerComponent) {

    // The PostScript convention is scanning from bottom to top. Passing positive height will create
    // a bitmap that is scanned top to bottom (windows convention), because height is 
    // negated in the function.
    //
    HBITMAP hbmImage = getHBITMAPFromPixels(width,labs(height),bitsPerComponent,(uint8_t *)pbBits);

    /*
    The image operators impose a coordinate system on the source image. 
    They consider the source image to be a rectangle h units high and w units wide. 
    Each sample occupies one square unit. The origin (0, 0) is in the 
    lower-left corner, with coordinates ranging from 0 to w horizontally and 0 to h vertically.
        .
        .
    The correspondence between this source image coordinate system (or image
    space) and user space is specified by a special matrix. This is a general linear
    transformation that defines a mapping from user space to image space; that is, it
    transforms user space coordinates to the corresponding coordinates in image
    space.

    !!! Why ??? It would seem better to translate image space to user space ?!?!?

    */

    XFORM *pImageXForm = (XFORM *)pvImageXForm;

    /*
    An image that happens to use the PostScript conventions (scanning from left to
    right and bottom to top) can be described by the image matrix

        [w 0 0 h 0 0]

    !!! And what if it doesn't ???!!!! GEEZ WTF !!

    An image that is scanned from left to right and top to bottom (a commonly used order) is
    described by the image matrix

        [w 0 0 –h 0 h]
    */

    /*
    I think to properly cover the possibility that this matrix will be something besides 
    the simple matrix above, I have to transform (1,1) with it, in which case I would
    like to have an example!!!
    */
#if _DEBUG
    if ( ! ( labs(width) == (long)pImageXForm -> eM11 ) && ! ( labs(height) == (long)abs(pImageXForm -> eM22)) ) {
        __debugbreak();
    }
#endif

    FLOAT fWidthImageSpace = pImageXForm -> eM11 + pImageXForm -> eM12;
    FLOAT fHeightImageSpace = pImageXForm -> eM21 + pImageXForm -> eM22;

    long widthImageSpace = (long)fWidthImageSpace;
    long heightImageSpace = (long)fHeightImageSpace;

    /*
    What is absolutely NOT clear here, and in all of the Adobe documentation about images, is why
    do we have BOTH width/height AND this transformation matrix, that appears to simply replicate 
    width/height and offers no other information !?!?!?! WHAT THE FUCK !!!! this is yet
    another example of horrible documentation that leaves obvious questions (that it also GENERATES)
    completely un-answered!
    */

#if 0

    XFORM xFormImageInverse;
    calcInverseTransform(pImageXForm,&xFormImageInverse);//{inverse[0][0],inverse[0][1],inverse[1][0],inverse[1][1],inverse[2][1],inverse[2][2]};

    FLOAT fWidthUserSpace = xFormImageInverse.eM11 * fWidthImageSpace + xFormImageInverse.eM12 * fHeightImageSpace + xFormImageInverse.eDx;
    FLOAT fHeightUserSpace = xFormImageInverse.eM21 * fWidthImageSpace + xFormImageInverse.eM22 * fHeightImageSpace + xFormImageInverse.eDy;;

    XFORM *pXFormToPageSpace = (XFORM *)pvPSCurrentCTM;
    //FLOAT xResult = pXFormToPageSpace -> eM11 + pXFormToPageSpace -> eM12;
    //FLOAT yResult = pXFormToPageSpace -> eM21 + pXFormToPageSpace -> eM22;

    //long widthDeviceSpace = (long)(toDeviceSpace.eM11 * xResult + toDeviceSpace.eM12 * yResult);
    //long heightDeviceSpace = (long)(toDeviceSpace.eM21 * xResult + toDeviceSpace.eM22 * yResult);

    long widthDeviceSpace = (long)(toDeviceSpace.eM11 * fWidthUserSpace + toDeviceSpace.eM12 * fHeightUserSpace + toDeviceSpace.eDx);
    long heightDeviceSpace = (long)(toDeviceSpace.eM21 * fWidthUserSpace + toDeviceSpace.eM22 * fHeightUserSpace + toDeviceSpace.eDy);

#else

    XFORM *pXFormToPageSpace = (XFORM *)pvPSCurrentCTM;

    FLOAT fWidthUserSpace = pXFormToPageSpace -> eM11 + pXFormToPageSpace -> eM12;
    FLOAT fHeightUserSpace = pXFormToPageSpace -> eM21 + pXFormToPageSpace -> eM22;

    long widthDeviceSpace = (long)(toDeviceSpace.eM11 * fWidthUserSpace + toDeviceSpace.eM12 * fHeightUserSpace);
    long heightDeviceSpace = (long)(toDeviceSpace.eM21 * fWidthUserSpace + toDeviceSpace.eM22 * fHeightUserSpace);

#endif

    long xDeviceSpace = (long)(toDeviceSpace.eM11 * pXFormToPageSpace -> eDx + toDeviceSpace.eM12 * pXFormToPageSpace -> eDy + toDeviceSpace.eDx);
    long yDeviceSpace = (long)(toDeviceSpace.eM21 * pXFormToPageSpace -> eDx + toDeviceSpace.eM22 * pXFormToPageSpace -> eDy + toDeviceSpace.eDy);

    if ( 0 > heightImageSpace )
        yDeviceSpace -= labs(heightDeviceSpace);

    images.push_back(new image(hbmImage,xDeviceSpace,yDeviceSpace,widthDeviceSpace,heightDeviceSpace,widthImageSpace,heightImageSpace));

    return S_OK;
    }


    HRESULT Renderer::GraphicElements::PostScriptJpegImage(HDC hdc,UINT_PTR pJPEGData,long dataSize,
                                                            UINT_PTR pvImageXForm,UINT_PTR pvPSCurrentCTM,long width,long height) {
    if ( NULL == pJPEGData )
        return E_INVALIDARG;
    uint8_t *pbImage = getPixelsFromJpeg((uint8_t *)pJPEGData,dataSize,width,height);
    HRESULT rc = PostScriptImage(hdc,(UINT_PTR)pbImage,pvImageXForm,pvPSCurrentCTM,width,height,8);
    delete [] pbImage;
    return rc;
    }


    HRESULT Renderer::GraphicElements::PostScriptStencilMask(long cbMaskingBytes,UINT_PTR pbMaskingBytes,
                                                                BOOL polarity,COLORREF currentColor,
                                                                UINT_PTR pvImageXForm,UINT_PTR pvPSCurrentCTM,long width,long height) {

    uint8_t *pBytes = new uint8_t[cbMaskingBytes];
    memcpy(pBytes,(uint8_t *)pbMaskingBytes,cbMaskingBytes);

    XFORM *pImageXForm = (XFORM *)pvImageXForm;
    long widthImageSpace = (long)(pImageXForm -> eM11 + pImageXForm -> eM12);
    long heightImageSpace = (long)(pImageXForm -> eM21 + pImageXForm -> eM22);

    XFORM *pXForm = (XFORM *)pvPSCurrentCTM;
    long widthDeviceSpace = (long)(toDeviceSpace.eM11 * (pXForm -> eM11 + pXForm -> eM12) + toDeviceSpace.eM12 * (pXForm -> eM21 + pXForm -> eM22));
    long heightDeviceSpace = (long)(toDeviceSpace.eM21 * (pXForm -> eM11 + pXForm -> eM12) + toDeviceSpace.eM22 * (pXForm -> eM21 + pXForm -> eM22));

    long xDeviceSpace = (long)(toDeviceSpace.eM11 * pXForm -> eDx + toDeviceSpace.eM12 * pXForm -> eDy + toDeviceSpace.eDx);
    long yDeviceSpace = (long)(toDeviceSpace.eM21 * pXForm -> eDx + toDeviceSpace.eM22 * pXForm -> eDy + toDeviceSpace.eDy);

    imageMasks.push_back(new imageMask(pBytes,currentColor,xDeviceSpace,yDeviceSpace,widthDeviceSpace,heightDeviceSpace,
                                            widthImageSpace,heightImageSpace,polarity));

    return S_OK;
    }


    HRESULT Renderer::GraphicElements::NonPostScriptImage(HDC hdc,HBITMAP hBitmap,FLOAT x0,FLOAT y0,FLOAT width,FLOAT height) {

    if ( NULL == hBitmap )
        return E_INVALIDARG;

    POINTF ptDevice{x0,y0};
    transformPoint(&toDeviceSpace,&ptDevice,&ptDevice);

    POINTF ptSize{width,height};
    scalePoint(&toDeviceSpace,&ptSize.x,&ptSize.y);

    HDC hdcSource = CreateCompatibleDC(hdc);
    SelectObject(hdcSource,hBitmap);

    long cb = GetObject(hBitmap,0,NULL);

    BYTE *pBitmap = new BYTE[cb];
    GetObject(hBitmap,cb,pBitmap);

    BITMAPINFOHEADER *pHeader = (BITMAPINFOHEADER *)pBitmap;

    SIZE sizeBM{pHeader -> biWidth,pHeader -> biHeight};

    delete [] pBitmap;

    if ( 0.0f == width || 0.0f == height ) {
        ptSize.x = (FLOAT)sizeBM.cx;
        ptSize.y = (FLOAT)sizeBM.cy;
        BitBlt(hdc,(long)ptDevice.x,(long)ptDevice.y,(long)ptSize.x,(long)ptSize.y,hdcSource,0,0,SRCCOPY);
    } else
        StretchBlt(hdc,(long)ptDevice.x,(long)ptDevice.y,(long)ptSize.x,(long)ptSize.y,hdcSource,0,0,sizeBM.cx,sizeBM.cy,SRCCOPY);

    DeleteDC(hdcSource);

    return S_OK;
    }


    HRESULT Renderer::GraphicElements::NonPostScriptJpegImage(HDC hdc,UINT_PTR pJpegData,long dataSize,FLOAT x0,FLOAT y0,FLOAT width,FLOAT height) {

    if ( NULL == pJpegData )
        return E_INVALIDARG;

    uint8_t *pbImage = getPixelsFromJpeg((uint8_t *)pJpegData,dataSize,(long)width,(long)height);

    HBITMAP hbmImage = getHBITMAPFromPixels((long)width,(long)-height,8,pbImage);

    HRESULT rc = NonPostScriptImage(hdc,hbmImage,x0,y0,width,height);

    DeleteObject(hbmImage);

    delete [] pbImage;

    return S_OK;
    }


    HRESULT Renderer::applyImage(HDC hdc,GraphicElements::image *pImage) {
    HDC hdcSource = CreateCompatibleDC(hdc);
    SelectObject(hdcSource,pImage -> hBitmap);
    BOOL rv = StretchBlt(hdc,pImage -> locPixels.x,pImage -> locPixels.y,pImage -> widthPixels,labs(pImage -> heightPixels),
                hdcSource,0,0,pImage -> widthImage,labs(pImage -> heightImage),SRCCOPY);
    DeleteDC(hdcSource);
    return S_OK;
    }


//#define DO_STRETCHBLT 1

    HRESULT Renderer::applyStencilMask(HDC hdc,GraphicElements::imageMask *pMask) {

    HDC hdcSource = CreateCompatibleDC(hdc);
    HBITMAP hbmSource = CreateBitmap(pMask -> widthImage,pMask -> heightImage,1,32,NULL);
    SelectObject(hdcSource,hbmSource);

#if DO_STRETCHBLT

    StretchBlt(hdcSource,0,0,pMask -> widthImage,pMask -> heightImage,hdc,
                    pMask -> locPixels.x,pMask -> locPixels.y,pMask -> widthPixels,pMask -> heightPixels,SRCCOPY);

    long padding = 8 - pMask -> widthImage % 8;

    uint8_t masks[]{0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

    uint8_t maskIndex = 0;
    uint32_t pbIndex = 0;

    for ( long y = 0; y < pMask -> heightImage; y++) {

        for ( long x = 0; x < pMask -> widthImage; x++) {

            if ( pMask -> polarity == (pMask -> pbMaskingBytes[pbIndex] & masks[maskIndex]) )
                SetPixel(hdcSource,x,y,pMask -> currentColor);

            maskIndex++;
            if ( 8 == maskIndex ) {
                pbIndex++;
                maskIndex = 0;
            }

        }
        /*
        Each row of the source image begins on a byte boundary. If the number of data bits
        per row is not a multiple of 8, the end of the row must be padded with extra bits
        to fill out the last byte. The PostScript interpreter ignores these padding bits. 
        */
        maskIndex = 0;
        if ( 0 < padding )
            pbIndex++;

    }

    BOOL rvx = StretchBlt(hdc,pMask -> locPixels.x,pMask -> locPixels.y,pMask -> widthPixels,pMask -> heightPixels,hdcSource,0,0,pMask -> widthImage,pMask -> heightImage,SRCCOPY);

#else

    BitBlt(hdcSource,0,0,pMask -> widthPixels,pMask -> heightPixels,hdc,pMask -> locPixels.x,pMask -> locPixels.y,SRCCOPY);

    long padding = 8 - (pMask -> widthImage % 8);
    if ( 8 == padding )
        padding = 0;
    uint32_t sampleStrideBits = pMask -> widthImage + padding;
    uint32_t sampleStrideBytes = sampleStrideBits / 8;

    uint32_t cellSize = (uint32_t)round((FLOAT)sampleStrideBits / (FLOAT)pMask -> widthPixels);

    uint8_t masks[]{0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

    uint32_t countPixels = 0;

    for ( uint32_t y = 0; y < pMask -> heightImage; y += cellSize )
        for ( uint32_t x = 0; x < sampleStrideBits; x += cellSize )
            countPixels++;

    uint32_t countInCell = cellSize * cellSize;
    boolean *pShowPixel = new boolean[countPixels];
    memset(pShowPixel,0,countPixels * sizeof(boolean));

    long targetIndex = 0;

    for ( uint32_t y = 0; y < pMask -> heightImage; y += cellSize ) {

        uint8_t maskIndex = 0;
        uint32_t pbIndex = y * sampleStrideBytes;

        for ( uint32_t x = 0; x < sampleStrideBits; x += cellSize ) {

            uint32_t countOnes = 0;

            for ( uint32_t k = 0; k < cellSize && (x + k) < pMask -> widthImage; k++ ) {

                for ( uint32_t j = 0; j < cellSize && (y + j) < pMask -> heightImage; j++ ) {
                    if ( pMask -> polarity == (pMask -> pbMaskingBytes[pbIndex + j * sampleStrideBytes] & masks[maskIndex]) )
                        countOnes++;
                }

                maskIndex++;
                if ( 8 == maskIndex ) {
                    pbIndex++;
                    maskIndex = 0;
                }

            }

            pShowPixel[targetIndex++] = (countOnes > countInCell / 2) ? true : false;

        }
    }

    targetIndex = 0;
    long yPixel = -1;
    for ( uint32_t y = 0; y < pMask -> heightImage; y += cellSize ) {
        yPixel++;
        long xPixel = -1;
        for ( uint32_t x = 0; x < sampleStrideBits; x += cellSize ) {
            xPixel++;
            if ( pShowPixel[targetIndex++] )
                SetPixel(hdcSource,xPixel,yPixel,pMask -> currentColor);
        }
    }

    BOOL rvx = BitBlt(hdc,pMask -> locPixels.x,pMask -> locPixels.y,pMask -> widthPixels,pMask -> heightPixels,hdcSource,0,0,SRCCOPY);

    delete [] pShowPixel;

#endif

    DeleteObject(hbmSource);
    DeleteDC(hdcSource);

    return S_OK;
    }