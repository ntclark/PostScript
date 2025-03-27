
#include "FontManager.h"

#define FT_CURVE_TAG_ON 0x01

    HRESULT font::RenderGlyph(HDC hdc,unsigned short bGlyph,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,POINTF *pStartPoint,POINTF *pEndPoint) {

    if ( font::fontType::ftype3 == fontType ) 
        return drawType3Glyph(hdc,bGlyph,pPSXform,pXformToDeviceSpace,pStartPoint,pEndPoint);

    if ( ! ( font::fontType::ftype42 == fontType ) )
        return E_UNEXPECTED;

    return drawType42Glyph(hdc,bGlyph,pPSXform,pXformToDeviceSpace,pStartPoint,pEndPoint);
    }


    HRESULT font::drawType3Glyph(HDC,unsigned short,UINT_PTR,UINT_PTR,POINTF *pStartPoint,POINTF *pEndPoint) {
    return E_NOTIMPL;
    }


    HRESULT font::drawType42Glyph(HDC hdc,unsigned short bGlyph,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,POINTF *pStartPoint,POINTF *pEndPoint) {

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

    uint16_t glyphIndex;

    get_GlyphIndex(bGlyph,&glyphIndex);

    pbGlyphData = getGlyphData(glyphIndex);

    if ( NULL == pbGlyphData ) {
        if ( (long)bGlyph <= countGlyphs ) {
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

    FontManager::pIRenderer -> put_TransformMatrix((UINT_PTR)pXformToDeviceSpace);

    if ( NULL == FontManager::pIGraphicElements )
        FontManager::pIRenderer -> QueryInterface(IID_IGraphicElements,reinterpret_cast<void **>(&FontManager::pIGraphicElements));

    matrix *pMatrix = new matrix();

    // Transform glyph units to PAGE space

    // Scale Glyph Coordinates to points
    pMatrix -> scale(scaleFUnitsToPoints);

    // If the coordinates are not scaled up by some amount,
    // very poor visual performance in rasterization occurs. It is
    // not clear why, though it probably has to do with
    // roundoff, itself due to lack of precision in FLOAT
    pMatrix -> scale(64.0f);

    // Transform those using the current postscript CTM
    // which will put the coordinates in PAGE space
    pMatrix -> concat((XFORM *)pPSXform);

    if ( NULL == FontManager::pIRenderer ) {
        POINTF initialPoint{pStartPoint -> x,pStartPoint -> y};
        pMatrix -> concat((XFORM *)pXformToDeviceSpace);
        matrix::transformPoints((XFORM *)pXformToDeviceSpace,(GS_POINT *)&initialPoint,1);
        pMatrix -> move(initialPoint.x,initialPoint.y);
    } else {
        // startPoint (initialPoint) should be in page space coordinates
        // That is, the coordinates in the range PDF Height x PDF Width (792x612)
        // essentially after the current postscript transformation has been 
        // applied.
        // The GlyphRenderer will convert this to device (GDI) coordinates
        //matrix::transformPoints((XFORM *)pPSXform,(GS_POINT *)&initialPoint,1);
        FontManager::pIRenderer -> put_Origin(*pStartPoint);
        FontManager::pIRenderer -> put_DownScale(64.0f);
    }

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
            char szX[128];
            sprintf_s<128>(szX,"Invalid font: A glyph (%x) has a contour whose first point is OFF the contour",bGlyph);
            //MessageBox(NULL,szX,__FILE__,MB_OK);
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

                FontManager::pIGraphicElements -> QuadraticBezierTo((FLOAT)pControl -> x,(FLOAT)pControl -> y,(FLOAT)endPoint.x,(FLOAT)endPoint.y);
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

    if ( ! ( NULL == FontManager::pIGraphicElements ) ) {
        FontManager::pIGraphicElements -> FillPath();
        FontManager::pIRenderer -> Reset();
    }

    if ( ! ( NULL == pEndPoint ) ) {
        POINTF ptAdvance{pGlyphGeometry -> AdvanceWidth() * scaleFUnitsToPoints * PointSize(),0.0f};
        pEndPoint -> x = pStartPoint -> x + ptAdvance.x;
        pEndPoint -> y = pStartPoint -> y + ptAdvance.y;
    }

    delete pMatrix;

    if ( ! ( NULL == pSimpleGlyph ) )
        delete pSimpleGlyph;

    if ( ! ( NULL == pCompositeGlyph ) )
        delete pCompositeGlyph;

    return S_OK;
    }