
#include "job.h"

#include "PostScript objects\graphicsState.h"
#include "PostScript objects\font.h"

    void graphicsState::setFont(class font *pFont) {

    pCurrentFont = pFont;

    pCurrentFont -> pSfntsArray = NULL;

    pJob -> push(pCurrentFont);

    pJob -> operatorBegin();

    pJob -> push(pJob -> pSfntsLiteral);

    pJob -> operatorWhere();

    pJob -> operatorEnd();

    long countGlyphs = 0;

    if ( pJob -> pTrueConstant == pJob -> pop() ) {

        pJob -> push(pJob -> pSfntsLiteral);
        pJob -> operatorGet();

        pCurrentFont -> pSfntsArray = reinterpret_cast<class array *>(pJob -> pop());

        pJob -> push(pCurrentFont);
        pJob -> push(pJob -> pCharStringsLiteral);
        pJob -> operatorGet();
        pJob -> operatorLength();

        countGlyphs = pJob -> pop() -> IntValue();

    }

    pJob -> push(pCurrentFont);
    pJob -> push(pJob -> pFontTypeLiteral);
    pJob -> operatorGet();

    object *pFontType = pJob -> pop();

    switch ( pFontType -> IntValue() ) {
    case 3:
        pCurrentFont -> fontType = font::ftype3;
        break;
    case 42:
        pCurrentFont -> fontType = font::ftype42;
        break;
    }

    pCurrentFont -> load(countGlyphs);

    return;
    }


    font *graphicsState::findFont(char *pszFontName) {

    LOGFONT logFont{0};

    strcpy(logFont.lfFaceName,pszFontName);

    HFONT hFont = CreateFontIndirect(&logFont);

    if ( NULL == hFont )
        return NULL;

    HGDIOBJ oldFont = SelectFont(hdcSurface,hFont);

    DWORD cbFont = GetFontData(hdcSurface,0L,0L,NULL,0L);

    SelectFont(hdcSurface,oldFont);

    if ( GDI_ERROR == cbFont ) 
        return NULL;

    BYTE *pbFont = new BYTE[cbFont];

    DWORD rv = GetFontData(hdcSurface,0L,0L,pbFont,cbFont);

    pCurrentFont = new (pJob -> CurrentObjectHeap()) class font(pJob,pszFontName);

    pCurrentFont -> fontType = font::ftype42;

    pCurrentFont -> pSfntsArray = NULL;

    pCurrentFont -> type42Load(pbFont);

    return pCurrentFont;
    }