#pragma once

#include "job.h"

#define GLYPH_BMP_PIXELS    128

#include "Stacks/gdiParametersStack.h"
#include "Stacks/pathParametersStack.h"
#include "Stacks/psTransformsStack.h"
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
        void concat(FLOAT *);
        void concat(XFORM *);

        void restored();

        // ALL graphics primitives take values in USER space
        // as input. That is, values verbatim as they exist
        // in the postscript file.

        void moveto();
        void moveto(object *pX,object *pY);
        void moveto(FLOAT x,FLOAT y);
        void moveto(GS_POINT *pPt);
        void moveto(POINTF *pPt);

        void rmoveto();
        void rmoveto(FLOAT x,FLOAT y);
        void rmoveto(GS_POINT pt);

        void lineto();
        void lineto(object *pX,object *pY);
        void lineto(FLOAT x,FLOAT y);
        void lineto(GS_POINT *pPt);

        void rlineto();
        void rlineto(FLOAT x,FLOAT );
        void rlineto(GS_POINT);

        void curveto();
        void quadcurveto();

        void arcto(FLOAT xCenter,FLOAT yCenter,FLOAT radius,FLOAT angle1,FLOAT angle2);

        void dot(GS_POINT at,FLOAT radius);

        void newpath();
        void stroke();
        void closepath();
        void fillpath();
        void eofillpath();

        // ALL transformations are from USER space to PAGE space
        // USER space is that in which all numeric x,y values are in
        // in the postscript file.
        // PAGE space is the domain PDF Width x PDF Height
        // Transformation from USER space to PAGE space
        // uses the "current transformation matrix" (CTM)
        // and in the case of text, the current font matrix

        void translate(FLOAT x,FLOAT y);
        void rotate(FLOAT angle);
        void scale(FLOAT scaleX,FLOAT scaleY);
        void setTranslation(FLOAT x,FLOAT y);

        void transformPoint(matrix *pMatrix,FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2);
        void transformPoint(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2);
        void transformPoint(GS_POINT *ptIn,GS_POINT *ptOut);
        void transformPointInPlace(matrix *pMatrix,FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2);
        void transformPointInPlace(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2);

        void untransformPoint(matrix *pMatrix,FLOAT x,FLOAT y,FLOAT *x2,FLOAT *y2);
        void untransformPoint(FLOAT x,FLOAT y,FLOAT *x2,FLOAT *y2);
        void untransformPoint(POINTF *ptIn,POINTF *ptOut);
        void untransformPointInPlace(matrix *pMatrix,FLOAT x,FLOAT y,FLOAT *x2,FLOAT *y2);
        void untransformPointInPlace(FLOAT x,FLOAT y,FLOAT *x2,FLOAT *y2);

        void scalePoint(FLOAT x,FLOAT y,FLOAT *px2,FLOAT *py2);

        void setPageDevice(dictionary *pDictionary);

        void setGraphicsStateDict(char *pszDictName);

        void setFont(font *pFont);
        font *makeFont(array *pArray,font *pCopyFrom);
        font *makeFont(matrix *pMatix,font *pCopyFrom);
        font *scaleFont(FLOAT scaleFactor,font *pCopyFrom);

        void drawTextChar(BYTE bGlyph);
        void drawTextString();

        void drawType42Glyph(BYTE bGlyph,POINTF *pStartPoint,POINTF *pEendPoint);
        void drawType3Glyph(BYTE bGlyph);

#ifdef USE_RENDERER
#else
        void openGeometry();
        void closeGeometry();
        void renderGeometry();
#endif
        void setCacheDevice();

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
        void setLineWidth(FLOAT v);
        void setLineDash(array *pArray,FLOAT offset);

        void setColorSpace(colorSpace *);
        colorSpace *getColorSpace();

        void setColor(colorSpace *);
        void setRGBColor(COLORREF rgb);
        void setRGBColor(FLOAT r,FLOAT g,FLOAT b);

        POINTF *CurrentPoint();

        static void RenderGeometry();

        static void SetSurface(HWND hwndSurface,long pageNumber);
        static void initMatrix(HWND hwndClient,long pageNumber);

        static FLOAT degToRad;
        static FLOAT piOver2;

    private:

        job *pJob{NULL};

        font *CurrentFont() { return font::CurrentFont(); }

        GS_POINT currentUserSpacePoint POINT_TYPE_NAN_POINT;
        GS_POINT currentPageSpacePoint POINT_TYPE_NAN_POINT;

        static gdiParametersStack gdiParametersStack;
        static pathParametersStack pathParametersStack;
        static psTransformsStack psXformsStack;

        static long pageHeightPoints;
        static long pageWidthPoints;

        static long cyPageGutter;
        static long pageCount;

        friend class graphicsStateStack;
        friend struct pathParameters;
    };
   