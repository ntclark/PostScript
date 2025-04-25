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

#include "job.h"
#include "Stacks/dictionaryStack.h"


    dictionary *dictionaryStack::top() {
    if ( 0 == size() ) 
        throw stackunderflow("");
    return back();
    }


    dictionary *dictionaryStack::pop() {
    if ( 0 == size() ) 
        throw stackunderflow("");
    dictionary *pDictionary = back();
    pop_back();
    return pDictionary;
    }


    dictionary *dictionaryStack::bottom() {
    if ( 0 == size() ) 
        throw stackunderflow("");
    return front();
    }


    void dictionaryStack::setCurrent(dictionary *pDict) {
    push_back(pDict);
    return;
    }


    dictionary *dictionaryStack::find(char *pszName) {
    for ( deque <dictionary *>::reverse_iterator it = rbegin(); it != rend(); it++ ) 
        if ( (*it) -> exists(pszName) )
            return *it;
    return NULL;
    }


    void dictionaryStack::remove(dictionary *pDict) {
    for ( deque <dictionary *>::iterator it = begin(); it != end(); it++ ) {
        if ( *it == pDict ) {
            erase(it);
            return;
        }
    }
    return;
    }