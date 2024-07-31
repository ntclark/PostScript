#pragma once

#include "job.h"

#define GLYPH_POINT_TIC_SIZE ( 16.0f / scalePointsToPixels )
#define GLYPH_POINT_TIC_SIZE_SMALL ( 8.0f / scalePointsToPixels)

#define BEZIER_CURVE_GRANULARITY  0.1
#define GLYPH_BMP_PIXELS    128

#include "gdiParameters.h"

#include "PostScript objects/matrix.h"
#include "PostScript objects/binaryString.h"
#include "PostScript objects/dictionary.h"
#include "PostScript objects/save.h"

struct POINTD;

    struct GS_POINT {
        GS_POINT() : x(0.0),y(0.0) {}
        GS_POINT(POINT_TYPE px,POINT_TYPE py) : x(px), y(py) {}
        GS_POINT(GS_POINT *pPointd);
        POINT_TYPE x;
        POINT_TYPE y;
    };

    static GS_POINT &operator-=(GS_POINT &left,GS_POINT right) { 
        left.x -= right.x;
        left.y -= right.y; 
        return left; 
    }

    static GS_POINT operator-(GS_POINT left,GS_POINT right) {
        return left -= right;
    }

    static GS_POINT &operator+=(GS_POINT &left,GS_POINT right) {
        left.x += right.x;
        left.y += right.y;
        return left;
    }

    static GS_POINT operator+(GS_POINT left,GS_POINT right) {
        return left += right;
    }

    static GS_POINT &operator*=(GS_POINT &left,POINT_TYPE right) {
        left.x *= right;
        left.y *= right;
        return left; 
    }

    static GS_POINT operator*(POINT_TYPE left, GS_POINT right ) {
        return right *= left;
    }

    static GS_POINT operator*(GS_POINT left, POINT_TYPE right ) {
        return left *= right;
    }

    static POINT_TYPE dot( GS_POINT *pLeft, GS_POINT *pRight ) {
    return pLeft -> x * pRight -> x + pLeft -> y * pRight -> y;
    }

    static POINT_TYPE dot( GS_POINT &left, GS_POINT &right ) {
    return left.x * right.x + left.y * right.y; 
    }

    static GS_POINT perpendicular( GS_POINT that ) {
    return GS_POINT( -that.y, that.x ); 
    }


    struct POINTD {
        POINTD() : POINTD(0.0f,0.0f) {}
        POINTD(POINT_TYPE *ptrX,POINT_TYPE *ptrY) : px(ptrX), py(ptrY) {};
        POINTD(POINT_TYPE x,POINT_TYPE y) {
            px = new POINT_TYPE;
            py = new POINT_TYPE;
            *px = x;
            *py = y;
        }
        POINTD(GS_POINT *pGSPoint) : POINTD(pGSPoint -> x,pGSPoint -> y) {}
        POINTD(POINTD &rhs) {
            px = new POINT_TYPE;
            py = new POINT_TYPE;
            *px = *rhs.px;
            *py = *rhs.py;
        }
        ~POINTD() {
            delete px;
            delete py;
        }

        POINT_TYPE X() { return *px; }
        POINT_TYPE Y() { return *py; }

        POINT_TYPE *px{NULL};
        POINT_TYPE *py{NULL};
    };

    static POINTD &operator-=(POINTD &left,POINTD right) { 
        *left.px -= *right.px; 
        *left.py -= *right.py; 
        return left; 
    }

    static POINTD operator-(POINTD left,POINTD right) {
        return left -= right;
    }

    static POINTD &operator+=(POINTD &left,POINTD right) {
        *left.px += right.X();
        *left.py += right.Y();
        return left;
    }

    static POINTD operator+(POINTD left,POINTD right) {
        return left += right;
    }

    static POINTD &operator*=(POINTD &left,POINT_TYPE right) {
        *left.px *= right;
        *left.py *= right;
        return left; 
    }

    static POINTD operator*(POINT_TYPE left, POINTD right ) {
        return right *= left;
    }

    static POINTD operator*(POINTD left, POINT_TYPE right ) {
        return left *= right;
    }

    static POINT_TYPE dot( POINTD *left, POINTD *right ) {
    return left -> X() * right -> X() + left -> Y() * right -> Y();
    }

    static POINT_TYPE dot( POINTD &left, POINTD &right ) {
    return left.X() * right.X() + left.Y() * right.Y(); 
    }

    static POINTD perpendicular( POINTD that ) {
    return POINTD( -that.Y(), that.X() ); 
    }

   class graphicsState {
   public:

      graphicsState(job *pJob,HWND hwndSurface = NULL);
      ~graphicsState();

      void copyFrom(graphicsState *pOther);

      void concat(matrix *);
      void concat(array *);
      void concat(POINT_TYPE *);
      void concat(XFORM &);

      void revertMatrix();

      void restored();

      void moveto();
      void moveto(object *pX,object *pY);
      void moveto(POINT_TYPE x,POINT_TYPE y);
      void moveto(GS_POINT *pPt);
      void moveto(POINTD *pPointd);

      void rmoveto();
      void rmoveto(POINT_TYPE x,POINT_TYPE y);
      void rmoveto(GS_POINT pt);
      void rmoveto(POINTD *pPointd);

      void lineto();
      void lineto(object *pX,object *pY);
      void lineto(POINT_TYPE x,POINT_TYPE y);
      void lineto(GS_POINT *pPt);
      void lineto(POINTD *pPointd);

      void rlineto();
      void rlineto(POINT_TYPE x,POINT_TYPE );
      void rlineto(GS_POINT);
      void rlineto(POINTD *pPointd);

      void curveto();
      void curveto(GS_POINT *pPoints);

      void arcto(POINT_TYPE xCenter,POINT_TYPE yCenter,POINT_TYPE radius,POINT_TYPE angle1,POINT_TYPE angle2);

      void newpath(POINT_TYPE x = POINT_TYPE_NAN,POINT_TYPE y = POINT_TYPE_NAN);
      void stroke();
      void closepath();
      void fillpath();

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

      void setMatrix(object *pMatrix);
      void currentMatrix();

      void setGraphicsStateDict(char *pszDictName);

      void setFont(class font *pFont);
      font *makeFont(class matrix *pArray,class font *pCopyFrom);
      font *scaleFont(POINT_TYPE scaleFactor,class font *pCopyFrom);

      void drawGlyph(BYTE bGlyph);
      void drawType3Glyph(BYTE bGlyph);

      void setCacheDevice();

      long DisplayResolution() { return displayResolution; }

      void gSave();
      void gRestore();

      void save();
      void restore(class save *pSave);

      void showPage();

      //matrix *ToUserSpaceXForm() { return ToUserSpace(); }

      void filter();
      void colorImage();

      font *findFont(char *pszFontName);


      void setLineCap(long v);
      void setLineJoin(long v);
      void setLineWidth(POINT_TYPE v);
      void setLineDash(array *pArray,POINT_TYPE offset);

      void setRGBColor(COLORREF rgb);
      void setRGBColor(POINT_TYPE r,POINT_TYPE g,POINT_TYPE b);

      static void SetSurface(HWND hwndSurface,long pageNumber);
      static void initMatrix(HWND hwndClient,long pageNumber);

      static matrix *ToUserSpace();

      static void outlinePage();

      static POINT_TYPE degToRad;
      static POINT_TYPE piOver2;

   private:

      job *pJob{NULL};

      static matrix *pUserSpaceToDeviceSpace;
      matrix *pToUserSpace;

      std::vector<GS_POINT *> thesePoints;

      static XFORM *pTransform();

      class font *pCurrentFont{NULL};

      POINT_TYPE scaleGlyphSpacetoPixels{0.0f};

      POINT_TYPE totalRotation{0.0};

      GS_POINT pathBeginPoint;
      GS_POINT currentUserSpacePoint;
      GS_POINT currentPointsPoint;
      GS_POINT userSpaceDomain;

      POINT currentGDIPoint;

      static gdiParametersStack gdiParametersStack;

      void gdiMoveTo(POINT *pGDIPoint);

      static long pageHeightPoints;
      static long pageWidthPoints;
      static POINT_TYPE scalePointsToPixels;

      static long cyPageGutter;
      static long pageCount;
      static long cxClient;
      static long cyClient;
      static long displayResolution;

      friend class graphicsStateStack;
   };
   