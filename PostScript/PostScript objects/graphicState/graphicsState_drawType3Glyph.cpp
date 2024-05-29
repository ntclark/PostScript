
#include "PostScript objects/graphicsState.h"
#include "PostScript objects/font.h"


    void graphicsState::drawType3Glyph(BYTE bGlyph) {

    pJob -> push(pCurrentFont);
    pJob -> operatorBegin();
    pJob -> push(pJob -> pBuildGlyphLiteral);
    pJob -> operatorDup();
    pJob -> operatorWhere();

    if ( pJob -> pFalseConstant == pJob -> pop() ) {
        pJob -> pop();
        pJob -> push(pCurrentFont);
        pJob -> push(pJob -> pBuildCharLiteral);
    } else
        pJob -> operatorExch();

    pJob -> operatorGet();

    pJob -> operatorEnd();

    procedure *pBuildGlyph = reinterpret_cast<procedure *>(pJob -> pop());

    pJob -> push(pCurrentFont);
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

    pJob -> push(pCurrentFont);
    pJob -> push(pCharacterName);

#if 1

    concat(pCurrentFont -> getMatrix());

    pToUserSpace -> tx(lastPointsPoint.x);
    pToUserSpace -> ty(lastPointsPoint.y);

    pJob -> executeProcedure(pBuildGlyph);

    revertMatrix();

#else

    pJob -> executeProcedure(pBuildGlyph);

BitBlt(hdcSurface,128,128,GLYPH_BMP_PIXELS,GLYPH_BMP_PIXELS,hdcAlternate,0,0,SRCCOPY);
BitBlt(GetDC(HWND_DESKTOP),0,0,GLYPH_BMP_PIXELS,GLYPH_BMP_PIXELS,hdcAlternate,0,0,SRCCOPY);

DeleteObject(hbmGlyph);
hbmGlyph = NULL;
DeleteDC(hdcAlternate);
hdcAlternate = NULL;
#endif

    return;
    }


    void graphicsState::setCacheDevice() {

    object *pUpperRightY = pJob -> pop();
    object *pUpperRightX = pJob -> pop();
    object *pLowerLeftY = pJob -> pop();
    object *pLowerLeftX = pJob -> pop();
    object *pWidthY = pJob -> pop();
    object *pWidthX = pJob -> pop();

    pCurrentFont -> type3GlyphBoundingBox.lowerLeft.x = pLowerLeftX -> OBJECT_POINT_TYPE_VALUE;
    pCurrentFont -> type3GlyphBoundingBox.lowerLeft.y = pLowerLeftY -> OBJECT_POINT_TYPE_VALUE;
    pCurrentFont -> type3GlyphBoundingBox.upperRight.x = pUpperRightX -> OBJECT_POINT_TYPE_VALUE;
    pCurrentFont -> type3GlyphBoundingBox.upperRight.y = pUpperRightY -> OBJECT_POINT_TYPE_VALUE;
    pCurrentFont -> type3GlyphBoundingBox.advance.x = pWidthX -> OBJECT_POINT_TYPE_VALUE;
    pCurrentFont -> type3GlyphBoundingBox.advance.y = pWidthY -> OBJECT_POINT_TYPE_VALUE;

#if 0

    if ( ! ( NULL == hdcAlternate ) )
        DeleteDC(hdcAlternate);

    hdcAlternate = CreateCompatibleDC(NULL);

RECT rc{0,0.0,0};
rc.right = GLYPH_BMP_PIXELS;
rc.bottom = GLYPH_BMP_PIXELS;

    hbmGlyph = CreateBitmap(GLYPH_BMP_PIXELS,GLYPH_BMP_PIXELS,1,GetDeviceCaps(hdcAlternate,BITSPIXEL),NULL);
    SelectObject(hdcAlternate,hbmGlyph);

FillRect(hdcAlternate,&rc,CreateSolidBrush(RGB(255,255,255)));

    HPEN hPen = CreatePen(PS_SOLID,0,RGB(0,0,0));
    SelectObject(hdcAlternate,hPen);
    HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
    SelectObject(hdcAlternate,hBrush);

    glyphSpaceDomain.x = (long)(pCurrentFont -> type3GlyphBoundingBox.upperRight.x - pCurrentFont -> type3GlyphBoundingBox.lowerLeft.x);
    glyphSpaceDomain.y = (long)(pCurrentFont -> type3GlyphBoundingBox.upperRight.y - pCurrentFont -> type3GlyphBoundingBox.lowerLeft.y);

    scaleGlyphSpacetoPixels = GLYPH_BMP_PIXELS / (float)glyphSpaceDomain.x;

    SelectObject(hdcAlternate,hbmGlyph);

//MoveToEx(hdcAlternate,2,2,NULL);
//LineTo(hdcAlternate,(cx - 2),(cy - 2));

//BitBlt(GetDC(HWND_DESKTOP),0,0,GLYPH_BMP_PIXELS,GLYPH_BMP_PIXELS,hdcAlternate,0,0,SRCCOPY);
//BitBlt(hdcSurface,100,100,GLYPH_BMP_PIXELS,GLYPH_BMP_PIXELS,hdcAlternate,0,0,SRCCOPY);
#endif

    return;
    }