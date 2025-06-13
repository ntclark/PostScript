// Copyright 2017 EnVisioNate LLC. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <process.h>

#include "PostScript objects\object.h"

   object::object() :
      pszContents(NULL),
      pszName(NULL),
      doubleValue(-DBL_MAX),
      isExecuteOnly(false),
      theType(null),
      theValueType(unspecified)
   {
   return;
   }


   object::object(char c) :
      doubleValue(-DBL_MAX),
      isExecuteOnly(false),
      theType(atom),
      theValueType(character)
   {

   pszContents = new char[2];
   pszContents[0] = c;
   pszContents[1] = '\0';

   pszName = new char[2];
   pszName[0] = c;
   pszName[1] = '\0';

   return;
   }

   object::object(char *contents) :
      doubleValue(-DBL_MAX),
      isExecuteOnly(false),
      theType(null),
      theValueType(unspecified)
   {

   pszContents = new char[strlen(contents) + 1];
   pszContents[strlen(contents)] = '\0';
   strcpy(pszContents,contents);

   pszName = new char[strlen(contents) + 1];
   pszName[strlen(contents)] = '\0';
   strcpy(pszName,contents);

   parseValue();

   return;
   }


   object::object(char *contents,objectType t) :
      intValue(-LONG_MAX),
      isExecuteOnly(false),
      theType(t),
      theValueType(string)
   {

   long n = (DWORD)strlen(contents) + 1;

   pszContents = new char[n];
   pszContents[n - 1] = '\0';
   strcpy(pszContents,contents);

   pszName = new char[n];
   pszName[n - 1] = '\0';
   strcpy(pszName,contents);

   parseValue();

   return;
   }


   object::object(objectType t,valueType vt) :
      pszContents(NULL),
      pszName(NULL),
      doubleValue(-DBL_MAX),
      isExecuteOnly(false),
      theType(t),
      theValueType(vt)
   {
   return;
   }


   object::object(char *contents,objectType t,valueType vt) :
      intValue(-LONG_MAX),
      isExecuteOnly(false),
      theType(t),
      theValueType(vt)
   {

   long n = (DWORD)strlen(contents) + 1;

   pszContents = new char[n];
   pszContents[n - 1] = '\0';
   strcpy(pszContents,contents);

   pszName = new char[n];
   pszName[n - 1] = '\0';
   strcpy(pszName,contents);

   parseValue();

   return;
   }


   object::object(char *pStart,char *pEnd) :
      doubleValue(-DBL_MAX),
      isExecuteOnly(false),
      theType(null),
      theValueType(unspecified)
   {

   long n = (long)(pEnd - pStart);

   pszContents = new char[n + 1];
   pszContents[n] = '\0';
   memcpy(pszContents,(char *)pStart,n);

   pszName = new char[n + 1];
   pszName[n] = '\0';
   memcpy(pszName,(char *)pStart,n);

   parseValue();

   return;
   }


   object::object(char *pStart,char *pEnd,objectType t) :
      intValue(-LONG_MAX),
      isExecuteOnly(false),
      theType(t),
      theValueType(string)
   {

   long n = (long)(pEnd - pStart + 1);

   pszContents = new char[n];
   pszContents[n - 1] = '\0';
   memcpy((BYTE *)pszContents,pStart,n - 1);

   pszName = new char[n];
   pszName[n - 1] = '\0';
   memcpy((BYTE *)pszName,pStart,n - 1);

   parseValue();

   return;
   }


   object::object(char *pStart,char *pEnd,objectType t,valueType vt) :
      intValue(-LONG_MAX),
      isExecuteOnly(false),
      theType(t),
      theValueType(vt)
   {

   long n = (long)(pEnd - pStart + 1);

   pszContents = new char[n];
   pszContents[n - 1] = '\0';
   memcpy((BYTE *)pszContents,pStart,n - 1);

   pszName = new char[n];
   pszName[n - 1] = '\0';
   memcpy((BYTE *)pszName,pStart,n - 1);

   parseValue();

   return;
   }


   object::object(objectType t) :
      intValue(-LONG_MAX),
      isExecuteOnly(false),
      theType(t),
      theValueType(unspecified)
   {
   pszContents = NULL;
   pszName = NULL;
   return;
   }


   object::object(long value) :
      intValue(value),
      isExecuteOnly(false),
      theType(number),
      theValueType(integer)
   {

   pszContents = new char[64];
   memset(pszContents,0,64 * sizeof(char));
   sprintf(pszContents,"%ld",intValue);

   pszName = new char[64];
   memset(pszName,0,64 * sizeof(char));
   sprintf(pszName,"%ld",intValue);

   return;
   }


   object::object(double value) :
      doubleValue(value),
      isExecuteOnly(false),
      theType(number),
      theValueType(real)
   {

   pszContents = new char[64];
   memset(pszContents,0,64 * sizeof(char));
   sprintf(pszContents,"%lf",doubleValue);

   pszName = new char[64];
   memset(pszName,0,64 * sizeof(char));
   sprintf(pszName,"%lf",doubleValue);

   return;
   }


   object::~object() {
   delete [] pszContents;
   delete [] pszName;
   return;
   }

   char *object::Contents(char *pszNewContents) { 
   if ( pszNewContents ) {
      if ( pszContents )
         delete [] pszContents;
      pszContents = new char[strlen(pszNewContents) + 1];
      strcpy(pszContents,pszNewContents);
   }
   return pszContents;
   }

   char *object::Name(char *pszNewName) {

   if ( NULL == pszNewName )
      return pszName;

   if ( pszName )
      delete [] pszName;

   pszName = new char[strlen(pszNewName) + 1];
   pszName[strlen(pszNewName)] = '\0';
   strcpy(pszName,pszNewName);

   return pszName;
   }


   void object::parseValue() {

   if ( ! pszContents )
      return;

   char *p = pszContents;

   if ( ! *p )
      return;

   bool isReal = false;
   long radixCount = 0L;
   long digitCount = 0L;
   long preRadixDigitCount = 0L;

   while ( *p ) {

      if ( ( 'a' <= *p && *p <= 'z' ) || ( 'A' <= *p && *p <= 'Z' ) ) {

         if ( 0 == radixCount )
            return;

      } else if ( '0' <= *p && *p <= '9'  ) {

         digitCount++;
         if ( 0 == radixCount ) {
            preRadixDigitCount;
            if ( 3 == preRadixDigitCount )
               return;
         }

      } else if ( '.' == *p ) {

         isReal = true;

      } else if ( '#' == *p ) {

         if ( radixCount )
            return;

         radixCount++;

      } else 

         if ( ! ( '-' == *p || '+' == *p || 0x0A == *p || 0x0D == *p ) ) 
            return;

      p++;

   }

   if ( ! digitCount )
      return;

   if ( theType == null )
      theType = number;

   if ( isReal ) {
      doubleValue = atof(pszContents);
      if ( ! ( theValueType == object::string ) )
         theValueType = object::real;
   } else if ( 1 == radixCount ) {
      char *p = strchr(pszContents,'#');
      *p = '\0';
      long base = atol(pszContents);
      *p = '#';
      p = p + 1;
      intValue = strtoul(p,NULL,base);
      if ( ! ( theValueType == object::string ) )
         theValueType = object::radix;
   } else {
      intValue = atol(pszContents);
      if ( ! ( theValueType == object::string ) )
         theValueType = object::integer;
   } 

   return;
   }


   long object::IntValue(long value) { 
   if ( -LONG_MAX == value ) {
      if ( object::integer == theValueType )
         return intValue; 
      return atol(pszContents);
   }
   intValue = value; 
   sprintf(pszContents,"%ld",value); 
   theType = object::number;
   theValueType = object::integer;
   return intValue;
   }


   double object::DoubleValue(double value) {
   if ( -DBL_MAX == value ) {
      if ( object::real == theValueType )
         return doubleValue; 
      return atof(pszContents);
   }
   doubleValue = value; 
   sprintf(pszContents,"%lf",value); 
   theType = object::number;
   theValueType = object::real;
   return doubleValue;
   }

   float object::FloatValue(float value) {
   if ( -FLT_MAX == value ) 
      return (float)DoubleValue();
   return (float)DoubleValue((double)value);
   }


   double object::Value() {
   if ( object::number == theType ) {
      if ( object::integer == theValueType )
         return (double)intValue;
      return doubleValue;
   }
   return atof(pszContents);
   }
      
   char *object::TypeName() {

   switch ( theType ) {
   case atom:
      return "atom";
   case procedure:
      return "procedure";
   case dictionary:
      return "dictionary";
   case structureSpec:
      return "structureSpec";
   case directExecutable:
      return "operand";
   case matrix:
      return "matrix";
   case array:
      return "array";
   case packedarray:
      return "packedarray";
   case number:
      return "number";
   case logical:
      return "logical";
   case mark:
      return "mark";
   case null:
      return "null";
   case save:
      return "save";
   case pattern:
      return "pattern";
   case colorSpace:
      return "colorSpace";
   case font:
      return "font";
   case resource:
      return "resource";
   }
   return "";
   }

   char *object::ValueTypeName() {
   switch ( theValueType ) {
   case unspecified:
      return "unspecified";
   case container:
      return "container";
   case string:
      return "string";
   case character:
      return "character";
   case integer:
      return "integer";
   case real:
      return "real";
   case radix:
      return "radix";
   }
   return "";
   }

   void object::put(long index,object *) {
   MessageBox(NULL,"put is not implemented on a subclass of object. This is an error condition, job parsing is stopped","",MB_ICONEXCLAMATION);
   _endthread();
   return;
   }

   object *object::get(long index) {
   MessageBox(NULL,"get is not implemented on a subclass of object. This is an error condition, job parsing is stopped","",MB_ICONEXCLAMATION);
   _endthread();
   return NULL;
   }

   void object::execute() {
   MessageBox(NULL,"execute is not implemented on a subclass of object. This is an error condition, job parsing is stopped","",MB_ICONEXCLAMATION);
   _endthread();
   return;
   }

   char *object::ToString() {
   return Contents();
   }