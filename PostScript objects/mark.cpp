// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PostScript objects\mark.h"

   mark::mark(markType t) :
      theType(t),
      object("",object::mark)
   {
   return;
   }

   mark::~mark() {
   return;
   }