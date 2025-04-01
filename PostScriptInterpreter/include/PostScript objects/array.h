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

#include <map>

#include "object.h"

#define A 0
#define B 1
#define C 2
#define D 3
#define TX 4
#define TY 5

class job;

   class array : public object {
   public:

      array(job *pJob,char *pszName);
      array(job *pJob,char *pszName,char *pszValues);
      array(job *pJob,char *pszName,long initialCount);
      array(job *pJob,long initialCount);
      array(job *pJob,char *pszName,long initialCount,char *pszValues);

      virtual ~array();

      void insert(object *);

      virtual void putElement(long index,object *);
      virtual object *getElement(long index);

      virtual void setValue(long index,POINT_TYPE v);

      void clear();

      bool find(long index) { return entries.find(index) != entries.end() ; };

      long size() { return (long)entries.size(); };

      long sizeFromString(char *pszValues);

      char *stringValueFromArray(char *pszValue,long oneBasedIndex);

   protected:

      std::map<long,object *> entries;

      friend class job;
   };
   
