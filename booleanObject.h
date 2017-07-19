
#include "object.h"

   class booleanObject : public object {
   public:

      booleanObject(char *pszName);
      ~booleanObject();

   private:

      boolean value;

   };