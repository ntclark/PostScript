
#include "Stacks/dictionaryStack.h"

#include "error.h"


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