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

      void revertMatrix();

      void copyFrom(matrix *);

      void scale(POINT_TYPE scaleFactor);
      void scale(POINT_TYPE scaleX,POINT_TYPE scaleY);
      void translate(POINT_TYPE x,POINT_TYPE y);
      void rotate(POINT_TYPE angle);

      POINT_TYPE *Values() { return values; }

      POINT_TYPE a();
      POINT_TYPE b();
      POINT_TYPE c();
      POINT_TYPE d();
      POINT_TYPE tx();
      POINT_TYPE ty();

      POINT_TYPE aInverse(boolean force = false);
      POINT_TYPE bInverse(boolean force = false);
      POINT_TYPE cInverse(boolean force = false);
      POINT_TYPE dInverse(boolean force = false);
      POINT_TYPE txInverse(boolean force = false);
      POINT_TYPE tyInverse(boolean force = false);

      void a(POINT_TYPE v);
      void b(POINT_TYPE v);
      void c(POINT_TYPE v);
      void d(POINT_TYPE v);
      void tx(POINT_TYPE v);
      void ty(POINT_TYPE v);

      static void scale(XFORM *,FLOAT scaleFactor);

   private:

      POINT_TYPE values[6]{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};

      POINT_TYPE revertValues[6]{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};
      POINT_TYPE inverse[6]{6 * 0.0f};
      boolean invalidated{false};

      void invert();

   };