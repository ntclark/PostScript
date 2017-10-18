// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PostScript objects\name.h"

   name::name(char *pszValue) :
      object(pszValue,object::name)
   {
   }

   name::~name() {
   return;
   }

   