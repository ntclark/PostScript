// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PostScript objects\matrix.h"

   matrix::matrix() :
      object(object::matrix,object::container),
      a(1.0),
      b(0.0),
      c(0.0),
      d(1.0),
      tx(0.0),
      ty(0.0)
   {
   }

   matrix::matrix(matrix &rhs) {
   memcpy(this,&rhs,sizeof(matrix));
   return;
   }

   matrix::~matrix() {
   return;
   }

   void matrix::SetValue(long index,float value) {
   v[index] = value;
   return;
   }


   void matrix::concat(matrix *pSource) {
   concat(pSource -> v);
   return;
   }

   void matrix::concat(XFORM &winXForm) {
   concat(reinterpret_cast<float *>(&winXForm));
   return;
   }

   void matrix::concat(float *pValues) {
//
//   |  a'   b'   0  |   | a  b  0 |    | a' * a + b' * c + 0 * tx   , a' * b + b' * d + 0 * ty   , a' * 0 + b' * 0 + 0 * 1
//   |  c'   d'   0  | * | c  d  0 | =  | c' * a + d' * c + 0 * tx   , c' * b + d' * d + 0 * ty   , c' * 0 + d' * 0 + 0 * 1
//   |  tx'  ty'  1  |   | tx ty 1 |    | tx' * a + ty' * c + 1 * tx , tx' * b + ty' * d + 1 * ty , tx' * 0 + ty' * 0 + 1 * 1
// 
   float newValues[6];

   newValues[0] = pValues[0] * a + pValues[1] * c;
   newValues[1] = pValues[0] * b + pValues[1] * d;
   newValues[2] = pValues[2] * a + pValues[3] * c;
   newValues[3] = pValues[2] * b + pValues[3] * d;
   newValues[4] = pValues[4] * a + pValues[5] * c + tx;
   newValues[5] = pValues[4] * b + pValues[5] * d + ty;

   memcpy(v,newValues,6 * sizeof(float));

   return;
   }