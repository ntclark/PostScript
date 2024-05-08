
#include "job.h"

#include "PostScript objects\font.h"
#include "PostScript objects\matrix.h"
#include "PostScript objects\binaryString.h"

class array *font::pStandardEncoding = NULL;

    font::font(job *pj,char *pszName) :
        dictionary(pj,pszName,DEFAULT_DICTIONARY_SIZE)
    {

    theObjectType = object::objectType::font;

    if ( pszName )
        strcpy(szFamily,pszName);
    else
        memset(szFamily,0,sizeof(szFamily));

    pJob -> addFont(this);

    return;
    }


    font::font(job *pj,dictionary *pDict) :
        dictionary(pj,pDict -> Name(),DEFAULT_DICTIONARY_SIZE)
    {
    theObjectType = object::objectType::font;
    dictionary *pTarget = static_cast<dictionary *>(this);
    pTarget -> copyFrom(pDict);
    return;
    }


    font::font(job *pj,PdfDictionary *pFontDict,float fontSize) :
        dictionary(pj,"")
    {
    theObjectType = object::objectType::font;
    return;
    }


    font::~font() {

    if ( ! ( NULL == pszCharSet ) )
        delete [] pszCharSet;

    if ( ! ( NULL == pFontData ) )
        delete [] pFontData;

    if ( ! ( NULL == pNameIndex ) )
        delete [] pNameIndex;

    if ( ! ( NULL == pTopDictIndex ) )
        delete [] pTopDictIndex;

    if ( ! ( NULL == pStringIndex ) )
        delete [] pStringIndex;

    if ( ! ( NULL == pGlobalSubroutineIndex ) )
        delete [] pGlobalSubroutineIndex;

    if ( ! ( NULL == pCharStringsIndex ) )
        delete [] pCharStringsIndex;

    return;
    }


    void font::load(long gc) {

    glyphCount = (long)gc;

    pSfntsTable = reinterpret_cast<class binaryString *>(pSfntsArray -> getElement(0)) -> getData();

    tableDirectory.load(pSfntsTable);

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


    return;
    }

    char *font::translateText(char *pszText) {
    return NULL;
    }