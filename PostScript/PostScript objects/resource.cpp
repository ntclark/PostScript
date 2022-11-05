// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PostScript objects\resource.h"

   resource::resource(object *pc,object *pi,object *pn) : 
      object(objectType::resource),
      pCategory(pc),pInstance(pi),pName(pn) {
   return;
   }


   resource::~resource() {
   return;
   }


   char *resource::Name(char *pszNewName) {
   return pName -> Name();
   }
