// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "array.h"

   class job;

   class colorSpace : public array {
   public:

      colorSpace(job *,object *);
      colorSpace(job *,char *pszName);
      ~colorSpace();

      long ParameterCount();

   private:

      long parameterCount;

   };
