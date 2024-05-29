
#pragma once

#include "PdfEnabler_i.h"
#include "pdfEnabler\Page.h"

class job;
class font;

#include "PostScript objects\array.h"
#include "PostScript objects\dictionary.h"
#include "PostScript objects\graphicsState.h"

#define DEFAULT_POINT_SIZE  12

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
( ( ((l) >> 56) & 0x00000000000000FFLL ) |      \
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

#define BE_TO_LE_64(pb,v) {     \
int64_t x = 0L;                 \
memcpy(&x,pb,sizeof(int64_t));  \
v = htonll(x);                  \
pb += sizeof(int64_t);          \
}

struct INDEX {

    INDEX(BYTE *p) { 

        memset(this,0,sizeof(INDEX));

        u_short f = 0L;
        memcpy(&f,p,sizeof(u_short));
        objectCount = ntohs(f);
        p += sizeof(u_short);
        offsetByteCount = (long)p[0];
        p++;
        offsets = new long[objectCount + 2];
        memset(offsets,0,(objectCount + 2) * sizeof(long));
        for ( long k = 0; k <= objectCount; k++ ) {
            if ( 2 == offsetByteCount ) {
                memcpy(&f,p,sizeof(u_short));
                offsets[k] = ntohs(f);
            } else
                offsets[k] = (long)*p;
            p += offsetByteCount;
        }
        offsets[objectCount + 1] = offsets[objectCount];
        p -= offsetByteCount;
        data = p + 1;
        pEnd = data + offsets[objectCount] - 1;
        dataSize = (long)(pEnd - data);
        return;
    };

    short objectCount;
    long offsetByteCount;
    long *offsets;
    long dataSize;
    BYTE *data;
    BYTE *pEnd;

    };

   struct Range1 {
      long first;
      long countLeft;
   };


    struct otTableRecord {
        uint8_t tag[4];     // tableTag Table identifier.
        uint32_t checksum;  //Checksum for this table.
        uint32_t offset;    // Offset from beginning of font file.
        uint32_t length;    //Length of this table.

        void load(BYTE *pb) {
            memcpy(tag,pb,4 * sizeof(uint8_t));
            pb += 4 * sizeof(uint8_t);
            BE_TO_LE_32(pb,checksum)
            BE_TO_LE_32(pb,offset)
            BE_TO_LE_32(pb,length)
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

        BYTE *table(char *pszTableName) {
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

            BE_TO_LE_32(pb,sfntVersion)
            BE_TO_LE_16(pb,numTables)
            BE_TO_LE_16(pb,searchRange)
            BE_TO_LE_16(pb,entrySelector)
            BE_TO_LE_16(pb,rangeShift)

            for ( long k = 0; k < numTables; k++ ) {
                pTableRecords[k].load(pb);
                pb += sizeof(otTableRecord);
            }
        }
    };


    struct otHeadTable {

        uint16_t majorVersion;          //Major version number of the font header table — set to 1.
        uint16_t minorVersion;          //Minor version number of the font header table — set to 0.
        int32_t fontRevision;           //Fixed   fontRevision   Set by font manufacturer.
        uint32_t checksumAdjustment;    // To compute: set it to 0, sum the entire font as uint32, then store 0xB1B0AFBA - sum. If the font is used as a component in a font collection file, the value of this field will be invalidated by changes to the file structure and font table directory, and must be ignored.
        uint32_t magicNumber;           // Set to 0x5F0F3CF5.
        uint16_t flags;                 // 
/*
Bit 0: Baseline for font at y=0.
Bit 1: Left sidebearing point at x=0 (relevant only for TrueType rasterizers) — see the note below regarding variable fonts.
Bit 2: Instructions may depend on point size.
Bit 3: Force ppem to integer values for all internal scaler math; may use fractional ppem sizes if this bit is clear. It is strongly recommended that this be set in hinted fonts.
Bit 4: Instructions may alter advance width (the advance widths might not scale linearly).
Bit 5: This bit is not used in OpenType, and should not be set in order to ensure compatible behavior on all platforms. If set, it may result in different behavior for vertical layout in some platforms. (See Apple’s specification for details regarding behavior in Apple platforms.)
Bits 6–10: These bits are not used in Opentype and should always be cleared. (See Apple’s specification for details regarding legacy used in Apple platforms.)
Bit 11: Font data is “lossless” as a result of having been subjected to optimizing transformation and/or compression (such as e.g. compression mechanisms defined by ISO/IEC 14496-18, MicroType Express, WOFF 2.0 or similar) where the original font functionality and features are retained but the binary compatibility between input and output font files is not guaranteed. As a result of the applied transform, the DSIG table may also be invalidated.
Bit 12: Font converted (produce compatible metrics).
Bit 13: Font optimized for ClearType™. Note, fonts that rely on embedded bitmaps (EBDT) for rendering should not be considered optimized for ClearType, and therefore should keep this bit cleared.
Bit 14: Last Resort font. If set, indicates that the glyphs encoded in the 'cmap' subtables are simply generic symbolic representations of code point ranges and don’t truly represent support for those code points. If unset, indicates that the glyphs encoded in the 'cmap' subtables represent proper support for those code points.
Bit 15: Reserved, set to 0.
*/
        uint16_t unitsPerEm;            // Set to a value from 16 to 16384. Any value in this range is valid. In fonts that have TrueType outlines, a power of 2 is recommended as this allows performance optimizations in some rasterizers.
        int64_t created;                // LONGDATETIME   created   Number of seconds since 12:00 midnight that started January 1st 1904 in GMT/UTC time zone.
        int64_t modified;               // LONGDATETIME   modified   Number of seconds since 12:00 midnight that started January 1st 1904 in GMT/UTC time zone.
        int16_t xMin;                   // Minimum x coordinate across all glyph bounding boxes.
        int16_t yMin;                   // Minimum y coordinate across all glyph bounding boxes.
        int16_t xMax;                   // Maximum x coordinate across all glyph bounding boxes.
        int16_t yMax;                   // Maximum y coordinate across all glyph bounding boxes.
        uint16_t macStyle;              // 
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

        uint16_t lowestRecPPEM;         // Smallest readable size in pixels.
        int16_t fontDirectionHint;      // Deprecated (Set to 2).
/*
0: Fully mixed directional glyphs;
1: Only strongly left to right;
2: Like 1 but also contains neutrals;
-1: Only strongly right to left;
-2: Like -1 but also contains neutrals.
(A neutral character has no inherent directionality; it is not a character with zero (0) width. Spaces and punctuation are examples of neutral characters. Non-neutral characters are those with inherent directionality. For example, Roman letters (left-to-right) and Arabic letters (right-to-left) have directionality. In a “normal” Roman font where spaces and punctuation are present, the font direction hints should be set to two (2).)
*/

        int16_t indexToLocFormat;       // 0 for short offsets (Offset16), 1 for long (Offset32).
        int16_t glyphDataFormat;        // 0 for current format.

        void load(BYTE *pb) {
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
    };


    struct otGlyphHeader {
        int16_t countourCount;   // If the number of contours is greater than or equal to zero, this is a simple glyph. If negative, this is a composite glyph — the value -1 should be used for composite glyphs.
        int16_t xMin;               // Minimum x for coordinate data.
        int16_t yMin;               // Minimum y for coordinate data.
        int16_t xMax;               // Maximum x for coordinate data.
        int16_t yMax;
        void load(BYTE *pb) {
            BE_TO_LE_16(pb,countourCount)
            BE_TO_LE_16(pb,xMin)
            BE_TO_LE_16(pb,yMin)
            BE_TO_LE_16(pb,xMax)
            BE_TO_LE_16(pb,yMax)
        }
    };


    struct otHorizHeadTable {
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
        void load(BYTE *pb) {
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
    };


    struct otVertHeadTable {

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
        void load(BYTE *pb) {
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

    };


    class otSimpleGlyph {
    public:

        otSimpleGlyph(BYTE bGlyph,font *pFont,graphicsState *pGraphicsState,otGlyphHeader *ph,BYTE *pb,long cbPb);

        ~otSimpleGlyph();

        otGlyphHeader *pGlyphHeader{NULL};
        BYTE *pbGlyphData{NULL};

        uint16_t *pEndPtsOfContours{NULL};  // Array of point indices for the last point of each contour, in increasing numeric order.
        uint16_t instructionLength{0L};     // Total number of bytes for instructions. If instructionLength is zero, no instructions are present for this glyph, and this field is followed directly by the flags field.
        uint8_t *pInstructions{NULL};       // Array of instruction byte code for the glyph.
        uint8_t *pFlags{NULL};              // Array of flag elements. See below for details regarding the number of flag array elements.
        POINT_TYPE *pXCoordinates{NULL};       // Contour point x-coordinates. See below for details regarding the number of coordinate array elements. Coordinate for the first point is relative to (0,0); others are relative to previous point.
        POINT_TYPE *pYCoordinates{NULL};       // Contour point y-coordinates. See below for details regarding the number of coordinate array elements. Coordinate for the first point is relative to (0,0); others are relative to previous point.

        POINTD *pPoints{NULL};
        POINTD **pPointFirst{NULL};

        uint16_t *contourPointCount{NULL};
        POINT_TYPE  **pPointFirstX{NULL};
        POINT_TYPE **pPointFirstY{NULL};
        uint8_t **pFlagsFirst{NULL};

        int16_t contourCount{0};
        int16_t inputPointCount{0};
        int16_t pointCount{0};

        int32_t advanceWidth{0L};
        int32_t advanceHeight{0L};
        int32_t leftSideBearing{0L};
        int32_t rightSideBearing{0L};

        POINT_TYPE boundingBox[4]{0,0,0,0};
        POINTL phantomPoints[4];

    };


    struct otLongHorizontalMetric {
        uint16_t advanceWidth;      // Advance width, in font design units.
        int16_t lsb;
        void load(BYTE *pb) {
            BE_TO_LE_16(pb,advanceWidth)
            BE_TO_LE_16(pb,lsb)
        }
    };


    struct otHorizontalMetricsTable {
        otHorizontalMetricsTable(long cntGlyphs,long cntMetrics,BYTE *pb) {
            pHorizontalMetrics = new otLongHorizontalMetric[cntMetrics];
            pLeftSideBearings = NULL;
            for ( long k = 0; k < cntMetrics; k++ ) {
                pHorizontalMetrics[k].load(pb);
                pb += sizeof(otLongHorizontalMetric);
            }
            if ( cntMetrics > cntGlyphs )
                return;
            pLeftSideBearings = new int16_t[cntGlyphs - cntMetrics];
            for ( long k = 0; k < cntGlyphs - cntMetrics; k++ ) {
                BE_TO_LE_16(pb,pLeftSideBearings[k])
                pb += sizeof(int16_t);
            }
        }
        otLongHorizontalMetric *pHorizontalMetrics; // [numberOfHMetrics]   Paired advance width and left side bearing values for each glyph. Records are indexed by glyph ID.
        int16_t *pLeftSideBearings;                 // [numGlyphs - numberOfHMetrics] Left side bearings for glyph IDs greater than or equal to numberOfHMetrics.
        void load(BYTE *pb) {
        }
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
            pVerticalMetrics = new otLongVerticalMetric[cntMetrics];
            pTopSideBearings = NULL;
            for ( long k = 0; k < cntMetrics; k++ ) {
                pVerticalMetrics[k].load(pb);
                pb += sizeof(otLongVerticalMetric);
            }
            if ( cntMetrics > cntGlyphs )
                return;
            pTopSideBearings = new int16_t[cntGlyphs - cntMetrics];
            for ( long k = 0; k < cntGlyphs - cntMetrics; k++ ) {
                BE_TO_LE_16(pb,pTopSideBearings[k])
                pb += sizeof(int16_t);
            }
        }
        otLongVerticalMetric *pVerticalMetrics;
        int16_t *pTopSideBearings;
        void load(BYTE *pb) {
        }
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


    struct type3GlyphBoundingBox {
        type3GlyphBoundingBox() {
            memset(this,0,sizeof(type3GlyphBoundingBox));
        }
        GS_POINT lowerLeft;
        GS_POINT upperRight;
        GS_POINT advance;
    };

    class font : public dictionary {
    public:

        font(job *pJob,char *fontName);
        font(job *pJob,dictionary *,char *fontName);
        font(job *pJob,PdfDictionary *pFontDict,POINT_TYPE fontSize);
        ~font();

        void load(long glyphCount);
        void type3load();
        void type42Load(BYTE *pbData = NULL);

        char *fontName();

        void scalefont(POINT_TYPE sv);
        void setFontMatrix(class matrix *pMatrix);
        void transformGlyph(POINTD *pPointD,long countPoints);
        void transformGlyphInPlace(POINTD *pPointD,long countPoints);
        void transformGlyph(POINTD *pPointD,long countPoints,class matrix *pMatrix);
        void transformGlyphInPlace(POINTD *pPointD,long countPoints,class matrix *pMatrix);

        class matrix *getMatrix(object *pMatrixName = NULL);
        void putMatrix(class matrix *,object*pMatrixName = NULL);

        void gSave();
        void gRestore();

        long UnitsPerEm() { return pHeadTable -> unitsPerEm; }

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
        ftypeUnspecified = 99 };

        void SetCIDFont(boolean isCID) { isCIDFont = isCID; }

    private:

        otTableDirectory tableDirectory;

        BYTE *pGlyfTable{NULL};
        BYTE *pLocaTable{NULL};
        BYTE *pHmtxTable{NULL};
        BYTE *pVmtxTable{NULL};
        BYTE *pHheaTable{NULL};
        BYTE *pVheaTable{NULL};

        otHeadTable *pHeadTable{NULL};
        otHorizHeadTable *pHorizHeadTable{NULL};
        otVertHeadTable *pVertHeadTable{NULL};
        otHorizontalMetricsTable *pHorizontalMetricsTable{NULL};
        otVerticalMetricsTable *pVerticalMetricsTable{NULL};
        otOS2Table *pOS2Table{NULL};

        class array *pSfntsArray{NULL};
        BYTE *pSfntsTable{NULL};

        uint16_t glyphCount{0};

        char szFamily[64]{'\0'};
        enum fontType fontType{fontType::ftypeUnspecified};

        POINT_TYPE pointSize{DEFAULT_POINT_SIZE};
        POINT_TYPE scaleFUnitsToPoints{0.0f};
        class array *pBoundingBoxArray{NULL};
        type3GlyphBoundingBox type3GlyphBoundingBox;

        long charStringType{2L};

        boolean isCIDFont{false};

        friend class graphicsState;
        friend class otSimpleGlyph;

    };

/*
                  TABLE 5.1 Font types

TYPE     DESCRIPTION

Type 0   (LanguageLevel 2) A composite font—a font composed of other
         fonts, organized hierarchically. See Section 5.10, “Composite
         Fonts.”

Type 1   A font that defines glyph shapes by using a special encoded format.
         Details on this format are provided in a separate book, Adobe Type 1
         Font Format.

         The multiple-master font format is an extension of the Type 1 font
         format that allows the generation of a wide variety of typeface styles
         from a single font. For details of the construction and uses of
         multiple-master fonts, see Adobe Technical Note #5015, Type 1 Font
         Format Supplement.

Type 2   (LanguageLevel 3) A Compact Font Format (CFF) font. See
         Section 5.8.1, “Type 2 and Type 14 Fonts (CFF and Chameleon).”

Type 3   A font that defines glyphs with ordinary PostScript procedures,
         which are the values of entries named BuildGlyph or BuildChar in
         the font dictionary. See Section 5.7, “Type 3 Fonts.”

Types 9, 10, 11, 32 (LanguageLevel 3) 
         These FontType values identify a class of fontlike
         objects, called CIDFonts, that can be used as descendants in CIDkeyed
         fonts. See Section 5.11, “CID-Keyed Fonts.” CIDFonts have
         their own type numbering, specified by a separate CIDFontType
         entry; a Type 0 font and a Type 0 CIDFont are entirely different
         kinds of objects. CIDFonts are not considered to be base fonts.

Type 14 (LanguageLevel 3) 
         A Chameleon font. See Section 5.8.1, “Type 2
         and Type 14 Fonts (CFF and Chameleon).”

Type 42 (LanguageLevel 3) 
         A font based on the TrueType font format. See
         Section 5.8.2, “Type 42 Fonts (TrueType).”

*/


/*

                     TABLE 5.2 Entries common to all font dictionaries

   KEY      TYPE     VALUE

   FontType integer  (Required) The font type; see Table 5.1. Indicates where the glyph descriptions
                     are to be found and how they are represented.

   FontMatrix array  (Required) An array that transforms the glyph coordinate system into the user
                     coordinate system (see Section 5.4, “Glyph Metric Information”). For
                     example, Type 1 font programs from Adobe are usually defined in terms of a
                     1000-unit glyph coordinate system, and their initial font matrix is
                     [0.001 0 0 0.001 0 0]. When a font is derived by the scalefont or makefont
                     operator, the new matrix is concatenated with the existing font matrix to
                     yield a new copy of the font with a different font matrix.

   FontName    name  (Optional) The name of the font. This entry is for information only; it is not
                     used by the PostScript interpreter. Ordinarily, it is the same as the key passed
                     to definefont, but it need not be.

   FontInfo    dictionary (Optional) A dictionary containing font information that is not accessed by
                           the PostScript interpreter; see Table 5.5 on page 327.

   LanguageLevel integer   (Optional) The minimum LanguageLevel required for correct behavior of the
                        font. For example, any font that uses LanguageLevel 2 features for rendering
                        glyphs (such as a glyph description that uses rectfill or glyphshow) should
                        specify a LanguageLevel value of 2. On the other hand, the presence of
                        higher-LanguageLevel information that an interpreter can safely ignore does
                        not require LanguageLevel to be set to the higher LanguageLevel. For
                        example, an XUID entry in the font dictionary—LanguageLevel 2 information
                        that a LanguageLevel 1 interpreter can ignore—does not require setting
                        LanguageLevel to 2. Default value: 1.

   WMode    integer  (Optional; LanguageLevel 2) The writing mode, which determines which of
                     two sets of metrics will be used when glyphs are shown from the font. Mode 0
                     specifies horizontal writing; mode 1 specifies vertical writing (see Section 5.4,
                     “Glyph Metric Information”). LanguageLevel 1 implementations lacking
                     composite font extensions ignore this entry. Default value: 0.

   FID   fontID      (Inserted by definefont) A special object of type fontID that serves internal
                     purposes in the font machinery. The definefont operator inserts this entry. In
                     LanguageLevel 1, an FID entry must not previously exist in the dictionary
                     presented to definefont; the dictionary must have sufficient space to insert
                     this entry.


                     TABLE 5.3 Additional entries common to all base fonts

   KEY      TYPE     VALUE

   Encoding array    (Required) An array of character names to which character codes are
                     mapped. See Section 5.3, “Character Encoding.”

   FontBBox array    (Required) An array of four numbers in the glyph coordinate system giving
                     the left, bottom, right, and top coordinates, respectively, of the font bounding
                     box. The font bounding box is the smallest rectangle enclosing the shape that
                     would result if all of the glyphs of the font were placed with their origins coincident,
                     and then painted. This information is used in making decisions
                     about glyph caching and clipping. If all four values are 0, the PostScript interpreter
                     makes no assumptions based on the font bounding box.

                     If any value is nonzero, it is essential that the font bounding box be accurate;
                     if any glyph’s marks fall outside this bounding box, incorrect behavior may
                     result.

                     In many Type 1 fonts, the FontBBox array is executable, though there is no
                     good reason for this to be so. Programs that access FontBBox should invoke
                     an explicit get or load operator to avoid unintended execution.

   UniqueID integer  (Optional) An integer in the range 0 to 16,777,215 (224 - 1) that uniquely
                     identifies this font. See Section 5.6, “Unique ID Generation.”

   XUID     array    (Optional; LanguageLevel 2) An array of integers that uniquely identifies this
                     font or any variant of it. See Section 5.6, “Unique ID Generation.”

*/   

/*

                     TABLE 5.4 Additional entries specific to Type 1 fonts
   KEY         TYPE        VALUE
   PaintType   integer     (Required) A code indicating how the glyphs of the font are to be painted:
                           0 Glyph outlines are filled.
                           2 Glyph outlines (designed to be filled) are stroked.
                           To get a stroked-outline font, a program can copy the font dictionary, change
                           the PaintType from 0 to 2, add a StrokeWidth entry, and define a new font using
                           this dictionary. Note that the previously documented PaintType values of
                           1 and 3 are not supported.

   StrokeWidth number      (Optional) The stroke width (in units of the glyph coordinate system) for
                           stroked-outline fonts (PaintType 2). This entry is not initially present in
                           filled-outline fonts. It should be added (with a nonzero value) when a stroked
                           font is created from an existing filled font. Default value: 0.

   Metrics     dictionary  (Optional) A dictionary containing metric information (glyph widths and
                           sidebearings) for writing mode 0. This entry is not normally present in the
                           original definition of a font. Adding a Metrics entry to a font overrides the
                           metric information encoded in the glyph descriptions. See Sections 5.4,
                           “Glyph Metric Information,” and 5.9.2, “Changing Glyph Metrics.”
      
   Metrics2    dictionary  (Optional; LanguageLevel 2) Similar to Metrics, but for writing mode 1.
                           CDevProc procedure (Optional; LanguageLevel 2) A procedure that algorithmically derives global
                           changes to the font’s metrics. LanguageLevel 1 implementations lacking
                           composite font extensions ignore this entry.

   CharStrings dictionary  (Required) A dictionary associating character names (the keys in the dictionary)
                           with glyph descriptions. Each entry’s value is ordinarily a string (called a
                           charstring) that represents the glyph description for that character in a special
                           encoded format; see Adobe Type 1 Font Format for details. The value can also
                           be a PostScript procedure; see Section 5.9.3, “Replacing or Adding Individual
                           Glyphs.” This dictionary must have an entry whose key is .notdef.
   
   Private     dictionary (Required) A dictionary containing other internal information about the
                           font. See Adobe Type 1 Font Format for details.

   WeightVector array      (Required; multiple-master fonts only) An array specifying the contribution of
                           each master design to the current font instance. The array contains one number
                           per master design, each typically in the range 0.0 to 1.0; the sum of the array
                           elements must be 1.0. The values in the array are used for calculating the
                           weighted interpolation. Elements outside the allowed range may produce unexpected
                           results.
   
*/