// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "Stacks\dictionaryStack.h"

#include "error.h"

   void dictionaryStack::push(dictionary *pObject) {
   std::stack<dictionary *>::push(pObject);
   return;
   }


   dictionary *dictionaryStack::top() {
   if ( 0 == size() ) 
      throw stackunderflow("");
   return std::stack<dictionary *>::top();
   }


   dictionary *dictionaryStack::pop() {
   if ( 0 == size() ) 
      throw stackunderflow("");
   dictionary *pDictionary = std::stack<dictionary *>::top();
   std::stack<dictionary *>::pop();
   return pDictionary;
   }


   dictionary *dictionaryStack::find(char *pszName) {

   std::list<dictionary *> restack;

   dictionary *pCurrent = NULL;

   while ( size() ) {
      dictionary *pThis = pop();
      restack.insert(restack.end(),pThis);
      if ( 0 == strcmp(pThis -> Name(),pszName) && ( strlen(pThis -> Name()) == strlen(pszName) ) ) {
         pCurrent = pThis;
         break;
      }
   }

   for ( std::list<dictionary *>::reverse_iterator it = restack.rbegin(); it != restack.rend(); it++ ) 
      push((*it));

   return pCurrent;
   }