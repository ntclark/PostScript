#pragma once

#include "EnVisioNateSW_FontManager.h"

#include "matrix.h"

class job;
class font;
class graphicsState;

#define FT_CURVE_TAG_CONIC         0x00
#define FT_CURVE_TAG_CUBIC         0x02

#define CHARSET_KEY 15
#define ENCODING_KEY 16
#define CHARSTRINGS_KEY 17
#define CHARSTRINGTYPE_KEY 6

#define CUBIC_SPLINE_SECTIONS 100.0

#define ON_CURVE_POINT  0x01

#define X_SHORT_VECTOR  0x02
#define Y_SHORT_VECTOR  0x04
#define REPEAT_FLAG     0x08
#define X_POSITIVE      0x10  /* two meanings depending on X_SHORT_VECTOR */
#define X_SAME          0x10
#define Y_POSITIVE      0x20  /* two meanings depending on Y_SHORT_VECTOR */
#define Y_SAME          0x20

#define _WS2_32_WINSOCK_SWAP_LONGLONG(l)        \
(   ( ((l) >> 56) & 0x00000000000000FFLL ) |    \
    ( ((l) >> 40) & 0x000000000000FF00LL ) |    \
    ( ((l) >> 24) & 0x0000000000FF0000LL ) |    \
    ( ((l) >>  8) & 0x00000000FF000000LL ) |    \
    ( ((l) <<  8) & 0x000000FF00000000LL ) |    \
    ( ((l) << 24) & 0x0000FF0000000000LL ) |    \
    ( ((l) << 40) & 0x00FF000000000000LL ) |    \
    ( ((l) << 56) & 0xFF00000000000000LL ) )


__inline unsigned __int64 htonll ( unsigned __int64 Value ) {
    const unsigned __int64 Retval = _WS2_32_WINSOCK_SWAP_LONGLONG (Value);
    return Retval;
}

#define BE_TO_LE_16(pb,v) {     \
int16_t x = 0L;                 \
memcpy(&x,pb,sizeof(int16_t));  \
v = htons(x);                   \
pb += sizeof(int16_t);          \
}

#define BE_TO_LE_U16(pb,v) {    \
uint16_t x = 0L;                \
memcpy(&x,pb,sizeof(uint16_t)); \
v = htons(x);                   \
pb += sizeof(uint16_t);         \
}

#define BE_TO_LE_16_NO_ADVANCE(pb,v) {  \
int16_t x = 0L;                         \
memcpy(&x,pb,sizeof(int16_t));          \
v = htons(x);                           \
}

#define BE_TO_LE_32(pb,v) {     \
int32_t x = 0L;                 \
memcpy(&x,pb,sizeof(int32_t));  \
v = htonl(x);                   \
pb += sizeof(int32_t);          \
}

#define BE_TO_LE_U32(pb,v) {    \
uint32_t x = 0L;                \
memcpy(&x,pb,sizeof(uint32_t)); \
v = htonl(x);                   \
pb += sizeof(uint32_t);         \
}

#define BE_TO_LE_64(pb,v) {     \
int64_t x = 0L;                 \
memcpy(&x,pb,sizeof(int64_t));  \
v = htonll(x);                  \
pb += sizeof(int64_t);          \
}

    struct Range1 {
      long first;
      long countLeft;
   };


    struct otTableRecord {
        uint8_t tag[4];     // tableTag Table identifier.
        uint32_t checksum;  // Checksum for this table.
        uint32_t offset;    // Offset from beginning of font file.
        uint32_t length;    // Length of this table.

        void load(BYTE *pb) {
            memcpy(tag,pb,4 * sizeof(uint8_t));
            pb += 4 * sizeof(uint8_t);
            BE_TO_LE_U32(pb,checksum)
            BE_TO_LE_U32(pb,offset)
            BE_TO_LE_U32(pb,length)
        }
    };


    struct otTableDirectory {

        uint32_t  sfntVersion{0};     // 0x00010000 or 0x4F54544F ('OTTO') — see below.
        uint16_t  numTables{0};       // Number of tables.
        uint16_t  searchRange{0};     // Maximum power of 2 less than or equal to numTables, times 16 ((2**floor(log2(numTables))) * 16, where “**” is an exponentiation operator).
        uint16_t  entrySelector{0};   // Log2 of the maximum power of 2 less than or equal to numTables (log2(searchRange/16), which is equal to floor(log2(numTables))).
        uint16_t  rangeShift{0};      // numTables times 16, minus searchRange ((numTables * 16) - searchRange).
        //otTableRecord *pTableRecords{NULL};
        otTableRecord pTableRecords[32];//{NULL};
        
        BYTE *pTableDirectoryRoot{NULL};
        boolean isCFFFont{false};

        BYTE *table(const char *pszTableName) {
            for ( long k = 0; k < numTables; k++ ) 
                if ( 0 == strncmp((char *)pTableRecords[k].tag,pszTableName,(long)strlen(pszTableName)) ) 
                    return pTableDirectoryRoot + pTableRecords[k].offset;
            return NULL;
        }

        ~otTableDirectory() {
            //if ( ! ( NULL == pTableRecords ) )
            //   delete [] pTableRecords;
            return;
        }

        void load(BYTE *pb) {

            pTableDirectoryRoot = pb;

            BE_TO_LE_U32(pb,sfntVersion)
            isCFFFont = ( sfntVersion == 0x4F54544F );
            BE_TO_LE_U16(pb,numTables)
            BE_TO_LE_U16(pb,searchRange)
            BE_TO_LE_U16(pb,entrySelector)
            BE_TO_LE_U16(pb,rangeShift)

            for ( long k = 0; k < numTables; k++ ) {
                pTableRecords[k].load(pb);
                pb += sizeof(otTableRecord);
            }
        }
    };


    struct otMaxProfileTable {
        otMaxProfileTable(BYTE *pb) {
            BE_TO_LE_U32(pb,Version16Dot16)
            BE_TO_LE_U16(pb,numGlyphs)
            if ( ! ( 0x00010000 == Version16Dot16 ) )
                return;
            BE_TO_LE_U16(pb,maxPoints)
            BE_TO_LE_U16(pb,maxContours)
            BE_TO_LE_U16(pb,maxCompositePoints)
            BE_TO_LE_U16(pb,maxCompositeContours)
            BE_TO_LE_U16(pb,maxZones)
            BE_TO_LE_U16(pb,maxTwilightPoints)
            BE_TO_LE_U16(pb,maxStorage)
            BE_TO_LE_U16(pb,maxFunctionDefs)
            BE_TO_LE_U16(pb,maxInstructionDefs)
            BE_TO_LE_U16(pb,maxStackElements)
            BE_TO_LE_U16(pb,maxSizeOfInstructions)
            BE_TO_LE_U16(pb,maxComponentElements)
            BE_TO_LE_U16(pb,maxComponentDepth)
        }
        uint32_t Version16Dot16;           // version 0x00010000 for version 1.0.
        uint16_t numGlyphs{0};             // The number of glyphs in the font.
        uint16_t maxPoints{0};             // Maximum points in a non-composite glyph.
        uint16_t maxContours{0};           // Maximum contours in a non-composite glyph.
        uint16_t maxCompositePoints{0};    // Maximum points in a composite glyph.
        uint16_t maxCompositeContours{0};  // Maximum contours in a composite glyph.
        uint16_t maxZones{0};              // 1 if instructions do not use the twilight zone (Z0), or 2 if instructions do use Z0; should be set to 2 in most cases.
        uint16_t maxTwilightPoints{0};     // Maximum points used in Z0.
        uint16_t maxStorage{0};            // Number of Storage Area locations.
        uint16_t maxFunctionDefs{0};       // Number of FDEFs, equal to the highest function number + 1.
        uint16_t maxInstructionDefs{0};    // Number of IDEFs.
        uint16_t maxStackElements{0};      // Maximum stack depth across Font Program ('fpgm' table), CVT Program ('prep' table) and all glyph instructions (in the 'glyf' table).
        uint16_t maxSizeOfInstructions{0}; // Maximum byte count for glyph instructions.
        uint16_t maxComponentElements{0};  // Maximum number of components referenced at “top level” for any composite glyph.
        uint16_t maxComponentDepth{0};     // Maximum levels of recursion; 1 for simple components.
    };


    struct otHeadTable {
        otHeadTable(BYTE *pb) {
            BE_TO_LE_16(pb,majorVersion)
            BE_TO_LE_16(pb,minorVersion)
            BE_TO_LE_32(pb,fontRevision)
            BE_TO_LE_32(pb,checksumAdjustment)
            BE_TO_LE_32(pb,magicNumber)
            BE_TO_LE_16(pb,flags)
            BE_TO_LE_16(pb,unitsPerEm)
            BE_TO_LE_64(pb,created)
            BE_TO_LE_64(pb,modified)
            BE_TO_LE_16(pb,xMin)
            BE_TO_LE_16(pb,yMin)
            BE_TO_LE_16(pb,xMax)
            BE_TO_LE_16(pb,yMax)
            BE_TO_LE_16(pb,macStyle)
            BE_TO_LE_16(pb,lowestRecPPEM)
            BE_TO_LE_16(pb,fontDirectionHint)
            BE_TO_LE_16(pb,indexToLocFormat)
            BE_TO_LE_16(pb,glyphDataFormat)
        }
        uint16_t majorVersion;          //Major version number of the font header table — set to 1.
        uint16_t minorVersion;          //Minor version number of the font header table — set to 0.
        int32_t fontRevision;           //Fixed   fontRevision   Set by font manufacturer.
        uint32_t checksumAdjustment;    // To compute: set it to 0, sum the entire font as uint32, then store 0xB1B0AFBA - sum. If the font is used as a component in a font collection file, the value of this field will be invalidated by changes to the file structure and font table directory, and must be ignored.
        uint32_t magicNumber;           // Set to 0x5F0F3CF5.
        uint16_t flags;                 // 
        /*
        Bit 0: Baseline for font at y=0.
        Bit 1: Left sidebearing point at x=0 (relevant only for TrueType rasterizers) — see the note
                below regarding variable fonts.
        Bit 2: Instructions may depend on point size.
        Bit 3: Force ppem to integer values for all internal scaler math; may use fractional ppem 
                sizes if this bit is clear. It is strongly recommended that this be set in hinted fonts.
        Bit 4: Instructions may alter advance width (the advance widths might not scale linearly).
        Bit 5: This bit is not used in OpenType, and should not be set in order to ensure compatible 
                behavior on all platforms. If set, it may result in different behavior for vertical 
                layout in some platforms. (See Apple’s specification for details regarding behavior in Apple platforms.)
        Bits 6–10: These bits are not used in Opentype and should always be cleared. 
                (See Apple’s specification for details regarding legacy used in Apple platforms.)
        Bit 11: Font data is “lossless” as a result of having been subjected to optimizing 
                transformation and/or compression (such as e.g. compression mechanisms defined 
                by ISO/IEC 14496-18, MicroType Express, WOFF 2.0 or similar) where the original 
                font functionality and features are retained but the binary compatibility between 
                input and output font files is not guaranteed. As a result of the applied transform,
                the DSIG table may also be invalidated.
        Bit 12: Font converted (produce compatible metrics).
        Bit 13: Font optimized for ClearType™. Note, fonts that rely on embedded bitmaps (EBDT) 
                for rendering should not be considered optimized for ClearType, and therefore 
                should keep this bit cleared.
        Bit 14: Last Resort font. If set, indicates that the glyphs encoded in the 'cmap' subtables 
                are simply generic symbolic representations of code point ranges and don’t truly 
                represent support for those code points. If unset, indicates that the glyphs 
                encoded in the 'cmap' subtables represent proper support for those code points.
        Bit 15: Reserved, set to 0.
        */
        uint16_t unitsPerEm;    // Set to a value from 16 to 16384. Any value in this range is valid. In fonts that have TrueType outlines, a power of 2 is recommended as this allows performance optimizations in some rasterizers.
        int64_t created;        // LONGDATETIME   created   Number of seconds since 12:00 midnight that started January 1st 1904 in GMT/UTC time zone.
        int64_t modified;       // LONGDATETIME   modified  Number of seconds since 12:00 midnight that started January 1st 1904 in GMT/UTC time zone.
        int16_t xMin;           // Minimum x coordinate across all glyph bounding boxes.
        int16_t yMin;           // Minimum y coordinate across all glyph bounding boxes.
        int16_t xMax;           // Maximum x coordinate across all glyph bounding boxes.
        int16_t yMax;           // Maximum y coordinate across all glyph bounding boxes.
        uint16_t macStyle;      // 
        /*
        Bit 0: Bold (if set to 1)
        Bit 1: Italic (if set to 1)
        Bit 2: Underline (if set to 1)
        Bit 3: Outline (if set to 1)
        Bit 4: Shadow (if set to 1)
        Bit 5: Condensed (if set to 1)
        Bit 6: Extended (if set to 1)
        Bits 7–15: Reserved (set to 0).
        */

        uint16_t lowestRecPPEM;     // Smallest readable size in pixels.
        int16_t fontDirectionHint;  // Deprecated (Set to 2).
        /*
        0: Fully mixed directional glyphs;
        1: Only strongly left to right;
        2: Like 1 but also contains neutrals;
        -1: Only strongly right to left;
        -2: Like -1 but also contains neutrals.
        (A neutral character has no inherent directionality; it is not a character with zero (0) width. Spaces and punctuation are examples of neutral characters. Non-neutral characters are those with inherent directionality. For example, Roman letters (left-to-right) and Arabic letters (right-to-left) have directionality. In a “normal” Roman font where spaces and punctuation are present, the font direction hints should be set to two (2).)
        */

        int16_t indexToLocFormat;   // 0 for short offsets (Offset16), 1 for long (Offset32).
        int16_t glyphDataFormat;    // 0 for current format.

    };


    struct otGlyphHeader {
        int16_t contourCount;   // If the number of contours is greater than or equal to zero, this is a simple glyph. If negative, this is a composite glyph — the value -1 should be used for composite glyphs.
        int16_t xMin;           // Minimum x for coordinate data.
        int16_t yMin;           // Minimum y for coordinate data.
        int16_t xMax;           // Maximum x for coordinate data.
        int16_t yMax;
        void load(BYTE *pb) {
            BE_TO_LE_16(pb,contourCount)
            BE_TO_LE_16(pb,xMin)
            BE_TO_LE_16(pb,yMin)
            BE_TO_LE_16(pb,xMax)
            BE_TO_LE_16(pb,yMax)
        }
    };


    struct otHorizHeadTable {
        otHorizHeadTable(BYTE *pb) {
            BE_TO_LE_16(pb,majorVersion)
            BE_TO_LE_16(pb,minorVersion)
            BE_TO_LE_16(pb,ascender)
            BE_TO_LE_16(pb,descender)
            BE_TO_LE_16(pb,lineGap)
            BE_TO_LE_16(pb,advanceWidthMax)
            BE_TO_LE_16(pb,minLeftSideBearing)
            BE_TO_LE_16(pb,minRightSideBearing)
            BE_TO_LE_16(pb,xMaxExtent)
            BE_TO_LE_16(pb,caretSlopeRise)
            BE_TO_LE_16(pb,caretSlopeRun)
            BE_TO_LE_16(pb,caretOffset)
            BE_TO_LE_16(pb,reserved)
            BE_TO_LE_16(pb,reserved2)
            BE_TO_LE_16(pb,reserved3)
            BE_TO_LE_16(pb,resrved4)
            BE_TO_LE_16(pb,metricDataFormat)
            BE_TO_LE_16(pb,numberOfHMetrics)
        }
        uint16_t majorVersion;      // Major version number of the horizontal header table — set to 1.
        uint16_t minorVersion;      // Minor version number of the horizontal header table — set to 0.
        int16_t ascender;           // FWORD Typographic ascent—see note below.
        int16_t descender;          // FWORD Typographic descent—see note below.
        int16_t lineGap;            // FWORD Typographic line gap.
                                    // Negative LineGap values are treated as zero in some legacy platform implementations.
        uint16_t advanceWidthMax;   // UFWORD Maximum advance width value in 'hmtx' table.
        int16_t minLeftSideBearing; // FWORD Minimum left sidebearing value in 'hmtx' table for glyphs with contours (empty glyphs should be ignored).
        int16_t minRightSideBearing;// FWORD Minimum right sidebearing value; calculated as min(aw - (lsb + xMax - xMin)) for glyphs with contours (empty glyphs should be ignored).
        int16_t xMaxExtent;         // FWORD Max(lsb + (xMax - xMin)).
        int16_t caretSlopeRise;     // Used to calculate the slope of the cursor (rise/run); 1 for vertical.
        int16_t caretSlopeRun;      // 0 for vertical.
        int16_t caretOffset;        // The amount by which a slanted highlight on a glyph needs to be shifted to produce the best appearance. Set to 0 for non-slanted fonts
        int16_t reserved;           // (reserved) set to 0
        int16_t reserved2;          // (reserved) set to 0
        int16_t reserved3;          // (reserved) set to 0
        int16_t resrved4;           // (reserved) set to 0
        int16_t metricDataFormat;   //  0 for current format.
        uint16_t numberOfHMetrics;  // Number of hMetric entries in 'hmtx' table
    };


    struct otVertHeadTable {
        otVertHeadTable(BYTE *pb) {
            BE_TO_LE_32(pb,Version16Dot16)
            BE_TO_LE_16(pb,int16_t vertTypoAscender)
            BE_TO_LE_16(pb,vertTypoDescender)
            BE_TO_LE_16(pb,vertTypoLineGap)
            BE_TO_LE_16(pb,advanceHeightMax)
            BE_TO_LE_16(pb,minTopSideBearing)
            BE_TO_LE_16(pb,minBottomSideBearing)
            BE_TO_LE_16(pb,yMaxExtent)
            BE_TO_LE_16(pb,caretSlopeRise)
            BE_TO_LE_16(pb,caretSlopeRun)
            BE_TO_LE_16(pb,caretOffset)
            BE_TO_LE_16(pb,reserved)
            BE_TO_LE_16(pb,reserved1)
            BE_TO_LE_16(pb,reserved2)
            BE_TO_LE_16(pb,reserved3)
            BE_TO_LE_16(pb,metricDataFormat)
            BE_TO_LE_16(pb,numOfLongVerMetrics)
        }
        uint32_t Version16Dot16;        // version Version number of the vertical header table; 0x00011000 for version 1.1
        int16_t vertTypoAscender;       // The vertical typographic ascender for this font. It is the distance in FUnits from the vertical center baseline to the right edge of the design space for CJK / ideographic glyphs (or “ideographic em box”).
                                        // It is usually set to (head.unitsPerEm)/2. For example, a font with an em of 1000 FUnits will set this field to 500. See the Baseline tags section of the OpenType Layout Tag Registry for a description of the ideographic em-box.
        int16_t vertTypoDescender;      // The vertical typographic descender for this font. It is the distance in FUnits from the vertical center baseline to the left edge of the design space for CJK / ideographic glyphs.
                                        // It is usually set to -(head.unitsPerEm/2). For example, a font with an em of 1000 FUnits will set this field to -500.
        int16_t vertTypoLineGap;        // The vertical typographic gap for this font. An application can determine the recommended line spacing for single spaced vertical text for an OpenType font by the following expression: ideo embox width + vhea.vertTypoLineGap
        int16_t advanceHeightMax;       // The maximum advance height measurement -in FUnits found in the font. This value must be consistent with the entries in the vertical metrics table.
        int16_t minTopSideBearing;      // The minimum top sidebearing measurement found in the font, in FUnits. This value must be consistent with the entries in the vertical metrics table.
        int16_t minBottomSideBearing;   // The minimum bottom sidebearing measurement found in the font, in FUnits. This value must be consistent with the entries in the vertical metrics table.
        int16_t yMaxExtent;             // Defined as yMaxExtent = max(tsb + (yMax - yMin)).
        int16_t caretSlopeRise;         // The value of the caretSlopeRise field divided by the value of the caretSlopeRun Field determines the slope of the caret. A value of 0 for the rise and a value of 1 for the run specifies a horizontal caret. A value of 1 for the rise and a value of 0 for the run specifies a vertical caret. Intermediate values are desirable for fonts whose glyphs are oblique or italic. For a vertical font, a horizontal caret is best.
        int16_t caretSlopeRun;          // See the caretSlopeRise field. Value=1 for nonslanted vertical fonts.
        int16_t caretOffset;            // The amount by which the highlight on a slanted glyph needs to be shifted away from the glyph in order to produce the best appearance. Set value equal to 0 for nonslanted fonts.
        int16_t reserved;               // Set to 0.
        int16_t reserved1;              // Set to 0.
        int16_t reserved2;              // Set to 0.
        int16_t reserved3;              // Set to 0.
        int16_t metricDataFormat;       // Set to 0.
        uint16_t numOfLongVerMetrics;   // Number of advance heights in the vertical metrics table.
    };


    class otSimpleGlyph {
    public:

        otSimpleGlyph(BYTE bGlyph,font *pFont,otGlyphHeader *ph,BYTE *pb);

        ~otSimpleGlyph();

        otGlyphHeader *pGlyphHeader{NULL};
        BYTE *pbGlyphData{NULL};

        uint16_t *pEndPtsOfContours{NULL};  // Array of point indices for the last point of each contour, in increasing numeric order.
        uint16_t instructionLength{0L};     // Total number of bytes for instructions. If instructionLength is zero, no instructions are present for this glyph, and this field is followed directly by the flags field.
        uint8_t *pInstructions{NULL};       // Array of instruction byte code for the glyph.
        uint8_t *pFlags{NULL};              // Array of flag elements. See below for details regarding the number of flag array elements.

        POINT *pPoints{NULL};
        POINT **pPointFirst{NULL};

        uint16_t *contourPointCount{NULL};
        int32_t  **pPointFirstX{NULL};
        int32_t **pPointFirstY{NULL};
        uint8_t **pFlagsFirst{NULL};

        int16_t contourCount{0};
        int16_t inputPointCount{0};
        int16_t pointCount{0};

        int32_t advanceWidth{0L};
        int32_t advanceHeight{0L};
        int32_t leftSideBearing{0L};
        int32_t rightSideBearing{0L};

        int32_t boundingBox[4]{0,0,0,0};
        POINT phantomPoints[4];

    };


    struct otLongHorizontalMetric {
        uint16_t advanceWidth;
        int16_t lsb;
        void load(BYTE *pb) {
            BE_TO_LE_U16(pb,advanceWidth)
            BE_TO_LE_16(pb,lsb)
        }
    };


    struct otHorizontalMetricsTable {
        otHorizontalMetricsTable(long cntGlyphs,long cntMetrics,BYTE *pb) {
            pHorizontalMetrics = new otLongHorizontalMetric[cntGlyphs];
            for ( long k = 0; k < cntMetrics; k++ ) {
                pHorizontalMetrics[k].load(pb);
                pb += sizeof(otLongHorizontalMetric);
            }
            if ( cntMetrics == cntGlyphs )
                return;
            for ( long k = 0; k < cntGlyphs - cntMetrics; k++ ) {
                pHorizontalMetrics[cntMetrics + k].advanceWidth = pHorizontalMetrics[cntMetrics - 1].advanceWidth;
                BE_TO_LE_16(pb,pHorizontalMetrics[cntMetrics + k].lsb)
            }
        }
        otLongHorizontalMetric *pHorizontalMetrics{NULL}; // [numberOfHMetrics]   Paired advance width and left side bearing values for each glyph. Records are indexed by glyph ID.
        //int16_t *pLeftSideBearings{NULL};               // [numGlyphs - numberOfHMetrics] Left side bearings for glyph IDs greater than or equal to numberOfHMetrics.
    };


    struct otLongVerticalMetric {
        uint16_t advanceHeight;      // The advance height of the glyph. Unsigned integer in FUnits, in font design units.
        int16_t topSideBearing;
        void load(BYTE *pb) {
            BE_TO_LE_16(pb,advanceHeight)
            BE_TO_LE_16(pb,topSideBearing)
        }
    };


    struct otVerticalMetricsTable {
        otVerticalMetricsTable(long cntGlyphs,long cntMetrics,BYTE *pb) {
            pVerticalMetrics = new otLongVerticalMetric[cntGlyphs];
            for ( long k = 0; k < cntMetrics; k++ ) {
                pVerticalMetrics[k].load(pb);
                pb += sizeof(otLongVerticalMetric);
            }
            if ( cntMetrics == cntGlyphs )
                return;
            for ( long k = 0; k < cntGlyphs - cntMetrics; k++ ) {
                pVerticalMetrics[cntMetrics + k].advanceHeight = pVerticalMetrics[cntMetrics - 1].advanceHeight;
                BE_TO_LE_16(pb,pVerticalMetrics[cntMetrics + k].topSideBearing)
            }
        }
        otLongVerticalMetric *pVerticalMetrics;
    };


    struct otOS2Table {
        otOS2Table(BYTE *pBytes) {
            load(pBytes);
        }
        uint16_t version;           // 0x0005
        uint16_t xAvgCharWidth;
        uint16_t usWeightClass;
        uint16_t usWidthClass;
        uint16_t fsType;
        int16_t ySubscriptXSize;
        int16_t ySubscriptYSize;
        int16_t ySubscriptXOffset;
        int16_t ySubscriptYOffset;
        int16_t ySuperscriptXSize;
        int16_t ySuperscriptYSize;
        int16_t ySuperscriptXOffset;
        int16_t ySuperscriptYOffset;
        int16_t yStrikeoutSize;
        int16_t yStrikeoutPosition;
        int16_t sFamilyClass;
        uint8_t panose[10];
        uint32_t ulUnicodeRange1;   // Bits 0–31
        uint32_t ulUnicodeRange2;   // Bits 32–63
        uint32_t ulUnicodeRange3;   // Bits 64–95
        uint32_t ulUnicodeRange4;   // Bits 96–127
        uint8_t achVendID;
        uint16_t fsSelection;
        uint16_t usFirstCharIndex;
        uint16_t usLastCharIndex;
        int16_t sTypoAscender;
        int16_t sTypoDescender;
        int16_t sTypoLineGap;
        uint16_t usWinAscent;
        uint16_t usWinDescent;
        uint32_t ulCodePageRange1;  // Bits 0–31
        uint32_t ulCodePageRange2;  // Bits 32–63
        int16_t sxHeight;
        int16_t sCapHeight;
        uint16_t usDefaultChar;
        uint16_t usBreakChar;
        uint16_t usMaxContext;
        uint16_t usLowerOpticalPointSize;
        uint16_t usUpperOpticalPointSize;
        void load(BYTE *pb) {
            BE_TO_LE_16(pb,version);
            BE_TO_LE_16(pb,xAvgCharWidth);
            BE_TO_LE_16(pb,usWeightClass);
            BE_TO_LE_16(pb,usWidthClass);
            BE_TO_LE_16(pb,fsType);
            BE_TO_LE_16(pb,ySubscriptXSize);
            BE_TO_LE_16(pb,ySubscriptYSize);
            BE_TO_LE_16(pb,ySubscriptXOffset);
            BE_TO_LE_16(pb,ySubscriptYOffset);
            BE_TO_LE_16(pb,ySuperscriptXSize);
            BE_TO_LE_16(pb,ySuperscriptYSize);
            BE_TO_LE_16(pb,ySuperscriptXOffset);
            BE_TO_LE_16(pb,ySuperscriptYOffset);
            BE_TO_LE_16(pb,yStrikeoutSize);
            BE_TO_LE_16(pb,yStrikeoutPosition);
            BE_TO_LE_16(pb,sFamilyClass);
            pb += 10; // panose[10]
            BE_TO_LE_32(pb,ulUnicodeRange1);
            BE_TO_LE_32(pb,ulUnicodeRange2);
            BE_TO_LE_32(pb,ulUnicodeRange3);
            BE_TO_LE_32(pb,ulUnicodeRange4);
            pb++;//uint8_t achVendID;
            BE_TO_LE_16(pb,fsSelection);
            BE_TO_LE_16(pb,usFirstCharIndex);
            BE_TO_LE_16(pb,usLastCharIndex);
            BE_TO_LE_16(pb,sTypoAscender);
            BE_TO_LE_16(pb,sTypoDescender);
            BE_TO_LE_16(pb,sTypoLineGap);
            BE_TO_LE_16(pb,usWinAscent);
            BE_TO_LE_16(pb,usWinDescent);
            BE_TO_LE_32(pb,ulCodePageRange1);
            BE_TO_LE_32(pb,ulCodePageRange2);
            BE_TO_LE_16(pb,sxHeight);
            BE_TO_LE_16(pb,sCapHeight);
            BE_TO_LE_16(pb,usDefaultChar);
            BE_TO_LE_16(pb,usBreakChar);
            BE_TO_LE_16(pb,usMaxContext);
            BE_TO_LE_16(pb,usLowerOpticalPointSize);
            BE_TO_LE_16(pb,usUpperOpticalPointSize);
        }
    };


    struct otCmapTable {
        otCmapTable(BYTE *pbData) {
            BYTE *pbStart = pbData;
            BE_TO_LE_U16(pbData,version)
            BE_TO_LE_U16(pbData,numTables);
            pEncodingRecords = new otEncodingRecord[numTables];
            for ( long k = 0; k < numTables; k++ ) {
                BE_TO_LE_U16(pbData,pEncodingRecords[k].platformID)
                BE_TO_LE_U16(pbData,pEncodingRecords[k].encodingID)
                BE_TO_LE_U32(pbData,pEncodingRecords[k].subtableOffset)
                pEncodingRecords[k].tableAddress = (UINT_PTR)(void *)(pbStart + pEncodingRecords[k].subtableOffset);
            }
        }
        struct otEncodingRecord {
            uint16_t platformID{0};        // Platform ID.
            uint16_t encodingID{0};        // Platform-specific encoding ID.
            uint32_t subtableOffset{0};    // Byte offset from beginning of table to the subtable for this encoding.
            UINT_PTR tableAddress{0};
        };
        uint16_t version{0};        // Table version number (0).
        uint16_t numTables{0};      // Number of encoding tables that follow.
        otEncodingRecord  *pEncodingRecords{NULL};  //[numTables]
    };


    struct otCmapSubtableFormat4 {
        otCmapSubtableFormat4(BYTE **ppbData) {
            load(ppbData);
        }
        uint16_t format{0};        // Format number is set to ?
        uint16_t length{0};        // This is the length in bytes of the subtable.
        uint16_t language{0};      // For requirements on use of the language field, see “Use of the language field in 'cmap' subtables” in this document.

        uint16_t segCountX2{0};         // 2 × segCount.
        uint16_t searchRange{0};        // Maximum power of 2 less than or equal to segCount, times 2 ((2**floor(log2(segCount))) * 2, where “**” is an exponentiation operator)
        uint16_t entrySelector{0};      // Log2 of the maximum power of 2 less than or equal to segCount (log2(searchRange/2), which is equal to floor(log2(segCount)))
        uint16_t rangeShift{0};         // segCount times 2, minus searchRange ((segCount * 2) - searchRange)
        uint16_t *pEndCode{NULL};       // [segCount]   End characterCode for each segment, last=0xFFFF.
        uint16_t reservedPad{0};        // Set to 0.
        uint16_t *pStartCode{NULL};     // [segCount]   Start character code for each segment.
        uint16_t *pIdDelta{NULL};       // [segCount]   Delta for all character codes in segment.
        uint16_t *pIdRangeOffsets{NULL};// [segCount]   Offsets into glyphIdArray or 0
        uint16_t *pGlyphIdArray{NULL};  // [ ]

BYTE *pbStart{NULL};

        void load(BYTE **ppbData) {
pbStart = *ppbData;
            BE_TO_LE_U16(*ppbData,format)
            BE_TO_LE_U16(*ppbData,length);
            BE_TO_LE_U16(*ppbData,language);
        }

        void loadFormat4(BYTE **ppbData) {

            BE_TO_LE_U16(*ppbData,segCountX2)
            BE_TO_LE_U16(*ppbData,searchRange)
            BE_TO_LE_U16(*ppbData,entrySelector)
            BE_TO_LE_U16(*ppbData,rangeShift)

            uint16_t segCount = segCountX2 / 2;

            BYTE *pEndOfParams = *ppbData + 2 * (4 * segCount + 1);

            long cntIdValues = (length - (long)(pEndOfParams - pbStart)) / 2;

            uint16_t *pData = new uint16_t[4 * segCount + 1 + cntIdValues];

            pEndCode = pData;
            for ( long k = 0; k < segCount; k++ )
                BE_TO_LE_U16(*ppbData,pEndCode[k])

            BE_TO_LE_U16(*ppbData,reservedPad)

            pStartCode = pEndCode + segCount + 1;
            for ( long k = 0; k < segCount; k++ )
                BE_TO_LE_U16(*ppbData,pStartCode[k])

            pIdDelta = pStartCode + segCount;
            for ( long k = 0; k < segCount; k++ )
                BE_TO_LE_U16(*ppbData,pIdDelta[k])

            pIdRangeOffsets = pIdDelta + segCount;
            for ( long k = 0; k < segCount; k++ )
                BE_TO_LE_U16(*ppbData,pIdRangeOffsets[k])

            pGlyphIdArray = pIdRangeOffsets + segCount;
            for ( long k = 0; k < cntIdValues; k++ )
                BE_TO_LE_U16(*ppbData,pGlyphIdArray[k])

        }
    };


    struct otPostTable {
        otPostTable(BYTE *pbData) {
            BE_TO_LE_U32(pbData,Version16Dot16)
            BE_TO_LE_32(pbData,italicAngle)
            BE_TO_LE_16(pbData,underlinePosition)
            BE_TO_LE_16(pbData,underlineThickness)
            BE_TO_LE_U32(pbData,isFixedPitch)
            BE_TO_LE_U32(pbData,minMemType42)
            BE_TO_LE_U32(pbData,maxMemType42)
            BE_TO_LE_U32(pbData,minMemType1)
            BE_TO_LE_U32(pbData,maxMemType1)
        }
        uint32_t Version16Dot16{0};    // version 0x00010000 for version 1.0 0x00020000 for version 2.0 0x00025000 for version 2.5 (deprecated) 0x00030000 for version 3.0
        int32_t italicAngle{0};        // Italic angle in counter-clockwise degrees from the vertical. Zero for upright text, negative for text that leans to the right (forward).
        int16_t underlinePosition{0};  // Suggested y-coordinate of the top of the underline.
        int16_t underlineThickness{0}; // Suggested values for the underline thickness. In general, the underline thickness should match the thickness of the underscore character (U+005F LOW LINE), and should also match the strikeout thickness, which is specified in the OS/2 table.
        uint32_t isFixedPitch{0};      // Set to 0 if the font is proportionally spaced, non-zero if the font is not proportionally spaced (i.e. monospaced).
        uint32_t minMemType42{0};      // Minimum memory usage when an OpenType font is downloaded.
        uint32_t maxMemType42{0};      // Maximum memory usage when an OpenType font is downloaded.
        uint32_t minMemType1{0};       // Minimum memory usage when an OpenType font is downloaded as a Type 1 font.
        uint32_t maxMemType1{0};       // Maximum memory usage when an OpenType font is downloaded as a Type 1 font.
    };

    // TrueType fonts used in the Windows or Macintosh environments will
    // generally use the encoding specific to that system, such as ANSI for Windows ...
    // ... If there is no post table in a TrueType font in the Windows environment,
    // the Windows ANSI encoded can be assumed.
#if 0
    struct otPostTableHeader {
        uint16_t version;           // Version16Dot16. version 0x00010000 for version 1.0  0x00020000 for version 2.0 0x00025000 for version 2.5 (deprecated) 0x00030000 for version 3.0
        Fixed italicAngle;          // Italic angle in counter-clockwise degrees from the vertical. Zero for upright text, negative for text that leans to the right (forward).
        FWORD underlinePosition;    // Suggested y-coordinate of the top of the underline.
        FWORD underlineThickness;   // Suggested values for the underline thickness. In general, the underline thickness should match the thickness of the underscore character (U+005F LOW LINE), and should also match the strikeout thickness, which is specified in the OS/2 table.
        uint32 isFixedPitch;        // Set to 0 if the font is proportionally spaced, non-zero if the font is not proportionally spaced (i.e. monospaced).
        uint32 minMemType42;        // Minimum memory usage when an OpenType font is downloaded.
        uint32 maxMemType42;        // Maximum memory usage when an OpenType font is downloaded.
        uint32 minMemType1;         // Minimum memory usage when an OpenType font is downloaded as a Type 1 font.
        uint32 maxMemType1;         // Maximum memory usage when an OpenType font is downloaded as a Type 1 font.
    };
#endif

    struct type3GlyphBoundingBox {
        type3GlyphBoundingBox() {
            memset(this,0,sizeof(type3GlyphBoundingBox));
        }
        GS_POINT lowerLeft;
        GS_POINT upperRight;
        GS_POINT advance;
    };

    class font : public IFont_EVNSW {
    public:

        // IUnknown

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
        STDMETHOD_ (ULONG, AddRef)();
        STDMETHOD_ (ULONG, Release)();

        // IFont_EVNSW

        STDMETHOD(get_Matrix)(LPVOID pResult);
        STDMETHOD(put_Matrix)(UINT_PTR pMatrix);
        STDMETHOD(Scale)(FLOAT scaleX,FLOAT scaleY);
        STDMETHOD(Translate)(FLOAT translateX,FLOAT translateY);
        STDMETHOD(ConcatMatrix)(/*XFORM*/ UINT_PTR pXForm);
        STDMETHOD(FontName)(long cbName,char *szFontName);
        STDMETHOD(get_FontCookie)(UINT_PTR *ppCookie);
        STDMETHOD(get_FontType)(int *pFontType);
        STDMETHOD(get_GlyphId)(unsigned char charCode,int *pGlyphId);
        STDMETHOD(SaveState)();
        STDMETHOD(RestoreState)();

        font(char *fontName);
        font(font &);

        ~font();

    private:

        void load(long countGlyphs);
        void type3load();

        HRESULT type42Load(BYTE *pbData = NULL);

        char *fontName();

        void scaleFont(FLOAT sv);
        void setFontMatrix(matrix *pMatrix);
        void transformGlyph(GS_POINT *pPointD,long countPoints);
        void transformGlyphInPlace(GS_POINT *pPointD,long countPoints);
        void transformGlyph(GS_POINT *pPointD,long countPoints,class matrix *pMatrix);
        void transformGlyphInPlace(GS_POINT *pPointD,long countPoints,class matrix *pMatrix);

        void concat(matrix *);
        void pushMatrix(matrix *);
        void restoreMatrix();

        long UnitsPerEm() { return pHeadTable -> unitsPerEm; }

        FLOAT PointSize(FLOAT v = -FLT_MAX);

        enum fontType {
            ftype0 = 0,
            ftype1 = 1,
            ftype2 = 2,
            ftype3 = 3,
            ftype9 = 9,
            ftype10 = 10,
            ftype11 = 11,
            ftype14 = 14,
            ftype32 = 32,
            ftype42 = 42,
            ftypeUnspecified = 99
        };

        void SetCIDFont(boolean isCID) { isCIDFont = isCID; }

        long refCount{0};

        BYTE fontDataBegin{0x00};

        otTableDirectory tableDirectory;

        BYTE *pbFontData{NULL};

        BYTE *pGlyfTable{NULL};
        BYTE *pLocaTable{NULL};

        otMaxProfileTable *pMaxProfileTable{NULL};
        otHeadTable *pHeadTable{NULL};
        otHorizHeadTable *pHorizHeadTable{NULL};
        otVertHeadTable *pVertHeadTable{NULL};
        otHorizontalMetricsTable *pHorizontalMetricsTable{NULL};
        otVerticalMetricsTable *pVerticalMetricsTable{NULL};
        otOS2Table *pOS2Table{NULL};
        otPostTable *pPostTable{NULL};

        BYTE *pSfntsTable{NULL};

        long countGlyphs{0};

        char szFamily[64]{'\0'};
        wchar_t szwFamily[64]{L'0'};

        enum fontType fontType{fontType::ftypeUnspecified};

        FLOAT scaleFUnitsToPoints{0.0f};

        type3GlyphBoundingBox type3GlyphBoundingBox;

        long charStringType{2L};

        boolean isCIDFont{false};
        boolean isLoaded{false};

        BYTE fontDataEnd{0x00};

        DWORD cbFontData{0L};

        long dupCount{0L};
        UINT_PTR cookie{NULL};

        std::map<uint16_t,uint16_t> glyphIDMap;

        std::stack<matrix *> matrixStack;
        FLOAT pointSize{1.0f};
        FLOAT currentScale{1.0f};

        static char szFailureMessage[1024];

        friend class FontManager;
        friend class otSimpleGlyph;

    };

