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

#include <list>

class matrix;

    class psTransformsStack : private std::list<matrix *> {
    public:

        psTransformsStack();
        ~psTransformsStack();

        void initialize(job *pJob);
        void unInitialize() {
            delete this;
        }

        void Clear() { clear(); return; }

        void setMatrix(object *pMatrix);
        void revertMatrix();
        void defaultMatrix();

        void concat(matrix *);
        void concat(array *);
        void concat(FLOAT *);
        void concat(XFORM *);

        void translate(FLOAT x,FLOAT y);
        void rotate(FLOAT angle);
        void scale(FLOAT scaleX,FLOAT scaleY);
        void setTranslation(FLOAT x,FLOAT y);

        XFORM *CurrentTransform() { return back() -> XForm(); }

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

matrix *CurrentMatrix();

        void gSave();
        void gRestore();

        boolean isInitialized() { return 0 < size(); }

    private:

        job *pJob{NULL};
    };
