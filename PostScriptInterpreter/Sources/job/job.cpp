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

#include "StandardEncoding.h"
#include "ISOLatin1Encoding.h"

    void *job::pHeap = NULL;
    void *job::pCurrentHeap = NULL;
    void *job::pNextHeap = NULL;
    size_t job::currentlyAllocatedHeap = 0L;


    job::job(char *pszFileName,HWND hwndSurf) : hwndSurface(hwndSurf) {

    initializeHeap();

    pPostScriptInterpreter -> ConnectServices();

    pLanguageLevel = new (CurrentObjectHeap()) object(this,2L);

    pUserDict = new (CurrentObjectHeap()) dictionary(this,"userdict");
    pSystemDict = new (CurrentObjectHeap()) dictionary(this,"systemdict");
    pGlobalDict = new (CurrentObjectHeap()) dictionary(this,"globaldict");
    pStatusDict = new (CurrentObjectHeap()) dictionary(this,"statusdict");
    pErrorDict = new (CurrentObjectHeap()) dictionary(this,"errordict");
    pPdfDict = new (CurrentObjectHeap()) dictionary(this,"pdfdict");

    pFontDirectory = new (CurrentObjectHeap()) dictionary(this,"font directory");

    pGenericResourceImplementation = new (CurrentObjectHeap()) dictionary(this,"generic resource implementation");

    pStandardEncoding = new (CurrentObjectHeap()) array(this,"StandardEncoding");

    for ( long k = 0; k < 256; k++ )
        pStandardEncoding -> putElement(k,new (CurrentObjectHeap()) object(this,StandardEncoding[k]));

    pISOLatin1Encoding = new (CurrentObjectHeap()) array(this,"ISOLatin1Encoding");

    for ( long k = 0; k < 256; k++ )
        pISOLatin1Encoding -> putElement(k,new (CurrentObjectHeap()) object(this,ISOLatin1Encoding[k]));

    pDefaultColorSpace = new (CurrentObjectHeap()) colorSpace(this,"DeviceGray");

    memset(szPostScriptSourceFile,0,sizeof(szPostScriptSourceFile));

    pPostScriptSourceFile = NULL;

    pStringType = new (CurrentObjectHeap()) name(this,"stringtype");
    pBinaryStringType = new (CurrentObjectHeap()) name(this,"stringtype");
    pArrayType = new (CurrentObjectHeap()) name(this,"arraytype");
    pPackedArrayType = new (CurrentObjectHeap()) name(this,"packedarraytype");
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

    nameTypeMap[object::objectType::atom][object::valueType::container] = pArrayType;
    nameTypeMap[object::objectType::atom][object::valueType::string] = pStringType;
	nameTypeMap[object::objectType::atom][object::valueType::constantString] = pStringType;
    nameTypeMap[object::objectType::atom][object::valueType::character] = pStringType;
    nameTypeMap[object::objectType::atom][object::valueType::integer] = pIntegerType;
    nameTypeMap[object::objectType::atom][object::valueType::radix] = pIntegerType;
    nameTypeMap[object::objectType::atom][object::valueType::real] = pRealType;
    nameTypeMap[object::objectType::atom][object::valueType::binaryString] = pBinaryStringType;

    nameTypeMap[object::objectType::literal][object::valueType::string] = pStringType;
    nameTypeMap[object::objectType::procedure][object::valueType::string] = pStringType;
    nameTypeMap[object::objectType::procedure][object::valueType::executableProcedure] = pPackedArrayType;
    nameTypeMap[object::objectType::objTypeMatrix][object::valueType::container] = pArrayType;
    nameTypeMap[object::objectType::dictionaryObject][object::valueType::string] = pDictType;
    nameTypeMap[object::objectType::objTypeArray][object::valueType::container] = pArrayType;
    nameTypeMap[object::objectType::number][object::valueType::integer] = pIntegerType;
    nameTypeMap[object::objectType::number][object::valueType::radix] = pIntegerType;
    nameTypeMap[object::objectType::number][object::valueType::real] = pRealType;
    nameTypeMap[object::objectType::logical][object::valueType::string] = pBooleanType;
    nameTypeMap[object::objectType::mark][object::valueType::valueTypeUnspecified] = pMarkType;
    nameTypeMap[object::objectType::null][object::valueType::valueTypeUnspecified] = pNullType;
    nameTypeMap[object::objectType::objTypeSave][object::valueType::valueTypeUnspecified] = pSaveType;
    nameTypeMap[object::objectType::font][object::valueType::valueTypeUnspecified] = pFontType;
    nameTypeMap[object::objectType::directExecutable][object::valueType::valueTypeUnspecified] = pOperatorType;
    nameTypeMap[object::objectType::name][object::valueType::valueTypeUnspecified] = pNameType;

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

    pFontTypeLiteral = new (CurrentObjectHeap()) object(this,"FontType");
    pFontNameLiteral = new (CurrentObjectHeap()) object(this,"FontName");
    pFontMatrixLiteral = new (CurrentObjectHeap()) object(this,"FontMatrix");
    pFontBoundingBoxLiteral = new (CurrentObjectHeap()) literal(this,"FontBBox");

    pEncodingArrayLiteral = new (CurrentObjectHeap()) literal(this,"Encoding");
    pNotdefLiteral = new (CurrentObjectHeap()) class literal(this,".notdef");
    pCharStringsLiteral = new (CurrentObjectHeap()) literal(this,"CharStrings");

    pGlyphDirectoryLiteral = new (CurrentObjectHeap()) literal(this,"GlyphDirectory");
    pSfntsLiteral = new (CurrentObjectHeap()) literal(this,"sfnts");
    pPageSizeLiteral = new (CurrentObjectHeap()) literal(this,"PageSize");
    pFontMatrixLiteral = new (CurrentObjectHeap()) literal(this,"FontMatrix");
    pRestoreFontMatrixLiteral = new (CurrentObjectHeap()) literal(this,"RestoreFontMatrix");
    pBuildGlyphLiteral = new (CurrentObjectHeap()) literal(this,"BuildGlyph");
    pBuildCharLiteral = new (CurrentObjectHeap()) literal(this,"BuildChar");

    pSystemDict -> put("true",pTrueConstant);
    pSystemDict -> put("false",pFalseConstant);
    pSystemDict -> put("null",pNullConstant);
    pSystemDict -> put("DeviceGray",pDefaultColorSpace);
    pSystemDict -> put("DeviceRGB",new (CurrentObjectHeap()) colorSpace(this,"DeviceRGB"));

    pSystemDict -> put("=string",new (CurrentObjectHeap()) string(this,"=string"));
    pSystemDict -> put("=print",new (CurrentObjectHeap()) string(this,"=print"));

    dictionaryStack.setCurrent(pGlobalDict);
    dictionaryStack.setCurrent(pStatusDict);
    dictionaryStack.setCurrent(pErrorDict);
    dictionaryStack.setCurrent(pPdfDict);
    dictionaryStack.setCurrent(pUserDict);
    dictionaryStack.setCurrent(pSystemDict);

    pGraphicsState = new graphicsState(this);

    if ( pszFileName ) {
        FILE *f = fopen(pszFileName,"rb");
        if ( f ) {
            strcpy(szPostScriptSourceFile,pszFileName);
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

    hsemIsInitialized = CreateSemaphore(NULL,0,1,NULL);

    unsigned int threadAddr;

    _beginthreadex(NULL,65536,job::executeInitialization,(void *)this,0L,&threadAddr);

    return;
    }


    job::~job() {

    for ( comment *pComment : commentStack ) 
        delete pComment;

    commentStack.clear();

    for ( structureSpec *pStructure : structureStack )
        delete pStructure;

    structureStack.clear();

    while ( 0 < operandStack.size() )
        operandStack.pop();

    while ( 0 < dictionaryStack.size() )
        dictionaryStack.pop();

    nameTypeMap.clear();

    tokenProcedures.clear();

    antiDelimiters.clear();

    if ( pStorage )
        delete [] pStorage;

    delete pGraphicsState;

    releaseHeap();

    pPostScriptInterpreter -> Cycle();

    return;
    }


    void job::initializeHeap() {
    pHeap = GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,OBJECT_HEAP_SIZE);
    pCurrentHeap = (BYTE *)pHeap;
    pNextHeap = (BYTE *)pCurrentHeap;
    currentlyAllocatedHeap = OBJECT_HEAP_SIZE;
    }


    void job::releaseHeap() {
    GlobalFree(pHeap);
    pHeap = NULL;
    pCurrentHeap = NULL;
    pNextHeap = NULL;
    currentlyAllocatedHeap = 0L;
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
    return pCurrentHeap;
    }
    void job::resolve() {

    object *pTop = top();

    switch ( pTop -> ObjectType() ) {
    case object::objectType::number:
        return;

    default:
        operatorLoad();

    }

    return;
    }


    object *job::resolve(char *pszObjectName) {

    char *p = pszObjectName;
    boolean isNumeric = true;
    while ( *p ) {
        if ( ! isdigit(*p) && ! ( '.' == *p ) && ! ( '-' == *p ) ) {
            isNumeric = false;
            break;
        }
        p++;
    }

    if ( isNumeric )
        return NULL;

    dictionary *pDictionary = dictionaryStack.find(pszObjectName);

    if ( NULL == pDictionary )
        return NULL;

    return pDictionary -> retrieve(pszObjectName);
    }


    //boolean job::deleteObject(object *pObj) {

    //if ( NULL == pObj )
    //    return false;

    //if ( ! ( NULL == pObj -> pContainingDictionary ) ) 
    //    return false;

    //delete pObj;

    //return true;
    //}


    //void job::deleteNonContainedObject(object *pObj) {
    //delete pObj;
    //return;
    //}


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
    return pGraphicsState;
    }