// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "object.h"

   class string : public object {
   public:

      string(job *pJob,char *pszValue); 
      string(job *pJob,char *pStart,char *pEnd);
      ~string();

      virtual object *get(long index);
      virtual void put(long index,object *);

   private:

   };