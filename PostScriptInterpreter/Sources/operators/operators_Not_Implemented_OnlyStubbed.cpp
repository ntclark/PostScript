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

    static char szNullProcedure[]{"{ pop 0 }"};

    // Unimplemented, only stubbed

    // ( Not all such operators are in this file yet)


    void job::operatorCurrentcolortransfer() {
    /*
    currentcolortransfer 
        – currentcolortransfer redproc greenproc blueproc grayproc

    returns the current transfer functions in the graphics state (see Section 7.3,
    "Transfer Functions") for all four primary color components of the output device
    (cyan, magenta, yellow, and black). 

    If the current transfer functions were established via the setcolortransfer 
    operator, the four transfer functions are returned independently; if settransfer 
    was used, the same transfer function is repeated for all four.

    Errors: stackoverflow
    See Also: setcolortransfer, settransfer, currenttransfer
    */

    procedure *pRedProc = new (CurrentObjectHeap()) procedure(this,szNullProcedure,(char *)NULL,NULL);
    procedure *pGreenProc = new (CurrentObjectHeap()) procedure(this,szNullProcedure,(char *)NULL,NULL);
    procedure *pBlueProc = new (CurrentObjectHeap()) procedure(this,szNullProcedure,(char *)NULL,NULL);
    procedure *pGrayProc = new (CurrentObjectHeap()) procedure(this,szNullProcedure,(char *)NULL,NULL);

    push(pRedProc);
    push(pGreenProc);
    push(pBlueProc);
    push(pGrayProc);

    return;
    }


    void job::operatorCurrenthalftone() {
    /*
        currenthalftone 
            – currenthalftone halftone

    returns a halftone dictionary describing the current halftone screen parameter in
    the graphics state. If the current halftone was established via the setscreen or
    setcolorscreen operator instead of sethalftone, currenthalftone fabricates and 
    returns a halftone dictionary of type 1 or 2.

    Errors: stackoverflow, VMerror
    See Also: sethalftone, setscreen, setcolorscreen, sethalftone, currentscreen,
        currentcolorscreen
    */


    push(new (CurrentObjectHeap()) dictionary(this,0L));

    return;
    }

