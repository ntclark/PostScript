
#include "job.h"

#include "PostScript objects/font.h"
#include "PostScript objects/matrix.h"
#include "PostScript objects/binaryString.h"

    font::font(job *pj,char *pszName) :
        dictionary(pj,pszName,DEFAULT_DICTIONARY_SIZE)
    {

    theObjectType = object::objectType::font;

    if ( pszName )
        strcpy(szFamily,pszName);
    else
        memset(szFamily,0,sizeof(szFamily));

    class matrix *pFontMatrix = new (pJob -> CurrentObjectHeap()) class matrix(pJob);

    putMatrix(pFontMatrix);

    class matrix *pRestoreMatrix = getMatrix(pJob -> pRestoreFontMatrixLiteral);

    if ( NULL == pRestoreMatrix ) {
        pRestoreMatrix = new (pJob -> CurrentObjectHeap()) class matrix(pJob);
        pRestoreMatrix -> copyFrom(getMatrix());
    }

    putMatrix(pRestoreMatrix,pJob -> pRestoreFontMatrixLiteral);

    return;
    }

/*
    This font constructor is the one used by executing PostScript code.
    Probably from operator makefont or definefont when the font 
    is not in the FontDirectory by name.
*/
    font::font(job *pj,dictionary *pDict,char *pszFontName) :
        dictionary(pj,pDict -> Name(),DEFAULT_DICTIONARY_SIZE)
    {

    theObjectType = object::objectType::font;

    static_cast<dictionary *>(this) -> copyFrom(pDict);
    object *pName = retrieve("FontName");
    if ( NULL == pName ) 
        put("FontName",pszFontName);

    class matrix *pRestoreMatrix = getMatrix(pJob -> pRestoreFontMatrixLiteral);

    if ( NULL == pRestoreMatrix ) {
        pRestoreMatrix = new (pJob -> CurrentObjectHeap()) class matrix(pJob);
        pRestoreMatrix -> copyFrom(getMatrix());
    }

    putMatrix(pRestoreMatrix,pJob -> pRestoreFontMatrixLiteral);

    return;
    }


    font::font(job *pj,font *pCopyFrom) :
        dictionary(pj,pCopyFrom -> Name())
    {

    theObjectType = object::objectType::font;

    memcpy(&fontDataBegin,&pCopyFrom -> fontDataBegin,offsetof(font,fontDataEnd) - offsetof(font,fontDataBegin));

    static_cast<dictionary *>(this) -> copyFrom(reinterpret_cast<dictionary *>(pCopyFrom));

    putMatrix(/**/new (pJob -> CurrentObjectHeap()) class matrix(pJob));//*/pCopyFrom -> getMatrix());

    for ( std::pair<uint16_t,uint16_t> p : pCopyFrom -> glyphIDMap )
        glyphIDMap[p.first] = p.second;

    return;
    }


    font::~font() {
    return;
    }


    font *font::findFont(job *pJob,char *pszFamilyname) {

    LOGFONT logFont{0};

    strcpy(logFont.lfFaceName,pszFamilyname);

    if ( strchr(logFont.lfFaceName,'-') ) {
        char *p = strchr(logFont.lfFaceName,'-') + 1;
        if ( 0 == _stricmp("bold",p) ) {
            *(p - 1) = '\0';
        }
    }

    if ( 0 == _stricmp(logFont.lfFaceName,"Courier") ) {
        strcpy(logFont.lfFaceName,"Courier New");
    }

    HFONT hFont = CreateFontIndirect(&logFont);

    if ( NULL == hFont )
        return NULL;

    HDC hdcTemp = CreateCompatibleDC(NULL);

    HGDIOBJ oldFont = SelectFont(hdcTemp,hFont);

    DWORD cbFont = GetFontData(hdcTemp,0L,0L,NULL,0L);

    if ( GDI_ERROR == cbFont ) {
        SelectFont(hdcTemp,oldFont);
        DeleteDC(hdcTemp);
        return NULL;
    }

    font *pFont = new (pJob -> CurrentObjectHeap()) class font(pJob,pszFamilyname);

    pFont -> pbFontData = new BYTE[cbFont];

    DWORD rv = GetFontData(hdcTemp,0L,0L,pFont -> pbFontData,cbFont);

    SelectFont(hdcTemp,oldFont);

    DeleteDC(hdcTemp);

    pFont -> fontType = font::ftype42;

    pFont -> pSfntsArray = NULL;

    pFont -> type42Load(pFont -> pbFontData);

    return pFont;
    }


    void font::load(long countGlyphs) {

    if ( isLoaded )
        return;

    switch ( fontType ) {
    case ftype3:
        type3load();
        break;
    case ftype42:
        type42Load();
        break;
    }

    pJob -> push(this);
    pJob -> push(pJob -> pFontBoundingBoxLiteral);
    pJob -> operatorGet();

    pBoundingBoxArray = reinterpret_cast<class array *>(pJob -> pop());

    isLoaded = true;

    return;
    }


    void font::type3load() {
    return;
    }


    char *font::fontName() {
    object *pName = static_cast<dictionary *>(this) -> retrieve("FontName");
    if ( NULL == pName )
        return NULL;
    return pName -> Contents();
    }


    void font::gSave() {
    class matrix *pFontMatrix = getMatrix();
    class matrix *pRestoreFontMatrix = getMatrix(pJob -> pRestoreFontMatrixLiteral);
    pRestoreFontMatrix -> copyFrom(pFontMatrix);
    return;
    }


    void font::gRestore() {
    class matrix *pFontMatrix = getMatrix();
    class matrix *pRestoreFontMatrix = getMatrix(pJob -> pRestoreFontMatrixLiteral);
    pFontMatrix -> copyFrom(pRestoreFontMatrix);
    return;
    }