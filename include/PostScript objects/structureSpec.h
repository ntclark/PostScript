// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "object.h"

   class structureSpec : public object {
   public:

      structureSpec(char *pStart,char *pEnd);
      ~structureSpec();

   };