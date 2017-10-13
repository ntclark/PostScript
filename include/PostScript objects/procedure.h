
#pragma once

#include <list>

class job;

#include "object.h"

   class procedure : public object {
   public:

      procedure(job *p,char *pStart,char *pEnd);
      procedure(job *p,char *pStart,char **ppEnd);
      procedure(job *p);

      virtual ~procedure();

      virtual char *Contents(char *pszContents = NULL);

      void insert(object *);

      char *ToString();
      void clear();

      void execute();
      void bind();

      bool IsBound() { return isBound; };

   private:

      job *pJob;
      bool isBound;
      std::list<object *> entries;
      char *pszStringRepresentation;

      friend class job;

   };
   