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

    psTransformsStack::psTransformsStack() {
    return;
    }


    psTransformsStack::~psTransformsStack() {
    clear();
    return;
    }


    void psTransformsStack::initialize(job *pj) {
    pJob = pj;
    push_back(new (pJob -> CurrentObjectHeap()) matrix(pJob));
    return;
    }


    void psTransformsStack::gSave() {
    matrix *pMatrix = new (pJob -> CurrentObjectHeap()) matrix(pJob);
    pMatrix -> copyFrom(back());
    push_back(pMatrix);
    return;
    }


    void psTransformsStack::gRestore() {
    if ( 1 == size() )
{
        OutputDebugStringA("There has been a gRestore underflow\n");//Beep(2000,200);
        return;
}
    pop_back();
    return;
    }