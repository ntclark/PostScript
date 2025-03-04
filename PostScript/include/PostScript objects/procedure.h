// Copyright 2017 EnVisioNate LLC All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <vector>

class job;

#include "object.h"

   class procedure : public object {
   public:

      procedure(job *p,char *pStart,char *pEnd);
      procedure(job *p,char *pStart,char **ppEnd);
      procedure(job *p);

      virtual ~procedure();

      virtual char *Contents(char *pszContents = NULL);

      void insert(object *);

      virtual void putElement(long index,object *pObject);

      char *ToString();
      void clear();

      void execute();
      void bind();

      bool IsBound() { return isBound; };

   private:

      procedure(job *pj,char *pStart,char *pEnd,char **ppEnd);

      bool isBound{false};
      std::vector<object *> entries;
      char *pszStringRepresentation{NULL};
      procedure *pContainingProcedure{NULL};

      friend class job;

   };
   