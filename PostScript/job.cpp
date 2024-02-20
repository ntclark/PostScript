
#include "utilities.h"

#include "PostScript objects\matrix.h"
#include "job.h"
#include "Fonts\font.h"
#include "PostScript objects\graphicsState.h"

    long loggingOff = 0L;

    PdfUtility *job::pPdfUtility = NULL;

    job::job(char *pszFileName,PdfPage *pPage,HDC dc,RECT *prcWindowsClip,IPostScriptTakeText *pIPSTT) :

        pStorage(NULL),
        pEnd(NULL),

        hdcTarget(dc),

        pPdfPage(pPage),
        pIPostScriptTakeText(pIPSTT),

        isGlobalVM(true),
        hasExited(false),

        saveCount(0L),
        inputLineNumber(1L),
        procedureDefinitionLevel(0L),

        graphicsStateStack(this)

    {

    object::isCreatingSystemObjects = true;

    if ( ! pPdfUtility )
        pPdfUtility = new PdfUtility();

    pLanguageLevel = new (CurrentObjectHeap()) object(this,2L);

    pUserDict = new (CurrentObjectHeap()) dictionary(this,"userdict",true);

    pSystemDict = new (CurrentObjectHeap()) dictionary(this,"systemdict",true);
    pGlobalDict = new (CurrentObjectHeap()) dictionary(this,"globaldict",true);
    pStatusDict = new (CurrentObjectHeap()) dictionary(this,"statusdict",true);
    pErrorDict = new (CurrentObjectHeap()) dictionary(this,"errordict",true);
    pPdfDict = new (CurrentObjectHeap()) dictionary(this,"pdfdict",true);

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
        }
    }

    if ( prcWindowsClip )
        memcpy(&rcWindowsClip,prcWindowsClip,sizeof(RECT));
    else
        memset(&rcWindowsClip,0,sizeof(RECT));

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

    nameTypeMap[object::atom][object::container] = pArrayType;
    nameTypeMap[object::atom][object::string] = pStringType;
    nameTypeMap[object::atom][object::character] = pStringType;
    nameTypeMap[object::atom][object::integer] = pIntegerType;
    nameTypeMap[object::atom][object::radix] = pIntegerType;
    nameTypeMap[object::atom][object::real] = pRealType;

    nameTypeMap[object::procedure][object::string] = pStringType;
    nameTypeMap[object::dictionary][object::string] = pDictType;
    nameTypeMap[object::array][object::container] = pArrayType;
    nameTypeMap[object::number][object::integer] = pIntegerType;
    nameTypeMap[object::number][object::radix] = pIntegerType;
    nameTypeMap[object::number][object::real] = pRealType;
    nameTypeMap[object::logical][object::string] = pBooleanType;
    nameTypeMap[object::mark][object::unspecified] = pMarkType;
    nameTypeMap[object::null][object::unspecified] = pNullType;
    nameTypeMap[object::save][object::unspecified] = pSaveType;
    nameTypeMap[object::font][object::unspecified] = pFontType;
    nameTypeMap[object::directExecutable][object::unspecified] = pOperatorType;
    nameTypeMap[object::name][object::unspecified] = pNameType;

    tokenProcedures[HashCode(DSC_DELIMITER)] = &job::parseStructureSpec;
    tokenProcedures[HashCode(COMMENT_DELIMITER)] = &job::parseComment;
    tokenProcedures[HashCode(STRING_DELIMITER_BEGIN)] = &job::parseString;
    tokenProcedures[HashCode(HEX_STRING_DELIMITER_BEGIN)] = &job::parseHexString;
    tokenProcedures[HashCode(HEX85_DELIMITER_BEGIN)] = &job::parseHex85String;
    tokenProcedures[HashCode(LITERAL_DELIMITER)] = &job::parseLiteralName;

    antiDelimiters[HashCode(DSC_DELIMITER)] = "";
    antiDelimiters[HashCode(COMMENT_DELIMITER)] = "";
    antiDelimiters[HashCode(STRING_DELIMITER_BEGIN)] = STRING_DELIMITER_END;
    antiDelimiters[HashCode(HEX_STRING_DELIMITER_BEGIN)] = HEX_STRING_DELIMITER_END;
    antiDelimiters[HashCode(HEX85_DELIMITER_BEGIN)] = HEX85_DELIMITER_END;
    antiDelimiters[HashCode(LITERAL_DELIMITER)] = "";
    antiDelimiters[HashCode(ARRAY_DELIMITER_BEGIN)] = ARRAY_DELIMITER_END;

    pSystemDict -> insert("languagelevel",pLanguageLevel);

    pSystemDict -> insert(ARRAY_DELIMITER_BEGIN,&job::operatorMarkArrayBegin);
    pSystemDict -> insert(ARRAY_DELIMITER_END,&job::operatorMarkArrayEnd);

    pSystemDict -> insert(DICTIONARY_DELIMITER_BEGIN,&job::operatorMarkDictionaryBegin);
    pSystemDict -> insert(DICTIONARY_DELIMITER_END,&job::operatorMarkDictionaryEnd);

    pSystemDict -> insert(PROC_DELIMITER_BEGIN,&job::operatorMarkProcedureBegin);
    pSystemDict -> insert(PROC_DELIMITER_END,&job::operatorMarkProcedureEnd);

    pSystemDict -> insert("=",&job::operatorStdout);

    pSystemDict -> insert("add",&job::operatorAdd);
    pSystemDict -> insert("and",&job::operatorAnd);
    pSystemDict -> insert("arc",&job::operatorArc);
    pSystemDict -> insert("arcn",&job::operatorArcn);
    pSystemDict -> insert("array",&job::operatorArray);
    pSystemDict -> insert("ashow",&job::operatorAshow);
    pSystemDict -> insert("astore",&job::operatorAstore);
    pSystemDict -> insert("awidthshow",&job::operatorAwidthshow);
    pSystemDict -> insert("begin",&job::operatorBegin);
    pSystemDict -> insert("bind",&job::operatorBind);
    pSystemDict -> insert("cleartomark",&job::operatorCleartomark);
    pSystemDict -> insert("closepath",&job::operatorClosepath);
    pSystemDict -> insert("concat",&job::operatorConcat);
    pSystemDict -> insert("copy",&job::operatorCopy);
    pSystemDict -> insert("countdictstack",&job::operatorCountdictstack);
    pSystemDict -> insert("counttomark",&job::operatorCounttomark);
    pSystemDict -> insert("currentdict",&job::operatorCurrentdict);
    pSystemDict -> insert("currentglobal",&job::operatorCurrentglobal);
    pSystemDict -> insert("currentmatrix",&job::operatorCurrentmatrix);
    pSystemDict -> insert("currentscreen",&job::operatorCurrentscreen);
    pSystemDict -> insert("curveto",&job::operatorCurveto);
    pSystemDict -> insert("cvi",&job::operatorCvi);
    pSystemDict -> insert("cvn",&job::operatorCvn);
    pSystemDict -> insert("cvr",&job::operatorCvr);
    pSystemDict -> insert("cvx",&job::operatorCvx);
    pSystemDict -> insert("debug",&job::operatorDebug);
    pSystemDict -> insert("def",&job::operatorDef);
    pSystemDict -> insert("definefont",&job::operatorDefinefont);
    pSystemDict -> insert("defineresource",&job::operatorDefineresource);
    pSystemDict -> insert("dtransform",&job::operatorDtransform);
    pSystemDict -> insert("dict",&job::operatorDict);
    pSystemDict -> insert("div",&job::operatorDiv);
    pSystemDict -> insert("dup",&job::operatorDup);
    pSystemDict -> insert("end",&job::operatorEnd);
    pSystemDict -> insert("errordict",&job::operatorErrordict);
    pSystemDict -> insert("eofill",&job::operatorEofill);
    pSystemDict -> insert("eq",&job::operatorEq);
    pSystemDict -> insert("exch",&job::operatorExch);
    pSystemDict -> insert("exec",&job::operatorExec);
    pSystemDict -> insert("executeonly",&job::operatorExecuteonly);
    pSystemDict -> insert("exit",&job::operatorExit);
    pSystemDict -> insert("findfont",&job::operatorFindfont);
    pSystemDict -> insert("findresource",&job::operatorFindresource);
    pSystemDict -> insert("fill",&job::operatorFill);
    pSystemDict -> insert("flush",&job::operatorFlush);
    pSystemDict -> insert("for",&job::operatorFor);
    pSystemDict -> insert("forall",&job::operatorForall);
    pSystemDict -> insert("ge",&job::operatorGe);
    pSystemDict -> insert("get",&job::operatorGet);
    pSystemDict -> insert("grestore",&job::operatorGrestore);
    pSystemDict -> insert("gsave",&job::operatorGsave);
    pSystemDict -> insert("gt",&job::operatorGt);
    pSystemDict -> insert("idtransform",&job::operatorIdtransform);
    pSystemDict -> insert("if",&job::operatorIf);
    pSystemDict -> insert("ifelse",&job::operatorIfelse);
    pSystemDict -> insert("index",&job::operatorIndex);
    pSystemDict -> insert("ISOLatin1Encoding",&job::operatorISOLatin1Encoding);
    pSystemDict -> insert("itransform",&job::operatorItransform);
    pSystemDict -> insert("known",&job::operatorKnown);
    pSystemDict -> insert("length",&job::operatorLength);
    pSystemDict -> insert("lineto",&job::operatorLineto);
    pSystemDict -> insert("load",&job::operatorLoad);
    pSystemDict -> insert("loop",&job::operatorLoop);
    pSystemDict -> insert("makefont",&job::operatorMakefont);
    pSystemDict -> insert("makepattern",&job::operatorMakepattern);
    pSystemDict -> insert("matrix",&job::operatorMatrix);
    pSystemDict -> insert("moveto",&job::operatorMoveto);
    pSystemDict -> insert("mul",&job::operatorMul);
    pSystemDict -> insert("ne",&job::operatorNe);
    pSystemDict -> insert("neg",&job::operatorNeg);
    pSystemDict -> insert("newpath",&job::operatorNewpath);
    pSystemDict -> insert("not",&job::operatorNot);
    pSystemDict -> insert("pop",&job::operatorPop);
    pSystemDict -> insert("print",&job::operatorPrint);
    pSystemDict -> insert("product",&job::operatorProduct);
    pSystemDict -> insert("pstack",&job::operatorPstack);
    pSystemDict -> insert("put",&job::operatorPut);
    pSystemDict -> insert("putinterval",&job::operatorPutinterval);
    pSystemDict -> insert("readonly",&job::operatorReadonly);
    pSystemDict -> insert("rectclip",&job::operatorRectclip);
    pSystemDict -> insert("rectfill",&job::operatorRectfill);
    pSystemDict -> insert("rectstroke",&job::operatorRectstroke);
    pSystemDict -> insert("repeat",&job::operatorRepeat);
    pSystemDict -> insert("rlineto",&job::operatorRlineto);
    pSystemDict -> insert("rmoveto",&job::operatorRmoveto);
    pSystemDict -> insert("roll",&job::operatorRoll);
    pSystemDict -> insert("rotate",&job::operatorRotate);
    pSystemDict -> insert("round",&job::operatorRound);
    pSystemDict -> insert("save",&job::operatorSave);
    pSystemDict -> insert("scale",&job::operatorScale);
    pSystemDict -> insert("scalefont",&job::operatorScalefont);
    pSystemDict -> insert("selectfont",&job::operatorSelectfont);
    pSystemDict -> insert("setcolor",&job::operatorSetcolor);
    pSystemDict -> insert("setcolorspace",&job::operatorSetcolorspace);
    pSystemDict -> insert("setdash",&job::operatorSetdash);
    pSystemDict -> insert("setfont",&job::operatorSetfont);
    pSystemDict -> insert("setglobal",&job::operatorSetglobal);
    pSystemDict -> insert("setgray",&job::operatorSetgray);
    pSystemDict -> insert("setlinecap",&job::operatorSetlinecap);
    pSystemDict -> insert("setlinejoin",&job::operatorSetlinejoin);
    pSystemDict -> insert("setlinewidth",&job::operatorSetlinewidth);
    pSystemDict -> insert("setmatrix",&job::operatorSetmatrix);
    pSystemDict -> insert("setmiterlimit",&job::operatorSetmiterlimit);
    pSystemDict -> insert("setpagedevice",&job::operatorSetpagedevice);
    pSystemDict -> insert("setuserparams",&job::operatorSetuserparams);
    pSystemDict -> insert("show",&job::operatorShow);
    pSystemDict -> insert("showpage",&job::operatorShowpage);
    pSystemDict -> insert("StandardEncoding",&job::operatorStandardEncoding);
    pSystemDict -> insert("stopped",&job::operatorStopped);
    pSystemDict -> insert("string",&job::operatorString);
    pSystemDict -> insert("stroke",&job::operatorStroke);
    pSystemDict -> insert("sub",&job::operatorSub);
    pSystemDict -> insert("transform",&job::operatorTransform);
    pSystemDict -> insert("translate",&job::operatorTranslate);
    pSystemDict -> insert("type",&job::operatorType);
    pSystemDict -> insert("undef",&job::operatorUndef);
    pSystemDict -> insert("version",&job::operatorVersion);
    pSystemDict -> insert("vmstatus",&job::operatorVmstatus);
    pSystemDict -> insert("where",&job::operatorWhere);
    pSystemDict -> insert("widthshow",&job::operatorWidthshow);
    pSystemDict -> insert("xshow",&job::operatorXshow);
    pSystemDict -> insert("xyshow",&job::operatorXyshow);
    pSystemDict -> insert("yshow",&job::operatorYshow);

    pPdfDict -> insert("'",&job::pdfOperator_apostrophe);
    pPdfDict -> insert("\"",&job::pdfOperator_quote);
    pPdfDict -> insert("b",&job::pdfOperator_b);
    pPdfDict -> insert("b*",&job::pdfOperator_bStar);
    pPdfDict -> insert("B",&job::pdfOperator_B);
    pPdfDict -> insert("B*",&job::pdfOperator_BStar);
    pPdfDict -> insert("BDC",&job::pdfOperator_BDC);
    pPdfDict -> insert("BI",&job::pdfOperator_BI);
    pPdfDict -> insert("BMC",&job::pdfOperator_BMC);
    pPdfDict -> insert("BT",&job::pdfOperator_BT);
    pPdfDict -> insert("BX",&job::pdfOperator_BX);
    pPdfDict -> insert("c",&job::pdfOperator_c);
    pPdfDict -> insert("cm",&job::pdfOperator_cm);
    pPdfDict -> insert("CS",&job::pdfOperator_CS);
    pPdfDict -> insert("cs",&job::pdfOperator_cs);
    pPdfDict -> insert("d",&job::pdfOperator_d);
    pPdfDict -> insert("d0",&job::pdfOperator_d0);
    pPdfDict -> insert("d1",&job::pdfOperator_d1);
    pPdfDict -> insert("Do",&job::pdfOperator_Do);
    pPdfDict -> insert("DP",&job::pdfOperator_DP);
    pPdfDict -> insert("EI",&job::pdfOperator_EI);
    pPdfDict -> insert("EMC",&job::pdfOperator_EMC);
    pPdfDict -> insert("ET",&job::pdfOperator_ET);
    pPdfDict -> insert("EX",&job::pdfOperator_EX);
    pPdfDict -> insert("f*",&job::pdfOperator_fStar);
    pPdfDict -> insert("f",&job::pdfOperator_f);
    pPdfDict -> insert("F",&job::pdfOperator_F);
    pPdfDict -> insert("g",&job::pdfOperator_g);
    pPdfDict -> insert("G",&job::pdfOperator_G);
    pPdfDict -> insert("gs",&job::pdfOperator_gs);
    pPdfDict -> insert("h",&job::pdfOperator_h);
    pPdfDict -> insert("i",&job::pdfOperator_i);
    pPdfDict -> insert("ID",&job::pdfOperator_ID);
    pPdfDict -> insert("j",&job::pdfOperator_j);
    pPdfDict -> insert("J",&job::pdfOperator_J);
    pPdfDict -> insert("k",&job::pdfOperator_k);
    pPdfDict -> insert("K",&job::pdfOperator_K);
    pPdfDict -> insert("l",&job::pdfOperator_l);
    pPdfDict -> insert("m",&job::pdfOperator_m);
    pPdfDict -> insert("M",&job::pdfOperator_M);
    pPdfDict -> insert("MP",&job::pdfOperator_MP);
    pPdfDict -> insert("n",&job::pdfOperator_n);
    pPdfDict -> insert("q",&job::pdfOperator_q);
    pPdfDict -> insert("Q",&job::pdfOperator_Q);
    pPdfDict -> insert("re",&job::pdfOperator_re);
    pPdfDict -> insert("rg",&job::pdfOperator_rg);
    pPdfDict -> insert("RG",&job::pdfOperator_RG);
    pPdfDict -> insert("ri",&job::pdfOperator_ri);
    pPdfDict -> insert("s",&job::pdfOperator_s);
    pPdfDict -> insert("S",&job::pdfOperator_S);
    pPdfDict -> insert("sc",&job::pdfOperator_sc);
    pPdfDict -> insert("SC",&job::pdfOperator_SC);
    pPdfDict -> insert("SCN",&job::pdfOperator_SCN);
    pPdfDict -> insert("scn",&job::pdfOperator_scn);
    pPdfDict -> insert("sh",&job::pdfOperator_sh);
    pPdfDict -> insert("T*",&job::pdfOperator_TStar);
    pPdfDict -> insert("Tc",&job::pdfOperator_Tc);
    pPdfDict -> insert("Td",&job::pdfOperator_Td);
    pPdfDict -> insert("TD",&job::pdfOperator_TD);
    pPdfDict -> insert("Tf",&job::pdfOperator_Tf);
    pPdfDict -> insert("Th",&job::pdfOperator_Th);
    pPdfDict -> insert("TJ",&job::pdfOperator_TJ);
    pPdfDict -> insert("Tj",&job::pdfOperator_Tj);
    pPdfDict -> insert("TL",&job::pdfOperator_TL);
    pPdfDict -> insert("Tm",&job::pdfOperator_Tm);
    pPdfDict -> insert("Tr",&job::pdfOperator_Tr);
    pPdfDict -> insert("Ts",&job::pdfOperator_Ts);
    pPdfDict -> insert("Tw",&job::pdfOperator_Tw);
    pPdfDict -> insert("Tz",&job::pdfOperator_Tz);
    pPdfDict -> insert("v",&job::pdfOperator_v);
    pPdfDict -> insert("w",&job::pdfOperator_w);
    pPdfDict -> insert("W",&job::pdfOperator_W);
    pPdfDict -> insert("W*",&job::pdfOperator_WStar);
    pPdfDict -> insert("y",&job::pdfOperator_y);

    pSystemDict -> insert("globaldict",pGlobalDict);
    pSystemDict -> insert("systemdict",pSystemDict);
    pSystemDict -> insert("statusdict",pStatusDict);
    pSystemDict -> insert("userdict",pUserDict);
    pSystemDict -> insert("pdfdict",pPdfDict);
    pSystemDict -> insert("errordict",pErrorDict);

    pTrueConstant = new (CurrentObjectHeap()) booleanObject(this,"true");
    pFalseConstant = new (CurrentObjectHeap()) booleanObject(this,"false");
    pNullConstant = new (CurrentObjectHeap()) object(this,"");
    pCourier = new (CurrentObjectHeap()) class font(this,"Courier");

    pSystemDict -> insert("true",pTrueConstant);
    pSystemDict -> insert("false",pFalseConstant);
    pSystemDict -> insert("null",pNullConstant);
    pSystemDict -> insert("Courier",pCourier);
    pSystemDict -> insert("DeviceGray",pDefaultColorSpace);
    pSystemDict -> insert("DeviceRGB",new (CurrentObjectHeap()) colorSpace(this,"DeviceRGB"));

    graphicsStateStack.push(new (CurrentObjectHeap()) graphicsState(this,WindowsClipArea()));

    object::isCreatingSystemObjects = false;

    return;
    }


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


//
// I am not entirely comfortable that this is right.
//
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


    void job::addFont(font *pFont) {
    fontList.insert(fontList.end(),pFont);
    return;
    }


    void job::addProcedure(procedure *pProcedure) {
    char szTemp[32];
    sprintf(szTemp,"procedure%ld",(long)procedureList.size() + 1);
    pProcedure -> Name(szTemp);
    procedureList.insert(procedureList.end(),pProcedure);
    return;
    }


    boolean job::deleteObject(object *pObj) {

    if ( NULL == pObj )
        return false;

    if ( ! ( NULL == pObj -> pContainingDictionary ) ) 
        return false;

OutputDebugString("Deleting object: ");
OutputDebugString(pObj -> pszName);
OutputDebugString(" : ");
OutputDebugString(pObj -> pszContents);
OutputDebugString("\n");

    delete pObj;

    return true;
    }

    void job::deleteNonContainedObject(object *pObj) {

OutputDebugString("Deleting Non-contained object: ");
OutputDebugString(pObj -> pszName);
OutputDebugString(" : ");
OutputDebugString(pObj -> pszContents);
OutputDebugString("\n");

    delete pObj;

    return;
    }

