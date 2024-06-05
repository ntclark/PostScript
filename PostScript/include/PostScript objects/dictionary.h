// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <list>
#include <map>

class job;

#include "object.h"

   class dictionary : public object {
   public:

      dictionary(job *,char *);
      dictionary(job *,char *,enum objectType);
      dictionary(job *,enum objectType);

      virtual ~dictionary();

      void insert(char *,void (__thiscall job::*)());
      void insert(char *,object *);
      void insert(char *,char *);

      void put(char *psz,object *p) { insert(psz,p); };

      object *retrieve(char *);
      char *getKey(long index);
      boolean exists(char *);

      void remove(char *pszKey);

      long size() { return (long)entries.size(); };

   private:

      job *pJob;
      std::map<long,object *> entries;
      std::map<long,void (__thiscall job::*)()> operators;
      std::list<char *> keys;

   };
