
#pragma once

#include <list>
#include <map>
//#include "PostScript.h"

class job;

#include "object.h"

   class dictionary : public object {
   public:

      dictionary(job *,char *);
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

      void remove(char *pszKey);

      long size() { return (long)entries.size(); };

   private:

      job *pJob;
      std::map<long,object *> entries;
      std::map<long,void (__thiscall job::*)()> operators;
      std::list<char *> keys;

   };
