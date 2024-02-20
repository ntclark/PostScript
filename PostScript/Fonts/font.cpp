
#include "Fonts\font.h"

#include "PostScript objects\matrix.h"

#include "job.h"

class array *font::pStandardEncoding = NULL;

    font::font(job *pj,char *pszName) :
        dictionary(pj,pszName,object::font)
    {

    if ( pszName )
        strcpy(szFamily,pszName);
    else
        memset(szFamily,0,sizeof(szFamily));

    pPrivateDictionary = new (pJob -> CurrentObjectHeap()) dictionary(pj,"private");
    pCharStrings = new (pJob -> CurrentObjectHeap()) dictionary(pj,"CharStrings");
    pEncoding = new (pJob -> CurrentObjectHeap()) class array(pj,"Encoding",256);

    insert("FID",pszName);
    //   insert("FontType",pFontType);
    insert("Encoding",pEncoding);
    //   insert("FontBBox",&fontBBox);
    insert("Private",pPrivateDictionary);
    insert("CharStrings",pCharStrings);

    #if 0
    fontBBox.insert(new object(0L));
    fontBBox.insert(new object(0L));
    fontBBox.insert(new object(0L));
    fontBBox.insert(new object(0L));
    #endif

    pJob -> addFont(this);

    return;
    }


    font::font(job *pj,PdfDictionary *pFontDict,float fontSize) : 
        dictionary(pj,"",object::font)
    {

    pPrivateDictionary = new (pJob -> CurrentObjectHeap()) dictionary(pj,"private");
    pCharStrings = new (pJob -> CurrentObjectHeap()) dictionary(pj,"CharStrings");
    pEncoding = new (pJob -> CurrentObjectHeap()) class array(pj,"Encoding",256);

    if ( NULL == pStandardEncoding ) {

        pStandardEncoding = new (pJob -> CurrentObjectHeap())class array(pJob,"StandardEncoding");

        HRSRC hrsrc = FindResource(hModule,MAKEINTRESOURCE(ID_GLYPH_LIST),"#256");

        HGLOBAL hResource = LoadResource(hModule,hrsrc);

        char szTemp[MAX_PATH];
        strcpy(szTemp,_tempnam(NULL,NULL));
        FILE *fGlyphList= fopen(szTemp,"wb");

        SIZE_T sizeData = SizeofResource(hModule,hrsrc);
        void *pData = LockResource(hResource);
        fwrite(pData,sizeData,1,fGlyphList);
        fclose(fGlyphList);

        fGlyphList = fopen(szTemp,"rt");

        char szInput[128];
        while ( fgets(szInput,128,fGlyphList) ) {
            char *p = strtok(szInput,";");
            char *pHex = &p[strlen(p) + 1];
            long index;
            sscanf(pHex,"%x",&index);
            pStandardEncoding -> put(index,new (pJob -> CurrentObjectHeap()) object(pJob,p));
        }

        fclose(fGlyphList);

        DeleteFile(szTemp);

    }

    PdfObject *pFontDescriptor = pFontDict -> Object("FontDescriptor");

    BYTE *pValue = NULL;
    PdfDictionary *pFontDescriptorDictionary = NULL;

    if ( ! pFontDescriptor ) {

        pValue = pFontDict -> Value("BaseFont");

    } else {

        pFontDescriptorDictionary = pFontDescriptor -> Dictionary();

        FontFile(pFontDescriptorDictionary);

        pValue = pFontDescriptorDictionary -> Value("FontName");
    }

    if ( pValue )
        strcpy(szFamily,(char *)(pValue + 1));     // discard '/'

    PdfObject *pFontEncoding = pFontDict -> Object("Encoding");

    PdfDictionary *pEncodingDictionary = NULL;

    if ( pFontEncoding )
        pEncodingDictionary = pFontEncoding -> Dictionary();

    if ( pEncodingDictionary ) {

        pValue = pEncodingDictionary -> Value("Differences");

        if ( pValue ) {

            pDifferencesArray = new (pJob -> CurrentObjectHeap()) class array(pJob,"Differences");//,(char *)pValue);

            char *p = (char *)pValue;

            p = strtok((char *)pValue,"[ /\0x0D\0x0D]");

            long startIndex = 0;

            while ( p ) {

            object *pItem = new (pJob -> CurrentObjectHeap()) object(pJob,p);

            if ( object::integer == pItem -> ValueType() )
                startIndex = pItem -> IntValue();
            else {
                pDifferencesArray -> put(startIndex,pItem);
                startIndex++;
            }

            p = strtok(NULL," /]");

            }

        }

    }

    pValue = pFontDict -> Value("Subtype");

    if ( pValue ) {

        if ( 0 == strcmp("/Type1",(char *)pValue) ) 
            fontType = font::ftype1;

        else if ( 0 == strcmp("/Type2",(char *)pValue) )
            fontType = font::ftype2;

    }

    pValue = pFontDict -> Value("FirstChar");
    if ( pValue )
        firstChar = atol((char *)pValue);

    pValue = pFontDict -> Value("LastChar");
    if ( pValue )
        lastChar = atol((char *)pValue);

#if 1

#else
   pValue = pFontDescriptorDictionary -> Value("FontBBox");

   if ( pValue )
      pFontBBox = new class array(pJob,"FontBBox",(char *)pValue);

   pValue = pFontDescriptorDictionary -> Value("MissingWidth");

   if ( pValue )
      missingWidth = (float)atof((char *)pValue);

   pValue = pFontDescriptorDictionary -> Value("CharSet");

   if ( pValue ) {
      long n = (DWORD)strlen((char *)pValue) + 1;
      pszCharSet = new char[n];
      memset(pszCharSet,0,n * sizeof(char));
      strcpy(pszCharSet,(char *)pValue);
   }

#endif

    pValue = pFontDict -> Value("Widths");

    if ( pValue )
        pWidths = new (pJob -> CurrentObjectHeap()) class array(pJob,"Widths",(char *)pValue);

    //if ( '\0' == szFamily[0] )
    //   return;

    LOGFONT logFont = {0};

    HFONT currentFont = (HFONT)GetCurrentObject(pJob -> GetDC(),OBJ_FONT);

    GetObject(currentFont,sizeof(LOGFONT),&logFont);

    if ( ! ( '\0' == szFamily[0] ))
        strcpy(logFont.lfFaceName,szFamily);

#if USE_ANISOTROPIC
    logFont.lfHeight = (long)fontSize;
#else
    logFont.lfHeight = -MulDiv((long)fontSize, GetDeviceCaps(pJob -> GetDC(), LOGPIXELSY), 72);
#endif

    DeleteObject(currentFont);

    HFONT newFont = CreateFontIndirect(&logFont);

    HGDIOBJ oldFont = SelectObject(pJob -> GetDC(),newFont);

    return;
    }


    font::~font() {

    pJob -> deleteObject(pEncoding);
    pJob -> deleteObject(pFontBBox);
    pJob -> deleteObject(pPrivateDictionary);
    pJob -> deleteObject(pCharStrings);

    pJob -> deleteObject(pWidths);
    pJob -> deleteObject(pDifferencesArray);
    pJob -> deleteObject(pCharsetArray);

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


    char *font::translateText(char *pszText) {

// 10-12-2022 This is where the text capture outlines seem to be sized incorrectly.
// removing the return, they seem to be too wide. noting that with monospaced fonts
// they seem to be mostly correct.
return pszText;

    static char szResult[2048];

    memset(szResult,0,sizeof(szResult));

    char *p = pszText;
    while ( *p ) {

        long index = (long)*p;
        char *pGlyphName = p;

        if ( pDifferencesArray && pDifferencesArray -> find(index - 1) )
            pGlyphName = pDifferencesArray -> get(index - 1) -> Contents();

        else if ( pStandardEncoding -> find(index - 1) )
            pGlyphName = pStandardEncoding -> get(index - 1) -> Contents();

        sprintf(szResult + strlen(szResult),"%s",pGlyphName);

        p++;
    }

    return szResult;
    }