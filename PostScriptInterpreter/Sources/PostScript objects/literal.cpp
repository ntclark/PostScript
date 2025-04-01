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

#include "PostScript objects\literal.h"

   literal::literal(job *pJob,char *pValue) :
      literal(pJob,pValue,NULL) {}

   literal::literal(job *pJob,char *pStart,char *pEnd) :
      pszStringRepresentation(NULL),
      object(pJob,pStart,pEnd,object::objectType::literal,object::valueType::string,object::valueClassification::simple)
   {
   long n = (DWORD)strlen(Contents()) + 2;
   pszStringRepresentation = new char[n];
   memset(pszStringRepresentation,0,n * sizeof(char));
   pszStringRepresentation[0] = '/';
   strcpy(pszStringRepresentation + 1,Contents());
   return;
   }

   literal::~literal() {
   if ( pszStringRepresentation )
      delete []pszStringRepresentation;   
   return;
   }

   char *literal::ToString() {
   if ( pszStringRepresentation )
      delete [] pszStringRepresentation;
   long n = (DWORD)strlen(Contents()) + 4;
   pszStringRepresentation = new char[n];
   memset(pszStringRepresentation,0,n * sizeof(char));
   pszStringRepresentation[0] = '/';
   strcpy(pszStringRepresentation + 1,Contents());
   return pszStringRepresentation;
   }
   