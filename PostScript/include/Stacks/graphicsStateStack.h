// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <stack>

class graphicsState;
class job;

   class graphicsStateStack : public std::stack<graphicsState *> {
   public:
      graphicsStateStack(job *pJob);
      ~graphicsStateStack();

      void save();
      graphicsState *restore();
      graphicsState *current();

      job *pJob{NULL};

   private:

   };