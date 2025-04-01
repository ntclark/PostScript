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

#pragma once

#include <exception>

    class PStoPDFException : public std::exception {
    public:

        PStoPDFException(char *pszErrorMessage) { strcpy(szMessage,pszErrorMessage); };
        char *Message() { return szMessage; };

        virtual char *ExceptionName() { return "PStoPDFException";} ;

    private:
        static char szMessage[2048];

    };

#define errorclass(name)                                                   \
class name : public PStoPDFException {                                     \
public:                                                                    \
      name(char *pszErrorMessage) : PStoPDFException(pszErrorMessage) {};  \
      char *ExceptionName() { return #name; };                             \
};

errorclass(nonPostscriptException)
errorclass(accessViolationException)

errorclass(dictstackunderflow)
errorclass(invalidfont)
errorclass(rangecheck)
errorclass(stackunderflow)
errorclass(syntaxerror)
errorclass(typecheck)
errorclass(undefined)
errorclass(undefinedresource)
errorclass(nocurrentpoint)
errorclass(notimplemented)