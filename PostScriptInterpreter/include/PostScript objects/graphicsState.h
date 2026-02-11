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
#include "Stacks/psTransformsStack.h"
#include "Stacks/fontStack.h"
#include "pageParameters.h"

    class graphicsState {
    public:

        graphicsState(job *pJob);
        ~graphicsState();

        psTransformsStack *PSTransforms() { return pPSXformsStack; }

        void currentMatrix();

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

        void newpath();
        void stroke();
        void closepath();
        void fillpath();
        void eofillpath();
        void render();

        void setPageDevice(dictionary *pDictionary);

        void setGraphicsStateDict(char *pszDictName);

        void setFont(font *pFont);
        font *makeFont(array *pArray,font *pCopyFrom);
        font *makeFont(matrix *pMatix,font *pCopyFrom);
        font *scaleFont(FLOAT scaleFactor,font *pCopyFrom);

        void drawTextChar(BYTE bGlyph,boolean emitEvent = true,POINT *ptStartPDF = NULL,POINT *ptEndPDF = NULL);
        void drawTextString();

        void drawType1Glyph(uint16_t bGlyph,POINTF *pStartPoint,POINTF *pEndPoint,POINTF *pStartPDF = NULL,POINTF *pEndPDF = NULL);
        void drawType3Glyph(uint16_t bGlyph,POINTF *pStartPDF = NULL,POINTF *pEndPDF = NULL);
        void drawType42Glyph(uint16_t bGlyph,POINTF *pStartPoint,POINTF *pEndPoint,POINTF *pStartPDF = NULL,POINTF *pEndPDF = NULL);

        void setCacheDevice();

        void initialize();

        void gSave();
        void gRestore();

        void save();
        void restore(class save *pSave);

        void showPage();

        void filter();
        void image();
        void imageMask();
        void colorImage();

        font *findFont(char *pszFontName);

        void setLineCap(long v);
        void setLineJoin(long v);
        void setLineWidth(FLOAT v);
        void setMiterLimit(FLOAT v);

        void setLineDash(array *pArray,FLOAT offset);

        void getLineCap(long *pV);
        void getLineJoin(long *pV);
        void getLineWidth(FLOAT *pV);
        void getMiterLimit(FLOAT *pV);

        void setStrokeAdjustmentParameter(object *pBool);

        void setColorSpace(colorSpace *);
        colorSpace *getColorSpace();

        void setColor(colorSpace *);
        void setRGBColor(COLORREF rgb);
        void getRGBColor(COLORREF *pRGB);
        void setRGBColor(FLOAT r,FLOAT g,FLOAT b);
        void getRGBColor(FLOAT *pR,FLOAT *pG,FLOAT *pB);

        POINTF *CurrentPoint();

        font *CurrentFont() { return font::CurrentFont(); }

        static void RenderGeometry();

        static void SetSurface(HWND hwndSurface,long pageNumber);
        static void initMatrix(HWND hwndClient,long pageNumber);

        static FLOAT degToRad;
        static FLOAT piOver2;

    private:

        job *pJob{NULL};

        boolean strokeAdjustmentParameter{true};

        uint8_t *getBitmapBits(uint8_t *pbImage,long cbData,long width,long height,long bitsPerComponent,array *pDecodeArray);

        GS_POINT currentUserSpacePoint POINT_TYPE_NAN_POINT;

        static gdiParameters theGDIParameters;
        static pageParameters thePageParameters;
        static psTransformsStack *pPSXformsStack;

        static long pageHeightPoints;
        static long pageWidthPoints;

        static long cyPageGutter;
        static long pageCount;

        static long instanceCount;

        friend class graphicsStateStack;
    };
   