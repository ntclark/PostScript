// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PostScript objects\procedure.h"

#include "job.h"

   procedure::procedure(job *pj,char *pStart,char *pEnd) :
      pJob(pj),isBound(false),pszStringRepresentation(NULL),
      object(pStart,pEnd,object::procedure,object::string)
   {
   return;
   }

   procedure::procedure(job *pj) :
      pJob(pj),isBound(false),pszStringRepresentation(NULL),
      object(object::procedure)
   {
   return;
   }


   procedure::procedure(job *pj,char *pStart,char **ppEnd) :
      pJob(pj),isBound(false),pszStringRepresentation(NULL),
      object(object::procedure)
   {
   
   pJob -> operatorMarkProcedureBegin();

   pJob -> execute(pStart,ppEnd);

   std::list<object *> newEntries;

   object *pObject = pJob -> pop();

   while ( object::mark != pObject -> ObjectType() ) {
      newEntries.insert(newEntries.end(),pObject);
      pObject = pJob -> pop();
   }

   for ( std::list<object *>::reverse_iterator it = newEntries.rbegin(); it != newEntries.rend(); it++ ) 
      insert(*it);

   pJob -> addProcedure(this);

   return;
   }


   procedure::~procedure() {
   entries.clear();
   if ( pszStringRepresentation )
      delete pszStringRepresentation;
   return;
   }

   void procedure::insert(object *p) {
   entries.insert(entries.end(),p);
   return;
   }

#if 0
   void procedure::add(object *p) {
   entries.insert(entries.end(),p);
   return;
   }
#endif

   void procedure::clear() {
   entries.clear();
   return;
   }

   void procedure::execute() {

   try {

   pJob -> execute(this);

   } catch ( syntaxerror *pse ) {
      char szMessage[2048];
      sprintf(szMessage,"\n\n%s\n\nThere was a %s exception while executing the procedure(%s):\n%s",pse -> Message(),pse -> ExceptionName(),Name(),Contents());
      pPStoPDF -> queueLog(szMessage);
      throw;
   }

   return;
   }


   void procedure::bind() {

   if ( isBound )
      return;

   try {

   pJob -> bindProcedure(this);

   isBound = true;

   } catch ( syntaxerror *pse ) {
      char szMessage[2048];
      sprintf(szMessage,"\n\n%s\n\nThere was a %s while binding the procedure(%s):\n%s",pse -> Message(),pse -> ExceptionName(),Name(),Contents());
      pPStoPDF -> queueLog(szMessage);
      throw;
   }

   return;
   }


   char *procedure::Contents(char *pszContents) {
   if ( pszContents )
      return object::Contents(pszContents);
   return ToString();
   }


   char *procedure::ToString() {

   if ( pszStringRepresentation )
      delete [] pszStringRepresentation;

//   pJob -> parseProcedure(this);

   long n = 0L;
   for ( std::list<object *>::iterator ih = entries.begin(); ih != entries.end(); ih++ )
      n += (long)strlen((*ih) -> ToString());

   n += 4 * (long)entries.size();
   n += 4;

   pszStringRepresentation = new char[n];

   memset(pszStringRepresentation,0,n * sizeof(char));

   n = sprintf(pszStringRepresentation,"{");

   for ( std::list<object *>::iterator ih = entries.begin(); ih != entries.end(); ih++ )
      n += sprintf(pszStringRepresentation + n," %s ",(*ih) -> ToString());

   n += sprintf(pszStringRepresentation + n,"}");

   return pszStringRepresentation;
   }