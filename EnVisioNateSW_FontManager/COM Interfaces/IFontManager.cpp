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

#include "FontManager.h"

    HRESULT FontManager::SeekFont(char *pszFamilyName,long dupCount,IFont_EVNSW **ppFont) {

    if ( ! ppFont )
        return E_POINTER;

    for ( font *pFont : managedFonts ) {
        if ( 0 == strcmp(pFont -> szClientFamily,pszFamilyName) && strlen(pFont -> szClientFamily) == strlen(pszFamilyName) ) {
            if ( -1 == dupCount || ( pFont -> dupCount == dupCount ) ) {
                pFont -> QueryInterface(IID_IFont_EVNSW,reinterpret_cast<void **>(ppFont));
                return S_OK;
            }
        }
    }

    return E_FAIL;
    }


    static int cbFontStyles = 0;

    int CALLBACK FontManager::enumFonts(const LOGFONT *pLogFont,const TEXTMETRIC *pTextMetric,DWORD FontType,LPARAM lParam) {

    ENUMLOGFONTEX *pEnumLogFont = reinterpret_cast<ENUMLOGFONTEX *>(const_cast<LOGFONT *>(pLogFont));

    font *pFont = reinterpret_cast<font *>(lParam);

    char *p = new char[strlen((char *)(pEnumLogFont -> elfFullName)) + 1];
    strcpy(p,(char *)(pEnumLogFont -> elfFullName));
    pFont -> fontFullNames.push_back(p);

    p = new char[strlen((char *)(pEnumLogFont -> elfStyle)) + 1];
    strcpy(p,(char *)(pEnumLogFont -> elfStyle));
    pFont -> fontStyleNames.push_back(p);

    p = new char[strlen((char *)(pEnumLogFont -> elfScript)) + 1];
    strcpy(p,(char *)(pEnumLogFont -> elfScript));
    pFont -> fontScriptNames.push_back(p);

    pFont -> fontWeights.push_back(pLogFont -> lfWeight);

    return 1;
    }


    HRESULT FontManager::LoadFont(char *pszFamilyName,UINT_PTR cookie,IFont_EVNSW **ppFont) {

    if ( ! ppFont )
        return E_POINTER;

    if ( S_OK == SeekFont(pszFamilyName,-1L,ppFont) )
        return S_OK;

    font *pFont = new font(pszFamilyName);

    strcpy(pFont -> szInstalledFamily,translateFamily(pszFamilyName,pFont -> szInstalledQualifier));

    *ppFont = NULL;

    LOGFONT logFont{0};
    strcpy(logFont.lfFaceName,pFont -> InstalledFontName());

    HFONT hFont = CreateFontIndirect(&logFont);

    pFont -> cookie = cookie;

    if ( NULL == hFont )
        goto NonType42Font;

    {

    HDC hdcTemp = CreateCompatibleDC(NULL);

    HGDIOBJ oldFont = SelectFont(hdcTemp,hFont);

    DWORD cbData = GetFontData(hdcTemp,/*0x66637474*/0L,0L,NULL,0L);

    if ( GDI_ERROR == cbData ) {
        sprintf_s<1024>(FontManager::szFailureMessage,"The specified font (%s) is (probably) a TrueType or OpenType font, however, the GetFontData call returns failure on it",pFont -> InstalledFontName());
        FireErrorNotification(FontManager::szFailureMessage);
        goto NonType42Font;
    }

    memset(szFailureMessage,0,sizeof(szFailureMessage));

    FireErrorNotification(FontManager::szFailureMessage);

    pFont -> cbFontData = cbData;

    pFont -> pbFontData = new BYTE[cbData];

    GetFontData(hdcTemp,0L,0L,pFont -> pbFontData,cbData);

    SelectFont(hdcTemp,oldFont);

    logFont.lfCharSet = DEFAULT_CHARSET;
    logFont.lfPitchAndFamily = 0;

    EnumFontFamiliesEx(hdcTemp,&logFont,enumFonts,reinterpret_cast<LPARAM>(pFont),0L);

    int index = 0;
    for ( char *pfName : pFont -> fontFullNames ) {
        if ( 0 == strcmp(pFont -> InstalledFontName(),pfName) && strlen(pfName) == strlen(pFont -> InstalledFontName()) ) {
            strcpy(pFont -> szSelectedFontFullName,pfName);
            strcpy(pFont -> szSelectedFontStyleName,pFont -> fontStyleNames[index]);
            strcpy(pFont -> szSelectedFontScriptName,pFont -> fontScriptNames[index]);
            pFont -> selectedFontWeight = pFont -> fontWeights[index];
            break;
        }
        index++;
    }

    DeleteDC(hdcTemp);

    DeleteObject(hFont);

    pFont -> theFontType = FontType::type42;

    if ( ! ( S_OK == pFont -> type42Load(pFont -> pbFontData) ) ) 
        goto NonType42Font;

    }

    goto Type42Font;

NonType42Font:

    pFont -> theFontType = FontType::typeUnspecified;

Type42Font:

    managedFonts.push_back(pFont);

    pFont -> QueryInterface(IID_IFont_EVNSW,reinterpret_cast<void **>(ppFont));

    pIFont_Current = *ppFont;
    pIFont_Current -> AddRef();

    return S_OK;
    }


    HRESULT FontManager::DuplicateFont(IFont_EVNSW *pIFont,UINT_PTR cookie,IFont_EVNSW **ppIFont) {

    if ( ! ppIFont )
        return E_POINTER;

    if ( NULL == pIFont )
        return E_FAIL;

    font *pFontRHS = (static_cast<font *>(pIFont));

    font *pFont = new font(*pFontRHS);

    pFont -> cookie = cookie;

    pFont -> QueryInterface(IID_IFont_EVNSW,reinterpret_cast<void **>(ppIFont));

    managedFonts.push_back(pFont);

    return S_OK;
    }


    HRESULT FontManager::ChooseFont(HDC hdc,IFont_EVNSW **ppIFont) {

    if ( NULL == ppIFont )
        return E_POINTER;

    CHOOSEFONT chooseFont{0};
    LOGFONT logFont{0};

    chooseFont.lStructSize = sizeof(CHOOSEFONT);
    chooseFont.lpLogFont = &logFont;
    chooseFont.Flags = CF_TTONLY;

    if ( FALSE == ::ChooseFont(&chooseFont) )
        return S_FALSE;

    HRESULT rc = LoadFont(chooseFont.lpLogFont -> lfFaceName,0,ppIFont);

    if ( ! ( rc == S_OK ) )
        return rc;

    font *pFont = reinterpret_cast<font *>(*ppIFont);

    long pointSize = chooseFont.lpLogFont -> lfHeight;
    if ( 0 > pointSize )
        pointSize = MulDiv(-pointSize, 72, GetDeviceCaps(hdc, LOGPIXELSY));

    pFont -> PointSize((FLOAT)pointSize);

    return S_OK;
    }


    HRESULT FontManager::RenderGlyph(unsigned short bGlyph,UINT_PTR pPSXform,UINT_PTR pXformToDeviceSpace,POINTF *pStartPoint,POINTF *pEndPoint) {
    font *pFont = static_cast<font *>(pIFont_Current);
    return pFont -> RenderGlyph(bGlyph,pPSXform,pXformToDeviceSpace,pStartPoint,pEndPoint);
    }


    HRESULT FontManager::ScaleFont(FLOAT scaleFactor) {
    pIFont_Current -> Scale(scaleFactor,scaleFactor);
    return S_OK;
    }


    HRESULT FontManager::put_PointSize(FLOAT pointSize) {
    font *pFont = static_cast<font *>(pIFont_Current);
    pFont -> PointSize(pointSize);
    pFont -> matrixStack.top() -> XForm() -> eM11 = pointSize;
    pFont -> matrixStack.top() -> XForm() -> eM22 = pointSize;
    return S_OK;
    }


    HRESULT FontManager::get_CurrentFont(IFont_EVNSW **ppIFont) {
    if ( ! ppIFont )
        return E_POINTER;
    *ppIFont = pIFont_Current;
    return S_OK;
    }


    HRESULT FontManager::put_CurrentFont(IFont_EVNSW *pIFont) {
    pIFont_Current = pIFont;
    return S_OK;
    }