
#include "colorSpace.h"

#include "job.h"

   colorSpace::colorSpace(job *pj,object *po) :
      parameterCount(0),
      array(pj,po -> Name(),object::colorSpace)
   {
   if ( object::matrix == po -> ObjectType() ) {
_asm {
int 3;
}
   }
   parameterCount = size() - 1;
   return;
   }

   colorSpace::colorSpace(job *pj,char *pszName) :
      parameterCount(0),
      array(pj,pszName,object::colorSpace)
   {
   return;
   }


   colorSpace::~colorSpace() {
   return;
   }


   long colorSpace::ParameterCount() {
   if ( parameterCount )
      return size() - 1;
   if ( 0 == strcmp(Name(),"DeviceGray") )
      return 1;
   if ( 0 == strcmp(Name(),"DeviceRGB") ) 
      return 3;
//_asm {
//int 3;
//}
   return 3;
   }

