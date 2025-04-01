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
#include "PostScript objects\colorSpace.h"

    colorSpace::colorSpace(job *pj,object *po) :
        parameterCount(0),
        array(pj,po -> Name())
    {
    if ( object::objTypeMatrix == po -> ObjectType() ) {
        __debugbreak();
    }
    pFamilyName = po -> getElement(0);
    putElement(0,pFamilyName);
    if ( 0 == strcmp(pFamilyName -> Contents(),"Indexed") ) {
        array *pArray = reinterpret_cast<array *>(po -> getElement(1));
        if ( strcmp(pArray -> getElement(0) -> Name(),"Indexed") )
            pBaseName = pArray -> getElement(0);
        else
            pBaseName = pArray -> getElement(1);
        hiVal = (uint16_t)po -> getElement(2) -> IntValue();
        pLookup = po -> getElement(3);
        putElement(1,pBaseName);
    }
    parameterCount = size() - 1;
    return;
    }

    colorSpace::colorSpace(job *pj,char *pszName) :
        parameterCount(0),
        array(pj,pszName)
    {
    pFamilyName = new (pJob -> CurrentObjectHeap()) object(pj,pszName);
    putElement(0,pFamilyName);
    return;
    }


    colorSpace::~colorSpace() {
    return;
    }


    void colorSpace::setColor() {

    long size = ParameterCount();

    clear();

    insert(pFamilyName);

    for ( long k = size; k > 0; k-- )
        putElement(k,pJob -> pop());

    return;
    }


    long colorSpace::ParameterCount() {

    if ( 0 == strcmp(pFamilyName -> Contents(),"DeviceGray") )
        return 1;

    if ( 0 == strcmp(pFamilyName -> Contents(),"DeviceRGB") ) 
        return 3;

    if ( 0 == strcmp(pFamilyName -> Contents(),"Indexed") ) {
        if ( 0 == strcmp(pBaseName -> Contents(),"DeviceGray") )
            return 1;
        if ( 0 == strcmp(pBaseName -> Contents(),"DeviceRGB") )
            return 3;
    }

    char szMessage[1024];
    sprintf_s<1024>(szMessage,"Error: ColorSpace family %s is not implemented",pFamilyName -> Contents());
    //throw notimplemented(szMessage);
    return -1;
    }

