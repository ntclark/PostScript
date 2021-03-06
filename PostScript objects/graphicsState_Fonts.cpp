// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PostScript objects\graphicsState.h"
#include "job.h"

#include "Fonts\font.h"

   void graphicsState::setFont(char *pszFontDictName,float fs) {

#ifdef NDEBUG
   setFontSize(fs);
   return;
#endif

   PdfDictionary *pDict = pJob -> pPdfPage -> findResource(pszFontDictName,"Font");

   if ( ! pDict ) {
      setFontSize(fs);
      return;
   }

   pFont = new class font(pJob,pDict);

   setFontSize(fs);

   return;
   }


   void graphicsState::setFontSize(float v) {
//
//NTC: 12-17-2011
// I am getting a value of 1 for font size from the TF operator, the description of which indicates that the argument is
// not a size but a scale factor.
// I suspect that there is a "default" font size somewhere - in the document, page resources, or graphic state,
// and that with the Print Driver generated PDF documents, this default size is 1, and therefore, any TF operator
// will be sending a true size. 
//
   fontSize = v;
//
// as an interim solution to this - I will set the font size to 10, if it is currently 1
//
   if ( 1.0 == fontSize )
      fontSize = 10.0;

   if ( ! hdcTarget )
      return;

   HFONT currentFont = (HFONT)SelectObject(hdcTarget,oldFont);

   LOGFONT logFont = {0};

   GetObject(currentFont,sizeof(LOGFONT),&logFont);

   logFont.lfHeight = -MulDiv((long)fontSize, GetDeviceCaps(hdcTarget, LOGPIXELSY), 72);

   DeleteObject(currentFont);

   HFONT newFont = CreateFontIndirect(&logFont);

   oldFont = SelectObject(hdcTarget,newFont);

   return;
   }

