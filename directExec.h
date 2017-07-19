
#pragma once

#include "PostScript.h"

   class directExec : public object {
   public:

      directExec(char *pszContents,void (__thiscall job::*pProc)()) : object(pszContents,object::directExecutable), pProcedure(pProc) {};
      ~directExec() {};

      void (__thiscall job::*Procedure())() { return pProcedure; };

   private:

      void (__thiscall job::*pProcedure)();

   };