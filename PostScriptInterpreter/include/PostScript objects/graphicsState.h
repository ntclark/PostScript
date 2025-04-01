/*
Copyright 2025 EnVisioNate LLC

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the “Software”), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is the MIT License
*/

#pragma once

#include "job.h"

#define GLYPH_BMP_PIXELS    128

#include "gdiParameters.h"
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

        void drawType42Glyph(uint16_t bGlyph,POINTF *pStartPoint,POINTF *pEendPoint);
        void drawType3Glyph(uint16_t bGlyph);

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

        void setStrokeAdjustmentParameter(object *pBool);

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

        boolean strokeAdjustmentParameter{true};

        GS_POINT currentUserSpacePoint POINT_TYPE_NAN_POINT;
        GS_POINT currentPageSpacePoint POINT_TYPE_NAN_POINT;

        static gdiParameters theGDIParameters;
        static pathParametersStack pathParametersStack;
        static psTransformsStack psXformsStack;

        static long pageHeightPoints;
        static long pageWidthPoints;

        static long cyPageGutter;
        static long pageCount;

        friend class graphicsStateStack;
        friend struct pathParameters;
    };
   