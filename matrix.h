
#pragma once

#include "object.h"

   class matrix : public object {
   public:
      
      matrix();
      matrix(matrix &);
      ~matrix();

      void SetValue(long index,float value);

      virtual void concat(matrix *);
      virtual void concat(float *);
      virtual void concat(XFORM &);

   private:

      union {
         struct { float a,b,c,d,tx,ty; };
         float v[6];
      };

   };