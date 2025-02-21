
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

    DWORD cbData = GetFontData(hdcTemp,0L,0L,NULL,0L);

    if ( GDI_ERROR == cbData ) 
        goto NonType42Font;

    pFont -> cbFontData = cbData;

    pFont -> pbFontData = new BYTE[cbData];

    GetFontData(hdcTemp,0L,0L,pFont -> pbFontData,cbData);

    SelectFont(hdcTemp,oldFont);

    DeleteDC(hdcTemp);

    pFont -> fontType = font::ftype42;

    if ( ! ( S_OK == pFont -> type42Load(pFont -> pbFontData) ) ) 
        goto NonType42Font;

    }

    goto Type42Font;

NonType42Font:

    pFont -> fontType = font::ftypeUnspecified;

Type42Font:

    managedFonts.push_back(pFont);

    pFont -> QueryInterface(IID_IFont_EVNSW,reinterpret_cast<void **>(ppFont));

    if ( NULL == pIFont_Current ) {
        pIFont_Current = *ppFont;
        pIFont_Current -> AddRef();
    }

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