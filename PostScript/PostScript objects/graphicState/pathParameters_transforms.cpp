#include "job.h"

#include "pathParameters.h"

    POINT_TYPE pathParameters::toDeviceSpace[6]{1.0,0.0,0.0,1.0,0.0,0.0};
    POINT_TYPE pathParameters::toDeviceSpaceInverse[6]{6 * 0.0};


    void pathParameters::transformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult = toDeviceSpace[A] * x + toDeviceSpace[B] * y + toDeviceSpace[TX];
    POINT_TYPE yResult = toDeviceSpace[C] * x + toDeviceSpace[D] * y + toDeviceSpace[TY];
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void pathParameters::transformPointInPlace(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult = toDeviceSpace[A] * x + toDeviceSpace[B] * y;
    POINT_TYPE yResult = toDeviceSpace[C] * x + toDeviceSpace[D] * y;
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void pathParameters::untransformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult = toDeviceSpaceInverse[A] * x + toDeviceSpaceInverse[B] * y + toDeviceSpaceInverse[TX];
    POINT_TYPE yResult = toDeviceSpaceInverse[C] * x + toDeviceSpaceInverse[D] * y + toDeviceSpaceInverse[TY];
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void pathParameters::untransformPointInPlace(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult = toDeviceSpaceInverse[A] * x + toDeviceSpaceInverse[B] * y;
    POINT_TYPE yResult = toDeviceSpaceInverse[C] * x + toDeviceSpaceInverse[D] * y;
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void pathParameters::scalePoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    *pX2 = x * toDeviceSpace[A];
    *pY2 = y * toDeviceSpace[D];
    return;
    }


    void pathParameters::transform(GS_POINT *pPoints,uint16_t pointCount) {
    for ( long k = 0; k < pointCount; k++ ) {
        POINT_TYPE x = pPoints[k].x;
        POINT_TYPE y = pPoints[k].y;
        pPoints[k].x = toDeviceSpace[A] * x + toDeviceSpace[B] * y + toDeviceSpace[TX];
        pPoints[k].y = toDeviceSpace[C] * x + toDeviceSpace[D] * y + toDeviceSpace[TY];
    }
    return;
    }


    void pathParameters::transformInPlace(GS_POINT *pPoints,uint16_t pointCount) {
    for ( long k = 0; k < pointCount; k++ ) {
        POINT_TYPE x = pPoints[k].x;
        POINT_TYPE y = pPoints[k].y;
        pPoints[k].x = toDeviceSpace[A] * x + toDeviceSpace[B] * y;
        pPoints[k].y = toDeviceSpace[C] * x + toDeviceSpace[D] * y;
    }
    return;
    }


    void pathParameters::translate(GS_POINT *pPoints,uint16_t pointCount,GS_POINT *pToPoint) {
    for ( long k = 0; k < pointCount; k++ ) {
        pPoints[k].x += pToPoint -> x;
        pPoints[k].y += pToPoint -> y;
    }
    return;
    }


    void pathParameters::scale(GS_POINT *pPoints,uint16_t pointCount,POINT_TYPE scale) {
    for ( long k = 0; k < pointCount; k++ ) {
        pPoints[k].x = pPoints[k].x * scale;
        pPoints[k].y = pPoints[k].y * scale;
    }
    return;
    }
