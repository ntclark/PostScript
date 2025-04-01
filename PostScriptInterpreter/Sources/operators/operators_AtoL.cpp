/*
Copyright 2025 EnVisioNate LLC

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the “Software”), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is the MIT License
*/

#include "job.h"


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

    OutputDebugStringA("\n");
    OutputDebugStringA(top -> Contents());
    OutputDebugStringA("\n");
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
    if ( object::valueType::integer == p1 -> ValueType() && object::valueType::integer == p2 -> ValueType() )
        push(new (CurrentObjectHeap()) object(this,p1 -> IntValue() + p2 -> IntValue()));
    else
        push(new (CurrentObjectHeap()) object(this,p1 -> Value() + p2 -> Value()));
    return;
    }


    void job::operatorAload() {
/*
    aload 

        array aload any0 ... anyn - 1 array
        packedarray aload any0 ... anyn - 1 packedarray

    successively pushes all n elements of array or packedarray on the operand stack
    (where n is the length of the operand), and then pushes the operand itself.

*/

    object *pTop = pop();

    switch ( pTop -> ObjectType() ) {

    case object::objectType::objTypeArray: {
        class array *pArray = reinterpret_cast<class array *>(pTop);
        for ( long k = 0; k < pArray -> size(); k++ )
            push(pArray -> getElement(k));
        }
        break;

    case object::objectType::procedure: {
        class procedure *pProcedure = reinterpret_cast<procedure *>(pTop);
        for ( object *pObj : pProcedure -> entries ) 
            push(pObj);
        }
        break;

    default:
        char szMessage[1024];
        sprintf(szMessage,"operator aload: object %s is not an array or procedure",pTop -> Name());
        //throw typecheck(szMessage);
        return;
    }

    push(pTop);

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

            push(new (CurrentObjectHeap()) object(this,v));

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
    POINT_TYPE angle2 = pop() -> OBJECT_POINT_TYPE_VALUE;
    POINT_TYPE angle1 = pop() -> OBJECT_POINT_TYPE_VALUE;
    POINT_TYPE radius = pop() -> OBJECT_POINT_TYPE_VALUE;
    POINT_TYPE y = pop() -> OBJECT_POINT_TYPE_VALUE;
    POINT_TYPE x = pop() -> OBJECT_POINT_TYPE_VALUE;

    currentGS() -> arcto(x,y,radius,angle1,angle2);

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
    POINT_TYPE angle2 = pop() -> OBJECT_POINT_TYPE_VALUE;
    POINT_TYPE angle1 = pop() -> OBJECT_POINT_TYPE_VALUE;
    POINT_TYPE radius = pop() -> OBJECT_POINT_TYPE_VALUE;
    POINT_TYPE y = pop() -> OBJECT_POINT_TYPE_VALUE;
    POINT_TYPE x = pop() -> OBJECT_POINT_TYPE_VALUE;

    currentGS() -> arcto(x,y,radius,360.0f - angle1,360.0f - angle2);
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
   array *pArray = new (CurrentObjectHeap()) array(this,"unnamed",count);
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
        push(new (CurrentObjectHeap()) object(this,n - k - 1));
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

    object *pObj = pop();

    if ( ! ( object::dictionary == pObj -> ObjectType() ) && ! ( object::font == pObj -> ObjectType() ) ) {
        char szMessage[1024];
        sprintf(szMessage,"%s Line: %d: object %s is not a dictionary",__FUNCTION__,__LINE__,pObj -> Name());
        throw new typecheck(szMessage);
        return;
    }

    dictionary *pDictionary = reinterpret_cast<dictionary *>(pObj);

    dictionaryStack.setCurrent(pDictionary);

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
    while ( ! ( object::mark == top() -> ObjectType() ) ) {
        pop();
    }
    pop();
    return;
    }


    void job::operatorClip() {
/*
    clip 
        – clip –

    intersects the area inside the current clipping path with the area inside the current
    path to produce a new, smaller clipping path. The nonzero winding number rule
    (see “Nonzero Winding Number Rule” on page 195) is used to determine what
    points lie inside the current path, while the inside of the current clipping path is
    determined by whatever rule was used at the time the path was created.

    In general, clip produces a new path whose inside (according to the nonzero
    winding number rule) consists of all areas that are inside both of the original
    paths. The way this new path is constructed (the order of its segments, whether it
    self-intersects, and so forth) is not specified. clip treats an open subpath of the
    current path as though it were closed; it does not actually alter the path itself. It is
    permissible for the current path to be empty. The result of executing clip is always
    a nonempty clipping path, though it may enclose zero area.

    There is no way to enlarge the current clipping path (other than by initclip or
    initgraphics) or to set a new clipping path without reference to the current one.
    The recommended way of using clip is to bracket the clip operation and the
    sequence of graphics operations to be clipped between gsave and grestore or
    between clipsave and cliprestore. The grestore will restore the clipping path that
    was in effect before the gsave. The setgstate operator can also be used to reset the
    clipping path to an earlier state.

    Unlike fill and stroke, clip does not implicitly perform a newpath operation after
    it has finished using the current path. Any subsequent path construction operators
    will append to the current path unless newpath is invoked explicitly; this can cause 
    unexpected behavior.

    Errors: limitcheck
    See Also: eoclip, clippath, initclip, rectclip
*/

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
    pGraphicsState -> closepath();
    return;
    }

    void job::operatorColorimage() {
/*
    colorimage
        width height bits/comp matrix datasrc0 ... datasrcncomp - 1 multi ncomp colorimage -

    paints a sampled color image onto the current page. This description only
    summarizes the general behavior of the colorimage operator; see Section 4.10,
    "Images", for full details.

    The image is a rectangular array of width × height sample values, each consisting
    of 1, 3, or 4 color components, as specified by ncomp. Each component consists of
    bits/comp bits of data; valid values of bits/comp are 1, 2, 4, 8, or 12.
    All components are the same size.

    The image is considered to exist in its own source coordinate system, or image
    space. The rectangular boundary of the image has its lower-left corner at 
    coordinates (0, 0) and its upper-right corner at (width, height). 
    The matrix operand defines a transformation from user space to image space.
    
    If ncomp is 1, image samples have only one (gray) component; the behavior of
    colorimage is equivalent to that of image using the first five operands. If ncomp is
    3, samples consist of red, green, and blue components; if ncomp is 4, they consist
    of cyan, magenta, yellow, and black components. The 1-, 3-, and 4-component
    sample values are interpreted according to the DeviceGray, DeviceRGB, and
    DeviceCMYK color spaces, respectively (see Section 4.8.2, “Device Color Spaces”),
    regardless of the current color space.

    The multi operand is a boolean value that determines how colorimage obtains
    sample data from its data sources. If multi is false, there is a single data source,
    datasrc0; colorimage obtains all components from that source, interleaved on a
    per-sample basis. If multi is true, there are multiple data sources,
    datasrc0 through datasrcncomp - 1, one for each color component. The data
    sources may be procedures, strings, or files (including filtered files), but must all
    be of the same type (see Section 4.10.2, "Sample Representation").

    Unlike image and imagemask, colorimage does not have an alternate form in
    which the parameters are bundled into a single image dictionary operand. In
    LanguageLevels 2 and 3, given the appropriate image dictionary, the image 
    operator can do anything that colorimage can do, and much more. For example, image
    can interpret color samples in any color space, whereas colorimage is limited to
    the DeviceGray, DeviceRGB, and DeviceCMYK color spaces.

    Execution of this operator is not permitted in certain circumstances; see
    Section 4.8.1, Types of Color Space.

    Errors: invalidaccess, ioerror, limitcheck, rangecheck, stackunderflow,
    typecheck, undefined
*/

    currentGS() -> colorImage();

    return;
    }

    void job::operatorConcat() {
/*
    concat 
        matrix concat –

    applies the transformation represented by matrix to the user coordinate space.
    concat accomplishes this by concatenating matrix with the current transformation
    matrix (CTM); that is, it replaces the CTM with the matrix product matrix * CTM
    (see Section 4.3, “Coordinate Systems and Transformations”).
*/
    object *pMatrixOrArray = pop();
    switch ( pMatrixOrArray -> ObjectType() ) {
    case object::objectType::objTypeMatrix:
        currentGS() -> concat(reinterpret_cast<matrix *>(pMatrixOrArray));
        break;
    case object::objectType::objTypeArray:
        currentGS() -> concat(reinterpret_cast<array *>(pMatrixOrArray));
        break;
    default:
        char szMessage[1024];
        sprintf(szMessage,"operator begin: object %s is not a matrix or array",pMatrixOrArray -> Name());
        throw new typecheck(szMessage);
        return;
    }
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

    object *pTargetObject = pop();
    object *pSourceObject = pop();

    dictionary *pTarget = NULL;

    switch ( pTargetObject -> ObjectType() ) {
    case object::dictionary:
        pTarget = reinterpret_cast<dictionary *>(pTargetObject);
        break;

    case object::font:
        pTarget = static_cast<dictionary *>(reinterpret_cast<font *>(pTargetObject));
        break;

    default:
        __debugbreak();
        return;
    }

    dictionary *pSource = NULL;

    switch ( pSourceObject -> ObjectType() ) {
    case object::dictionary:
        pSource = reinterpret_cast<dictionary *>(pSourceObject);
        break;

    case object::font:
        pSource = static_cast<dictionary *>(reinterpret_cast<font *>(pSourceObject));
        break;

    default:
        __debugbreak();
        return;
    }

    pTarget -> copyFrom(pSource);
    push(pTarget);

    return;
    }

    void job::operatorCos() {
/*
    cos 
        angle cos real

    returns the cosine of angle, which is interpreted as an angle in degrees. The result
    is a real number.

    Errors: stackunderflow, typecheck
    See Also: atan, sin

*/
    object *pCos = new (CurrentObjectHeap()) object(this,cos(graphicsState::degToRad * pop() -> OBJECT_POINT_TYPE_VALUE));
    push(pCos);
    return;
    }


    void job::operatorCountdictstack() {
/*
    countdictstack 
        – countdictstack int

    counts the number of dictionaries currently on the dictionary stack and pushes
    this count on the operand stack.
*/
    push(new (CurrentObjectHeap()) object(this,(long)dictionaryStack.size()));
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

    // for gs compatibility until gs is history
    //while ( ! ( top() -> ObjectType() == object::mark ) ) {
    while ( ! ( top() -> ObjectType() == object::mark ) && ! ( 0 == strcmp(top() -> Contents(),"mark") ) ) {
        count++;
        entries.insert(entries.end(),pop());
    }

    for ( std::list<object *>::reverse_iterator it = entries.rbegin(); it != entries.rend(); it++ )
        push(*it);

    push(new (CurrentObjectHeap()) object(this,count));

    entries.clear();

    return;
    }


    void job::operatorCurrentcolorspace() {
/*
    currentcolorspace – 
        currentcolorspace array

    returns an array containing the family name and parameters of the current color
    space in the graphics state (see setcolorspace). The results are always returned in
    an array, even if the color space has no parameters and was specified to
    setcolorspace by name.

    Errors: stackoverflow
    See Also: setcolorspace, setcolor
*/

    push(currentGS() -> getColorSpace());

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

    void job::operatorCurrentfile() {

/*
    currentfile
       - curentfile file

    returns the file object from which the PostScript interpreter is currently or was
    most recently reading program input—that is, the topmost file object on the 
    execution stack. The returned file has the literal attribute. If there is no 
    file object on the execution stack, currentfile returns an invalid file object 
    that does not correspond to any file. This never occurs during execution of
    ordinary user programs.

    The file returned by currentfile is usually but not always the standard input file.
    An important exception occurs during interactive mode operation (see Section 3.8.3, 
    “Special Files”). In this case, the interpreter does not read directly from the
    standard input file; instead, it reads from a file representing an edited statement
    (each statement is represented by a different file).

    The currentfile operator is useful for obtaining images or other data residing in
    the program file itself (see the example below). At any given time, this file is 
    positioned at the end of the last PostScript token read from the file by the interpreter. 
    If that token was a number or a name immediately followed by a white-space character, 
    the file is positioned after the white-space character (the first, if are several); 
    otherwise, it is positioned after the last character of the token. 

*/
    push( new (CurrentObjectHeap()) file(this,szPostScriptSourceFile));
    return;
    }


    void job::operatorCurrentfont() {

/*
    currentfont – currentfont font
    – currentfont cidfont

    returns the current font or CIDFont dictionary, based on the font parameter in
    the graphics state. Normally, currentfont returns the value of the font parameter,
    as set by setfont or selectfont (and also returned by rootfont). However, when the
    font parameter denotes a composite font, and currentfont is executed inside the
    BuildGlyph, BuildChar, or CharStrings procedure of a descendant base font or
    CIDFont (or inside a procedure invoked by cshow), currentfont returns the current 
    descendant base font or CIDFont. (Of course, if the procedure calls setfont
    or selectfont first, rootfont and currentfont both return the newly selected font.)

    Errors: stackoverflow
    See Also: rootfont, selectfont, setfont
*/
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

    currentGS() -> currentMatrix();

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

    currentGS() -> curveto();

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
            push(new (CurrentObjectHeap()) object(this,pTop -> IntValue()));
        else
            push(new (CurrentObjectHeap()) object(this,atol(pTop -> Contents())));
    } else
        push(new (CurrentObjectHeap()) object(this,atol(pTop -> Contents())));
    return;
    }

    void job::operatorCvn() {
/*
    cvn 
        string cvn name

    (convert to name) converts the string operand to a name object that is lexically
    the same as the string. The name object is executable if the string was executable.
*/

    object *pTop = pop();

    long n = (DWORD)strlen(pTop -> Name()) + 2;

    char *pszTemp = new char[n];

    sprintf(pszTemp,"/%s",pTop -> Name());

    push(new (CurrentObjectHeap()) string(this,pszTemp));

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
        push(new (CurrentObjectHeap()) object(this,pTop -> OBJECT_POINT_TYPE_VALUE));
    else
        push(new (CurrentObjectHeap()) object(this,atof(pTop -> Contents())));
    return;
    }

    void job::operatorCvs() {
/*
    cvs
        any string cvs substring

    (convert to string) produces a text representation of an arbitrary object any, stores
    the text into string (overwriting some initial portion of its value), and returns a
    string object designating the substring actually used. If string is too small to hold
    the result of the conversion, a rangecheck error occurs.

    If any is a number, cvs produces a string representation of that number. If any is a
    boolean value, cvs produces either the string true or the string false. If any is a
    string, cvs copies its contents into string. If any is a name or an operator, cvs pro-
    duces the text representation of that name or the operator’s name. If any is any
    other type, cvs produces the text --nostringval--.

    If any is a real number, the precise format of the result string is implementation
    dependent and not under program control. For example, the value 0.001 might be
    represented as 0.001 or as 1.0E-3.
*/

    object *pTop = pop();

    if ( object::number == pTop -> ObjectType() ) {
        char szNumber[64];
        if ( object::valueType::real == pTop -> ValueType() )
            sprintf_s<64>(szNumber,"%g",pTop -> OBJECT_POINT_TYPE_VALUE);
        else
            sprintf_s<64>(szNumber,"%ld",pTop -> IntValue());
        push(new (CurrentObjectHeap()) literal(this,szNumber,NULL));
    } else
        push(new (CurrentObjectHeap()) literal(this,pTop -> Contents(),NULL));

    return;
    }

    void job::operatorCvx() {
/*
    cvx 
        any cvx any

    (convert to executable) makes the object on the top of the operand stack have the
    executable instead of the literal attribute.
*/

    top() -> theExecutableAttribute = object::executableAttribute::executable;

    return;
    }

    void job::operatorDebug() {
    __debugbreak();
    return;
    }

    void job::operatorDebuglevel() {
    object *pLevel = pop();
    pPStoPDF -> LogLevel((logLevel)pLevel -> IntValue());
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

    dictionaryStack.top() -> put(pop() -> Name(),pValue);

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

    dictionary *pDictionary = NULL;
    font *pFont = NULL;
    boolean isNew = false;

    switch ( po -> ObjectType() ) {
    case object::objectType::font:

        pFont = reinterpret_cast<font *>(po);
        pDictionary = static_cast<dictionary *>(pFont);
        break;

    case object::objectType::dictionary: {
        pDictionary = reinterpret_cast<dictionary *>(po);
        object *pName = pDictionary -> retrieve("FontName");
        if ( NULL == pName )
            pName = pKey;
        pFont = reinterpret_cast<font *>(pFontDirectory -> retrieve(pName -> pszContents));
        if ( NULL == pFont ) {
            pFont = new (CurrentObjectHeap()) font(this,pDictionary,pName -> Contents());
            isNew = true;
        } else {
            pFont -> copyFrom(pDictionary);
isNew = true;
        }
        pDictionary = reinterpret_cast<dictionary *>(pFont);
        }
        break;

    default: {
        char szMessage[1024];
        sprintf(szMessage,"operator aload: object %s is not an dictionary or font",po -> Name());
        throw new typecheck(szMessage);
        return;
        }
    }

    pDictionary -> Name(pKey -> Name());
    pDictionary -> put("FID",pKey);

    pFont -> SetCIDFont(pDictionary -> exists("CIDFontType"));

    if ( isNew )
        pFontDirectory -> put(pFont -> fontName(),pFont);

    push(pFont);

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

    resource *pResource = new (CurrentObjectHeap()) resource(this,pCategory,pInstance,pKey);

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

    dictionary *pDict = new (CurrentObjectHeap()) dictionary(this,pCount -> IntValue());

    push(pDict);

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

    push(new (CurrentObjectHeap()) object(this,v1 / v2));

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
        throw new dictstackunderflow(szMessage);
    }

    dictionary *pDictionary = dictionaryStack.pop();

    //if ( dictionaryStack.pUserDict == pDictionary || dictionaryStack.pSystemDict == pDictionary ) 
    //    dictionaryStack.setBottom(pDictionary);

    return;
    }

    void job::operatorEoclip() {
/*
    eoclip 
        – eoclip –

    intersects the area inside the current clipping path with the area inside the current
    path to produce a new, smaller clipping path. The even-odd rule (see “Even-Odd
    Rule” on page 196) is used to determine what points lie inside the current path,
    while the inside of the current clipping path is determined by whatever rule was
    used at the time the path was created. In all other respects, the behavior of eoclip
    is identical to that of clip.

    Errors: limitcheck
    See Also: clip, clippath, initclip, rectclip
*/

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

    currentGS() -> eofillpath();

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

   //if ( ! ( pAny2 -> ObjectType() == pAny1 -> ObjectType() ) ) {
   //   push(pFalseConstant);
   //   return;
   //}

    switch ( pAny1 -> ObjectType() ) {

    case object::number:
        if ( pAny1 -> Value() == pAny2 -> Value() )
            push(pTrueConstant);
        else
            push(pFalseConstant);
        return;

    case object::atom:
        if ( ! ( strlen(pAny1 -> Contents()) == strlen(pAny2 -> Contents()) ) ) {
            push(pFalseConstant);
            return;
        }

        if ( 0 == memcmp(pAny1 -> Contents(),pAny2 -> Contents(),strlen(pAny1 -> Contents())) )
            push(pTrueConstant);
        else
            push(pFalseConstant);

        return;

    case object::name:
    case object::literal:
        if ( 0 == strcmp(pAny1 -> Name(),pAny2 -> Name()) && strlen(pAny1 -> Name()) == strlen(pAny2 -> Name()) )
            push(pTrueConstant);
        else
            push(pFalseConstant);
        return;
#if 0
    case object::matrix: {

        if ( object::matrix != pAny2 -> ObjectType() ) {
            push(pFalseConstant);
            return;
        }

        // ?? got an instance of a matrix testing is equal to an integer ???
        // don't know if that is valid, 
        push(pTrueConstant);
        return;
        }
#endif

    case object::dictionary: {

        if ( object::dictionary != pAny2 -> ObjectType() ) {
            push(pFalseConstant);
            return;
        }

        dictionary *pDict1 = reinterpret_cast<dictionary *>(pAny1);
        dictionary *pDict2 = reinterpret_cast<dictionary *>(pAny2);
        if ( ! ( pDict1 -> size() == pDict2 -> size() ) ) {
            push(pFalseConstant);
            return;
        }

        // ?!?!?!?
        // Is this right (!?)

        if ( pDict1 -> hasSameEntries(pDict2) ) {
            push(pFalseConstant);
            return;
        }
#if 0
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
#endif
        push(pTrueConstant);
        }
        return;

    case object::objTypeMatrix:
printf("wtf");
    case object::objTypeArray: {

        if ( ! ( object::objTypeArray == pAny2 -> ObjectType() ) && ! ( object::objTypeMatrix == pAny2 -> ObjectType() ) ) {
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
            push(pDict1 -> getElement(k));
            push(pDict2 -> getElement(k));
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
    push(pSystemDict -> retrieve("theErrordict"));
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

    /*

    This is incomplete. Right now, it only works to de-reference literals or strings that may
    point to some procedure or operator in a dictionary.

    Specifically, it would NOT correctly parse something like "(3 2 add)" that is
    in the first example.

    More precisely, it is the cvx operator that is incomplete. In the case of example 1,
    that operator should have created something like a procedure.

    */
    object *pObject = top();

    if ( ! ( object::executableAttribute::executable == pObject -> theExecutableAttribute ) )
        return;

    for ( long k = 0; k < 2; k++ ) {

        if ( object::procedure == pObject -> ObjectType() ) {
            pop();
            reinterpret_cast<procedure *>(pObject) -> execute();
            return;
        }

        if ( object::objectType::directExecutable == pObject -> ObjectType() ) {
            pop();
            directExec *pDirectExec = reinterpret_cast<directExec *>(pObject);
            void (__thiscall job::*pOperator)() = pDirectExec -> Operator();
            (this ->* pOperator)();
            return;
        }

        resolve();

        pObject = top();

    }

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
    currentGS() -> fillpath();
    return;
    }

    void job::operatorFilter() {
/*
    filter
        datasrc dict param1 … paramn filtername filter file
        datatgt dict param1 … paramn filtername filter file

    creates and returns a filtered file (see Sections 3.8.4, “Filters,” and 3.13, “Filtered
    Files Details”).

    The first operand specifies the underlying data source or data target that the filter
    is to read or write. It can be a file, a procedure, or a string.
    The dict operand contains additional parameters that control how the filter is to
    operate. It can be omitted whenever all dictionary-supplied parameters have their
    default values for the given filter. The operands param1 through paramn are 
    additional parameters that some filters require as operands rather than in dict; most
    filters do not require these operands. The number and types of parameters specified 
    in dict or as operands depends on the filter name. The filtername operand identifies 
    the data transformation that the filter is to perform. The standard filter names are
    as follows:

        (missing table, page 590, also see page 85)

    An encoding filter is an output (writeable) file; a decoding filter is an input (readable)
    file. The file object returned by the filter can be used as an operand to normal file 
    input and output operators, such as read and write. Reading from an input filtered 
    file causes the filter to read from the underlying data source and transform the data. 
    Similarly, writing to an output filtered file causes the filter to transform the data 
    and write it to the underlying data target. 

    The filter operator disregards the current VM allocation mode. Instead, the returned 
    file object is created in global VM if and only if all the composite objects it
    retains after filter creation are in global VM. These objects include the underlying
    source or target object, the end-of-data string for SubFileDecode, and the dict operand 
    of DCTDecode or DCTEncode (which can contain strings and arrays used during operation 
    f the filter). 

    Note that the dict operand is not retained by filters other than DCTDecode and
    DCTEncode. The dictionary is used only as a container for parameters, which are
    completely processed by the filter operator. Therefore, the VM allocation mode of
    this dictionary is irrelevant.
    
    Errors: invalidaccess, ioerror, limitcheck, rangecheck, stackunderflow,
    typecheck, undefined

    See Also: file, closefile, resourceforall

*/

    currentGS() -> filter();

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

    object *pKey = pop();

    font *pFont = reinterpret_cast<font *>(pFontDirectory -> retrieve(pKey -> Name()));

    if ( NULL == pFont ) {
        pFont = currentGS() -> findFont(pKey -> Name());
        if ( NULL == pFont ) {
            char szMessage[1024];
            sprintf(szMessage,"operator: findfont. font %s is undefined",pKey -> Name());
            throw new invalidfont(szMessage);
            return;
        }
        pFontDirectory -> put(pFont -> fontName(),pFont);
    }

    push(pFont);

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

    object *pCategory = pop();
    object *pKey = pop();

    if ( 0 == strcmp(pCategory -> Name(),"Font") ) {

        object *pFont = pFontDirectory -> retrieve(pKey -> Name());

        if ( NULL == pFont ) {
            char szMessage[1024];
            sprintf(szMessage,"%s Line: %d, operator: findResource. The font resource %s was not found",__FUNCTION__,__LINE__,pKey -> Name());
            throw new undefinedresource(szMessage);
        }

        push(pFont);
        return;

    }

    boolean categoryExists = false;
    for ( resource *pResource : resourceList ) {
        if ( 0 == strcmp(pCategory -> Name(),pResource -> Category() ) ) {
            categoryExists = true;
            if ( 0 == strcmp(pKey -> Name(),pResource -> Name()) ) {
                push(pResource -> Instance());
                return;
            }
        }
    }

    char szMessage[1024];
    sprintf_s<1024>(szMessage,"%s: Line: %d: findresource error for key: %s and category: %s",__FUNCTION__,__LINE__,pKey -> Contents(),pCategory -> Contents());

    if ( ! categoryExists ) 
        throw new undefined(szMessage);
    else
        throw new undefinedresource(szMessage);

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
    procedure *pProc = reinterpret_cast<procedure *>(pop());
    object *pLimit = pop();
    object *pIncrement = pop();
    object *pInitial = pop();

    long initial = pInitial -> IntValue();
    long increment = pIncrement -> IntValue();
    long limit = pLimit -> IntValue();
    long control = initial;

    object *pControl = new (CurrentObjectHeap()) object(this,control);

    while ( ( 0 < increment && control <= limit ) || ( increment < 0 && control >= limit ) ) {
        push(pControl);
        executeProcedure(pProc);
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
    case object::objTypeArray: {
        array *pArray = reinterpret_cast<array *>(pSource);
        long n = pArray -> size();
        for ( long k = 0; k < n; k++ ) {
            push(pArray -> getElement(k));
            push(pProc);
            executeObject();
        }
        }
        break;

    case object::font:
        static_cast<dictionary *>(reinterpret_cast<font *>(pSource)) -> forAll(pProc);
        break;

    case object::dictionary: 
        reinterpret_cast<dictionary *>(pSource) -> forAll(pProc);
        break;

    case object::string: {
        __debugbreak();
        }
        break;

   default: {
        char szMessage[1024];
        sprintf_s<1024>(szMessage,"%s: Line: %ld. The source object type (%d) for forall does not appear to be a array, packedarray, dict, or string",__FUNCTION__,__LINE__,pSource -> ObjectType());
        throw new typecheck(szMessage);
        }
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
         v1 = p1 -> OBJECT_POINT_TYPE_VALUE;
      else
         v1 = (double) p1 -> IntValue();
      double v2 = 0.0;
      if ( object::real == p2 -> ValueType() )
         v2 = p2 -> OBJECT_POINT_TYPE_VALUE;
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
    dictionary *pFontDictionary = NULL;

    switch ( pTarget -> ObjectType() ) {

    case object::objTypeArray:
        push(reinterpret_cast<array *>(pTarget) -> getElement(pIndex -> IntValue()));
        return;

    case object::packedarray: {
        __debugbreak();
        }
        break;

    case object::font:
        pFontDictionary = static_cast<dictionary *>(reinterpret_cast<font *>(pTarget));

    case object::dictionary: {
        if ( NULL == pFontDictionary )
            pFontDictionary = reinterpret_cast<dictionary *>(pTarget);

        object *pObject = pFontDictionary -> retrieve(pIndex -> Name());

        if ( NULL == pObject ) {
            char szError[1024];
            sprintf(szError,"operator get: cannot find %s in font or dictionary %s (type: %s)",pIndex -> Name(),pFontDictionary -> Name(),pFontDictionary -> TypeName());
            throw new undefined(szError);
        }

        push(pObject);
        break;
        }

    case object::binaryString:
    case object::constantString:
    case object::hexString:
    case object::string: {
        push(new (CurrentObjectHeap()) object(this,(long)pTarget -> get(pIndex -> IntValue())));
        break;
        }

    }

    return;
    }

#if 0

    glyphshow name glyphshow –
    cid glyphshow –

    shows the glyph for a single character from the current font or CIDFont; the character 
    is identified by name if a base font or by CID if a CIDFont. If the current
    font is a composite (Type 0) font, an invalidfont error occurs.

    Unlike all other show variants, glyphshow bypasses the current font’s Encoding 
    array; it can access any character in the font, whether or not that character’s 
    name is present in the font’s encoding vector. glyphshow is the only show 
    variant that works directly with a CIDFont.

    For a base font, the behavior of glyphshow depends on the current font’s
    FontType value. For fonts that contain a CharStrings dictionary, such as Type 1
    fonts, glyphshow looks up name there to obtain a glyph description to execute. If
    name is not present in the CharStrings dictionary, glyphshow substitutes the
    .notdef entry, which must be present.

    For Type 3 fonts, if the font dictionary contains a BuildGlyph procedure,
    glyphshow pushes the current font dictionary and name on the operand stack and
    then invokes BuildGlyph in the usual way (see Section 5.7, “Type 3 Fonts”). If
    there is no BuildGlyph procedure, but only a BuildChar procedure, glyphshow
    searches the font’s Encoding array for an occurrence of name. If it finds one, it
    pushes the font dictionary and the array index on the operand stack, then invokes
    BuildChar in the usual way. If name is not present in the encoding, glyphshow
    substitutes the name .notdef and repeats the search. If .notdef is not present either,
    an invalidfont error occurs.

    For a CIDFont, glyphshow proceeds as show would for a CID-keyed font whose
    mapping algorithm yields this CIDFont with cid as the character selector. A
    rangecheck error occurs if cid is outside the valid range of CIDs (see Appendix B).
    Except for the means of selecting the character to be shown, glyphshow behaves
    the same as show. Like show, glyphshow can access glyphs that are already in the
    font cache; glyphshow does not always need to execute the character’s glyph description.

    Errors: invalidaccess, invalidfont, nocurrentpoint, stackunderflow, typecheck
    See Also: show

#endif

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

    currentGS() -> gRestore();
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
         v1 = p1 -> OBJECT_POINT_TYPE_VALUE;
      else
         v1 = (double) p1 -> IntValue();
      double v2 = 0.0;
      if ( object::real == p2 -> ValueType() )
         v2 = p2 -> OBJECT_POINT_TYPE_VALUE;
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

    currentGS() -> gSave();

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
    proc2 if bool is false. The ifelse operator pushes no results of its own on the operand stack
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


    void job::operatorImage() {
/*
    image 

        width height bits/sample matrix datasrc image –
        dict image – (LanguageLevel 2)

    paints a sampled image onto the current page. This description only summarizes
    the general behavior of the image operator; see Section 4.10, “Images,” for full details.

    The image is a rectangular array of width × height sample values, each consisting
    of bits/sample bits of data. Valid values of bits/sample are 1, 2, 4, 8, or 12. The data
    is received as a sequence of characters—that is, 8-bit integers in the range 0 to 255.
    If bits/sample is less than 8, sample values are packed from left to right within a
    character (see Section 4.10.2, “Sample Representation”).

    The image is considered to exist in its own coordinate system, or image space. The
    rectangular boundary of the image has its lower-left corner at coordinates (0, 0)
    and its upper-right corner at (width, height). The matrix operand defines a 
    transformation from user space to image space.

    In the first form of the operator, the parameters are specified as separate operands. 
    This form always renders a monochrome image according to the
    DeviceGray color space, regardless of the current color space in the graphics state.
    This is the only form supported in LanguageLevel 1.

    In the second form (LanguageLevel 2), the parameters are contained as entries in
    an image dictionary dict, which is supplied as the single operand. This form 
    renders either a monochrome or a color image, according to the current color space.
    The number of component values per source sample and the interpretation of
    those values depend on the color space.

    In LanguageLevel 1, datasrc must be a procedure. In LanguageLevel 2 or 3, it may
    be any data source—a procedure, a string, or a readable file, including a filtered
    file (see Section 3.13, “Filtered Files Details”).

    If datasrc is a procedure, it is executed repeatedly to obtain the actual image data.
    datasrc must return a string on the operand stack containing any number of additional 
    characters of sample data. The sample values are assumed to be received in
    a fixed order: (0,0) to (widtrh - 1,0), then (0,1) to (width - 1,1), and so on. If
    datasrc returns a string of length 0, image will terminate execution prematurely.
    Execution of this operator is not permitted in certain circumstances; see

    Section 4.8.1, “Types of Color Space.”

    Errors: invalidaccess, ioerror, limitcheck, rangecheck, stackunderflow,
    typecheck, undefined, undefinedresult
    See Also: imagemask, colorimage
*/

    currentGS() -> image();

    return;
    }


    void job::operatorImagemask() {
/*
    imagemask 
        width height polarity matrix datasrc imagemask –
        dict imagemask – (LanguageLevel 2)

    uses a monochrome sampled image as a stencil mask of 1-bit samples to control
    where to apply paint to the current page in the current color 
    (see “Stencil Masking” on page 302.)

    In the first form of the operator (LanguageLevel 1), the parameters are specified as
    separate operands. In the second form (LanguageLevel 2), the parameters are
    contained as entries in an image dictionary dict, which is supplied as the
    single operand. The behavior of the operator is the same in both forms.
    imagemask uses the width, height, matrix, and datasrc operands in precisely the
    same way the image operator uses them. The polarity operand is a boolean value
    that determines the polarity of the mask. If polarity is true, portions of the page
    corresponding to mask values of 1 are painted in the current color, while those
    corresponding to mask values of 0 are left unchanged; if polarity is false, these
    polarities are reversed. The polarity controls the interpretation of mask samples
    only; it has no effect on the color of the pixels that are painted.

    In the second form of imagemask, the polarity is specified by means of the
    Decode entry in the image dictionary. Decode values of [1 0] and [0 1] denote
    polarity values of true and false, respectively.

    In LanguageLevel 1, datasrc must be a procedure. In LanguageLevel 2 or 3, it may
    be any data source—a procedure, a string, or a readable file, including a filtered
    file (see Section 3.13, “Filtered Files Details”). 

    Errors: invalidaccess, ioerror, limitcheck, stackunderflow, typecheck,
    undefinedresult

    See Also: image, colorimage

*/

    object *pDict = pop();

    if ( object::objectType::dictionary == pDict -> ObjectType() ) 
        return;

    pop();
    pop();
    pop();
    pop();

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
    POINT_TYPE x,y;
    object *pTopObject = pop();

    switch ( pTopObject -> ObjectType() ) {
    case object::objTypeMatrix: {
        matrix *pMatrix = reinterpret_cast<matrix *>(pTopObject);
        y = pop() -> OBJECT_POINT_TYPE_VALUE;
        x = pop() -> OBJECT_POINT_TYPE_VALUE;
        currentGS() -> untransformPointInPlace(pMatrix,x,y,&x,&y);
        }
        break;

    default: {
        y = pTopObject -> OBJECT_POINT_TYPE_VALUE;
        x = pop() -> OBJECT_POINT_TYPE_VALUE;
        currentGS() -> untransformPointInPlace(x,y,&x,&y);
        }
    }

    push(new (CurrentObjectHeap()) object(this,x));
    push(new (CurrentObjectHeap()) object(this,y));
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
   push(pISOLatin1Encoding);
   return;
   }

    void job::operatorInitmatrix() {

/*
    initmatrix 

        – initmatrix –

    sets the current transformation matrix (CTM) in the graphics state to the default
    matrix for the current output device. This matrix transforms the default 
    user coordinate system to device space (see Section 4.3.1, “User Space and Device
    Space”). For a page device, the default matrix is initially established by the
    setpagedevice operator.

    There are few situations in which a PostScript program should invoke initmatrix
    explicitly. A page description that invokes initmatrix usually produces incorrect
    results if it is embedded within another, composite page.

    Errors: none
    See Also: defaultmatrix, setmatrix, currentmatrix
*/

operatorDebug();
    //currentGS() -> initMatrix();
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
    POINT_TYPE x,y;
    object *pTopObject = pop();

    switch ( pTopObject -> ObjectType() ) {
    case object::objTypeMatrix: {
        matrix *pMatrix = reinterpret_cast<matrix *>(pTopObject);
        y = pop() -> OBJECT_POINT_TYPE_VALUE;
        x = pop() -> OBJECT_POINT_TYPE_VALUE;
        currentGS() -> untransformPoint(pMatrix,x,y,&x,&y);
        }
        break;

    default: {
        y = pTopObject -> OBJECT_POINT_TYPE_VALUE;
        x = pop() -> OBJECT_POINT_TYPE_VALUE;
        currentGS() -> untransformPoint(x,y,&x,&y);
        }
    }

    push(new (CurrentObjectHeap()) object(this,x));
    push(new (CurrentObjectHeap()) object(this,y));
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
    object *pDictObject = pop();
    dictionary *pDictionary = NULL;
    if ( object::objectType::dictionary == pDictObject -> ObjectType() ) 
        pDictionary = reinterpret_cast<dictionary *>(pDictObject);
    else if ( object::objectType::font == pDictObject -> ObjectType() )
        pDictionary = static_cast<dictionary *>(reinterpret_cast<font *>(pDictObject));
    else {
       char szMessage[1024];
        sprintf(szMessage,"operator begin: object %s is not a dictionary",pDictObject -> Name());
        throw new typecheck(szMessage);
        return;
    }

    if ( pDictionary -> exists(pKey -> Contents()) )
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
   
    case object::objTypeArray:
        length = reinterpret_cast<array *>(pItem) -> size();
        break;

    case object::font:
        length = static_cast<dictionary *>(reinterpret_cast<font *>(pItem)) -> size();
        break;

    case object::dictionary:
        length = reinterpret_cast<dictionary *>(pItem) -> size();
        break;

    case object::atom: {

        switch ( pItem -> ValueType() ) {
        case object::string:
        case object::character:
            length = (long)reinterpret_cast<string *>(pItem) -> length();
            break;

        case object::constantString:
            length = (long)reinterpret_cast<constantString *>(pItem) -> length();
            break;

        case object::binaryString:
            length = (DWORD)strlen(pItem -> Contents()) / 2;
            break;

        default: 
            char szError[1024];
            sprintf(szError,"File: %s, Line: %d, in operator length: object = %s, the type is (%s,%s) invalid",__FILE__,__LINE__,pItem -> Name(),pItem -> TypeName(),pItem -> ValueTypeName());
            throw new typecheck(szError);
        }
        }
        break;

    default: {
        char szError[1024];
        sprintf(szError,"File: %s, Line: %d in operator length: object = %s, the type is (%s,%s) invalid",__FILE__,__LINE__,pItem -> Name(),pItem -> TypeName(),pItem -> ValueTypeName());
        throw new typecheck(szError);
        }

    }

    push(new (CurrentObjectHeap()) object(this,length));

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
    pGraphicsState -> lineto();
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

        std::map<size_t,name *>::iterator it = validNames.find(std::hash<std::string>()(pKey -> Name()));

        if ( ! ( it == validNames.end() ) ) {
            push(it -> second);
            return;
        }

        push(pKey);
        {
        char szMessage[1024];
        sprintf_s<1024>(szMessage,"%s,Line: %d. key %s is undefined",__FUNCTION__,__LINE__,pKey -> Name());
        throw new undefined(szMessage);
        }
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
