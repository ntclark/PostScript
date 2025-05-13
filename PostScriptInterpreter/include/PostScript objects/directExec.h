/*
Copyright 2025 EnVisioNate LLC

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the “Software”), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is the MIT License
*/

#pragma once

#include "PostScriptInterpreter.h"

   class directExec : public object {
   public:

      directExec(job *pJob,char *pszContents,void (__thiscall job::*pProc)()) : 
            object(pJob,pszContents,object::objectType::directExecutable,object::valueType::executableOperator,
                    object::valueClassification::simple,object::executableAttribute::executable), pOperator(pProc) {};

      directExec(job *pJob,char *pszContents,void (__thiscall AdobeType1Fonts::*pProc)()) : 
            object(pJob,pszContents,object::objectType::directExecutable,object::valueType::executableOperator,
                    object::valueClassification::simple,object::executableAttribute::executable),
                        pAdobeType1FontsOperator(pProc) {};

      void (__thiscall job::*Operator())() { return pOperator; };
      void (__thiscall AdobeType1Fonts::*AdobeType1FontsOperator())() { return pAdobeType1FontsOperator; };

   private:

      void (__thiscall job::*pOperator)();

      void (__thiscall AdobeType1Fonts::*pAdobeType1FontsOperator)();

   };