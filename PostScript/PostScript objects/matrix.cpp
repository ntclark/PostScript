
#include "job.h"

#include "PostScript objects\matrix.h"
#include <stdio.h>

#define A 0
#define B 1
#define C 2
#define D 3
#define TX 4
#define TY 5

    matrix::matrix(job *pJob) :
        array(pJob,6)
    {
    theObjectType = object::objectType::matrix;
    a(1.0);
    b(0.0);
    c(0.0);
    d(1.0);
    tx(0.0);
    ty(0.0);
    }

    matrix::~matrix() {
    return;
    }

    void matrix::SetValue(long index,float value) {
    entries[index] = new (pJob -> CurrentObjectHeap()) object(pJob,value);
    return;
    }

    float matrix::a() { return entries[A] -> FloatValue(); }
    float matrix::b() { return entries[B] -> FloatValue(); }
    float matrix::c() { return entries[C] -> FloatValue(); }
    float matrix::d() { return entries[D] -> FloatValue(); }
    float matrix::tx() { return entries[TX] -> FloatValue(); }
    float matrix::ty() { return entries[TY] -> FloatValue(); }

    void matrix::a(float v) { entries[A] = new (pJob -> CurrentObjectHeap()) object(pJob,v); }
    void matrix::b(float v) { entries[B] = new (pJob -> CurrentObjectHeap()) object(pJob,v); }
    void matrix::c(float v) { entries[C] = new (pJob -> CurrentObjectHeap()) object(pJob,v); }
    void matrix::d(float v) { entries[D] = new (pJob -> CurrentObjectHeap()) object(pJob,v); }
    void matrix::tx(float v) { entries[TX] = new (pJob -> CurrentObjectHeap()) object(pJob,v); }
    void matrix::ty(float v) { entries[TY] = new (pJob -> CurrentObjectHeap()) object(pJob,v); }

    char *matrix::ToString() {
    static char szMatrix[64];
    sprintf_s<64>(szMatrix,"%5.2f, %5.2f, %5.2f, %5.2f, %5.2f, %5.2f",a(),b(),c(),d(),tx(),ty());
    return szMatrix;
    }

    void matrix::concat(matrix *pSource) {
    float values[] = {pSource -> a(),pSource -> b(),pSource -> c(),pSource -> d(),pSource -> tx(),pSource -> ty()};
    matrix::concat(values);
    return;
    }

    void matrix::concat(XFORM &winXForm) {
    matrix::concat(reinterpret_cast<float *>(&winXForm));
    return;
    }

    void matrix::concat(float *pPrime) {

//                                              A                       B
//   |  a'   b'   0  |   | a  b  0 |    |  a'*a +  b'*c + 0*tx ,  a'*b +  b'*d + 0*ty ,  a'*0 +  b'*0 + 0*1
//                                              C                       D
//   |  c'   d'   0  | * | c  d  0 | =  |  c'*a +  d'*c + 0*tx ,  c'*b +  d'*d + 0*ty ,  c'*0 +  d'*0 + 0*1
//                                              TX                      TY
//   |  tx'  ty'  1  |   | tx ty 1 |    | tx'*a + ty'*c + 1*tx , tx'*b + ty'*d + 1*ty , tx'*0 + ty'*0 + 1*1
//
    float newValues[] = {0.0,0.0,0.0,0.0,0.0,0.0};

    newValues[A] = pPrime[A] * a() + pPrime[B] * c();
    newValues[B] = pPrime[A] * b() + pPrime[B] * d();
    newValues[C] = pPrime[C] * a() + pPrime[D] * c();
    newValues[D] = pPrime[C] * b() + pPrime[D] * d();
    newValues[TX] = pPrime[TX] * a() + pPrime[TY] * c() + tx();
    newValues[TY] = pPrime[TX] * b() + pPrime[TY] * d() + ty();

    a(newValues[A]);
    b(newValues[B]);
    c(newValues[C]);
    d(newValues[D]);
    tx(newValues[TX]);
    ty(newValues[TY]);

    return;
    }