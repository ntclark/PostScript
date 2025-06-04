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

    pSystemDict -> put("=",&job::operatorStdout);

    pSystemDict -> put("abs",&job::operatorAbs);
    pSystemDict -> put("add",&job::operatorAdd);
    pSystemDict -> put("aload",&job::operatorAload);
    pSystemDict -> put("and",&job::operatorAnd);
    pSystemDict -> put("arc",&job::operatorArc);
    pSystemDict -> put("arcn",&job::operatorArcn);
    pSystemDict -> put("array",&job::operatorArray);
    pSystemDict -> put("ashow",&job::operatorAshow);
    pSystemDict -> put("astore",&job::operatorAstore);
    pSystemDict -> put("awidthshow",&job::operatorAwidthshow);
    pSystemDict -> put("begin",&job::operatorBegin);
    pSystemDict -> put("bind",&job::operatorBind);
    pSystemDict -> put("cleartomark",&job::operatorCleartomark);
    pSystemDict -> put("clip",&job::operatorClip);
    pSystemDict -> put("closefile",&job::operatorClosefile);
    pSystemDict -> put("closepath",&job::operatorClosepath);
    pSystemDict -> put("colorimage",&job::operatorColorimage);
    pSystemDict -> put("concat",&job::operatorConcat);
    pSystemDict -> put("copy",&job::operatorCopy);
    pSystemDict -> put("cos",&job::operatorCos);
    pSystemDict -> put("countdictstack",&job::operatorCountdictstack);
    pSystemDict -> put("counttomark",&job::operatorCounttomark);
    pSystemDict -> put("currentcolorspace",&job::operatorCurrentcolorspace);
    pSystemDict -> put("currentdict",&job::operatorCurrentdict);
    pSystemDict -> put("currentfile",&job::operatorCurrentfile);
    pSystemDict -> put("currentfont",&job::operatorCurrentfont);
    pSystemDict -> put("currentglobal",&job::operatorCurrentglobal);
    pSystemDict -> put("currentlinecap",&job::operatorCurrentlinecap);
    pSystemDict -> put("currentlinejoin",&job::operatorCurrentlinejoin);
    pSystemDict -> put("currentlinewidth",&job::operatorCurrentlinewidth);
    pSystemDict -> put("currentmatrix",&job::operatorCurrentmatrix);
    pSystemDict -> put("currentmiterlimit",&job::operatorCurrentmiterlimit);
    pSystemDict -> put("currentscreen",&job::operatorCurrentscreen);
    pSystemDict -> put("curveto",&job::operatorCurveto);
    pSystemDict -> put("cvi",&job::operatorCvi);
    pSystemDict -> put("cvn",&job::operatorCvn);
    pSystemDict -> put("cvr",&job::operatorCvr);
    pSystemDict -> put("cvrs",&job::operatorCvrs);
    pSystemDict -> put("cvs",&job::operatorCvs);
    pSystemDict -> put("cvx",&job::operatorCvx);
    pSystemDict -> put("debug",&job::operatorDebug);
    pSystemDict -> put("debuglevel",&job::operatorDebuglevel);
    pSystemDict -> put("def",&job::operatorDef);
    pSystemDict -> put("definefont",&job::operatorDefinefont);
    pSystemDict -> put("defineresource",&job::operatorDefineresource);
    pSystemDict -> put("dtransform",&job::operatorDtransform);
    pSystemDict -> put("dict",&job::operatorDict);
    pSystemDict -> put("div",&job::operatorDiv);
    pSystemDict -> put("dup",&job::operatorDup);
    pSystemDict -> put("end",&job::operatorEnd);
    pSystemDict -> put("eoclip",&job::operatorEoclip);
    pSystemDict -> put("eofill",&job::operatorEofill);
    pSystemDict -> put("eq",&job::operatorEq);
    pSystemDict -> put("errordict",&job::operatorErrordict);
    pSystemDict -> put("exch",&job::operatorExch);
    pSystemDict -> put("exec",&job::operatorExec);
    pSystemDict -> put("eexec",&job::operatorEexec);
    pSystemDict -> put("executeonly",&job::operatorExecuteonly);
    pSystemDict -> put("exit",&job::operatorExit);
    pSystemDict -> put("filter",&job::operatorFilter);
    pSystemDict -> put("findfont",&job::operatorFindfont);
    pSystemDict -> put("findresource",&job::operatorFindresource);
    pSystemDict -> put("fill",&job::operatorFill);
    pSystemDict -> put("flush",&job::operatorFlush);
    pSystemDict -> put("for",&job::operatorFor);
    pSystemDict -> put("forall",&job::operatorForall);
    pSystemDict -> put("ge",&job::operatorGe);
    pSystemDict -> put("get",&job::operatorGet);
    pSystemDict -> put("grestore",&job::operatorGrestore);
    pSystemDict -> put("gsave",&job::operatorGsave);
    pSystemDict -> put("gt",&job::operatorGt);
    pSystemDict -> put("idtransform",&job::operatorIdtransform);
    pSystemDict -> put("if",&job::operatorIf);
    pSystemDict -> put("ifelse",&job::operatorIfelse);
    pSystemDict -> put("image",&job::operatorImage);
    pSystemDict -> put("imagemask",&job::operatorImagemask);
    pSystemDict -> put("index",&job::operatorIndex);
    pSystemDict -> put("initclip",&job::operatorInitclip);
    pSystemDict -> put("initgraphics",&job::operatorInitgraphics);
    pSystemDict -> put("initmatrix",&job::operatorInitmatrix);
    pSystemDict -> put("ISOLatin1Encoding",&job::operatorISOLatin1Encoding);
    pSystemDict -> put("itransform",&job::operatorItransform);
    pSystemDict -> put("known",&job::operatorKnown);
    pSystemDict -> put("le",&job::operatorLe);
    pSystemDict -> put("length",&job::operatorLength);
    pSystemDict -> put("lineto",&job::operatorLineto);
    pSystemDict -> put("load",&job::operatorLoad);
    pSystemDict -> put("loop",&job::operatorLoop);
    pSystemDict -> put("lt",&job::operatorLt);
    pSystemDict -> put("makefont",&job::operatorMakefont);
    pSystemDict -> put("makepattern",&job::operatorMakepattern);
    pSystemDict -> put("mark",&job::operatorMark);
    pSystemDict -> put(ARRAY_DELIMITER_BEGIN,&job::operatorMarkArrayBegin);
    pSystemDict -> put(ARRAY_DELIMITER_END,&job::operatorMarkArrayEnd);
    pSystemDict -> put(DICTIONARY_DELIMITER_BEGIN,&job::operatorMarkDictionaryBegin);
    pSystemDict -> put(DICTIONARY_DELIMITER_END,&job::operatorMarkDictionaryEnd);
    pSystemDict -> put(PROC_DELIMITER_BEGIN,&job::operatorMarkProcedureBegin);
    pSystemDict -> put(PROC_DELIMITER_END,&job::operatorMarkProcedureEnd);
    pSystemDict -> put("matrix",&job::operatorMatrix);
    pSystemDict -> put("moveto",&job::operatorMoveto);
    pSystemDict -> put("mul",&job::operatorMul);
    pSystemDict -> put("ne",&job::operatorNe);
    pSystemDict -> put("neg",&job::operatorNeg);
    pSystemDict -> put("newpath",&job::operatorNewpath);
    pSystemDict -> put("noaccess",&job::operatorNoaccess);
    pSystemDict -> put("not",&job::operatorNot);
    pSystemDict -> put("pop",&job::operatorPop);
    pSystemDict -> put("print",&job::operatorPrint);
    pSystemDict -> put("product",&job::operatorProduct);
    pSystemDict -> put("pstack",&job::operatorPstack);
    pSystemDict -> put("put",&job::operatorPut);
    pSystemDict -> put("putinterval",&job::operatorPutinterval);
    pSystemDict -> put("quadcurveto",&job::operatorQuadcurveto);
    pSystemDict -> put("quit",&job::operatorQuit);
    pSystemDict -> put("readonly",&job::operatorReadonly);
    pSystemDict -> put("readstring",&job::operatorReadstring);
    pSystemDict -> put("rcheck",&job::operatorRcheck);
    pSystemDict -> put("rectclip",&job::operatorRectclip);
    pSystemDict -> put("rectfill",&job::operatorRectfill);
    pSystemDict -> put("rectstroke",&job::operatorRectstroke);
    pSystemDict -> put("render",&job::operatorRender);
    pSystemDict -> put("repeat",&job::operatorRepeat);
    pSystemDict -> put("restore",&job::operatorRestore);
    pSystemDict -> put("rlineto",&job::operatorRlineto);
    pSystemDict -> put("rmoveto",&job::operatorRmoveto);
    pSystemDict -> put("roll",&job::operatorRoll);
    pSystemDict -> put("rotate",&job::operatorRotate);
    pSystemDict -> put("round",&job::operatorRound);
    pSystemDict -> put("save",&job::operatorSave);
    pSystemDict -> put("scale",&job::operatorScale);
    pSystemDict -> put("scalefont",&job::operatorScalefont);
    pSystemDict -> put("selectfont",&job::operatorSelectfont);
    pSystemDict -> put("setcachedevice",&job::operatorSetcachedevice);
    pSystemDict -> put("setcmykcolor",&job::operatorSetcmykcolor);
    pSystemDict -> put("setcolor",&job::operatorSetcolor);
    pSystemDict -> put("setcolorspace",&job::operatorSetcolorspace);
    pSystemDict -> put("setdash",&job::operatorSetdash);
    pSystemDict -> put("setfont",&job::operatorSetfont);
    pSystemDict -> put("setglobal",&job::operatorSetglobal);
    pSystemDict -> put("setgray",&job::operatorSetgray);
    pSystemDict -> put("setlinecap",&job::operatorSetlinecap);
    pSystemDict -> put("setlinejoin",&job::operatorSetlinejoin);
    pSystemDict -> put("setlinewidth",&job::operatorSetlinewidth);
    pSystemDict -> put("setmatrix",&job::operatorSetmatrix);
    pSystemDict -> put("setmiterlimit",&job::operatorSetmiterlimit);
    pSystemDict -> put("setpagedevice",&job::operatorSetpagedevice);
    pSystemDict -> put("setrgbcolor",&job::operatorSetrgbcolor);
    pSystemDict -> put("setstrokeadjust",&job::operatorSetstrokeadjust);
    pSystemDict -> put("setuserparams",&job::operatorSetuserparams);
    pSystemDict -> put("show",&job::operatorShow);
    pSystemDict -> put("showpage",&job::operatorShowpage);
    pSystemDict -> put("sin",&job::operatorSin);
    pSystemDict -> put("StandardEncoding",&job::operatorStandardEncoding);
    pSystemDict -> put("stopped",&job::operatorStopped);
    pSystemDict -> put("string",&job::operatorString);
    pSystemDict -> put("stroke",&job::operatorStroke);
    pSystemDict -> put("sub",&job::operatorSub);
    pSystemDict -> put("transform",&job::operatorTransform);
    pSystemDict -> put("translate",&job::operatorTranslate);
    pSystemDict -> put("type",&job::operatorType);
    pSystemDict -> put("undef",&job::operatorUndef);
    pSystemDict -> put("undefinefont",&job::operatorUndefinefont);
    pSystemDict -> put("undefineresource",&job::operatorUndefineresource);
    pSystemDict -> put("version",&job::operatorVersion);
    pSystemDict -> put("vmstatus",&job::operatorVmstatus);
    pSystemDict -> put("where",&job::operatorWhere);
    pSystemDict -> put("widthshow",&job::operatorWidthshow);
    pSystemDict -> put("xshow",&job::operatorXshow);
    pSystemDict -> put("xyshow",&job::operatorXyshow);
    pSystemDict -> put("yshow",&job::operatorYshow);

