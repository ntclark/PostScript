// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "Stacks\objectStack.h"

#include "error.h"

   void objectStack::push(object *pObject) {
   std::stack<object *>::push(pObject);
   return;
   }


   object *objectStack::top() {
   if ( 0 == size() ) 
      throw stackunderflow("");
   return std::stack<object *>::top();
   }


   object *objectStack::pop() {
   if ( 0 == size() ) 
      throw stackunderflow("There was an underflow in the PostScript operand stack");
   object *pObject = std::stack<object *>::top();
   std::stack<object *>::pop();
   return pObject;
   }