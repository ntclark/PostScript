
#include "job.h"
#include "PostScript objects\graphicsState.h"

#include "PostScript objects\font.h"
#include "PostScript objects\string.h"
#include "PostScript objects\binaryString.h"
#include "PostScript objects\pattern.h"
#include "PostScript objects\mark.h"
#include "PostScript objects\literal.h"
#include "PostScript objects\directExec.h"
#include "PostScript objects\file.h"
#include "PostScript objects\save.h"

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
    matrix *pMatrix = reinterpret_cast<matrix *>(pop());

    font *pFont = currentGS() -> makeFont(pMatrix,reinterpret_cast<font *>(pop()));

    pFontDirectory -> put(pFont -> fontName(),pFont);

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
   push(new (CurrentObjectHeap()) pattern(this));
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
   push(new (CurrentObjectHeap()) mark(this,object::valueType::arrayMark));
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

   array *pArray = new (CurrentObjectHeap()) array(this,"auto");

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
   push(new (CurrentObjectHeap()) mark(this,object::valueType::dictionaryMark));
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

    std::vector<object *> toInsertValues;
    std::vector<object *> toInsertKeys;

    while ( ! ( object::mark == top() -> ObjectType() ) ) {
        toInsertValues.push_back(pop());
        toInsertKeys.push_back(pop());
    }

    dictionary *pDict = new (CurrentObjectHeap()) dictionary(this,"inline",(long)toInsertValues.size());

    for ( long k = (long)toInsertValues.size() - 1; k > -1; k-- ) 
        pDict -> put(toInsertKeys[k] -> Name(),toInsertValues[k]);

    toInsertValues.clear();
    toInsertKeys.clear();

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

   push(new (CurrentObjectHeap()) mark(this,object::valueType::procedureMark));

   return;
   }

   void job::operatorMarkProcedureEnd() {

   std::list<object *> entries;

   object *pObject = pop();

   while ( object::mark != pObject -> ObjectType() ) {
      entries.insert(entries.end(),pObject);
      pObject = pop();
   }

   procedure *pProcedure = new (CurrentObjectHeap()) procedure(this);

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

    push(new (CurrentObjectHeap()) matrix(this));

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

    currentGS() -> moveto();

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
      push(new (CurrentObjectHeap()) object(this,p1 -> IntValue() * p2 -> IntValue()));
   else {
      double v1 = p1 -> DoubleValue();
      if ( object::integer == p1 -> ValueType() )
         v1 = (double)p1 -> IntValue();
      double v2 = p2 -> DoubleValue();
      if ( object::integer == p2 -> ValueType() )
         v2 = (double)p2 -> IntValue();
      push(new (CurrentObjectHeap()) object(this,v1 * v2));
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
      push(new (CurrentObjectHeap()) object(this,-1 * p -> IntValue()));
   else
      push(new (CurrentObjectHeap()) object(this,-1.0 * p -> DoubleValue()));
   return;
   }

    void job::operatorNewpath() {
/*
    newpath 
        – newpath –

    initializes the current path in the graphics state to an empty path. The current
    point becomes undefined.
*/
    currentGS() -> newpath();
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
        push(new (CurrentObjectHeap()) object(this,v));
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
    push(new (CurrentObjectHeap()) string(this,PRODUCT_NAME));

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
        reinterpret_cast<array *>(pTarget) -> putElement(pIndex -> IntValue(),pValue);
        break;

    case object::dictionary:
        reinterpret_cast<dictionary *>(pTarget) -> put(pIndex -> Name(),pValue);
        break;

    case object::font:
        static_cast<dictionary *>(pTarget) -> put(pIndex -> Name(),pValue);
        break;

    case object::procedure:
        reinterpret_cast<procedure *>(pTarget) -> putElement(pIndex -> IntValue(),pValue);
        break;

    case object::matrix:
        reinterpret_cast<matrix *>(pTarget) -> SetValue(pIndex -> IntValue(),pValue -> OBJECT_POINT_TYPE_VALUE);
        break;

    case object::atom:
        switch ( pTarget -> ValueType() ) {
        case object::valueType::string: {
            string *pString = static_cast<string *>(pTarget);
            pString -> put(pIndex -> IntValue(),(BYTE)pValue -> IntValue());
            }
            break;
        }

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

    long n = pop() -> IntValue();

    long index = pIndex -> IntValue();

    if ( object::array == pObject1 -> ObjectType() || object::packedarray == pObject1 -> ObjectType() ) {

        array *pArray = reinterpret_cast<array *>(pObject1);

        switch ( pObject2 -> ObjectType() ) {
        case object::array:
        case object::packedarray: {
            array *pSource = reinterpret_cast<array *>(pObject2);
            for ( long k = 0; k < n; k++ ) 
            pArray -> putElement(index++,pSource -> getElement(k));
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

    } else if ( object::atom == pObject1 -> ObjectType() && 
                ( object::valueType::string == pObject1 -> ValueType() || object::valueType::constantString == pObject1 -> ValueType() || 
                    object::valueType::hexString == pObject1 -> ValueType() || object::valueType::binaryString == pObject1 -> ValueType() ) ) {

        /*if ( ! ( pObject1 -> ObjectType() == pObject2 -> ObjectType() ) || ! ( pObject1 -> ValueType() == pObject2 -> ValueType() ) ) {
            char szMessage[1024];
            sprintf(szMessage,"operator putinterval: the type of object %s is invalid (%s) should be string",pObject2 -> Name(),pObject2 -> TypeName());
            throw typecheck(szMessage);
        }*/

        for ( long k = 0; k < n; k++ ) 
            pObject1 -> put(index++,pObject2 -> get(k));

    } else {
        char szMessage[1024];
        sprintf(szMessage,"operator putinterval: the type of an object is invalid (%s) should be array, packedarray, or string",pObject1 -> TypeName());
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
    top() -> theAccessAttribute = object::accessAttribute::readOnly;
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

   void job::operatorRestore() {
/*
    restore
        save restore -

*/
    save *pSave = reinterpret_cast<save *>(pop());

    currentGS() -> restore(pSave);

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

    currentGS() -> rlineto();

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

    currentGS() -> rmoveto();

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
    long j = pop() -> IntValue();
    long n = pop() -> IntValue();

    object **pObjects = new object *[n];

    long *pIndex = new long[n + 1];
    
    for ( long k = 0; k < n; k++ ) {
        pIndex[k] = n - k - 1;
        pObjects[pIndex[k]] = pop();
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
    POINT_TYPE angle;

    switch ( pTop -> ObjectType() ) {
    case object::objectType::matrix: 
        angle = pop() -> DoubleValue();
        break;

    default:
        angle = pTop -> DoubleValue();
        currentGS() -> rotate(angle);
        return;
    }

    //!!???!!! angle = -angle;

    POINT_TYPE cosAngle = cos(angle * graphicsState::degToRad);
    POINT_TYPE sinAngle = sin(angle * graphicsState::degToRad);

    matrix *pMatrix = new (CurrentObjectHeap()) matrix(this);
    pMatrix -> a(cosAngle);
    pMatrix -> b(sinAngle);
    pMatrix -> c(-sinAngle);
    pMatrix -> d(cosAngle);

    push(pMatrix);

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
            push(new (CurrentObjectHeap()) object(this,(double)((long)v + 1.0)));
         else
            push(new (CurrentObjectHeap()) object(this,(double)((long)v)));
      } else {
         if ( -v + (long)v >= 0.5 )
            push(new (CurrentObjectHeap()) object(this,(double)((long)v - 1.0)));
         else
            push(new (CurrentObjectHeap()) object(this,(double)((long)v)));

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
    currentGS() -> save();
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
    POINT_TYPE sx,sy;

    switch ( pTop -> ObjectType() ) {
    case object::objectType::matrix: 
        sy = pop() -> DoubleValue();
        sx = pop() -> DoubleValue();
        break;

    default:
        sy = pTop -> DoubleValue();
        sx = pop() -> DoubleValue();
        currentGS() -> scale(sx,sy);
        return;
    }

    matrix *pMatrix = new (CurrentObjectHeap()) matrix(this);
    pMatrix -> a(sx);
    pMatrix -> d(sy);
    push(pMatrix);
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
    object *pFontScale = pop();

    font *pFont = currentGS() -> scaleFont(pFontScale -> OBJECT_POINT_TYPE_VALUE,reinterpret_cast<font *>(pop()));

    pFontDirectory -> put(pFont -> fontName(),pFont);

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

    object *pScaleOrMatrix = pop();

    operatorFindfont();

    font *pFont = reinterpret_cast<font *>(top());

    currentGS() -> setFont(pFont);

    push(pScaleOrMatrix);

    if ( object::valueType::real == pScaleOrMatrix -> ValueType() )
        operatorScalefont();
    else
        operatorMakefont();

    pop();

    return;
    }

    void job::operatorSetcachedevice() {
/*
    setcachedevice
        wx wy llx lly urx ury setcachedevice –

    passes width and bounding box information to the PostScript interpreter’s font
    machinery. setcachedevice can be executed only within the context of a
    BuildGlyph, BuildChar, or CharStrings procedure for a font or CIDFont.

    The procedure must invoke setcachedevice, setcachedevice2, or setcharwidth 
    before executing graphics operators to define and paint the glyph. setcachedevice requests
    the font machinery to transfer the results of those operators both into the font
    cache, if possible, and onto the current page.

    The operands to setcachedevice are all numbers interpreted in the glyph 
    coordinate system (see Section 5.4, “Glyph Metric Information”). wx and wy 
    define the basic width vector for this glyph—in other words, the normal 
    position of the origin of the next glyph relative to origin of this one.
    llx and lly are the coordinates of the lower-left corner, and urx and ury 
    the upperright corner, of the glyph bounding box. The glyph bounding box 
    is the smallest rectangle, oriented with the glyph coordinate system axes,
    that completely encloses all marks placed on the page as a result of executing
    the glyph’s description.

    For a glyph defined as a path, this may be determined by means of the pathbbox
    operator. The font machinery needs this information to make decisions about
    clipping and caching. The declared bounding box must be correct—in other
    words, sufficiently large to enclose the entire glyph. If any marks fall outside this
    bounding box, the result is unpredictable.

    setcachedevice installs identical sets of metrics for writing modes 0 and 1, while
    setcachedevice2 installs separate metrics.

    After execution of setcachedevice and until the termination of the BuildGlyph,
    BuildChar, or CharStrings procedure, invocation of color-setting operators or the
    image operator is not allowed; see Section 4.8, “Color Spaces.” Note that use of
    the imagemask operator is permitted.

    Errors: stackunderflow, typecheck, undefined
    See Also: setcachedevice2, setcharwidth, setcachelimit, cachestatus
*/

    currentGS() -> setCacheDevice();

    return;
    }

    void job::operatorSetcmykcolor() {
/*
    setcmykcolor 
        cyan magenta yellow black setcmykcolor –

    sets the current color space in the graphics state to DeviceCMYK and the current
    color to the component values specified by cyan, magenta, yellow, and black. Each
    component must be a number in the range 0.0 to 1.0. If any of the operands is
    outside this range, the nearest valid value is substituted without error indication.

    Color values set by setcmykcolor are not affected by black-generation and 
    undercolor-removal computations (see Section 7.2.3, “Conversion from DeviceRGB to DeviceCMYK”).

    Execution of this operator is not permitted in certain circumstances; see
    Section 4.8.1, “Types of Color Space.”

    Errors: stackunderflow, typecheck, undefined
    See Also: currentrgbcol
*/

    pop();
    pop();
    pop();
    pop();

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
        pCurrentColorSpace = new (CurrentObjectHeap()) colorSpace(this,reinterpret_cast<array *>(pTop));
    else
        pCurrentColorSpace = new (CurrentObjectHeap()) colorSpace(this,pTop -> Name());

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

   POINT_TYPE offset = pop() -> DoubleValue();
   class array *pArray = reinterpret_cast<array *>(pop());

   currentGS() -> setLineDash(pArray,offset);
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

    object *pObject = pop();

    font *pFont = NULL;
    dictionary *pDictionary = NULL;

    switch ( pObject -> ObjectType() ) {
    case object::objectType::dictionary: {
        pDictionary = reinterpret_cast<dictionary *>(pObject);
        object *pName = pDictionary -> retrieve("FontName");
        if ( ! ( NULL == pName ) ) 
            pFont = reinterpret_cast<font *>(pFontDirectory -> retrieve(pName -> pszContents));
        if ( NULL == pFont ) {
            char szMessage[1024];
            sprintf(szMessage,"operator: setfont. font %s is undefined",pObject -> Name());
            throw invalidfont(szMessage);
            return;
        }
        }
        break;

    case object::objectType::font: 
        pFont = reinterpret_cast<font *>(pObject);
        break;

    default: {
        char szMessage[1024];
        sprintf(szMessage,"operator: setfont. invalid object type specified for object %s",pObject -> Name());
        throw syntaxerror(szMessage);
        return;
        }
    }

    currentGS() -> setFont(pFont);

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
   currentGS() -> setLineCap(pop() -> IntValue());
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
   currentGS() -> setLineJoin(pop() -> IntValue());
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
    currentGS() -> setLineWidth(pop() -> OBJECT_POINT_TYPE_VALUE);

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
    object *pMatrix = pop();
    currentGS() -> setMatrix(pMatrix);
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
    currentGS() -> setPageDevice(reinterpret_cast<dictionary *>(pop()));

    return;
    }

    void job::operatorSetrgbcolor() {
/*
    setrgbcolor 
        red green blue setrgbcolor –

    sets the current color space in the graphics state to DeviceRGB and 
    the current color to the component values specified by red, green, 
    and blue. Each component must be a number in the range 0.0 to 1.0. 
    If any of the operands is outside this range, the nearest valid 
    value is substituted without error indication.

    Execution of this operator is not permitted in certain circumstances; see
    Section 4.8.1, “Types of Color Space."

    Errors: stackunderflow, typecheck, undefined

    See Also: currentrgbcolor, setcolorspace, setcolor, setgray, sethsbcolor,
    setcmykcolor

*/
    object *pBlue = pop();
    object *pGreen = pop();
    object *pRed = pop();
    currentGS() -> setRGBColor(pRed -> DoubleValue(),pGreen -> DoubleValue(),pBlue -> DoubleValue());
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
    object *pObject = reinterpret_cast<string *>(top());

    operatorLength();

    long strSize = pop() -> IntValue();

    binaryString *pBinary = NULL;
    string *pString = NULL;

    if ( object::valueType::binaryString == pObject -> ValueType() )
        pBinary = reinterpret_cast<binaryString *>(pObject);
    else
        pString = reinterpret_cast<string *>(pObject);

    for ( long k = 0; k < strSize; k++ ) {

        BYTE glyphIndex;
        if ( ! ( NULL == pBinary ) )
            glyphIndex = pBinary -> get(k);
        else
            glyphIndex = pString -> get(k);

        currentGS() -> drawGlyph(glyphIndex);

    }

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

    currentGS() -> showPage();

   return;
   }

    void job::operatorSin() {
/*
    sin 
        angle sin real

    returns the sine of angle, which is interpreted as an angle in degrees. The result is a
    real number.

    Errors: stackunderflow, typecheck
    See Also: cos, atan

*/
    object *pSin = new (CurrentObjectHeap()) object(this,sin(graphicsState::degToRad * pop() -> DoubleValue()));
    push(pSin);
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

   push(pStandardEncoding);

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
        executeProcedure(reinterpret_cast<procedure *>(pObject));
    } else  
        executeObject();

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
   
   push(new (CurrentObjectHeap()) object(this,pszString,(pszString + pCount -> IntValue())));

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

    currentGS() -> stroke();
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
      push(new (CurrentObjectHeap()) object(this,(long)(p1 -> IntValue() - p2 -> IntValue())));
   else
      push(new (CurrentObjectHeap()) object(this,(double)(p1 -> DoubleValue() - p2 -> DoubleValue())));
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

    POINT_TYPE x,y;
    object *pTopObject = pop();

    switch ( pTopObject -> ObjectType() ) {
    case object::matrix: {
        matrix *pMatrix = reinterpret_cast<matrix *>(pTopObject);
        y = pop() -> DoubleValue();
        x = pop() -> DoubleValue();
        currentGS() -> transformPoint(pMatrix,x,y,&x,&y);
        }
        break;

    default: {
        y = pTopObject -> DoubleValue();
        x = pop() -> DoubleValue();
        currentGS() -> transformPoint(x,y,&x,&y);
        }
    }

    push(new (CurrentObjectHeap()) object(this,x));
    push(new (CurrentObjectHeap()) object(this,y));
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
    POINT_TYPE ty;

    switch ( pTop -> ObjectType() ) {
    case object::objectType::matrix: 
        ty = pop() -> DoubleValue();
        break;

    default:
        ty = pTop -> DoubleValue();
        currentGS() -> translate(pop() -> DoubleValue(),ty);
        return;
    }

    matrix *pMatrix = new (CurrentObjectHeap()) matrix(this);
    pMatrix -> tx(pop() -> DoubleValue());
    pMatrix -> ty(ty);

    push(pMatrix);

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
    object *pDictObject = pop();
    dictionary *pDict = NULL;

    if ( object::dictionary == pDictObject -> ObjectType() )
        pDict = reinterpret_cast<dictionary *>(pDictObject);
    else if ( object::font == pDictObject -> ObjectType() )
        pDict = static_cast<dictionary *>(reinterpret_cast<font *>(pDictObject));
    else {
        char szMessage[1024];
        sprintf_s<1024>(szMessage,"Operator undef: the type of the dict operand is invalid");
        throw typecheck(szMessage);
        return;
    }

    if ( ! pDict -> exists(pKey -> Name()) )
        return;

    pDict -> remove(pKey -> Name());

    return;
    }

    void job::operatorUndefinefont() {
/*
    undefinefont
        key undefinefont –

    removes key and its associated value (a font or CIDFont dictionary) from the font
    directory, reversing the effect of a previous definefont operation. 
    undefinefont is a special case of the undefineresource operator applied to 
    the Font category. For details, see undefineresource and Section 3.9, “Named Resources.”

    Errors: stackunderflow, typecheck
    See Also: definefont, undefineresource
*/

    object *pKey = pop();

    if ( ! pFontDirectory -> exists(pKey -> Name()) )
        return;

    pFontDirectory -> remove(pKey -> Name());

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
   push(new (CurrentObjectHeap()) object(this,PS_VERSION));

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

   push(new (CurrentObjectHeap()) object(this,saveCount));
   push(new (CurrentObjectHeap()) object(this,1024L));
   push(new (CurrentObjectHeap()) object(this,1024L * 1024L));

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

    dictionary *pCurrent = NULL;

    if ( ! ( NULL == pTop -> Name() ) )
        pCurrent = dictionaryStack.find(pTop -> Name());

    if ( ! ( NULL == pCurrent ) ) {
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

    See section, 3.14.5 Encoded Number Strings
    At this point, the use of a numstring operand is not implemented

*/
    object *pObject = pop();

    array *pNumberArray = reinterpret_cast<array *>(pObject);

    pObject = reinterpret_cast<string *>(top());

    operatorLength();

    long strSize = pop() -> IntValue();

    binaryString *pBinary = NULL;
    string *pString = NULL;

    if ( object::valueType::binaryString == pObject -> ValueType() )
        pBinary = reinterpret_cast<binaryString *>(pObject);
    else
        pString = reinterpret_cast<string *>(pObject);

    for ( long k = 0; k < strSize; k++ ) {

        object *pChar = NULL;

        BYTE glyphIndex;
        if ( ! ( NULL == pBinary ) )
            glyphIndex = pBinary -> get(k);
        else
            glyphIndex = pString -> get(k);

        currentGS() -> drawGlyph(glyphIndex);

        object *pX = pNumberArray -> getElement(k);
        push(pX);
        push(pZeroConstant);
        currentGS() -> rmoveto();
    }

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
    object *pNumberArray = pop();
    object *pString = pop();
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