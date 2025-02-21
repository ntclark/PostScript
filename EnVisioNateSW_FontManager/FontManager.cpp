
#define DEFINE_DATA

#include "EnVisioNateSW_FontManager.h"

#include "FontManager_i.c"

#include "Renderer_i.c"

    IRenderer *FontManager::pIRenderer = NULL;
    IGraphicElements *FontManager::pIGraphicElements = NULL;

    IFont_EVNSW *FontManager::pIFont_Current = NULL;
    std::list<font *> FontManager::managedFonts;

    FontManager::FontManager(IUnknown *pUnkOuter) {
    if ( NULL == pIRenderer ) {
        CoCreateInstance(CLSID_Renderer,NULL,CLSCTX_ALL,IID_IRenderer,reinterpret_cast<void **>(&pIRenderer));
        pIRenderer -> QueryInterface(IID_IGraphicElements,reinterpret_cast<void **>(&pIGraphicElements));
    }
    return;
    }


    FontManager::~FontManager() {
    for ( font *pFont : managedFonts)
        delete pFont;
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