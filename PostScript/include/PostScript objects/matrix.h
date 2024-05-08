// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "object.h"
#include "PostScript objects/array.h"

   class matrix : public array {
   public:
      
      matrix(job *pJob);
      ~matrix();

      void SetValue(long index,float value);

      virtual void concat(matrix *);
      virtual void concat(float *);
      virtual void concat(XFORM &);

      virtual char *ToString();

      float a();
      float b();
      float c();
      float d();
      float tx();
      float ty();

      void a(float v);
      void b(float v);
      void c(float v);
      void d(float v);
      void tx(float v);
      void ty(float v);

   };