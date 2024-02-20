// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <stack>

#include "PostScript objects\dictionary.h"

   class dictionaryStack : public std::stack<dictionary *> {
   public:

      dictionary *top();
      dictionary *pop();

      dictionary *find(char *pszName);

      void push(dictionary *);
      void remove(dictionary *);

   private:

   };