
#include "PostScript objects/graphicsState.h"
#include "PostScript objects/font.h"

void LoadGlyph(BYTE bIndex,BYTE *pbGlyphData,long cbData,BYTE *pbMetrixData);

    otSimpleGlyph::otSimpleGlyph(BYTE bGlyph,font *pFont,graphicsState *pGraphicsState,otGlyphHeader *ph,BYTE *pbInput,long cbGlyphData) :
        pGlyphHeader(ph),
        pbGlyphData(pbInput)
    {

    memset(phantomPoints,0,sizeof(phantomPoints));

    contourCount = pGlyphHeader -> countourCount;

    pEndPtsOfContours = new uint16_t[contourCount];

    pbInput += sizeof(otGlyphHeader);

    for ( long k = 0; k < contourCount; k++ )
        BE_TO_LE_16(pbInput,pEndPtsOfContours[k])

    inputPointCount = pEndPtsOfContours[contourCount - 1] + 1;

    contourPointCount = new uint16_t[contourCount];
    contourPointCount[0] = pEndPtsOfContours[0] + 2;
    for ( long k = 1; k < contourCount; k++ )
        contourPointCount[k] = pEndPtsOfContours[k] - pEndPtsOfContours[k - 1] + 1;

    BE_TO_LE_16(pbInput,instructionLength);

    if ( 0 < instructionLength ) {
        pInstructions = new uint8_t[instructionLength];
        memcpy(pInstructions,pbInput,instructionLength * sizeof(uint8_t));
        pbInput += instructionLength;
    }

    uint8_t *pFlagsInputStart = pbInput;

    pointCount = 0;

    while ( pointCount < inputPointCount ) {
        pointCount++;
        if ( ! ( 0 == ( *pbInput++ & REPEAT_FLAG ) ) ) 
            pointCount += *pbInput++;
    }

    pointCount += contourCount;

    uint8_t *pFlagsInputEnd = pbInput;
    uint8_t *pCoordinatesInputStart = pbInput;

    pFlagsFirst = new uint8_t *[contourCount];
    pPointFirstX = new POINT_TYPE *[contourCount];
    pPointFirstY = new POINT_TYPE *[contourCount];

    pFlags = new uint8_t[pointCount];
    pXCoordinates = new POINT_TYPE[pointCount];
    pYCoordinates = new POINT_TYPE[pointCount];

    memset(pFlagsFirst,0,contourCount * sizeof(uint8_t *));
    memset(pPointFirstX,0,contourCount * sizeof(POINT_TYPE *));
    memset(pPointFirstY,0,contourCount * sizeof(POINT_TYPE *));

    memset(pFlags,0,pointCount * sizeof(uint8_t));
    memset(pXCoordinates,0,pointCount * sizeof(POINT_TYPE));
    memset(pYCoordinates,0,pointCount * sizeof(POINT_TYPE));

    uint8_t *pXInput = pCoordinatesInputStart;

    uint8_t *pFResult = pFlags;
    POINT_TYPE *pXResult = pXCoordinates;

    POINT_TYPE x = 0;

    uint16_t contourIndex = 0;
    uint16_t pointsInContour = 0;

    pFlagsFirst[0] = pFResult;
    pPointFirstX[0] = pXResult;

    uint8_t *pfInput = pFlagsInputStart;

    while ( pfInput < pFlagsInputEnd ) {

        *pFResult++ = *pfInput;

        int16_t deltaX = 0;

        uint8_t c = *pfInput++;

        if ( c & X_SHORT_VECTOR ) {

            deltaX = (uint8_t)*pXInput;

            if ( 0 == ( c & X_POSITIVE ) ) 
                deltaX = -deltaX;

            pXInput += 1;

        } else if ( ! ( c & X_SAME ) ) {

            BE_TO_LE_16(pXInput,deltaX);

        }

        pointsInContour++;

        if ( pointsInContour == contourPointCount[contourIndex] ) {
            *pFResult++ = *pFlagsFirst[contourIndex];
            *pXResult++ = *pPointFirstX[contourIndex];
            contourIndex++;
            pointsInContour = 0;
            if ( contourIndex < contourCount ) {
                pFlagsFirst[contourIndex] = pFResult;
                pPointFirstX[contourIndex] = pXResult;
            }
        }

        x += (POINT_TYPE)deltaX;

        *pXResult++ = x;

        if ( 0 == ( c & REPEAT_FLAG ) )
            continue;

        uint8_t count = *pfInput++;

        c = c & ~REPEAT_FLAG;

        for ( ; count > 0; count-- ) {

            *pFResult = c;

            deltaX = 0;

            if ( c & X_SHORT_VECTOR ) {

                deltaX = (uint8_t)*pXInput;

                if ( 0 == ( c & X_POSITIVE ) ) 
                    deltaX = -deltaX;

                pXInput += 1;

            } else if ( ! ( c & X_SAME ) ) {

                BE_TO_LE_16(pXInput,deltaX);

            }

            pointsInContour++;

            if ( pointsInContour == contourPointCount[contourIndex] ) {
                *pFResult++ = *pFlagsFirst[contourIndex];
                *pXResult++ = *pPointFirstX[contourIndex];
                contourIndex++;
                pointsInContour = 0;
                if ( contourIndex < contourCount ) {
                    pFlagsFirst[contourIndex] = pFResult;
                    pPointFirstX[contourIndex] = pXResult;
                }
            }

            x += (POINT_TYPE)deltaX;

            *pXResult = x;

            pFResult++;
            pXResult++;

        }

    }

    if ( contourIndex < contourCount )
        *pXResult = *pPointFirstX[contourIndex];

    POINT_TYPE *pYResult = pYCoordinates;

    uint8_t *pYInput = pXInput;

    POINT_TYPE y = 0;

    contourIndex = 0;
    pointsInContour = 0;

    pPointFirstY[0] = pYResult;

    pfInput = pFlagsInputStart;

    while ( pfInput < pFlagsInputEnd ) {

        if ( 0 == pointsInContour ) 
            pPointFirstY[contourIndex] = pYResult;

        uint8_t c = *pfInput++;

        int16_t deltaY = 0;

        if ( c & Y_SHORT_VECTOR ) {

            deltaY = (uint8_t)*pYInput;

            if ( 0 == ( c & Y_POSITIVE ) ) 
                deltaY = -deltaY;

            pYInput += 1;

        } else if ( ! ( c & Y_SAME ) ) {

            BE_TO_LE_16(pYInput,deltaY);

        }

        y += (POINT_TYPE)deltaY;

        pointsInContour++;

        if ( pointsInContour == contourPointCount[contourIndex] ) {
            *pYResult++ = *pPointFirstY[contourIndex];
            contourIndex++;
            pointsInContour = 0;
            if ( contourIndex < contourCount )
                pPointFirstY[contourIndex] = pYResult;
        }

        *pYResult++ = y;

        if ( 0 == ( c & REPEAT_FLAG ) )
            continue;

        uint8_t count = *pfInput++;

        for ( ; count > 0; count-- ) {

            deltaY = 0;

            if ( c & Y_SHORT_VECTOR ) {

                deltaY = (uint8_t)*pYInput;

                if ( 0 == ( c & Y_POSITIVE ) ) 
                    deltaY = -deltaY;

                pYInput += 1;

            } else if ( ! ( c & Y_SAME ) ) {

                BE_TO_LE_16(pYInput,deltaY);

            }

            pointsInContour++;

            if ( pointsInContour == contourPointCount[contourIndex] ) {
                *pYResult++ = *pPointFirstY[contourIndex];
                contourIndex++;
                pointsInContour = 0;
                if ( contourIndex < contourCount )
                    pPointFirstY[contourIndex] = pYResult;
            }

            y += (POINT_TYPE)deltaY;

            *pYResult++ = y;

        }

    }

    if ( contourIndex < contourCount )
        *pYResult = *pPointFirstY[contourIndex];

    pPoints = new POINTD[pointCount];
    pPointFirst = new POINTD *[contourCount];
    pointsInContour = 0;
    contourIndex = 0;

    pPointFirst[0] = pPoints;

    for ( long k = 0; k < pointCount; k++ ) {

        pPoints[k].px = pXCoordinates + k;
        pPoints[k].py = pYCoordinates + k;

        pointsInContour++;

        if ( pointsInContour > contourPointCount[contourIndex] ) {
            contourIndex++;
            pPointFirst[contourIndex] = pPoints + k;
            pointsInContour = 1;
        }

    }

    long hmTableIndex = min(bGlyph,pFont -> pHorizHeadTable -> numberOfHMetrics - 1);

    otLongHorizontalMetric horizontalMetric = pFont -> pHorizontalMetricsTable -> pHorizontalMetrics[hmTableIndex];

    advanceWidth = horizontalMetric.advanceWidth;
    advanceHeight = 0L;
    leftSideBearing = horizontalMetric.lsb;
    long bearing_y = 0L;

    if ( ! ( NULL == pFont -> pVerticalMetricsTable ) ) {

        otLongVerticalMetric verticalMetric = pFont -> pVerticalMetricsTable -> pVerticalMetrics[bGlyph];

        bearing_y = verticalMetric.topSideBearing;
        advanceHeight = verticalMetric.advanceHeight;

    } else {

        bearing_y = pFont -> pHorizHeadTable -> ascender - pGlyphHeader -> yMax;
        advanceHeight = abs(pFont -> pHorizHeadTable -> ascender - pFont -> pHorizHeadTable -> descender);

    }

    phantomPoints[0] = { pGlyphHeader -> xMin - leftSideBearing,0L };
    phantomPoints[1] = { 0L, pGlyphHeader -> yMax + bearing_y };
    phantomPoints[2] = { pGlyphHeader -> xMin - leftSideBearing + advanceWidth, 0L };
    phantomPoints[3] = { 0L, pGlyphHeader -> yMax + bearing_y - advanceHeight };

    rightSideBearing = advanceWidth - (leftSideBearing + pGlyphHeader -> xMax - pGlyphHeader -> xMin);

    pXResult = pXCoordinates;
    pYResult = pYCoordinates;

    boundingBox[0] = FLT_MAX;
    boundingBox[1] = FLT_MAX;
    boundingBox[2] = -FLT_MAX;
    boundingBox[3] = -FLT_MAX;

    for ( long k = 0; k < pointCount; k++, pXResult++, pYResult++ ) {
        //*pXResult += (int16_t)-phantomPoints[0].x;
        *pXResult += leftSideBearing;
        //*pYResult += (int16_t)phantomPoints[0].y;
        boundingBox[0] = min(boundingBox[0],*pXResult);
        boundingBox[1] = min(boundingBox[1],*pYResult);
        boundingBox[2] = max(boundingBox[2],*pXResult);
        boundingBox[3] = max(boundingBox[3],*pYResult);
    }

    return;
    }


    otSimpleGlyph::~otSimpleGlyph() {

    delete [] pEndPtsOfContours;

    if ( ! ( NULL == pInstructions ) )
        delete [] pInstructions;

    delete [] pFlags;
    delete [] contourPointCount;
    delete [] pXCoordinates;
    delete [] pYCoordinates;
    delete [] pPointFirstX;
    delete [] pPointFirstY;
    delete [] pFlagsFirst;

}

#if 0
    void font::positionSimpleGlyph(otSimpleGlyph *pGlyph,POINTL basePoint) {

    for ( long k = 0; k < pGlyph -> pointCount; k++ ) {
        *pGlyph -> pPoints[k].px = *pGlyph -> pPoints[k].px + pHeadTable -> xMin + basePoint.x;
        *pGlyph -> pPoints[k].py = *pGlyph -> pPoints[k].py + pHeadTable -> yMin + basePoint.y;
    }

    }

    void font::scaleSimpleGlyph(otSimpleGlyph *pGlyph) {

    float scaleFactor = 1000.0f / (float)pHeadTable -> unitsPerEm;
    scaleFactor = pointSize * scaleFactor;
    //if ( ! ( 0.0f == scale ) )
    //    scaleFactor = scale * scaleFactor;

    for ( long k = 0; k < pGlyph -> pointCount; k++ ) {
        *pGlyph -> pPoints[k].px = scaleFactor * *pGlyph -> pPoints[k].px;
        *pGlyph -> pPoints[k].py = scaleFactor * *pGlyph -> pPoints[k].py;
    }

    return;
    }
#endif