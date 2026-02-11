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

#include "Renderer.h"

    HRESULT RendererNoWindow::QueryInterface(REFIID refIID,void **pvResult) {

    if ( ! pvResult )
        return E_POINTER;

    if ( IID_IRenderer == refIID ) {
        *pvResult = static_cast<IRenderer *>(this);
        return S_OK;
    }

    return pMainRenderer -> QueryInterface(refIID,pvResult);
    }


    ULONG RendererNoWindow::AddRef() {
    return pMainRenderer -> AddRef();
    }


    ULONG RendererNoWindow::Release() {
    return pMainRenderer -> Release();
    }


    HRESULT RendererNoWindow::put_ToDeviceTransform(UINT_PTR pXFormToDeviceSpace) {
    memcpy(&pMainRenderer -> pIGraphicElements -> toDeviceSpace,(void *)pXFormToDeviceSpace,sizeof(XFORM));
    pMainRenderer -> pIGraphicElements -> calcInverseTransform(&pMainRenderer -> pIGraphicElements -> toDeviceSpace,&pMainRenderer -> pIGraphicElements -> toDeviceInverse);
    return S_OK;
    }


    HRESULT RendererNoWindow::put_ToPageTransform(UINT_PTR pvXForm) {
    XFORM *pXForm = (XFORM *)pvXForm;
    memcpy(&pMainRenderer -> pGraphicsStateManager -> parametersStack.top() -> toPageSpace,(void *)pXForm,sizeof(XFORM));
    pMainRenderer -> pGraphicsStateManager -> parametersStack.top() -> hasXForm = ! 
        (1.0f == pXForm -> eM11 && 0.0f == pXForm -> eM12 && 0.0f == pXForm -> eM21 && 1.0f == pXForm -> eM22 &&
                0.0f == pXForm -> eDx && 0.0f == pXForm -> eDy);
    return S_OK;
    }


    HRESULT RendererNoWindow::put_ScaleTransform(UINT_PTR pvXForm) {
    XFORM *pXForm = (XFORM *)pvXForm;
    memcpy(&pMainRenderer -> pGraphicsStateManager -> parametersStack.top() -> scaleXForm,(void *)pXForm,sizeof(XFORM));
    pMainRenderer -> pGraphicsStateManager -> parametersStack.top() -> hasScale = ! (1.0f == pXForm -> eM11 && 1.0f == pXForm -> eM22);
    return S_OK;
    }


    HRESULT RendererNoWindow::put_DownScale(FLOAT ds) {
    pMainRenderer -> pGraphicsStateManager -> parametersStack.top() -> downScale = ds; 
    pMainRenderer -> pGraphicsStateManager -> parametersStack.top() -> hasDownScale = ! ( 1.0f == ds );
    return S_OK;
    }


    HRESULT RendererNoWindow::put_Origin(POINTF ptOrigin) { 
    pMainRenderer -> pGraphicsStateManager -> parametersStack.top() -> origin = ptOrigin;
    return S_OK;
    }


    HRESULT RendererNoWindow::SetRenderLive(HDC,RECT *) {
    return S_OK;
    }


    HRESULT RendererNoWindow::UnSetRenderLive() {
    pMainRenderer -> renderLive = false;
    return S_OK;
    }


    HRESULT RendererNoWindow::Render(HDC,RECT *) {
    return S_OK;
    }


    HRESULT RendererNoWindow::Discard() {
    if ( NULL == pMainRenderer -> pIGraphicElements -> pFirstPath )
        return S_OK;
    Renderer::GraphicElements::path *p = pMainRenderer -> pIGraphicElements -> pFirstPath;
    while ( ! ( p == NULL ) ) {
        Renderer::GraphicElements::path *pNext = p -> pNextPath;
        delete p;
        p = pNext;
    }
    pMainRenderer -> pIGraphicElements -> pFirstPath = NULL;
    pMainRenderer -> pIGraphicElements -> pCurrentPath = NULL;
    return S_OK;
    }


    HRESULT RendererNoWindow::ClearRect(HDC,RECT *,COLORREF) {
    memset(pMainRenderer -> szStatusMessage,0,1024 * sizeof(char));
    memset(pMainRenderer -> szErrorMessage,0,1024 * sizeof(char));
    pMainRenderer -> pIConnectionPointContainer -> fire_Clear();
    return S_OK;
    }


    HRESULT RendererNoWindow::WhereAmI(long xPixels,long yPixels,FLOAT *pX,FLOAT *pY) {

    if ( ! pX && ! pY )
        return E_POINTER;

    POINTF ptPixels{(FLOAT)xPixels,(FLOAT)yPixels};
    pMainRenderer -> pIGraphicElements -> transformPoint(&pMainRenderer -> pGraphicsStateManager -> parametersStack.top() -> toPageSpace,&ptPixels,&ptPixels);

    if ( pX ) 
        *pX = ptPixels.x;

    if ( pY )
        *pY = ptPixels.y;

    return S_OK;
    }


    HRESULT RendererNoWindow::SaveState() {
    return pMainRenderer -> pGraphicsStateManager -> Save();
    }


    HRESULT RendererNoWindow::RestoreState() {
    return pMainRenderer -> pGraphicsStateManager -> Restore();
    }


    UINT_PTR RendererNoWindow::SetNoGraphicsRendering() {
    return reinterpret_cast<UINT_PTR>(this);
    }