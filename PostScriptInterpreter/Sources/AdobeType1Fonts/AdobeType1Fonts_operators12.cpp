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

#include "AdobeType1Fonts/AdobeType1Fonts.h"

#include "PostScript objects/object.h"
#include "Stacks/objectStack.h"


    void AdobeType1Fonts::abs() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::add() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::and() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::callothersubr() {
/*
    callothersubr 
        arg1 . . . argn n othersubr# callothersubr (12 16) –

    is a mechanism used by Type 1 BuildChar to make calls on the
    PostScript interpreter. Arguments argn through arg1 are pushed
    onto the PostScript interpreter operand stack, and the PostScript
    language procedure in the othersubr# position in the OtherSubrs
    array in the Private dictionary (or a built-in function equivalent
    to this procedure) is executed. Note that the argument order will
    be reversed when pushed onto the PostScript interpreter operand
    stack. 

    After the arguments are pushed onto the PostScript interpreter 
    operand stack, the PostScript interpreter performs a begin
    operation on systemdict followed by a begin operation on the
    font dictionary prior to executing the OtherSubrs entry. When
    the OtherSubrs entry completes its execution, the PostScript
    interpreter performs two end operations prior to returning to
    Type 1 BuildChar charstring execution. Use pop commands to
    retrieve results from the PostScript operand stack back to the
    Type 1 BuildChar operand stack. 

    See Chapter 8, "Using Subroutines" for details on using callothersubr.
*/

    uint32_t subNumber = pOperandStack -> pop() -> IntValue();
    uint32_t countArgs = pOperandStack -> pop() -> IntValue();

    for ( uint32_t k = 0; k < countArgs; k++ )
        pJob -> push(pOperandStack -> pop());

    pJob -> push(pJob -> SystemDict());
    pJob -> execute("begin",NULL,NULL);
    pJob -> push(pType1Glyph -> pDictionary);
    pJob -> execute("begin",NULL,NULL);

    array *pSubsArray = reinterpret_cast<array *>(pType1Glyph -> pPrivateDictionary -> retrieve("OtherSubrs"));

    object *pSub = pSubsArray -> getElement(subNumber);

    if ( NULL == pSub || ! ( object::objectType::procedure == pSub -> ObjectType() ) ) {
        __debugbreak();
    }

    pJob -> executeProcedure(reinterpret_cast<procedure *>(pSub));

    pJob -> execute("end",NULL,NULL);
    pJob -> execute("end",NULL,NULL);

    return;
    }

    void AdobeType1Fonts::div() {

    object *pDenominator = pOperandStack -> pop();
    object *pNumerator = pOperandStack -> pop();

    FLOAT result = (FLOAT)pNumerator -> DoubleValue()/ (FLOAT)pDenominator -> DoubleValue();

    pOperandStack -> push(new (pJob -> CurrentObjectHeap()) object(pJob,result));

    return;
    }

    void AdobeType1Fonts::dotsection() {
/*
    dotsection 
        – dotsection (12 0)

    brackets an outline section for the dots in letters such as “i”,“ j”,
    and “!”. This is a hint command that indicates that a section of a
    charstring should be understood as describing such a feature,
    rather than as part of the main outline. For more details, see se
*/
    return;
    }

    void AdobeType1Fonts::drop() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::dup() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::eq() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::exch() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::flex() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::flex1() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::get() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::hflex() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::hflex1() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::hstem3() {
/*
    hstem3 
        y0 dy0 y1 dy1 y2 dy2 hstem3 (12 2)

    declares the vertical ranges of three horizontal stem zones
    between the y coordinates y0 and y0 + dy0, y1 and y1 + dy1, and
    between y2 and y2 + dy2, where y0, y1 and y2 are all relative to
    the y coordinate of the left sidebearing point. The hstem3 command 
    sorts these zones by the y values to obtain the lowest, middle and 
    highest zones, called ymin, ymid and ymax respectively. 
    The corresponding dy values are called dymin, dymid and dymax. 
    These stems and the counters between them will all be controlled. 
    These coordinates must obey certain restrictions:
        • dymin = dymax
        • The distance from ymin + dymin/2 to ymid + dymid/2 must
            equal the distance from ymid + dymid/2 to ymax + dymax/2. In
            other words, the distance from the center of the bottom stem
            to the center of the middle stem must be the same as the 
            distance from the center of the middle stem to the center of the
            top stem.

    If a charstring uses an hstem3 command in the hints for a character, 
    the charstring must not use hstem commands and it must
    use the same hstem3 command consistently if hint replacement
    is performed.

    The hstem3 command is especially suited for controlling the
    stems and counters of symbols with three horizontally oriented
    features with equal vertical widths and with equal white space
    between these features, such as the mathematical equivalence
    symbol or the division symbol.
*/

    pOperandStack -> pop();
    pOperandStack -> pop();
    pOperandStack -> pop();
    pOperandStack -> pop();
    pOperandStack -> pop();
    pOperandStack -> pop();

Beep(2000,200);
    return;
    }

    void AdobeType1Fonts::ifelse() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::index() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::load() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::mul() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::neg() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::not() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::or() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::pop() {
/*
        pop 
        – pop (12 17) number

    removes a number from the top of the PostScript interpreter 
    operand stack and pushes that number onto the Type 1 BuildChar
    operand stack. This command is used only to retrieve a result
    from an OtherSubrs procedure. For more details, see Chapter 8,
    “Using Subroutines.”
*/
    object *pObj = pJob -> pop();

    pOperandStack -> push(pObj);

    return;
    }

    void AdobeType1Fonts::put() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::random() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::roll() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::sbw() {
/*
    sbw 
        sbx sby wx wy sbw (12 7)

    sets the left sidebearing point to (sbx, sby) and sets the character
    width vector to (wx, wy) in character space. This command also
    sets the current point to (sbx, sby), but does not place the point
    in the character path. Use rmoveto for the first point in the path.
    The name sbw stands for sidebearing and width; the x and y 
    components of both the left sidebearing and width must be specified.
    If the y components of both the left sidebearing and the width are
    0, then the hsbw command should be used. Either sbw or hsbw
    must be used once as the first command in a character outline
    definition. It must be used only once.
*/
    FLOAT wy = pOperandStack -> pop() -> FloatValue();
    FLOAT wx = pOperandStack -> pop() -> FloatValue();
    FLOAT sby = pOperandStack -> pop() -> FloatValue();
    FLOAT sbx = pOperandStack -> pop() -> FloatValue();

    pType1Glyph -> leftSideBearing[0] = sbx;
    pType1Glyph -> leftSideBearing[1] = sby;
    pType1Glyph -> characterWidth[0] = wx;
    pType1Glyph -> characterWidth[1] = wy;

    //pIGraphicElements -> SetCurrentPoint(&pType1Glyph -> leftSideBearing[0],
    //                                        &pType1Glyph -> leftSideBearing[1]);

    return;
    }

    void AdobeType1Fonts::seac() {
/*
    seac 
        asb adx ady bchar achar seac (12 6)

    for standard encoding accented character, makes an 
    accented character from two other characters in its 
    font program. The asb argument is the x component of 
    the left sidebearing of the accent; this value must 
    be the same as the sidebearing value given in the hsbw 
    or sbw command in the accent’s own charstring. The
    origin of the accent is placed at (adx, ady) relative 
    to the origin of the base character. The bchar argument 
    is the character code of the base character, and the 
    achar argument is the character code of the accent character. 
    Both bchar and achar are codes that these characters are 
    assigned in the Adobe StandardEncoding vector, given in 
    an Appendix in the PostScript Language Reference Manual.

    Furthermore, the characters represented by achar and bchar must
    be in the same positions in the font’s encoding vector as 
    the positions they occupy in the Adobe StandardEncoding vector. If the
    name of both components of an accented character do not
    appear in the Adobe StandardEncoding vector, the accented 
    character cannot be built using the seac command.
    The FontBBox entry in the font dictionary must be large enough
    to accommodate both parts of the accented character. The sbw or
    hsbw command that begins the accented character must be the
    same as the corresponding command in the base character.
    Finally, seac is the last command in the charstring for the
    accented character because the accent and base characters’ 
    charstrings each already end with their own endchar commands.

    The use of this command saves space in a Type 1 font program,
    but its use is restricted to those characters whose parts are defined
    in the Adobe StandardEncoding vector. In situations where use of
    the seac command is not possible, use of Subrs subroutines is a
    more general means for creating accented characters.
*/

    pOperandStack -> pop();
    pOperandStack -> pop();
    pOperandStack -> pop();
    pOperandStack -> pop();
    pOperandStack -> pop();

Beep(2000,200);

    return;
    }

    void AdobeType1Fonts::setcurrentpoint() {
/*
    setcurrentpoint 
        x y setcurrentpoint (12 33)

    sets the current point in the Type 1 font format BuildChar to
    (x, y) in absolute character space coordinates without performing
    a charstring moveto command. This establishes the current point
    for a subsequent relative path building command. The
    setcurrentpoint command is used only in conjunction with
    results from OtherSubrs procedures.
*/
    FLOAT y = pOperandStack -> pop() -> FloatValue();
    FLOAT x = pOperandStack -> pop() -> FloatValue();
    pIGraphicElements -> SetCurrentPoint(&x,&y);
    return;
    }

    void AdobeType1Fonts::sqrt() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::store() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::sub() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::vstem3() {
/*
    vstem3 
        x0 dx0 x1 dx1 x2 dx2 vstem3 (12 1)

    declares the horizontal ranges of three vertical stem zones
    between the x coordinates x0 and x0 + dx0, x1 and x1 + dx1, and
    x2 and x2 + dx2, where x0, x1 and x2 are all relative to the 
    x coordinate of the left sidebearing point. The vstem3 command 
    sorts these zones by the x values to obtain the leftmost, 
    middle and rightmost zones, called xmin, xmid and xmax respectively. 
    The corresponding dx values are called dxmin, dxmid and dxmax.
    These stems and the counters between them will all be controlled. 
    These coordinates must obey certain restrictions described as follows:
        • dxmin = dxmax
        • The distance from xmin + dxmin/2 to xmid + dxmid/2 must
            equal the distance from xmid + dxmid/2 to xmax + dxmax/2. In
            other words, the distance from the center of the left stem to
            the center of the middle stem must be the same as the distance
            from the center of the middle stem to the center of the right
            stem.

    If a charstring uses a vstem3 command in the hints for a character, 
    the charstring must not use vstem commands and it must use
    the same vstem3 command consistently if hint replacement is
    performed.

    The vstem3 command is especially suited for controlling the
    stems and counters of characters such as a lower case “m.”
*/
    return;
    }

