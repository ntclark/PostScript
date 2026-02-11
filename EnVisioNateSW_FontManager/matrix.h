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

#include "pointType.h"

#define A 0
#define B 1
#define C 2
#define D 3
#define TX 4
#define TY 5

    class matrix {
    public:

        matrix();
        matrix(FLOAT a,FLOAT b,FLOAT c,FLOAT d,FLOAT tx,FLOAT ty);
        matrix(FLOAT *pv);

        matrix(matrix &rhs);

        ~matrix();

        void SetMatrix(XFORM *pXForm);

        void identity();

        void concat(matrix *);
        void concat(XFORM *);

        void copyFrom(matrix *);

        void scale(FLOAT scaleFactor);
        void scale(FLOAT scaleX,FLOAT scaleY);
        void translate(GS_POINT &translateTo);
        void move(GS_POINT &translateTo);
        void move(FLOAT newX,FLOAT newY);

        void transformPoints(GS_POINT *pPoints,uint16_t pointCount);
        void transformPoints(POINT *pPoints,uint16_t pointCount);
        void transformPointsInPlace(POINT *pPoints,uint16_t pointCount);
        void transformPointsInPlace(POINTF *pPoints,uint16_t pointCount);

        void transformPoint(XFORM *pXForm,POINTF *ptIn,POINTF *ptOut);
        void transformPointInPlace(XFORM *pXForm,POINTF *ptIn,POINTF *ptOut);
        void transformPoint(XFORM *pXForm,POINT *ptIn,POINT *ptOut);
        void unTransformPoint(POINTF *ptIn,POINTF *ptOut);
        void unTransformPoint(POINT *ptIn,POINT *ptOut);
        void unTransformPointInPlace(POINTF *ptIn,POINTF *ptOut);

        static void transformPoints(XFORM *pXForm,GS_POINT *pPoints,uint16_t pointCount);
        static void transformPointsInPlace(XFORM *pXForm,GS_POINT *pPoints,uint16_t pointCount);

        XFORM *XForm() { return &xForm; }

    private:

        void invert();

        XFORM xForm{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};
        XFORM xFormInverse{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};

    };