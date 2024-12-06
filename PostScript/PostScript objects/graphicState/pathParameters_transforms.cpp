#include "job.h"

#include "pathParameters.h"

int Mx3Inverse(double *sourceMatrix,double *targetMatrix);
int MxV(double *a,double *b,double *c);
int MxM(double *a,double *b,double *c);

    double pathParameters::untransformGDIMatrix[3][3];
    double pathParameters::untransformPDFMatrix[3][3];

    XFORM *pathParameters::pTransform() {

    static XFORM theXForm;

    theXForm.eM11 = (float)pUserSpaceToDeviceSpace -> a();
    theXForm.eM21 = (float)pUserSpaceToDeviceSpace -> b();
    theXForm.eM12 = (float)pUserSpaceToDeviceSpace -> c();
    theXForm.eM22 = (float)pUserSpaceToDeviceSpace -> d();
    theXForm.eDx = (float)pUserSpaceToDeviceSpace -> tx();
    theXForm.eDy = (float)pUserSpaceToDeviceSpace -> ty();

    double theMatrix[3][3];

    theMatrix[0][0] = (double)theXForm.eM11;
    theMatrix[0][1] = (double)theXForm.eM21;
    theMatrix[0][2] = (double)theXForm.eDx;

    theMatrix[1][0] = (double)theXForm.eM12;
    theMatrix[1][1] = (double)theXForm.eM22;
    theMatrix[1][2] = (double)theXForm.eDy;

    theMatrix[2][0] = 0.0;
    theMatrix[2][1] = 0.0;
    theMatrix[2][2] = 1.0;

    Mx3Inverse(&theMatrix[0][0],&untransformGDIMatrix[0][0]);
/*
double v[]{0.5,0.75,1.0};
double vAnswer[]{0.0,0.0,0.0};
double prod[][3]{ {0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};

MxV(&theMatrix[0][0],v,vAnswer);
MxV(&untransformGDIMatrix[0][0],vAnswer,v);

MxM(&theMatrix[0][0],&untransformGDIMatrix[0][0],&prod[0][0]);
*/
    return &theXForm;
    }


    void pathParameters::untransformGDI(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX,POINT_TYPE *pY) {
    double b[] = {(double)x,(double)y,0.0};
    double c[] = {0.0,0.0,0.0};
    MxV(&untransformGDIMatrix[0][0],b,c);
    *pX = (POINT_TYPE)c[0];
    *pY = (POINT_TYPE)c[1];
    return;
    }


    XFORM *pathParameters::pIdentityTransform() {
    static XFORM theXForm;
    theXForm.eM11 = 1.0f;
    theXForm.eM21 = 0.0f;
    theXForm.eM12 = 0.0f;
    theXForm.eM22 = 1.0f;
    theXForm.eDx = 0.0f;
    theXForm.eDy = 0.0f;
    return &theXForm;
    }


    void pathParameters::setMatrix(POINT_TYPE *pValues) {
    pToUserSpace -> a(pValues[A]);
    pToUserSpace -> b(pValues[B]);
    pToUserSpace -> c(pValues[C]);
    pToUserSpace -> d(pValues[D]);
    pToUserSpace -> tx(pValues[TX]);
    pToUserSpace -> ty(pValues[TY]);
    return;
    }


    void pathParameters::currentMatrix() {
    matrix *pMatrix = reinterpret_cast<matrix *>(pPStoPDF -> currentJob() -> pop());
    pMatrix -> a(pToUserSpace -> a());
    pMatrix -> b(pToUserSpace -> b());
    pMatrix -> c(pToUserSpace -> c());
    pMatrix -> d(pToUserSpace -> d());
    pMatrix -> tx(pToUserSpace -> tx());
    pMatrix -> ty(pToUserSpace -> ty());
    pPStoPDF -> currentJob() -> push(pMatrix);
    return;
    }


    void pathParameters::revertMatrix() {
    pToUserSpace -> revertMatrix();
    return;
    }


    void pathParameters::concat(matrix *pSource) {
    pToUserSpace -> concat(pSource);
    return;
    }


    void pathParameters::concat(array *pSource) {
    pToUserSpace -> concat(pSource);
    return;
    }


    void pathParameters::concat(XFORM &winXForm) {
    pToUserSpace -> concat(winXForm);
    return;
    }


    void pathParameters::concat(POINT_TYPE *pValues) {
    pToUserSpace -> concat(pValues);
    return;
    }


    void pathParameters::translate(POINT_TYPE x,POINT_TYPE y) {
    POINT_TYPE values[] = {1.0,0.0,0.0,1.0,x,y};
    concat(values);
    return;
    }


    void pathParameters::scale(POINT_TYPE scaleX,POINT_TYPE scaleY) {
    POINT_TYPE values[] = {scaleX,0.0,0.0,scaleY,0.0,0.0};
    concat(values);
    return;
    }


    void pathParameters::rotate(POINT_TYPE angle) {
    POINT_TYPE cosAngle = cos(-angle * graphicsState::degToRad);
    POINT_TYPE sinAngle = sin(-angle * graphicsState::degToRad);
    POINT_TYPE values[] = {cosAngle,sinAngle,-sinAngle,cosAngle,0.0,0.0};
    concat(values);
    totalRotation -= angle;
    return;
    }


    void pathParameters::transformPoint(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    *pX2 = pMatrix -> a() * x + pMatrix -> b() * y + pMatrix -> tx();
    *pY2 = pMatrix -> c() * x + pMatrix -> d() * y + pMatrix -> ty();
    return;
    }


    void pathParameters::transformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult = pToUserSpace -> a() * x + pToUserSpace -> b() * y + pToUserSpace -> tx();
    POINT_TYPE yResult = pToUserSpace -> c() * x + pToUserSpace -> d() * y + pToUserSpace -> ty();
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void pathParameters::transformPointInPlace(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    *pX2 = pMatrix -> a() * x + pMatrix -> b() * y;
    *pY2 = pMatrix -> c() * x + pMatrix -> d() * y;
    return;
    }


    void pathParameters::transformPointInPlace(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult = pToUserSpace -> a() * x + pToUserSpace -> b() * y;
    POINT_TYPE yResult = pToUserSpace -> c() * x + pToUserSpace -> d() * y;
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void pathParameters::untransformPoint(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult = pMatrix -> aInverse() * x + pMatrix -> bInverse() * y + pMatrix -> txInverse();
    POINT_TYPE yResult = pMatrix -> cInverse() * x + pMatrix -> dInverse() * y + pMatrix -> tyInverse();
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void pathParameters::untransformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult = pToUserSpace -> aInverse() * x + pToUserSpace -> bInverse() * y + pToUserSpace -> txInverse();
    POINT_TYPE yResult = pToUserSpace -> cInverse() * x + pToUserSpace -> dInverse() * y + pToUserSpace -> tyInverse();
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void pathParameters::untransformPointInPlace(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult = pMatrix -> aInverse() * x + pMatrix -> bInverse() * y;
    POINT_TYPE yResult = pMatrix -> cInverse() * x + pMatrix -> dInverse() * y;
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void pathParameters::untransformPointInPlace(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    POINT_TYPE xResult = pToUserSpace -> aInverse() * x + pToUserSpace -> bInverse() * y;
    POINT_TYPE yResult = pToUserSpace -> cInverse() * x + pToUserSpace -> dInverse() * y;
    *pX2 = xResult;
    *pY2 = yResult;
    return;
    }


    void pathParameters::scalePoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2) {
    *pX2 = x * pToUserSpace -> a();
    *pY2 = y * pToUserSpace -> d();
    return;
    }


    void pathParameters::transform(GS_POINT *pPoints,uint16_t pointCount) {
    for ( long k = 0; k < pointCount; k++ ) {
        POINT_TYPE x = pPoints[k].x;
        POINT_TYPE y = pPoints[k].y;
        pPoints[k].x = pToUserSpace -> a() * x + pToUserSpace -> b() * y + pToUserSpace -> tx();
        pPoints[k].y = pToUserSpace -> c() * x + pToUserSpace -> d() * y + pToUserSpace -> ty();
    }
    return;
    }


    void pathParameters::transformInPlace(GS_POINT *pPoints,uint16_t pointCount) {
    for ( long k = 0; k < pointCount; k++ ) {
        POINT_TYPE x = pPoints[k].x;
        POINT_TYPE y = pPoints[k].y;
        pPoints[k].x = pToUserSpace -> a() * x + pToUserSpace -> b() * y;
        pPoints[k].y = pToUserSpace -> c() * x + pToUserSpace -> d() * y;
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
