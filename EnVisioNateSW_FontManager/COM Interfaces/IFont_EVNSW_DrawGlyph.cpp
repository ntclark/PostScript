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

#include "FontManager.h"

#define FT_CURVE_TAG_ON 0x01

    HRESULT font::RenderGlyph(unsigned short bGlyph,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,POINTF *pStartPoint,POINTF *pEndPoint) {

    if ( FontType::type3 == theFontType ) 
        return drawType3Glyph(bGlyph,pPSXform,pXformToDeviceSpace,pStartPoint,pEndPoint);

    if ( ! ( FontType::type42 == theFontType ) )
        return E_UNEXPECTED;

    return drawType42Glyph(bGlyph,pPSXform,pXformToDeviceSpace,pStartPoint,pEndPoint);
    }


    HRESULT font::drawType3Glyph(unsigned short,UINT_PTR,UINT_PTR,POINTF *pStartPoint,POINTF *pEndPoint) {
    return E_NOTIMPL;
    }


    HRESULT font::drawType42Glyph(unsigned short bGlyph,UINT_PTR pPSXform,UINT_PTR pxxXformToDeviceSpace,POINTF *pStartPoint,POINTF *pEndPoint) {

    BYTE *pbGlyphData = NULL;

    /*
    By definition, glyph index zero points to the “missing character” glyph, 
    which is the glyph that is displayed if a character is not found in the 
    font’s 'cmap' table. A missing character is commonly represented by a 
    blank box or a space. If the font does not contain an outline for this
    glyph, then the first and second offsets should have the same value. 
    This also applies to any other glyphs without an outline, such as the 
    glyph for the space character: if a glyph has no outline or instructions, 
    then loca[n] = loca[n+1].
    */

    uint16_t glyph = bGlyph;

    if ( 0 < encodingTable.size() ) {
        if ( ! ( encodingTable.end() == encodingTable.find((uint32_t)glyph) ) ) {
            const char *pszCharTableIndex = encodingTable[(uint32_t)glyph];
            std::map<uint32_t,char *> *pCharStrings = &charStringsTable;
            if ( 0 == pCharStrings -> size() )
                pCharStrings = &font::adobeGlyphList;
            for ( std::pair<uint32_t,char *> pPair : *pCharStrings ) {
                if ( 0 == strcmp(pszCharTableIndex,pPair.second) ) {
                    glyph = pPair.first;
                    break;
                }
            }
        }
    }

    uint16_t glyphIndex;

    get_GlyphIndex(glyph,&glyphIndex);

    pbGlyphData = getGlyphData(glyphIndex);

    if ( NULL == pbGlyphData ) {
        if ( (long)glyph <= countGlyphs ) {
            otLongHorizontalMetric pMetric =  pHorizontalMetricsTable -> pHorizontalMetrics[glyphIndex];
            if ( ! ( NULL == pEndPoint ) ) {
                pEndPoint -> x = pStartPoint -> x + pMetric.advanceWidth * scaleFUnitsToPoints * PointSize();
                pEndPoint -> y = pStartPoint -> y;
            }
            return S_OK;
        }
        return E_FAIL;
    }

    otGlyphHeader glyphHeader{0};

    glyphHeader.load(pbGlyphData);

    otSimpleGlyph *pSimpleGlyph = NULL;
    otCompositeGlyph *pCompositeGlyph = NULL;

    otGlyphGeometry *pGlyphGeometry = NULL;

    if ( 0 > glyphHeader.contourCount ) {
        pCompositeGlyph = new otCompositeGlyph(this,&glyphHeader,pbGlyphData);
        pGlyphGeometry = static_cast<otGlyphGeometry *>(pCompositeGlyph);
    } else {
        pSimpleGlyph = new otSimpleGlyph(glyphIndex,this,&glyphHeader,pbGlyphData);
        pGlyphGeometry = static_cast<otGlyphGeometry *>(pSimpleGlyph);
    }

    if ( NULL == FontManager::pIGraphicElements )
        FontManager::pIRenderer -> QueryInterface(IID_IGraphicElements,reinterpret_cast<void **>(&FontManager::pIGraphicElements));

    FontManager::pIRenderer -> SaveState();

    matrix *pMatrix = new matrix();

    // Transform glyph units to PAGE space

    // Scale Glyph Coordinates to points. Glyphs are typically defined as 2048 units per M
    // This scale would make the height of an M be 1, scaling by PointSize later
    // would make the Height of an M be PointSize
    pMatrix -> scale(scaleFUnitsToPoints);

    // If the coordinates are not artifically scaled up by some amount,
    // very poor visual performance in rasterization occurs. It is
    // not clear why, though it probably has to do with
    // roundoff, itself due to lack of precision in FLOAT
    pMatrix -> scale(64.0f);

    // startPoint (initialPoint) should be in page space coordinates
    // That is, the coordinates in the range PDF Height x PDF Width (792x612)
    // essentially after the current postscript transformation has been 
    // applied.

    POINTF ptStart{pStartPoint -> x,pStartPoint -> y};
    pMatrix -> transformPoint((XFORM *)pPSXform,&ptStart,&ptStart);
    FontManager::pIRenderer -> put_Origin(ptStart);
    FontManager::pIRenderer -> put_DownScale(64.0f);

    // Set the Renderer "ToPage" transform to the "ToPage" transform 
    // currently in effect (the CTM)
    //
    // HOWEVER, anytime zero (origin) based point data is sent to the
    // Renderer, don't forget to zero out the translation component
    // of the "ToPage" transform. The "origin" is set as the current
    // point in the current parsing of the PS file and that is 
    // sent to the Renderer IN PAGE SPACE coordinates (which DOES
    // use the translation components of the CTM)
    //
    XFORM xFormScaleOnly = *(XFORM *)pPSXform;
    xFormScaleOnly.eDx = 0.0f;
    xFormScaleOnly.eDy = 0.0f;
    FontManager::pIRenderer -> put_ToPageTransform((UINT_PTR)&xFormScaleOnly);

    // Transform Glyph coordinates using current font matrix
    pMatrix -> concat(matrixStack.top());

    pMatrix -> transformPoints(pGlyphGeometry -> Points(),pGlyphGeometry -> PointCount());

    FontManager::pIGraphicElements -> NewPath();

    for ( long k = 0; k < pGlyphGeometry -> ContourCount(); k++ ) {

        uint16_t pointCount = pGlyphGeometry -> ContourPointCount((uint16_t)k);

        uint8_t *pOnCurve = pGlyphGeometry -> FlagsFirst((uint16_t)k);
        POINT *pPoints = pGlyphGeometry -> PointFirst((uint16_t)k);

        if ( pOnCurve[0] & FT_CURVE_TAG_ON ) {
            FontManager::pIGraphicElements -> MoveTo((FLOAT)pPoints -> x,(FLOAT)pPoints -> y);
            currentPoint.x = pPoints -> x;
            currentPoint.y = pPoints -> y;
        } else {
            sprintf(FontManager::szFailureMessage,"Invalid font: A glyph (%x) has a contour whose first point is OFF the contour",glyph);
            FontManager::FireErrorNotification(FontManager::szFailureMessage);
            FontManager::pIGraphicElements -> MoveTo((FLOAT)pPoints -> x,(FLOAT)pPoints -> y);
            currentPoint.x = pPoints -> x;
            currentPoint.y = pPoints -> y;
        }

        long pointIndex = 0;

        while ( pointIndex < pointCount - 1 ) {

            pointIndex++;

            if ( pOnCurve[pointIndex] & FT_CURVE_TAG_ON ) {
                POINT *ppt = pPoints + pointIndex;
                FontManager::pIGraphicElements -> LineTo((FLOAT)ppt -> x,(FLOAT)ppt ->y);
                currentPoint.x = ppt -> x;
                currentPoint.y = ppt -> y;
                continue;
            }

            // This is the first point OFF the curve
            // It is a (first) control point
            int controlIndices[32]{pointIndex,0,0,0,0};
            int bezierIndex = 0;

            /*
 
            https://www.truetype-typography.com/ttoutln.htm

            In Figure 1, the points numbered 4, 5 and 6 correspond to the A, B and C of 
            the above equations, and together define a section of the glyph outline. 
            Where multiple off-curve points occur consecutively, an on-curve 
            point is interpolated exactly half-way between them. 
            Thus there is an implied on-curve point between points 7 and 8, 
            between points 8 and 9, and so on. 
            The curve running from point 6 to point 11 is a single quadratic B-spline, 
            decomposed to 4 quadratic Bézier curves in the rasterizer.
            */

CaptureBezier:

            pointIndex++;

            if ( 0 == ( pOnCurve[pointIndex] & FT_CURVE_TAG_ON ) && pointIndex < pointCount ) {
                // The next point is OFF the curve
                bezierIndex++;
                controlIndices[bezierIndex] = pointIndex;
                goto CaptureBezier;
            }

            POINT startPoint{currentPoint.x,currentPoint.y};

            long controlIndex = 0;

            POINT lastPoint = (pointIndex == pointCount) ? pPoints[0] : pPoints[pointIndex];

            POINT endPoint = lastPoint;
            POINT *pStart = &startPoint;
            POINT *pEnd = &endPoint;

            do {

                POINT *pControl = pPoints + controlIndices[controlIndex];

                endPoint = lastPoint;

                if ( controlIndex < bezierIndex && controlIndices[controlIndex + 1] < pointCount ) {
                    endPoint.x = (LONG)((pControl -> x + (pPoints + controlIndices[controlIndex + 1]) -> x) / 2.0f);
                    endPoint.y = (LONG)((pControl -> y + (pPoints + controlIndices[controlIndex + 1]) -> y) / 2.0f);
                }

                FontManager::pIGraphicElements -> QuadraticBezier((FLOAT)pControl -> x,(FLOAT)pControl -> y,(FLOAT)endPoint.x,(FLOAT)endPoint.y);
                currentPoint.x = endPoint.x;
                currentPoint.y = endPoint.y;

                startPoint.x = pEnd -> x;
                startPoint.y = pEnd -> y;

                controlIndex++;

            } while ( controlIndex <= bezierIndex );

        }

        FontManager::pIGraphicElements -> LineTo((FLOAT)pPoints -> x,(FLOAT)pPoints -> y);
        currentPoint.x = pPoints -> x;
        currentPoint.y = pPoints -> y;

        FontManager::pIGraphicElements -> ClosePath();

    }

    FontManager::pIGraphicElements -> FillPath();
    FontManager::pIRenderer -> RestoreState();

    if ( ! ( NULL == pEndPoint ) ) {
        POINTF ptAdvance{pGlyphGeometry -> AdvanceWidth() * scaleFUnitsToPoints * PointSize(),0.0f};
        XFORM *px = (XFORM *)pPSXform;
        pEndPoint -> x = pStartPoint -> x + ptAdvance.x * px -> eM11;
        pEndPoint -> y = pStartPoint -> y + ptAdvance.y * px -> eM22;
    }

    delete pMatrix;

    if ( ! ( NULL == pSimpleGlyph ) )
        delete pSimpleGlyph;

    if ( ! ( NULL == pCompositeGlyph ) )
        delete pCompositeGlyph;

    return S_OK;
    }