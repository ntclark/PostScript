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

#include "PostScript objects\graphicsState.h"

   void job::pdfOperator_apostrophe() {
/*
   string  '
      Move to the next line and show a text string. This operator shall have the same effect as the code
         T*
         string Tj
*/
   pdfOperator_TStar();
   pdfOperator_Tj();
   return;
   }

   void job::pdfOperator_quote() {
/*
   aw ac string  "

   Move to the next line and show a text string, using aw as the word spacing and ac as the character 
   spacing (setting the corresponding parameters in the text state). aw and ac shall be numbers 
   expressed in unscaled text space units. This operator shall have the same effect as this code:

      aw Tw
      ac Tc
      string '
*/
   object *pString = pop();
   object *pAc = pop();
   object *pAw = pop();
   push(pAw);
   pdfOperator_Tw();
   push(pAc);
   pdfOperator_Tc();
   push(pString);
   pdfOperator_apostrophe();
   return;
   }

   void job::pdfOperator_b() {
/*
   —  b

   Close, fill, and then stroke the path, using the nonzero winding number rule to determine the region 
   to fill. This operator shall have the same effect as the sequence h B. 
   See also 11.7.4.4, "Special Path-Painting Considerations".
*/
   return;
   }

   void job::pdfOperator_bStar() {
/*
   — b*

   Close, fill, and then stroke the path, using the even-odd rule to determine the region to fill. 
   This operator shall have the same effect as the sequence h B*. 
   See also 11.7.4.4, "Special Path-Painting Considerations".
*/
   return;
   }

   void job::pdfOperator_B() {
/*
   —  B

   Fill and then stroke the path, using the nonzero winding number rule to determine the region to fill. 
   This operator shall produce the same result as constructing two identical path objects, painting the 
   first with f and the second with S.

   NOTE 
      The filling and stroking portions of the operation consult different values of several graphics 
      state parameters, such as the current colour. 

   See also 11.7.4.4, "Special Path-Painting Considerations".
*/
   return;
   }

   void job::pdfOperator_BStar() {
/*
   — B*

   Fill and then stroke the path, using the even-odd rule to determine the region to fill. 
   This operator shall produce the same result as B, except that the path is filled as if 
   with f* instead of f. 

   See also 11.7.4.4, "Special Path-Painting Considerations".
*/
   return;
   }

   void job::pdfOperator_BDC() {
/*
   tag properties  BDC

   Begin a marked-content sequence with an associated property list, terminated by a balancing 
   EMC operator. tag shall be a name object indicating the role or significance of the sequence. 
   properties shall be either an inline dictionary containing the property list or a name object 
   associated with it in the Properties subdictionary of the current resource dictionary 
   (see 14.6.2, “Property Lists”).
*/
   return;
   }

   void job::pdfOperator_BI() {
/*
   — BI

   Begin an inline image object.
*/



   return;
   }

   void job::pdfOperator_BMC() {
/*
   tag  BMC

   Begin a marked-content sequence terminated by a balancing EMC operator. tag shall be a name object 
   indicating the role or significance of the sequence.
*/
   pop();
   return;
   }

   void job::pdfOperator_BT() {
/*
   BT

      Begin a text object, initializing the text matrix, Tm, and the text line matrix, 
      Tlm, to the identity matrix. Text objects shall not be nested; a second BT 
      shall not appear before an ET.
*/
Beep(2000,200);
#if 0
   graphicsStateStack.current() -> beginText();
#endif
   return;
   }

   void job::pdfOperator_BX() {
/*
   — BX

   (PDF 1.1) Begin a compatibility section. Unrecognized operators (along with their operands) 
   shall be ignored without error until the balancing EX operator is encountered.
*/
   return;
   }

   void job::pdfOperator_c() {
/*
   x1 y1 x2 y2 x3 y3  c

      curveto

      Append a cubic Bézier curve to the current path. The curve shall extend from the current point to the point (x3, y3), 
      using (x1, y1) and (x2, y2) as the Bézier control points (see 8.5.2.2, "Cubic Bézier Curves"). The new current 
      point shall be (x3, y3).
*/
   operatorCurveto();
   return;
   }

   void job::pdfOperator_cm() {
/*
   a b c d e f cm

   Modify the current transformation matrix (CTM) by concatenating the specified matrix 
   (see 8.3.2, "Coordinate Spaces"). Although the operands specify a matrix, they shall 
   be written as six separate numbers, not as an array.
*/
   POINT_TYPE d[6];
   for ( long k = 5; k > -1; k-- )
      d[k] = pop() -> OBJECT_POINT_TYPE_VALUE;

   currentGS() -> concat(d);
   return;
   }
   
   void job::pdfOperator_CS() {
/*
   name CS

   (PDF 1.1) Set the current colour space to use for stroking operations. The operand name 
   shall be a name object. If the colour space is one that can be specified by a name and no 
   additional parameters (DeviceGray, DeviceRGB, DeviceCMYK, and certain cases of Pattern), 
   the name may be specified directly. Otherwise, it shall be a name defined in the ColorSpace 
   subdictionary of the current resource dictionary (see 7.8.3, "Resource Dictionaries"); 
   the associated value shall be an array describing the colour space (see 8.6.3, "Colour Space Families").

   The names DeviceGray, DeviceRGB, DeviceCMYK, and Patternalways identify the corresponding 
   colour spaces directly; they never refer to resources in the ColorSpace subdictionary.

   The CS operator shall also set the current stroking colour to its initial value, which 
   depends on the colour space:

   In a DeviceGray, DeviceRGB, CalGray, or CalRGB colour space, the initial colour shall have all components 
   equal to 0.0.

   In a DeviceCMYK colour space, the initial colour shall be [0.0 0.0 0.0 1.0].

   In a Lab or ICCBased colour space, the initial colour shall have all components equal to 0.0 unless that 
   falls outside the intervals specified by the space’s Range entry, in which case the nearest valid value 
   shall be substituted.

   In an Indexed colour space, the initial colour value shall be 0.

   In a Separation or DeviceN colour space, the initial tint value shall be 1.0 for all colorants.

   In a Pattern colour space, the initial colour shall be a pattern object that causes nothing to 
   be painted.

*/
   pop();
   return;
   }

   void job::pdfOperator_cs() {
/*
   cs
      (PDF 1.1) Same as CS but used for nonstroking operations.
*/
   pop();
   return;
   }

   void job::pdfOperator_d() {
/*
   dashArray dashPhase d

      Set the line dash pattern in the graphics state (see 8.4.3.6, "Line Dash Pattern").
*/
   pop();
   pop();
   return;
   }

   void job::pdfOperator_d0() {
/*
   wx wy  d0

   Set width information for the glyph and declare that the glyph description specifies both its shape and its colour.

   NOTE
      This operator name ends in the digit 0.

   wx denotes the horizontal displacement in the glyph coordinate system; it shall be consistent with the 
   corresponding width in the font’s Widths array. wy shall be 0 (see 9.2.4, "Glyph Positioning and Metrics").
   This operator shall only be permitted in a content stream appearing in a Type 3 font’s CharProcs dictionary. 
   It is typically used only if the glyph description executes operators to set the colour explicitly.
*/
   pop();
   pop();
   return;
   }

   void job::pdfOperator_d1() {
/*
   wx wy llx lly urx ury d1

   Set width and bounding box information for the glyph and declare that the glyph description specifies only shape, not colour.

   NOTE
      This operator name ends in the digit 1.

   wx denotes the horizontal displacement in the glyph coordinate system; it shall be consistent with the 
   corresponding width in the font’s Widths array. wy shall be 0 (see 9.2.4, "Glyph Positioning and Metrics").
   llx and lly denote the coordinates of the lower-left corner, and urxand ury denote the upper-right corner, 
   of the glyph bounding box. The glyph bounding box is the smallest rectangle, oriented with the axes of the 
   glyph coordinate system, that completely encloses all marks placed on the page as a result of executing 
   the glyph’s description. The declared bounding box shall be correct—in other words, sufficiently large 
   to enclose the entire glyph. If any marks fall outside this bounding box, the result is unpredictable.

   A glyph description that begins with the d1 operator should not execute any operators that set the 
   colour (or other colour-related parameters) in the graphics state; any use of such operators shall 
   be ignored. The glyph description is executed solely to determine the glyph’s shape. Its colour shall 
   be determined by the graphics state in effect each time this glyph is painted by a text-showing operator. 
   For the same reason, the glyph description shall not include an image; however, an image mask is acceptable, 
   since it merely defines a region of the page to be painted with the current colour.

   This operator shall be used only in a content stream appearing in a Type 3 font’s CharProcs dictionary.
*/
   pop();
   pop();
   pop();
   pop();
   pop();
   pop();
   return;
   }

   void job::pdfOperator_Do() {
/*
   name Do

      Paint the specified XObject. The operand name shall appear as a key in the XObject subdictionary of 
      the current resource dictionary (see 7.8.3, "Resource Dictionaries"). The associated value shall 
      be a stream whose Type entry, if present, is XObject. The effect of Do depends on the value of the 
      XObject’s Subtype entry, which may be Image (see 8.9.5, "Image Dictionaries"), Form (see 8.10, "Form XObjects"), 
      or PS (see 8.8.2, "PostScript XObjects").
*/
   pop();
   return;
   }

   void job::pdfOperator_DP() {
/*
   tag properties DP

   Designate a marked-content point with an associated property list. tagshall be a name object indicating 
   the role or significance of the point. properties shall be either an inline dictionary containing the 
   property list or a name object associated with it in the Properties subdictionary of the current resource 
   dictionary (see 14.6.2, “Property Lists”).
*/
   pop();
   pop();
   return;
   }

   void job::pdfOperator_EI() {
/*
   — EI

   End an inline image object.
*/
   return;
   }

   void job::pdfOperator_EMC() {
/*
   — EMC

   End a marked-content sequence begun by a BMC or BDC operator.
*/
   return;
   }

   void job::pdfOperator_ET() {
/*
      End a text object, discarding the text matrix.   
*/
Beep(2000,200);
#if 0
   graphicsStateStack.current() -> endText();
#endif
   return;
   }


   void job::pdfOperator_EX() {
/*
   — EX

   (PDF 1.1) End a compatibility section begun by a balancing BX operator. Ignore any unrecognized 
   operands and operators from previous matching BX onward.
*/
   return;
   }

   void job::pdfOperator_fStar() {
/*
   f*
      eofill
         Fill path using even-odd rule
*/
   operatorEofill();
   return;
   }

   void job::pdfOperator_f() {
/*
   f
      fillpath
         Fill the path, using the nonzero winding number rule to determine the region to fill (see 8.5.3.3.2, 
         "Nonzero Winding Number Rule"). Any subpaths that are open shall be implicitly closed before being filled.
*/
   return;
   }

   void job::pdfOperator_F() {
/*
   — F

   Equivalent to f; included only for compatibility. Although PDF readerapplications shall be able to 
   accept this operator, PDF writer applications should use f instead.
*/
   pdfOperator_f();
   return;
   }

   void job::pdfOperator_g() {
/*
gray
g
Same as G but used for nonstroking operations.
*/
   pop();
   return;
   }

   void job::pdfOperator_G() {
/*
   gray  G

   Set the stroking colour space to DeviceGray (or the DefaultGray colour space; see 8.6.5.6, 
   "Default Colour Spaces") and set the gray level to use for stroking operations. 
   gray shall be a number between 0.0 (black) and 1.0 (white).
*/
   pop();
   return;
   }

   void job::pdfOperator_gs() {
/*
   dictName gs

      (PDF 1.2) Set the specified parameters in the graphics state. dictName shall be the name 
      of a graphics state parameter dictionary in the ExtGState subdictionary of the current 
      resource dictionary (see the next sub-clause).
*/
   currentGS() -> setGraphicsStateDict(pop() -> Contents());
   return;
   }


   void job::pdfOperator_h() {
/*
   closepath
      Close subpath
*/
   operatorClosepath();
   return;
   }

   void job::pdfOperator_i() {
/*
   flatness  i

   Set the flatness tolerance in the graphics state (see 10.6.2, "Flatness Tolerance"). 
   flatness is a number in the range 0 to 100; a value of 0 shall specify the output 
   device’s default flatness tolerance.
*/
   pop();
   return;
   }

   void job::pdfOperator_ID() {
//
//NTC: 02-19-2012: This is not really an operator, but is an object in an inline image specification.
// It will appear between BI and EI and is *followed* by binary data that is the I mage  D efinition.
// The size of the Image Data is specified in the operators or data that comes after BI (/W, /H, etc).
//

/*

   On page 215:

   ID       
      Begin the image data for an inline image object.
*/




   return;
   }


   void job::pdfOperator_j() {
/*
   j
      setlinejoin
         Set line join style
*/
   operatorSetlinejoin();
   return;
   }

   void job::pdfOperator_J() {
/*
   lineCap J

      Set the line cap style in the graphics state (see 8.4.3.3, "Line Cap Style").
*/
   operatorSetlinecap();
   return;
   }
   void job::pdfOperator_k() {
/*
   c m y   k
      setcmykcolor
         Set CMYK color for nonstroking operations
*/
   pop();
   pop();
   pop();
   return;
   }

   void job::pdfOperator_K() {
/*
   c m y   K
      Set the stroking colour space to DeviceCMYK (or the DefaultCMYKcolour space; see 8.6.5.6, 
      "Default Colour Spaces") and set the colour to use for stroking operations. Each operand 
      shall be a number between 0.0 (zero concentration) and 1.0 (maximum concentration). 
      The behaviour of this operator is affected by the overprint mode (see 8.6.7, "Overprint Control").
*/
   pop();
   pop();
   pop();
   return;
   }

   void job::pdfOperator_l() {
/*
   lineto
      Append straight line segment to path
*/
   operatorLineto();
   return;
   }

   void job::pdfOperator_m() {
/*
   moveto
      Begin new subpath
*/
   operatorMoveto();
   return;
   }

   void job::pdfOperator_M() {
/*
   M
      setmiterlimit
         Set miter limit
*/
   operatorSetmiterlimit();
   return;
   }

   void job::pdfOperator_MP() {
/*
   tag MP

   Designate a marked-content point. tag shall be a name object indicating the role or significance of the point.
*/
   pop();
   return;
   }


   void job::pdfOperator_n() {
/*
   End path without filling or stroking
*/
   return;
   }


   void job::pdfOperator_q() {
/*
   gsave
      Save graphics state
*/
Beep(2000,200);
   //graphicsStateStack.gSave();
   return;
   }

   void job::pdfOperator_Q() {
/*
   Q
      Restore the graphics state by removing the most recently saved state from the stack and making it 
      the current state (see 8.4.2, "Graphics State Stack").
*/
Beep(2000,200);
   //graphicsStateStack.gRestore();
   return;
   }

   void job::pdfOperator_re() {
/*
   x y width height  re

      Append a rectangle to the current path as a complete subpath, with lower-left corner (x, y) and dimensions
      width and height in user space. The operation

         x y width height re

      is equivalent to

         x y m
         (x + width) y l
         (x + width) (y + height) l
         x (y + height) l
         h
*/
   object *pHeight = pop();
   object *pWidth = pop();
   object *pY = pop();
   object *pX = pop();

   push(pX);
   push(pY);
   pdfOperator_m();
   push(pX);
   push(pWidth);
   operatorAdd();
   operatorDup();
   push(pY);
   pdfOperator_l();
   push(pY);
   push(pHeight);
   operatorAdd();
   pdfOperator_l();
   push(pX);
   push(pY);
   push(pHeight);
   operatorAdd();
   pdfOperator_l();
   pdfOperator_h();
   
   return;
   }

   void job::pdfOperator_rg() {
/*
   r g b rg

   Same as RG but used for nonstroking operations.
*/
   pop();
   pop();
   pop();
   return;
   }

   void job::pdfOperator_RG() {
/*
   r g b RG

   Set the stroking colour space to DeviceRGB (or the DefaultRGB colour space; see 8.6.5.6, "Default Colour Spaces") 
   and set the colour to use for stroking operations. Each operand shall be a number between 0.0 (minimum intensity) 
   and 1.0 (maximum intensity).

*/
   pop();
   pop();
   pop();
   return;
   }

   
   void job::pdfOperator_ri() {
/*
   intent ri

   (PDF 1.1) Set the colour rendering intent in the graphics state (see 8.6.5.8, "Rendering Intents").
*/
   pop();
   return;
   }

   void job::pdfOperator_s() {
/*
   — s

   Close and stroke the path. This operator shall have the same effect as the sequence h S.
*/
   return;
   }

   void job::pdfOperator_S() {
/*
   S  
      stroke
         Stroke path
*/
   operatorStroke();
   return;
   }

   void job::pdfOperator_sc() {
/*
   c1 … cn  sc

   (PDF 1.1) Same as SC but used for nonstroking operations.
*/
   return;
   }

   void job::pdfOperator_SC() {
/*
   c1 … cn SC

   (PDF 1.1) Set the colour to use for stroking operations in a device, CIE-based (other than ICCBased), 
   or Indexed colour space. The number of operands required and their interpretation depends on 
   the current stroking colour space:

   For DeviceGray, CalGray, and Indexed colour spaces, one operand shall be required (n = 1).

   For DeviceRGB, CalRGB, and Lab colour spaces, three operands shall be required (n = 3).

   For DeviceCMYK, four operands shall be required (n = 4).
*/
   return;
   }

   void job::pdfOperator_SCN() {
/*
   setcolor
      (PDF 1.2) Set color for stroking operations (ICCBaseda nd special colour spaces)
*/
//
// 9-13-2011: The stack depth for this operator depends on the current colorspace.
// TODO: implement the colorspace operator to know what this stack depth is
#if 0
   pop();
   pop();
   pop();
   pop();
#endif
   return;
   }

   void job::pdfOperator_scn() {
/*
   setcolor
      (PDF 1.2) Set color for nonstroking operations (ICCBased and special colour spaces)
*/
#if 0
   pop();
   pop();
   pop();
   pop();
#endif
   return;
   }

   void job::pdfOperator_sh() {
/*
   name sh

   (PDF 1.3) Paint the shape and colour shading described by a shading dictionary, subject to 
   the current clipping path. The current colour in the graphics state is neither used nor altered. 
   The effect is different from that of painting a path using a shading pattern as the current colour.
   name is the name of a shading dictionary resource in the Shadingsubdictionary of the current resource 
   dictionary (see 7.8.3, "Resource Dictionaries"). All coordinates in the shading dictionary are 
   interpreted relative to the current user space. (By contrast, when a shading dictionary is used 
   in a type 2 pattern, the coordinates are expressed in pattern space.) All colours are interpreted 
   in the colour space identified by the shading dictionary’s ColorSpace entry (see Table 78). 
   The Background entry, if present, is ignored.

   This operator should be applied only to bounded or geometrically defined shadings. If applied to an 
   unbounded shading, it paints the shading’s gradient fill across the entire clipping region, 
   which may be time-consuming.
*/
   pop();
   return;
   }
   void job::pdfOperator_TStar() {
/*
   T*
      Move to the start of the next line. This operator has the same effect as the code

         0 -Tl Td

      where Tl denotes the current leading parameter in the text state. The negative of 
      Tl is used here because Tl is the text leading expressed as a positive number. 

      Going to the next line entails decreasing the y coordinate.
*/
//   graphicsStateStack.current() -> translateTextMatrix(0.0f,-graphicsStateStack.current() -> getTextLeading());

Beep(2000,200);
#if 0
   graphicsStateStack.current() -> startLine(0.0f,-graphicsStateStack.current() -> getTextLeading());
#endif
   return;
   }

   void job::pdfOperator_Tc() {
/*
   charSpace  Tc

   Set the character spacing, Tc, to charSpace, which shall be a number expressed in unscaled text space units. 
   Character spacing shall be used by the Tj, TJ, and ' operators. 

   Initial value: 0.
*/
   object *pCS = pop();
Beep(2000,200);
#if 0
   graphicsStateStack.current() -> setCharacterSpacing(pCS -> OBJECT_POINT_TYPE_VALUE);
#endif
   return;
   }

   void job::pdfOperator_Td() {
/*
      tx ty Td
         Move to the start of the next line, offset from the start of the current line by (tx, ty). 
         tx and ty shall denote numbers expressed in unscaled text space units. More precisely, 
         this operator shall perform these assignments:
*/
   object *pty = pop();
   object *ptx = pop();
Beep(2000,200);
#if 0
   graphicsStateStack.current() -> startLine(ptx -> OBJECT_POINT_TYPE_VALUE,pty -> OBJECT_POINT_TYPE_VALUE);
#endif
   return;
   }


   void job::pdfOperator_TD() {
/*
   tx ty    TD

   Move to the start of the next line, offset from the start of the current line by (tx, ty).
   As a side effect, this operator shall set the leading parameter in the text state.

   This operator shall have the same effect as this code:

      -ty TL
      tx ty Td
*/
   operatorDup();
   object *pTy = pop();
Beep(2000,200);
#if 0
   graphicsStateStack.current() -> setTextLeading(-pTy -> OBJECT_POINT_TYPE_VALUE);
#endif
   pdfOperator_Td();
   return;
   }

    void job::pdfOperator_Tf() {
/*
    font size  Tf

    Set the text font, Tf, to font and the text font size, Tfs, to size. font shall be the name of a 
    font resource in the Font subdictionary of the current resource dictionary; size shall be a 
    number representing a scale factor. There is no initial value for either font or size; they 
    shall be specified explicitly by using Tf before any text is shown.
*/
    object *pfSize = pop();

    operatorFindfont();

    font *pFont = reinterpret_cast<font *>(top());

    push(pfSize);

    operatorScalefont();

    operatorSetfont();

    return;
    }

   void job::pdfOperator_Th() {
/*
   scaling Th
*/
   object *pHS = pop();
Beep(2000,200);
#if 0
   graphicsStateStack.current() -> setHorizontalTextScaling(pHS -> OBJECT_POINT_TYPE_VALUE / 100.0f);
#endif
   return;
   }

   void job::pdfOperator_Tj() {
/*
   string Tj
      show
         Show text
*/
   operatorShow();
   return;
   }

   void job::pdfOperator_TJ() {
/*
   array TJ

   Show one or more text strings, allowing individual glyph positioning. Each element of array shall 
   be either a string or a number. If the element is a string, this operator shall show the string. 

   If it is a number, the operator shall adjust the text position by that amount; that is, it shall 
   translate the text matrix, Tm. The number shall be expressed in thousandths of a unit of text 
   space (see 9.4.4, "Text Space Details"). This amount shall be subtracted from the current horizontal 
   or vertical coordinate, depending on the writing mode. In the default coordinate system, a 
   positive adjustment has the effect of moving the next glyph painted either to the left or 
   down by the given amount. Figure 46 shows an example of the effect of passing offsets to TJ.
*/

   if ( 0 == operandStack.size() )
      return;
   
   array *pArray = reinterpret_cast<array *>(pop());

Beep(2000,200);
#if 0
   graphicsState *pCurrentState = graphicsStateStack.current();

   long count = pArray -> size();

   for ( long k = 0; k < count; k++ ) {

      object *pItem = pArray -> getElement(k);

      if ( object::number == pItem -> ObjectType() )
         pCurrentState -> translateTextMatrixTJ(pItem -> OBJECT_POINT_TYPE_VALUE,0.0f);

      else if ( object::string == pItem -> ValueType() )
         pCurrentState -> addText(pItem -> Contents());

   }
#endif
   return;
   }


   void job::pdfOperator_TL() {
/*
   leading  TL

   Set the text leading, Tl, to leading, which shall be a number expressed in unscaled 
   text space units. Text leading shall be used only by the T*, ', and " operators. 

   Initial value: 0.
*/
   object *pTL = pop();
Beep(2000,200);
#if 0
   graphicsStateStack.current() -> setTextLeading(pTL -> OBJECT_POINT_TYPE_VALUE);
#endif
   return;
   }

   void job::pdfOperator_Tm() {
/*
   a b c d e f  Tm

      Set the text matrix, Tm, and the text line matrix, Tlm:
      The operands shall all be numbers, and the initial value for Tm and Tlm shall be the 
      identity matrix, [1 0 0 1 0 0]. Although the operands specify a matrix, they shall 
      be passed to Tm as six separate numbers, not as an array.

      The matrix specified by the operands shall not be concatenated onto the current text matrix, 
      but shall replace it.
*/
   POINT_TYPE d[6];
   for ( long k = 5; k > -1; k-- )
      d[k] = pop() -> OBJECT_POINT_TYPE_VALUE;

Beep(2000,200);
#if 0
   graphicsStateStack.current() -> setTextMatrix(d);
#endif
   return;
   }

   void job::pdfOperator_Tr() {
/*
   render Tr

   Set the text rendering mode, Tmode, to render, which shall be an integer. 

   Initial value: 0.
*/
   object *pMode = pop();
Beep(2000,200);
#if 0
   graphicsStateStack.current() -> setTextRenderingMode(pMode -> IntValue());
#endif
   return;
   }

   void job::pdfOperator_Ts() {
/*
   rise Ts

   Set the text rise, Trise, to rise, which shall be a number expressed in unscaled text space units. 

   Initial value: 0.
*/
   object *pTR = pop();
Beep(2000,200);
#if 0
   graphicsStateStack.current() -> setTextRise(pTR -> OBJECT_POINT_TYPE_VALUE);
#endif
   return;
   }


   void job::pdfOperator_Tw() {
/*
   wordSpace Tw

   Set the word spacing, Tw, to wordSpace, which shall be a number expressed in unscaled text space units. 
   Word spacing shall be used by the Tj, TJ, and ' operators. 

   Initial value: 0.

*/
   object *pWS = pop();
Beep(2000,200);
#if 0
   graphicsStateStack.current() -> setWordSpacing(pWS -> OBJECT_POINT_TYPE_VALUE);
#endif
   return;
   }

   
   void job::pdfOperator_Tz() {
/*
   scale Tz

   Set the horizontal scaling, Th, to (scale ÷ 100). scale shall be a number specifying the percentage of 
   the normal width. 

   Initial value: 100 (normal width).
*/
   object *pTZ = pop();
Beep(2000,200);
#if 0
   graphicsStateStack.current() -> setHorizontalTextScaling(pTZ -> OBJECT_POINT_TYPE_VALUE / 100.0f);
#endif
   return;
   }

   void job::pdfOperator_v() {
/*
   x2 y2 x3 y3 v

   Append a cubic Bézier curve to the current path. The curve shall extend from the current point 
   to the point (x3, y3), using the current point and (x2, y2) as the Bézier control points 
   (see 8.5.2.2, "Cubic Bézier Curves"). The new current point shall be (x3, y3).
*/
   pop();
   pop();
   pop();
   pop();
   return;
   }

   void job::pdfOperator_w() {
/*
   lineWidth w
      Set the line width in the graphics state (see 8.4.3.2, "Line Width").
*/
   pop();
   return;
   }

   void job::pdfOperator_W() {
/*
   eoclip
      Set clipping path using even-odd rule
*/
   return;
   }


   void job::pdfOperator_WStar() {
/*
     — W*

      Modify the current clipping path by intersecting it with the current path, using the 
      even-odd rule to determine which regions lie inside the clipping path.
*/
   return;
   }


   void job::pdfOperator_y() {
/*
   x1 y1 x3 y3  y

   Append a cubic Bézier curve to the current path. The curve shall extend from the current point 
   to the point (x3, y3), using (x1, y1) and (x3, y3) as the Bézier control points 
   (see 8.5.2.2, "Cubic Bézier Curves"). The new current point shall be (x3, y3).
*/
   pop();
   pop();
   pop();
   pop();
   return;
   }