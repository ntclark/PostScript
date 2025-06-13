// Copyright 2017 EnVisioNate LLC. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PostScript objects\structureSpec.h"

   structureSpec::structureSpec(char *pStart,char *pEnd) :
      object(pStart,pEnd,object::structureSpec,object::string)
   {
   }

   structureSpec::~structureSpec() {
   }   