
#pragma once

#include <list>
#include <vector>
#include <map>

class job;

#include "object.h"

   class dictionary : public object {
   public:

      dictionary(job *,char *);
      dictionary(job *,char *,bool isSystemObject);
      dictionary(job *,char *,enum objectType);
      dictionary(job *,enum objectType);

      virtual ~dictionary();

      void insert(char *,void (__thiscall job::*)());
      void insert(char *,object *);
      void insert(char *,char *);

      void put(char *psz,object *p) { insert(psz,p); };
      void retrieve(char *,void (__thiscall job::**)());

      object *retrieve(char *);
      char *getKey(long index);
      boolean exists(char *);
      boolean exists(object *);

      void remove(char *pszKey);
      void remove(object *pObj);

      long size() { return (long)entries.size(); };

   private:

      std::map<long,object *> entries;
      std::map<long,void (__thiscall job::*)()> operators;
      std::vector<char *> keys;

   };
