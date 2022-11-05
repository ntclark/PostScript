// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "PdfEnabler_i.h"
#include "pdfEnabler\Page.h"

class job;

#include "PostScript objects\array.h"
#include "PostScript objects\dictionary.h"

#define CHARSET_KEY 15
#define ENCODING_KEY 16
#define CHARSTRINGS_KEY 17
#define CHARSTRINGTYPE_KEY 6

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

   class font : public dictionary {

   public:

      font(job *pJob,char *fontName);
      font(job *pJob,PdfDictionary *pFontDict);
      ~font();

      void scalefont(float sv) { scale = sv; };

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

      char *translateText(char *);

   private:

      void FontFile(PdfDictionary *pDescriptorDictionary);
      void FontFile3(PdfObject *pFontFile);

      char szFamily[64];
      enum fontType fontType;
      float scale;

      job *pJob;

      class array encoding;
      class array *pFontBBox;

      dictionary privateDictionary;
      dictionary charStrings;

      long firstChar,lastChar;
      float missingWidth;
      char *pszCharSet;
      class array *pWidths;
      class array *pDifferencesArray;
      class array *pCharsetArray;

      BYTE *pFontData;
      INDEX *pNameIndex;
      INDEX *pTopDictIndex;
      INDEX *pStringIndex;
      INDEX *pGlobalSubroutineIndex;
      INDEX *pCharStringsIndex;

      long *pSID;
      long *pEncoding;
      long codeCount;

      long charStringType;

      static class array *pStandardEncoding;

      friend class graphicsState;

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