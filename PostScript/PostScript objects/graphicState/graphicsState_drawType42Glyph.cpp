
#define CANVAS_ITY_IMPLEMENTATION
#include "canvas_ity.hpp"
#include <fstream>

#include "PostScript objects/graphicsState.h"
#include "PostScript objects/font.h"

    void graphicsState::drawGlyph(BYTE bGlyph) {

#if 0

    long height = 768;
    long width = 1024;

    canvas_ity::canvas theCanvas(1024,768);

    FILE *fX = fopen("C:\\Development\\PostScript\\arial.ttf","rb");

    fseek(fX,0,SEEK_END);
    DWORD cb = ftell(fX);
    fseek(fX,0,SEEK_SET);

    BYTE *pbFont = new BYTE[cb];
    fread(pbFont,1,cb,fX);

    fclose(fX);

    theCanvas.set_font(pbFont,cb,64.0f);

    theCanvas.set_color(canvas_ity::fill_style,0.0f,1.0f,0.0f,1.0f);
    theCanvas.set_color(canvas_ity::stroke_style,0.0f,1.0f,0.0f,1.0f);

    theCanvas.stroke_text("Hello World",64.0f,64.0f,1000.0f);

    theCanvas.set_color(canvas_ity::fill_style,0.25f,0.25f,0.25f,0.0f);
    theCanvas.fill_rectangle( 0.0f, 0.0f, 1024, 768.0f );

    // Fetch the rendered RGBA pixels from the entire canvas.
    unsigned char *image = new unsigned char[ height * width * 4 ];
    theCanvas.get_image_data( image, width, height, width * 4, 0, 0 );
    // Write them out to a TGA image file (TGA uses BGRA order).
    unsigned char header[] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        width & 255, width >> 8, height & 255, height >> 8, 32, 40 };
    for ( int pixel = 0; pixel < height * width; ++pixel )
        std::swap( image[ pixel * 4 + 0 ], image[ pixel * 4 + 2 ] );

    std::ofstream stream( "C:\\Development\\PostScript\\example.tga", std::ios::binary );
    stream.write( reinterpret_cast< char * >( header ), sizeof( header ) );
    stream.write( reinterpret_cast< char * >( image ), height * width * 4 );

    stream.close();

#endif

    if ( NULL == hdcSurface )
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

    pJob -> push(pCurrentFont);
    pJob -> push(pJob -> pEncodingArrayLiteral);
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

        GS_POINT basePoint = lastUserSpacePoint;

#if 0
        pCurrentFont -> positionSimpleGlyph(pSimpleGlyph,basePoint);
        pCurrentFont -> scaleSimpleGlyph(pSimpleGlyph);
#endif

        POINTD *pPoints = pSimpleGlyph -> pPoints;
        uint8_t *pFlags = pSimpleGlyph -> pFlags;

        scale(pPoints,pSimpleGlyph -> pointCount,pCurrentFont -> scaleFUnitsToPoints);

        pCurrentFont -> transformGlyphInPlace(pPoints,pSimpleGlyph -> pointCount);

        transformInPlace(pPoints,pSimpleGlyph -> pointCount);

        POINTD ptBaseline{(POINT_TYPE)lastUserSpacePoint.x,(POINT_TYPE)(/*userSpaceDomain.y - */lastUserSpacePoint.y)};

        translate(pPoints,pSimpleGlyph -> pointCount,&ptBaseline);

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

GS_POINT *pGSPoint = new GS_POINT(pThisPoint);

                    if ( onCurve )
                        thesePoints.push_back(pGSPoint);//pThisPoint);

                } else {

                    POINTD *pPoint2 = onCurve ? pThisPoint : lerp(pEndPoint,pThisPoint,0.5f);

                    if ( 0 == thesePoints.size() || ( lastPointOnCurve && onCurve ) ) 
{
GS_POINT *pGSPoint = new GS_POINT(pPoint2);
                         thesePoints.push_back(pGSPoint);//pPoint2);
}

                    else if ( ( ! lastPointOnCurve ) || onCurve ) {

GS_POINT *pGSPoint = thesePoints.back();
                        POINTD *pPoint1 = new POINTD(pGSPoint);//thesePoints.back();
                        POINTD *pControl1 = lerp(pPoint1,pEndPoint,2.0f / 3.0f);
                        POINTD *pControl2 = lerp(pPoint2,pEndPoint,2.0f / 3.0f);
                        bezierCurve( pPoint1, pControl1, pControl2, pPoint2, 0.125f);

                    }

                }

                pEndPoint = pThisPoint;
                lastPointOnCurve = onCurve;

            }

            boolean isFirst = true;
            //for ( POINTD *pPoint : thesePoints ) {
            for ( GS_POINT *pPoint : thesePoints ) {
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

        moveto(&basePoint);

        delete pSimpleGlyph;

    }

    return;
    }

