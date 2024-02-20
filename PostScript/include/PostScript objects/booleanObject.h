// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "object.h"

   class booleanObject : public object {
   public:

      booleanObject(job *pJob,char *pszName);
      ~booleanObject();

   private:

      boolean value;

   };