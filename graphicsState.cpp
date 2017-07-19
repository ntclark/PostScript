
#include "graphicsState.h"
#include "font.h"
#include <math.h>
#include "utilities.h"

   char graphicsState::szCurrentText[4096];

   graphicsState::graphicsState(job *pj,HDC dc,RECT *prcWindowsClip) :
      pJob(pj),
      pFont(NULL),
      hdcTarget(dc),
      oldFont(NULL),
      textLeading(0.0f),
      fontSize(1.0f),
      textRise(0.0f),
      characterSpacing(0.0f),
      wordSpacing(0.0f),
      horizontalTextScaling(1.0),
      textRenderingMode(0L),
      writingMode(0L),
      lineCap(0L),
      lineJoin(0L),
      cxPDFPage(0L),
      cyPDFPage(0L),
      pdfRotation(0L),
      matrix(),
      textMatrix(),
      textMatrixIdentity(),
      textMatrixPDF(),
      textMatrixPDFIdentity(),
      textMatrixRotation(),
      textLineMatrixPDF()
   {

   memset(&lastMove,0,sizeof(POINTL));
   memset(&lastPoint,0,sizeof(POINTL));
   memset(&rcTextObjectWindows,0,sizeof(RECT));

   if ( hdcTarget && pJob && pJob -> pPdfPage ) {

      RECT rcPDFPage;
      double rotation = 0.0;

      pJob -> pPdfPage -> getPageSize(&rcPDFPage);

      pJob -> pPdfPage -> getRotation(&rotation);

      pdfRotation = (float)rotation;

      rotation = -rotation * atan(1.0) / 45.0;

      cxPDFPage = rcPDFPage.right - rcPDFPage.left;
      cyPDFPage = rcPDFPage.bottom - rcPDFPage.top;

      a = (float)(prcWindowsClip -> right - prcWindowsClip -> left) / (float)cxPDFPage;
      d = -(float)(prcWindowsClip -> bottom - prcWindowsClip -> top) / (float)cyPDFPage;

      tx = (float)(prcWindowsClip -> left);
      ty = (float)(prcWindowsClip -> bottom);

      SetGraphicsMode(hdcTarget,GM_ADVANCED);
      SetMapMode(hdcTarget,MM_ANISOTROPIC);
      SetWorldTransform(hdcTarget,pTransform());

      textMatrixRotation.a = (float)cos(rotation);
      textMatrixRotation.b = (float)sin(rotation);
      textMatrixRotation.c = -textMatrixRotation.b;
      textMatrixRotation.d = textMatrixRotation.a;

      textMatrixIdentity = *static_cast<matrix *>(this);

      textMatrix = textMatrixIdentity;

      oldFont = SelectObject(hdcTarget,GetStockObject(DEFAULT_GUI_FONT));

   }

   return;
   }

   graphicsState::graphicsState(graphicsState &copy) : matrix(copy) {
   memcpy(this,&copy,sizeof(graphicsState));
   return;
   }

   graphicsState::~graphicsState() {
   if ( pFont )
      delete pFont;
   return;
   }

   void graphicsState::setTextMatrix(float *pValues) {

   sendText();

   textMatrix = textMatrixIdentity;
   textMatrixPDF = textMatrixPDFIdentity;

   textMatrix.concat(pValues);
   textMatrixPDF.concat(pValues);

   textLineMatrix = textMatrix;
   textLineMatrixPDF = textMatrixPDF;

   return;
   }

   void graphicsState::translateTextMatrix(float ptx,float pty) {
   float pValues[] = {1.0, 0.0, 0.0, 1.0, ptx, pty};
   textMatrix.concat(pValues);
   textMatrixPDF.concat(pValues);
   return;
   }

   void graphicsState::translateTextMatrixTJ(float ptx,float pty) {
   float pValues[] = {1.0, 0.0, 0.0, 1.0, -ptx * fontSize / 1000.0f, pty};
   textMatrix.concat(pValues);
   textMatrixPDF.concat(pValues);
   if ( 0.0f > ptx && 0.0f == pty ) {
      RECT rcText;
      float deltaX = measureText(" ",&rcText);
      if ( 0.0f == deltaX || 0.0f > deltaX )
         return;
      long count = (long)(pValues[4] / deltaX);
      if ( 0 == count && ( abs(pValues[4]) > 0.60 * abs(deltaX) ) )
         count = 1L;
      for ( long k = 0; k < count; k++ )
         sprintf(szCurrentText + strlen(szCurrentText)," ");
   }
   return;
   }

   void graphicsState::beginText() {
   sendText();
   textMatrix = textMatrixIdentity;
   textMatrixPDF = textMatrixPDFIdentity;
   textLineMatrix = textMatrix;
   textLineMatrixPDF = textMatrixPDF;
   return;
   }

   void graphicsState::addText(char *pszText) {
   sprintf(szCurrentText + strlen(szCurrentText),"%s",pszText);
   show(pszText);
   return;
   }

   void graphicsState::sendText() {

   if ( ! szCurrentText[0] ) {
      rcTextObjectWindows.left = 0L;
      rcTextObjectWindows.top = 0L;
      return;
   }

   long cx = rcTextObjectWindows.right - rcTextObjectWindows.left;
   long cy = rcTextObjectWindows.bottom - rcTextObjectWindows.top;

   rcTextObjectWindows.left = (long)((double)(rcTextObjectWindows.left - textMatrixIdentity.tx) / textMatrixIdentity.a);
   rcTextObjectWindows.bottom = cyPDFPage - (long)((double)(rcTextObjectWindows.top - textMatrixIdentity.ty) / textMatrixIdentity.d);

   rcTextObjectWindows.right = rcTextObjectWindows.left + cx;
   rcTextObjectWindows.top = rcTextObjectWindows.bottom - cy;

   rcTextObjectPDF.right = rcTextObjectPDF.left + cx;
   rcTextObjectPDF.top = rcTextObjectPDF.bottom + cy;

   if ( 0.0 == textMatrix.a ) {

      long xOriginal = rcTextObjectWindows.left;
      long yOriginal = rcTextObjectWindows.bottom;

      long x = rcTextObjectWindows.left;
      rcTextObjectWindows.left = rcTextObjectWindows.top;
      rcTextObjectWindows.top = x;
      x = rcTextObjectWindows.right;
      rcTextObjectWindows.right = rcTextObjectWindows.bottom;
      rcTextObjectWindows.bottom = x;

      long cx = rcTextObjectWindows.right - xOriginal;
      rcTextObjectWindows.left -= cx;
      rcTextObjectWindows.right -= cx;

      long cy = rcTextObjectWindows.bottom - yOriginal;
      rcTextObjectWindows.top -= cy;
      rcTextObjectWindows.bottom -= cy;

   }

   if ( ! ( 0.0 == textMatrixRotation.b ) ) {

      long t = rcTextObjectPDF.left;
      rcTextObjectPDF.left = rcTextObjectPDF.top;
      rcTextObjectPDF.bottom = cxPDFPage - t;
      rcTextObjectPDF.right = rcTextObjectPDF.left + cy;
      rcTextObjectPDF.top = rcTextObjectPDF.bottom + cx;

   }

   if ( pJob -> pIPostScriptTakeText ) {
   
      char szValue[256];
      char *pszEncodedValue = NULL;

      memset(szValue,0,sizeof(szValue));

      if ( pFont )
         strncpy(szValue,pFont -> translateText(szCurrentText),255);
      else
         strncpy(szValue,szCurrentText,255);
      
#if 1
      ASCIIHexEncode(szValue,strlen(szValue),&pszEncodedValue);
      pJob -> pIPostScriptTakeText -> TakeText(&rcTextObjectPDF,pszEncodedValue);
      delete [] pszEncodedValue;
#else
      pJob -> pIPostScriptTakeText -> TakeText(&rcTextObjectPDF,szValue);
#endif

   }

   memset(szCurrentText,0,sizeof(szCurrentText));
   rcTextObjectWindows.left = 0L;
   rcTextObjectWindows.top = 0L;

   return;
   }

   void graphicsState::startLine(float tx,float ty) {
   sendText();
   textMatrix = textLineMatrix;
   textMatrixPDF = textLineMatrixPDF;
   translateTextMatrix(tx,ty);
   textLineMatrix = textMatrix;
   textLineMatrixPDF = textMatrixPDF;
   return;
   }

   void graphicsState::endText() {
   sendText();
   textMatrix = textMatrixIdentity;
   textMatrixPDF = textMatrixPDFIdentity;
   textLineMatrix = textMatrix;
   return;
   }


   XFORM *graphicsState::pTransform() {
   static XFORM theXForm;
   theXForm.eM11 = a;
   theXForm.eM12 = b;
   theXForm.eM21 = c;
   theXForm.eM22 = d;
   theXForm.eDx = tx;
   theXForm.eDy = ty;
   return &theXForm;
   }


   XFORM *graphicsState::pTextTransform() {
   static XFORM theXForm;
   theXForm.eM11 = textMatrix.a;
   theXForm.eM12 = textMatrix.b;
   theXForm.eM21 = textMatrix.c;
   theXForm.eM22 = textMatrix.d;
   theXForm.eDx = textMatrix.tx;
   theXForm.eDy = textMatrix.ty;
   return &theXForm;
   }


   void graphicsState::concat(matrix *pSource) {
   matrix::concat(pSource);
   if ( hdcTarget )
      SetWorldTransform(hdcTarget,pTransform());
   return;
   }


   void graphicsState::concat(XFORM &winXForm) {
   matrix::concat(winXForm);
   if ( hdcTarget )
      SetWorldTransform(hdcTarget,pTransform());
   return;
   }


   void graphicsState::concat(float *pValues) {
   matrix::concat(pValues);
   if ( hdcTarget )
      SetWorldTransform(hdcTarget,pTransform());
   return;
   }

   void graphicsState::restored() {
   SetWorldTransform(hdcTarget,pTransform());
   return;
   }

   void graphicsState::moveto() {
   if ( ! hdcTarget ) 
      return;
   object *pY = pJob -> pop();
   object *pX = pJob -> pop();
   lastMove.x = pX -> IntValue();
   lastMove.y = pY -> IntValue();
#if DRAW_GRAPHICS
   MoveToEx(hdcTarget,lastMove.x,lastMove.y,NULL);
#endif
   return;
   }

   void graphicsState::lineto() {
   if ( ! hdcTarget ) 
      return;
   object *pY = pJob -> pop();
   object *pX = pJob -> pop();
   lastPoint.x = pX -> IntValue();
   lastPoint.y = pY -> IntValue();
#if DRAW_GRAPHICS
   LineTo(hdcTarget,lastPoint.x,lastPoint.y);
#endif
   return;
   }

   void graphicsState::closepath() {
   if ( ! hdcTarget )
      return;
   if ( 0 == lastPoint.x )
      return;
#if DRAW_GRAPHICS
   LineTo(hdcTarget,lastMove.x,lastMove.y);
#endif
   return;
   }


   void graphicsState::show(char *pszxText) {

   RECT rcText = {0,0,0,0};

   char *pszTranslatedText = pszxText;
   if ( pFont )
      pszTranslatedText = pFont -> translateText(pszxText);

   float deltaX = measureText(pszTranslatedText,&rcText);

   if ( 0 == rcTextObjectWindows.left && 0 == rcTextObjectWindows.top ) {

      rcTextObjectWindows.left = (long)textMatrix.tx;
      rcTextObjectWindows.top = (long)textMatrix.ty;
      rcTextObjectWindows.right = rcTextObjectWindows.left;
      rcTextObjectWindows.bottom = rcTextObjectWindows.top;

      rcTextObjectPDF.left = (long)textMatrixPDF.tx;
      rcTextObjectPDF.bottom = (long)textMatrixPDF.ty;
      rcTextObjectPDF.right = rcTextObjectPDF.left;
      rcTextObjectPDF.top = rcTextObjectPDF.bottom;

   }

   if ( 0.0 == textMatrix.a )
      textMatrix.c = -textMatrix.c;
   else
      textMatrix.d = -textMatrix.d;

   SetWorldTransform(hdcTarget,pTextTransform());

   if ( 0.0 == textMatrix.a )
      textMatrix.c = -textMatrix.c;
   else
      textMatrix.d = -textMatrix.d;

#if DRAW_TEXT
   DrawTextEx(hdcTarget,pszTranslatedText,-1,&rcText,DT_NOCLIP | DT_BOTTOM,NULL);
#endif

#if DRAW_RECTANGLES
   HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
   HGDIOBJ oldBrush = SelectObject(hdcTarget,hBrush);
   FrameRect(hdcTarget,&rcText,hBrush);
   DeleteObject(SelectObject(hdcTarget,oldBrush));
#endif

   float pValues[] = {1.0, 0.0, 0.0, 1.0, deltaX, 0.0f};

   textMatrix.concat(pValues);
//
// 10-27-2011: The matrix to track PDF Units *should* translate by the Widths value in the 
// font. This is (the only ?) place where widths (and height) are susceptable to the size of the
// drawn text which is in turn affected by the size of the host window. 
// This causes the small changes in width and height of the profile rectangles.
//
   textMatrixPDF.concat(pValues);

   rcTextObjectWindows.right += rcText.right - rcText.left;
   rcTextObjectWindows.bottom = rcTextObjectWindows.top + max(rcTextObjectWindows.bottom - rcTextObjectWindows.top,rcText.bottom - rcText.top);

   SetWorldTransform(hdcTarget,pTransform());

   return;
   }


   float graphicsState::measureText(char *pszText,RECT *pResult) {

   memset(pResult,0,sizeof(RECT));

   if ( 0.0 == textMatrix.a )
      textMatrix.c = -textMatrix.c;
   else
      textMatrix.d = -textMatrix.d;

   SetWorldTransform(hdcTarget,pTextTransform());

   if ( 0.0 == textMatrix.a )
      textMatrix.c = -textMatrix.c;
   else
      textMatrix.d = -textMatrix.d;

   DrawTextEx(hdcTarget,pszText,-1,pResult,DT_CALCRECT,NULL);
   
   long cy = pResult -> bottom - pResult -> top;
   pResult -> top -= cy;
   pResult -> bottom -= cy;
   
   SetWorldTransform(hdcTarget,pTransform());

   return (float)(pResult -> right - pResult -> left);
   }


   void graphicsState::setGraphicsStateDict(char *pszDictName) {

   PdfDictionary *pDict = pJob -> pPdfPage -> findResource(pszDictName,"ExtGState");

   if ( ! pDict )
      return;

   BYTE *pValue = NULL;
   PdfElement *pElement = pDict -> Element("OPM");

   if ( pElement ) {
      pValue = pElement -> Value();
      if ( pValue ) {
      }
   }

   pElement = pDict -> Element("SA");

   if ( pElement ) {
      pValue = pElement -> Value();
      if ( pValue ) {
      }
   }

   return;
   }