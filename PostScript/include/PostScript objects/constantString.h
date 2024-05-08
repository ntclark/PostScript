#pragma once

// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "string.h"

   class constantString : public string {
   public:

      constantString(job *pJob,char *pStart,char *pEnd);

      virtual ~constantString();

   private:

   };