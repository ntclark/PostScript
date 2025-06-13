// Copyright 2017 EnVisioNate LLC. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <stack>

#include "PostScript objects\object.h"

   class objectStack : public std::stack<object *> {
   public:

      object *top();
      object *pop();
      void push(object *);
  
   private:

   };