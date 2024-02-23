
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

   pFont = new class font(pJob,pDict,fs);

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

   if ( ! pJob -> GetDC() )
      return;

   HFONT currentFont = (HFONT)GetCurrentObject(pJob -> GetDC(),OBJ_FONT);

   LOGFONT logFont = {0};

   GetObject(currentFont,sizeof(LOGFONT),&logFont);

#if USE_ANISOTROPIC
   logFont.lfHeight = (long)fontSize;
#else
   logFont.lfHeight = -MulDiv((long)fontSize, GetDeviceCaps(pJob -> GetDC(), LOGPIXELSY), 72);
#endif

   DeleteObject(currentFont);

   HFONT newFont = CreateFontIndirect(&logFont);

   oldFont = SelectObject(pJob -> GetDC(),newFont);

   return;
   }

