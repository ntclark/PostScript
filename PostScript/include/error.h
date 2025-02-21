
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