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

/*
    See Appendix 2 in the document "Adobe Type 1 Font Format"
        Copyright © 1990 Adobe Systems Incorporated.
        page 89
*/

std::map<uint8_t,char *> AdobeType1Fonts::type1CharStringCommands {
    {0,"error"},{1,"hstem"},{3,"vstem"},{4,"vmoveto"},{5,"rlineto"},{6,"hlineto"},{7,"vlineto"},
    {8,"rrcurveto"},{9,"closepath"},{10,"callsubr"},{11,"return"},{13,"hsbw"},{14,"endchar"},
    {16,"blend"},{18,"hstemhm"},{19,"hintmask"},{20,"cntrmask"},{21,"rmoveto"},{22,"hmoveto"},
    {23,"vstemhm"},{24,"rcurveline"},{25,"rlinecurve"},{26,"vvcurveto"},{27,"hhcurveto"},
    {29,"callgsubr"},{30,"vhcurveto"},{31,"hvcurveto"},{99,"<invalid command>"}
};

std::map<uint8_t,char *> AdobeType1Fonts::type1CharStringSubCommands {
    {0,"dotsection"},{1,"vstem3"},{2,"hstem3"},{3,"and"},{4,"or"},{5,"not"},{6,"seac"},
    {7,"sbw"},{8,"store"},{9,"abs"},{10,"add"},{11,"sub"},{12,"div"},{13,"load"},{14,"neg"},
    {15,"eq"},{16,"callothersubr"},{17,"pop"},{18,"drop"},{20,"put"},{21,"get"},{22,"ifelse"},
    {23,"random"},{24,"mul"},{26,"sqrt"},{27,"dup"},{28,"exch"},{29,"index"},{30,"roll"},
    {33,"setcurrentpoint"},{34,"hflex"},{35,"flex"},
    {36,"hflex1"},{37,"flex1"},{99,"<invalid subcommand>"}
};

    job *AdobeType1Fonts::pJob = NULL;
    dictionary *AdobeType1Fonts::pOperators = NULL;
    IRenderer *AdobeType1Fonts::pIRenderer = NULL;
    IGraphicElements *AdobeType1Fonts::pIGraphicElements = NULL;

    object *AdobeType1Fonts::pZeroValue = NULL;
    objectStack *AdobeType1Fonts::pOperandStack = NULL;
    type1Glyph *AdobeType1Fonts::pType1Glyph = NULL;

    void AdobeType1Fonts::Initialize(job *pj,IRenderer *pir,IGraphicElements *pige) {

    if ( ! ( NULL == pJob ) ) 
        Shutdown();

    if ( NULL == pOperandStack ) 
        pOperandStack = new objectStack();

    pJob = pj;

    pIRenderer = pir;
    pIRenderer -> AddRef();

    pIGraphicElements = pige;
    pIGraphicElements -> AddRef();

    pZeroValue = new (pJob -> CurrentObjectHeap()) object(pJob,0.0f);

    pOperators = new (pJob -> CurrentObjectHeap()) dictionary(pJob,128);

    pOperators -> put("blend",&AdobeType1Fonts::blend);
    pOperators -> put("callgsubr",&AdobeType1Fonts::callgsubr);
    pOperators -> put("callsubr",&AdobeType1Fonts::callsubr);
    pOperators -> put("closepath",&AdobeType1Fonts::closepath);
    pOperators -> put("cntrmask",&AdobeType1Fonts::cntrmask);
    pOperators -> put("endchar",&AdobeType1Fonts::endchar);
    pOperators -> put("error",&AdobeType1Fonts::error);
    pOperators -> put("escape",&AdobeType1Fonts::escape);
    pOperators -> put("hhcurveto",&AdobeType1Fonts::hhcurveto);
    pOperators -> put("hintmask",&AdobeType1Fonts::hintmask);
    pOperators -> put("hlineto",&AdobeType1Fonts::hlineto);
    pOperators -> put("hmoveto",&AdobeType1Fonts::hmoveto);
    pOperators -> put("hsbw",&AdobeType1Fonts::hsbw);
    pOperators -> put("hstem",&AdobeType1Fonts::hstem);
    pOperators -> put("hstemhm",&AdobeType1Fonts::hstemhm);
    pOperators -> put("hvcurveto",&AdobeType1Fonts::hvcurveto);
    pOperators -> put("rcurveline",&AdobeType1Fonts::rcurveline);
    pOperators -> put("return",&AdobeType1Fonts::operatorReturn);
    pOperators -> put("rlinecurve",&AdobeType1Fonts::rlinecurve);
    pOperators -> put("rlineto",&AdobeType1Fonts::rlineto);
    pOperators -> put("rmoveto",&AdobeType1Fonts::rmoveto);
    pOperators -> put("rrcurveto",&AdobeType1Fonts::rrcurveto);
    pOperators -> put("vhcurveto",&AdobeType1Fonts::vhcurveto);
    pOperators -> put("vlineto",&AdobeType1Fonts::vlineto);
    pOperators -> put("vmoveto",&AdobeType1Fonts::vmoveto);
    pOperators -> put("vstem",&AdobeType1Fonts::vstem);
    pOperators -> put("vstemhm",&AdobeType1Fonts::vstemhm);
    pOperators -> put("vvcurveto",&AdobeType1Fonts::vvcurveto);

    pOperators -> put("abs",&AdobeType1Fonts::abs);
    pOperators -> put("add",&AdobeType1Fonts::add);
    pOperators -> put("and",&AdobeType1Fonts::and);
    pOperators -> put("callothersubr",&AdobeType1Fonts::callothersubr);
    pOperators -> put("div",&AdobeType1Fonts::div);
    pOperators -> put("dotsection",&AdobeType1Fonts::dotsection);
    pOperators -> put("drop",&AdobeType1Fonts::drop);
    pOperators -> put("dup",&AdobeType1Fonts::dup);
    pOperators -> put("eq",&AdobeType1Fonts::eq);
    pOperators -> put("exch",&AdobeType1Fonts::exch);
    pOperators -> put("flex",&AdobeType1Fonts::flex);
    pOperators -> put("flex1",&AdobeType1Fonts::flex1);
    pOperators -> put("get",&AdobeType1Fonts::get);
    pOperators -> put("hflex",&AdobeType1Fonts::hflex);
    pOperators -> put("hflex1",&AdobeType1Fonts::hflex1);
    pOperators -> put("hstem3",&AdobeType1Fonts::hstem3);
    pOperators -> put("ifelse",&AdobeType1Fonts::ifelse);
    pOperators -> put("index",&AdobeType1Fonts::index);
    pOperators -> put("load",&AdobeType1Fonts::load);
    pOperators -> put("mul",&AdobeType1Fonts::mul);
    pOperators -> put("neg",&AdobeType1Fonts::neg);
    pOperators -> put("not",&AdobeType1Fonts::not);
    pOperators -> put("or",&AdobeType1Fonts::or);
    pOperators -> put("pop",&AdobeType1Fonts::pop);
    pOperators -> put("put",&AdobeType1Fonts::put);
    pOperators -> put("random",&AdobeType1Fonts::random);
    pOperators -> put("roll",&AdobeType1Fonts::roll);
    pOperators -> put("sbw",&AdobeType1Fonts::sbw);
    pOperators -> put("seac",&AdobeType1Fonts::seac);
    pOperators -> put("setcurrentpoint",&AdobeType1Fonts::setcurrentpoint);
    pOperators -> put("sqrt",&AdobeType1Fonts::sqrt);
    pOperators -> put("store",&AdobeType1Fonts::store);
    pOperators -> put("sub",&AdobeType1Fonts::sub);
    pOperators -> put("vstem3",&AdobeType1Fonts::vstem3);

    pOperators -> put("<invalid command>",&AdobeType1Fonts::invalidCommand);
    pOperators -> put("<invalid subcommand>",&AdobeType1Fonts::invalidCommand);

    return;
    }


    void AdobeType1Fonts::Shutdown() {

    if ( ! ( NULL == pIRenderer ) )
        pIRenderer -> Release();

    if ( ! ( NULL == pIGraphicElements ) )
        pIGraphicElements -> Release();

    return;
    }