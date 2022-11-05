// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "PostScript objects\object.h"

   class resource : public object {
   public:

      resource(object *pCategory,object *pInstance,object *pName);
      ~resource();

      virtual char *Name(char *pszNewName = NULL);

   private:

      object *pCategory;
      object *pInstance;
      object *pName;

   };
