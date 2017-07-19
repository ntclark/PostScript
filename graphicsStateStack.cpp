
#include "graphicsStateStack.h"
#include "graphicsState.h"

   graphicsStateStack::graphicsStateStack(job *pJob) {
   return;
   }

   graphicsStateStack::~graphicsStateStack() {
//NTC: 12-17-2011
// There are significant memory management problems in this system.
// In the current usage model - this is somewhat okay. The way it works is that this
// dll is loaded manually and forced to close - and all memory allocated during it's 
// use should revert back in full. 
// At some level - this may be the way that memory management is handled in a firmware
// level postscript interepreter.
//
#if 0
   graphicsState *pGraphicsState = NULL;
   while ( pGraphicsState = restore() )
      delete pGraphicsState;
#endif
   return;
   }

   void graphicsStateStack::save() {
   std::stack<graphicsState *>::push(new graphicsState(*current()));
   return;
   }


   graphicsState *graphicsStateStack::restore() {
   if ( 0 == size() ) 
      throw stackunderflow("There was an underflow in the PostScript graphics state stack.");
//NTC: 12-17-2011
// Encountering underflows when analyzing general PDF files - specifically, ones that have not
// been created by the CursiVision print driver. 
// This may indicate that there is still a lot of work to do to be sure the system is PDF 1.5
// compatible - as I believe the CursiVision print driver is producing 1.4 level PDF documents,
// and most other documents would be higher level.
#if 1
   if ( 1 > size() ) 
#endif
   std::stack<graphicsState *>::pop();
   graphicsState *pCurrent = current();
   pCurrent -> restored();
   return pCurrent;
   }


   graphicsState *graphicsStateStack::current() {
   if ( 0 == size() ) 
      throw stackunderflow("There was an underflow in the PostScript graphics state stack.");
   return std::stack<graphicsState *>::top();
   }
