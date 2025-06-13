// Copyright 2017 EnVisioNate LLC. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <windows.h>
#include <float.h>

   class object {
   public:

      enum objectType {
         atom = 0,
         procedure = 1,
         dictionary = 2,
         structureSpec = 3,
         directExecutable = 4,
         matrix = 5,
         array = 6,
         packedarray = 7,
         number = 8,
         logical = 9,
         mark = 10,
         null = 11,
         save = 12,
         pattern = 13,
         colorSpace = 14,
         font = 15,
         name = 16,
         literal = 17,
         resource = 18
      };

      enum valueType {
         invalidValueType = 99,
         unspecified = 100,
         container = 101,
         string = 102,
         character = 103,
         integer = 104,
         real = 105,
         radix = 106,
         executableProcedure = 107
      };

      object();

      object(char c);
      object(char *pszContents);
      object(char *pszContents,objectType theType);
      object(char *pszContents,objectType theType,valueType theValueType);

      object(char *pStart,char *pEnd);
      object(char *pStart,char *pEnd,objectType theType);
      object(char *pStart,char *pEnd,objectType theType,valueType theValueType);

      object(objectType theType);
      object(objectType,valueType);
      object(long value);
      object(double value);

      virtual ~object();

      virtual void put(long index,object *);
      virtual object *get(long index);
      virtual void execute();
      virtual char *ToString();

      virtual char *Name(char *pszNewName = NULL);

      virtual char *Contents(char *pszContents = NULL);

      char *TypeName();
      char *ValueTypeName();
      long IntValue(long value = -LONG_MAX);
      double DoubleValue(double value = -DBL_MAX);
      float FloatValue(float value = -FLT_MAX);
      double Value();

      void SetExecuteOnly(bool v) { isExecuteOnly = v; };
      bool IsExecuteOnly() { return isExecuteOnly; };

      enum objectType ObjectType() { return theType; };
      enum valueType ValueType( enum valueType vt = invalidValueType ) { if ( vt != invalidValueType ) theValueType = vt; return theValueType; };

   private:

      void parseValue();

      char *pszContents;
      char *pszName;

      union {
         long intValue;
         double doubleValue;
      };

      objectType theType;
      valueType theValueType;

      bool isExecuteOnly;

   };
