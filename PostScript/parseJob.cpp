// Copyright 2017 EnVisioNate LLC. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utilities.h"

#include "job.h"
#include "PostScript objects\directExec.h"
#include "PostScript objects\literal.h"
#include "PostScript objects\string.h"

   long job::parseJob(bool useThread) {

   pPStoPDF -> clearLog();

   if ( useThread ) {
      unsigned int threadAddr;
      HANDLE hJobThread = (HANDLE)_beginthreadex(NULL,65536,job::_execute,(void *)this,CREATE_SUSPENDED,&threadAddr);
      ResumeThread(hJobThread);
   } else
      _execute((void *)this);

   return 0L;
   }

   unsigned int __stdcall job::_execute(void *pvThis) {

   job *pThis = reinterpret_cast<job *>(pvThis);

   try {

   pThis -> execute((char *)(pThis -> pStorage));

   } 

   catch ( PStoPDFException *pe ) {
      char szMessage[1024];
      sprintf(szMessage,"\n\nA %s exception occurred: %s\n",pe -> ExceptionName(),pe -> Message());
      pPStoPDF -> queueLog(szMessage);
      pPStoPDF -> queueLog("\nStack Trace:\n");
      pThis -> operatorPstack();
   }

   catch ( ... ) {

      pPStoPDF -> queueLog("\n\nThe job is cancelled due to errors");
      _endthread();

   }

   return 0L;
   }


   long job::execute(procedure *pProcedure) {

   for ( std::list<object *>::iterator it = pProcedure -> entries.begin(); it != pProcedure -> entries.end(); it++ ) {
      push(*it);
      execute();
   }

   return 0L;
   }


   bool job::seekOperator() {

   object *pKey = top();

   if ( object::number == pKey -> ObjectType() || object::literal == pKey -> ObjectType() )
      return false;

   operatorWhere();

   if ( top() == pFalseConstant ) {
      pop();
      push(pKey);
      return false;
   }

   pop();

   dictionary *pDictionary = reinterpret_cast<dictionary *>(pop());

   object *pObject = pDictionary -> retrieve(pKey -> Name());

   if ( object::directExecutable == pObject -> ObjectType() ) {
      push(pObject);
      return true;
   }

   push(pKey);

   return false;
   }


   bool job::seekDefinition() {

   object *pKey = top();

   if ( object::number == pKey -> ObjectType() || object::literal == pKey -> ObjectType() )
      return true;

   operatorWhere();

   if ( top() == pFalseConstant ) {
      pop();
      push(pKey);
      return false;
   }

   pop();

   dictionary *pDictionary = reinterpret_cast<dictionary *>(pop());

   push(pDictionary -> retrieve(pKey -> Name()));

   return false;
   }


   long job::execute() {

   object *pInitialObject = top();

   bool isLiteral = seekDefinition();
   
   if ( isLiteral )
      return 0L;

   object *pObject = pop();

   switch ( pObject -> ObjectType() ) {

   case object::directExecutable: {
      directExec *pDirectExec = reinterpret_cast<directExec *>(pObject);
      void (__thiscall job::*pOperator)() = pDirectExec -> Procedure();
      (this ->* pOperator)();
      return 1L;
      }

   default: 
      if ( object::procedure == pObject -> ObjectType() && pInitialObject != pObject ) {
/*
      3.5.3 Deferred Execution
   
      ...

      Why did the interpreter treat the procedure as data in the first line of the example
      but execute it in the second, despite the procedure having the executable attribute
      in both cases? There is a special rule that determines this behavior: An executable
      array or packed array encountered directly by the interpreter is treated as data
      (pushed on the operand stack), but an executable array or packed array encountered
      indirectly—as a result of executing some other object, such as a name or an
      operator—is invoked as a procedure.
*/
         procedure *pProcedure = reinterpret_cast<procedure *>(pObject);
         pProcedure -> execute();
      } else
         push(pObject);
   }

   return 0L;
   }


   long job::execute(char *pszString,char **ppEnd,long length) {

   char *p = pszString;
   char *pNext = p;
   char *pLogStart = NULL;

   if ( -1L != length ) {
      char *pEnd = p + length;
      while ( p < pEnd ) {
         if ( *p == '\0' ) 
            *p = ' ';
         p++;
      }
      p = pszString;
   }

   try {

   do {

      pLogStart = p;

      ADVANCE_THRU_WHITE_SPACE(p)

      if ( ! *p )
         break;

      char *pCollectionDelimiter = collectionDelimiterPeek(p,&pNext);
   
      char *pDelimiter = NULL;

      if ( ! pCollectionDelimiter )
         pDelimiter = (char *)delimiterPeek(p,&pNext);

      if ( ! loggingOff && ! ppEnd )
         pPStoPDF -> queueLog(pLogStart,pNext);

      pLogStart = pNext;

      if ( pDelimiter ) {

         (this ->* tokenProcedures[HashCode((char *)pDelimiter)])(pNext,&pNext);

      } else {

         object *po = NULL;

         if ( pCollectionDelimiter && *pCollectionDelimiter == '{' ) {

            po = new procedure(this,pNext,&pNext);

            push(po);

         } else {

            if ( ppEnd && pCollectionDelimiter && *pCollectionDelimiter == '}' ) {

               *ppEnd = pNext;

               return 0L;

            }

            if ( pCollectionDelimiter )
               po = new object(pCollectionDelimiter);
            else
               po = new object(parseObject(pNext,&pNext));

            push(po);

            if ( ! ppEnd ) {

               resolve();

               po = top();

               if ( object::procedure == po -> ObjectType() ) {
                  pop();
                  execute(reinterpret_cast<procedure *>(po));
               } else  
                  execute();

//
//NTC: 02-19-2012: Inline images start with the BI operator, and end with the EI operator, with an intervening
// ID "operator" that contains the image data after the operator.
// Until inline images are implemented completely and correctly, I am going to parse from any ID operator, through to the EI operator
// and ignore the inline image image data.
//
               if ( 2 == strlen(po -> Contents()) && 0 == strncmp(po -> Contents(),"ID",2) ) {

                  while ( ! ( 'E' == pNext[0] && 'I' == pNext[1] && ( 0x0A == pNext[2] || 0x0D == pNext[2] || ' ' == pNext[2] ) ) )
                     pNext++;

                  pNext += 3;

               }

            }

         }

      }

      p = pNext;

      if ( ! loggingOff && ! ppEnd )
         pPStoPDF -> queueLog(pLogStart,pNext);

      pLogStart = pNext;

   } while ( p );
   
   if ( ! loggingOff )
      pPStoPDF -> queueLog(pLogStart,pNext);

   } catch ( syntaxerror se ) {
      throw;
   }

   loggingOff--;

   loggingOff = max(0,loggingOff);

   return 0;
   }


   void job::bindProcedure(procedure *pProcedure) {

   std::list<object *> newEntries;

   for ( std::list<object *>::iterator it = pProcedure -> entries.begin(); it != pProcedure -> entries.end(); it++ ) {
      if ( object::procedure == (*it) -> ObjectType() ) {
         procedure *pProcedure = reinterpret_cast<procedure *>(*it);
         pProcedure -> bind();
         newEntries.insert(newEntries.end(),pProcedure);
      } else {
         push(*it);
         seekOperator();
         newEntries.insert(newEntries.end(),pop());
      }
   }

   pProcedure -> entries.clear();

   for ( std::list<object *>::iterator it = newEntries.begin(); it != newEntries.end(); it++ )
      pProcedure -> entries.insert(pProcedure -> entries.end(),(*it));

   return;
   }

   char *job::collectionDelimiterPeek(char *pStart,char **ppEnd) {
   static char result[32];
   memset(result,0,sizeof(result));
   ADVANCE_THRU_WHITE_SPACE(pStart)
   if ( ! pStart )
      return NULL;
   for ( long k = 0; 1; k++ ) {
      if ( ! psCollectionDelimiters[k][0] )
         return NULL;
      if ( *pStart == psCollectionDelimiters[k][0] ) {
         if ( 2 == psCollectionDelimiterLength[k] ) {
            if ( psCollectionDelimiters[k][1] == *(pStart + 1) ) {
               *ppEnd = pStart + 2;
               result[0] = psCollectionDelimiters[k][0];
               result[1] = psCollectionDelimiters[k][1];
               return result;
            }
         } else {
            *ppEnd = pStart + 1;
            result[0] = psCollectionDelimiters[k][0];
            return result;
         }
      }
   }
   return NULL;
   }


   char *job::delimiterPeek(char *pStart,char **ppEnd) {
   static char result[32];
   memset(result,0,sizeof(result));
   ADVANCE_THRU_WHITE_SPACE(pStart)
   if ( ! pStart )
      return NULL;
   for ( long k = 0; 1; k++ ) {
      if ( ! psDelimiters[k][0] )
         return NULL;
      if ( *pStart == psDelimiters[k][0] ) {
         if ( 2 == psDelimiterLength[k] ) {
            if ( psDelimiters[k][1] == *(pStart + 1) ) {
               *ppEnd = pStart + 2;
               result[0] = psDelimiters[k][0];
               result[1] = psDelimiters[k][1];
               return result;
            }
         } else {
            *ppEnd = pStart + 1;
            result[0] = psDelimiters[k][0];
            return result;
         }
      }
   }
   return NULL;
   }


   void job::parseLiteralName(char *pStart,char **pEnd) {
   char *p = pStart;
   ADVANCE_THRU_WHITE_SPACE(p)
   char *pBegin = p;
   ADVANCE_TO_END(p)
   push(new literal(pBegin,p));
   ADVANCE_THRU_WHITE_SPACE(p)
   *pEnd = p;
   return;
   }


   char * job::parseObject(char *pStart,char **pEnd) {
   static char result[MAX_PATH];
   memset(result,0,sizeof(result));
   char *p = pStart;
   ADVANCE_THRU_WHITE_SPACE(p)
   char *pBegin = p;
   p++;
   ADVANCE_TO_END(p)
   *pEnd = p;
   strncpy(result,(char *)pBegin,p - pBegin);
   return result;
   }



   void job::parseStructureSpec(char *pStart,char **ppEnd) {
   char *p = pStart;
   ADVANCE_THRU_EOL(p)
   *ppEnd = p;
   structureSpecs.insert(structureSpecs.end(),new structureSpec(pStart,*ppEnd));
   return;
   }


   void job::parseComment(char *pStart,char **ppEnd) {
   char *p = pStart;
   ADVANCE_THRU_EOL(p)
   *ppEnd = p;
   commentStack.insert(commentStack.end(),new comment(pStart,*ppEnd));
   return;
   }


   void job::parseString(char *pStart,char **ppEnd) {
   char *p = pStart;
   *ppEnd = NULL;

   parse(STRING_DELIMITER_BEGIN,STRING_DELIMITER_END,p,ppEnd);

   if ( *ppEnd ) {

      char *px = strchr(p,'\\');

      if ( px && px < *ppEnd ) {

         long n = (long)(*ppEnd - p + 1);
         char *pTemp = new char[n];
         char *pTarget = new char[n];
         memset(pTemp,0,n * sizeof(char));
         memset(pTarget,0,n * sizeof(char));

         strncpy(pTemp,p,n - 1);
         px = strchr(pTemp,'\\');
         char *pBegin = pTemp;
         while ( px ) {
            *px = '\0';
            strcpy(pTarget + strlen(pTarget),pBegin);
            pBegin = px + 1;
            if ( '\\' == *pBegin ) {
               pTarget[strlen(pTarget)] = '\\';
               pBegin++;
            }
            px = strchr(pBegin,'\\');
         }

         strcpy(pTarget + strlen(pTarget),pBegin);

         push(new string(pTarget));

         delete [] pTemp;
         delete [] pTarget;

      } else
         push(new string(p,*ppEnd));

      *ppEnd = *ppEnd + 1;
   }
   return;
   }


   void job::parseHexString(char *pStart,char **ppEnd) {
   char *p = pStart;
   *ppEnd = NULL;

   parse(HEX_STRING_DELIMITER_BEGIN,HEX_STRING_DELIMITER_END,p,ppEnd);

   if ( *ppEnd ) {
      char c = **ppEnd;
      **ppEnd = '\0';

      long n = (DWORD)strlen((char *)p) + 1;
      char *pszNew = new char[n];
      pszNew[n - 1] = '\0';
      strcpy(pszNew,(char *)p);

      ASCIIHexDecodeInPlace(pszNew);

      push(new string(pszNew));

      delete [] pszNew;

      **ppEnd = c;
      *ppEnd = *ppEnd + 1;
   }

   return;
   }


   void job::parseHex85String(char *pStart,char **pEnd) {
   return;
   }
