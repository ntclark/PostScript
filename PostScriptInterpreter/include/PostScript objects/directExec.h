// Copyright 2017 EnVisioNate LLC All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "PostScript.h"

   class directExec : public object {
   public:

      directExec(job *pJob,char *pszContents,void (__thiscall job::*pProc)()) : 
            object(pJob,pszContents,object::objectType::directExecutable,object::valueType::executableOperator,
                    object::valueClassification::simple,object::executableAttribute::executable), pOperator(pProc) {};

      ~directExec() {};

      void (__thiscall job::*Operator())() { return pOperator; };

   private:

      void (__thiscall job::*pOperator)();

   };