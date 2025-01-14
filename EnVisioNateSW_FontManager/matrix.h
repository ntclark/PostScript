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
        void transformPoints(XFORM *pXForm,GS_POINT *pPoints,uint16_t pointCount);

        XFORM *XForm() { return &xForm; }

    private:

        XFORM xForm{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};

    };