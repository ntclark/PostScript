
#include "FontManager.h"

    HRESULT font::QueryInterface(REFIID refIID,void **pvResult) {

    if ( ! pvResult )
        return E_POINTER;

    *pvResult = NULL;

    if ( IID_IUnknown == refIID ) 
        *pvResult = static_cast<IUnknown *>(this);

    else if ( IID_IFont_EVNSW == refIID )
        *pvResult = static_cast<IFont_EVNSW *>(this);

    if ( * pvResult ) {
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
    }


    ULONG font::AddRef() {
    return ++refCount;
    }


    ULONG font::Release() {
    if ( 1 == refCount ) {
        delete this;
        return 0;
    }
    return --refCount;
    }


    HRESULT font::put_Matrix(UINT_PTR pMatrix) {
    matrixStack.top() -> SetMatrix((XFORM *)pMatrix);
    return S_OK;
    }


    HRESULT font::get_Matrix(LPVOID pResult) {
    if ( ! pResult )
        return E_POINTER;
    memcpy((void *)pResult,matrixStack.top() -> XForm(),sizeof(XFORM));
    return S_OK;
    }


    HRESULT font::Scale(FLOAT scaleX,FLOAT scaleY) {
    XFORM scaleMatrix{scaleX,0.0f,0.0f,scaleY,0.0f,0.0f};
    ConcatMatrix((UINT_PTR)&scaleMatrix);
    PointSize(scaleX * PointSize());
    currentScale *= scaleX;
    return S_OK;
    }


    HRESULT font::Translate(FLOAT translateX,FLOAT translateY) {
    XFORM xForm{1.0f,0.0f,0.0f,1.0f,translateX,translateY};
    ConcatMatrix((UINT_PTR)&xForm);
    return S_OK;
    }


    HRESULT font::ConcatMatrix(UINT_PTR pXForm) {
    XFORM result{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};
    CombineTransform(&result,(XFORM *)pXForm,(XFORM *)matrixStack.top() -> XForm());
    memcpy(matrixStack.top() -> XForm(),&result,sizeof(XFORM));
    return S_OK;
    }


    HRESULT font::FontName(long cb,char *pszName) {
    strncpy(pszName,szFamily,cb);
    return S_OK;
    }


    HRESULT font::get_FontCookie(UINT_PTR *pCookie) {
    if ( ! pCookie )
        return E_POINTER;
    *pCookie = cookie;
    return S_OK;
    }


    HRESULT font::get_FontType(int *pFontType) {
    if ( ! pFontType )
        return E_POINTER;
    *pFontType = (int)fontType;
    return S_OK;
    }


    HRESULT font::get_GlyphId(unsigned char charCode,int *pGlyphId) {
    if ( ! pGlyphId )
        return E_POINTER;
    *pGlyphId = glyphIDMap[charCode];
    return S_OK;
    }


    HRESULT font::SaveState() {
    matrixStack.push(new matrix(*matrixStack.top()));
    return S_OK;
    }


    HRESULT font::RestoreState() {
    if ( 1 == matrixStack.size() ) {
        matrixStack.top() -> identity();
        currentScale = 1.0f;
        PointSize(1.0f);
        return S_OK;
    }
    FLOAT scale = matrixStack.top() -> XForm() -> eM11;
    delete matrixStack.top();
    matrixStack.pop();
    FLOAT restoredScale = matrixStack.top() -> XForm() -> eM11;
    PointSize(restoredScale * PointSize() / scale);
    currentScale = restoredScale;
    return S_OK;
    }