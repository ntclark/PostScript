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
        matrix(FLOAT a,FLOAT b,FLOAT c,FLOAT d,FLOAT tx,FLOAT ty) : values{a,b,c,d,tx,ty} {}
        matrix(FLOAT *pv) : values{pv[0],pv[1],pv[2],pv[3],pv[4],pv[5]} {};

        ~matrix();

        void SetValue(long index,FLOAT value);
        void SetValues(FLOAT *pValues);

        virtual char *ToString();

        void identity();

        void concat(matrix *);
        void concat(FLOAT *);
        void concat(XFORM &);

        void revertMatrix();

        void copyFrom(matrix *);

        void scale(FLOAT scaleFactor);
        void scale(FLOAT scaleX,FLOAT scaleY);
        void translate(GS_POINT &translateTo);
        void move(GS_POINT &translateTo);

        void transformPoints(GS_POINT *pPoints,uint16_t pointCount);
        void transformPoints(POINT *pPoints,uint16_t pointCount);
        void transformPoints(FLOAT *xForm,GS_POINT *pPoints,uint16_t pointCount);

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

        FLOAT *Values() { return values; }

    private:

        FLOAT values[6]{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};
        FLOAT revertValues[6]{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};
        FLOAT inverse[6]{6 * 0.0f};
        boolean invalidated{false};

        void invert();

    };