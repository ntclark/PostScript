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

#include <Windows.h>
#include <Gdiplus.h>
#include <gdiplusinit.h>
#include <iostream>
#include <streambuf>

#include "job.h"

#include "pathParameters.h"

    Gdiplus::Bitmap *pPadBitmap = NULL;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput = 0L;

    HBITMAP SaveBitmapFile(BYTE *pbImage,DWORD cbData);
    HBITMAP createTemporaryBitmap(long cx,long cy,long bitsPerComponent,BYTE *pbData);

    void graphicsState::image() {

    // width height bits/sample matrix datasrc image –

    object *pSource = pJob -> pop();

    if ( ! ( object::dictionary == pSource -> ObjectType() ) ) {
        char szMessage[1024];
        sprintf_s<1024>(szMessage,"Error: Only language level 2 and above is supported for the image operator");
        throw notimplemented(szMessage);
        return;

    }

    dictionary *pSourceDictionary = reinterpret_cast<dictionary *>(pSource);
    matrix *pMatrix = reinterpret_cast<matrix *>(pSourceDictionary -> retrieve("ImageMatrix"));
    object *pBitsPerComponent = pSourceDictionary -> retrieve("BitsPerComponent");
    object *pHeight = pSourceDictionary -> retrieve("Height");
    object *pWidth = pSourceDictionary -> retrieve("Width");
    array *pDecodeArray = reinterpret_cast<array *>(pSourceDictionary -> retrieve("Decode"));

    if ( ! ( 1 == pSourceDictionary -> retrieve("ImageType") -> IntValue() ) ) {
        char szMessage[1024];
        sprintf_s<1024>(szMessage,"Error: Only image type 1 is currently implemented for Language Level 2 image operator");
        throw notimplemented(szMessage);
        return;
    }

    DWORD cbData = 0;
    uint8_t *pbImage = NULL;
    class filter *pFilter = NULL;

    if ( object::objectType::filter == pSource -> ObjectType() ) 
        pFilter = reinterpret_cast<class filter *>(pSource);
    else
        pFilter = reinterpret_cast<class filter *>(pSourceDictionary -> retrieve("DataSource"));

    pbImage = pFilter -> getBinaryData(&cbData,NULL);

    uint8_t bitsPerComponent = (uint8_t) pBitsPerComponent -> IntValue();
    uint16_t width = (uint16_t)pWidth -> IntValue();
    uint16_t height = (uint16_t)pHeight -> IntValue();

    HBITMAP hbmResult = NULL;

    uint8_t *pbNext = pbImage;
    uint8_t *pbLast = pbImage + cbData;

    colorSpace *pColorSpace = getColorSpace();

    object *csFamily = pColorSpace -> getElement(0);

    if ( 0 == strcmp(csFamily -> Contents(),"Indexed") ) {

        object *pLookup = pColorSpace -> Lookup();

        uint16_t hiVal = pColorSpace -> HiVal();

        uint16_t widthBytes = 3 * width;
        uint16_t stride = ((((width * 24) + 31) & ~31) >> 3);

        uint16_t padding = stride - widthBytes;

        DWORD cbRGBImage = height * stride;

        uint8_t *pbRGBImage = new uint8_t[cbRGBImage];

        memset(pbRGBImage,0xFF,cbRGBImage);

        if ( object::valueType::binaryString == pLookup -> ValueType() ) {

            uint8_t *pbColorData = reinterpret_cast<binaryString *>(pLookup) -> getData();

            /*
            If lookup is a string object, it must be of length m × (hival + 1), where m is the
            number of color components in the base color space. Each byte in the string is 
            interpreted as an integer. To look up an index, the PostScript interpreter multiplies
            the index by m and uses the result to access the lookup string. The m bytes beginning
            at that position in the string are interpreted as coded values for the m color components 
            of the base color space. Those bytes are treated as 8-bit integers in the range 0 to 255, 
            which are then divided by 255, yielding component values in the range 0.0 to 1.0. 
            */

            uint8_t m = (uint8_t)pColorSpace -> ParameterCount();

            uint32_t compIndex = 1;
            uint16_t byteCount = 0;

            uint8_t *pbRGBTarget = pbRGBImage - 3;

            do {

                uint16_t index;

                switch ( bitsPerComponent ) {
                case 12: {
                    if ( 0 == compIndex % 2 ) {
                        uint16_t x = *(uint16_t *)pbNext;
                        x &= 0xFFF0;
                        x = x >> 4;
                        index = x;
                    } else {
                        uint16_t x = *(uint16_t *)pbNext;
                        x &= 0x000F;
                        x = x << 12;
                        pbNext += 2;
                        uint16_t y = *(uint16_t *)pbNext;
                        y &= 0xFFF0;
                        y = y >> 4;
                        index = x & y;
                    }
                    }
                    break;

                case 8: {
                    uint8_t x = *pbNext;
                    index = (uint16_t)x;
                    pbNext++;
                    }
                    break;

                case 4: {
                    uint8_t x = *pbNext;
                    if ( 0 == compIndex % 2 ) {
                        x = x & 0x0F;
                        pbNext++;
                    } else {
                        x = x & 0xF0;
                        x = x >> 4;
                    }
                    index = (uint16_t)x;
                    }
                    break;

                }

                uint8_t *pbComponent = pbColorData + m * index;

                pbRGBTarget += 3;

                if ( byteCount > stride ) {
                    pbRGBTarget += padding;
                    byteCount = 0;
                }

                if ( 3 == m ) {
                    pbRGBTarget[2] = pbComponent[0];
                    pbRGBTarget[1] = pbComponent[1];
                    pbRGBTarget[0] = pbComponent[2];
                } else
                    memset(pbRGBTarget,*pbComponent,3);

                byteCount += 3;

                compIndex++;

            } while ( pbNext < pbLast );

        }

        hbmResult = createTemporaryBitmap(width,height,8,pbRGBImage);

#if 0
HDC hdcx = CreateCompatibleDC(NULL);
SelectObject(hdcx,hbmResult);
BitBlt(GetDC(HWND_DESKTOP),0,0,width,height,hdcx,0,0,SRCCOPY);
DeleteDC(hdcx);
#endif

        renderImage(hbmResult,width,height);

        DeleteObject(hbmResult);

        pFilter -> releaseData();

        delete [] pbRGBImage;

        return;

    } 

    uint8_t TwoNminus1 = (uint8_t)pow(2,pBitsPerComponent -> IntValue()) - 1;

#if 0
    do {

        for ( uint8_t k = 0; k < step; k++ ) {

        }

        pbNext += step;

    } while ( pbNext < pbLast );
#endif

#if 1

    for ( long k = 0; k < 3; k++ ) {

if ( 2 * k >= pDecodeArray -> size() )
break;

        POINT_TYPE dMin = pDecodeArray -> getElement(0 + 2 * k) -> OBJECT_POINT_TYPE_VALUE;
        POINT_TYPE dMax = pDecodeArray -> getElement(1 + 2 * k) -> OBJECT_POINT_TYPE_VALUE;

        if ( 0.0 == dMin && 1.0 == dMax )
            continue;

        POINT_TYPE factor = (POINT_TYPE)(dMax - dMin) / (POINT_TYPE)TwoNminus1;
        uint8_t *pbNext = pbImage + k;
        uint8_t *pbLast = pbNext + cbData;
        do {
            *pbNext = (uint8_t)(dMin + ( (POINT_TYPE)*pbNext * factor ) * TwoNminus1);
            pbNext += 3;
        } while ( pbNext < pbLast );
    }
#endif

#if 1

    if ( 0 == strcmp(csFamily -> Contents(),"DeviceRGB") ) {
        uint8_t *pbNext = pbImage;
        uint8_t *pbLast = pbNext + cbData - 3;
        do {
            uint8_t t = *pbNext;
            *pbNext = *(pbNext + 2);
            *(pbNext + 2) = t;
            pbNext += 3;
        } while ( pbNext < pbLast );
    }

#endif

    hbmResult = createTemporaryBitmap(pWidth -> IntValue(),pHeight -> IntValue(),pBitsPerComponent -> IntValue(),pbImage);

    renderImage(hbmResult,pWidth,pHeight);

    DeleteObject(hbmResult);

    pFilter -> releaseData();

    return;
    }


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

    HBITMAP hbmResult = SaveBitmapFile(pbImage,cbData);

    renderImage(hbmResult,pWidth,pHeight);

    pFilter -> releaseData();

    return;
    }


    void graphicsState::renderImage(HBITMAP hbmResult,uint16_t width,uint16_t height) {
    job::pIGraphicElements_External -> Image(pPStoPDF -> GetDC(),hbmResult,
                                                (UINT_PTR)psXformsStack.top() -> XForm(),(FLOAT)width,(FLOAT)height);
    DeleteObject(hbmResult);
    return;
    }


    void graphicsState::renderImage(HBITMAP hbmResult,object *pWidth,object *pHeight) {
    renderImage(hbmResult,(uint16_t)pWidth -> IntValue(),(uint16_t)pHeight -> IntValue());
    return;
    }


    HBITMAP SaveBitmapFile(BYTE *pbImage,DWORD cbData) {

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

    return hbmResult;
    }


    HBITMAP createTemporaryBitmap(long cx,long cy,long bitsPerComponent,BYTE *pBits) {

    BITMAPINFO bitMapInfo;

    memset(&bitMapInfo,0,sizeof(bitMapInfo));

    bitMapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitMapInfo.bmiHeader.biWidth = cx;
    bitMapInfo.bmiHeader.biHeight = -cy;
    bitMapInfo.bmiHeader.biPlanes = 1;
    bitMapInfo.bmiHeader.biBitCount = 3 * (unsigned short)bitsPerComponent;
    bitMapInfo.bmiHeader.biCompression = BI_RGB;

    HBITMAP hBitmap = CreateDIBSection(NULL,&bitMapInfo,DIB_RGB_COLORS,NULL,NULL,0L);

    SetDIBits(NULL,hBitmap,0,cy,pBits,&bitMapInfo,DIB_RGB_COLORS);

//SetBitmapBits(hBitmap,cx * cy * 3 * bitsPerComponent,pBits);

    return hBitmap;
    }