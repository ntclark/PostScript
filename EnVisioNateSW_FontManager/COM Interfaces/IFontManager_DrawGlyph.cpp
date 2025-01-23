
#include "FontManager.h"
#include <functional>

#define FT_CURVE_TAG_ON 0x01

#ifdef USE_RENDERER
    HRESULT FontManager::RenderGlyph(HDC hdc,BYTE bGlyph,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,IRenderer *pIGlyphRenderer,POINTF *pStartPoint,POINTF *pEndPoint) {
#else
    HRESULT FontManager::RenderGlyph(HDC hdc,BYTE bGlyph,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,IGlyphRenderer *pIGlyphRenderer,POINTF *pStartPoint,POINTF *pEndPoint) {
#endif

    font *pFont = static_cast<font *>(pIFont_Current);

    if ( font::fontType::ftype3 == pFont -> fontType ) 
        return drawType3Glyph(hdc,bGlyph,pPSXform,pXformToDeviceSpace,pIGlyphRenderer,pStartPoint,pEndPoint);

    if ( ! ( font::fontType::ftype42 == pFont -> fontType ) )
        return E_UNEXPECTED;

    HRESULT rc = drawType42Glyph(hdc,bGlyph,pPSXform,pXformToDeviceSpace,pIGlyphRenderer,pStartPoint,pEndPoint);

    return rc;
    }

#ifdef USE_RENDERER
    HRESULT FontManager::drawType3Glyph(HDC,BYTE,UINT_PTR,UINT_PTR,IRenderer *,POINTF *pStartPoint,POINTF *pEndPoint) {
    return E_NOTIMPL;
    }


    HRESULT FontManager::drawType42Glyph(HDC hdc,BYTE bGlyph,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,IRenderer *pIGlyphRenderer,POINTF *pStartPoint,POINTF *pEndPoint) {
#else
    HRESULT FontManager::drawType3Glyph(HDC,BYTE,UINT_PTR,UINT_PTR,IGlyphRenderer *,POINTF *pStartPoint,POINTF *pEndPoint) {
    return E_NOTIMPL;
    }


    HRESULT FontManager::drawType42Glyph(HDC hdc,BYTE bGlyph,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,IGlyphRenderer *pIGlyphRenderer,POINTF *pStartPoint,POINTF *pEndPoint) {
#endif

    font *pFont = static_cast<font *>(pIFont_Current);

    BYTE *pbGlyphData = NULL;
/*
    5.5 GlyphDirectory 

        ...

        Starting with PostScript version 2015, the interpreter checks for the existence
        of a gdir table in the sfnts array, and if found, uses GlyphDirectory in place of
        the loca and glyf tables.
*/

    if ( ! ( NULL == pFont -> tableDirectory.table("gdir") ) ) {
        MessageBox(NULL,__FILE__,"TODO: Implement gdir",MB_OK);
        return E_NOTIMPL;
#if 0
        pJob -> push(pFont);
        pJob -> operatorDup();
        pJob -> operatorDup();
        pJob -> push(pJob -> pEncodingArrayLiteral);
        pJob -> operatorGet();

        // The BYTE bGlyph is the "index" into the Encoding array

        pJob -> push(new (pJob -> CurrentObjectHeap()) object(pJob,bGlyph));

        pJob -> operatorGet();

        // The string object pGlyphName is the index into the CharStrings array

        object *pGlyphName = pJob -> pop();

        pJob -> push(pJob -> pCharStringsLiteral);
        pJob -> operatorGet();
        pJob -> push(pGlyphName);
        pJob -> operatorGet();

        object *pLocaAndHtmxOffset = pJob -> pop();

        pJob -> push(pJob -> pGlyphDirectoryLiteral);
        pJob -> operatorGet();

        pJob -> push(pLocaAndHtmxOffset);

        pJob -> operatorGet();

        class binaryString *pBinaryString = reinterpret_cast<class binaryString *>(pJob -> pop());

        pbGlyphData =  pBinaryString -> getData();
#endif
    } else {

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

        uint32_t offsetInGlyphTable = 0;

        uint32_t *pGlyphOffset = (uint32_t *)pFont -> pLocaTable + pFont -> glyphIDMap[bGlyph];
        uint32_t *pNextOffset = (uint32_t *)pFont -> pLocaTable + pFont -> glyphIDMap[bGlyph] + 1;

        if ( ! ( *pGlyphOffset == *pNextOffset ) ) {
            BE_TO_LE_U32(pGlyphOffset,offsetInGlyphTable)
            pbGlyphData = pFont -> pGlyfTable + offsetInGlyphTable;
        } else {
            if ( (long)bGlyph <= pFont -> countGlyphs ) {
                uint32_t glyphId = pFont -> glyphIDMap[bGlyph];
                otLongHorizontalMetric pMetric = pFont -> pHorizontalMetricsTable -> pHorizontalMetrics[glyphId];
                if ( ! ( NULL == pEndPoint ) ) {
                    pEndPoint -> x = pStartPoint -> x + pMetric.advanceWidth * pFont -> scaleFUnitsToPoints * pFont -> PointSize();
                    pEndPoint -> y = pStartPoint -> y;
                }
                return S_OK;
            }
            return E_FAIL;
        }

    }

    if ( NULL == pbGlyphData ) 
        return E_FAIL;

    otGlyphHeader glyphHeader{0};

    otSimpleGlyph *pSimpleGlyph = NULL;

    glyphHeader.load(pbGlyphData);

    if ( 0 > glyphHeader.contourCount ) {
        MessageBox(NULL,__FILE__,"TODO: Implement compound glyphs",MB_OK);
        return E_NOTIMPL;
    }

    std::function<void()> newPathAction{NULL};
    std::function<void()> closePathAction{NULL};
    std::function<void(POINT *pt)> moveToAction{NULL};
    std::function<void(POINT *pt)> lineToAction{NULL};
    std::function<void(POINT *pt1,POINT *pt2,POINT *pt3)> quadraticBezierAction{NULL};
    std::function<void(POINT *ptStart,POINT *ptControl1,POINT *ptControl2,POINT *ptEnd)> cubicBezierAction{NULL};
    std::function<void(POINT *pt,uint8_t curveFlag,uint8_t ptIndex)> dotAction{NULL};
    std::function<void(POINT *ptStart)> endFigureAction{NULL};

    if ( ! ( NULL == pIGlyphRenderer ) ) {

        BOOL isPrepared = FALSE;
        pIGlyphRenderer -> get_IsPrepared(&isPrepared);
        if ( ! isPrepared ) 
            pIGlyphRenderer -> Prepare(hdc);

        pIGlyphRenderer -> put_TransformMatrix((UINT_PTR)pXformToDeviceSpace);

        if ( NULL == pIGraphicElements_External )
            pIGlyphRenderer -> QueryInterface(IID_IGraphicElements,reinterpret_cast<void **>(&pIGraphicElements_External));

        newPathAction = [=]() {
            pIGraphicElements_External -> NewPath();
        };

        closePathAction = [=]() {
            pIGraphicElements_External -> ClosePath();
        };

        moveToAction = [=](POINT *pt) { 
            pIGraphicElements_External -> MoveTo((FLOAT)pt -> x,(FLOAT)pt -> y);
            currentPoint.x = pt -> x;
            currentPoint.y = pt -> y;
        };

        lineToAction = [=](POINT *pt) { 
            pIGraphicElements_External -> LineTo((FLOAT)pt -> x,(FLOAT)pt -> y);
            currentPoint.x = pt -> x;
            currentPoint.y = pt -> y;
        };

        dotAction = [=](POINT *,uint8_t,uint8_t) {
        };

        quadraticBezierAction = [=](POINT *pt0,POINT *pt2,POINT *pt3) {
            pIGraphicElements_External -> QuadraticBezierTo((FLOAT)pt2 -> x,(FLOAT)pt2 -> y,(FLOAT)pt3 -> x,(FLOAT)pt3 -> y);
            currentPoint.x = pt3 -> x;
            currentPoint.y = pt3 -> y;
        };

        cubicBezierAction = [=](POINT *pt0,POINT *pt1,POINT *pt2,POINT *pt3) {
            pIGraphicElements_External -> CubicBezierTo((FLOAT)pt1 -> x,(FLOAT)pt1 -> y,(FLOAT)pt2 -> x,(FLOAT)pt2 -> y,(FLOAT)pt3 -> x,(FLOAT)pt3 -> y);
            currentPoint.x = pt3 -> x;
            currentPoint.y = pt3 -> y;
        };

        endFigureAction = [=](POINT *ptStart) {
            lineToAction(ptStart);
        };

    } else {

        newPathAction = [=]() {
            BeginPath(hdc);
        };

        closePathAction = [=]() {
        };

        moveToAction = [=](POINT *pt) { 
            MoveToEx(hdc,pt -> x,pt -> y,NULL);
            currentPoint.x = pt -> x;
            currentPoint.y = pt -> y;
        };

        lineToAction = [=](POINT *pt) { 
            LineTo(hdc,pt -> x,pt -> y);
            currentPoint.x = pt -> x;
            currentPoint.y = pt -> y;
        };

        endFigureAction = [=](POINT *ptStart) {
            lineToAction(ptStart);
        };

        dotAction = [=](POINT *pt,uint8_t curveFlag,uint8_t index) {
            POINT ptCurrent;
            GetCurrentPositionEx(hdc,&ptCurrent);
            long width = 2;
            if ( curveFlag & ON_CURVE_POINT )
                width = 4;
            else if ( curveFlag == 0xFF )
                width = 1;
            MoveToEx(hdc,width + pt -> x,width + pt -> y,NULL);
            LineTo(hdc,-width + pt -> x,width + pt -> y);
            LineTo(hdc,-width + pt -> x,-width + pt -> y);
            LineTo(hdc,width + pt -> x,- width + pt -> y);
            LineTo(hdc,width + pt -> x,width + pt -> y);
            if ( ! ( curveFlag == 0xFF ) ) {
                char szX[64];
                //sprintf_s<16>(szX,"%02x-%02d",curveFlag & 0x03,index);
                sprintf_s<64>(szX,"%04d-%04d",pt -> x,pt -> y);
                RECT rc{pt -> x + width + 8,pt -> y,pt -> x + width + 128,pt -> y + 32};
                DrawTextEx(hdc,szX,-1,&rc,0L,NULL);
            }
            MoveToEx(hdc,ptCurrent.x,ptCurrent.y,NULL);
        };

        quadraticBezierAction = [=](POINT *pt1,POINT *pt2,POINT *pt3) {
            POINT pThePts[3];
            pThePts[0] = *pt1;
            pThePts[1] = *pt2;
            pThePts[2] = *pt3;
            PolyBezierTo(hdc,pThePts,3);
            currentPoint.x = pt3 -> x;
            currentPoint.y = pt3 -> y;
        };

        cubicBezierAction = [=](POINT *pt0,POINT *pt1,POINT *pt2,POINT *pt3) {
            POINT pThePts[3];
            pThePts[0] = *pt1;
            pThePts[1] = *pt2;
            pThePts[2] = *pt3;
            PolyBezierTo(hdc,pThePts,3);
            currentPoint.x = pt3 -> x;
            currentPoint.y = pt3 -> y;
        };

    }

    pSimpleGlyph = new otSimpleGlyph(bGlyph,pFont,&glyphHeader,pbGlyphData);

#if 0
    matrix *pMatrix = new matrix();

    // Scale Glyph Coordinates to points
    pMatrix -> scale(pFont -> scaleFUnitsToPoints);

    // Transform Glyph coordinates in points using current font matrix
    pMatrix -> concat(pFont -> matrixStack.top());

    // If the coordinates are not scaled up by some amount,
    // very poor performance in rasterization occurs. It is
    // not clear why, though it probably has to do with
    // roundoff, itself due to lack of precision in FLOAT
    XFORM scaleUp{64.0f,0.0f,0.0f,64.0f,0.0f,0.0f};
    pMatrix -> concat(&scaleUp);

    // Transform those using the current postscript CTM
    // which will put the coordinates in PAGE space
    pMatrix -> concat((XFORM *)pPSXform);

    POINTF initialPoint{pStartPoint -> x,pStartPoint -> y};

    if ( NULL == pIGlyphRenderer ) {
        pMatrix -> concat((XFORM *)pXformToDeviceSpace);
        matrix::transformPoints((XFORM *)pXformToDeviceSpace,(GS_POINT *)&initialPoint,1);
        pMatrix -> move(initialPoint.x,initialPoint.y);
    } else {
        // startPoint (initialPoint) should be in page space coordinates
        // That is, the coordinates in the range PDF Height x PDF Width
        // essentially after the current postscript transformation has been 
        // applied.
        // The GlyphRenderer will convert this to device (GDI) coordinates
        matrix::transformPoints((XFORM *)pPSXform,(GS_POINT *)&initialPoint,1);
        pIGlyphRenderer -> put_Origin(initialPoint);
        pIGlyphRenderer -> put_DownScale(64.0f);
    }

    pMatrix -> transformPoints(pSimpleGlyph -> pPoints,pSimpleGlyph -> pointCount);
#else

    // startPoint (initialPoint) should be in page space coordinates
    // That is, the coordinates in the range PDF Height x PDF Width
    // essentially after the current postscript transformation has been 
    // applied.
    // The GlyphRenderer will convert this to device (GDI) coordinates

    POINTF initialPoint{pStartPoint -> x,pStartPoint -> y};

    //matrix::transformPoints((XFORM *)pPSXform,(GS_POINT *)&initialPoint,1);
    pIGlyphRenderer -> put_Origin(initialPoint);
    pIGlyphRenderer -> put_DownScale(pFont -> PointSize() * pFont -> scaleFUnitsToPoints);
#endif

    //if ( NULL == pIGlyphRenderer ) {
    //    uint8_t ptIndex = 0;
    //    for ( long k = 0; k < pSimpleGlyph -> pGlyphHeader -> contourCount; k++ ) {
    //        uint8_t *pOnCurve = pSimpleGlyph -> pFlagsFirst[k];
    //        for ( long j = 0; j < pSimpleGlyph -> contourPointCount[k]; j++ )
    //            dotAction(pSimpleGlyph -> pPointFirst[k] + j,pOnCurve[j],ptIndex++);
    //    }
    //}

    newPathAction();

    for ( long k = 0; k < pSimpleGlyph -> pGlyphHeader -> contourCount; k++ ) {

        uint16_t pointCount = pSimpleGlyph -> contourPointCount[k];

        uint8_t *pOnCurve = pSimpleGlyph -> pFlagsFirst[k];
        POINT *pPoints = pSimpleGlyph -> pPointFirst[k];

        if ( pOnCurve[0] & FT_CURVE_TAG_ON )
            moveToAction(pPoints);
        else {
            char szX[128];
            sprintf_s<128>(szX,"Invalid font: A glyph (%x) has a contour whose first point is OFF the contour",bGlyph);
            MessageBox(NULL,szX,__FILE__,MB_OK);
        }

        long pointIndex = 0;

        while ( pointIndex < pointCount - 1 ) {

            pointIndex++;

            if ( pOnCurve[pointIndex] & FT_CURVE_TAG_ON ) {
                lineToAction(pPoints + pointIndex);
                continue;
            }

            // This is the first point OFF the curve
            // It is a (first) control point
            int controlIndices[]{pointIndex,0,0,0,0};
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

                quadraticBezierAction(pStart,pControl,&endPoint);

                startPoint.x = pEnd -> x;
                startPoint.y = pEnd -> y;

                controlIndex++;

            } while ( controlIndex <= bezierIndex );

        }

        lineToAction(pPoints);

        closePathAction();

    }

    if ( ! ( NULL == pIGraphicElements_External ) ) {
        pIGraphicElements_External -> FillPath();
        pIGlyphRenderer -> Reset();
    }

#if 0
    if ( ! ( NULL == pEndPoint ) ) {
        POINTF ptAdvance{pSimpleGlyph -> advanceWidth,0.0f};
        pMatrix -> unTransformPoint(&ptAdvance,&ptAdvance);
        pEndPoint -> x = pStartPoint -> x + ptAdvance.x;
        pEndPoint -> y = pStartPoint -> y + ptAdvance.y;
    }

    delete pMatrix;
#endif

    delete pSimpleGlyph;

    return S_OK;
    }