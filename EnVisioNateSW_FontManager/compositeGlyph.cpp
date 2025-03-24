
#include "font.h"

    otCompositeGlyph::otCompositeGlyph(font *pFont,otGlyphHeader *ph,BYTE *pbInput) : 
        pGlyphHeader(ph),
        pbGlyphData(pbInput)
    {

    pbInput += sizeof(otGlyphHeader);

    countComponents = 0;

    uint16_t theFlags;
    uint16_t dummy;

    do {

        BE_TO_LE_U16(pbInput,theFlags)
        BE_TO_LE_U16(pbInput,dummy)

        pbInput += sizeof(uint16_t);

        if ( theFlags & ARG_1_AND_2_ARE_WORDS ) 
            pbInput += sizeof(uint16_t);

        if ( theFlags & WE_HAVE_A_SCALE )
            pbInput += sizeof(uint16_t);
        else if ( theFlags & WE_HAVE_AN_X_AND_Y_SCALE )
            pbInput += 2 * sizeof(uint16_t);
        else if ( theFlags & WE_HAVE_A_TWO_BY_TWO )
            pbInput += 4 * sizeof(uint16_t);

        countComponents++;

    } while ( theFlags & MORE_COMPONENTS );

    ppGlyphRecord = new componentGlyphRecord*[countComponents];

    pbInput = pbGlyphData;
    pbInput += sizeof(otGlyphHeader);

    countComponents = 0;

    boolean doContinue = true;

    do {

        componentGlyphRecord *p = new componentGlyphRecord();

        ppGlyphRecord[countComponents] = p;

        BE_TO_LE_U16(pbInput,p -> flags)
        BE_TO_LE_U16(pbInput,p -> glyphIndex)

        p -> arePointNumbers = true;
        if ( p -> flags & ARGS_ARE_XY_VALUES ) 
            p -> arePointNumbers = false;

        if ( p -> flags & ARG_1_AND_2_ARE_WORDS) {
            if ( p -> flags & ARGS_ARE_XY_VALUES ) {
                BE_TO_LE_16(pbInput,p -> argument1.X)
                BE_TO_LE_16(pbInput,p -> argument2.Y)
            } else {
                BE_TO_LE_U16(pbInput,p -> argument1.parentPointNumber)
                BE_TO_LE_U16(pbInput,p -> argument2.childPointNumber)
            }
        } else {
            if ( p -> flags & ARGS_ARE_XY_VALUES ) {
                p -> argument1.vint8 = *(int8_t *)pbInput++;
                p -> argument2.vint8 = *(int8_t *)pbInput++;
                p -> argument1.X = (int16_t)p -> argument1.vint8;
                p -> argument2.Y = (int16_t)p -> argument2.vint8;
            } else {
                p -> argument1.vuint8 = *(uint8_t *)pbInput++;
                p -> argument2.vuint8 = *(uint8_t *)pbInput++;
                p -> argument1.parentPointNumber = (uint16_t)p -> argument1.vuint8;
                p -> argument2.childPointNumber = (uint16_t)p -> argument2.vuint8;
            }
        }

        p -> has1Scale = false;
        p -> has2Scale = false;
        p -> hasTransform = false;

        if ( p -> flags & WE_HAVE_A_SCALE ) {
            uint16_t f2dot14;
            BE_TO_LE_U16(pbInput,f2dot14)
            LEU16_TO_F2DOT14(f2dot14,p -> scale[0])
            p -> scale[1] = p -> scale[0];
            p -> has1Scale = true;
        } else if ( p -> flags & WE_HAVE_AN_X_AND_Y_SCALE ) {
            uint16_t f2dot14;
            BE_TO_LE_U16(pbInput,f2dot14)
            LEU16_TO_F2DOT14(f2dot14,p -> scale[0])
            BE_TO_LE_U16(pbInput,f2dot14)
            LEU16_TO_F2DOT14(f2dot14,p -> scale[1])
            p -> has2Scale = true;
        } else if ( p -> flags & WE_HAVE_A_TWO_BY_TWO ) {
            uint16_t f2dot14;
            BE_TO_LE_U16(pbInput,f2dot14)
            LEU16_TO_F2DOT14(f2dot14,p -> scale[0])
            BE_TO_LE_U16(pbInput,f2dot14)
            LEU16_TO_F2DOT14(f2dot14,p -> scale[1])
            BE_TO_LE_U16(pbInput,f2dot14)
            LEU16_TO_F2DOT14(f2dot14,p -> scale[2])
            BE_TO_LE_U16(pbInput,f2dot14)
            LEU16_TO_F2DOT14(f2dot14,p -> scale[3])
            p -> hasTransform = true;
        }

        countComponents++;

        doContinue = p -> flags & MORE_COMPONENTS;

    } while ( doContinue );

    ppSimpleGlyphs = new otSimpleGlyph*[countComponents];
    ppCompositeGlyphs = new otCompositeGlyph*[countComponents];
    ppGlyphGeometrys = new otGlyphGeometry*[countComponents];

    //ppGlyphRecord = new componentGlyphRecord*[countComponents];

    countSimpleGlyphs = 0;
    countCompositeGlyphs = 0;

    for ( uint16_t k = 0; k < countComponents; k++ ) {

        componentGlyphRecord *pComponent = ppGlyphRecord[k];

        BYTE *pbComponentGlyphData = pFont -> getGlyphData(pComponent -> glyphIndex);

        otGlyphHeader componentGlyphHeader{0};
        componentGlyphHeader.load(pbComponentGlyphData);

        //ppGlyphRecord[k] = pComponent;

        if ( 0 < componentGlyphHeader.contourCount ) {
            ppSimpleGlyphs[countSimpleGlyphs] = new otSimpleGlyph(pComponent -> glyphIndex,pFont,&componentGlyphHeader,pbComponentGlyphData);
            ppGlyphGeometrys[k] = static_cast<otGlyphGeometry *>(ppSimpleGlyphs[countSimpleGlyphs]);
            countSimpleGlyphs++;
        } else {
            ppCompositeGlyphs[countCompositeGlyphs] = new otCompositeGlyph(pFont,&componentGlyphHeader,pbComponentGlyphData);
            ppGlyphGeometrys[k] = static_cast<otGlyphGeometry *>(ppCompositeGlyphs[countCompositeGlyphs]);
            countCompositeGlyphs++;
        }

        contourCount += ppGlyphGeometrys[k] -> ContourCount();

        pointCount += ppGlyphGeometrys[k] -> PointCount();

        advanceWidth = max(advanceWidth,(int32_t)ppGlyphGeometrys[k] -> AdvanceWidth());

    }

    pPoints = new POINT[pointCount];
    pFlags = new uint8_t[pointCount];

    contourPointCount = new uint16_t[contourCount];
    pEndPtsOfContours = new uint16_t[contourCount];

    pPointFirst = new POINT *[contourCount];
    pFlagsFirst = new uint8_t*[contourCount];

    uint16_t endIndex = 0;
    uint16_t pointIndex = 0;
    uint16_t contourIndex = 0;

    for ( uint16_t k = 0; k < countComponents; k++ ) {

        otGlyphGeometry *pGlyphGeometry = ppGlyphGeometrys[k];

        componentGlyphRecord *pComponent = ppGlyphRecord[k];

        uint16_t componentGlyphPointIndex = 0;

        for ( uint16_t j = 0; j < pGlyphGeometry -> ContourCount(); j++ ) {

            pEndPtsOfContours[contourIndex] = pGlyphGeometry -> ContourLastIndex(j);

            contourPointCount[contourIndex] = 0;

            pPointFirst[contourIndex] = pPoints + pointIndex;
            pFlagsFirst[contourIndex] = pFlags + pointIndex;

            while ( componentGlyphPointIndex <= pGlyphGeometry -> ContourLastIndex(j) ) {

                POINT pt = {(pGlyphGeometry -> Points() + componentGlyphPointIndex) -> x,(pGlyphGeometry -> Points() + componentGlyphPointIndex) -> y};

                if ( ! pComponent -> arePointNumbers ) {

                    pt.x += pComponent -> argument1.X;
                    pt.y += pComponent -> argument2.Y;

                    if ( pComponent -> has1Scale ) {
                        pt.x = (LONG)((FLOAT)pt.x * pComponent -> scale[0]);
                        pt.y = (LONG)((FLOAT)pt.y * pComponent -> scale[0]);
                    } else if ( pComponent -> has2Scale ) {
                        pt.x = (LONG)((FLOAT)pt.x * pComponent -> scale[0]);
                        pt.y = (LONG)((FLOAT)pt.y * pComponent -> scale[1]);
                    } else if ( pComponent -> hasTransform) {
                        POINT xx = {pt.x,pt.y};
                        FLOAT x = (FLOAT)pt.x * pComponent -> scale[0] + (FLOAT)pt.y * pComponent -> scale[1];
                        FLOAT y = (FLOAT)pt.x * pComponent -> scale[2] + (FLOAT)pt.y * pComponent -> scale[3];
                        pt.x = (LONG)x;
                        pt.y = (LONG)y;
                    }
                    pPoints[pointIndex].x = pt.x;
                    pPoints[pointIndex].y = pt.y;

                } else {

MessageBox(NULL,"Not implemented","Error",MB_OK);
                    POINT pt = {(pGlyphGeometry -> Points() + componentGlyphPointIndex) -> x,(pGlyphGeometry -> Points() + componentGlyphPointIndex) -> y};

                }

                pFlags[pointIndex] = *(pGlyphGeometry -> Flags() + componentGlyphPointIndex);

                componentGlyphPointIndex++;
                pointIndex++;
                contourPointCount[contourIndex]++;
            }

            contourIndex++;

        }

    }

    return;
    }


    otCompositeGlyph::~otCompositeGlyph() {

    for ( long k = 0; k < countSimpleGlyphs; k++ )
        delete ppSimpleGlyphs[k];
    delete [] ppSimpleGlyphs;

    for ( long k = 0; k < countCompositeGlyphs; k++ )
        delete ppCompositeGlyphs[k];
    delete [] ppCompositeGlyphs;

    for ( long k = 0; k < countComponents; k++ )
        delete ppGlyphRecord[k];
    delete [] ppGlyphRecord;

    delete [] ppGlyphGeometrys;

    delete [] pPoints;
    delete [] pFlags;

    delete [] contourPointCount;
    delete [] pEndPtsOfContours;

    delete [] pPointFirst;
    delete [] pFlagsFirst;

    return;
    }