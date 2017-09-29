
#include "job.h"
#include "graphicsState.h"
#include "string.h"
#include "font.h"
#include "pattern.h"
#include "mark.h"
#include "save.h"

   void job::operatorStdout() {
/*
   = 
      any = –

   pops an object from the operand stack, produces a text representation of that object’s
   value, and writes the result to the standard output file, followed by a newline
   character. The text is that produced by the cvs operator; thus, = prints the value of
   a number, boolean, string, name, or operator object and prints --nostringval-- for
   an object of any other type.

   The name = is not special. In PostScript programs it must be delimited by whitespace
   or special characters the same as names composed of alphabetical
*/
   object *top = pop();
   pPStoPDF -> queueLog("\n");
   pPStoPDF -> queueLog(top -> Contents());
   pPStoPDF -> queueLog("\n");
   return;
   }

   void job::operatorAdd() {
/*
   add 
      num1 num2 add sum

   returns the sum of num1 and num2. If both operands are integers and the result is
   within integer range, the result is an integer; otherwise, the result is a real number.
*/
   object *p1 = pop();
   object *p2 = pop();
   if ( object::integer == p1 -> ObjectType() && object::integer == p2 -> ObjectType() )
      push(new object(p1 -> IntValue() + p2 -> IntValue()));
   else
      push(new object(p1 -> Value() + p2 -> Value()));
   return;
   }


   void job::operatorAnd() {
/*
   and 
      bool1 bool2 and bool3
      int1 int2 and int3

   returns the logical conjunction of the operands if they are boolean. If the operands
   are integers, and returns the bitwise “and” of their binary representations.

   Examples
      true true and => true % A complete truth table
      true false and => false
      false true and => false
      false false and => false
      99 1 and => 1
      52 7 and => 4

      Errors: stackunderflow, typecheck
      See Also: or, xor, not, true, false
*/

   object *p1 = pop();
   object *p2 = pop();

   if ( object::logical == p1 -> ObjectType() && object::logical == p2 -> ObjectType() ) {

      if ( p1 == pTrueConstant && p2 == pTrueConstant )
         push(pTrueConstant);
      else 
         push(pFalseConstant);

   } else {

      if ( object::integer == p1 -> ObjectType() && object::integer == p2 -> ObjectType() ) {

         long v = p1 -> IntValue() & p2 -> IntValue();

         push(new object(v));

      } else {
         __debugbreak();
      }

   }
   return;
   }

   void job::operatorArc() {
/*
   arc 
      x y r angle1 angle2 arc –

   appends an arc of a circle to the current path, possibly preceded by a straight line
   segment. The arc is centered at coordinates (x, y) in user space, with radius r. The
   operands angle1 and angle2 define the endpoints of the arc by specifying the angles
   of the vectors joining them to the center of the arc. The angles are measured in degrees
   counterclockwise from the positive x axis of the current user coordinate system
   (see Figure 8.1).

               FIGURE 8.1 arc operator

   The arc produced is circular in user space. If user space is scaled nonuniformly
   (that is, differently in the x and y dimensions), the resulting curve will be elliptical
   in device space.

   If there is a current point, a straight line segment from the current point to the
   first endpoint of the arc is added to the current path preceding the arc itself. If the
   current path is empty, this initial line segment is omitted. In either case, the second
   endpoint of the arc becomes the new current point.

   If angle2 is less than angle1, it is increased by multiples of 360 until it becomes
   greater than or equal to angle1. No other adjustments are made to the two angles.
   In particular, the angle subtended by the arc is not reduced modulo 360; if the difference
   angle2 - angle1 exceeds 360, the resulting path will trace portions of the
   circle more than once.

   The arc is represented internally by one or more cubic Bézier curves (see curveto)
   approximating the required shape. This is done with sufficient accuracy to produce
   a faithful rendition of the required arc. However, a program that reads the
   constructed path using pathforall will encounter curveto segments where arcs
   were specified originally.
*/
   pop();
   pop();
   pop();
   pop();
   pop();
   return;
   }

   void job::operatorArcn() {
/*
   arcn 
      x y r angle1 angle2 arcn –

   (arc negative) appends an arc of a circle to the current path, possibly preceded by
   a straight line segment. Its behavior is identical to that of arc, except that the
   angles defining the endpoints of the arc are measured clockwise from the positive
   x axis of the user coordinate system, rather than counterclockwise. If angle2 is
   greater than angle1, it is decreased by multiples of 360 until it becomes less than or
   equal to angle1.

      Example
         newpath
         0 0 2 0 90 arc
         0 0 1 90 0 arcn
         closepath
         (0,0) (1,0)
         45°

   This example constructs a 90-degree “windshield wiper swath” 1 unit wide,with
   an outer radius of 2 units, centered at the coordinate origin (see Figure 8.3).

*/
   pop();
   pop();
   pop();
   pop();
   pop();
   return;
   }

   void job::operatorArray() {
/*
   array 
      int array array

   creates an array of length int, each of whose elements is initialized with a null object,
   and pushes this array on the operand stack. The int operand must be a nonnegative
   integer not greater than the maximum allowable array length (see
   Appendix B). The array is allocated in local or global VM according to the current
   VM allocation mode (see Section 3.7.2, “Local and Global VM” ).

   Example
      3 array => [null null null]
*/

   long count = pop() -> IntValue();
   array *pArray = new array(this,"unnamed",count);
   push(pArray);
   return;
   }

   void job::operatorAshow() {
/*
      ashow 
         ax ay string ashow –

   paints glyphs for the characters of string in a manner similar to show; however,
   while doing so, ashow adjusts the width of each glyph shown by adding ax to the
   glyph’s x width and ay to its y width, thus modifying the spacing between glyphs.
   The numbers ax and ay are x and y displacements in the user coordinate system,
   not in the glyph coordinate system.
   This operator enables fitting a string of text to a specific width by adjusting all the
   spacing between glyphs by a uniform amount. For a discussion of glyph widths,
   see Section 5.4, “Glyph Metric Information.”
*/
   pop();
   pop();
   pop();
   return;
   }

   void job::operatorAstore() {
/*

   astore 

      any0 … anyn-1 array astore array

   stores the objects any0 to anyn-1 from the operand stack into array, where n is the
   length of array. The astore operator first removes the array operand from the stack
   and determines its length. It then removes that number of objects from the stack,
   storing the topmost one into element n - 1 of array and the bottommost one into
   element 0. Finally, it pushes array back on the stack. Note that an astore operation
   cannot be performed on packed arrays.

   If the value of array is in global VM and any of the objects any0 through anyn-1 are
   composite objects whose values are in local VM, an invalidaccess error occurs (see
   Section 3.7.2, “Local and Global VM”).

   Example

      (a) (bcd) (ef) 3 array astore Þ [(a) (bcd) (ef)]

   This example creates a three-element array, stores the strings a, bcd, and ef into it
   as elements 0, 1, and 2, and leaves the array object on the operand stack.

   Errors: invalidaccess, stackunderflow, typecheck
*/
   
   array *pArray = reinterpret_cast<array *>(top());

   operatorLength();

   object *pCount = pop();

   long n = pCount -> IntValue();

   for ( long k = 0; k < n; k++ ) {

      object *pObject = pop();

      push(pArray);
      push(new object(n - k - 1));
      push(pObject);

      operatorPut();

   }
   
   push(pArray);

   return;
   }


   void job::operatorAwidthshow() {
/*
   awidthshow 
      cx cy char ax ay string awidthshow –

   paints glyphs for the characters of string in a manner similar to show, but combines
   the special effects of ashow and widthshow. awidthshow adjusts the width
   of each glyph shown by adding ax to its x width and ay to its y width, thus modifying
   the spacing between glyphs. Furthermore, awidthshow modifies the width of
   each occurrence of the glyph for the character char by an additional amount
   (cx , cy). The interpretation of char is as described for the widthshow operator.
   This operator enables fitting a string of text to a specific width by adjusting the
   spacing between all glyphs by a uniform amount, while independently controlling
   the width of the glyph for a specific character, such as the space character. For a
   discussion of glyph widths, see Section 5.4, “Glyph Metric Information.”
*/

   pop();
   pop();
   pop();
   pop();
   pop();
   pop();
   return;
   }


   void job::operatorBegin() {

/*
   begin 
      dict begin –

   pushes dict on the dictionary stack, making it the current dictionary and installing
   it as the first of the dictionaries consulted during implicit name lookup and by
   def, load, store, and where.

   Errors: dictstackoverflow, invalidaccess, stackunderflow, typecheck
*/

   object *pDictionary = pop();

   if ( object::dictionary == pDictionary -> ObjectType() || object::font == pDictionary -> ObjectType() ) {
      dictionaryStack.push(reinterpret_cast<dictionary *>(pDictionary));
      return;
   }

   char szMessage[1024];
   sprintf(szMessage,"operator begin: object %s is not a dictionary",pDictionary -> Name());
   throw typecheck(szMessage);

   return;
   }


   void job::operatorBind() {
/*
   bind 
      proc bind proc

   replaces executable operator names in proc by their values. For each element of
   proc that is an executable name, bind looks up the name in the context of the current
   dictionary stack as if by the load operator. If the name is found and its value
   is an operator object, bind replaces the name with the operator in proc. If the
   name is not found or its value is not an operator, bind does not make a change.

   For each procedure object contained within proc, bind applies itself recursively to
   that procedure, makes the procedure read-only, and stores it back into proc. bind
   applies to both arrays and packed arrays, but it treats their access attributes differently.
   It will ignore a read-only array; that is, it will neither bind elements of the
   array nor examine nested procedures. On the other hand, bind will operate on a
   packed array (which always has read-only or even more restricted access), disregarding
   its access attribute. No error occurs in either case.

   The effect of bind is that all operator names in proc and in procedures nested
   within proc to any depth become tightly bound to the operators themselves. During
   subsequent execution of proc, the interpreter encounters the operators themselves
   rather than their names. See Section 3.12, “Early Name Binding.”

   In LanguageLevel 3, if the user parameter IdiomRecognition is true, then after replacing
   executable names with operators, bind compares proc with every template
   procedure defined in instances of the IdiomSet resource category. If it finds a
   match, it returns the associated substitute procedure. See Section 3.12.1, “bind
   Operator.”
*/

   // Not implemented yet. Instead - procedures will be properly resolved during 
   // execution time. That logic should then help in implementing this operator.

   procedure *pProcedure = reinterpret_cast<procedure *>(pop());

   pProcedure -> bind();

   push(pProcedure);

   return;
   }

   void job::operatorCleartomark() {
/*
   cleartomark 
      mark obj1 … objn cleartomark –

   pops entries from the operand stack repeatedly until it encounters a mark, which
   it also pops from the stack. obj1 through objn are any objects other than marks.
*/
   while ( object::mark != top() -> ObjectType() ) {
      pop();
   }
   pop();
   return;
   }

   void job::operatorClosepath() {
/*
   closepath 
      – closepath –

   closes the current subpath by appending a straight line segment connecting the
   current point to the subpath’s starting point, which is generally the point most recently
   specified by moveto (see Section 4.4, “Path Construction”).
   closepath terminates the current subpath; appending another segment to the current
   path will begin a new subpath, even if the new segment begins at the endpoint
   reached by the closepath operation. If the current subpath is already closed
   or the current path is empty, closepath does nothing.
*/
   graphicsStateStack.current() -> closepath();
   return;
   }

   void job::operatorConcat() {
/*
   concat 
      matrix concat –

   applies the transformation represented by matrix to the user coordinate space.
   concat accomplishes this by concatenating matrix with the current transformation
   matrix (CTM); that is, it replaces the CTM with the matrix product matrix ´ CTM
   (see Section 4.3, “Coordinate Systems and Transformations”).
*/
   graphicsStateStack.current() -> concat(reinterpret_cast<matrix *>(pop()));
   return;
   }

   void job::operatorCopy() {
/*
   copy 

      any1 … anyn n copy any1 … anyn any1 … anyn

      array1 array2 copy subarray2
      dict1 dict2 copy dict2
      string1 string2 copy substring2
      packedarray1 array2 copy subarray2
      gstate1 gstate2 copy gstate2

   performs two entirely different functions, depending on the type of the topmost
   operand.

   In the first form, where the top element on the operand stack is a nonnegative integer
   n, copy pops n from the stack and duplicates the top n elements on the stack
   as shown above. This form of copy operates only on the objects themselves, not
   on the values of composite objects.

   Examples

      (a) (b) (c) 2 copy => (a) (b) (c) (b) (c)
      (a) (b) (c) 0 copy => (a) (b) (c)
   
   In the other forms, copy copies all the elements of the first composite object into
   the second. The composite object operands must be of the same type, except that
   a packed array can be copied into an array (and only into an array—copy cannot
   copy into packed arrays, because they are read-only). This form of copy copies the
   value of a composite object. This is quite different from dup and other operators
   that copy only the objects themselves (see Section 3.3.1, “Simple and Composite
   Objects”). However, copy performs only one level of copying. It does not apply
   recursively to elements that are themselves composite objects; instead, the values
   of those elements become shared.

   In the case of arrays or strings, the length of the second object must be at least as
   great as the first; copy returns the initial subarray or substring of the second operand
   into which the elements were copied. Any remaining elements of array2 or
   string2 are unaffected.

   In the case of dictionaries, LanguageLevel 1 requires that dict2 have a length (as returned
   by the length operator) of 0 and a maximum capacity (as returned by the
   maxlength operator) at least as great as the length of dict1. LanguageLevels 2 and 3
   do not impose this restriction, since dictionaries can expand when necessary.
   The literal/executable and access attributes of the result are normally the same as
   those of the second operand. However, in LanguageLevel 1 the access attribute of
   dict2 is copied from that of dict1.

   If the value of the destination object is in global VM and any of the elements copied
   from the source object are composite objects whose values are in local VM, an
   invalidaccess error occurs (see Section 3.7.2, “Local and Global VM”).

   Example

      /a1 [1 2 3] def
      a1 dup length array copy => [1 2 3]
      
   Errors: invalidaccess, rangecheck, stackoverflow, stackunderflow, typecheck
   See Also: dup, get, put, putinterval
*/

   object *pTop = top();

   if ( object::number == pTop -> ObjectType() && object::integer == pTop -> ValueType() && pTop -> IntValue() >= 0 ) {

      pTop = pop();

      long n = pTop -> IntValue();

      std::list<object *> entries;

      for ( long k = 0; k < n; k++ )
         entries.insert(entries.end(),pop());

      for ( std::list<object *>::reverse_iterator it = entries.rbegin(); it != entries.rend(); it++ )
         push(*it);

      for ( std::list<object *>::reverse_iterator it = entries.rbegin(); it != entries.rend(); it++ )
         push(*it);

      entries.clear();
   
      return;

   }

   switch ( pTop -> ObjectType() ) {

   case object::dictionary: {
      dictionary *pTarget = reinterpret_cast<dictionary *>(pop());
      dictionary *pSource = reinterpret_cast<dictionary *>(pop());

      long n = pSource -> size();

      for ( long k = 0; k < n; k++ ) 
         pTarget -> put(pSource -> getKey(k),pSource -> retrieve(pSource -> getKey(k)));

      push(pTarget);

      }
      break;

   default:
      __debugbreak();
      break;
   }

   return;
   }

   void job::operatorCountdictstack() {
/*
   countdictstack 
      – countdictstack int

   counts the number of dictionaries currently on the dictionary stack and pushes
   this count on the operand stack.
*/
   push(new object((long)dictionaryStack.size()));
   return;
   }


   void job::operatorCounttomark() {
/*
   counttomark 
      mark obj1 … objn counttomark mark obj1 … objn n

   counts the number of objects on the operand stack, starting with the top element
   and continuing down to but not including the first mark encountered. obj1
   through objn are any objects other than marks.
*/

   std::list<object *> entries;

   long count = 0;

   while ( object::mark != top() -> ObjectType() ) {
      count++;
      entries.insert(entries.end(),pop());
   }

   for ( std::list<object *>::reverse_iterator it = entries.rbegin(); it != entries.rend(); it++ )
      push(*it);

   push(new object(count));

   entries.clear();

   return;
   }

   void job::operatorCurrentdict() {
/*
   currentdict 
      – currentdict dict

   pushes the current dictionary (the dictionary on the top of the dictionary stack)
   on the operand stack. currentdict does not pop the dictionary stack; it just pushes
   a duplicate of its top element on the operand stack.

*/
   push(dictionaryStack.top());
   return;
   }

   void job::operatorCurrentglobal() {
/*
   currentglobal 
      – currentglobal bool

   returns the VM allocation mode currently in effect.
*/
   if ( isGlobalVM )
      push(pTrueConstant);
   else
      push(pFalseConstant);
   return;
   }

   void job::operatorCurrentmatrix() {
/*
   currentmatrix 
      matrix currentmatrix matrix

   replaces the value of matrix with the current transformation matrix (CTM) in the
   graphics state and pushes this modified matrix back on the operand stack (see
   Section 4.3.2, “Transformations”).
*/

   return;
   }

   void job::operatorCurrentscreen() {
/*
   currentscreen 
         currentscreen frequency angle proc
         currentscreen frequency angle halftone (LanguageLevel 2)

   returns the frequency, angle, and spot function of the current halftone screen parameter
   in the graphics state (see Section 7.4, “Halftones”), assuming that the
   halftone was established via the setscreen operator. If setcolorscreen was used instead,
   the values returned describe the screen for the gray color component only.
   If the current halftone was defined via the sethalftone operator, currentscreen returns
   a halftone dictionary describing its properties in place of the spot function.

   For type 1 halftone dictionaries, the values returned for frequency and angle are
   taken from the dictionary’s Frequency and Angle entries; for all other halftone
   types, currentscreen returns a frequency of 60 and an angle of 0.

   Errors: stackoverflow
   See Also: setscreen, setcolorscreen,
*/

   push(pTrueConstant);
   push(pTrueConstant);
   push(pTrueConstant);

   pPStoPDF -> queueLog("\n");
   pPStoPDF -> queueLog("NOT IMPLEMENTED: currentscreen");
   pPStoPDF -> queueLog("\n");
   
   return;
   }


   void job::operatorCurveto() {
/*
   curveto 
      x1 y1 x2 y2 x3 y3 curveto –

   appends a section of a cubic Bézier curve to the current path between the current
   point (x0, y0) and the endpoint (x3, y3), using (x1, y1) and (x2, y2) as the Bézier control
   points. The endpoint (x3, y3) becomes the new current point. If the current
   point is undefined because the current path is empty, a nocurrentpoint error
   occurs.

   The four points (x0, y0), (x1, y1), (x2, y2), and (x3, y3) define the shape of the curve
   geometrically (see Figure 8.6). The curve is always entirely enclosed by the convex
   quadrilateral defined by the four points. It starts at (x0, y0), is tangent to the line
   from (x0, y0) to (x1, y1) at that point, and leaves the starting point in that direction.
   It ends at (x3, y3), is tangent to the line from (x2, y2) to (x3, y3) at that point, and
   approaches the endpoint from that direction. The lengths of the lines from (x0, y0)
   to (x1, y1) and from (x2, y2) to (x3, y3) represent, in a sense, the “velocity” of the
   path at the endpoints.

   Mathematically, a cubic Bézier curve is derived from a pair of parametric cubic
   equations:

      (missing)

   The cubic section produced by curveto is the path traced by x(t) and y(t) as t
   ranges from 0 to 1. The Bézier control points corresponding to this curve are:

      (missing)

*/

   pop();
   pop();
   pop();
   pop();
   pop();

   return;
   }

   void job::operatorCvi() {
/*
   cvi 
      num cvi int
      string cvi int

   (convert to integer) takes an integer, real, or string object from the stack and
   produces an integer result. If the operand is an integer, cvi simply returns it. If the
   operand is a real number, it truncates any fractional part (that is, rounds it toward
   0) and converts it to an integer. If the operand is a string, cvi invokes the equivalent
   of the token operator to interpret the characters of the string as a number
   according to the PostScript syntax rules. If that number is a real number, cvi converts
   it to an integer. A rangecheck error occurs if a real number is too large to
   convert to an integer. (See the round, truncate, floor, and ceiling operators, which
   remove fractional parts without performing type conversion.)
*/

//
//NTC: 1-6-2011: It is not clear why "version" is getting converted to a number like "2016" or 2017"
//If it is (originally) a real number, this may mean that it's value is something like 2.016 (?) or 20.16 ?
//But typically real -> integer conversion would end up with 2 or 20 in this case.
//
   object *pTop = pop();
   if ( object::number == pTop -> ObjectType() ) {
      if ( object::integer == pTop -> ValueType() || object::radix == pTop -> ValueType() )
         push(new object(pTop -> IntValue()));
      else
         push(new object(atol(pTop -> Contents())));
   } else
      push(new object(atol(pTop -> Contents())));
   return;
   }

   void job::operatorCvn() {
/*
   cvn 
      string cvn name

   (convert to name) converts the string operand to a name object that is lexically
   the same as the string. The name object is executable if the string was executable.
*/
//
// TODO: It is not clear what "executable" means at this time
//
   object *pTop = pop();

   long n = (DWORD)strlen(pTop -> Name()) + 2;
   char *pszTemp = new char[n];

   sprintf(pszTemp,"/%s",pTop -> Name());

   push(new string(pszTemp));

   delete [] pszTemp;

   return;
   }

   void job::operatorCvr() {
/*
   cvr 
      num cvr real
      string cvr real

   (convert to real) takes an integer, real, or string object and produces a real result.

   If the operand is an integer, cvr converts it to a real number. If the operand is a
   real number, cvr simply returns it. If the operand is a string, cvr invokes the equivalent
   of the token operator to interpret the characters of the string as a number
   according to the PostScript syntax rules. If that number is an integer, cvr converts
   it to a real number.

   Errors: invalidaccess, limitcheck, stackunderflow, syntaxerror, typecheck,undefinedresult
*/

   object *pTop = pop();
   if ( object::number == pTop -> ObjectType() )
      push(new object(pTop -> DoubleValue()));
   else
      push(new object(atof(pTop -> Contents())));
   return;
   }


   void job::operatorCvx() {
/*
   cvx 
      any cvx any

   (convert to executable) makes the object on the top of the operand stack have the
   executable instead of the literal attribute.
*/
   return;
   }

   void job::operatorDebug() {
   __debugbreak();
   return;
   }

   void job::operatorDef() {
/*
   def 
      key value def –

   associates key with value in the current dictionary—the one on the top of the dictionary
   stack (see Section 3.4, “Stacks”). If key is already present in the current
   dictionary, def simply replaces its value; otherwise, def creates a new entry for key
   and stores value with it.

   If the current dictionary is in global VM and value is a composite object whose
   value is in local VM, an invalidaccess error occurs (see Section 3.7.2, “Local and
   Global VM”).
*/

   object *pValue = pop();
   object *pKey = pop();

   dictionary *pDictionary = dictionaryStack.top();

   pDictionary -> insert(pKey -> Name(),pValue);

#if 0
   push(pValue);

   operatorWhere();

   if ( top() == pFalseConstant ) {
      pop();
   } else {
      pop();
      dictionary *pDictionary = reinterpret_cast<dictionary *>(pop());
      pValue = pDictionary -> retrieve(pValue -> Name());
   }

   pValue -> Name(pKey -> Name());
   dictionaryStack.top() -> insert(pKey -> Name(),pValue);
#endif

   return;
   }

   void job::operatorDefinefont() {
/*

   definefont 
      key font definefont font
      key cidfont definefont cidfont

   registers font or cidfont in the Font resource category as an instance associated with
   key (usually a name). definefont first checks that font or cidfont is a well-formed
   dictionary—in other words, that it contains all entries required in that type of dictionary.
   It inserts an additional entry whose key is FID and whose value is an object
   of type fontID. It makes the dictionary’s access read-only. Finally, it associates
   key with font or cidfont in the font directory.

   definefont distinguishes between a CIDFont and a font by the presence or absence
   of a CIDFontType entry. If the operand is a CIDFont, definefont also inserts a
   FontType entry with an appropriate value (see Table 5.11 on page 370).

   If the operand is a composite font (see Section 5.10, “Composite Fonts”),
   definefont inserts the entries EscChar, ShiftIn, and ShiftOut if they are not present
   but are required; it may also insert the implementation-dependent entries
   PrefEnc, MIDVector, and CurMID. All the descendant fonts must have been
   registered by definefont previously; descendant CIDFonts must have been either
   registered by definefont or defined as CIDFont resource instances (with defineresource).

   In LanguageLevel 1, the dictionary must be large enough to accommodate all of
   the additional entries inserted by definefont. The font must not have been registered
   previously, and an FID entry must not be present.

   In LanguageLevels 2 and 3, a Font resource instance can be associated with more
   than one key. If font or cidfont has already been registered, definefont does not
   alter it in any way.

   Subsequent invocation of findfont with key will return the same resource instance.
   Font registration is subject to the normal semantics of virtual memory (see
   Section 3.7, “Memory Management”). In particular, the lifetime of the definition
   depends on the VM allocation mode at the time definefont is executed. A local
   definition can be undone by a subsequent restore operation.

   definefont is actually a special case of defineresource operating on the Font category.

   For details, see defineresource and Section 3.9, “Named Resources.”

   Errors: dictfull, invalidaccess, invalidfont, limitcheck, rangecheck,stackunderflow, typecheck
   See Also: makefont, scalefont, setfont, defineresource, FontDirectory,GlobalFontDirectory, setglobal
*/

   object *po = pop();
   object *pKey = pop();

   switch ( po -> ObjectType() ) {

   case object::font: {
      font *pFont = reinterpret_cast<font *>(po);
      pFont -> Name(pKey -> Name());
      pFont -> insert("FID",pKey);
      }
      break;

   case object::dictionary: {
      dictionary *pDictionary = reinterpret_cast<dictionary *>(po);
      pDictionary -> Name(pKey -> Name());
      pDictionary -> insert("FID",pKey);
      }
      break;

   default:
      __debugbreak();
      break;
   }

   push(po);

   return;
   }


   void job::operatorDefineresource() {
/*
   defineresource 
      key instance category defineresource instance

   associates a resource instance with a resource name in a specified category.
   category is a name object that identifies a resource category, such as Font (see
   Section 3.9.2, “Resource Categories”). key is a name or string object that will be
   used to identify the resource instance. (Names and strings are interchangeable;
   other types of keys are permitted but are not recommended.) instance is the resource
   instance itself; its type must be appropriate to the resource category.

   Before defining the resource instance, defineresource verifies that the instance object
   is the correct type. Depending on the resource category, it may also perform
   additional validation of the object and may have other side effects (see
   Section 3.9.2); these side effects are determined by the DefineResource procedure
   in the category implementation dictionary. Finally, defineresource makes the object
   read-only if its access is not already restricted.

   The lifetime of the definition depends on the VM allocation mode at the time
   defineresource is executed. If the current VM allocation mode is local
   (currentglobal returns false), the effect of defineresource is undone by the next
   nonnested restore operation. If the current VM allocation mode is global
   (currentglobal returns true), the effect of defineresource persists until global VM
   is restored at the end of the job. If the current job is not encapsulated, the effect of
   a global defineresource operation persists indefinitely, and may be visible to other
   execution contexts.

   Local and global definitions are maintained separately. If a new resource instance
   is defined with the same category and key as an existing one, the new definition
   overrides the old one. The precise effect depends on whether the old definition is
   local or global and whether the new definition (current VM allocation mode) is
   local or global. There are two main cases:

      • The new definition is local. defineresource installs the new local definition,
      replacing an existing local definition if there is one. If there is an existing global
      definition, defineresource does not disturb it. However, the global definition is
      obscured by the local one. If the local definition is later removed, the global
      definition reappears.

      • The new definition is global. defineresource first removes an existing local definition
      if there is one. It then installs the new global definition, replacing an
      existing global definition if there is one.
      defineresource can be used multiple times to associate a given resource instance
      with more than one key.

   If the category name is unknown, an undefined error occurs. If the instance is of
   the wrong type for the specified category, a typecheck error occurs. If the instance
   is in local VM but the current VM allocation mode is global, an invalidaccess
   error occurs; this is analogous to storing a local object into a global dictionary.
   Other errors can occur for specific categories; for example, when dealing with the
   Font or CIDFont category, defineresource may execute an invalidfont error.
*/

   object *pCategory = pop();
   object *pInstance = pop();
   object *pKey = pop();

   resource *pResource = new resource(pCategory,pInstance,pKey);

   resourceList.insert(resourceList.end(),pResource);

   push(pResource);

   return;
   }

   void job::operatorDict() {
/*
   dict 
      int dict dict

   creates an empty dictionary with an initial capacity of int elements and pushes the
   created dictionary object on the operand stack. int is expected to be a nonnegative
   integer. The dictionary is allocated in local or global VM according to the VM allocation
   mode (see Section 3.7.2, “Local and Global VM”).

   In LanguageLevel 1, the resulting dictionary has a maximum capacity of int elements.
   Attempting to exceed that limit causes a dictfull error.

   In LanguageLevels 2 and 3, the int operand specifies only the initial capacity; the
   dictionary can grow beyond that capacity if necessary. The dict operator immediately
   consumes sufficient VM to hold int entries. If more than that number of entries
   are subsequently stored in the dictionary, additional VM is consumed at that
   time.

   There is a cost associated with expanding a dictionary beyond its initial allocation.
   For efficiency reasons, a dictionary is expanded in chunks rather than one element
   at a time, so it may contain a substantial amount of unused space. If a program
   knows how large a dictionary it needs, it should create one of that size
   initially. On the other hand, if a program cannot predict how large the dictionary
   will eventually grow, it should choose a small initial allocation sufficient for its
   immediate needs. The built-in writeable dictionaries (for example, userdict) follow
   the latter convention.
*/

   object *pCount = pop();

   dictionary *pDictionary = new dictionary(this,"unnamed");

//   dictionaryList.insert(dictionaryList.end(),pDictionary);

   push(pDictionary);

   return;
   }

   void job::operatorDiv() {
/*
   div 
      num1 num2 div quotient

   divides num1 by num2, producing a result that is always a real number even if both
   operands are integers. Use idiv instead if the operands are integers and an integer
   result is desired.
*/
   object *p2 = pop();
   object *p1 = pop();
   
   double v1 = p1 -> Value();
   double v2 = p2 -> Value();

   push(new object(v1 / v2));

   return;
   }

   void job::operatorDtransform() {
/*
   dtransform 
      dx1 dy1 dtransform dx2 dy2
      dx1 dy1 matrix dtransform dx2 dy2

(delta transform) applies a transformation matrix to the distance vector (dx1, dy1),
returning the transformed distance vector (dx2, dy2). The first form of the operator
uses the current transformation matrix in the graphics state to transform the
distance vector from user space to device space coordinates. The second form applies
the transformation specified by the matrix operand rather than the CTM.

A delta transformation is similar to an ordinary transformation (see Section 4.3,
“Coordinate Systems and Transformations”), but does not use the translation
components tx and ty of the transformation matrix. The distance vectors are thus
positionless in both the original and target coordinate spaces, making this operator
useful for determining how distances map from user space to device space.
*/
   double x,y;
   matrix *pMatrix = NULL;
   switch ( top() -> ObjectType() ) {
   case object::matrix:
      pMatrix = reinterpret_cast<matrix *>(pop());
      y = pop() -> Value();
      x = pop() -> Value();
      break;
   default:
      pMatrix = pCurrentMatrix;
      y = pop() -> Value();
      x = pop() -> Value();
   }
   push(new object(x));
   push(new object(y));
   return;
   }

   void job::operatorDup() {
/*
   dup 
      any dup any any

   duplicates the top element on the operand stack. dup copies only the object; the
   value of a composite object is not copied but is shared. See Section 3.3, “Data
   Types and Objects.”
*/

//   resolve();

   push(top());

   return;
   }


   void job::operatorEnd() {
/*
   end 
      – end –

   pops the current dictionary off the dictionary stack, making the dictionary below
   it the current dictionary. If end tries to pop the bottommost instance of userdict,
   a dictstackunderflow error occurs.
*/

   if ( 0 == dictionaryStack.size() ) {
      char szMessage[1024];
      sprintf(szMessage,"operator: end. An attempt was made to pop a dictionary off an empty dictionary stack");
      throw dictstackunderflow(szMessage);
   }

dictionary *pDictionary =
   dictionaryStack.pop();
   return;
   }

   
   void job::operatorEofill() {
/*
   eofill 
      – eofill –

   paints the area inside the current path with the current color. The even-odd rule is
   used to determine what points lie inside the path (see “Even-Odd Rule” on
   page 196). In all other respects, the behavior of eofill is identical to that of fill.
*/
   return;
   }

   void job::operatorEq() {
/*
   eq 
      any1 any2 eq bool

   pops two objects from the operand stack and pushes true if they are equal, or false
   if not. The definition of equality depends on the types of the objects being compared.
   Simple objects are equal if their types and values are the same. Strings are
   equal if their lengths and individual elements are equal. Other composite objects
   (arrays and dictionaries) are equal only if they share the same value. Separate values
   are considered unequal, even if all the components of those values are the
   same.

   This operator performs some type conversions. Integers and real numbers can be
   compared freely: an integer and a real number representing the same mathematical
   value are considered equal by eq. Strings and names can likewise be compared
   freely: a name defined by some sequence of characters is equal to a string whose
   elements are the same sequence of characters.

   The literal/executable and access attributes of objects are not considered in comparisons
   between objects.
*/
   object *pAny2 = pop();
   object *pAny1 = pop();

   if ( pAny1 == pAny2 ) {
      push(pTrueConstant);
      return;
   }

   if ( pAny2 -> ObjectType() != pAny1 -> ObjectType() ) {

      push(pFalseConstant);

//
// NTC: !!!! It is unclear why the eq (or ne) operator is comparing FID to the literal 
// numeric value 256 !!!!
//
#if 0
      if ( pAny2 == pNullConstant || pAny1 == pNullConstant ) {
         push(pFalseConstant);
         return;
      }

      if ( pAny1 -> ObjectType() != object::number ) {

         push(pAny1);

         operatorWhere();

         object *pIsFound = pop();

         if ( pTrueConstant == pIsFound ) {
            dictionary *pDict = reinterpret_cast<dictionary *>(pop());
            push(pDict -> retrieve(pAny1 -> Name()));
            push(pAny2);
            return operatorEq();
         }

      }

      if ( pAny2 -> ObjectType() != object::number ) {

         push(pAny2);

         operatorWhere();

         object *pIsFound = pop();

         if ( pTrueConstant == pIsFound ) {
            dictionary *pDict = reinterpret_cast<dictionary *>(pop());
            push(pAny1);
            push(pDict -> retrieve(pAny2 -> Name()));
            return operatorEq();
         }

      }

_asm {
int 3;
}
      push(pFalseConstant);
#endif

      return;
   }

   switch ( pAny1 -> ObjectType() ) {

   case object::number:
      if ( pAny1 -> Value() == pAny2 -> Value() )
         push(pTrueConstant);
      else
         push(pFalseConstant);
      return;

   case object::atom:

      if ( strlen(pAny1 -> Contents()) != strlen(pAny2 -> Contents()) ) {
         push(pFalseConstant);
         return;
      }

      if ( 0 == memcmp(pAny1 -> Contents(),pAny2 -> Contents(),strlen(pAny1 -> Contents())) )
         push(pTrueConstant);
      else
         push(pFalseConstant);

      return;

   case object::dictionary: {
      if ( object::dictionary != pAny2 -> ObjectType() ) {
         push(pFalseConstant);
         return;
      }
      dictionary *pDict1 = reinterpret_cast<dictionary *>(pAny1);
      dictionary *pDict2 = reinterpret_cast<dictionary *>(pAny2);
      if ( pDict1 -> size() != pDict2 -> size() ) {
         push(pFalseConstant);
         return;
      }
      long n = pDict1 -> size();
      for ( long k = 0; k < n; k++ ) {
         push(pDict1 -> retrieve(pDict1 -> getKey(k)));
         push(pDict2 -> retrieve(pDict2 -> getKey(k)));
         operatorEq();
         if ( pFalseConstant == pop() ) {
            push(pFalseConstant);
            return;
         }
      }
      push(pTrueConstant);
      }
      return;
   
   case object::array: {
      if ( object::array != pAny2 -> ObjectType() ) {
         push(pFalseConstant);
         return;
      }
      array *pDict1 = reinterpret_cast<array *>(pAny1);
      array *pDict2 = reinterpret_cast<array *>(pAny2);
      if ( pDict1 -> size() != pDict2 -> size() ) {
         push(pFalseConstant);
         return;
      }
      long n = pDict1 -> size();
      for ( long k = 0; k < n; k++ ) {
         push(pDict1 -> get(k));
         push(pDict2 -> get(k));
         operatorEq();
         if ( pFalseConstant == pop() ) {
            push(pFalseConstant);
            return;
         }
      }
      push(pTrueConstant);
      }
      return;

   default:
      __debugbreak();
      push(pFalseConstant);
   }

   return;
   }


   void job::operatorErrordict() {
/*
   errordict 
      – errordict dict

   pushes the dictionary object errordict on the operand stack (see Section 3.11, “Errors”).
   errordict is not an operator; it is a name in systemdict associated with the
   dictionary object.
*/
   push(systemDict.retrieve("errordict"));
   return;
   }

   void job::operatorExch() {
/*
   exch 
      any1 any2 exch any2 any1

   exchanges the top two elements on the operand stack.
*/
   object *p2 = pop();
   object *p1 = pop();
   push(p2);
   push(p1);
   return;
   }

   void job::operatorExec() {
/*
   exec 
      any exec –

   pushes the operand on the execution stack, executing it immediately. The effect of
   executing an object depends on the object’s type and literal/executable attribute;
   see Section 3.5, “Execution.” In particular, executing a literal object will cause it
   only to be pushed back on the operand stack. Executing a procedure, however,
   will cause the procedure to be called.

      Examples
         (3 2 add) cvx exec => 5
         3 2 /add exec => 3 2 /add
         3 2 /add cvx exec => 5

   In the first example, the string 3 2 add is made executable and then executed. Executing
   a string causes its characters to be scanned and interpreted according to the
   PostScript language syntax rules.

   In the second example, the literal objects 3, 2, and /add are pushed on the operand
   stack, then exec is applied to /add. Since /add is a literal name, executing it simply
   causes it to be pushed back on the operand stack. The exec operator in this case
   has no useful effect.

   In the third example, the literal name /add on the top of the operand stack is
   made executable by cvx. Applying exec to this executable name causes it to be
   looked up and the add operation to be performed.
*/
   object *pObject = top();

   if ( object::procedure == pObject -> ObjectType() ) {
      procedure *pProcedure = reinterpret_cast<procedure *>(pop());
      pProcedure -> execute();
   } else
      execute();

   return;
   }

   void job::operatorExecuteonly() {
/*
   executeonly 

      array executeonly array
      packedarray executeonly packedarray
      file executeonly file
      string executeonly string

   reduces the access attribute of an array, packed array, file, or string object to
   execute-only (see Section 3.3.2, “Attributes of Objects”). Access can only be reduced
   by this operator, never increased. When an object is execute-only, its value
   cannot be read or modified explicitly by PostScript operators (an invalidaccess
   error will result), but it can still be executed by the PostScript interpreter—for example,
   by invoking it with the exec operator.

   executeonly affects the access attribute only of the object that it returns. If there
   are other composite objects that share the same value, their access attributes are
   unaffected.

   Errors: invalidaccess, stackunderflow, typecheck
   See Also: rcheck, wcheck, xcheck, readonly, noaccess
*/

   object *pObject = top();

   pObject -> SetExecuteOnly(true);

   return;
   }

   void job::operatorExit() {
/*
   exit 
      – exit –

   terminates execution of the innermost, dynamically enclosing instance of a looping
   context without regard to lexical relationship. A looping context is a procedure
   invoked repeatedly by one of the following control operators:

      cshow             forall      pathforall
      filenameforall    kshow       repeat
      for               loop        resourceforall

   exit pops the execution stack down to the level of that operator. The interpreter
   then resumes execution at the next object in normal sequence after that operator.
   exit does not affect the operand stack or dictionary stack. Any objects pushed on
   these stacks during execution of the looping context remain after the context is
   exited.

   If exit would escape from the context of a run or stopped operator, an invalidexit
   error occurs (still in the context of the run or stopped). If there is no enclosing
   looping context, the interpreter prints an error message and executes the built-in
   operator quit. This never occurs during execution of ordinary user programs, because
   they are enclosed by a stopped context.
*/
   hasExited = true;
   return;
   }

   void job::operatorFill() {
/*
   fill 
      – fill –

   paints the area inside the current path with the current color. The nonzero winding
   number rule is used to determine what points lie inside the path
   (see “Nonzero Winding Number Rule” on page 195).
   fill implicitly closes any open subpaths of the current path before painting. Any
   previous contents of the filled area are obscured, so an area can be erased by filling
   it with the current color set to white.
   After filling the current path, fill clears it with an implicit newpath operation. To
   preserve the current path across a fill operation, use the sequence
*/
   return;
   }

   void job::operatorFlush() {
/*
   flush 
      – flush –

   causes any buffered characters for the standard output file to be delivered immediately.
   In general, a program requiring output to be sent immediately, such as
   during real-time, two-way interactions, should call flush after generating that output.
*/
   return;
   }

   void job::operatorFindfont() {
/*
   findfont 

      key findfont font
      key findfont cidfont

   obtains a Font resource instance whose name is key and pushes the instance
   (which may be a font or CIDFont dictionary) on the operand stack (see
   Section 5.1, “Organization and Use of Fonts”). key may be a key previously passed
   to definefont, in which case the Font resource instance associated with key (in the
   font directory) is returned.

   If the Font resource identified by key is not defined in virtual memory, findfont
   takes an action that varies according to the environment in which the PostScript
   interpreter is operating. In some environments, findfont may attempt to read a
   font definition from an external source, such as a file. In other environments,
   findfont substitutes a default font or executes the invalidfont error. findfont is a
   special case of findresource applied to the Font category. See Section 3.9, “Named
   Resources.”

   findfont, like findresource, normally looks first for Font resources defined in local
   VM, then for those defined in global VM. However, if the current VM allocation
   mode is global, findfont considers only Font resources defined in global VM. If
   findfont needs to load a font or CIDFont into VM, it may use either local or global
   VM; see Section 3.9.2, “Resource Categories,” for more information.

   findfont is not an operator, but rather a built-in procedure. It may be redefined
   a PostScript program that requires different strategies for finding fonts.

   Errors: invalidfont, stackunderflow, typecheck
*/

   pop();

   push(pCourier);

#if 0

   for ( std::list<font *>::iterator it = fontList.begin(); it != fontList.end(); it++ ) {

      font *pFont = (*it);

      if ( pFont -> exists("FID") ) {
         object *pName = pFont ->retrieve("FID");
         if ( 0 == strcmp(pName->Name(),pKey->Contents()) ) {
            push(pFont);
            return;
         }
      }

   }

#if 0
   for ( std::list<font *>::iterator it = fontList.begin(); it != fontList.end(); it++ ) {
      font *pFont = (*it);
      if ( ( 0 == strcmp(pFont -> Name(),pKey -> Name()) ) && strlen(pFont -> Name()) == strlen(pKey -> Name()) ) {
         push(pFont);
         return;
      }
   }

   dictionary *pDictionary = dictionaryStack.find(pKey -> Name());
   if ( pDictionary ) {
      push(pDictionary);
      return;
   }
#endif

#if 0
   char szMessage[1024];
   sprintf(szMessage,"operator findfont: invalid font '%s'",pKey -> Name());
   throw invalidfont(szMessage);
   return;
#endif
   
   font *pFont = new font(this,pKey -> Name());
   push(pFont);

#endif
   return;
   }

   void job::operatorFindresource() {
/*
   findresource 
      key category findresource instance

   attempts to obtain a named resource instance in a specified category. category is a
   name object that identifies a resource category, such as Font (see Section 3.9.2,
   “Resource Categories”). key is a name or string object that identifies the resource
   instance. (Names and strings are interchangeable; other types of keys are permitted
   but are not recommended.) If it succeeds, findresource pushes the resource
   instance on the operand stack; this is an object whose type depends on the resource
   category.

   8.2 Operator Details

   findresource first attempts to obtain a resource instance that has previously been
   defined in virtual memory by defineresource. If the current VM allocation mode
   is local, findresource considers local resource definitions first, then global definitions
   (see defineresource). However, if the current VM allocation mode is global,
   findresource considers only global resource definitions.

   If the requested resource instance is not currently defined in VM, findresource attempts
   to obtain it from an external source. The way this is done is not specified
   by the PostScript language; it varies among different implementations and different
   resource categories. The effect of this action is to create an object in VM and
   execute defineresource. findresource then returns the newly created object. If key
   is not a name or a string, findresource will not attempt to obtain an external resource.
   When findresource loads an object into VM, it may use global VM even if the current
   VM allocation mode is local. In other words, it may set the VM allocation
   mode to global (true setglobal) while loading the resource instance and executing
   defineresource. The policy for whether to use global or local VM resides in the
   FindResource procedure for the specific resource category; see Section 3.9.2, “Resource
   Categories.”

   During its execution, findresource may remove the definitions of resource instances
   that were previously loaded into VM by findresource. The mechanisms
   and policies for this depend on the category and the implementation; reclamation
   of resources may occur at times other than during execution of findresource.
   However, resource definitions that were made by explicit execution of defineresource
   are never disturbed by automatic reclamation.

   If the specified resource category does not exist, an undefined error occurs. If the
   category exists but there is no instance whose name is key, an undefinedresource
   error occurs.

   Errors: stackunderflow, typecheck, undefined, undefinedresource
   See Also: defineresource, resourcestatus, resourceforall, undefineresource
*/

   object *pKey = pop();
   object *pCategory = pop();

   for ( std::list<resource *>::iterator it = resourceList.begin(); it != resourceList.end(); it++ ) {

      resource *pResource = (*it);

      if ( 0 == strcmp(pResource -> Name(),pKey -> Contents()) ) {
         push(pResource);
         return;
      }

   }

   pPStoPDF -> queueLog("\n");
   pPStoPDF -> queueLog("NOT IMPLEMENTED: findresource");
   pPStoPDF -> queueLog("\n");

   return;
   }


   void job::operatorFor() {
/*
   for 
      initial increment limit proc for –

   executes the procedure proc repeatedly, passing it a sequence of values from initial
   by steps of increment to limit. The for operator expects initial, increment, and limit to
   be numbers. It maintains a temporary internal variable, known as the control
   variable, which it first sets to initial. Then, before each repetition, it compares the
   control variable to the termination value limit. If limit has not been exceeded, for
   pushes the control variable on the operand stack, executes proc, and adds increment
   to the control variable.

   The termination condition depends on whether increment is positive or negative.
   If increment is positive, for terminates when the control variable becomes greater
   than limit. If increment is negative, for terminates when the control variable becomes
   less than limit. If initial meets the termination condition, for does not execute
   proc at all. If proc executes the exit operator, for terminates prematurely.
   Usually, proc will use the value on the operand stack for some purpose. However,
   if proc does not remove the value, it will remain there. Successive executions of
   proc will cause successive values of the control variable to accumulate on the operand
   stack.
*/
   object *pProc = pop();
   object *pLimit = pop();
   object *pIncrement = pop();
   object *pInitial = pop();

   long initial = atol(pInitial -> Contents());
   long increment = atol(pIncrement -> Contents());
   long limit = atol(pLimit -> Contents());
   long control = initial;

   object *pControl = new object(control);

   while ( ( increment > 0 && control <= limit ) || ( increment < 0 && control >= limit ) ) {
      push(pControl);
      push(pProc);
      execute();
      control += increment;
      pControl -> IntValue(control);
   }

   return;
   }

   void job::operatorForall() {
/*
   forall 
         array proc forall –
         packedarray proc forall –
         dict proc forall –
         string proc forall –

   enumerates the elements of the first operand, executing the procedure proc for
   each element. If the first operand is an array, packed array, or string object, forall
   pushes an element on the operand stack and executes proc for each element in the
   object, beginning with the element whose index is 0 and continuing sequentially.
   In the case of a string, the elements pushed on the operand stack are integers in
   the range 0 to 255, not 1-character strings.

   If the first operand is a dictionary, forall pushes a key and a value on the operand
   stack and executes proc for each key-value pair in the dictionary. The order in
   which forall enumerates the entries in the dictionary is arbitrary. New entries put
   in the dictionary during the execution of proc may or may not be included in the
   enumeration. Existing entries removed from the dictionary by proc will not be encountered
   later in the enumeration.

   If the first operand is empty (that is, has length 0), forall does not execute proc at
   all. If proc executes the exit operator, forall terminates prematurely.
   Although forall does not leave any results on the operand stack when it is finished,
   the execution of proc may leave arbitrary results there. If proc does not remove
   each enumerated element from the operand stack, the elements will accumulate
   there.

   Examples
      0 [ 13 29 3 -8 21 ] { add } forall => 58
      /d 2 dict def
      d /abc 123 put
      d /xyz (test) put
      d { } forall => /xyz (test) /abc 123
   
   Errors: invalidaccess, stackoverflow, stackunderflow, typecheck
   See Also: for, repeat, loop, exit
*/

   procedure *pProc = reinterpret_cast<procedure *>(pop());
   object *pSource = pop();

   switch ( pSource -> ObjectType() ) {
   case object::array: {
      array *pArray = reinterpret_cast<array *>(pSource);
      long n = pArray -> size();
      for ( long k = 0; k < n; k++ ) {
         push(pArray -> get(k));
         push(pProc);
         execute();
      }
      }
      break;

   case object::font:
   case object::dictionary: {
      dictionary *pDictionary = reinterpret_cast<dictionary *>(pSource);
      long n = pDictionary -> size();
      for ( long k = 0; k < n; k++ ) {
         push(pDictionary -> retrieve(pDictionary -> getKey(k)));
         push(pProc);
         execute();
      }
      }
      break;

   case object::string: {
      __debugbreak();
      }
      break;

   default:
      __debugbreak();
      break;
   }

   return;
   }

   void job::operatorGe() {
/*
   ge 
      num1 num2 ge bool
      string1 string2 ge bool

   pops two objects from the operand stack and pushes true if the first operand is
   greater than or equal to the second, or false otherwise. If both operands are numbers,
   ge compares their mathematical values. If both operands are strings, ge
   compares them element by element, treating the elements as integers in the range
   0 to 255, to determine whether the first string is lexically greater than or equal to
   the second. If the operands are of other types or one is a string and the other is a
   number, a typecheck error occurs.
*/
   object *p2 = pop();
   object *p1 = pop();

   if ( object::number == p1 -> ObjectType() && object::number == p2 -> ObjectType() ) {
      double v1 = 0.0;
      if ( object::real == p1 -> ValueType() )
         v1 = p1 -> DoubleValue();
      else
         v1 = (double) p1 -> IntValue();
      double v2 = 0.0;
      if ( object::real == p2 -> ValueType() )
         v2 = p2 -> DoubleValue();
      else
         v2 = (double) p2 -> IntValue();
      if ( v1 >= v2 )
         push(pTrueConstant);
      else 
         push(pFalseConstant);
      return;
   }

   if ( 0 <= strcmp(p1 -> Contents(),p2 -> Contents()) )
      push(pTrueConstant);
   else
      push(pFalseConstant);

   return;
   }

   void job::operatorGet() {
/*
   get 
      array index get any
      packedarray index get any
      dict key get any
      string index get int

   returns a single element from the value of the first operand. If the first operand is
   an array, a packed array, or a string, get treats the second operand as an index and
   returns the element identified by the index, counting from 0. index must be in the
   range 0 to n - 1, where n is the length of the array, packed array, or string. If it is
   outside this range, a rangecheck error occurs.

   If the first operand is a dictionary, get looks up the second operand as a key in the
   dictionary and returns the associated value. If the key is not present in the dictionary,
   an undefined error occurs.

   Errors: invalidaccess, rangecheck, stackunderflow, typecheck, undefined

*/
   object *pIndex = pop();
   object *pTarget = pop();

   switch ( pTarget -> ObjectType() ) {

   case object::array:
      push(reinterpret_cast<array *>(pTarget) -> get(pIndex -> IntValue()));
      return;

   case object::packedarray: {
      __debugbreak();
      }
      break;

   case object::font:
   case object::dictionary: {
      dictionary *pDict = reinterpret_cast<dictionary *>(pTarget);
      object *pObject = pDict -> retrieve(pIndex -> Name());
      if ( ! pObject ) {
         char szError[1024];
         sprintf(szError,"operator get: cannot find %s in font or dictionary %s (type: %s)",pIndex -> Name(),pDict -> Name(),pDict->TypeName());
         throw undefined(szError);
      }
      push(pObject);
      break;
      }

   case object::string: {
      __debugbreak();
      break;
      }

   }

   return;
   }

   void job::operatorGrestore() {
/*
   grestore 
      – grestore –

   resets the current graphics state from the one on the top of the graphics state stack
   and pops the graphics state stack, restoring the graphics state in effect at the time
   of the matching gsave operation. This operator provides a simple way to undo
   complicated transformations and other graphics state modifications without having
   to reestablish all graphics state parameters individually.
   If the topmost graphics state on the stack was saved with save rather than gsave
   (that is, if there has been no gsave operation since the most recent unmatched
   save), grestore restores that topmost graphics state without popping it from the
   stack. If there is no unmatched save (which can happen only during an unencapsulated
   job) and the graphics state stack is empty, grestore has no effect.
*/
   return;
   }

   void job::operatorGt() {
/*
   gt 
      num1 num2 gt bool
      string1 string2 gt bool

   pops two objects from the operand stack and pushes true if the first operand is
   greater than the second, or false otherwise. If both operands are numbers, gt compares
   their mathematical values. If both operands are strings, gt compares them
   element by element, treating the elements as integers in the range 0 to 255, to
   determine whether the first string is lexically greater than the second. If the operands
   are of other types or one is a string and the other is a number, a typecheck
   error occurs.
*/
   object *p2 = pop();
   object *p1 = pop();
   
   if ( object::number == p1 -> ObjectType() && object::number == p2 -> ObjectType() ) {
      double v1 = 0.0;
      if ( object::real == p1 -> ValueType() )
         v1 = p1 -> DoubleValue();
      else
         v1 = (double) p1 -> IntValue();
      double v2 = 0.0;
      if ( object::real == p2 -> ValueType() )
         v2 = p2 -> DoubleValue();
      else
         v2 = (double) p2 -> IntValue();
      if ( v1 > v2 )
         push(pTrueConstant);
      else 
         push(pFalseConstant);
      return;
   }

   if ( 0 < strcmp(p1 -> Contents(),p2 -> Contents()) )
      push(pTrueConstant);
   else
      push(pFalseConstant);

   return;
   }

   void job::operatorGsave() {
/*
   gsave 
      – gsave –

   pushes a copy of the current graphics state on the graphics state stack (see
   Section 4.2, “Graphics State”). All elements of the graphics state are saved, including
   the current transformation matrix, current path, clipping path, and identity of
   the raster output device, but not the contents of raster memory. The saved state
   can later be restored by a matching grestore. After saving the graphics state, gsave
   resets the clipping path stack in the current graphics state to empty.

   The save operator also implicitly performs a gsave operation, but restoring a
   graphics state saved by save is slightly different from restoring one saved by gsave;
   see the descriptions of grestore and grestoreall.
   Note that, unlike save, gsave does not return a save object on the operand stack to
   represent the saved state. gsave and grestore work strictly in a stacklike fashion,
   except for the wholesale restoration performed by restore and grestoreall.
*/
   return;
   }


   void job::operatorIf() {
/*
   if 
      bool proc if –

   removes both operands from the stack, then executes proc if bool is true. The if operator
   pushes no results of its own on the operand stack, but proc may do so (see
   Section 3.5, “Execution”).

   Example
      3 4 lt {(3 is less than 4)} if => (3 is less than 4)

   Errors: stackunderflow, typecheck
   See Also: ifelse
*/

   procedure *pProcedure = reinterpret_cast<procedure *>(pop());
   booleanObject *bo = NULL;
   try {
   bo = reinterpret_cast<booleanObject *>(pop());
   } catch ( typecheck etc ) {
      throw;
   }
   if ( bo == pTrueConstant ) 
      pProcedure -> execute();
   return;
   }

   void job::operatorIfelse() {
/*
   ifelse 
      bool proc1 proc2 ifelse –

   removes all three operands from the stack, then executes proc1 if bool is true or
   proc2 if bool is false. The ifelse operator pushes no results of its own on the operand
*/

   procedure *pProc2 = reinterpret_cast<procedure *>(pop());
   procedure *pProc1 = reinterpret_cast<procedure *>(pop());
   booleanObject *bo = reinterpret_cast<booleanObject *>(pop());

   if ( bo == pTrueConstant )
      pProc1 -> execute();
   else
      pProc2 -> execute();

   return;
   }

   void job::operatorIndex() {
/*
   index 
      anyn … any0 n index anyn … any0 anyn

   removes the nonnegative integer n from the operand stack, counts down to the
   nth element from the top of the stack, and pushes a copy of that element on the
   stack.

   Examples

      (a) (b) (c) (d) 0 index => (a) (b) (c) (d) (d)
      (a) (b) (c) (d) 3 index => (a) (b) (c) (d) (a)

   Errors: rangecheck, stackunderflow, typecheck
   See Also: copy, dup, roll

*/
   object *pCount = pop();

   long n = pCount -> IntValue();

   std::list<object *> replacements;
   for ( long k = 0; k < n; k++ )
      replacements.insert(replacements.end(),pop());

   object *pDuplicate = operandStack.top();

   for ( std::list<object *>::reverse_iterator it = replacements.rbegin(); it != replacements.rend(); it++ )
      push(*it);

   push(pDuplicate);

   replacements.clear();

   return;
   }

void job::operatorIdtransform() {
/*
   idtransform 
      dx1 dy1 idtransform dx2 dy2
      dx1 dy1 matrix idtransform dx2 dy2

   (inverse delta transform) applies the inverse of a transformation matrix to the distance
   vector (dx1, dy1), returning the transformed distance vector (dx2, dy2). The
   first form of the operator uses the inverse of the current transformation matrix in
   the graphics state to transform the distance vector from device space to user space
   coordinates. The second form applies the inverse of the transformation specified
   by the matrix operand rather than that of the CTM.

   A delta transformation is similar to an ordinary transformation (see Section 4.3,
   “Coordinate Systems and Transformations”), but does not use the translation
   components tx and ty of the transformation matrix. The distance vectors are thus
   positionless in both the original and target coordinate spaces, making this operator
*/
   double x,y;
   matrix *pMatrix = NULL;
   switch ( top() -> ObjectType() ) {
   case object::matrix:
      pMatrix = reinterpret_cast<matrix *>(pop());
      y = pop() -> Value();
      x = pop() -> Value();
      break;
   default:
      pMatrix = pCurrentMatrix;
      y = pop() -> Value();
      x = pop() -> Value();
   }
   push(new object(x));
   push(new object(y));
   return;
   }

   void job::operatorISOLatin1Encoding() {
/*
   ISOLatin1Encoding 
      – ISOLatin1Encoding array

   pushes the ISO Latin-1 encoding vector on the operand stack. This is a
   256-element literal array object, indexed by character codes, whose values are the
   character names for those codes. ISOLatin1Encoding is not an operator; it is a
   name in systemdict associated with the array object.

   Latin-text fonts produced by Adobe usually use the StandardEncoding encoding
   vector. However, they contain all the characters needed to support the use of
   ISOLatin1Encoding. A font can have its Encoding array changed to
   ISOLatin1Encoding by means of the procedure shown in Section 5.9.1, “Changing
   the Encoding Vector.” The contents of ISOLatin1Encoding are documented in
   Appendix E.

   Errors: stackoverflow
   See Also: StandardEncoding, findencoding
*/
   push(&ISOLatin1Encoding);
   return;
   }

   void job::operatorItransform() {
/*
   itransform 
      x1 y1 itransform x2 y2
      x1 y1 matrix itransform x2 y2

   (inverse transform) applies the inverse of a transformation matrix to the coordinates
   (x1, y1), returning the transformed coordinates (x2, y2). The first form of the
   operator uses the inverse of the current transformation matrix in the graphics
   state to transform device space coordinates to user space. The second form applies
   the inverse of the transformation specified by the matrix operand rather than that
   of the CTM.
*/
   double x,y;
   matrix *pMatrix = NULL;
   switch ( top() -> ObjectType() ) {
   case object::matrix:
      pMatrix = reinterpret_cast<matrix *>(pop());
      y = pop() -> Value();
      x = pop() -> Value();
      break;
   default:
      pMatrix = pCurrentMatrix;
      y = pop() -> Value();
      x = pop() -> Value();
   }
   push(new object(x));
   push(new object(y));
   return;
   }

   void job::operatorKnown() {
/*
   known 
      dict key known bool

   returns true if there is an entry in the dictionary dict whose key is key; otherwise, it
   returns false. dict does not have to be on the dictionary stack.
*/
   object *pKey = pop();
   dictionary *pDict = reinterpret_cast<dictionary *>(pop());
   if ( pDict -> exists(pKey -> Contents()) )
      push(pTrueConstant);
   else
      push(pFalseConstant);
   return;
   }

   void job::operatorLength() {
/*
   length 
         array length int
         packedarray length int
         dict length int
         string length int
         name length int

   returns the number of elements in the value of its operand if the operand is an
   array, a packed array, or a string. If the operand is a dictionary, length returns the
   current number of entries it contains (as opposed to its maximum capacity, which
   is returned by maxlength). If the operand is a name object, the length returned is
   the number of characters in the text string that defines it.

   Examples

      [1 2 4] length => 3
      [ ] length => 0 % An array of zero length
      /ar 20 array def
      ar length => 20
      /mydict 5 dict def
      mydict length => 0
      mydict /firstkey (firstvalue) put
      mydict length => 1
      (abc\n) length => 4 % Newline (\n) is one character
      ( ) length => 0 % No characters between ( and )
      /foo length => 3

   Errors: invalidaccess, stackunderflow, typecheck
   See Also: maxlength

*/
   object *pItem = pop();
   long length = 0L;

   switch ( pItem -> ObjectType() ) {
   
   case object::array:
      length = reinterpret_cast<array *>(pItem) -> size();
      break;

   case object::font:
   case object::dictionary:
      length = reinterpret_cast<dictionary *>(pItem) -> size();
      break;

   case object::atom: {

      switch ( pItem -> ValueType() ) {
      case object::string:
      case object::character:
         length = (DWORD)strlen(pItem -> Contents());
         break;
      default: 
         char szError[1024];
         sprintf(szError,"in operator length: object = %s, the type is (%s,%s) invalid",pItem -> Name(),pItem -> TypeName(),pItem -> ValueTypeName());
         throw typecheck(szError);
      }
      }
      break;

   default: {
      char szError[1024];
      sprintf(szError,"in operator length: object = %s, the type is (%s,%s) invalid",pItem -> Name(),pItem -> TypeName(),pItem -> ValueTypeName());
      throw typecheck(szError);
      }

   }

   push(new object(length));

   return;
   }

   void job::operatorLineto() {
/*
   lineto 
      x y lineto –

   appends a straight line segment to the current path (see Section 4.4, “Path Construction”),
   starting from the current point and extending to the coordinates
   (x, y) in user space. The endpoint (x, y) becomes the new current point.
   If the current point is undefined because the current path is empty, a nocurrentpoint
   error occurs.
*/
   graphicsStateStack.current() -> lineto();
   return;
   }

   void job::operatorLoad() {
/*
   load 
      key load value

   searches for key in each dictionary on the dictionary stack, starting with the topmost
   (current) dictionary. If key is found in some dictionary, load pushes the associated
   value on the operand stack; otherwise, an undefined error occurs.

   load looks up key the same way the interpreter looks up executable names that it
   encounters during execution. However, load always pushes the associated value
   on the operand stack; it never executes the value.

   Examples
      /avg {add 2 div} def
      /avg load => {add 2 div}

   Errors: invalidaccess, stackunderflow, typecheck, undefined
*/

   object *pKey = top();

   operatorWhere();

   if ( top() == pFalseConstant ) {
      pop();
      push(pKey);
      char szMessage[1024];
      sprintf(szMessage,"operator: load. Value %s is undefined",pKey -> Name());
      throw undefined(szMessage);
   }

   pop();

   dictionary *pDictionary = reinterpret_cast<dictionary *>(pop());

   push(pDictionary -> retrieve(pKey -> Name()));

   return;
   }

   void job::operatorLoop() {
/*
   loop 
      proc loop –

   repeatedly executes proc until proc executes the exit operator, at which point
   interpretation resumes at the object next in sequence after the loop operator.
   Control also leaves proc if the stop operator is executed. If proc never executes exit
   or stop, an infinite loop results, which can be broken only via an external interrupt
   (see interrupt).
*/
   object *pProcedure = pop();
   hasExited = false;
   while ( ! hasExited )
      pProcedure -> execute();

   return;
   }

   void job::operatorMakefont() {
/*

   makefont 

      font matrix makefont font¢
      cidfont matrix makefont cidfont¢

   applies matrix to font or cidfont, producing a new font¢ or cidfont¢ whose glyphs are
   transformed by matrix when they are shown. makefont first creates a copy of font
   or cidfont. Then it replaces the copy’s FontMatrix entry with the result of concatenating
   the existing entry with matrix. It inserts two additional entries, OrigFont
   and ScaleMatrix, whose purpose is internal to the implementation. Finally, it returns
   the result as font¢ or cidfont¢.

   Normally, makefont does not copy subsidiary objects in the dictionary, such as
   the CharStrings and FontInfo subdictionaries; these are shared with the original
   font or CIDFont. However, if font is a composite font, makefont recursively
   copies any descendant Type 0 font dictionaries and updates their FontMatrix entries
   as well. It does not copy descendant base fonts or CIDFonts.

   Showing glyphs from font¢ or cidfont¢ produces the same results as showing from
   font or cidfont after having transformed user space by matrix. makefont is essentially
   a convenience operator that permits the desired transformation to be encapsulated
   in the font or CIDFont description. The most common transformation is
   to scale by a uniform factor in both the x and y dimensions. scalefont is a special
   case of the more general makefont and should be used for such uniform scaling.
   Another operator, selectfont, combines the effects of findfont and makefont.

   The interpreter keeps track of font or CIDFont dictionaries recently created by
   makefont. Calling makefont multiple times with the same font or cidfont and
   matrix will usually return the same result. However, it is usually more efficient for
   a PostScript program to apply makefont only once for each font or CIDFont that
   it needs and to keep track of the resulting dictionaries on its own.

   See Chapter 5 for general information about fonts and CIDFonts, and Section 4.3,
   “Coordinate Systems and Transformations,” for a discussion of transformations.

   The derived dictionary is allocated in local or global VM according to whether the
   original dictionary is in local or global VM. This behavior is independent of the 
   current VM allocation mode.

   Example

   /Helvetica findfont [10 0 0 12 0 0] makefont setfont

   This example obtains the standard Helvetica font, which is defined with a 1-unit
   line height, and scales it by a factor of 10 in the x dimension and 12 in the y
   dimension. This produces a font 12 units high (that is, a 12-point font in default
   user space) whose glyphs are “condensed” in the x dimension by a ratio of 10/12.

   Errors: invalidfont, rangecheck, stackunderflow, typecheck, VMerror
*/
   object *pMatrix = pop();
   object *pFont = pop();

   push(pFont);

   return;
   }

   void job::operatorMakepattern() {
/*
   makepattern 
      dict matrix makepattern pattern

   instantiates the pattern defined by the pattern dictionary dict, producing an instance
   of the pattern locked to the current user space. After verifying that dict is a
   prototype pattern dictionary with all required entries (see Section 4.9, “Patterns”),
   makepattern creates a copy of dict in local VM, adding an Implementation
   entry for use by the PostScript interpreter. Only the contents of dict itself are
   copied; any subsidiary composite objects the dictionary contains are not copied,
   but are shared with the original dictionary.

   makepattern saves a copy of the current graphics state, to be used later when the
   pattern’s PaintProc procedure is called to render the pattern cell. It then modifies
   certain parameters in the saved graphics state, as follows:

      • Concatenates matrix with the saved copy of the current transformation matrix
      • Adjusts the resulting matrix to ensure that the device space can be tiled properly
        with a pattern cell of the given size in accordance with the pattern’s tiling
        type
      • Resets the current path to empty
      • Replaces the clipping path with the pattern cell bounding box specified by the
        pattern dictionary’s BBox entry
      • Replaces the current device with a special one provided by the PostScript
        implementation

   Finally, makepattern makes the new dictionary read-only and returns it on the
   operand stack. The resulting pattern dictionary is suitable for use as an operand to
   setpattern or as a color value in a Pattern color space.
*/
   pop();
   push(new pattern(this));
   return;
   }

   void job::operatorMarkArrayBegin() {
/*
   [ 
      – [ mark

   pushes a mark object on the operand stack (the same as the mark and << operators).
   The customary use of the [ operator is to mark the beginning of an indefinitely
   long sequence of objects that will eventually be formed into a new array
   object by a matching ] operator. See the discussion of array syntax in Section 3.2,
   “Syntax,” and of array construction in Section 3.6, “Overview of Basic Operators.”
*/
   push(new mark(mark::array));
   return;
   }


   void job::operatorMarkArrayEnd() {
/*
   ] 
      mark obj0 … objn-1 ] array

   creates a new array of n elements (where n is the number of elements above the
   topmost mark on the operand stack), stores those elements into the array, and returns
   the array on the operand stack. The ] operator stores the topmost object
   from the stack into element n - 1 of array and the bottommost one (the one immediately
   above the mark) into element 0 of array. It removes all the array elements
   from the stack, as well as the mark object.

   The array is allocated in local or global VM according to the current VM allocation
   mode. An invalidaccess error occurs if the array is in global VM and any of
   the objects obj0 … objn-1 are in local VM. See Section 3.7.2, “Local and Global
   VM.”

      Examples
         [5 4 3] =>  % A three-element array, with elements 5, 4, 3
         mark 5 4 3 counttomark array astore exch pop => % Same as above
         [1 2 add] => % A one-element array, with element 3

   The first two lines of code above have the same effect, but the second line uses
   lower-level array and stack manipulation primitives instead of [ and ].
   In the last example, note that the PostScript interpreter acts on all of the array elements
   as it encounters them (unlike its behavior with the { … } syntax for executable
   array construction), so the add operator is executed before the array is
   constructed.

*/

   std::list<object *> entries;

   object *pObject = pop();

   while ( object::mark != pObject -> ObjectType() ) {
      entries.insert(entries.end(),pObject);
      pObject = pop();
   }

   array *pArray = new array(this,"auto");

   for ( std::list<object *>::reverse_iterator it = entries.rbegin(); it != entries.rend(); it++ ) 
      pArray -> insert(*it);

   push(pArray);

   return;
   }

   void job::operatorMarkDictionaryBegin() {
/*
   << 
      – << mark

   pushes a mark object on the operand stack (the same as the mark and [ operators).
*/
   push(new mark(mark::dictionary));
   return;
   }

   void job::operatorMarkDictionaryEnd() {
/*
   >> 
      mark key1 value1 … keyn valuen >> dict

   creates and returns a dictionary containing the specified key-value pairs. The operands
   are a mark followed by an even number of objects, which the operator uses
   alternately as keys and values to be inserted into the dictionary. The dictionary is
   allocated space for precisely the number of key-value pairs supplied.

   The dictionary is allocated in local or global VM according to the current VM
   allocation mode. An invalidaccess error occurs if the dictionary is in global VM
   and any keys or values are in local VM (see Section 3.7.2, “Local and Global
   VM”). A rangecheck error occurs if there is an odd number of objects above the
   topmost mark on the stack.

   The >> operator is equivalent to the following code:

      counttomark 2 idiv
      dup dict
      begin
         {def} repeat
         pop
         currentdict
      end

   Example

      << /Duplex true
         /PageSize [612 792]
         /Collate false
      >> setpagedevice

   This example constructs a dictionary containing three key-value pairs, which it
   immediately passes to the setpagedevice operator.
*/

   dictionary *pDict = new dictionary(this,"inline");

   while ( object::mark != top() -> ObjectType() ) {
      object *pValue = pop();
      object *pKey = pop();
      pDict -> insert(pKey -> Name(),pValue);
   }

   pop();

   push(pDict);

   return;
   }


   void job::operatorMarkProcedureBegin() {
/*
   { 
      – { mark

   (documentation does not specifically provide this description)
*/

   push(new mark(mark::procedure));

   return;
   }

   void job::operatorMarkProcedureEnd() {

   std::list<object *> entries;

   object *pObject = pop();

   while ( object::mark != pObject -> ObjectType() ) {
      entries.insert(entries.end(),pObject);
      pObject = pop();
   }

   procedure *pProcedure = new procedure(this);

   addProcedure(pProcedure);

   for ( std::list<object *>::reverse_iterator it = entries.rbegin(); it != entries.rend(); it++ ) 
      pProcedure -> insert(*it);

   push(pProcedure);

   return;
   }

   void job::operatorMatrix() {
/*
   matrix 
      – matrix matrix

   returns a six-element array object filled with the identity matrix

         [1 0 0 1 0 0]

   This matrix represents the identity transformation, which leaves all coordinates
   unchanged. The array is allocated in local or global VM according to the current
   VM allocation mode (see Section 3.7.2, “Local and Global VM”).

   Example
      matrix
      6 array identmatrix

   Both lines of code above return the same result on the stack.

*/

   push(new matrix());

   return;
   }


   void job::operatorMoveto() {
/*
   moveto 
      x y moveto –

   starts a new subpath of the current path (see Section 4.4, “Path Construction”) by
   setting the current point in the graphics state to the coordinates (x, y) in user
   space. No new line segments are added to the current path.
   If the previous path operation in the current path was moveto or rmoveto, that
   point is deleted from the current path and the new moveto point replaces it.
*/
   graphicsStateStack.current() -> moveto();
   return;
   }

   void job::operatorMul() {
/*
   mul 
      num1 num2 mul product

   returns the product of num1 and num2. If both operands are integers and the result
   is within integer range, the result is an integer; otherwise, the result is a real
   number.
*/
   object *p2 = pop();
   object *p1 = pop();
   
   if ( object::integer == p1 -> ValueType() && object::integer == p2 -> ValueType() )
      push(new object(p1 -> IntValue() * p2 -> IntValue()));
   else {
      double v1 = p1 -> DoubleValue();
      if ( object::integer == p1 -> ValueType() )
         v1 = (double)p1 -> IntValue();
      double v2 = p2 -> DoubleValue();
      if ( object::integer == p2 -> ValueType() )
         v2 = (double)p2 -> IntValue();
      push(new object(v1 * v2));
   }
   return;
   }

   void job::operatorNe() {
/*
   ne 
      any1 any2 ne bool

   pops two objects from the operand stack and pushes false if they are equal, or true
   if not. What it means for objects to be equal is presented in the description of the
   eq operator.

   Errors: invalidaccess, stackunderflow
   See Also: eq, ge, gt, le, lt
*/

   operatorEq();
   if ( pop() == pFalseConstant )
      push(pTrueConstant);
   else
      push(pFalseConstant);

   return;
   }

   void job::operatorNeg() {
/*
   neg 
      num1 neg num2  

   returns the negative of num1. The type of the result is the same as the type of num1
   unless num1 is the smallest (most negative) integer, in which case the result is a
   real number.
*/
   object *p = pop();
   if ( object::integer == p -> ValueType() )
      push(new object(-1 * p -> IntValue()));
   else
      push(new object(-1.0 * p -> DoubleValue()));
   return;
   }

   void job::operatorNewpath() {
/*
   newpath 
      – newpath –

   initializes the current path in the graphics state to an empty path. The current
   point becomes undefined.
*/
   return;
   }

   void job::operatorNot() {
/*
   not 
      bool1 not bool2
      int1 not int2

   returns the logical negation of the operand if it is boolean. If the operand is an
   integer, not returns the bitwise complement (ones complement) of its binary representation.
*/
//
// TODO: I'm not sure of the "ones complement" operation being ^=
//
   object *pTop = pop();

   if ( object::logical == pTop -> ObjectType() ) {
      if ( pTop == pTrueConstant )
         push(pFalseConstant);
      else
         push(pTrueConstant);
   } else {
      long v = pTop -> IntValue();
      v ^= 0xFFFF;
      push(new object(v));
   }

   return;
   }

   void job::operatorPop() {
   pop();
   return;
   }

   void job::operatorPrint() {
/*
   print
      string print –

   writes the characters of string to the standard output file (see Section 3.8, “File Input
   and Output”). This operator provides the simplest means of sending text to
   an application or an interactive user. Note that print is a file operator; it has nothing
   to do with painting glyphs for characters on the current page (see show) or
   with sending the current page to a raster output device (see showpage).
*/
   operatorStdout();
   return;
   }

   void job::operatorProduct() {
/*
   product 
      – product string

   is a read-only string object that is the name of the product in which the PostScript
   interpreter is running. The value of this string is typically a manufacturer-defined
   trademark; it has no direct connection with specific features of the PostScript
   language.
*/
   push(new string(PRODUCT_NAME));

   return;
   }

   void job::operatorPstack() {
/*
   pstack any1 … anyn pstack any1 … anyn

   writes text representations of every object on the stack to the standard output file,
   but leaves the stack unchanged. pstack applies the == operator to each element of
   the stack, starting with the topmost element. See the == operator for a description
   of its effects.
*/

   std::list<object *> entries;

   while ( operandStack.size() ) 
      entries.insert(entries.end(),pop());

   pPStoPDF -> queueLog("\n");
   for ( std::list<object *>::reverse_iterator it = entries.rbegin(); it != entries.rend(); it++ ) {
      push(*it);
      pPStoPDF -> queueLog("Name:");
      pPStoPDF -> queueLog((*it) -> Name());
      pPStoPDF -> queueLog(", Type:");
      pPStoPDF -> queueLog((*it) -> TypeName());
      pPStoPDF -> queueLog(", ValueType:");
      pPStoPDF -> queueLog((*it) -> ValueTypeName());
      pPStoPDF -> queueLog(", Contents:");
      pPStoPDF -> queueLog((*it) -> Contents());
      pPStoPDF -> queueLog("\n");
   }

   entries.clear();
   
   return;
   }


   void job::operatorPut() {
/*
   put 
      array index any put –
      dict key any put –
      string index int put –

   replaces a single element of the value of the first operand. If the first operand is an
   array or a string, put treats the second operand as an index and stores the third
   operand at the position identified by the index, counting from 0. index must be in
   the range 0 to n - 1, where n is the length of the array or string. If it is outside this
   range, a rangecheck error occurs.

   If the first operand is a dictionary, put uses the second operand as a key and the
   third operand as a value, and stores this key-value pair into dict. If key is already
   present as a key in dict, put simply replaces its value by any; otherwise, put creates
   a new entry for key and associates any with it. In LanguageLevel 1, if dict is already
   full, a dictfull error occurs.

   If the value of array or dict is in global VM and any is a composite object whose
   value is in local VM, an invalidaccess error occurs (see Section 3.7.2, “Local and
   Global VM”).
*/

   object *pValue = pop();
   object *pIndex = pop();
   object *pTarget = pop();

   switch ( pTarget -> ObjectType() ) {

   case object::array:
      reinterpret_cast<array *>(pTarget) -> put(pIndex -> IntValue(),pValue);
      break;

   case object::dictionary:
      reinterpret_cast<dictionary *>(pTarget) -> put(pIndex -> Name(),pValue);
      break;

//
//NTC: 01-05-2011. It is unclear why put is being called with a procedure target but
// for now it is allowed but does nothing
//
   case object::procedure:
//      reinterpret_cast<procedure *>(pTarget) -> put(pIndex -> IntValue(),pValue);
      break;

   case object::matrix:
      reinterpret_cast<matrix *>(pTarget) -> SetValue(pIndex -> IntValue(),pValue -> FloatValue());
      break;

   default:
      __debugbreak();
      break;
   }
   return;
   }

   void job::operatorPutinterval() {
/*
   putinterval 
      array1 index array2 putinterval –
      array1 index packedarray2 putinterval –
      string1 index string2 putinterval –

   replaces a subsequence of the elements of the first operand by the entire contents
   of the third operand. The subsequence that is replaced begins at index in the first
   operand; its length is the same as the length of the third operand.

   The objects are copied from the third operand to the first, as if by a sequence of
   individual get and put operations. In the case of arrays, if the copied elements are
   themselves composite objects, the values of those objects are shared between
   array2 and array1 (see Section 3.3.1, “Simple and Composite Objects”).

   putinterval requires index to be a valid index in array1 or string1 such that index
   plus the length of array2 or string2 is not greater than the length of array1 or string1.

   If the value of array1 is in global VM and any of the elements copied from array2 or
   packedarray2 are composite objects whose values are in local VM, an invalidaccess
   error occurs (see Section 3.7.2, “Local and Global VM”).

   Examples

      /ar [5 8 2 7 3] def
      ar 1 [(a) (b) (c)] putinterval
      ar ==> [5 (a) (b) (c) 3]

      /st (abc) def
      st 1 (de) putinterval
      st ==> (ade)

   Errors: invalidaccess, rangecheck, stackunderflow, typecheck

*/
   object *pObject2 = pop();
   object *pIndex = pop();
   object *pObject1 = pop();

   push(pObject2);
   operatorLength();
   long n = (pop()) -> IntValue();

   long index = pIndex -> IntValue();

   if ( object::array == pObject1 -> ObjectType() || object::packedarray == pObject1 -> ObjectType() ) {

      array *pArray = reinterpret_cast<array *>(pObject1);

      switch ( pObject2 -> ObjectType() ) {
      case object::array:
      case object::packedarray: {
         array *pSource = reinterpret_cast<array *>(pObject2);
         for ( long k = 0; k < n; k++ ) 
            pArray -> put(index++,pSource -> get(k));
         }
         break;

      case object::string: {
         }
         break;

      default: {
         char szMessage[1024];
         sprintf(szMessage,"operator putinterval: the type of object %s is invalid (%s) should be array or string",pObject2 -> Name(),pObject2 -> TypeName());
         throw typecheck(szMessage);
         }
      }

   } else if ( object::atom == pObject1 -> ObjectType() && object::string == pObject1 -> ValueType() ) {

      if ( object::atom != pObject2 -> ObjectType() || object::string != pObject2 -> ValueType() ) {
         char szMessage[1024];
         sprintf(szMessage,"operator putinterval: the type of object %s is invalid (%s) should be string",pObject2 -> Name(),pObject2 -> TypeName());
         throw typecheck(szMessage);
      }

      string *pSource = reinterpret_cast<string *>(pObject2);
      string *pTarget = reinterpret_cast<string *>(pObject1);
      for ( long k = 0; k < n; k++ ) 
         pTarget -> put(index++,pSource -> get(k));

   } else {
      char szMessage[1024];
      sprintf(szMessage,"operator putinterval: the type of object %s is invalid (%s) should be array, packedarray, or string",pObject1 -> Name(),pObject1 -> TypeName());
      throw typecheck(szMessage);
   }

   return;
   }


   void job::operatorReadonly() {
/*
   readonly

      array readonly array
      packedarray readonly packedarray
      dict readonly dict
      file readonly file
      string readonly string

   reduces the access attribute of an array, packed array, dictionary, file, or string object
   to read-only (see Section 3.3.2, “Attributes of Objects”). Access can only be
   reduced by this operator, never increased. When an object is read-only, its value
   cannot be modified by PostScript operators (an invalidaccess error will result),
   but it can still be read by operators or executed by the PostScript interpreter.
   For an array, packed array, file, or string object, readonly affects the access attribute
   only of the object that it returns. If there are other objects that share the
   same value, their access attributes are unaffected. However, in the case of a dictionary,
   readonly affects the value of the object, so all dictionary objects sharing
   the same dictionary are affected.

*/
   return;
   }

   void job::operatorRectclip() {
/*
   rectclip 
      x y width height rectclip –
      numarray rectclip –
      numstring rectclip –

   intersects the area inside the current clipping path with a rectangular path defined
   by the operands to produce a new, smaller clipping path. In the first form, the operands
   are four numbers that define a single rectangle. In the other two forms, the
   operand is an array or an encoded number string that defines an arbitrary number
   of rectangles (see Sections 3.14.5, “Encoded Number Strings,” and 4.6.5,
   “Rectangles”). After computing the new clipping path, rectclip clears the current
   path with an implicit newpath operation.

   Assuming width and height are positive, the first form of the operator is equivalent
   to the following code:

*/
   object *pTop = pop();
   switch ( pTop -> ObjectType() ) {
   case object::number:
      pop();
      pop();
      pop();
      break;
   default:
      break;
   }
   return;
   }

   void job::operatorRectfill() {
/*
   rectfill 
      x y width height rectfill –
      numarray rectfill –
      numstring rectfill –

   paints the area inside a path consisting of one or more rectangles defined by the
   operands, using the current color. In the first form, the operands are four numbers
   that define a single rectangle. In the other two forms, the operand is an array
   or an encoded number string that defines an arbitrary number of rectangles (see
   Sections 3.14.5, “Encoded Number Strings,” and 4.6.5, “Rectangles”). rectfill
   neither reads nor alters the current path in the graphics state.

   Assuming width and height are positive, the first form of the operator is equivalent
   to the following code:
*/
   object *pTop = pop();
   switch ( pTop -> ObjectType() ) {
   case object::array:
      return;
   default:
      switch ( pTop -> ValueType() ) {
      case object::string:
         return;
      default:
         pop(); 
         pop();
         pop();
      }
   }
   return;
   }

   void job::operatorRectstroke() {
/*
   rectstroke 
      x y width height rectstroke –
      x y width height matrix rectstroke –
      numarray rectstroke –
      numarray matrix rectstroke –
      numstring rectstroke –
      numstring matrix rectstroke –

   strokes a path consisting of one or more rectangles defined by the operands. In
   the first two forms, the operands x, y, width, and height are four numbers that define
   a single rectangle. In the remaining forms, numarray or numstring is an array
   or an encoded number string that defines an arbitrary number of rectangles (see
   Sections 3.14.5, “Encoded Number Strings,” and 4.6.5, “Rectangles”). rectstroke
   neither reads nor alters the current path in the graphics state.

   The forms of the operator that include a matrix operand concatenate it to the current
   transformation matrix after defining the path, but before stroking it. The resulting
   matrix affects the line width and dash pattern, if any, but not the path
   itself.

   Assuming width and height are positive, the first two forms of the operator are
   equivalent to the following code:

*/
   object *pTop = pop();
   switch ( pTop -> ObjectType() ) {

   case object::matrix: {
      object *pNext = pop();
      switch ( pNext -> ObjectType() ) {
      case object::number:
         pop();
         pop();
         pop();
         break;
      default:
         break;
      }
      }

   case object::number:
      pop();
      pop();
      pop();
      break;

   default:
      break;
   }

   return;
   }

   void job::operatorRepeat() {
/*
   repeat 
      int proc repeat –

   executes the procedure proc int times, where int is a nonnegative integer. This operator
   removes both operands from the stack before executing proc for the first
   time. If proc executes the exit operator, repeat terminates prematurely. repeat
   leaves no results of its own on the stack, but proc may do so.
*/
   procedure *pProcedure = reinterpret_cast<procedure *>(pop());
   object *pCount = pop();

   for ( long k = 0; k < pCount -> IntValue(); k++ )
      pProcedure -> execute();

   return;
   }

   void job::operatorRlineto() {
/*
   rlineto 
      dx dy rlineto –

   (relative lineto) appends a straight line segment to the current path (see
   Section 4.4, “Path Construction”), starting from the current point and extending
   dx user space units horizontally and dy units vertically. That is, the operands dx
   and dy are interpreted as relative displacements from the current point rather
   than as absolute coordinates. In all other respects, the behavior of rlineto is identical
   to that of lineto.

   If the current point is undefined because the current path is empty, a
   nocurrentpoint error occurs.
*/
   pop();
   pop();
   return;
   }

   void job::operatorRmoveto() {
/*
   rmoveto 
      dx dy rmoveto –

   (relative moveto) starts a new subpath of the current path (see Section 4.4, “Path
   Construction”) by displacing the coordinates of the current point dx user space
   units horizontally and dy units vertically, without connecting it to the previous
   current point. That is, the operands dx and dy are interpreted as relative displacements
   from the current point rather than as absolute coordinates. In all other respects,
   the behavior of rmoveto is identical to that of moveto.
   If the current point is undefined because the current path is empty, a
   nocurrentpoint error occurs.

*/
   pop();
   pop();
   return;
   }


   void job::operatorRoll() {
/*
   roll 
      ANYn-1 … ANY0 n j roll ANY(j-1)mod.n … ANY0 ANYn-1 … ANYj.mod.n

   performs a circular shift of the objects anyn-1 through any0 on the operand stack
   by the amount j. Positive j indicates upward motion on the stack, whereas negative
   j indicates downward motion.

   n must be a nonnegative integer and j must be an integer. roll first removes these
   operands from the stack; there must be at least n additional elements. It then performs
   a circular shift of these n elements by j positions.

   If j is positive, each shift consists of removing an element from the top of the stack
   and inserting it between element n - 1 and element n of the stack, moving all intervening 
   elements one level higher on the stack. If j is negative, each shift consists
   of removing element n - 1 of the stack and pushing it on the top of the stack,
   moving all intervening elements one level lower on the stack.

         Examples
         (a) (b) (c) 3 -1 roll  (b) (c) (a)
          0   1   2              1   2   0

         (a) (b) (c) (d) 4 -2 roll  (c) (d) (a) (b)
          0   1   2   3              2   3   0   1

         (a) (b) (c) 3 1 roll  (c) (a) (b)
          0   1   2             2   0   1

         (a) (b) (c) (d) 4 2 roll (c) (d) (a) (b)
          0   1   2   3            2   3   0   1

         (a) (b) (c) 3 0 roll  (a) (b) (c)

*/
   long j = atol(pop() -> Contents());
   long n = atol(pop() -> Contents());

   object **pObjects = new object *[n];
   long *pIndex = new long[n + 1];

   for ( long k = 0; k < n; k++ ) {
      pObjects[n - k - 1] = pop();
      pIndex[k] = n - k - 1;
   }

   pIndex[n] = -1L;

   if ( j > 0 ) {

      for ( long k = 0; k < j; k++ ) {
         
         long topOfStack = pIndex[0];

         for ( long sp = 0; sp < n; sp++ )
            pIndex[sp] = pIndex[sp + 1];

         pIndex[n - 1] = topOfStack;

      }

   } else {

      for ( long k = 0; k < -j; k++ ) {

         long bottomOfStack = pIndex[n - 1];

         for ( long sp = n; sp > 0; sp-- )
            pIndex[sp] = pIndex[sp - 1];

         pIndex[0] = bottomOfStack;

      }

   }

   for ( long k = n - 1; k >= 0; k-- )
      push(pObjects[pIndex[k]]);

   delete [] pObjects;
   delete [] pIndex;

   return;
   }

   void job::operatorRotate() {
/*
   rotate 
      angle rotate –
      angle matrix rotate matrix

   rotates the axes of the user coordinate space by angle degrees counterclockwise
   about the origin, or returns a matrix representing this transformation. The position
   of the coordinate origin and the sizes of the coordinate units are unaffected.

   The transformation is represented by the matrix

                     | cosA  sinA   0 |
               R  =  | -sinA cosA   0 |
                     | 0     0      1 |


   where A is the angle specified by the angle operand. The first form of the operator
   applies this transformation to the user coordinate system by concatenating matrix
   R with the current transformation matrix (CTM); that is, it replaces the CTM
   with the matrix product R X CTM. The second form replaces the value of the
   matrix operand with an array representing matrix R and pushes the result back on
   the operand stack without altering the CTM. See Section 4.3.3, “Matrix Representation
   and Manipulation,” for a discussion of how matrices are represented as
   arrays.

*/
   object *pTop = pop();

   if ( object::matrix == pTop -> ObjectType() )
      pop();

   if ( object::matrix == pTop -> ObjectType() )
      push(pTop);

   return;
   }

   void job::operatorRound() {
/*
   round 
      num1 round num2

   returns the integer value nearest to num1. If num1 is equally close to its two nearest
   integers, round returns the greater of the two. The type of the result is the same as
   the type of the operand.

      Examples
         3.2 round => 3.0
         6.5 round => 7.0
         -4.8 round => -5.0
         -6.5 round => -6.0
         99 round => 99
*/
   if ( object::integer == top() -> ValueType() )
      push(pop());
   else {
      double v = pop() -> Value();
      if ( v > 0.0 ) {
         if ( v - (long)v >= 0.5 )
            push(new object((double)((long)v + 1.0)));
         else
            push(new object((double)((long)v)));
      } else {
         if ( -v + (long)v >= 0.5 )
            push(new object((double)((long)v - 1.0)));
         else
            push(new object((double)((long)v)));

      }
   }
   return;
   }

   void job::operatorSave() {
/*
   save 
      – save save

   creates a snapshot of the current state of virtual memory (VM) and returns a save
   object representing that snapshot. The save object is composite and logically belongs
   to the local VM, regardless of the current VM allocation mode.
   Subsequently, the returned save object may be presented to restore to reset VM to
   this snapshot. See Section 3.7, “Memory Management,” for a description of VM
   and of the effects of save and restore. See the restore operator for a detailed description
   of what is saved in the snapshot.
   save also saves the current graphics state by pushing a copy of it on the graphics
   state stack in a manner similar to gsave. This saved graphics state is restored by
   restore and grestoreall.
*/
   push(new save());
   return;
   }

   void job::operatorScale() {
/*
   scale 
      sx sy scale –
      sx sy matrix scale matrix

   scales the units of the user coordinate space by a factor of sx units horizontally and
   sy units vertically, or returns a matrix representing this transformation. The position
   of the coordinate origin and the orientation of the axes are unaffected.
   The transformation is represented by the matrix

                     | sx 0   0 |
               S  =  | 0  sy  0 |
                     | 0  0   1 |

   The first form of the operator applies this transformation to the user coordinate
   system by concatenating matrix S with the current transformation matrix (CTM);
   that is, it replaces the CTM with the matrix product S ´ CTM. The second form
   replaces the value of the matrix operand with an array representing matrix S and
   pushes the result back on the operand stack without altering the CTM. See
   Section 4.3.3, “Matrix Representation and Manipulation,” for a discussion of how
   matrices are represented as arrays.

*/
   object *pTop = pop();

   if ( object::matrix == pTop -> ObjectType() )
      pop();

   pop();

   if ( object::matrix == pTop -> ObjectType() )
      push(pTop);
   
   return;
   }

   void job::operatorScalefont() {
/*
   scalefont 
         font scale scalefont font
         cidfont scale scalefont cidfont

   applies the scale factor scale to font or cidfont, producing a new font¢ or cidfont¢
   whose glyphs are scaled by scale (in both the x and y dimensions) when they are
   shown. scalefont first creates a copy of font or cidfont. Then it replaces the copy’s
   FontMatrix entry with the result of scaling the existing FontMatrix by scale. It inserts
   two additional entries, OrigFont and ScaleMatrix, whose purpose is internal
   to the implementation. Finally, it returns the result as font¢ or cidfont¢.
   Showing glyphs from font¢ or cidfont¢ produces the same results as showing from
   font or cidfont after having scaled user space by scale in by means of the scale operator.
   scalefont is essentially a convenience operator that enables the desired scale
   factor to be encapsulated in the font or CIDFont description. Another operator,
   makefont, performs more general transformations than simple scaling. See the
   description of makefont for more information on how the transformed font is derived.
   selectfont combines the effects of findfont and scalefont.

*/
   float sf = (float)atol(pop() -> Name());
   font *pFont = reinterpret_cast<font *>(pop());
   pFont -> scalefont(sf);
   push(pFont);
   return;
   }

   void job::operatorSelectfont() {
/*
   selectfont 

      key scale selectfont –
      key matrix selectfont –

      obtains a Font resource instance whose name is key, transforms the instance
      (which may be a font or CIDFont dictionary) according to scale or matrix, and establishes
      it as the font parameter in the graphics state. selectfont is equivalent to
      one of the following, according to whether the second operand is a number or a
      matrix:

         key findfont scale scalefont setfont
         key findfont matrix makefont setfont

      If the Font resource instance named by key is already defined in virtual memory,
      selectfont obtains the corresponding dictionary directly and does not execute
      findfont. However, if the Font resource instance is not defined, selectfont invokes
      findfont. In the latter case, it actually executes the name object findfont, so it uses
      the current definition of that name in the environment of the dictionary stack. On
      the other hand, redefining scalefont, makefont, or setfont would not alter the behavior
      of selectfont.

      selectfont can give rise to any of the errors possible for the component operations,
      including arbitrary errors from a user-defined findfont procedure.
      Example

         /Helvetica 10 selectfont % More efficient
         /Helvetica findfont 10 scalefont setfont

      Both lines of code above have the same effect, but the first one is almost always
      more efficient.

      Errors: invalidfont, rangecheck, stackunderflow, typecheck
      See Also: findfont, makefont, scalefont, setfont
*/
#if 1
   pop();
   pop();
#else
   object *pScaleOrMatrix = pop();
   object *pKey = pop();

   switch ( pScaleOrMatrix -> ObjectType() ) {
   case object::number:
      graphicsStateStack.current() -> setFontScale(pScaleOrMatrix -> FloatValue());
      break;

   default:
_asm {
   int 3;
}
      break;
   }
#endif
   return;
   }

   void job::operatorSetcolor() {
/*
   setcolor
 
      comp1 … compn setcolor –
      pattern setcolor –
      comp1 … compn pattern setcolor –

   sets the current color in the graphics state.

   The appropriate form of the operator depends on the current color space. All
   color spaces except Pattern use the first form, in which the operands comp1
   through compn specify the values of the color components describing the desired
   color. The number of components and the valid ranges of their values depend on
   the specific characteristics of the color space; see Section 4.8, “Color Spaces.” (In
   the case of an Indexed color space, the single operand comp1 is actually an index
   into the space’s color table rather than a true color component.) If the wrong
   number of components is specified, an error will occur, such as stackunderflow or
   typecheck. If a component value is outside the valid range, the nearest valid value
   will be substituted without error indication.

   The second and third forms of setcolor are used when the current color space is a
   Pattern space. In both forms, the pattern operand is a pattern dictionary describing
   the pattern to be established as the current color. The values of the dictionary’s
   PatternType and PaintType entries determine whether additional operands
   are needed:

      • Shading patterns (PatternType 2) or colored tiling patterns (PatternType 1,
        PaintType 1) use the second form of the operator, in which the pattern dictionary
        is the only operand.

      • Uncolored tiling patterns (PatternType 1, PaintType 2) use the third form, in
        which the dictionary is accompanied by one or more component values in the
        pattern’s underlying color space, defining the color in which the pattern is to
        be painted.

   The setcolorspace operator initializes the current color to a value that depends on
   the specific color space selected.

   Execution of this operator is not permitted in certain circumstances; see
   Section 4.8.1, “Types of Color Space.”
*/

   long size = pCurrentColorSpace -> ParameterCount();

   pCurrentColorSpace -> clear();

   for ( long k = 0; k < size; k++ )
      pCurrentColorSpace -> insert(pop());

   return;
   }

   void job::operatorSetcolorspace() {
/*
   setcolorspace 
      array setcolorspace –
      name setcolorspace –

   sets the current color space in the graphics state. It also initializes the current
   color to a value that depends on the specific color space selected. The initial value
   of the current color space is DeviceGray.
   In the first form of the operator, the color space is specified by an array of the
   form

         [family param1 … paramn]

   where family is the name of the color space family and the parameters
   param1 through paramn further describe the space within that family. The number
   and meanings of these parameters vary depending on the family; see Section 4.8,
   “Color Spaces,” for details.

   In the second form, the color space is specified by its family name only. This is allowed
   only for those color space families that require no parameters: DeviceGray,
   DeviceRGB, DeviceCMYK, and Pattern. Specifying a color space by name is equivalent
   to specifying it by a one-element array containing just that name with no
   other parameters.

   Execution of this operator is not permitted in certain circumstances; see
   Section 4.8.1, “Types of Color Space.”
*/

   object *pTop = pop();

   if ( object::array == pTop -> ObjectType() )
      pCurrentColorSpace = new colorSpace(this,reinterpret_cast<array *>(pTop));
   else
      pCurrentColorSpace = new colorSpace(this,pTop -> Name());

   return;
   }

   void job::operatorSetdash() {
/*
   setdash 
      array offset setdash –

   sets the dash pattern parameter in the graphics state. This parameter controls the
   lines to be drawn by subsequent invocations of stroke and related operators, such
   as rectstroke and ustroke. An empty (zero-length) array operand denotes solid,
   unbroken lines. If array is not empty, its elements (which must be nonnegative
   numbers and not all zero) define the sequence of dashes and gaps constituting the
   dash pattern.

   The elements of array alternately specify the length of a dash and the length of a
   gap between dashes, expressed in units of the user coordinate system. The stroke
   operator uses these elements cyclically; when it reaches the end of the array, it
   starts again at the beginning.

   Dashed strokes wrap around curves and corners in the same way as solid strokes.
   The ends of each dash are treated with the current line cap, and corners within a
   dash are treated with the current line join. stroke takes no measures to coordinate
   the dash pattern with features of the path itself; it simply dispenses dashes along
   the path in the pattern defined by array.

   The offset operand can be thought of as the “phase” of the dash pattern relative to
   the start of the path. It is interpreted as a distance into the dash pattern (measured
   in user space units) at which to start the pattern. Before beginning to stroke a
   path, the stroke operator cycles through the elements of array, adding up distances
   and alternating dashes and gaps as usual, but without generating any output.
   When the accumulated distance reaches the value specified by offset, it begins
   stroking from the starting point of the path, using the dash pattern from the point
   that has been reached. Each subpath of a path is treated independently; the dash
   pattern is restarted and the offset reapplied at the beginning of each subpath.

   Examples
      [ ] 0 setdash % Solid, unbroken lines
      [3] 0 setdash % 3 units on, 3 units off, …
      [2] 1 setdash % 1 on, 2 off, 2 on, 2 off, …
      [2 1] 0 setdash % 2 on, 1 off, 2 on, 1 off, …
      [3 5] 6 setdash % 2 off, 3 on, 5 off, 3 on, 5 off, …
      [2 3] 11 setdash % 1 on, 3 off, 2 on, 3 off, 2 on, …

*/
   pop();
   pop();
   return;
   }

   void job::operatorSetfont() {
/*
   setfont 
      font setfont –
      cidfont setfont –

   establishes font or cidfont as the font parameter in the graphics state (subsequently
   returned by rootfont). This in turn determines the current font or CIDFont (returned
   by currentfont)—the font to be used by subsequent glyph operators, such
   as show and stringwidth, or the CIDFont to be used by a subsequent glyphshow
   operator. The operand must be a valid font or CIDFont dictionary. See
   Section 5.1, “Organization and Use of Fonts.”
*/

   pop();

   return;
   }

   void job::operatorSetglobal() {
/*
   setglobal 
      bool setglobal –

   sets the VM allocation mode: true denotes global, false denotes local. This controls
   the VM region in which the values of new composite objects are to be allocated
   (see Section 3.7, “Memory Management”). It applies to objects created implicitly
   by the scanner and to those created explicitly by PostScript operators.

   Modifications to the VM allocation mode are subject to save and restore. In an
   interpreter that supports multiple execution
*/

   object *pValue = pop();

   bool bValue = false;
   if ( pValue == pTrueConstant )
      bValue = true;
   else if ( pValue == pFalseConstant )
      bValue = false;
   else {
      if ( 0 == strcmp("true",pValue -> Contents()) )
         bValue = true;
      else
         bValue = false;
   }

   isGlobalVM = bValue;

   return;
   }


   void job::operatorSetgray() {
/*
   setgray 
      num setgray –

   sets the current color space in the graphics state to DeviceGray and the current
   color to the gray level specified by num. The gray level must be a number in the
   range 0.0 to 1.0, with 0.0 denoting black and 1.0 denoting white. If num is outside
   this range, the nearest valid value is substituted without error indication.
   Execution of this operator is not permitted in certain circumstances; see
   Section 4.8.1, “Types of Color Space.”
*/
   pop();
   return;
   }

   void job::operatorSetlinecap() {
/*
   setlinecap 
      int setlinecap –

   sets the line cap parameter in the graphics state to int, which must be 0, 1, or 2.
   This parameter controls the shape to be painted at the ends of open subpaths (and
   dashes, if any) by subsequent invocations of stroke and related operators, such as
   ustroke (see Section 4.5.1, “Stroking”). Possible values are as follows (see
   Figure 8.9):

      0 Butt cap. The stroke is squared off at the endpoint of the path. There is no
      projection beyond the end of the path.

      1 Round cap. A semicircular arc with a diameter equal to the line width is
      drawn around the endpoint and filled in.

      2 Projecting square cap. The stroke continues beyond the endpoint of the
      path for a distance equal to half the line width and is then squared off.

*/
   graphicsStateStack.current() -> setLineCap(pop() -> IntValue());
   return;
   }

   void job::operatorSetlinejoin() {
/*
   setlinejoin 
      int setlinejoin –

   sets the line join parameter in the graphics state to int, which must be 0, 1, or 2.
   This parameter controls the shape to be painted at corners by subsequent invocations
   of stroke and related operators, such as rectstroke and ustroke (see
   Section 4.5.1, “Stroking”). Possible values are as follows (see Figure 8.10):

      0 Miter join. The outer edges of the strokes for the two segments are
      extended until they meet at an angle, as in a picture frame. If the segments
      meet at too sharp an angle (as defined by the miter limit parameter—see
      setmiterlimit), a bevel join is used instead.
      Butt cap Round cap Projecting square cap

      1 Round join. A circular arc with a diameter equal to the line width is drawn
      around the point where the two segments meet and is filled in, producing
      a rounded corner. stroke draws a full circle at this point; if path segments
      shorter than half the line width meet at sharp angles, an unintended
      “wrong side” of this circle may appear.

      2 Bevel join. The two segments are finished with butt caps (see setlinecap),
      and the resulting notch beyond the ends of the segments is filled with a
      triangle.

   Join styles are significant only at points where consecutive segments of a path
   connect at an angle. Segments that meet or intersect fortuitously receive no special
   treatment. Curved segments are actually rendered as sequences of straight line
   segments, and the current line join is applied to the “corners” between these segments.
   However, for typical values of the flatness parameter (see setflat), the corners
   are so shallow that the difference between join styles is not visible.
*/
   pop();
   return;
   }

   void job::operatorSetlinewidth() {
/*
   setlinewidth 
      num setlinewidth –

   sets the line width parameter in the graphics state to num. This parameter controls
   the thickness of lines to be drawn by subsequent invocations of stroke and related
   operators, such as rectstroke and ustroke. When stroking a path, stroke paints all
   points whose perpendicular distance from the path in user space is less than or
   equal to half the absolute value of num. The effect produced in device space depends
   on the current transformation matrix (CTM) in effect at the time the path
   is stroked. If the CTM specifies scaling by different factors in the x and y dimen-
   sions, the thickness of stroked lines in device space will vary according to their
   orientation.

   A line width of 0 is acceptable, and is interpreted as the thinnest line that can be
   rendered at device resolution—1 device pixel wide. However, some devices cannot
   reproduce 1-pixel lines, and on high-resolution devices, they are nearly invisible.
   Since the results of rendering such “zero-width” lines are device-dependent,
   their use is not recommended.

   The actual line width achieved by stroke can differ from the requested width by as
   much as 2 device pixels, depending on the positions of lines with respect to the
   pixel grid. Automatic stroke adjustment (see setstrokeadjust) can be used to ensure
   uniform line width.
*/
   pop();
   return;
   }

   void job::operatorSetmatrix() {
/*
   setmatrix 
      matrix setmatrix –

   sets the current transformation matrix (CTM) in the graphics state to matrix without
   reference to the former CTM. Except in device setup procedures, the use of
   this operator should be very rare. PostScript programs should ordinarily modify
   the CTM with the translate, scale, rotate, and concat operators rather than replace
   it.
*/
   object *pObject = pop();
   return;
   }

   void job::operatorSetmiterlimit() {
/*
   setmiterlimit 
      num setmiterlimit –

   sets the miter limit parameter in the graphics state to num, which must be a number
   greater than or equal to 1. This parameter controls the treatment of corners by
   stroke and related operators, such as rectstroke and ustroke (see Section 4.5.1,
   “Stroking”), when miter joins have been specified by setlinejoin. When path segments
   connect at a sharp angle, a miter join will result in a spike that extends well
   beyond the connection point. The purpose of the miter limit is to cut off such
   spikes when they become objectionably long.

   At any given corner, the miter length is the distance from the point at which the
   inner edges of the strokes intersect to the point at which their outer edges intersect
   (see Figure 8.11). This distance increases as the angle between the segments
   decreases. If the ratio of the miter length to the line width exceeds the specified
   miter limit, the stroke operator treats the corner with a bevel join instead of a
   miter join.

   The ratio of miter length to line width is directly related to the angle j between
   the segments in user space by the following formula:

            miterLength / lineWidth = 1.0 / (sin(phi/2))

   Example miter limit values are:

      • 1.414 cuts off miters (converts them to bevels) at angles less than 90 degrees.
      • 2.0 cuts off miters at angles less than 60 degrees.
      • 10.0 cuts off miters at angles less than 11 degrees.
      • 1.0 cuts off miters at all angles, so that bevels are always produced even when
      miters are specified.

   The default value of the miter limit is 10.0.
*/
   pop();
   return;
   }

   void job::operatorSetpagedevice() {
/*
   setpagedevice 
      dict setpagedevice –

   modifies the contents of the page device dictionary in the graphics state based on
   the contents of the dictionary operand. The operand is a request dictionary containing
   requested new values for one or more page device parameters. If valid for
   the current page device, these requested values are merged by setpagedevice into
   the current page device dictionary. The interpretation of these parameters is described
   in Section 6.2, “Page Device Parameters.”

   The results of setpagedevice are cumulative. The request dictionary for any given
   invocation is not required to include any particular keys; parameter values established
   in previous invocations will persist unless explicitly overridden. This
   cumulative behavior applies not only to the top-level dictionary, but also recursively
   to the subdictionaries InputAttributes, OutputAttributes, and Policies, as
   well as to some types of details dictionaries.

   The result of executing setpagedevice is to instantiate a page device dictionary,
   perform the equivalent of initgraphics and erasepage, and install the new device
   dictionary as an implicit part of the graphics state. The effects of setpagedevice
   are subject to save and restore, gsave and grestore, and setgstate.

   setpagedevice can be used by system administrators to establish a default state for
   a device by invoking it as part of an unencapsulated job (see Section 3.7.7, “Job
   Execution Environment”). This default state persists until the next restart of the
   PostScript interpreter. Some PostScript implementations store some of the device
   parameters in persistent storage when setpagedevice is executed as part of an unencapsulated
   job, making those parameters persist through interpreter restart.

   setpagedevice reinitializes everything in the graphics state except the font parameter,
   including parameters not affected by initgraphics. Device-dependent rendering
   parameters, such as the halftone screen, transfer functions, flatness tolerance,
   and color rendering dictionary, are reset to built-in default values or to ones provided
   in the Install procedure of the page device dictionary.

   When the current device in the graphics state is not a page device—for example,
   after nulldevice has been invoked or when an interactive display device is active—
   setpagedevice creates a new device dictionary from scratch before merging in the
   parameters from dict. The contents of this dictionary are implementationdependent.

   If a device’s BeginPage or EndPage procedure invokes setpagedevice, an
   undefined error occurs.
*/
   pop();
   return;
   }

   void job::operatorSetuserparams() {
/*
   setuserparams 
      dict setuserparams –

   attempts to set one or more user parameters whose keys and new values are contained
   in the dictionary dict. The dictionary is merely a container for key-value
   pairs; setuserparams reads the information from the dictionary but does not retain
   the dictionary itself. User parameters whose keys are not mentioned in the
   dictionary are left unchanged.

   Each parameter is identified by a key, which is always a name object. If the named
   user parameter does not exist in the implementation, it is ignored. If a specified
   numeric value is not achievable by the implementation, the nearest achievable
   value is substituted without error indication.

   String values should consist of nonnull characters; if a null character is present, it
   will terminate the string. String-valued parameters may be subject not only to the
   general implementation limit on strings (noted in Appendix B) but also to
   implementation-dependent limits specific to certain parameters. If either limit is
   exceeded, a limitcheck error occurs.

   The names of user parameters and details of their semantics are given in
   Appendix C. Additional parameters are described in the PostScript Language Reference
   Supplement and in product-specific documentation. Some user parameters
   have default values that are system parameters with the same names. These defaults
   can be set by setsystemparams.

   User parameters, unlike system parameters, can be set without supplying a password.
   Alterations to user parameters are subject to save and restore. In an interpreter
   that supports multiple execution contexts, user parameters are maintained
   separately for each context.

      Example
         << /MaxFontItem 7500 >> setuserparams

   This example attempts to set the MaxFontItem user parameter to 7500.
*/
   dictionary *pDict = reinterpret_cast<dictionary *>(pop());
   return;
   }

   void job::operatorShow() {
/*
   show 
      string show –

   paints glyphs for the characters identified by the elements of string on the current
   page starting at the current point, using the font face, size, and orientation specified
   by the current font (as returned by currentfont). The spacing from each glyph
   to the next is determined by the glyph’s width, which is an (x, y) displacement
   that is part of the glyph description. When it is finished, show adjusts the current
   point in the graphics state by the sum of the widths of all the glyphs shown. show
   requires that the current point initially be defined (for example, by moveto);
   otherwise, a nocurrentpoint error occurs.

   If a character code would index beyond the end of the font’s Encoding array, or if
   the character mapping algorithm goes out of bounds in other ways, a rangecheck
   error occurs.

   See Chapter 5 for complete information about the definition, manipulation, and
   rendition of fonts.
*/
   string *pString = reinterpret_cast<string *>(pop());
   graphicsStateStack.current() -> addText(pString -> Contents());
   return;
   }


   void job::operatorShowpage() {
/*
   showpage 
      – showpage –

   transmits the contents of the current page to the current output device, causing
   any marks painted on the page to be rendered on the output medium. showpage
   then erases the current page and reinitializes the graphics state in preparation for
   composing the next page. (The actions of showpage may be modified by the
   EndPage procedure, as discussed below.)

   If the current device is a page device that was installed by setpagedevice
   (LanguageLevel 2), the precise behavior of showpage is determined by the values
   of parameters in the page device dictionary (see Sections 6.1.1, “Page Device Dictionary,”
   and 6.2, “Page Device Parameters”). Parameters affecting the behavior
   of showpage include NumCopies, Collate, Duplex, and perhaps others as well.

   Whether or not the current device is a page device, the precise manner in which
   the current page is transmitted is device-dependent. For certain devices (such as
   displays), no action is required, because the current page is visible while it is being
   composed.

   The main actions of showpage are as follows:

      1. Executes the EndPage procedure in the page device dictionary, passing an integer
         page count on the operand stack along with a reason code indicating that
         the procedure was called from showpage; see Section 6.2.6, “Device Initialization
         and Page Setup,” for more information.

      2. If the boolean result returned by the EndPage procedure is true, transmits the
         page’s contents to the current output device and performs the equivalent of an
         erasepage operation, clearing the contents of raster memory in preparation
         for the next page. If the EndPage procedure returns false, showpage skips this
         step.

      3. Performs the equivalent of an initgraphics operation, reinitializing the graphics
         state for the next page.

      4. Executes the BeginPage procedure in the page device dictionary, passing an
         integer page count on the operand stack.

      If the BeginPage or EndPage procedure invokes showpage, an undefined error
      occurs.

      For a device that produces output on a physical medium such as paper, showpage
      can optionally transmit multiple copies of the page in step 2 above. In Language-
      Level 2 or 3, the page device parameter NumCopies specifies the number of copies
      to be transmitted. In LanguageLevel 1 (or in higher LanguageLevels if NumCopies
      is null), the number of copies is given by the value associated with the name
      #copies in the naming environment defined by the current dictionary stack. (The
      default value of #copies is 1, defined in userdict.) For example, the code

         /#copies 5 def
         showpage

      prints five copies of the current page, then erases the current page and reinitializes
      the graphics state.

      Errors: limitcheck, undefined
*/
   return;
   }

   void job::operatorStandardEncoding() {
/*
   StandardEncoding 

      – StandardEncoding array

   pushes the standard encoding vector on the operand stack. This is a 256-element
   literal array object, indexed by character codes, whose values are the character
   names for those codes. See Section 5.3, “Character Encoding,” for an explanation
   of encoding vectors. StandardEncoding is not an operator; it is a name in
   systemdict associated with the array object.

   StandardEncoding is the Adobe standard encoding vector used by most Latin-text
   fonts, but not by special fonts, such as Symbol. A new Latin-text font having no
   unusual encoding requirements should specify its Encoding entry to be the value
   of StandardEncoding rather than define its own private array. The contents of the
   standard encoding vector are tabulated in Appendix E.

   Errors: stackoverflow
   See Also: ISOLatin1Encoding, findencoding
*/

   push(&StandardEncoding);

   return;
   }

   void job::operatorStopped() {
/*
   stopped 
      any stopped bool

   executes any, which is typically, but not necessarily, a procedure, executable file,
   or executable string object. If any runs to completion normally, stopped returns
   false on the operand stack. If any terminates prematurely as a result of executing
   stop, stopped returns true. Regardless of the outcome, the interpreter resumes execution
   at the next object in normal sequence after stopped.

   This mechanism provides an effective way for a PostScript program to “catch”
   errors or other premature terminations, retain control, and perhaps perform its
   own error recovery. See Section 3.11, “Errors.”

   When an error occurs, the standard error handler sets newerror to true in the
   $error dictionary. When using stopped to catch and continue from an error
   (without invoking handleerror), it is prudent to explicitly reset newerror to false
   in $error; otherwise, any subsequent execution of stop may result in inadvertent
   reporting of the leftover error. Also, note that the standard error handler sets the
   VM allocation mode to local.

      Example
      { … } stopped
         {handleerror}
      if

   If execution of the procedure { … } causes an error, the default error reporting
   procedure is invoked (by handleerror). In any event, normal execution continues
   at the token following the if operator.
*/

   object *pObject = top();

   if ( object::procedure == pObject -> ObjectType() ) {
      pop();
      execute(reinterpret_cast<procedure *>(pObject));
   } else  
      execute();

   push(pFalseConstant);

   return;
   }

   void job::operatorString() {
/*
      string 
         int string string

   creates a string of length int, each of whose elements is initialized with the integer
   0, and pushes this string on the operand stack. The int operand must be a nonnegative
   integer not greater than the maximum allowable string length (see
   Appendix B). The string is allocated in local or global VM according to the current
   VM allocation mode; see Section 3.7.2, “Local and Global VM.”

   Errors: limitcheck, rangecheck, stackunderflow, typecheck, VMerror
*/

   object *pCount = pop();

   char *pszString = new char[pCount -> IntValue() + 1];
   memset(pszString,0,(pCount -> IntValue() + 1) * sizeof(char));
   
   push(new object(pszString,(pszString + pCount -> IntValue())));

   delete [] pszString;

   return;
   }


   void job::operatorStroke() {
/*
   stroke 
      – stroke –

   paints a line centered on the current path, with sides parallel to the path segments.
   The line’s graphical properties are defined by various parameters of the graphics
   state. Its thickness is determined by the current line width parameter (see
   setlinewidth) and its color by the current color (see setcolor). The joints between
   connected path segments and the ends of open subpaths are painted with the current
   line join (see setlinejoin) and the current line cap (see setlinecap), respectively.
   The line is either solid or broken according to the dash pattern established by
   setdash. Uniform stroke width can be ensured by enabling automatic stroke adjustment
   (see setstrokeadjust). All of these graphics state parameters are consulted
   at the time stroke is executed; their values during the time the path is being
   constructed are irrelevant.

   If a subpath is degenerate (consists of a single-point closed path or of two or more
   points at the same coordinates), stroke paints it only if round line caps have been
   specified, producing a filled circle centered at the single point. If butt or projecting
   square line caps have been specified, stroke produces no output, because the
   orientation of the caps would be indeterminate. A subpath consisting of a singlepoint
   open path produces no output.

   After painting the current path, stroke clears it with an implicit newpath operation. 
   To preserve the current path across a stroke operation, use the sequence

      gsave
      fill
      grestore
*/
   return;
   }

   void job::operatorSub() {
/*
   sub 
      num1 num2 sub difference

   returns the result of subtracting num2 from num1. If both operands are integers
   and the result is within integer range, the result is an integer; otherwise, the result
   is a real number.
*/
   object *p2 = pop();
   object *p1 = pop();
   if ( object::integer == p1 -> ValueType() && object::integer == p2 -> ValueType() )
      push(new object((long)(p1 -> IntValue() - p2 -> IntValue())));
   else
      push(new object((double)(p1 -> DoubleValue() - p2 -> DoubleValue())));
   return;
   }

   void job::operatorTransform() {
/*
   transform 
      x1 y1 transform x2 y2
      x1 y1 matrix transform x2 y2

   applies a transformation matrix to the coordinates (x1, y1), returning the transformed
   coordinates (x2, y2). The first form of the operator uses the current transformation
   matrix in the graphics state to transform user space coordinates to
   device space. The second form applies the transformation specified by the matrix
   operand rather than the CTM.
*/
   double x,y;
   matrix *pMatrix = NULL;
   switch ( top() -> ObjectType() ) {
   case object::matrix:
      pMatrix = reinterpret_cast<matrix *>(pop());
      y = pop() -> Value();
      x = pop() -> Value();
      break;
   default:
      pMatrix = pCurrentMatrix;
      y = pop() -> Value();
      x = pop() -> Value();
   }
   push(new object(x));
   push(new object(y));
   return;
   }

   void job::operatorTranslate() {
/*
   translate 
      tx ty translate –
      tx ty matrix translate matrix

   moves the origin of the user coordinate space by tx units horizontally and ty units
   vertically, or returns a matrix representing this transformation. The orientation of
   the axes and the sizes of the coordinate units are unaffected.
   The transformation is represented by the matrix

                     | 1  0  0 |
               T  =  | 0  1  0 |
                     | tx ty 1 |

   The first form of the operator applies this transformation to the user coordinate
   system by concatenating matrix T with the current transformation matrix (CTM);
   that is, it replaces the CTM with the matrix product T ´ CTM. The second form
   replaces the value of the matrix operand with an array representing matrix T and
   pushes the result back on the operand stack without altering the CTM. See
   Section 4.3.3, “Matrix Representation and Manipulation,” for a discussion of how
   matrices are represented as arrays.
*/

   object *pTop = pop();
   if ( object::matrix == pTop -> ObjectType() )
      pop();
   pop();

   if ( object::matrix == pTop -> ObjectType() )
      push(pTop);

   return;
   }
   
   void job::operatorType() {
/*
   type 
      any type name

   returns a name object that identifies the type of the object any. The possible
   names that type can return are as follows:

      arraytype         nametype
      booleantype       nulltype
      dicttype          operatortype
      filetype          packedarraytype
      fonttype          realtype
      gstatetype        savetype
      integertype       stringtype
      marktype

   The name fonttype identifies an object of type fontID. It has nothing to do with a
   font dictionary, which is identified by dicttype the same as any other dictionary.

   The returned name has the executable attribute. This makes it convenient to perform
   type-dependent processing of an object simply by executing the name returned
   by type in the context of a dictionary that defines all the type names to
   have procedure values (this is how the == operator works).

   The set of types is subject to enlargement in future revisions of the language. A
   program that examines the types of arbitrary objects should be prepared to behave
   reasonably if type returns a name that is not in this list.

   Errors: stackunderflow
*/
   object *pObject = pop();

   push(nameTypeMap[pObject -> ObjectType()][pObject -> ValueType()]);

   return;
   }

   void job::operatorUndef() {
/*
   undef 
      dict key undef –

   removes key and its associated value from the dictionary dict. dict does not need to
   be on the dictionary stack. No error occurs if key is not present in dict.

   If the value of dict is in local VM, the effect of undef can be undone by a subsequent
   restore operation. That is, if key was present in dict at the time of the matching
   save operation, restore will reinstate key and its former value. But if dict is in
   global VM, the effect of undef is permanent.

   Errors: invalidaccess, stackunderflow, typecheck
   See Also: def, put, undefinefont
*/

   object *pKey = pop();
   dictionary *pDict = reinterpret_cast<dictionary *>(pop());

   if ( ! pDict -> exists(pKey -> Contents()) )
      return;

   pDict -> remove(pKey -> Contents());

   return;
   }

   void job::operatorVersion() {
/*
   version 
      – version string

   returns a string that identifies the version of the PostScript interpreter being used.
   This identification does not include information about the language features or
   the hardware or operating system environment in which the PostScript interpreter
   is running.
*/
   push(new object(PS_VERSION));

   return;
   }

   void job::operatorVmstatus() {
/*
   vmstatus 
      – vmstatus level used maximum

   returns three integers describing the state of the PostScript interpreter’s virtual
   memory (VM). level is the current depth of save nesting—in other words, the
   number of save operations that have not been matched by a restore operation.
   used and maximum measure VM resources in units of 8-bit bytes; used is the number
   of bytes currently in use and maximum is the maximum available capacity.

   VM consumption is monitored separately for local and global VM. The used and
   maximum values apply to either local or global VM according to the current VM
   allocation mode (see setglobal).

   The used value is meaningful only immediately after a garbage collection has
   taken place (see vmreclaim). At other times, it may be too large because it includes
   memory occupied by objects that have become inaccessible, but have not yet been
   reclaimed.

   The maximum value is an estimate of the maximum size to which the current VM
   (local or global) could grow, assuming that all other uses of available memory reCHAPTER
   main constant. Because that assumption is never valid in practice, there is some
   uncertainty about the maximum value. Also, in some environments (workstations,
   for instance), the PostScript interpreter can obtain more memory from the operating
   system. In this case, memory is essentially inexhaustible and the maximum
   value is meaningless—it is an extremely large number.

   Errors: stackoverflow
   See Also: setuserparams

*/

   push(new object(saveCount));
   push(new object(1024L));
   push(new object(1024L * 1024L));

   return;
   }

   void job::operatorWhere() {
/*
   where 

      key where dict true (if found)
      false (if not found)

   determines which dictionary on the dictionary stack, if any, contains an entry
   whose key is key. where searches for key in each dictionary on the dictionary stack,
   starting with the topmost (current) dictionary. If key is found in some dictionary,
   where returns that dictionary object and the boolean value true; otherwise, where
   simply returns false.

   Errors: invalidaccess, stackoverflow, stackunderflow, typecheck
*/

   object *pTop = pop();

   std::list<dictionary *> restack;

   dictionary *pCurrent = NULL;

   while ( dictionaryStack.size() ) {
      dictionary *pThis = dictionaryStack.pop();
      restack.insert(restack.end(),pThis);
      if ( pThis -> exists(pTop -> Name()) ) {
         pCurrent = pThis;
         break;
      }
   }

   for ( std::list<dictionary *>::reverse_iterator it = restack.rbegin(); it != restack.rend(); it++ ) 
      dictionaryStack.push((*it));

   if ( pCurrent ) {
      push(pCurrent);
      push(pTrueConstant);
      return;
   }

   push(pFalseConstant);

   return;
   }

   void job::operatorWidthshow() {
/*
   widthshow 
      cx cy char string widthshow –

   paints glyphs for the characters of string in a manner similar to show; however,
   while doing so, it adjusts the width of each occurrence of the character char’s
   glyph by adding cx to its x width and cy to its y width, thus modifying the spacing
   between it and the next glyph. This operator enables fitting a string of text to a
   specific width by adjusting the width of the glyph for a specific character, such as
   the space character.

   char is an integer used as a character code. For a base font, char is simply an integer
   in the range 0 to 255 that is compared to successive elements of string. For a
   composite font, char is compared to integers computed from the character mapping
   algorithm (see Section 5.10.1, “Character Mapping”). The font number, f,
   and character code, c, that are selected by the character mapping algorithm are
   combined into a single integer according to the FMapType value of the immediate
   parent of the selected base font. For FMapType values of 4 and 5, the integer value
   is (f ´ 128) + c; for all other FMapType values (with one exception, noted below),
   it is (f ´ 256) + c.

      Example
         /Helvetica findfont 12 scalefont setfont
         14 60 moveto (Normal spacing) show
         14 46 moveto 6 0 8#040 (Wide word spacing) widthshow

   An exception to the above occurs for an FMapType value of 9 if the character
   mapping algorithm selects a character name or a CID rather than a character
   code. In this case, char is compared to an integer formed from the entire input
   character code, interpreted high-order byte first. For example, if the CMap defines
   the mappings

         0 usefont         % Following mapping uses font number 0
            1 begincidchar
               <8140> 633  % Maps two-byte code <8140> to CID 633
            endcidchar

         1 usefont         % Following mapping uses font number 1
            2 beginbfchar
               <61> /a     % Maps one-byte code <61> to character name /a
               <40> <A9>   % Maps one-byte code <40> to character code <A9>
            endbfchar

         and string is <81406140>, char is compared to 33308 (16#8140), 97 (16#61), and
         425 ((1 ´ 256) + 16#A9), in sequence.

*/
   pop();
   pop();
   pop();
   pop();
   return;
   }

   void job::operatorXshow() {
/*
   xshow 
      string numarray xshow –
      string numstring xshow –

   is similar to xyshow; however, for each glyph shown, xshow extracts only one
   number from numarray or numstring. It uses that number as the x displacement
   and the value 0 as the y displacement. In all other respects, xshow behaves the
   same as xyshow.

*/
   pop();
   pop();
   return;
   }

   void job::operatorXyshow() {
/*
   xyshow 
      string numarray xyshow –
      string numstring xyshow –

   paints glyphs for the characters of string in a manner similar to show. After painting
   each glyph, it extracts two successive numbers from the array numarray or the
   encoded number string numstring. These two numbers, interpreted in user space,
   determine the position of the origin of the next glyph relative to the origin of the
   glyph just shown. The first number is the x displacement and the second number
   is the y displacement. In other words, the two numbers override the glyph’s normal
   width.

   If numarray or numstring is exhausted before all the characters of string have been
   shown, a rangecheck error occurs. See Section 5.1.4, “Glyph Positioning,” for further
   information about xyshow, and Section
*/
   pop();
   pop();
   return;
   }

   void job::operatorYshow() {
/*
   yshow 
      string numarray yshow –
      string numstring yshow –

   is similar to xyshow; however, for each glyph shown, yshow extracts only one
   number from numarray or numstring. It uses that number as the y displacement
   and the value 0 as the x displacement. In all other respects, yshow behaves the
   same as xyshow.
*/
   pop();
   pop();
   return;
   }