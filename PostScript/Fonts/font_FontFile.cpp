
#include "job.h"

#include "Fonts\font.h"

   void font::FontFile(PdfDictionary *pDescriptorDictionary) {

   if ( ! pDescriptorDictionary ) 
      return;

   if ( pDescriptorDictionary -> Object("FontFile1") ) {

   } else {

      if ( pDescriptorDictionary -> Object("FontFile2") ) {

      } else {

         if ( pDescriptorDictionary -> Object("FontFile3") ) {
            FontFile3(pDescriptorDictionary -> Object("FontFile3"));
         }

      }

   }

   return;
   }


   void font::FontFile3(PdfObject *pFontFile) {

   PdfStream *pStream = pFontFile -> Stream();
   pStream -> Decompress();
   long n = pStream -> BinaryDataSize();

   pFontData = new BYTE[n];
   memcpy(pFontData,pStream -> Storage(),n);

   long versionMajor = (long)pFontData[0];
   long versionMinor = (long)pFontData[1];
   long headerSize = (long)pFontData[2];
   long offsetByteCount = (long)pFontData[3];

   pNameIndex = new INDEX(pFontData + headerSize);

   pTopDictIndex = new INDEX(pNameIndex -> pEnd);

   BYTE *p = pTopDictIndex -> data;

   BYTE *pEnd = p + pTopDictIndex -> offsets[1];

   long charsetOffset = -LONG_MAX;
   long encodingOffset = -LONG_MAX;
   long charStringsOffset = -LONG_MAX;

   while ( p < pEnd ) {

      long v = -LONG_MAX;

      long b0 = (long)p[0];
      long b1 = (long)p[1];
      long b2 = (long)p[2];
      long b3 = (long)p[3];
      long b4 = (long)p[4];

      BYTE *pNext = p + 1;
/*
Operators and operands may be distinguished by inspection of their first 
byte: 0–21 specify operators and 28, 29, 30, and 32–254 specify operands 
(numbers). Byte values 22–27, 31, and 255 are reserved. An operator may 
be preceded by up to a maximum of 48 operands.
*/
      bool isOperand = true;

      if ( 0 <= b0 && b0 <= 21 ) {

         isOperand = false;

         if ( CHARSET_KEY == b0 ) 
            charsetOffset = pJob -> pop() -> IntValue();

         if ( ENCODING_KEY == b0 )
            encodingOffset = pJob -> pop() -> IntValue();

         if ( CHARSTRINGS_KEY == b0 )
            charStringsOffset = pJob -> pop() -> IntValue();

         if ( 12 == b0 ) {

            b0 = p[1];

            if ( CHARSTRINGTYPE_KEY == b0 )
               charStringType = pJob -> pop() -> IntValue();

            pNext++;

         }

      } else if ( 30 == b0 ) {

         char szNumber[32];

         memset(szNumber,0,sizeof(szNumber));

         long k = 0;

         bool endFound = false;

         do {

            p++;

            BYTE nibbles[] = {0xF0,0x0F};

            for ( long j = 0; j < 2; j++ ) {

               BYTE lsn = nibbles[j] & *p;

               if ( 0 == j )
                  lsn = lsn >> 4;

               if ( 0x0F == lsn ) {
                  endFound = true;
                  break;
               }

               long v = (long)lsn;
   
               if ( 0x00 <= v && v <= 0x09 )
                  szNumber[k] = 0x30 + lsn;
               else if ( 0x0a == v )
                  szNumber[k] = '.';
               else if ( 0x0b == v )
                  szNumber[k] = 'E';
               else if ( 0x0c == v ) {
                  szNumber[k] = 'E';
                  szNumber[++k] = '-';
               } else if ( 0x0e == v ) 
                  szNumber[k] = '-';

               k++;

            }

         } while ( ! endFound );

         pNext = p;

         pJob -> push(new (pJob -> CurrentObjectHeap()) object(pJob,szNumber));

      } else if ( 32 <= b0 && b0 <= 246 ) {
         v = b0 - 139;

      } else if ( 247 <= b0 && b0 <= 250 ) {
         v = (b0 - 247) * 256 + b1 + 108;
         pNext++;

      } else if ( 251 <= b0 && b0 <= 254 ) {
         v = -(b0 - 251) * 256 - b1 - 108;
         pNext++;

      } else if ( 28 == b0 ) {
         v = (b1 << 8) | b2;
         pNext += 2;

      } else if ( 29 == b0 ) {
         v = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
         pNext += 4;

      }

      if ( -LONG_MAX != v )
         pJob -> push(new (pJob -> CurrentObjectHeap()) object(pJob,v));

      p = pNext;

   }

   pStringIndex = new INDEX(pTopDictIndex -> pEnd);

   pGlobalSubroutineIndex = new INDEX(pStringIndex -> pEnd);

   if ( ! ( -LONG_MAX == encodingOffset ) ) {

      BYTE *pEncodingData = pFontData + encodingOffset;

      p = pEncodingData;

      codeCount = 0L;
      long rangeCount = 0L;
      pEncoding = NULL;

      if ( 0 == *p ) {

         p++;
         codeCount = (long)p[0];
         pEncodingGlyphs = new long[codeCount + 1];

         p++;
         for ( long k = 0; k < codeCount; k++ ) 
            pEncodingGlyphs[k] = (long)p[k];

         pEncodingGlyphs[codeCount] = pEncodingGlyphs[codeCount - 1];

      } else if ( 1 == *p ) {

         p++;

         rangeCount = (long)p[0];

         Range1 *ranges = new Range1[rangeCount];

         for ( long k = 0; k < rangeCount; k++ ) {
            p++;
            ranges[k].first = (long)p[0];
            ranges[k].countLeft = (long)p[1];
            p++;
         }

      }

   }

   if ( ! ( -1L == charStringsOffset ) ) {
      pCharStringsIndex = new INDEX(pFontData + charStringsOffset);
      pSID = new long[pCharStringsIndex -> objectCount];
   }

   //NTC: 12-06-2022
   //
   // The federal f1040 throws an exception here when charsetOffset is not getting 
   // set and retains it's initial value of -LONG_MAX
   // Setting the value to 0, as in the case of = -1L, just causes a postscript
   // operand stack underflow.
   //
   if ( -1L == charsetOffset ) //|| -LONG_MAX == charsetOffset ) 
      charsetOffset = 0L;

   if ( 0 == charsetOffset ) {

   } else if ( 1 == charsetOffset ) {

   } else if ( 2 == charsetOffset ) {

   } else {

      BYTE *pCharsets = pFontData + charsetOffset;

      p = pCharsets;

      if ( 0 == (long)p[0] ) {
         p++;
         for ( long k = 0; k < pCharStringsIndex -> objectCount; k++ ) {
            u_short f = 0;
            memcpy(&f,p,sizeof(u_short));
            pSID[k] = ntohs(f);
            p += 2;
         }

      } else if ( 1 == (long)p[0] ) {

printf("\nhello world");
      } else if ( 2 == (long)p[0] ) {

printf("\nhello world");
      }

   }

   return;
   }