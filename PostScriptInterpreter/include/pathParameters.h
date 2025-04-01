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


#include "Renderer_i.h"

    struct pathParameters {

    public:

        pathParameters();
        pathParameters(pathParameters *pRHS);
        ~pathParameters();

        void initialize();

        void redirectType3();
        void unRedirectType3();

        void initMatrix(HWND hwndClient,long pageNumber,long pageHeightPoints);

        // Graphics primitives

        void moveto(GS_POINT *pPt);
        void moveto(FLOAT x,FLOAT y);

        void rmoveto(GS_POINT *pPt);
        void rmoveto(FLOAT x,FLOAT y);

        void lineto(GS_POINT *pPt);
        void lineto(FLOAT x,FLOAT y);

        void rlineto(GS_POINT *pPt);
        void rlineto(FLOAT x,FLOAT );

        void arcto(FLOAT xCenter,FLOAT yCenter,FLOAT radius,FLOAT angle1,FLOAT angle2);

        void curveto(FLOAT x0,FLOAT y0,FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2,FLOAT x3,FLOAT y3);
        void quadcurveto(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2);

        static XFORM *ToDeviceSpace() { return &toDeviceSpace; }
        static void RenderGeometry();

        // Paths

        void newpath();
        void stroke();
        void closepath();
        void fillpath();
        void eofillpath();

    private:

        IGraphicElements *pIGraphicElements{NULL};

        static IRenderer *pIRenderer_text;
        static IGraphicElements *pIGraphicElements_text;

        static long cxClient;
        static long cyClient;
        static long cyWindow;
        static long displayResolution;

        static FLOAT scalePointsToPixels;
        static FLOAT renderingHeight;

        static XFORM toDeviceSpace;
        static XFORM toDeviceSpaceInverse;

        friend class graphicsState;

    };
