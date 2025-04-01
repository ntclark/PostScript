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


#include "object.h"
#include "PostScript objects/array.h"

    class matrix : public array {
    public:

        matrix(job *pJob);
        ~matrix();

        void SetValue(long index,FLOAT value);

        //virtual char *ToString();

        void identity();

        void concat(matrix *);
        void concat(array *);
        void concat(XFORM *);
        void concat(FLOAT *);

        void revertMatrix();

        void copyFrom(matrix *);

        void scale(FLOAT scaleFactor);
        void scale(FLOAT scaleX,FLOAT scaleY);
        void translate(FLOAT toX,FLOAT toY);
        void rotate(FLOAT angle);

        FLOAT *Values();
        XFORM *XForm();

        FLOAT a();
        FLOAT b();
        FLOAT c();
        FLOAT d();
        FLOAT tx();
        FLOAT ty();

        FLOAT aInverse(boolean force = false);
        FLOAT bInverse(boolean force = false);
        FLOAT cInverse(boolean force = false);
        FLOAT dInverse(boolean force = false);
        FLOAT txInverse(boolean force = false);
        FLOAT tyInverse(boolean force = false);

        void a(FLOAT v);
        void b(FLOAT v);
        void c(FLOAT v);
        void d(FLOAT v);
        void tx(FLOAT v);
        void ty(FLOAT v);

        static void scale(XFORM *,FLOAT scaleFactor);

    private:

        XFORM xForm{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};

        FLOAT revertValues[6]{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};
        FLOAT inverse[6]{6 * 0.0f};
        boolean invalidated{false};

        void invert();

    };