
#include "PostScript objects/graphicsState.h"
#include "PostScript objects/font.h"

    void graphicsState::drawGlyph(BYTE bGlyph) {

    uint32_t locTableEntrySize = 16;
    if ( 1 == pCurrentFont -> pHeadTable -> indexToLocFormat )
        locTableEntrySize = 32;


    if ( NULL == hdcSurface )
        return;

    pJob -> push(pCurrentFont);
    pJob -> push(pJob -> pEncodingLiteral);
    pJob -> operatorGet();

    // The BYTE bGlyph is the "index" into the Encoding array

    pJob -> push(new (pJob -> CurrentObjectHeap()) object(pJob,bGlyph));
    pJob -> operatorGet();

    // The string object pGlyphName is the index into the CharStrings array

    object *pGlyphName = pJob -> pop();

    pJob -> push(pCurrentFont);
    pJob -> push(pJob -> pCharStringsLiteral);
    pJob -> operatorGet();
    pJob -> push(pGlyphName);
    pJob -> operatorGet();

    // pLocaAndHtmxOffset is the "value" in the CharStrings array AT the index/key with pGlyphName
    //
    // It is the offset into the htmx and loca tables.
    //
    // Resolve the offset into the glyf table as the entry AT the offset in the loca table
    // Example: At the nth BYTE into the loca table is a number, either 16 or 32 bit,
    //      THAT # is the offset into the glyf table.
    // 
    // The relevant data from the htmx table STARTS at this offset value

    object *pLocaAndHtmxOffset = pJob -> pop();

    uint32_t offsetInGlyphTable = 0;

    BYTE *pGlyphOffset = pCurrentFont -> pLocaTable + pLocaAndHtmxOffset -> IntValue();

    if ( 0 == pCurrentFont -> pHeadTable -> indexToLocFormat ) {
        BE_TO_LE_16(pGlyphOffset,offsetInGlyphTable)
        offsetInGlyphTable *= 2;
    } else
        BE_TO_LE_32(pGlyphOffset,offsetInGlyphTable)

/*
    5.5 GlyphDirectory 

        ...

        Starting with PostScript version 2015, the interpreter checks for the existence
        of a gdir table in the sfnts array, and if found, uses GlyphDirectory in place of
        the loca and glyf tables.
*/

    BYTE *pbGlyphData = NULL;
    long cbGlyphData = 0L;
    BYTE *pbHorizMetrixData = NULL;
    BYTE *pbVertMetrixData = NULL;

    if ( ! ( NULL == pCurrentFont -> tableDirectory.table("gdir") ) ) {

        pJob -> push(pCurrentFont);
        pJob -> push(pJob -> pGlyphDirectoryLiteral);
        pJob -> operatorGet();

        pJob -> push(pLocaAndHtmxOffset);
        pJob -> operatorGet();

        class binaryString *pBinaryString = reinterpret_cast<class binaryString *>(pJob -> pop());
        pbGlyphData =  pBinaryString -> getData();
        cbGlyphData = pBinaryString -> length();

    } else {

        pbGlyphData = pCurrentFont -> pGlyfTable + offsetInGlyphTable;

        // cbGlyphData = ???

    }

    otGlyphHeader glyphHeader{0};

    otSimpleGlyph *pSimpleGlyph = NULL;

    glyphHeader.load(pbGlyphData);

    if ( -1 < glyphHeader.countourCount ) {

        std::vector<POINTD *> theOnPoints;
        std::vector<POINTD *> theOffPoints;

        pSimpleGlyph = new otSimpleGlyph(bGlyph,pCurrentFont,this,&glyphHeader,pbGlyphData,cbGlyphData);

        POINTL basePoint = lastPoint;

        pCurrentFont -> positionSimpleGlyph(pSimpleGlyph,basePoint);

        //pCurrentFont -> scaleSimpleGlyph(pSimpleGlyph);

        POINTD *pPoints = pSimpleGlyph -> pPoints;
        uint8_t *pFlags = pSimpleGlyph -> pFlags;

        long pointIndex = 0;

        for ( long k = 0; k < pSimpleGlyph -> pGlyphHeader -> countourCount; k++ ) {

            boolean beginPointOnCurve = false;
            boolean lastPointOnCurve = false;

            POINTD *pBeginPoint = NULL;
            POINTD *pEndPoint = NULL;

            for ( long j = 0; j < pSimpleGlyph -> contourPointCount[k]; j++,pointIndex++ ) {

                POINTD *pThisPoint = pPoints + pointIndex;
                uint8_t *pFlag = pFlags + pointIndex;

                boolean onCurve = *pFlag & ON_CURVE_POINT;

                if ( pPStoPDF -> drawGlyphRenderingPoints() ) {
                    if ( onCurve )
                        theOnPoints.push_back(pThisPoint);
                    else
                        theOffPoints.push_back(pThisPoint);
                }

                if ( 0 == j ) { 

                    pBeginPoint = pThisPoint;
                    beginPointOnCurve = onCurve;

                    if ( onCurve )
                        thesePoints.push_back(pThisPoint);

                } else {

                    POINTD *pPoint2 = onCurve ? pThisPoint : lerp(pEndPoint,pThisPoint,0.5f);

                    if ( 0 == thesePoints.size() || ( lastPointOnCurve && onCurve ) ) 
                         thesePoints.push_back(pPoint2);

                    else if ( ( ! lastPointOnCurve ) || onCurve ) {

                        POINTD *pPoint1 = thesePoints.back();
                        POINTD *pControl1 = lerp(pPoint1,pEndPoint,2.0f / 3.0f);
                        POINTD *pControl2 = lerp(pPoint2,pEndPoint,2.0f / 3.0f);
                        bezierCurve( pPoint1, pControl1, pControl2, pPoint2, 0.125f);

                    }

                }

                pEndPoint = pThisPoint;
                lastPointOnCurve = onCurve;

            }

            boolean isFirst = true;
            for ( POINTD *pPoint : thesePoints ) {
                if ( isFirst ) {
                    moveto(pPoint);
                    isFirst = false;
                    continue;
                }
                lineto(pPoint);
            }

            lineto(thesePoints[0]);

            thesePoints.clear();

            if ( pPStoPDF -> drawGlyphRenderingPoints() ) {

                for ( POINTD *pPoint : theOffPoints ) {
                    moveto(pPoint);
                    rmoveto(GLYPH_POINT_TIC_SIZE,GLYPH_POINT_TIC_SIZE);
                    rlineto(-2*GLYPH_POINT_TIC_SIZE,0L);
                    rlineto(0L,-2*GLYPH_POINT_TIC_SIZE);
                    rlineto(2*GLYPH_POINT_TIC_SIZE,0L);
                    rlineto(0L,2*GLYPH_POINT_TIC_SIZE);
                }

                for ( POINTD *pPoint : theOnPoints ) {
                    moveto(pPoint);
                    rmoveto(GLYPH_POINT_TIC_SIZE,GLYPH_POINT_TIC_SIZE);
                    rlineto(-2*GLYPH_POINT_TIC_SIZE,-2*GLYPH_POINT_TIC_SIZE);
                    rmoveto(2*GLYPH_POINT_TIC_SIZE,0);
                    rlineto(-2*GLYPH_POINT_TIC_SIZE,2*GLYPH_POINT_TIC_SIZE);
                }

                theOffPoints.clear();
                theOnPoints.clear();

            }

        }

        delete pSimpleGlyph;

        basePoint.x += glyphHeader.xMax - glyphHeader.xMin;

        moveto(basePoint);

    }

    return;
    }

