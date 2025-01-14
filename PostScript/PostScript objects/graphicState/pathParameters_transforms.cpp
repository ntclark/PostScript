#include "job.h"

#include "pathParameters.h"


    void pathParameters::transformPoint(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    FLOAT xResult = toDeviceSpace.eM11 * x + toDeviceSpace.eM12 * y + toDeviceSpace.eDx;
    FLOAT yResult = toDeviceSpace.eM21 * x + toDeviceSpace.eM22 * y + toDeviceSpace.eDy;
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void pathParameters::transformPointInPlace(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    FLOAT xResult = toDeviceSpace.eM11 * x + toDeviceSpace.eM12 * y;
    FLOAT yResult = toDeviceSpace.eM21 * x + toDeviceSpace.eM22 * y;
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void pathParameters::untransformPoint(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    FLOAT xResult = toDeviceSpaceInverse.eM11 * x + toDeviceSpaceInverse.eM12 * y + toDeviceSpaceInverse.eDx;
    FLOAT yResult = toDeviceSpaceInverse.eM21 * x + toDeviceSpaceInverse.eM22 * y + toDeviceSpaceInverse.eDy;
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void pathParameters::untransformPointInPlace(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    FLOAT xResult = toDeviceSpaceInverse.eM11 * x + toDeviceSpaceInverse.eM12 * y;
    FLOAT yResult = toDeviceSpaceInverse.eM21 * x + toDeviceSpaceInverse.eM22 * y;
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void pathParameters::scalePoint(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2) {
    *pX2 = x * toDeviceSpace.eM11;
    *pY2 = y * toDeviceSpace.eM22;
    return;
    }


    void pathParameters::transform(GS_POINT *pPoints,uint16_t pointCount) {
    for ( long k = 0; k < pointCount; k++ ) {
        FLOAT x = pPoints[k].x;
        FLOAT y = pPoints[k].y;
        pPoints[k].x = toDeviceSpace.eM11 * x + toDeviceSpace.eM12 * y + toDeviceSpace.eDx;
        pPoints[k].y = toDeviceSpace.eM21 * x + toDeviceSpace.eM22 * y + toDeviceSpace.eDy;
    }
    return;
    }


    void pathParameters::transformInPlace(GS_POINT *pPoints,uint16_t pointCount) {
    for ( long k = 0; k < pointCount; k++ ) {
        FLOAT x = pPoints[k].x;
        FLOAT y = pPoints[k].y;
        pPoints[k].x = toDeviceSpace.eM11 * x + toDeviceSpace.eM12 * y;
        pPoints[k].y = toDeviceSpace.eM21 * x + toDeviceSpace.eM22 * y;
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


    void pathParameters::scale(GS_POINT *pPoints,uint16_t pointCount,FLOAT scale) {
    for ( long k = 0; k < pointCount; k++ ) {
        pPoints[k].x = pPoints[k].x * scale;
        pPoints[k].y = pPoints[k].y * scale;
    }
    return;
    }
