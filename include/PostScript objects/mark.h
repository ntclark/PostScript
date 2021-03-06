// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "object.h"

   class mark : public object {
   public:

      enum markType {
         array = 1,
         dictionary = 2,
         procedure = 3,
         unspecified = 4
      };

      mark(markType);
      ~mark();

   private:

      markType theType;
   
   };
