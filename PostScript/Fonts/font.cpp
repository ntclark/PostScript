
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


    font::font(job *pj,PdfDictionary *pFontDict,POINT_TYPE fontSize) :
        dictionary(pj,"")
    {
    theObjectType = object::objectType::font;
    return;
    }


    font::~font() {
    return;
    }


    void font::load(long countGlyphs) {

    glyphCount = (uint16_t)countGlyphs;

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

    return;
    }


    void font::type3load() {
    return;
    }


    void font::type42Load(BYTE *pbData) {

    if ( ! ( NULL == pSfntsArray ) ) {
        pSfntsTable = reinterpret_cast<class binaryString *>(pSfntsArray -> getElement(0)) -> getData();
        tableDirectory.load(pSfntsTable);
    } else
        tableDirectory.load(pbData);

    pHeadTable = new otHeadTable();

    pHeadTable -> load(tableDirectory.table("head"));

    pGlyfTable = tableDirectory.table("glyf");
    if ( NULL == pGlyfTable )
        pGlyfTable = tableDirectory.table("glyx");

    pLocaTable = tableDirectory.table("loca");
    if ( NULL == pLocaTable )
        pLocaTable = tableDirectory.table("locx");

    pHmtxTable = tableDirectory.table("hmtx");

    pVmtxTable = tableDirectory.table("vmtx");

    pHheaTable = tableDirectory.table("hhea");

    pHorizHeadTable = new otHorizHeadTable();
    pHorizHeadTable -> load(pHheaTable);

    pVheaTable = tableDirectory.table("vhea");
    if ( ! ( NULL == pVheaTable ) )
        pVertHeadTable -> load(pVheaTable);

    pHorizontalMetricsTable = new otHorizontalMetricsTable(glyphCount,pHorizHeadTable -> numberOfHMetrics,pHmtxTable);

    if ( ! ( NULL == pVheaTable ) )
        pVerticalMetricsTable = new otVerticalMetricsTable(glyphCount,pHorizHeadTable -> numberOfHMetrics,pVmtxTable);

    if ( ! ( NULL == tableDirectory.table("os2") ) )
        pOS2Table = new otOS2Table(tableDirectory.table("os2"));

    scaleFUnitsToPoints = pointSize / (float)pHeadTable -> unitsPerEm;

    if ( ! ( NULL == pOS2Table ) )
        scaleFUnitsToPoints = scaleFUnitsToPoints *  (float)pHeadTable -> unitsPerEm / (float)(pOS2Table -> sTypoAscender - pOS2Table -> sTypoDescender);

    if ( ! exists(pJob -> pFontTypeLiteral -> Name()) )
        put(pJob -> pFontTypeLiteral -> Name(),"42");

    if ( ! exists(pJob -> pFontNameLiteral -> Name()) )
        put(pJob -> pFontNameLiteral -> Name(),szFamily);

    if ( ! exists(pJob -> pFontMatrixLiteral -> Name()) )
        put(pJob -> pFontMatrixLiteral -> Name(),new (pJob -> CurrentObjectHeap()) class matrix(pJob));

///FontMatrix [1 0 0 1 0 0] def
///FontBBox [0 0 0 0] def
    return;
    }


    char *font::fontName() {
    object *pName = static_cast<dictionary *>(this) -> retrieve("FontName");
    if ( NULL == pName )
        return NULL;
    return pName -> Contents();
    }


    void font::scalefont(POINT_TYPE scale) {
    class matrix *pFontMatrix = getMatrix();
    pFontMatrix -> scale(scale);
    pointSize = scale;
    return;
    }


    class matrix *font::getMatrix(object *pFontMatrixName) {
    pJob -> push(this);
    if ( NULL == pFontMatrixName ) 
        pJob -> push(pJob -> pFontMatrixLiteral);
    else
        pJob -> push(pFontMatrixName);
    pJob -> operatorKnown();
    if ( pJob -> pFalseConstant == pJob -> pop() ) 
        return NULL;
    pJob -> push(this);
    if ( NULL == pFontMatrixName ) 
        pJob -> push(pJob -> pFontMatrixLiteral);
    else
        pJob -> push(pFontMatrixName);
    pJob -> operatorGet();
    return reinterpret_cast<class matrix *>(pJob -> pop());
    }


    void font::putMatrix(class matrix *pFontMatrix,object *pFontMatrixName) {
    pJob -> push(this);
    if ( NULL == pFontMatrixName )
        pJob -> push(pJob -> pFontMatrixLiteral);
    else
        pJob -> push(pFontMatrixName);
    pJob -> push(pFontMatrix);
    pJob -> operatorPut();
    return;
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


    void font::setFontMatrix(class matrix *pMatrix) {
    class matrix *pFontMatrix = getMatrix();
    pFontMatrix -> concat(pMatrix);
    putMatrix(pFontMatrix);
    return;
    }


    void font::transformGlyph(POINTD *pPoints,long countPoints) {
    class matrix *pFontMatrix = getMatrix();
    for ( long k = 0; k < countPoints; k++ ) {
        POINT_TYPE x = pPoints[k].X();
        POINT_TYPE y = pPoints[k].Y();
        *pPoints[k].px = pFontMatrix -> a() * x + pFontMatrix -> b() * y + pFontMatrix -> tx();
        *pPoints[k].py = pFontMatrix -> c() * x + pFontMatrix -> d() * y + pFontMatrix -> ty();
    }
    return;
    }


    void font::transformGlyphInPlace(POINTD *pPoints,long countPoints) {
    class matrix *pFontMatrix = getMatrix();
    for ( long k = 0; k < countPoints; k++ ) {
        POINT_TYPE x = pPoints[k].X();
        POINT_TYPE y = pPoints[k].Y();
        *pPoints[k].px = pFontMatrix -> a() * x + pFontMatrix -> b() * y;
        *pPoints[k].py = pFontMatrix -> c() * x + pFontMatrix -> d() * y;
    }
    return;
    }


    void font::transformGlyph(POINTD *pPoints,long countPoints,class matrix *pSource) {
    for ( long k = 0; k < countPoints; k++ ) {
        POINT_TYPE x = pPoints[k].X();
        POINT_TYPE y = pPoints[k].Y();
        *pPoints[k].px = pSource -> a() * x + pSource -> b() * y + pSource -> tx();
        *pPoints[k].py = pSource -> c() * x + pSource -> d() * y + pSource -> ty();
    }
    return;
    }


    void font::transformGlyphInPlace(POINTD *pPoints,long countPoints,class matrix *pSource) {
    for ( long k = 0; k < countPoints; k++ ) {
        POINT_TYPE x = pPoints[k].X();
        POINT_TYPE y = pPoints[k].Y();
        *pPoints[k].px = pSource -> a() * x + pSource -> b() * y;
        *pPoints[k].py = pSource -> c() * x + pSource -> d() * y;
    }
    return;
    }