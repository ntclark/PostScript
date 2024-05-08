// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PostScript objects\colorSpace.h"

#include "job.h"

   colorSpace::colorSpace(job *pj,object *po) :
      parameterCount(0),
      array(pj,po -> Name())
   {
   if ( object::matrix == po -> ObjectType() ) {
      __debugbreak();
   }
   parameterCount = size() - 1;
   return;
   }

   colorSpace::colorSpace(job *pj,char *pszName) :
      parameterCount(0),
      array(pj,pszName)
   {
   return;
   }


   colorSpace::~colorSpace() {
   return;
   }


   long colorSpace::ParameterCount() {
   if ( parameterCount )
      return size() - 1;
   if ( 0 == strcmp(Name(),"DeviceGray") )
      return 1;
   if ( 0 == strcmp(Name(),"DeviceRGB") ) 
      return 3;
//_asm {
//int 3;
//}
   return 3;
   }

