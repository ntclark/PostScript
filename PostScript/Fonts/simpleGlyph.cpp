
#include "job.h"

    otSimpleGlyph::otSimpleGlyph(BYTE bGlyph,font *pFont,graphicsState *pGraphicsState,otGlyphHeader *ph,BYTE *pbInput) : 
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
    contourPointCount[0] = pEndPtsOfContours[0] + 1;
    for ( long k = 1; k < contourCount; k++ )
        contourPointCount[k] = pEndPtsOfContours[k] - pEndPtsOfContours[k - 1];

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

    uint8_t *pFlagsInputEnd = pbInput;

    pFlags = new uint8_t[pointCount];
    memset(pFlags,0,pointCount * sizeof(uint8_t));

    pointCount = 0;

    pbInput = pFlagsInputStart;

    while ( pointCount < inputPointCount ) {
        pFlags[pointCount] = *pbInput;
        if ( 0 == ( pFlags[pointCount] & REPEAT_FLAG ) ) {
            pointCount++;
            pbInput++;
            continue;
        }
        pbInput++;
        pFlags[pointCount] &= ~ REPEAT_FLAG;
        uint8_t flag = pFlags[pointCount];
        uint8_t repeatCount = *pbInput;
        for ( uint8_t k = 0; k < repeatCount; k++ )
            pFlags[++pointCount] = flag;
        pointCount++;
        pbInput++;
    }

    uint8_t *pCoordinatesInputStart = pbInput;

    pFlagsFirst = new uint8_t *[contourCount];
    pPointFirstX = new POINT_TYPE *[contourCount];
    pPointFirstY = new POINT_TYPE *[contourCount];

    pXCoordinates = new POINT_TYPE[pointCount];
    pYCoordinates = new POINT_TYPE[pointCount];

    memset(pFlagsFirst,0,contourCount * sizeof(uint8_t *));
    memset(pPointFirstX,0,contourCount * sizeof(POINT_TYPE *));
    memset(pPointFirstY,0,contourCount * sizeof(POINT_TYPE *));

    memset(pXCoordinates,0,pointCount * sizeof(POINT_TYPE));
    memset(pYCoordinates,0,pointCount * sizeof(POINT_TYPE));

    uint8_t *pXInput = pCoordinatesInputStart;

    uint8_t *pFResult = pFlags;
    POINT_TYPE *pXResult = pXCoordinates;

    POINT_TYPE x = 0;

    uint16_t contourIndex = 0;

    pFlagsFirst[contourIndex] = pFResult;
    pPointFirstX[contourIndex] = pXResult;

    for ( uint8_t pointIndex = 0; pointIndex < pointCount; pointIndex++ ) {

        int16_t deltaX = 0;

        if ( pFlags[pointIndex] & X_SHORT_VECTOR ) {

            deltaX = (uint8_t)*pXInput;

            if ( 0 == ( pFlags[pointIndex] & X_POSITIVE ) ) 
                deltaX = -deltaX;

            pXInput += 1;

        } else if ( 0 == ( pFlags[pointIndex] & X_SAME ) ) {

            BE_TO_LE_16(pXInput,deltaX);

        }

        x += (POINT_TYPE)deltaX;

        pXResult[pointIndex] = x;

        if ( pointIndex == pEndPtsOfContours[contourIndex] ) {
            contourIndex++;
            if ( contourIndex < contourCount ) {
                pFlagsFirst[contourIndex] = pFResult + pointIndex + 1;
                pPointFirstX[contourIndex] = pXResult + pointIndex + 1;
            }
        }

    }

    POINT_TYPE *pYResult = pYCoordinates;

    uint8_t *pYInput = pXInput;

    POINT_TYPE y = 0;

    contourIndex = 0;

    pPointFirstY[contourIndex] = pYResult;

    for ( uint8_t pointIndex = 0; pointIndex < pointCount; pointIndex++ ) {

        int16_t deltaY = 0;

        if ( pFlags[pointIndex] & Y_SHORT_VECTOR ) {

            deltaY = (uint8_t)*pYInput;

            if ( 0 == ( pFlags[pointIndex] & Y_POSITIVE ) ) 
                deltaY = -deltaY;

            pYInput += sizeof(uint8_t);

        } else if ( 0 == ( pFlags[pointIndex] & Y_SAME ) ) {

            BE_TO_LE_16(pYInput,deltaY);

        }

        y += (POINT_TYPE)deltaY;

        // Only the on-curve flag is relevant after
        // parsing the points

        pFResult[pointIndex] = pFlags[pointIndex] & ON_CURVE_POINT;

        pYResult[pointIndex] = y;

        if ( pointIndex == pEndPtsOfContours[contourIndex] ) {
            contourIndex++;
            if ( contourIndex < contourCount )
                pPointFirstY[contourIndex] = pYResult + pointIndex + 1;
        }

    }

    pPoints = new GS_POINT[pointCount];
    pPointFirst = new GS_POINT *[contourCount];

    contourIndex = 0;

    pPointFirst[contourIndex] = pPoints;

    for ( long k = 0; k < pointCount; k++ ) {
        pPoints[k].x = *(pXCoordinates + k);
        pPoints[k].y = *(pYCoordinates + k);
        if ( k == pEndPtsOfContours[contourIndex] ) {
            contourIndex++;
            if ( contourIndex < contourCount )
                pPointFirst[contourIndex] = pPoints + k + 1;
        }
    }

    uint16_t glyphId = pFont -> glyphIDMap[(uint16_t)bGlyph];

    long hmTableIndex = min((uint16_t)glyphId,pFont -> pHorizHeadTable -> numberOfHMetrics - 1);

    otLongHorizontalMetric *pHorizontalMetric = pFont -> pHorizontalMetricsTable -> pHorizontalMetrics + hmTableIndex;

    advanceWidth = pHorizontalMetric -> advanceWidth;
    advanceHeight = 0L;
    leftSideBearing = pHorizontalMetric -> lsb;
    if ( pFont -> pHeadTable -> flags & 0x0010 )
        leftSideBearing = pGlyphHeader -> xMin;

    long bearing_y = 0L;

    if ( ! ( NULL == pFont -> pVerticalMetricsTable ) ) {

        otLongVerticalMetric verticalMetric = pFont -> pVerticalMetricsTable -> pVerticalMetrics[bGlyph];

        bearing_y = verticalMetric.topSideBearing;
        advanceHeight = verticalMetric.advanceHeight;

    } else {

        bearing_y = pFont -> pHorizHeadTable -> ascender - pGlyphHeader -> yMax;
        advanceHeight = abs(pFont -> pHorizHeadTable -> ascender - pFont -> pHorizHeadTable -> descender);

    }

/*
    The Microsoft rasterizer v.1.7 or later will always add four “phantom points” to the end
    of every outline to allow either widths or heights to be controlled.

    Point “n” will be placed at the glyph left sidebearing point; point “n+1” will be placed at the
    advance width point; point “n+2” will be placed at the top origin; and point “n+3” will
    be placed at the advance height point.
*/
    phantomPoints[0] = { /*pGlyphHeader -> xMin - */leftSideBearing,0L };
    phantomPoints[1] = { /*pGlyphHeader -> xMin - leftSideBearing + */advanceWidth, 0L };
    phantomPoints[2] = { 0L, /*pGlyphHeader -> yMax +*/ bearing_y };
    phantomPoints[3] = { 0L, /*pGlyphHeader -> yMax + bearing_y - */advanceHeight };

    rightSideBearing = advanceWidth - (leftSideBearing + pGlyphHeader -> xMax - pGlyphHeader -> xMin);

    pXResult = pXCoordinates;
    pYResult = pYCoordinates;

    boundingBox[0] = FLT_MAX;
    boundingBox[1] = FLT_MAX;
    boundingBox[2] = -FLT_MAX;
    boundingBox[3] = -FLT_MAX;

    for ( long k = 0; k < pointCount; k++, pXResult++, pYResult++ ) {
        *pXResult += leftSideBearing;
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