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

    HRESULT Renderer::put_ToDeviceTransform(UINT_PTR pXFormToDeviceSpace) {
    memcpy(&pIGraphicElements -> toDeviceSpace,(void *)pXFormToDeviceSpace,sizeof(XFORM));
    pIGraphicElements -> calcInverseTransform(&pIGraphicElements -> toDeviceSpace,&pIGraphicElements -> toDeviceInverse);
    return S_OK;
    }


    HRESULT Renderer::put_ToPageTransform(UINT_PTR pvXForm) {
    XFORM *pXForm = (XFORM *)pvXForm;
    memcpy(&pGraphicsStateManager -> parametersStack.top() -> toPageSpace,(void *)pXForm,sizeof(XFORM));
    pGraphicsStateManager -> parametersStack.top() -> hasXForm = ! 
        (1.0f == pXForm -> eM11 && 0.0f == pXForm -> eM12 && 0.0f == pXForm -> eM21 && 1.0f == pXForm -> eM22 &&
                0.0f == pXForm -> eDx && 0.0f == pXForm -> eDy);
    return S_OK;
    }


    HRESULT Renderer::put_ScaleTransform(UINT_PTR pvXForm) {
    XFORM *pXForm = (XFORM *)pvXForm;
    memcpy(&pGraphicsStateManager -> parametersStack.top() -> scaleXForm,(void *)pXForm,sizeof(XFORM));
    pGraphicsStateManager -> parametersStack.top() -> hasScale = ! (1.0f == pXForm -> eM11 && 1.0f == pXForm -> eM22);
    return S_OK;
    }


    HRESULT Renderer::put_DownScale(FLOAT ds) {
    pGraphicsStateManager -> parametersStack.top() -> downScale = ds; 
    pGraphicsStateManager -> parametersStack.top() -> hasDownScale = ! ( 1.0f == ds );
    return S_OK;
    }


    HRESULT Renderer::put_Origin(POINTF ptOrigin) { 
    pGraphicsStateManager -> parametersStack.top() -> origin = ptOrigin;
    return S_OK;
    }


    HRESULT Renderer::SetRenderLive(HDC hdc,RECT *pDrawingRect) {
    setupRenderer(hdc,pDrawingRect);
    renderLive = true;
    return S_OK;
    }


    HRESULT Renderer::UnSetRenderLive() {
    renderLive = false;
    return S_OK;
    }


    HRESULT Renderer::Render(HDC hdc,RECT *pDrawingRect) {

    if ( NULL == pIGraphicElements -> pFirstPath )
        return E_UNEXPECTED;

    setupRenderer(hdc,pDrawingRect);

    setupPathAndSink();

    long oldHash = 0;
    GraphicElements::path *pPrior = NULL;
    GraphicElements::path *p = pIGraphicElements -> pFirstPath;

    boolean oldIsFillPath = p -> isFillPath;

    while ( ! ( NULL == p ) ) {

        if ( p -> pFirstPrimitive == p -> pLastPrimitive ) {
            p = p -> pNextPath;
            continue;
        }

        if ( ! ( GraphicElements::primitive::type::strokePathMarker == p -> pLastPrimitive -> theType ) && 
                ! ( GraphicElements::primitive::type::fillPathMarker == p -> pLastPrimitive -> theType ) && 
                   ! ( GraphicElements::primitive::type::closePathMarker == p -> pLastPrimitive -> theType ) ) {
            sprintf(Renderer::szStatusMessage,"Warning: There was an unclosed path.\n"
                                                    "All paths must be closed with either of stroke, fill, or close.\n"
                                                    "This graphics element will not appear.");
            pIConnectionPointContainer -> fire_ErrorNotification(Renderer::szStatusMessage);
            p = p -> pNextPath;
            continue;
        }

        if ( ! ( oldHash == p -> hashCode) || ! ( oldIsFillPath == p -> isFillPath ) ) {
            if ( 0 < oldHash ) {
                closeSink();
                fillRender(pPrior);
                shutdownPathAndSink();
                setupPathAndSink();
            }
            pIGraphicParameters -> resetParameters(p -> szLineSettings);
            oldHash = p -> hashCode;
            oldIsFillPath = p -> isFillPath;
            pPrior = p;
        }

        GraphicElements::path::pathAction pa = p -> apply(p -> isFillPath);

        p -> clear();

        p = p -> pNextPath;

    }

    closeSink();

    if ( ! ( NULL == pPrior ) )
        fillRender(pPrior);

    for ( GraphicElements::image *pImage: pIGraphicElements -> images ) {
        applyImage(hdc,pImage);
        delete pImage;
    }

    pIGraphicElements -> images.clear();

    for ( GraphicElements::imageMask *pMask : pIGraphicElements -> imageMasks ) {
        applyStencilMask(hdc,pMask);
        delete pMask;
    }

    pIGraphicElements -> imageMasks.clear();

    pIGraphicElements -> clearPaths();

    shutdownPathAndSink();

    shutdownRenderer();

    return S_OK;
    }


    HRESULT Renderer::Discard() {
    if ( NULL == pIGraphicElements -> pFirstPath )
        return S_OK;
    GraphicElements::path *p = pIGraphicElements -> pFirstPath;
    while ( ! ( p == NULL ) ) {
        GraphicElements::path *pNext = p -> pNextPath;
        delete p;
        p = pNext;
    }
    pIGraphicElements -> pFirstPath = NULL;
    pIGraphicElements -> pCurrentPath = NULL;
    return S_OK;
    }


    HRESULT Renderer::ClearRect(HDC hdc,RECT *pRect,COLORREF theColor) {
    FillRect(hdc,pRect,CreateSolidBrush(theColor));
    memset(szStatusMessage,0,1024 * sizeof(char));
    memset(szErrorMessage,0,1024 * sizeof(char));
    pIConnectionPointContainer -> fire_Clear();
    return S_OK;
    }


    HRESULT Renderer::WhereAmI(long xPixels,long yPixels,FLOAT *pX,FLOAT *pY) {

    if ( ! pX && ! pY )
        return E_POINTER;

    POINTF ptPixels{(FLOAT)xPixels,(FLOAT)yPixels};
    pIGraphicElements -> transformPoint(&pGraphicsStateManager -> parametersStack.top() -> toPageSpace,&ptPixels,&ptPixels);

    if ( pX ) 
        *pX = ptPixels.x;

    if ( pY )
        *pY = ptPixels.y;

    return S_OK;
    }


    UINT_PTR Renderer::SetNoGraphicsRendering() {
    if ( NULL == pIRendererNoWindow )
        pIRendererNoWindow = new RendererNoWindow(this);
    return reinterpret_cast<UINT_PTR>(pIRendererNoWindow);
    }