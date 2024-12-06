#include "job.h"

#include "pathParameters.h"

    void graphicsState::drawType3Glyph(BYTE bGlyph) {

    pJob -> push(fontStack.top());
    pJob -> operatorBegin();
    pJob -> push(pJob -> pBuildGlyphLiteral);
    pJob -> operatorDup();
    pJob -> operatorWhere();

    if ( pJob -> pFalseConstant == pJob -> pop() ) {
        pJob -> pop();
        pJob -> push(fontStack.top());
        pJob -> push(pJob -> pBuildCharLiteral);
    } else
        pJob -> operatorExch();

    pJob -> operatorGet();

    pJob -> operatorEnd();

    procedure *pBuildGlyph = reinterpret_cast<procedure *>(pJob -> pop());

    pJob -> push(fontStack.top());
    pJob -> push(pJob -> pEncodingArrayLiteral);
    pJob -> operatorGet();

/*
    When a PostScript program tries to show a glyph from a Type 3 font, and the
    glyph is not already present in the font cache, the PostScript interpreter:

    1. Uses the character code as an index into the current font’s Encoding array,
        obtaining the corresponding character name. (This step is omitted during a
        glyphshow operation.)

    2. Pushes the current font dictionary and the character name on the operand stack.

    3. Executes the font’s BuildGlyph procedure. BuildGlyph must remove these two
        objects from the operand stack and use this information to construct the 
        requested glyph. This typically involves determining the glyph description 
        needed, supplying glyph metric information, constructing the glyph, and painting it.

    All Type 3 fonts must include a character named .notdef. The BuildGlyph procedure 
    should be able to accept that character name regardless of whether such a character 
    is encoded in the Encoding array. If the BuildGlyph procedure is given a character 
    name it does not recognize, it can handle that condition by painting the glyph for 
    the .notdef character instead. 

*/

    // The BYTE bGlyph is the "index" into the Encoding array

    pJob -> push(new (pJob -> CurrentObjectHeap()) object(pJob,bGlyph));
    pJob -> operatorGet();

    object *pCharacterName = pJob -> pop();

    pJob -> push(fontStack.top());
    pJob -> push(pCharacterName);

    concat(fontStack.top() -> getMatrix());

    pathParametersStack.top() -> pToUserSpace -> tx(pathParametersStack.top() -> currentPointsPoint.x);
    pathParametersStack.top() -> pToUserSpace -> ty(pathParametersStack.top() -> currentPointsPoint.y);

    boolean oldDoRasterize = setDefaultToRasterize(true);

    savePath(true);

    pJob -> executeProcedure(pBuildGlyph);

    revertMatrix();

    fillpath(true);

    setDefaultToRasterize(oldDoRasterize);

    return;
    }


    void graphicsState::setCacheDevice() {

    object *pUpperRightY = pJob -> pop();
    object *pUpperRightX = pJob -> pop();
    object *pLowerLeftY = pJob -> pop();
    object *pLowerLeftX = pJob -> pop();
    object *pWidthY = pJob -> pop();
    object *pWidthX = pJob -> pop();

    fontStack.top() -> type3GlyphBoundingBox.lowerLeft.x = pLowerLeftX -> OBJECT_POINT_TYPE_VALUE;
    fontStack.top() -> type3GlyphBoundingBox.lowerLeft.y = pLowerLeftY -> OBJECT_POINT_TYPE_VALUE;
    fontStack.top() -> type3GlyphBoundingBox.upperRight.x = pUpperRightX -> OBJECT_POINT_TYPE_VALUE;
    fontStack.top() -> type3GlyphBoundingBox.upperRight.y = pUpperRightY -> OBJECT_POINT_TYPE_VALUE;
    fontStack.top() -> type3GlyphBoundingBox.advance.x = pWidthX -> OBJECT_POINT_TYPE_VALUE;
    fontStack.top() -> type3GlyphBoundingBox.advance.y = pWidthY -> OBJECT_POINT_TYPE_VALUE;

    return;
    }