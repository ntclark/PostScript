
#include "FontManager.h"
#include <functional>

#define FT_CURVE_TAG_ON 0x01

    HRESULT FontManager::RenderGlyph(HDC hdc,BYTE bGlyph,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,IGlyphRenderer *pIGlyphRenderer,POINTF *pStartPoint,POINTF *pEndPoint) {

    font *pFont = static_cast<font *>(pIFont_Current);

    if ( font::fontType::ftype3 == pFont -> fontType ) 
        return drawType3Glyph(hdc,bGlyph,pPSXform,pXformToDeviceSpace,pIGlyphRenderer,pStartPoint,pEndPoint);

    if ( ! ( font::fontType::ftype42 == pFont -> fontType ) )
        return E_UNEXPECTED;

    //if ( NULL == pIGlyphRenderer )
    //    BeginPath(hdc);

    HRESULT rc = drawType42Glyph(hdc,bGlyph,pPSXform,pXformToDeviceSpace,pIGlyphRenderer,pStartPoint,pEndPoint);

    //if ( NULL == pIGlyphRenderer ) {
    //    EndPath(hdc);
    //    StrokePath(hdc);
    //}

    return rc;
    }


    HRESULT FontManager::drawType3Glyph(HDC,BYTE,UINT_PTR,UINT_PTR,IGlyphRenderer *,POINTF *pStartPoint,POINTF *pEndPoint) {
    return E_NOTIMPL;
    }


    HRESULT FontManager::drawType42Glyph(HDC hdc,BYTE bGlyph,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,IGlyphRenderer *pIGlyphRenderer,POINTF *pStartPoint,POINTF *pEndPoint) {

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
MessageBox(NULL,__FILE__,"Not Impl",MB_OK);
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
                pEndPoint -> x = pStartPoint -> x + pMetric.advanceWidth * pFont -> scaleFUnitsToPoints * pFont -> PointSize();
                pEndPoint -> y = pStartPoint -> y;
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

    if ( 0 > glyphHeader.contourCount ) 
        return E_NOTIMPL;

    std::function<void(POINT *pt)> moveToAction{NULL};
    std::function<void(POINT *pt)> lineToAction{NULL};
    std::function<void(POINT *pt1,POINT *pt2,POINT *pt3)> quadraticBezierAction{NULL};
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

        moveToAction = [=](POINT *pt) { 
            pIGraphicElements_External -> NewPath();
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

        quadraticBezierAction = [=](POINT *,POINT *pt2,POINT *pt3) {
            pIGraphicElements_External -> QuadraticBezierTo((FLOAT)pt2 -> x,(FLOAT)pt2 -> y,(FLOAT)pt3 -> x,(FLOAT)pt3 -> y);
        };

        endFigureAction = [=](POINT *ptStart) {
            lineToAction(ptStart);
        };

    } else {

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
return;
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
                char szX[16];
                sprintf_s<16>(szX,"%02x-%02d",curveFlag & 0x03,index);
                RECT rc{pt -> x + width + 8,pt -> y,pt -> x + width + 60,pt -> y + 32};
                DrawTextEx(hdc,szX,-1,&rc,0L,NULL);
            }
            MoveToEx(hdc,ptCurrent.x,ptCurrent.y,NULL);
        };

        quadraticBezierAction = [=](POINT *pt1,POINT *pt2,POINT *pt3) {
            for ( FLOAT t = 0.0; t <= 1.0; t += BEZIER_CURVE_GRANULARITY ) {
                FLOAT x = (1.0f - t) * (1.0f - t) * pt1 -> x + 2.0f * t * (1.0f - t) * pt2 -> x + t * t * pt3 -> x;
                FLOAT y = (1.0f - t) * (1.0f - t) * pt1-> y + 2.0f * t * (1.0f - t) * pt2 -> y + t * t * pt3 -> y;
                POINT pt{(LONG)x,(LONG)y};
                lineToAction(&pt);
            }
        };

        HPEN hPen = CreatePen(PS_SOLID,1,RGB(255,0,0));
        HGDIOBJ oldObj = SelectObject(hdc,hPen);

    }

    pSimpleGlyph = new otSimpleGlyph(bGlyph,pFont,&glyphHeader,pbGlyphData);

    matrix *pMatrix = new matrix();

    pMatrix -> scale(pFont -> scaleFUnitsToPoints);

    pMatrix -> concat(pFont -> matrixStack.top());

    pMatrix -> concat((XFORM *)pPSXform);

    POINT initialPoint{(LONG)pStartPoint -> x,(LONG)pStartPoint -> y};

    if ( NULL == pIGlyphRenderer ) {
        pMatrix -> concat((XFORM *)pXformToDeviceSpace);
        GetCurrentPositionEx(hdc,&initialPoint);
        //POINTF pt{pStartPoint -> x,pStartPoint -> y};
        //pMatrix -> transformPoints((XFORM *)pXformToDeviceSpace,(GS_POINT *)&pt,1);
        //pMatrix -> move((FLOAT)pt.x,(FLOAT)pt.y);
        pMatrix -> move((FLOAT)initialPoint.x,(FLOAT)initialPoint.y);
    } else {
        pMatrix -> move(pStartPoint -> x,pStartPoint -> y);
    }

    pMatrix -> transformPoints(pSimpleGlyph -> pPoints,pSimpleGlyph -> pointCount);

    pMatrix -> transformPoints(&initialPoint,1);

    if ( NULL == pIGlyphRenderer ) {
        uint8_t ptIndex = 0;
        for ( long k = 0; k < pSimpleGlyph -> pGlyphHeader -> contourCount; k++ ) {
            uint8_t *pOnCurve = pSimpleGlyph -> pFlagsFirst[k];
            for ( long j = 0; j < pSimpleGlyph -> contourPointCount[k]; j++ )
                dotAction(pSimpleGlyph -> pPointFirst[k] + j,pOnCurve[j],ptIndex++);
        }
    }

    for ( long k = 0; k < pSimpleGlyph -> pGlyphHeader -> contourCount; k++ ) {

        uint16_t pointCount = pSimpleGlyph -> contourPointCount[k];

        uint8_t *pOnCurve = pSimpleGlyph -> pFlagsFirst[k];
        POINT *pPoints = pSimpleGlyph -> pPointFirst[k];

        if ( pOnCurve[0] & FT_CURVE_TAG_ON ) {
            moveToAction(pPoints);
            //dotAction(pPoints,0x01);
        } else {
MessageBox(NULL,__FILE__,"Shouldn't happen",MB_OK);
        }

        long pointIndex = 0;

        while ( pointIndex < pointCount - 1 ) {

            pointIndex++;

            if ( pOnCurve[pointIndex] & FT_CURVE_TAG_ON ) {
                lineToAction(pPoints + pointIndex);
                continue;
            }

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
                bezierIndex++;
                controlIndices[bezierIndex] = pointIndex;
                goto CaptureBezier;
            }

            long controlIndex = 0;

            POINT startPoint{currentPoint.x,currentPoint.y};

            POINT lastPoint;

            if ( pointIndex == pointCount ) {
                lastPoint.x = pPoints -> x;
                lastPoint.y = pPoints -> y;
            } else {
                lastPoint.x = (pPoints + pointIndex) -> x;
                lastPoint.y = (pPoints + pointIndex) -> y;
            }

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

                quadraticBezierAction(pStart,pControl,pEnd);

                startPoint.x = pEnd -> x;
                startPoint.y = pEnd -> y;

                controlIndex++;

            } while ( controlIndex <= bezierIndex );

        }

        endFigureAction(pSimpleGlyph -> pPointFirst[k]);

    }

    if ( ! ( NULL == pIGraphicElements_External ) )
        pIGraphicElements_External -> FillPath();

    pEndPoint -> x = pStartPoint -> x + pSimpleGlyph -> advanceWidth * pFont -> scaleFUnitsToPoints * pFont -> PointSize() / pMatrix -> XForm() -> eM11;
    pEndPoint -> y = pStartPoint -> y;

    delete pMatrix;

    delete pSimpleGlyph;

    return S_OK;
    }