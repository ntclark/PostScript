/*
Copyright 2025 EnVisioNate LLC

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the �Software�), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is the MIT License
*/

#pragma once


    class gdiParameters {
    public:

        //HRESULT SaveState();
        //HRESULT RestoreState();

        colorSpace *pColorSpace{NULL};

        void setLineCap(long v);
        void setLineJoin(long v);
        void setLineWidth(FLOAT v);
        void setMiterLimit(FLOAT v);
        void setLineDash(FLOAT *pValues,long countValues,FLOAT offset);

        void getLineCap(long *pv);
        void getLineJoin(long *pv);
        void getLineWidth(FLOAT *pv);
        void getMiterLimit(FLOAT *pv);

        void setColorSpace(colorSpace *pColorSpace);
        colorSpace *getColorSpace();

        void setColor(colorSpace *pColorSpace);
        void setRGBColor(COLORREF rgb);
        void getRGBColor(COLORREF *pRGB);
        void setRGBColor(FLOAT r,FLOAT g,FLOAT b);
        void getRGBColor(FLOAT *pR,FLOAT *pG,FLOAT *pB);

    };
