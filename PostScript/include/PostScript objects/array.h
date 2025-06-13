// Copyright 2017 EnVisioNate LLC. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <map>

#include "object.h"

class job;

   class array : public object {
   public:

      array(job *pJob,char *pszName);
      array(job *pJob,char *pszName,objectType ot);
      array(job *pJob,char *pszName,char *pszValues);
      array(job *pJob,char *pszName,long initialCount);
      array(job *pJob,long initialCount);

      virtual ~array();

      void insert(object *);

      void put(long index,object *);

      object *get(long index);

      void clear();

      bool find(long index) { return entries.find(index) != entries.end() ; };

      long size() { return (long)entries.size(); };

   private:

      job *pJob;
      std::map<long,object *> entries;

   };
   
