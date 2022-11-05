// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PostScript.h"

   class comment {
   public:

      comment(char *ps,char *pe) : pStart(ps), pEnd(pe) {};
      ~comment() {};

   public:

      char *pStart,*pEnd;

   };
   