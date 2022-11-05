// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "PdfEnabler_i.h"
#include "pdfEnabler\Page.h"
#include "PostScript_i.h"

#include "PostScript objects\procedure.h"
#include "Stacks\graphicsStateStack.h"
#include "comment.h"
#include "PostScript objects\structureSpec.h"
#include "Stacks\objectStack.h"
#include "Stacks\dictionaryStack.h"
#include "PostScript objects\resource.h"
#include "PostScript objects\booleanObject.h"
#include "PostScript objects\name.h"
#include "PostScript objects\array.h"
#include "PostScript objects\colorSpace.h"
#include "error.h"
#include "PostScript objects\matrix.h"

   extern long loggingOff;

   class job {
   public:

      job(char *pszFileName,PdfPage *pPdfPage,HDC hdc,RECT *prcWindowsClip,IPostScriptTakeText *pIPostScriptTakeText);
      ~job();

      HDC GetDC() { return hdcTarget; };
      RECT *WindowsClipArea() { return &rcWindowsClip; };

      long parseJob(bool useThread = true);

      long execute(char *,char **ppEnd = NULL,long length = -1L);
      long execute();
      long execute(procedure *);

      void bindProcedure(procedure *pProcedure);

      void addFont(class font *);
      void addProcedure(procedure *);

      PdfUtility *GetPdfUtility() { return pPdfUtility; };

      object* top() { return operandStack.top(); };
      object* pop() { return operandStack.pop(); };
      void push(object *pObject) { operandStack.push(pObject); };

      void operatorStdout();

      void operatorAdd();
      void operatorAnd();
      void operatorArc();
      void operatorArcn();
      void operatorArray();
      void operatorAshow();
      void operatorAstore();
      void operatorAwidthshow();
      void operatorBegin();
      void operatorBind();
      void operatorCleartomark();
      void operatorClosepath();
      void operatorConcat();
      void operatorCopy();
      void operatorCountdictstack();
      void operatorCounttomark();
      void operatorCurrentdict();
      void operatorCurrentglobal();
      void operatorCurrentmatrix();
      void operatorCurrentscreen();
      void operatorCurveto();
      void operatorCvi();
      void operatorCvn();
      void operatorCvr();
      void operatorCvx();
      void operatorDebug();
      void operatorDef();
      void operatorDefinefont();
      void operatorDefineresource();
      void operatorDict();
      void operatorDiv();
      void operatorDtransform();
      void operatorDup();
      void operatorEnd();
      void operatorEofill();
      void operatorEq();
      void operatorErrordict();
      void operatorExch();
      void operatorExit();
      void operatorExec();
      void operatorExecuteonly();
      void operatorFill();
      void operatorFindfont();
      void operatorFindresource();
      void operatorFlush();
      void operatorFor();
      void operatorForall();
      void operatorGe();
      void operatorGet();
      void operatorGrestore();
      void operatorGsave();
      void operatorGt();
      void operatorIdtransform();
      void operatorIf();
      void operatorIfelse();
      void operatorIndex();
      void operatorISOLatin1Encoding();
      void operatorItransform();
      void operatorKnown();
      void operatorLength();
      void operatorLineto();
      void operatorLoad();
      void operatorLoop();
      void operatorMakefont();
      void operatorMakepattern();
      void operatorMarkArrayBegin();
      void operatorMarkArrayEnd();
      void operatorMarkDictionaryBegin();
      void operatorMarkDictionaryEnd();
      void operatorMarkProcedureBegin();
      void operatorMarkProcedureEnd();
      void operatorMatrix();
      void operatorMoveto();
      void operatorMul();
      void operatorNe();
      void operatorNeg();
      void operatorNewpath();
      void operatorNot();
      void operatorPop();
      void operatorPrint();
      void operatorProduct();
      void operatorPstack();
      void operatorPut();
      void operatorPutinterval();
      void operatorReadonly();
      void operatorRectclip();
      void operatorRectfill();
      void operatorRectstroke();
      void operatorRepeat();
      void operatorRlineto();
      void operatorRmoveto();
      void operatorRoll();
      void operatorRotate();
      void operatorRound();
      void operatorSave();
      void operatorScale();
      void operatorScalefont();
      void operatorSelectfont();
      void operatorSetcolor();
      void operatorSetcolorspace();
      void operatorSetdash();
      void operatorSetfont();
      void operatorSetglobal();
      void operatorSetgray();
      void operatorSetlinecap();
      void operatorSetlinejoin();
      void operatorSetlinewidth();
      void operatorSetmatrix();
      void operatorSetmiterlimit();
      void operatorSetpagedevice();
      void operatorSetuserparams();
      void operatorShow();
      void operatorShowpage();
      void operatorStandardEncoding();
      void operatorStopped();
      void operatorString();
      void operatorStroke();
      void operatorSub();
      void operatorTransform();
      void operatorTranslate();
      void operatorType();
      void operatorUndef();
      void operatorVersion();
      void operatorVmstatus();
      void operatorWhere();
      void operatorWidthshow();
      void operatorXshow();
      void operatorXyshow();
      void operatorYshow();

      void pdfOperator_apostrophe();
      void pdfOperator_quote();
      void pdfOperator_b();
      void pdfOperator_bStar();
      void pdfOperator_B();
      void pdfOperator_BStar();
      void pdfOperator_BDC();
      void pdfOperator_BI();
      void pdfOperator_BMC();
      void pdfOperator_BT();
      void pdfOperator_BX();
      void pdfOperator_c();
      void pdfOperator_cm();
      void pdfOperator_CS();
      void pdfOperator_cs();
      void pdfOperator_d();
      void pdfOperator_d0();
      void pdfOperator_d1();
      void pdfOperator_Do();
      void pdfOperator_DP();
      void pdfOperator_EI();
      void pdfOperator_EMC();
      void pdfOperator_ET();
      void pdfOperator_EX();
      void pdfOperator_fStar();
      void pdfOperator_f();
      void pdfOperator_F();
      void pdfOperator_g();
      void pdfOperator_G();
      void pdfOperator_gs();
      void pdfOperator_h();
      void pdfOperator_i();
      void pdfOperator_ID();
      void pdfOperator_j();
      void pdfOperator_J();
      void pdfOperator_k();
      void pdfOperator_K();
      void pdfOperator_m();
      void pdfOperator_M();
      void pdfOperator_MP();
      void pdfOperator_n();
      void pdfOperator_l();
      void pdfOperator_q();
      void pdfOperator_Q();
      void pdfOperator_re();
      void pdfOperator_rg();
      void pdfOperator_RG();
      void pdfOperator_ri();
      void pdfOperator_s();
      void pdfOperator_S();
      void pdfOperator_sc();
      void pdfOperator_SC();
      void pdfOperator_SCN();
      void pdfOperator_scn();
      void pdfOperator_sh();
      void pdfOperator_TStar();
      void pdfOperator_Tc();
      void pdfOperator_Td();
      void pdfOperator_TD();
      void pdfOperator_Tf();
      void pdfOperator_Th();
      void pdfOperator_Tj();
      void pdfOperator_TJ();
      void pdfOperator_TL();
      void pdfOperator_Tm();
      void pdfOperator_Tr();
      void pdfOperator_Ts();
      void pdfOperator_Tw();
      void pdfOperator_Tz();
      void pdfOperator_v();
      void pdfOperator_w();
      void pdfOperator_W();
      void pdfOperator_WStar();
      void pdfOperator_y();

   private:

      void initialize();

      void resolve();
      bool seekDefinition();
      bool seekOperator();

      void parse(char *pszBeginDelimiter,char *pszEndDelimiter,char *pStart,char **ppEnd);
      char *parseObject(char *pStart,char **pEnd);

      void parseStructureSpec(char *pStart,char **ppEnd);
      void parseComment(char *pStart,char **ppEnd);
      void parseString(char *pStart,char **ppEnd);
      void parseHexString(char *pStart,char **ppEnd);
      void parseHex85String(char *pStart,char **ppEnd);
      void parseLiteralName(char *apStart,char **ppEnd);

      std::map<long,void (__thiscall job::*)(char *pStart,char **ppEnd)> tokenProcedures;
      std::map<long,char *> antiDelimiters;

      std::list<comment *> commentStack;
      std::map<long,object *> literalNames;
      std::list<structureSpec *> structureSpecs;

      std::map<object::objectType,std::map<object::valueType,object *>> nameTypeMap;

      objectStack operandStack;
      dictionaryStack dictionaryStack;
      graphicsStateStack graphicsStateStack;

      std::list<font *> fontList;
      std::list<resource *> resourceList;
      std::list<procedure *> procedureList;

      dictionary globalDict;
      dictionary systemDict;
      dictionary userDict;
      dictionary errorDict;
      dictionary statusDict;
      dictionary pdfDict;

      booleanObject *pTrueConstant;
      booleanObject *pFalseConstant;
      object *pNullConstant;

      font *pCourier;

      name *pStringType;
      name *pArrayType;
      name *pIntegerType;
      name *pRealType;
      name *pBooleanType;
      name *pDictType;
      name *pMarkType;
      name *pNullType;
      name *pSaveType;
      name *pFontType;
      name *pOperatorType;
      name *pNameType;

      object languageLevel;

      array StandardEncoding;
      array ISOLatin1Encoding;

      matrix defaultMatrix;
      matrix *pCurrentMatrix;

      colorSpace defaultColorSpace;
      colorSpace *pCurrentColorSpace;

      long saveCount;
      long inputLineNumber;
      long procedureDefinitionLevel;

      char *collectionDelimiterPeek(char *,char **);
      char *delimiterPeek(char *,char **);
      char *token();

      bool isGlobalVM;
      bool hasExited;

      char *pStorage,*pEnd;

      PdfPage *pPdfPage;

      HDC hdcTarget;
      RECT rcWindowsClip;
      IPostScriptTakeText *pIPostScriptTakeText;

      static PdfUtility *pPdfUtility;

      static unsigned int __stdcall _execute(void *);

      friend class graphicsState;
      friend class PStoPDF;

   };
