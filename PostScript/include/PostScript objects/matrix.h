#pragma once

#include "object.h"
#include "PostScript objects/array.h"

   class matrix : public array {
   public:
      
      matrix(job *pJob);
      ~matrix();

      void SetValue(long index,POINT_TYPE value);

      virtual char *ToString();

      void identity();

      void concat(matrix *);
      void concat(array *);
      void concat(POINT_TYPE *);
      void concat(XFORM &);

      void revertTransform();

      void copyFrom(matrix *);

      void scale(POINT_TYPE scaleFactor);

      POINT_TYPE a();
      POINT_TYPE b();
      POINT_TYPE c();
      POINT_TYPE d();
      POINT_TYPE tx();
      POINT_TYPE ty();

      POINT_TYPE aInverse();
      POINT_TYPE bInverse();
      POINT_TYPE cInverse();
      POINT_TYPE dInverse();
      POINT_TYPE txInverse();
      POINT_TYPE tyInverse();

      void a(POINT_TYPE v);
      void b(POINT_TYPE v);
      void c(POINT_TYPE v);
      void d(POINT_TYPE v);
      void tx(POINT_TYPE v);
      void ty(POINT_TYPE v);

   private:

      POINT_TYPE revertValues[6]{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};
      POINT_TYPE inverse[6]{6 * 0.0f};
      boolean invalidated{false};

      void invert();

   };