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

    void AdobeType1Fonts::invalidCommand() {
Beep(2000,200);
__debugbreak();
    return;
    }

    void AdobeType1Fonts::type2Command() {
Beep(2000,200);
__debugbreak();
    }

    void AdobeType1Fonts::blend() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::callgsubr() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::callsubr() {
/*
    callsubr 
        subr# callsubr (10) –

    calls a charstring subroutine with index subr# from the Subrs
    array in the Private dictionary. Each element of the Subrs array is
    a charstring encoded and encrypted like any other charstring.

    Arguments pushed on the Type 1 BuildChar operand stack prior
    to calling the subroutine, and results pushed on this stack by the
    subroutine, act according to the manner in which the subroutine
    is coded. These subroutines are generally used to encode
    sequences of path commands that are repeated throughout the
    font program, for example, serif outline sequences. Subroutine
    calls may be nested 10 deep. See Chapter 8, "Using Subroutines,"
    for other uses for subroutines, such as changing hints.
*/

    uint32_t subNumber = pOperandStack -> pop() -> IntValue();

    array *pSubsArray = reinterpret_cast<array *>(pType1Glyph -> pPrivateDictionary -> retrieve("Subrs"));

    object *pSub = pSubsArray -> getElement(subNumber);

    if ( ! ( object::valueType::binaryString == pSub -> ValueType() ) ) {
__debugbreak();
    }

    binaryString *pBinaryString = reinterpret_cast<binaryString *>(pSub);

    std::list<type1Token *> tokens;

    AdobeType1Fonts::decryptType1CharString(pBinaryString -> getData(),(uint32_t)pBinaryString -> length(),pType1Glyph -> countRandomBytes,&tokens);

    processTokens(&tokens);

    tokens.clear();

    return;
    }

    void AdobeType1Fonts::closepath() {
/*
    closepath 
        – closepath (9)

    closepath closes a subpath. Adobe strongly recommends that all
    character subpaths end with a closepath command, otherwise
    when an outline is stroked (by setting PaintType equal to 2) you
    may get unexpected behavior where lines join. Note that, unlike
    the closepath command in the PostScript language, this command does 
    not reposition the current point. Any subsequent rmoveto must be 
    relative to the current point in force before the Type 1 font 
    format closepath command was given. 

    Make sure that any subpath section formed by the closepath
    command intended to be zero length, is zero length. If not, the
    closepath command may cause a “spike” or “hangnail” (if the
    subpath doubles back onto itself) with unexpected results.
*/
    pIGraphicElements -> ClosePath();
    return;
    }

    void AdobeType1Fonts::cntrmask() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::endchar() {
/*
    endchar 
        – endchar (14)

    finishes a charstring outline definition and must be the 
    last command in a character’s outline (except for accented 
    characters defined using seac). When endchar is executed, 
    Type 1 BuildChar performs several tasks. It executes a 
    setcachedevice operation, using a bounding box it computes 
    directly from the character outline and using the width 
    information acquired from a previous hsbw or sbw operation. 
    (Note that this is not the same order of events as in Type 3 Fonts.) 
    BuildChar then calls a special version of fill or stroke depending 
    on the value of PaintType in the font dictionary. The Type 1 font 
    format supports only PaintType 0 (fill) and 2 (outline). 

    Note that this single fill or stroke implies that there can be 
    only one path (possibly containing several subpaths) that can 
    be created to be filled or stroked by the endchar command.
*/

    switch ( pType1Glyph -> thePaintType ) {
    case type1Glyph::paintType::fill:
        pIGraphicElements -> FillPath();
        break;
    case type1Glyph::paintType::stroke:
        pIGraphicElements -> StrokePath();
        break;
    }

    FLOAT x = pType1Glyph -> leftSideBearing[0] + pType1Glyph -> characterWidth[0];
    FLOAT y = pType1Glyph -> leftSideBearing[1];

    pIGraphicElements -> SetCurrentPoint(&x,&y);

    return;
    }

    void AdobeType1Fonts::error() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::escape() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::hhcurveto() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::hintmask() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::hlineto() {
/*
    hlineto 
        dx hlineto (6)

    for horizontal lineto. Equivalent to dx 0 rlineto
*/

    pIGraphicElements -> LineToRelative(pOperandStack -> pop() -> FloatValue(),0.0f);
    return;
    }

    void AdobeType1Fonts::hmoveto() {
/*
    hmoveto 
        dx hmoveto (22)

    for horizontal moveto. Equivalent to dx 0 rmoveto.
*/

    pOperandStack -> push(pZeroValue);

    rmoveto();
    
    return;
    }

    void AdobeType1Fonts::hsbw() {
/*
    hsbw 
        sbx wx hsbw (13)

    sets the left sidebearing point at (sbx, 0) and sets the character
    width vector to (wx, 0) in character space. This command also
    sets the current point to (sbx, 0), but does not place the point in
    the character path. Use rmoveto for the first point in the path.

    The name hsbw stands for horizontal sidebearing and width; horizontal 
    indicates that the y component of both the sidebearing
    and width is 0. Either sbw or hsbw must be used once as the first
    command in a character outline definition. It must be used only
    once. In non-marking characters, such as the space character, the
    left sidebearing point should be (0, 0).

*/
    object *pWX = pOperandStack -> pop();
    object *pSBX = pOperandStack -> pop();

    pType1Glyph -> leftSideBearing[0] = pSBX -> FloatValue();
    pType1Glyph -> leftSideBearing[1] = 0.0f;
    pType1Glyph -> characterWidth[0] = pWX -> FloatValue();
    pType1Glyph -> characterWidth[1] = 0.0f;

    //pIGraphicElements -> SetCurrentPoint(&pType1Glyph -> leftSideBearing[0],
    //                                        &pType1Glyph -> leftSideBearing[1]);

    return;
    }

    void AdobeType1Fonts::hstem() {
/*
    hstem 
        y dy hstem (1)

    declares the vertical range of a horizontal stem zone 
    (see the following section for more information about horizontal stem
    hints) between the y coordinates y and y + dy, where y is relative to
    the y coordinate of the left sidebearing point. Horizontal stem
    zones within a set of stem hints for a single character may not
    overlap other horizontal stem zones. Use hint replacement to
    avoid stem hint overlaps. For more details on hint replacement,
    see section 8.1, “Changing Hints Within a Character,” in Chapter
    8, “Using Subroutines.”
*/

    object *pDy = pOperandStack -> pop();
    object *pY = pOperandStack -> pop();

    return;
    }

    void AdobeType1Fonts::hstemhm() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::hvcurveto() {
/*
    hvcurveto 
        dx1 dx2 dy2 dy3 hvcurveto (31)

    for horizontal-vertical curveto. Equivalent to dx1 0 dx2 dy2 0 dy3
    rrcurveto. This command eliminates two arguments from an
    rrcurveto call when the first Bézie
*/
    FLOAT dy3 = pOperandStack -> pop() -> FloatValue();
    FLOAT dy2 = pOperandStack -> pop() -> FloatValue();
    FLOAT dx2 = pOperandStack -> pop() -> FloatValue();
    FLOAT dx1 = pOperandStack -> pop() -> FloatValue();

    curveto(dx1,0.0f,dx2,dy2,0.0f,dy3);
    return;
    }

    void AdobeType1Fonts::rcurveline() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::operatorReturn() {
/*
    return 
        – return (11) –

    returns from a Subrs array charstring subroutine (that had been
    called with a callsubr command) and continues execution in the
    calling charstring
*/

    return;
    }

    void AdobeType1Fonts::rlinecurve() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::rlineto() {
/*
    rlineto 
        dx dy rlineto (5)

    behaves like rlineto in the PostScript language.
*/
    object *pDy = pOperandStack -> pop();
    object *pDx = pOperandStack -> pop();

    pIGraphicElements -> LineToRelative(pDx -> FloatValue(),pDy -> FloatValue());

    return;
    }

    void AdobeType1Fonts::rmoveto() {
/*
    rmoveto 
        dx dy rmoveto (21)

    behaves like rmoveto in the PostScript language

*/
    object *pDy = pOperandStack -> pop();
    object *pDx = pOperandStack -> pop();

/*
    NOTE: From hsbw and sbw documentation:

        "Use rmoveto for the first point in the path."
        which contradicts "behaves like rmoveto ...."
*/

    if ( pType1Glyph -> firstRMove ) {
        pIGraphicElements -> NewPath();
        pIGraphicElements -> MoveTo(pDx -> FloatValue(),pDy -> FloatValue());
    } else
        pIGraphicElements -> MoveToRelative(pDx -> FloatValue(),pDy -> FloatValue());

    pType1Glyph -> firstRMove = false;
    return;
    }

    void AdobeType1Fonts::rrcurveto() {
/*
    rrcurveto 
        dx1 dy1 dx2 dy2 dx3 dy3 rrcurveto (8)

    for relative rcurveto. Whereas the arguments to the rcurveto
    operator in the PostScript language are all relative to the current
    point, the arguments to rrcurveto are relative to each other.
    Equivalent to dx1 dy1 (dx1 + dx2) (dy1 + dy2) (dx1 + dx2 + dx3) 
    (dy1 + dy2 + dy3) rcurveto.

*/

    FLOAT dy3 = pOperandStack -> pop() -> FloatValue();
    FLOAT dx3 = pOperandStack -> pop() -> FloatValue();
    FLOAT dy2 = pOperandStack -> pop() -> FloatValue();
    FLOAT dx2 = pOperandStack -> pop() -> FloatValue();
    FLOAT dy1 = pOperandStack -> pop() -> FloatValue();
    FLOAT dx1 = pOperandStack -> pop() -> FloatValue();

    curveto(dx1,dy1,dx2,dy2,dx3,dy3);
    return;
    }

    void AdobeType1Fonts::vhcurveto() {
/*
    vhcurveto 
        dy1 dx2 dy2 dx3 vhcurveto (30)

    for vertical-horizontal curveto. Equivalent to 0 dy1 dx2 dy2 dx3 0
    rrcurveto. This command eliminates two arguments from an
    rrcurveto call when the first Bézier t
*/
    FLOAT dx3 = pOperandStack -> pop() -> FloatValue();
    FLOAT dy2 = pOperandStack -> pop() -> FloatValue();
    FLOAT dx2 = pOperandStack -> pop() -> FloatValue();
    FLOAT dy1 = pOperandStack -> pop() -> FloatValue();

    curveto(0.0f,dy1,dx2,dy2,dx3,0.0f);
    return;
    }

    void AdobeType1Fonts::vlineto() {
/*
    vlineto 
        dy vlineto (7)

    for vertical lineto. Equivalent to 0 dy rlineto.
*/
    object *pDy = pOperandStack -> pop();

    pIGraphicElements -> LineToRelative(0.0f,pDy -> FloatValue());

    return;
    }

    void AdobeType1Fonts::vmoveto() {
/*
    vmoveto 
        dy vmoveto (4)

    for vertical moveto. This is equivalent to 0 dy rmoveto
*/
    object *pDy = pOperandStack -> pop();

    pOperandStack -> push(pZeroValue);
    pOperandStack -> push(pDy);

    rmoveto();

    return;
    }

    void AdobeType1Fonts::vstem() {
/*
    vstem 
        x dx vstem (3)

    declares the horizontal range of a vertical stem zone 
    (see the following section for more information about 
    vertical stem hints) between the x coordinates x and x+dx, 
    where x is relative to the x coordinate of the left 
    sidebearing point. Vertical stem zones within a set of stem 
    hints for a single character may not overlap other vertical 
    stem zones. Use hint replacement to avoid stem hint overlap. 

    For more details on hint replacement, see section
    8.1, “Changing Hints Within a Character,” in Chapter 8, “Using
    Subroutines.”

*/
    object *pDx = pOperandStack -> pop();
    object *pX = pOperandStack -> pop();

    return;
    }

    void AdobeType1Fonts::vstemhm() {
    type2Command();
    return;
    }

    void AdobeType1Fonts::vvcurveto() {
    type2Command();
    return;
    }


    void AdobeType1Fonts::curveto(FLOAT dx1,FLOAT dy1,FLOAT dx2,FLOAT dy2,FLOAT dx3,FLOAT dy3) {
/*
    rrcurveto 
        dx1 dy1 dx2 dy2 dx3 dy3 rrcurveto (8)

    for relative rcurveto. Whereas the arguments to the rcurveto
    operator in the PostScript language are all relative to the current
    point, the arguments to rrcurveto are relative to each other.
    Equivalent to dx1 dy1 (dx1 + dx2) (dy1 + dy2) (dx1 + dx2 + dx3) 
    (dy1 + dy2 + dy3) rcurveto.

*/
    FLOAT dx0;
    FLOAT dy0;
    pIGraphicElements -> GetCurrentPoint(&dx0,&dy0);

    pIGraphicElements -> CubicBezier(dx0,dy0,(dx0 + dx1),(dy0 + dy1),(dx0 + dx1 + dx2),(dy0 + dy1 + dy2),
                                        (dx0 + dx1 + dx2 + dx3),(dy0 + dy1 + dy2 + dy3));

    return;
    }
