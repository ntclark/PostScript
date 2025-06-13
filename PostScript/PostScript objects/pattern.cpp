// Copyright 2017 EnVisioNate LLC. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PostScript objects\pattern.h"

   pattern::pattern(job *pj) :
      dictionary(pj,object::pattern)
   {
   return;
   }

   pattern::~pattern() {
   return;
   }