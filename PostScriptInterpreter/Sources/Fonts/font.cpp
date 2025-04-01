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

#include "FontManager_i.c"

    IFontManager *font::pIFontManager = NULL;

    font::font(job *pj,char *pszName) :
        dictionary(pj,pszName,DEFAULT_DICTIONARY_SIZE)
    {

    theObjectType = object::objectType::font;

    pIFontManager -> LoadFont(pszName,(UINT_PTR)(void *)this,&pIFont);

    loadDictionary();
    return;
    }


    void font::loadDictionary() {

    if ( ! exists(pJob -> pFontTypeLiteral -> Name()) ) {
        char szType[16];
        int fontType;
        pIFont -> get_FontType(&fontType);
        sprintf_s<16>(szType,"%d",fontType);
        put(pJob -> pFontTypeLiteral -> Name(),szType);
    }

    if ( ! exists(pJob -> pFontNameLiteral -> Name()) )
        put(pJob -> pFontNameLiteral -> Name(),fontName());

    if ( ! exists(pJob -> pFontMatrixLiteral -> Name()) )
           put(pJob -> pFontMatrixLiteral -> Name(),new (pJob -> CurrentObjectHeap()) matrix(pJob));

    if ( ! exists(pJob -> pFontBoundingBoxLiteral -> Name()) ) {
        array *pArray = new (pJob -> CurrentObjectHeap()) class array(pJob,4);
        pArray -> putElement(0,0);
        pArray -> putElement(1,0);
        pArray -> putElement(2,0);
        pArray -> putElement(3,0);
        put(pJob -> pFontBoundingBoxLiteral -> Name(),pArray);
    }

    pCharStrings = NULL;

    if ( ! exists(pJob -> pCharStringsLiteral -> Name() ) ) {

        pCharStrings = new (pJob -> CurrentObjectHeap()) dictionary(pJob,pJob -> pCharStringsLiteral -> Name());
        put(pJob -> pCharStringsLiteral -> Name(),pCharStrings);
        for ( uint16_t charCode = 0; charCode < 256; charCode++ ) {
            char szCharCode[8];
            sprintf_s<8>(szCharCode,"%ld",charCode);
            uint16_t glyphId;
            pIFont -> get_GlyphIndex(charCode,&glyphId);
            pCharStrings -> put(szCharCode,new (pJob -> CurrentObjectHeap()) object(pJob,(long)glyphId));
        }

    } else

        pCharStrings = (dictionary *)retrieve(pJob -> pCharStringsLiteral -> Name());

    DWORD cb = 0;
    for ( long k = 0; k < pCharStrings -> size(); k++ ) {
        if ( object::objectType::number == pCharStrings -> retrieve(k) -> ObjectType() )
            cb += 8 + 2 + (DWORD)strlen(pCharStrings -> retrieveKey(k));
    }
    char *pszCharStrings = (char *)CoTaskMemAlloc(cb + 1);
    memset(pszCharStrings,0,(cb + 1) * sizeof(char));
    char *pszEnd = pszCharStrings + cb;
    cb = 0L;
    for ( long k = 0; k < pCharStrings -> size(); k++ ) {
        if ( object::objectType::number == pCharStrings -> retrieve(k) -> ObjectType() ) {
            cb += sprintf(pszCharStrings + cb,"%s;%08d",pCharStrings -> retrieveKey(k),pCharStrings -> retrieve(k) -> IntValue());
            cb++;
        }
    }

    pIFont -> SetCharStrings((UINT_PTR)pszCharStrings);

    CoTaskMemFree(pszCharStrings);

    pEncoding = NULL;

    if ( ! exists(pJob -> pEncodingArrayLiteral -> Name() ) ) {

        pEncoding = new (pJob -> CurrentObjectHeap()) class array(pJob,pJob -> pEncodingArrayLiteral -> Name(),256);
        put(pJob -> pEncodingArrayLiteral -> Name(),pEncoding);

        for ( long k = 0; k < 256; k++ )
            pEncoding -> putElement(k,pJob -> pNotdefLiteral);

        dictionary *pCharStrings = reinterpret_cast<dictionary *>(retrieve(pJob -> pCharStringsLiteral -> Name()));

        for ( uint16_t charCode = 0; charCode < 256; charCode++ ) {
            char szCharCode[8];
            sprintf_s<8>(szCharCode,"%ld",charCode);
            pEncoding -> putElement(charCode,pCharStrings -> retrieveKey(szCharCode));
        }

    } else

        pEncoding = (array *)retrieve(pJob -> pEncodingArrayLiteral -> Name());

    cb = 0;
    for ( long k = 0; k < pEncoding -> size(); k++ )
        cb += 4 + 2 + (DWORD)strlen(pEncoding -> getElement(k) -> Contents());
    char *pszEncoding = (char *)CoTaskMemAlloc(cb + 1);
    memset(pszEncoding,0,(cb + 1) * sizeof(char));
    pszEnd = pszEncoding + cb;
    cb = 0L;
    for ( long k = 0; k < pEncoding -> size(); k++ ) {
        cb += sprintf(pszEncoding + cb,"%04ld;%s",k,pEncoding -> getElement(k) -> Contents());
        cb++;
    }

    pIFont -> SetEncoding((UINT_PTR)pszEncoding);

    CoTaskMemFree(pszEncoding);

    return;
    }


    font::font(job *pj,dictionary *pDict,char *pszFontName) :
        font(pj,pszFontName) 
    {
    static_cast<dictionary *>(this) -> copyFrom(pDict);
    return;
    }


    font::font(font &rhs) : dictionary(rhs) {
    isCIDFont = rhs.isCIDFont;
    dupCount = rhs.dupCount + 1;
    static_cast<dictionary *>(this) -> copyFrom(static_cast<dictionary *>(&rhs));
    pIFontManager -> DuplicateFont(rhs.pIFont,(UINT_PTR)(void *)this,&pIFont);
    loadDictionary();
    return;
    }


    font::~font() {
    return;
    }


    void font::initialize() {
    if ( NULL == pIFontManager )
        CoCreateInstance(CLSID_FontManager,NULL,CLSCTX_ALL,IID_IFontManager,reinterpret_cast<void **>(&pIFontManager));
    return;
    }


    font *font::findFont(job *pJob,char *pszFamilyName) {

    IFont_EVNSW *pIFont;
    if ( S_OK == pIFontManager -> SeekFont(pszFamilyName,-1L,&pIFont) ) {
        UINT_PTR fontCookie;
        pIFont -> get_FontCookie(&fontCookie);
        return (font *)fontCookie;
    }

    return new (pJob -> CurrentObjectHeap()) font(pJob,pszFamilyName);
    }


    void font::setFont(font *pFont) {
    pIFontManager -> put_CurrentFont(pFont -> pIFont);
    return;
    }


    font *font::CurrentFont() {
    IFont_EVNSW *pIFont = NULL;
    HRESULT hr = pIFontManager -> get_CurrentFont(&pIFont);
    if ( ! ( S_OK == hr ) )
        return NULL;
    UINT_PTR ptrCookie;
    pIFont -> get_FontCookie(&ptrCookie);
    return (font *)(void *)ptrCookie;
    }


    char *font::fontName() {
    static char szFontName[128];
    pIFont -> FontName(128,szFontName);
    return szFontName;
    }


    font *font::makeFont(array *pArray,font *pCopyFrom) {
    XFORM xForm{pArray -> getElement(A) -> OBJECT_POINT_TYPE_VALUE,pArray -> getElement(B) -> OBJECT_POINT_TYPE_VALUE,
                    pArray -> getElement(C) -> OBJECT_POINT_TYPE_VALUE,pArray -> getElement(D) -> OBJECT_POINT_TYPE_VALUE,
                    pArray -> getElement(TX) -> OBJECT_POINT_TYPE_VALUE,pArray -> getElement(TY) -> OBJECT_POINT_TYPE_VALUE};
    return makeFont(&xForm,pCopyFrom);
    }


    font *font::makeFont(matrix *pMatrix,font *pCopyFrom) {
    XFORM xForm{pMatrix -> a(),pMatrix -> b(),pMatrix -> c(), pMatrix -> d(),pMatrix -> tx(),pMatrix -> ty()};
    return makeFont(&xForm,pCopyFrom);
    }


    font *font::makeFont(XFORM *pXForm,font *pCopyFrom) {
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

    font *pFont = new (pCopyFrom -> Job() -> CurrentObjectHeap()) font(*pCopyFrom);
    pFont -> setFontMatrix(pXForm);
    return pFont;
    }


    font *font::scaleFont(FLOAT scaleFactor,font *pFont) {
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

    pFont -> pIFont -> Scale(scaleFactor,scaleFactor);
    return pFont;
    }