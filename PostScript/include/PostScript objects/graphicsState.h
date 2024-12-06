#pragma once

#include "job.h"

#define GLYPH_POINT_TIC_SIZE ( 16.0f / scalePointsToPixels )
#define GLYPH_POINT_TIC_SIZE_SMALL ( 8.0f / scalePointsToPixels)

#define BEZIER_CURVE_GRANULARITY  0.1
#define GLYPH_BMP_PIXELS    128

#include "Stacks/gdiParametersStack.h"
#include "Stacks/pathParametersStack.h"
#include "Stacks/fontStack.h"
#include "pathParameters.h"

   class graphicsState {
   public:

      graphicsState(job *pJob);
      ~graphicsState();

      void setMatrix(object *pMatrix);
      void currentMatrix();
      void revertMatrix();

      void concat(matrix *);
      void concat(array *);
      void concat(POINT_TYPE *);
      void concat(XFORM &);

      void restored();

      void moveto();
      void moveto(object *pX,object *pY);
      void moveto(POINT_TYPE x,POINT_TYPE y);
      void moveto(GS_POINT *pPt);

      void rmoveto();
      void rmoveto(POINT_TYPE x,POINT_TYPE y);
      void rmoveto(GS_POINT pt);

      void lineto();
      void lineto(object *pX,object *pY);
      void lineto(POINT_TYPE x,POINT_TYPE y);
      void lineto(GS_POINT *pPt);

      void rlineto();
      void rlineto(POINT_TYPE x,POINT_TYPE );
      void rlineto(GS_POINT);

      void curveto();
      void curveto(GS_POINT *pPoints);

      void arcto(POINT_TYPE xCenter,POINT_TYPE yCenter,POINT_TYPE radius,POINT_TYPE angle1,POINT_TYPE angle2);

      void dot(GS_POINT at,POINT_TYPE radius);

      void newpath(POINT_TYPE x = POINT_TYPE_NAN,POINT_TYPE y = POINT_TYPE_NAN);
      void stroke();
      void closepath();
      void fillpath(boolean doRasterization = false);
      void eofillpath(boolean doRasterization = false);

      boolean savePath(boolean doSave);

      boolean setDefaultToRasterize(boolean doRasterization);

      void translate(POINT_TYPE x,POINT_TYPE y);
      void rotate(POINT_TYPE angle);
      void scale(POINT_TYPE scaleX,POINT_TYPE scaleY);

      void transformPoint(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2);
      void transformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2);
      void transformPointInPlace(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2);
      void transformPointInPlace(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2);

      void untransformPoint(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *x2,POINT_TYPE *y2);
      void untransformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *x2,POINT_TYPE *y2);
      void untransformPointInPlace(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *x2,POINT_TYPE *y2);
      void untransformPointInPlace(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *x2,POINT_TYPE *y2);

      void scalePoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *px2,POINT_TYPE *py2);

      void transform(GS_POINT *pPoints,uint16_t pointCount);
      void transformInPlace(GS_POINT *pPoints,uint16_t pointCount);

      void translate(GS_POINT *pPoints,uint16_t pointCount,GS_POINT *pToPoint);
      void scale(GS_POINT *pPoints,uint16_t pountCount,POINT_TYPE scale);

      void setPageDevice(class dictionary *pDictionary);

      void setGraphicsStateDict(char *pszDictName);

      void setFont(font *pFont);
      font *makeFont(class matrix *pArray,font *pCopyFrom);
      font *scaleFont(POINT_TYPE scaleFactor,font *pCopyFrom);

      void drawGlyph(BYTE bGlyph);
      void drawType3Glyph(BYTE bGlyph);

      void setCacheDevice();

      long DisplayResolution() { return displayResolution; }

      void initialize();

      void gSave();
      void gRestore();

      void save();
      void restore(class save *pSave);

      void showPage();

      void filter();
      void image();
      void colorImage();
      void renderImage(HBITMAP hbmResult,object *pWidth,object *pHeight);
      void renderImage(HBITMAP hbmResult,uint16_t width,uint16_t height);

      font *findFont(char *pszFontName);

      void setLineCap(long v);
      void setLineJoin(long v);
      void setLineWidth(POINT_TYPE v);
      void setLineDash(array *pArray,POINT_TYPE offset);

      void setColorSpace(colorSpace *);
      colorSpace *getColorSpace();

      void setColor(colorSpace *);
      void setRGBColor(COLORREF rgb);
      void setRGBColor(POINT_TYPE r,POINT_TYPE g,POINT_TYPE b);

      static void SetSurface(HWND hwndSurface,long pageNumber);
      static void initMatrix(HWND hwndClient,long pageNumber);

      static void outlinePage();

      static POINT_TYPE degToRad;
      static POINT_TYPE piOver2;

   private:

      job *pJob{NULL};

      static gdiParametersStack gdiParametersStack;
      static pathParametersStack pathParametersStack;
      static fontStack fontStack;

      void gdiMoveTo(POINT *pGDIPoint);

      static long pageHeightPoints;
      static long pageWidthPoints;
      static POINT_TYPE scalePointsToPixels;

      static long cyPageGutter;
      static long pageCount;
      static long cxClient;
      static long cyClient;
      static long cyWindow;
      static long displayResolution;

      friend class graphicsStateStack;
      friend struct pathParameters;
   };
   