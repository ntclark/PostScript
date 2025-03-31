#pragma once

#include <map>

#include "object.h"

#define A 0
#define B 1
#define C 2
#define D 3
#define TX 4
#define TY 5

class job;

   class array : public object {
   public:

      array(job *pJob,char *pszName);
      array(job *pJob,char *pszName,char *pszValues);
      array(job *pJob,char *pszName,long initialCount);
      array(job *pJob,long initialCount);
      array(job *pJob,char *pszName,long initialCount,char *pszValues);

      virtual ~array();

      void insert(object *);

      virtual void putElement(long index,object *);
      virtual object *getElement(long index);

      virtual void setValue(long index,POINT_TYPE v);

      void clear();

      bool find(long index) { return entries.find(index) != entries.end() ; };

      long size() { return (long)entries.size(); };

      long sizeFromString(char *pszValues);

      char *stringValueFromArray(char *pszValue,long oneBasedIndex);

   protected:

      std::map<long,object *> entries;

      friend class job;
   };
   
