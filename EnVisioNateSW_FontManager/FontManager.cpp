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

#define DEFINE_DATA

#include "EnVisioNateSW_FontManager.h"

#include "FontManager_i.c"

#include "Renderer_i.c"

    IRenderer *FontManager::pIRenderer = NULL;
    IGraphicElements *FontManager::pIGraphicElements = NULL;

    IFont_EVNSW *FontManager::pIFont_Current = NULL;
    std::list<font *> FontManager::managedFonts;

    char FontManager::szFailureMessage[]{'\0'};
    FontManager::_IConnectionPointContainer *FontManager::pIConnectionPointContainer = NULL;

    FontManager::FontManager(IUnknown *pUnkOuter) {
    if ( NULL == pIRenderer ) {
        HRESULT rc = CoCreateInstance(CLSID_Renderer,NULL,CLSCTX_ALL,IID_IRenderer,reinterpret_cast<void **>(&pIRenderer));
        pIRenderer -> QueryInterface(IID_IGraphicElements,reinterpret_cast<void **>(&pIGraphicElements));
    }
    pIConnectionPointContainer = new _IConnectionPointContainer(this);
    pIConnectionPoint = new _IConnectionPoint(this);
    return;
    }


    FontManager::~FontManager() {

    Reset();

    delete pIConnectionPointContainer;
    delete pIConnectionPoint;

    pIConnectionPointContainer = NULL;
    pIConnectionPoint = NULL;

    if ( ! ( NULL == pIEnumConnectionPoints ) )
        delete pIEnumConnectionPoints;

    pIEnumConnectionPoints = NULL;

    if ( ! ( NULL == pIEnumerateConnections ) )
        delete pIEnumerateConnections;

    pIEnumerateConnections = NULL;

    pIGraphicElements -> Release();
    pIRenderer -> Release();

    pIGraphicElements = NULL;
    pIRenderer = NULL;

    font::adobeGlyphList.clear();

    return;
    }


    void substName(char *pszSource,char *pszTarget,char *pszReplacement);

    char *FontManager::translateFamily(char *pszFamilyName,char *pszReturnedQualifier) {

    static char szSanitizedName[256];
    memset(szSanitizedName,0,256 * sizeof(char));

    if ( ! ( NULL == pszReturnedQualifier ) )
        pszReturnedQualifier[0] = '\0';

    strcpy(szSanitizedName,pszFamilyName);

    substName(szSanitizedName,(char *)"Courier",(char *)"Courier New");
    substName(szSanitizedName,(char *)"Helvetica",(char *)"Arial");
    substName(szSanitizedName,(char *)"Times-Roman",(char *)"Times New Roman");

    if ( strchr(szSanitizedName,'-') ) {
        char *p = strchr(szSanitizedName,'-');
        if ( ! ( NULL == pszReturnedQualifier ) )
            strcpy(pszReturnedQualifier,p + 1);
        *p = '\0';
    }

    return szSanitizedName;
    }


    void substName(char *pszSource,char *pszTarget,char *pszReplacement) {

    char *p = strstr(pszSource,pszTarget);

    if ( 0 == strcmp(pszSource,pszTarget) ) {
        strcpy(pszSource,pszReplacement);
        return;
    }

    while ( p ) {
        long n = (long)strlen(p + strlen(pszTarget)) + 1;
        char *pszRemainder = new char[n];
        pszRemainder[n - 1] = '\0';
        strcpy(pszRemainder,p + strlen(pszTarget));
        sprintf(p,"%s%s",pszReplacement,pszRemainder);
        p = strstr(pszSource,pszTarget);
    }

    return;
    }