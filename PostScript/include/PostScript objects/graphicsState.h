#pragma once

#include "job.h"

#define GLYPH_POINT_TIC_SIZE ( 16.0f / scalePointsToPixels )
#define GLYPH_POINT_TIC_SIZE_SMALL ( 8.0f / scalePointsToPixels)

#define BEZIER_CURVE_GRANULARITY  0.1
#define GLYPH_BMP_PIXELS    128

#include "PostScript objects/matrix.h"
#include "PostScript objects/binaryString.h"
#include "PostScript objects/dictionary.h"


struct SplineSet{
    double a;
    double b;
    double c;
    double d;
    double x;
};

struct POINTD;

    struct GS_POINT {
        GS_POINT() : x(0.0),y(0.0) {}
        GS_POINT(POINT_TYPE px,POINT_TYPE py) : x(px), y(py) {}
        GS_POINT(POINTD *pPointd);
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

      void newPath();

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

      void closepath();
      void fillpath();

      void transformPoint(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2);
      void transformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2);
      void untransformPoint(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *x2,POINT_TYPE *y2);
      void untransformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *x2,POINT_TYPE *y2);

      void scalePoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *px2,POINT_TYPE *py2);

      void transform(POINTD *pPoints,uint16_t pointCount);
      void transformInPlace(POINTD *pPoints,uint16_t pointCount);

      void translate(POINTD *pPoints,uint16_t pointCount,POINTD *pToPoint);
      void scale(POINTD *pPoints,uint16_t pountCount,POINT_TYPE scale);

      void setLineCap(long v) { lineCap = v; }
      void setLineJoin(long v) { lineJoin = v; }
      void setLineWidth(POINT_TYPE v) { lineWidth = v; }

      void setPageDevice(class dictionary *pDictionary);

      void initMatrix();
      void setMatrix(object *pMatrix);

      void setGraphicsStateDict(char *pszDictName);

      void setFont(class font *pFont);

      void drawGlyph(BYTE bGlyph);
      void drawType3Glyph(BYTE bGlyph);

      void setCacheDevice();

      long DisplayResolution() { return displayResolution; }

      void gSave();
      void gRestore();

      matrix *ToUserSpaceXForm() { return pToUserSpace; }

      void filter();
      void colorImage();

      HDC activeDC() { if ( ! ( NULL == hdcAlternate ) ) return hdcAlternate; return hdcSurface; }

      font *findFont(char *pszFontName);

   private:

      job *pJob{NULL};

      static matrix *pFromUserSpaceToDeviceSpace;
      matrix *pToUserSpace{NULL};

      POINTD *lerp(POINTD *pStart,POINTD *pEnd,POINT_TYPE slope);
      void lerp(GS_POINT *pStart,GS_POINT *pEnd,POINT_TYPE slope,GS_POINT *pResult);
      void bezierCurve(POINTD *point_1,POINTD *control_1,POINTD *control_2,POINTD *point_2,POINT_TYPE angular);
      void bezierCurve(GS_POINT *pPoint1,GS_POINT *pPoint2,GS_POINT *pPoint3,GS_POINT *pPoint4,POINT_TYPE angular);
      void addTessellation(POINTD *point_1,POINTD *control_1,POINTD *control_2,POINTD *point_2,POINT_TYPE angular,long limit);
      void addTessellation(GS_POINT *point_1,GS_POINT *control_1,GS_POINT *control_2,GS_POINT *point_2,POINT_TYPE angular,long limit);

      std::vector<GS_POINT *> thesePoints;

      XFORM *pTransform();

      POINT_TYPE lineWidth{0.0f};
      long lineCap{0L};
      long lineJoin{0L};

      class font *pCurrentFont{NULL};

      long pageHeightPoints{0L};
      long pageWidthPoints{0L};

      POINT_TYPE scalePointsToPixels{0.0f};
      POINT_TYPE scaleGlyphSpacetoPixels{0.0f};
      GS_POINT glyphSpaceDomain;

      GS_POINT lastUserSpacePoint;
      GS_POINT lastUserSpaceMovedToPoint;
      GS_POINT lastPointsPoint;

      static long cxClient;
      static long cyClient;
      static long displayResolution;
      static HWND hwndSurface;
      static HDC hdcSurface;
      static HDC hdcAlternate;
      static HBITMAP hbmGlyph;

      friend class graphicsStateStack;
   };
   