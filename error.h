
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