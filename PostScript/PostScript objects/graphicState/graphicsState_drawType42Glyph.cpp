
#include "PostScript objects/graphicsState.h"
#include "PostScript objects/font.h"

    void graphicsState::drawGlyph(BYTE bGlyph) {

    if ( NULL == pPStoPDF -> GetDC() )
        return;

    pJob -> push(pCurrentFont);
    pJob -> push(pJob -> pFontTypeLiteral);
    pJob -> operatorGet();

    object *pFontType = pJob -> pop();

    if ( 0 == strcmp(pFontType -> Contents(),"3") ) {
        drawType3Glyph(bGlyph);
        return;
    }

    if ( ! ( 0 == strcmp(pFontType -> Contents(),"42") ) )
        return;

    BYTE *pbGlyphData = NULL;

/*
    5.5 GlyphDirectory 

        ...

        Starting with PostScript version 2015, the interpreter checks for the existence
        of a gdir table in the sfnts array, and if found, uses GlyphDirectory in place of
        the loca and glyf tables.
*/

    if ( ! ( NULL == pCurrentFont -> tableDirectory.table("gdir") ) ) {

        pJob -> push(pCurrentFont);
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

    } else {

        uint32_t offsetInGlyphTable = 0;
        uint32_t *pGlyphOffset = (uint32_t *)pCurrentFont -> pLocaTable + pCurrentFont -> glyphIDMap[bGlyph];
        BE_TO_LE_U32(pGlyphOffset,offsetInGlyphTable)
        pbGlyphData = pCurrentFont -> pGlyfTable + offsetInGlyphTable;

    }

    otGlyphHeader glyphHeader{0};

    otSimpleGlyph *pSimpleGlyph = NULL;

    glyphHeader.load(pbGlyphData);

    if ( 0 > glyphHeader.countourCount ) 
        return;

    GS_POINT basePoint = currentUserSpacePoint;

    //if ( 0 == pCurrentFont -> glyphIDMap[bGlyph] ) {
    //    POINT_TYPE deltaX = pSimpleGlyph -> advanceWidth;
    //    basePoint.x += pCurrentFont -> pointSize * deltaX / (POINT_TYPE)pCurrentFont -> pHeadTable -> unitsPerEm;
    //    moveto(&basePoint);
    //    return;
    //}

    pSimpleGlyph = new otSimpleGlyph(bGlyph,pCurrentFont,this,&glyphHeader,pbGlyphData);

    scale(pSimpleGlyph -> pPoints,pSimpleGlyph -> pointCount,1.0 / (POINT_TYPE)pCurrentFont -> pHeadTable -> unitsPerEm);

    pCurrentFont -> transformGlyphInPlace(pSimpleGlyph -> pPoints,pSimpleGlyph -> pointCount);

    class matrix *pMatrix = new (pJob -> CurrentObjectHeap()) matrix(pJob);

    pMatrix -> tx(basePoint.x);
    pMatrix -> ty(basePoint.y);

    concat(pMatrix);

    for ( long k = 0; k < pSimpleGlyph -> pGlyphHeader -> countourCount; k++ ) {

        uint8_t *pOnCurve = pSimpleGlyph -> pFlagsFirst[k];

        uint16_t pointCount = pSimpleGlyph -> contourPointCount[k];

        GS_POINT *pCurr = pSimpleGlyph -> pPointFirst[k] + pointCount - 1;
        GS_POINT *pNext = pSimpleGlyph -> pPointFirst[k];

        uint16_t currCurveIndex = pointCount - 1;
        uint16_t nextCurveIndex = 0;

        if ( pOnCurve[currCurveIndex] )
            moveto(pCurr -> x,pCurr -> y);
        else
            if ( pOnCurve[nextCurveIndex] )
                moveto(pNext -> x,pNext -> y);
            else 
                moveto((pCurr -> x + pNext -> x) / 2.0f,(pCurr -> y + pNext -> y) / 2.0f);

        for ( long pointIndex = 0; pointIndex < pointCount; pointIndex++ ) {

            pCurr = pNext;
            currCurveIndex = nextCurveIndex;

            pNext = pSimpleGlyph -> pPointFirst[k] + ( pointIndex + 1 ) % pointCount;
            nextCurveIndex = ( pointIndex + 1 ) % pointCount;

            if ( pOnCurve[currCurveIndex] )
                lineto(pCurr -> x,pCurr -> y);

            else {

                GS_POINT next2{pNext -> x,pNext -> y};

                if ( ! pOnCurve[nextCurveIndex] ) {
                    next2.x = (pCurr -> x + pNext -> x) / 2.0;
                    next2.y = (pCurr -> y + pNext -> y) / 2.0;
                }

                GS_POINT currentPoint{currentUserSpacePoint.x,currentUserSpacePoint.y};

                GS_POINT *pCurrent = &currentPoint;
                GS_POINT *pOne = pCurr;
                GS_POINT *pEnd = &next2;

                for ( POINT_TYPE t = 0.0; t <= 1.0; t += BEZIER_CURVE_GRANULARITY ) {
                    POINT_TYPE x = (1.0 - t) * (1.0 - t) * pCurrent -> x + 2.0 * (1.0 - t) * t * pOne -> x + t * t * pEnd -> x;
                    POINT_TYPE y = (1.0 - t) * (1.0 - t) * pCurrent -> y + 2.0 * (1.0 - t) * t * pOne -> y + t * t * pEnd -> y;
                    lineto(x,y);
                }

            }
        }

    }

    fillpath();

    revertMatrix();

    POINT_TYPE deltaX = pSimpleGlyph -> advanceWidth;

    basePoint.x += pCurrentFont -> pointSize * deltaX / (POINT_TYPE)pCurrentFont -> pHeadTable -> unitsPerEm;

    moveto(&basePoint);

    delete pSimpleGlyph;

    return;
    }

