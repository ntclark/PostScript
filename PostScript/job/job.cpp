
#include "utilities.h"

#include "job.h"

#include "PostScript objects\matrix.h"
#include "PostScript objects\font.h"
#include "PostScript objects\literal.h"
#include "PostScript objects\graphicsState.h"

    PdfUtility *job::pPdfUtility = NULL;

    job::job(char *pszFileName,HWND hwndSurf,IPostScriptTakeText *pIPSTT) :

        pStorage(NULL),
        pEnd(NULL),

        pIPostScriptTakeText(pIPSTT),

        isGlobalVM(true),
        hasExited(false),

        saveCount(0L),
        inputLineNumber(1L),
        procedureDefinitionLevel(0L),

        hwndSurface(hwndSurf),

        graphicsStateStack(this)

    {
    if ( ! pPdfUtility )
        pPdfUtility = new PdfUtility();

    pLanguageLevel = new (CurrentObjectHeap()) object(this,2L);

    pUserDict = new (CurrentObjectHeap()) dictionary(this,"userdict");
    pSystemDict = new (CurrentObjectHeap()) dictionary(this,"systemdict");
    pGlobalDict = new (CurrentObjectHeap()) dictionary(this,"globaldict");
    pStatusDict = new (CurrentObjectHeap()) dictionary(this,"statusdict");
    pErrorDict = new (CurrentObjectHeap()) dictionary(this,"errordict");
    pPdfDict = new (CurrentObjectHeap()) dictionary(this,"pdfdict");

    pFontDirectory = new (CurrentObjectHeap()) dictionary(this,"font directory");

    pStandardEncoding = new (CurrentObjectHeap()) array(this,"StandardEncoding",256);
    pISOLatin1Encoding = new (CurrentObjectHeap()) array(this,"ISOLatin1Encoding",256);

    pDefaultMatrix = new (CurrentObjectHeap()) matrix(this);
    pCurrentMatrix = pDefaultMatrix;

    pDefaultColorSpace = new (CurrentObjectHeap()) colorSpace(this,"DeviceGray");
    pCurrentColorSpace = pDefaultColorSpace;

    if ( pszFileName ) {
        FILE *f = fopen(pszFileName,"rb");
        if ( f ) {
            fseek(f,0,SEEK_END);
            long fileSize = ftell(f);
            fseek(f,0,SEEK_SET);
            pStorage = new char[fileSize + 1];
            pEnd = pStorage + fileSize;
            *pEnd = '\0';
            fread(pStorage,fileSize,1,f);
            fclose(f);
            countPages = getPageCount(pszFileName);
        }
    }

    pStringType = new (CurrentObjectHeap()) name(this,"stringtype");
    pArrayType = new (CurrentObjectHeap()) name(this,"arraytype");
    pIntegerType = new (CurrentObjectHeap()) name(this,"integertype");
    pRealType = new (CurrentObjectHeap()) name(this,"realtype");
    pBooleanType = new (CurrentObjectHeap()) name(this,"booleantype");
    pDictType = new (CurrentObjectHeap()) name(this,"dicttype");
    pMarkType = new (CurrentObjectHeap()) name(this,"marktype");
    pNullType = new (CurrentObjectHeap()) name(this,"nulltype");
    pSaveType = new (CurrentObjectHeap()) name(this,"savetype");
    pFontType = new (CurrentObjectHeap()) name(this,"fonttype");
    pOperatorType = new (CurrentObjectHeap()) name(this,"operatortype");
    pNameType = new (CurrentObjectHeap()) name(this,"nametype");

    // Resource names
    new (CurrentObjectHeap()) name(this,"Font");

    nameTypeMap[object::atom][object::container] = pArrayType;
    nameTypeMap[object::atom][object::string] = pStringType;
    nameTypeMap[object::atom][object::character] = pStringType;
    nameTypeMap[object::atom][object::integer] = pIntegerType;
    nameTypeMap[object::atom][object::radix] = pIntegerType;
    nameTypeMap[object::atom][object::real] = pRealType;

    nameTypeMap[object::literal][object::string] = pStringType;
    nameTypeMap[object::procedure][object::string] = pStringType;
    nameTypeMap[object::dictionary][object::string] = pDictType;
    nameTypeMap[object::array][object::container] = pArrayType;
    nameTypeMap[object::number][object::integer] = pIntegerType;
    nameTypeMap[object::number][object::radix] = pIntegerType;
    nameTypeMap[object::number][object::real] = pRealType;
    nameTypeMap[object::logical][object::string] = pBooleanType;
    nameTypeMap[object::mark][object::valueTypeUnspecified] = pMarkType;
    nameTypeMap[object::null][object::valueTypeUnspecified] = pNullType;
    nameTypeMap[object::save][object::valueTypeUnspecified] = pSaveType;
    nameTypeMap[object::font][object::valueTypeUnspecified] = pFontType;
    nameTypeMap[object::directExecutable][object::valueTypeUnspecified] = pOperatorType;
    nameTypeMap[object::name][object::valueTypeUnspecified] = pNameType;

    tokenProcedures[std::hash<std::string>()(DSC_DELIMITER)] = &job::parseStructureSpec;
    tokenProcedures[std::hash<std::string>()(COMMENT_DELIMITER)] = &job::parseComment;
    tokenProcedures[std::hash<std::string>()(STRING_DELIMITER_BEGIN)] = &job::parseString;
    tokenProcedures[std::hash<std::string>()(HEX_STRING_DELIMITER_BEGIN)] = &job::parseHexString;
    tokenProcedures[std::hash<std::string>()(HEX85_DELIMITER_BEGIN)] = &job::parseHex85String;
    tokenProcedures[std::hash<std::string>()(LITERAL_DELIMITER)] = &job::parseLiteralName;

    antiDelimiters[std::hash<std::string>()(DSC_DELIMITER)] = "";
    antiDelimiters[std::hash<std::string>()(COMMENT_DELIMITER)] = "";
    antiDelimiters[std::hash<std::string>()(STRING_DELIMITER_BEGIN)] = STRING_DELIMITER_END;
    antiDelimiters[std::hash<std::string>()(HEX_STRING_DELIMITER_BEGIN)] = HEX_STRING_DELIMITER_END;
    antiDelimiters[std::hash<std::string>()(HEX85_DELIMITER_BEGIN)] = HEX85_DELIMITER_END;
    antiDelimiters[std::hash<std::string>()(LITERAL_DELIMITER)] = "";
    antiDelimiters[std::hash<std::string>()(ARRAY_DELIMITER_BEGIN)] = ARRAY_DELIMITER_END;

    pSystemDict -> put("languagelevel",pLanguageLevel);

    pSystemDict -> put(ARRAY_DELIMITER_BEGIN,&job::operatorMarkArrayBegin);
    pSystemDict -> put(ARRAY_DELIMITER_END,&job::operatorMarkArrayEnd);

    pSystemDict -> put(DICTIONARY_DELIMITER_BEGIN,&job::operatorMarkDictionaryBegin);
    pSystemDict -> put(DICTIONARY_DELIMITER_END,&job::operatorMarkDictionaryEnd);

    pSystemDict -> put(PROC_DELIMITER_BEGIN,&job::operatorMarkProcedureBegin);
    pSystemDict -> put(PROC_DELIMITER_END,&job::operatorMarkProcedureEnd);

#include "job_operators.cpp"

#include "job_pdfOperators.cpp"

    pSystemDict -> put("globaldict",pGlobalDict);
    pSystemDict -> put("systemdict",pSystemDict);
    pSystemDict -> put("statusdict",pStatusDict);
    pSystemDict -> put("userdict",pUserDict);
    pSystemDict -> put("pdfdict",pPdfDict);
    pSystemDict -> put("theErrordict",pErrorDict);

    pTrueConstant = new (CurrentObjectHeap()) booleanObject(this,"true");
    pFalseConstant = new (CurrentObjectHeap()) booleanObject(this,"false");
    pNullConstant = new (CurrentObjectHeap()) object(this,"null");
    pZeroConstant =  new (CurrentObjectHeap()) object(this,"0");
    pOneConstant =  new (CurrentObjectHeap()) object(this,"1");
    pEncodingLiteral = new (CurrentObjectHeap()) literal(this,"Encoding");
    pCharStringsLiteral = new (CurrentObjectHeap()) literal(this,"CharStrings");
    pGlyphDirectoryLiteral = new (CurrentObjectHeap()) literal(this,"GlyphDirectory");
    pSfntsLiteral = new (CurrentObjectHeap()) literal(this,"sfnts");

    pCourier = new (CurrentObjectHeap()) class font(this,"Courier");

    // Consider loading the fontDirectory from the font constructor
    pFontDirectory -> put("Courier",pCourier);

    pSystemDict -> put("true",pTrueConstant);
    pSystemDict -> put("false",pFalseConstant);
    pSystemDict -> put("null",pNullConstant);
    pSystemDict -> put("Courier",pCourier);
    pSystemDict -> put("DeviceGray",pDefaultColorSpace);
    pSystemDict -> put("DeviceRGB",new (CurrentObjectHeap()) colorSpace(this,"DeviceRGB"));

    dictionaryStack.setCurrent(pGlobalDict);
    dictionaryStack.setCurrent(pStatusDict);
    dictionaryStack.setCurrent(pErrorDict);
    dictionaryStack.setCurrent(pPdfDict);
    dictionaryStack.setCurrent(pUserDict);
    dictionaryStack.setCurrent(pSystemDict);

    graphicsStateStack.push(new (CurrentObjectHeap()) graphicsState(this,hwndSurface));

    return;
    }


    /*
        Don't use CurrentObjectHeap with anything that is NOT derived
        from object.
        The compiler will not complain if you do, but the current heap pointer
        will not advance unless it is actually using the object::new operator.
        This will cause subsequent pointers to non-object entities to point
        to the first one.
    */
    void *job::CurrentObjectHeap() {
    return object::pCurrentHeap;
    }


    job::~job() {

    if ( pStorage )
        delete [] pStorage;

    for ( comment *pComment : commentStack) 
        delete pComment;

    commentStack.clear();

    return;
    }


    void job::resolve() {

    object *pTop = top();

    switch ( pTop -> ObjectType() ) {
    case object::number:
        return;

    default:
        operatorLoad();

    }

    return;
    }


    object *job::resolve(char *pszObjectName) {

    dictionary *pDictionary = dictionaryStack.find(pszObjectName);

    if ( NULL == pDictionary )
        return NULL;

    return pDictionary -> retrieve(pszObjectName);
    }


    void job::addFont(font *pFont) {
    fontList.insert(fontList.end(),pFont);
    return;
    }


    boolean job::deleteObject(object *pObj) {

    if ( NULL == pObj )
        return false;

    if ( ! ( NULL == pObj -> pContainingDictionary ) ) 
        return false;

    delete pObj;

    return true;
    }


    void job::deleteNonContainedObject(object *pObj) {

    delete pObj;

    return;
    }


    long job::getPageCount(char *pszFileName) {

    FILE *f = fopen(pszFileName,"rb");
    if ( NULL == f ) 
       return 0L;

    fseek(f,0,SEEK_END);
    long fileSize = ftell(f);
    fseek(f,fileSize - 256,SEEK_SET);

    char szData[256];
    fread(szData,256,1,f);
    fclose(f);

    char *p = strstr(szData,"%%Pages: ");
    if ( NULL == p )
        return 0L;

    p += 9;

    return atol(p);
    }


    graphicsState *job::currentGS() {
    return graphicsStateStack.top();
    }