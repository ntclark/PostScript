
#pragma once

#include "object.h"
#include <map>

   class string : public object {
   public:

      string(job *pJob,char *pszValue); 
      string(job *pJob,char *pStart,char *pEnd);

      ~string();

      virtual BYTE get(long index);
      virtual void put(long index,BYTE);

      size_t length() { return NULL == pszUnescapedString ? strlen(Contents()) : strlen(pszUnescapedString); };

   protected:

      string(job *pJob,object::valueType valueType);
      string(job *pJob,char *pszValue,object::valueType valueType);
      string(job *pJob,char *pStart,char *pEnd,object::valueType valueType);

   private:

      char *pszUnescapedString{NULL};
      std::map<uint16_t,BYTE> escapedValues;

   };