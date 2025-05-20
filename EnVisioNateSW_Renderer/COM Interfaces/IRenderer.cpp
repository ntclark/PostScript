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


    HRESULT Renderer::put_ToPageTransform(UINT_PTR pXForm) {
    memcpy(&pGraphicsStateManager -> parametersStack.top() -> toPageSpace,(void *)pXForm,sizeof(XFORM));
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

    std::map<long,std::list<GraphicElements::path *> *> strokePathsMap;
    std::map<long,std::list<GraphicElements::path *> *> fillPathsMap;

    long fileOccurance = 0L;

    std::vector<std::pair<long,long>> fillPathSorter;
    std::vector<std::pair<long,long>> strokePathSorter;

    //
    // Place all paths into either the group of StrokePaths, or FillPaths
    // Note that the "group", is a list of all paths that have a particular set
    // of graphics parameters, i.e. the colors and linestyles
    // the StrokePaths group also contains ClosePath paths
    //

    GraphicElements::path *p = pIGraphicElements -> pFirstPath;

    while ( ! ( NULL == p ) ) {

        //
        // This happens when there are no primitives in the path at all
        // other than the initial newPath marker.
        // I am not entirely sure this is okay, but I'm removing the
        // path from any fendering
        //
        if ( p -> pFirstPrimitive == p -> pLastPrimitive ) {
            p = p -> pNextPath;
            continue;
        }

        if ( ! p -> isClosed() ) {
            //
            // This DOES happen, I notice when there's a newpath, a moveto, followed by the next newpath
            // that is, an orphaned path
            //
            // Whether this is okay is still under scrutiny
            //
            sprintf(Renderer::szStatusMessage,"Warning: An unclosed path was encountered. All paths should "
                                                    "be closed with ClosePath(), StrokePath(), or FillPath(). "
                                                    "This graphics element will not appear.");
            pIConnectionPointContainer -> fire_ErrorNotification(Renderer::szStatusMessage);
            p = p -> pNextPath;
            continue;
        }

        if ( p -> isFillPath ) {
            if ( fillPathsMap.find(p -> hashCode) == fillPathsMap.end() ) {
                fillPathsMap[p -> hashCode] = new std::list<GraphicElements::path *>();
                fileOccurance++;
                fillPathSorter.push_back(std::pair<long,long>(p -> hashCode,fileOccurance));
            }
            fillPathsMap[p -> hashCode] -> push_back(p);
        } else {
            if ( strokePathsMap.find(p -> hashCode) == strokePathsMap.end() ) {
                strokePathsMap[p -> hashCode] = new std::list<GraphicElements::path *>();
                fileOccurance++;
                strokePathSorter.push_back(std::pair<long,long>(p -> hashCode,fileOccurance));
            }
            strokePathsMap[p -> hashCode] -> push_back(p);
        }


        p = p -> pNextPath;
    }

    //
    // Sort the FillPath group by their graphics parameters
    //

    std::sort(fillPathSorter.begin(), fillPathSorter.end(), [=](std::pair<long,long> &a, std::pair<long,long > &b) { return a.second < b.second; });

    for ( std::pair<long,long> sortedPair : fillPathSorter ) {

        std::list<GraphicElements::path *> *pList = fillPathsMap[sortedPair.first];

        //
        // Each path list in the map uses the same graphics parameters. 
        // The graphics parameters only need to be applied at the beginning
        // of emitting the path elements to the GeometrySink
        //
        pIGraphicParameters -> resetParameters(pList -> front() -> szLineSettings);

        setupPathAndSink();

        for ( GraphicElements::path *p : *pList ) {

            //
            // This cannot happen because this path is a fill path and it
            // can only be in a list (in the FillPathMap) if it was closed
            // with FillPath()
            // But I keep it in just to be sure
#if 1
            if ( GraphicElements::primitive::type::strokePathMarker == p -> pLastPrimitive -> theType ||
                   GraphicElements::primitive::type::closePathMarker == p -> pLastPrimitive -> theType ) {
                sprintf(Renderer::szStatusMessage,"Warning: There was a StrokePath(), or ClosePath(), "
                                                        "command encountered in a path intended to be filled. "
                                                        "This graphics element will not appear.");
                pIConnectionPointContainer -> fire_ErrorNotification(Renderer::szStatusMessage);
                continue;
            }

            if ( GraphicElements::primitive::type::fillPathMarker == p -> pLastPrimitive -> theType )
#endif
                GraphicElements::path::pathAction pa = p -> apply(true);

            p -> clear();

        }

        closeSink();

        fillRender();

        shutdownPathAndSink();

        pList -> clear();

    }

    for ( std::pair<long,std::list<GraphicElements::path *>*> pPair : fillPathsMap ) {
        pPair.second -> clear();
        delete pPair.second;
    }

    fillPathsMap.clear();

    std::sort(strokePathSorter.begin(), strokePathSorter.end(), [=](std::pair<long,long> &a, std::pair<long, long > &b) { return a.second < b.second; });

    for ( std::pair<long,long> sortedPair : strokePathSorter ) {

        std::list<GraphicElements::path *> *pList = strokePathsMap[sortedPair.first];

        pIGraphicParameters -> resetParameters(pList -> front() -> szLineSettings);

        setupPathAndSink();

        for ( GraphicElements::path *p : *pList ) {

#if 1

            // As above, this shouldn't happen because it wouldn't be in the
            // StrokePathMap;
            if ( GraphicElements::primitive::type::fillPathMarker == p -> pLastPrimitive -> theType ) {
                sprintf(Renderer::szStatusMessage,"Warning: There was a FillPath() command encountered in a path "
                                                        "intended to be Stroked. "
                                                        "This graphics element will not appear");
                pIConnectionPointContainer -> fire_ErrorNotification(Renderer::szStatusMessage);
                continue;
            }

            if ( GraphicElements::primitive::type::strokePathMarker == p -> pLastPrimitive -> theType ||
                    GraphicElements::primitive::type::closePathMarker == p -> pLastPrimitive -> theType )
#endif
                GraphicElements::path::pathAction pa = p -> apply(false);

            p -> clear();

        }

        closeSink();

        strokeRender();

        shutdownPathAndSink();

        pList -> clear();

    }

    for ( std::pair<long,std::list<GraphicElements::path *>*> pPair : strokePathsMap ) {
        pPair.second -> clear();
        delete pPair.second;
    }

    strokePathsMap.clear();

    pIGraphicElements -> clearPaths();

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