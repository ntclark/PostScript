// Copyright 2017 EnVisioNate LLC All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <deque>

#include "PostScript objects\dictionary.h"

   class dictionaryStack : public std::deque<dictionary *> {
   public:

      dictionary *top();
      dictionary *pop();
      dictionary *bottom();

      void setCurrent(dictionary *);

      dictionary *find(char *pszName);

      void remove(dictionary *);

   private:

   };