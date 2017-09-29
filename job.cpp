
#include "utilities.h"

#include "matrix.h"
#include "job.h"
#include "font.h"
#include "graphicsState.h"

   long loggingOff = 0L;

   PdfUtility *job::pPdfUtility = NULL;

   job::job(char *pszFileName,PdfPage *pPage,HDC dc,RECT *prcWindowsClip,IPostScriptTakeText *pIPSTT) :

      pStorage(NULL),
      pEnd(NULL),

      hdcTarget(dc),

      pPdfPage(pPage),
      pIPostScriptTakeText(pIPSTT),

      userDict(this,"userdict"),
      systemDict(this,"systemdict"),
      globalDict(this,"globaldict"),
      statusDict(this,"statusdict"),
      errorDict(this,"errordict"),
      pdfDict(this,"pdfdict"),

      pTrueConstant(NULL),
      pFalseConstant(NULL),
      pNullConstant(NULL),

      pCourier(NULL),

      isGlobalVM(true),
      hasExited(false),

      saveCount(0L),
      inputLineNumber(1L),
      procedureDefinitionLevel(0L),

      defaultMatrix(),
      pCurrentMatrix(&defaultMatrix),

      defaultColorSpace(this,"DeviceGray"),
      pCurrentColorSpace(&defaultColorSpace),

      languageLevel(2L),
      StandardEncoding(this,"StandardEncoding",256),
      ISOLatin1Encoding(this,"ISOLatin1Encoding",256),

      graphicsStateStack(this)

   {

   if ( ! pPdfUtility )
      pPdfUtility = new PdfUtility();

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
   
   initialize();

   return;
   }


   void job::initialize() {

   std::map<object::valueType,object *> innerMap;

   pStringType = new name("stringtype");
   pArrayType = new name("arraytype");
   pIntegerType = new name("integertype");
   pRealType = new name("realtype");
   pBooleanType = new name("booleantype");
   pDictType = new name("dicttype");
   pMarkType = new name("marktype");
   pNullType = new name("nulltype");
   pSaveType = new name("savetype");
   pFontType = new name("fonttype");
   pOperatorType = new name("operatortype");
   pNameType = new name("nametype");

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

   systemDict.insert("languagelevel",&languageLevel);

   systemDict.insert(ARRAY_DELIMITER_BEGIN,&job::operatorMarkArrayBegin);
   systemDict.insert(ARRAY_DELIMITER_END,&job::operatorMarkArrayEnd);

   systemDict.insert(DICTIONARY_DELIMITER_BEGIN,&job::operatorMarkDictionaryBegin);
   systemDict.insert(DICTIONARY_DELIMITER_END,&job::operatorMarkDictionaryEnd);

   systemDict.insert(PROC_DELIMITER_BEGIN,&job::operatorMarkProcedureBegin);
   systemDict.insert(PROC_DELIMITER_END,&job::operatorMarkProcedureEnd);

   systemDict.insert("=",&job::operatorStdout);

   systemDict.insert("add",&job::operatorAdd);
   systemDict.insert("and",&job::operatorAnd);
   systemDict.insert("arc",&job::operatorArc);
   systemDict.insert("arcn",&job::operatorArcn);
   systemDict.insert("array",&job::operatorArray);
   systemDict.insert("ashow",&job::operatorAshow);
   systemDict.insert("astore",&job::operatorAstore);
   systemDict.insert("awidthshow",&job::operatorAwidthshow);
   systemDict.insert("begin",&job::operatorBegin);
   systemDict.insert("bind",&job::operatorBind);
   systemDict.insert("cleartomark",&job::operatorCleartomark);
   systemDict.insert("closepath",&job::operatorClosepath);
   systemDict.insert("concat",&job::operatorConcat);
   systemDict.insert("copy",&job::operatorCopy);
   systemDict.insert("countdictstack",&job::operatorCountdictstack);
   systemDict.insert("counttomark",&job::operatorCounttomark);
   systemDict.insert("currentdict",&job::operatorCurrentdict);
   systemDict.insert("currentglobal",&job::operatorCurrentglobal);
   systemDict.insert("currentmatrix",&job::operatorCurrentmatrix);
   systemDict.insert("currentscreen",&job::operatorCurrentscreen);
   systemDict.insert("curveto",&job::operatorCurveto);
   systemDict.insert("cvi",&job::operatorCvi);
   systemDict.insert("cvn",&job::operatorCvn);
   systemDict.insert("cvr",&job::operatorCvr);
   systemDict.insert("cvx",&job::operatorCvx);
   systemDict.insert("debug",&job::operatorDebug);
   systemDict.insert("def",&job::operatorDef);
   systemDict.insert("definefont",&job::operatorDefinefont);
   systemDict.insert("defineresource",&job::operatorDefineresource);
   systemDict.insert("dtransform",&job::operatorDtransform);
   systemDict.insert("dict",&job::operatorDict);
   systemDict.insert("div",&job::operatorDiv);
   systemDict.insert("dup",&job::operatorDup);
   systemDict.insert("end",&job::operatorEnd);
   systemDict.insert("errordict",&job::operatorErrordict);
   systemDict.insert("eofill",&job::operatorEofill);
   systemDict.insert("eq",&job::operatorEq);
   systemDict.insert("exch",&job::operatorExch);
   systemDict.insert("exec",&job::operatorExec);
   systemDict.insert("executeonly",&job::operatorExecuteonly);
   systemDict.insert("exit",&job::operatorExit);
   systemDict.insert("findfont",&job::operatorFindfont);
   systemDict.insert("findresource",&job::operatorFindresource);
   systemDict.insert("fill",&job::operatorFill);
   systemDict.insert("flush",&job::operatorFlush);
   systemDict.insert("for",&job::operatorFor);
   systemDict.insert("forall",&job::operatorForall);
   systemDict.insert("ge",&job::operatorGe);
   systemDict.insert("get",&job::operatorGet);
   systemDict.insert("grestore",&job::operatorGrestore);
   systemDict.insert("gsave",&job::operatorGsave);
   systemDict.insert("gt",&job::operatorGt);
   systemDict.insert("idtransform",&job::operatorIdtransform);
   systemDict.insert("if",&job::operatorIf);
   systemDict.insert("ifelse",&job::operatorIfelse);
   systemDict.insert("index",&job::operatorIndex);
   systemDict.insert("ISOLatin1Encoding",&job::operatorISOLatin1Encoding);
   systemDict.insert("itransform",&job::operatorItransform);
   systemDict.insert("known",&job::operatorKnown);
   systemDict.insert("length",&job::operatorLength);
   systemDict.insert("lineto",&job::operatorLineto);
   systemDict.insert("load",&job::operatorLoad);
   systemDict.insert("loop",&job::operatorLoop);
   systemDict.insert("makefont",&job::operatorMakefont);
   systemDict.insert("makepattern",&job::operatorMakepattern);
   systemDict.insert("matrix",&job::operatorMatrix);
   systemDict.insert("moveto",&job::operatorMoveto);
   systemDict.insert("mul",&job::operatorMul);
   systemDict.insert("ne",&job::operatorNe);
   systemDict.insert("neg",&job::operatorNeg);
   systemDict.insert("newpath",&job::operatorNewpath);
   systemDict.insert("not",&job::operatorNot);
   systemDict.insert("pop",&job::operatorPop);
   systemDict.insert("print",&job::operatorPrint);
   systemDict.insert("product",&job::operatorProduct);
   systemDict.insert("pstack",&job::operatorPstack);
   systemDict.insert("put",&job::operatorPut);
   systemDict.insert("putinterval",&job::operatorPutinterval);
   systemDict.insert("readonly",&job::operatorReadonly);
   systemDict.insert("rectclip",&job::operatorRectclip);
   systemDict.insert("rectfill",&job::operatorRectfill);
   systemDict.insert("rectstroke",&job::operatorRectstroke);
   systemDict.insert("repeat",&job::operatorRepeat);
   systemDict.insert("rlineto",&job::operatorRlineto);
   systemDict.insert("rmoveto",&job::operatorRmoveto);
   systemDict.insert("roll",&job::operatorRoll);
   systemDict.insert("rotate",&job::operatorRotate);
   systemDict.insert("round",&job::operatorRound);
   systemDict.insert("save",&job::operatorSave);
   systemDict.insert("scale",&job::operatorScale);
   systemDict.insert("scalefont",&job::operatorScalefont);
   systemDict.insert("selectfont",&job::operatorSelectfont);
   systemDict.insert("setcolor",&job::operatorSetcolor);
   systemDict.insert("setcolorspace",&job::operatorSetcolorspace);
   systemDict.insert("setdash",&job::operatorSetdash);
   systemDict.insert("setfont",&job::operatorSetfont);
   systemDict.insert("setglobal",&job::operatorSetglobal);
   systemDict.insert("setgray",&job::operatorSetgray);
   systemDict.insert("setlinecap",&job::operatorSetlinecap);
   systemDict.insert("setlinejoin",&job::operatorSetlinejoin);
   systemDict.insert("setlinewidth",&job::operatorSetlinewidth);
   systemDict.insert("setmatrix",&job::operatorSetmatrix);
   systemDict.insert("setmiterlimit",&job::operatorSetmiterlimit);
   systemDict.insert("setpagedevice",&job::operatorSetpagedevice);
   systemDict.insert("setuserparams",&job::operatorSetuserparams);
   systemDict.insert("show",&job::operatorShow);
   systemDict.insert("showpage",&job::operatorShowpage);
   systemDict.insert("StandardEncoding",&job::operatorStandardEncoding);
   systemDict.insert("stopped",&job::operatorStopped);
   systemDict.insert("string",&job::operatorString);
   systemDict.insert("stroke",&job::operatorStroke);
   systemDict.insert("sub",&job::operatorSub);
   systemDict.insert("transform",&job::operatorTransform);
   systemDict.insert("translate",&job::operatorTranslate);
   systemDict.insert("type",&job::operatorType);
   systemDict.insert("undef",&job::operatorUndef);
   systemDict.insert("version",&job::operatorVersion);
   systemDict.insert("vmstatus",&job::operatorVmstatus);
   systemDict.insert("where",&job::operatorWhere);
   systemDict.insert("widthshow",&job::operatorWidthshow);
   systemDict.insert("xshow",&job::operatorXshow);
   systemDict.insert("xyshow",&job::operatorXyshow);
   systemDict.insert("yshow",&job::operatorYshow);

   pdfDict.insert("'",&job::pdfOperator_apostrophe);
   pdfDict.insert("\"",&job::pdfOperator_quote);
   pdfDict.insert("b",&job::pdfOperator_b);
   pdfDict.insert("b*",&job::pdfOperator_bStar);
   pdfDict.insert("B",&job::pdfOperator_B);
   pdfDict.insert("B*",&job::pdfOperator_BStar);
   pdfDict.insert("BDC",&job::pdfOperator_BDC);
   pdfDict.insert("BI",&job::pdfOperator_BI);
   pdfDict.insert("BMC",&job::pdfOperator_BMC);
   pdfDict.insert("BT",&job::pdfOperator_BT);
   pdfDict.insert("BX",&job::pdfOperator_BX);
   pdfDict.insert("c",&job::pdfOperator_c);
   pdfDict.insert("cm",&job::pdfOperator_cm);
   pdfDict.insert("CS",&job::pdfOperator_CS);
   pdfDict.insert("cs",&job::pdfOperator_cs);
   pdfDict.insert("d",&job::pdfOperator_d);
   pdfDict.insert("d0",&job::pdfOperator_d0);
   pdfDict.insert("d1",&job::pdfOperator_d1);
   pdfDict.insert("Do",&job::pdfOperator_Do);
   pdfDict.insert("DP",&job::pdfOperator_DP);
   pdfDict.insert("EI",&job::pdfOperator_EI);
   pdfDict.insert("EMC",&job::pdfOperator_EMC);
   pdfDict.insert("ET",&job::pdfOperator_ET);
   pdfDict.insert("EX",&job::pdfOperator_EX);
   pdfDict.insert("f*",&job::pdfOperator_fStar);
   pdfDict.insert("f",&job::pdfOperator_f);
   pdfDict.insert("F",&job::pdfOperator_F);
   pdfDict.insert("g",&job::pdfOperator_g);
   pdfDict.insert("G",&job::pdfOperator_G);
   pdfDict.insert("gs",&job::pdfOperator_gs);
   pdfDict.insert("h",&job::pdfOperator_h);
   pdfDict.insert("i",&job::pdfOperator_i);
   pdfDict.insert("ID",&job::pdfOperator_ID);
   pdfDict.insert("j",&job::pdfOperator_j);
   pdfDict.insert("J",&job::pdfOperator_J);
   pdfDict.insert("k",&job::pdfOperator_k);
   pdfDict.insert("K",&job::pdfOperator_K);
   pdfDict.insert("l",&job::pdfOperator_l);
   pdfDict.insert("m",&job::pdfOperator_m);
   pdfDict.insert("M",&job::pdfOperator_M);
   pdfDict.insert("MP",&job::pdfOperator_MP);
   pdfDict.insert("n",&job::pdfOperator_n);
   pdfDict.insert("q",&job::pdfOperator_q);
   pdfDict.insert("Q",&job::pdfOperator_Q);
   pdfDict.insert("re",&job::pdfOperator_re);
   pdfDict.insert("rg",&job::pdfOperator_rg);
   pdfDict.insert("RG",&job::pdfOperator_RG);
   pdfDict.insert("ri",&job::pdfOperator_ri);
   pdfDict.insert("s",&job::pdfOperator_s);
   pdfDict.insert("S",&job::pdfOperator_S);
   pdfDict.insert("sc",&job::pdfOperator_sc);
   pdfDict.insert("SC",&job::pdfOperator_SC);
   pdfDict.insert("SCN",&job::pdfOperator_SCN);
   pdfDict.insert("scn",&job::pdfOperator_scn);
   pdfDict.insert("sh",&job::pdfOperator_sh);
   pdfDict.insert("T*",&job::pdfOperator_TStar);
   pdfDict.insert("Tc",&job::pdfOperator_Tc);
   pdfDict.insert("Td",&job::pdfOperator_Td);
   pdfDict.insert("TD",&job::pdfOperator_TD);
   pdfDict.insert("Tf",&job::pdfOperator_Tf);
   pdfDict.insert("Th",&job::pdfOperator_Th);
   pdfDict.insert("TJ",&job::pdfOperator_TJ);
   pdfDict.insert("Tj",&job::pdfOperator_Tj);
   pdfDict.insert("TL",&job::pdfOperator_TL);
   pdfDict.insert("Tm",&job::pdfOperator_Tm);
   pdfDict.insert("Tr",&job::pdfOperator_Tr);
   pdfDict.insert("Ts",&job::pdfOperator_Ts);
   pdfDict.insert("Tw",&job::pdfOperator_Tw);
   pdfDict.insert("Tz",&job::pdfOperator_Tz);
   pdfDict.insert("v",&job::pdfOperator_v);
   pdfDict.insert("w",&job::pdfOperator_w);
   pdfDict.insert("W",&job::pdfOperator_W);
   pdfDict.insert("W*",&job::pdfOperator_WStar);
   pdfDict.insert("y",&job::pdfOperator_y);

   dictionaryStack.push(&globalDict);
   dictionaryStack.push(&systemDict);
   dictionaryStack.push(&statusDict);
   dictionaryStack.push(&userDict);
   dictionaryStack.push(&pdfDict);

   systemDict.insert("globaldict",&globalDict);
   systemDict.insert("systemdict",&systemDict);
   systemDict.insert("statusdict",&statusDict);
   systemDict.insert("userdict",&userDict);
   systemDict.insert("pdfdict",&pdfDict);

   systemDict.insert("errordict",&errorDict);

   pTrueConstant = new booleanObject("true");
   pFalseConstant = new booleanObject("false");

   pNullConstant = new object("");

   pCourier = new class font(this,"Courier");

   systemDict.insert("true",pTrueConstant);
   systemDict.insert("false",pFalseConstant);
   systemDict.insert("null",pNullConstant);
   systemDict.insert("DeviceGray",&defaultColorSpace);
   systemDict.insert("DeviceRGB",new colorSpace(this,"DeviceRGB"));

   graphicsStateStack.push(new graphicsState(this,GetDC(),WindowsClipArea()));

   return;
   }


   job::~job() {

   if ( pStorage )
      delete [] pStorage;

   for ( std::list<comment *>::iterator it = commentStack.begin(); it != commentStack.end(); it++ ) 
      delete (*it);

   commentStack.clear();

   while ( operandStack.size() ) 
      delete pop();

   delete pTrueConstant;
   delete pFalseConstant;

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
