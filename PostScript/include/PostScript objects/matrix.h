// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "object.h"

   class matrix : public object {
   public:
      
      matrix();
      matrix(matrix &);
      ~matrix();

      void SetValue(long index,float value);

      virtual void concat(matrix *);
      virtual void concat(float *);
      virtual void concat(XFORM &);

   private:

      union {
         struct { float a,b,c,d,tx,ty; };
         float v[6];
      };

   };