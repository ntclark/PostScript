// Copyright 2017 EnVisioNate LLC All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PostScript objects\booleanObject.h"

   booleanObject::booleanObject(job *pJob,char *psz) :
      object(pJob,psz,object::objectType::logical,object::valueType::trueOrFalse,object::valueClassification::simple)
   {
   if ( 0 == _stricmp(psz,"true") )
      value = true;
   else
      value = false;
   return;
   }


   booleanObject::~booleanObject() {
   return;
   }