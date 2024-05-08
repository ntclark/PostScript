// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <exception>

    class PStoPDFException : std::exception {

    public:

        PStoPDFException(char *pszErrorMessage) { strcpy(szMessage,pszErrorMessage); };
        char *Message() { return szMessage; };

        virtual char *ExceptionName() { return "PStoPDFException";} ;

    private:
        char szMessage[2048];

    };

#define errorclass(name)                                                   \
class name : public PStoPDFException {                                     \
public:                                                                    \
      name(char *pszErrorMessage) : PStoPDFException(pszErrorMessage) {};  \
      char *ExceptionName() { return #name; };                             \
};

errorclass(dictstackunderflow)
errorclass(invalidfont)
errorclass(rangecheck)
errorclass(stackunderflow)
errorclass(syntaxerror)
errorclass(typecheck)
errorclass(undefined)
errorclass(undefinedresource)