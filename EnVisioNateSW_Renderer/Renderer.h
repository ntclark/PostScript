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

#pragma once

#include <vector>
#include <stack>
#include <list>
#include <map>
#include <algorithm>

#include <d2d1_3.h>
#include <olectl.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <wincodec.h>

#include "Renderer_i.h"

#define MY_EVENT_INTERFACE IID_IRendererNotifications
#define MY_EVENT_CLASS IRendererNotifications

#define GUID_PRINT_LENGTH 40

int Mx3Inverse(double *pSource,double *pTarget);

    class Renderer : public IRenderer {
    public:

        Renderer(IUnknown *pUnkOuter);
        ~Renderer();

        //   IUnknown

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
        STDMETHOD_ (ULONG, AddRef)();
        STDMETHOD_ (ULONG, Release)();

    private:

        // IRenderer

        STDMETHOD(put_ToDeviceTransform)(UINT_PTR /*XFORM */ pToDeviceSpaceXForm);
        STDMETHOD(put_ToPageTransform)(UINT_PTR /*XFORM*/ pXForm);
        STDMETHOD(put_ScaleTransform)(UINT_PTR /*XFORM*/ pScaleXForm);
        STDMETHOD(put_DownScale)(FLOAT ds);
        STDMETHOD(put_Origin)(POINTF ptOrigin);

        STDMETHOD(SetRenderLive)(HDC hdc,RECT *pDrawingRect);
        STDMETHOD(UnSetRenderLive)();
        STDMETHOD(Render)(HDC hdc,RECT *pDrawingRect);

        STDMETHOD(Discard)();
        STDMETHOD(ClearRect)(HDC hdc,RECT *pREct,COLORREF theColor);
        STDMETHOD(WhereAmI)(long xPixels,long yPixels,FLOAT *pX,FLOAT *pY);

        STDMETHOD(SaveState)();
        STDMETHOD(RestoreState)();

        // IGraphicElements

        class GraphicElements : public IUnknown {
        public:

            GraphicElements(Renderer *pp);
            ~GraphicElements();

            //   IUnknown

            STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
            STDMETHOD_(ULONG,AddRef()) { return pParent -> AddRef(); }
            STDMETHOD_(ULONG,Release()) { return pParent -> Release(); }

        private:

            STDMETHOD(NewPath)();
            STDMETHOD(ClosePath)();
            STDMETHOD(StrokePath)();
            STDMETHOD(FillPath)();

            STDMETHOD(MoveTo)(FLOAT x,FLOAT y);
            STDMETHOD(MoveToRelative)(FLOAT x,FLOAT y);
            STDMETHOD(LineTo)(FLOAT x,FLOAT y);
            STDMETHOD(LineToRelative)(FLOAT x,FLOAT y);
            STDMETHOD(Arc)(FLOAT xCenter,FLOAT yCenter,FLOAT radius,FLOAT startAngle,FLOAT endAngle);
            STDMETHOD(Ellipse)(FLOAT xCenter,FLOAT yCenter,FLOAT xRadius,FLOAT yRadius);
            STDMETHOD(Circle)(FLOAT xcCenter,FLOAT yCenter,FLOAT radius);
            STDMETHOD(CubicBezier)(FLOAT x0,FLOAT y0,FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2,FLOAT x3,FLOAT y3);
            STDMETHOD(QuadraticBezier)(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2);

            STDMETHOD(PostScriptImage)(HDC hdc,UINT_PTR pbBits,UINT_PTR /*xForm*/ pImageXForm,UINT_PTR /*xForm*/ pPSCurrentCTM,long width,long height,long bitsPerComponent);

            STDMETHOD(PostScriptJpegImage)(HDC hdc,UINT_PTR pJpegData,long dataSize,
                                            UINT_PTR /*xForm*/ pImageXForm,UINT_PTR /*xForm*/ pPSCurrentCTM,long width,long height);

            STDMETHOD(PostScriptStencilMask)(long cbMaskingBytes,UINT_PTR pbMaskingBytes,BOOL polarity,COLORREF currentColor,
                                                UINT_PTR /*xForm*/ pImageXForm,UINT_PTR /*xForm*/ pPSCurrentCTM,long width,long height);

            STDMETHOD(NonPostScriptImage)(HDC hdc,HBITMAP hBitmap,FLOAT x0,FLOAT y0,FLOAT width,FLOAT height);
            STDMETHOD(NonPostScriptJpegImage)(HDC hdc,UINT_PTR pJpegData,long dataSize,FLOAT x0,FLOAT y0,FLOAT width,FLOAT height);

            STDMETHOD(GetCurrentPoint)(FLOAT *pX,FLOAT *pY);
            STDMETHOD(SetCurrentPoint)(FLOAT *pX,FLOAT *pY);

            struct primitive {

                enum type {
                    move = 0,
                    line = 1,
                    arc = 2,
                    ellipse = 3,
                    cubicBezier = 4,
                    quadraticBezier = 5,
                    newPathMarker = 24,
                    closePathMarker = 25,
                    strokePathMarker = 26,
                    fillPathMarker = 27
                };

                primitive(GraphicElements *pp,type t) : 
                    pParent(pp), 
                    theType(t),
                    origin(pp -> pParent -> pGraphicsStateManager -> parametersStack.top() -> origin),
                    downScale(pp -> pParent -> pGraphicsStateManager -> parametersStack.top() -> downScale),
                    xForm(pp -> pParent -> pGraphicsStateManager -> parametersStack.top() -> toPageSpace),
                    scaleXForm(pp -> pParent -> pGraphicsStateManager -> parametersStack.top() -> scaleXForm),
                    hasXForm(pp -> pParent -> pGraphicsStateManager -> parametersStack.top() -> hasXForm),
                    hasScale(pp -> pParent -> pGraphicsStateManager -> parametersStack.top() -> hasScale),
                    hasDownScale(pp -> pParent -> pGraphicsStateManager -> parametersStack.top() -> hasDownScale) {}

                ~primitive() {}

                virtual void transform() {}
                virtual void *dataOne() { return (void *)&vertex; }
                virtual void *dataTwo() { return NULL; }
                virtual void logPrimitive() {
                    if ( 0 == pParent -> pParent -> pIConnectionPoint -> CountConnections() )
                        return;
                    switch ( theType ) {
                    case newPathMarker:
                        sprintf_s<1024>(Renderer::szLogMessage,"NewPath");
                        break;
                    case closePathMarker:
                        sprintf_s<1024>(Renderer::szLogMessage,"ClosePath");
                        break;
                    case strokePathMarker:
                        sprintf_s<1024>(Renderer::szLogMessage,"StrokePath");
                        break;
                    case fillPathMarker:
                        sprintf_s<1024>(Renderer::szLogMessage,"FillPath");
                        break;
                    default:
                        return;
                    }
                    pParent -> pParent -> pIConnectionPointContainer -> fire_LogNotification(Renderer::szLogMessage);
                }

                type theType;

                primitive *pNextPrimitive{NULL};
                primitive *pPriorPrimitive{NULL};

                FLOAT downScale{1.0f};
                POINTF origin{0.0f,0.0f};
                XFORM xForm{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};
                XFORM scaleXForm{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};
                GraphicElements *pParent{NULL};
                boolean hasDownScale{false};
                boolean hasXForm{false};
                boolean hasScale{false};
                POINTF vertex{0.0f,0.0f};

            };

            struct movePrimitive : primitive {
                movePrimitive(GraphicElements *pp,POINTF *pPoint) : primitive(pp,type::move) {
                    vertex.x = pPoint -> x;
                    vertex.y = pPoint -> y;
                    return;
                    }
                void transform() { 
                    if ( hasDownScale ) {
                        vertex.x /= downScale;
                        vertex.y /= downScale;
                    }
                    if ( hasXForm )
                        pParent -> transformPoint(&xForm,&vertex,&vertex);
                    if ( hasScale ) 
                        pParent -> transformPoint(&scaleXForm,&vertex,&vertex);
                    vertex.x += origin.x;
                    vertex.y += origin.y;
                    pParent -> transformPoint(&pParent -> toDeviceSpace,&vertex,&vertex);
                }
                void logPrimitive() {
                    if ( 0 == pParent -> pParent -> pIConnectionPoint -> CountConnections() )
                        return;
                    sprintf_s<1024>(Renderer::szLogMessage,"Move(%5.0f,%5.0f)",vertex.x,vertex.y);
                    pParent -> pParent -> pIConnectionPointContainer -> fire_LogNotification(Renderer::szLogMessage);
                }
            };

            struct linePrimitive : primitive {
                linePrimitive(GraphicElements *pp,POINTF *pPoint) : primitive(pp,type::line) {
                    vertex.x = pPoint -> x;
                    vertex.y = pPoint -> y;
                    return;
                    }
                void transform() { 
                    if ( hasDownScale ) {
                        vertex.x /= downScale;
                        vertex.y /= downScale;
                    }
                    if ( hasXForm )
                        pParent -> transformPoint(&xForm,&vertex,&vertex);
                    if ( hasScale ) 
                        pParent -> transformPoint(&scaleXForm,&vertex,&vertex);
                    vertex.x += origin.x;
                    vertex.y += origin.y;
                    pParent -> transformPoint(&pParent -> toDeviceSpace,&vertex,&vertex);
                 }
                void logPrimitive() {
                    if ( 0 == pParent -> pParent -> pIConnectionPoint -> CountConnections() )
                        return;
                    sprintf_s<1024>(Renderer::szLogMessage,"Line(%5.0f,%5.0f)",vertex.x,vertex.y);
                    pParent -> pParent -> pIConnectionPointContainer -> fire_LogNotification(Renderer::szLogMessage);
                }
            };

            struct arcPrimitive : primitive {
                arcPrimitive(GraphicElements *pp,D2D1_ARC_SEGMENT *pSegment) : primitive(pp,type::arc) {
                    arcSegment.point.x = pSegment -> point.x;
                    arcSegment.point.y = pSegment -> point.y;
                    arcSegment.size.width = pSegment -> size.width;
                    arcSegment.size.height = pSegment -> size.height;
                    arcSegment.rotationAngle = pSegment -> rotationAngle;
                    arcSegment.sweepDirection = pSegment -> sweepDirection;
                    arcSegment.arcSize = pSegment -> arcSize;
                    return;
                }
                void transform() {

                    if ( hasDownScale ) {
                        arcSegment.point.x /= downScale;
                        arcSegment.point.y /= downScale;
                        arcSegment.size.width /= downScale;
                        arcSegment.size.height /= downScale;
                    }

                    if ( hasXForm )
                        pParent -> transformPoint(&xForm,&arcSegment.point,&arcSegment.point);
                    if ( hasScale ) 
                        pParent -> transformPoint(&scaleXForm,&arcSegment.point,&arcSegment.point);
                    arcSegment.point.x += origin.x;
                    arcSegment.point.y += origin.y;
                    pParent -> transformPoint(&pParent -> toDeviceSpace,&arcSegment.point,&arcSegment.point);

                    if ( hasXForm )
                        pParent -> scalePoint(&xForm,&arcSegment.size.width,&arcSegment.size.height);
                    if ( hasScale )
                        pParent -> scalePoint(&scaleXForm,&arcSegment.size.width,&arcSegment.size.height);
                    pParent -> scalePoint(&pParent -> toDeviceSpace,&arcSegment.size.width,&arcSegment.size.height);

                    return;
                }
                void logPrimitive() {
                    if ( 0 == pParent -> pParent -> pIConnectionPoint -> CountConnections() )
                        return;
                    sprintf_s<1024>(Renderer::szLogMessage,"Arc  at: %5.0f, %5.0f, size: %5.0f by %5.0f, rotation angle %5.0f, sweep: %s, arc size: %s",
                                                arcSegment.point.x,arcSegment.point.y,arcSegment.size.width,arcSegment.size.height,
                                                arcSegment.rotationAngle,arcSegment.sweepDirection == 0 ? "counter clockwise" : "clockwise",
                                                arcSegment.arcSize == D2D1_ARC_SIZE_SMALL ? "small" : "larget");
                    pParent -> pParent -> pIConnectionPointContainer -> fire_LogNotification(Renderer::szLogMessage);
                }
                void *dataOne() { return (void *)&arcSegment; }
                D2D1_ARC_SEGMENT arcSegment{{0.0f,0.0f},{0.0f,0.0f},0.0f,D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,D2D1_ARC_SIZE_SMALL};
            };

            struct ellipsePrimitive : primitive {
                ellipsePrimitive(GraphicElements *pp,D2D1_ELLIPSE *pSegment) : primitive(pp,type::ellipse) {
                    ellipseSegment.point.x = pSegment -> point.x;
                    ellipseSegment.point.y = pSegment -> point.y;
                    ellipseSegment.radiusX = pSegment -> radiusX;
                    ellipseSegment.radiusY = pSegment -> radiusY;
                    return;
                }
                void transform() {
                    if ( hasDownScale ) {
                        ellipseSegment.point.x /= downScale;
                        ellipseSegment.point.y /= downScale;
                        ellipseSegment.radiusX /= downScale;
                        ellipseSegment.radiusY /= downScale;
                    }
                    if ( hasXForm )
                        pParent -> transformPoint(&xForm,&ellipseSegment.point,&ellipseSegment.point);
                    if ( hasScale )
                        pParent -> transformPoint(&scaleXForm,&ellipseSegment.point,&ellipseSegment.point);
                    ellipseSegment.point.x += origin.x;
                    ellipseSegment.point.y += origin.y;
                    pParent -> transformPoint(&pParent -> toDeviceSpace,&ellipseSegment.point,&ellipseSegment.point);

                    if ( hasXForm )
                        pParent -> scalePoint(&xForm,&ellipseSegment.radiusX,&ellipseSegment.radiusY);
                    if ( hasScale )
                        pParent -> scalePoint(&scaleXForm,&ellipseSegment.radiusX,&ellipseSegment.radiusY);
                    pParent -> scalePoint(&pParent -> toDeviceSpace,&ellipseSegment.radiusX,&ellipseSegment.radiusY);
                    return;
                }
                void logPrimitive() {
                    if ( 0 == pParent -> pParent -> pIConnectionPoint -> CountConnections() )
                        return;
                    sprintf_s<1024>(Renderer::szLogMessage,"Ellipse  at: %5.0f, %5.0f, XRadius: %5.0f, YRadius: %5.0f",
                                                ellipseSegment.point.x,ellipseSegment.point.y,ellipseSegment.radiusX,ellipseSegment.radiusY);
                    pParent -> pParent -> pIConnectionPointContainer -> fire_LogNotification(Renderer::szLogMessage);
                }
                void *dataOne() { return (void *)&ellipseSegment; }
                D2D1_ELLIPSE ellipseSegment{{0.0f,0.0f},0.0f,0.0f};
            };

            struct bezierPrimitive : primitive {
                bezierPrimitive(GraphicElements *pp,POINTF *pFirstPoint,D2D1_BEZIER_SEGMENT *pSegment) : primitive(pp,type::cubicBezier) { 
                    vertex.x = pFirstPoint -> x;
                    vertex.y = pFirstPoint -> y;
                    bezierSegment.point1.x = pSegment -> point1.x;
                    bezierSegment.point1.y = pSegment -> point1.y;
                    bezierSegment.point2.x = pSegment -> point2.x;
                    bezierSegment.point2.y = pSegment -> point2.y;
                    bezierSegment.point3.x = pSegment -> point3.x;
                    bezierSegment.point3.y = pSegment -> point3.y;
                    return;
                }
                void transform() { 
                    if ( hasDownScale ) {
                        vertex.x /= downScale;
                        vertex.y /= downScale;
                        bezierSegment.point1.x /= downScale;
                        bezierSegment.point1.y /= downScale;
                        bezierSegment.point2.x /= downScale;
                        bezierSegment.point2.y /= downScale;
                        bezierSegment.point3.x /= downScale;
                        bezierSegment.point3.y /= downScale;
                    }
                    if ( hasXForm )
                        pParent -> transformPoint(&xForm,&vertex,&vertex);
                    if ( hasScale )
                        pParent -> transformPoint(&scaleXForm,&vertex,&vertex);
                    vertex.x += origin.x;
                    vertex.y += origin.y;
                    pParent -> transformPoint(&pParent -> toDeviceSpace,&vertex,&vertex);

                    if ( hasXForm )
                        pParent -> transformPoint(&xForm,&bezierSegment.point1,&bezierSegment.point1);
                    if ( hasScale )
                        pParent -> transformPoint(&scaleXForm,&bezierSegment.point1,&bezierSegment.point1);
                    bezierSegment.point1.x += origin.x;
                    bezierSegment.point1.y += origin.y;
                    pParent -> transformPoint(&pParent -> toDeviceSpace,&bezierSegment.point1,&bezierSegment.point1);

                    if ( hasXForm )
                        pParent -> transformPoint(&xForm,&bezierSegment.point2,&bezierSegment.point2);
                    if ( hasScale )
                        pParent -> transformPoint(&scaleXForm,&bezierSegment.point2,&bezierSegment.point2);
                    bezierSegment.point2.x += origin.x;
                    bezierSegment.point2.y += origin.y;
                    pParent -> transformPoint(&pParent -> toDeviceSpace,&bezierSegment.point2,&bezierSegment.point2);

                    if ( hasXForm )
                        pParent -> transformPoint(&xForm,&bezierSegment.point3,&bezierSegment.point3);
                    if ( hasScale )
                        pParent -> transformPoint(&scaleXForm,&bezierSegment.point3,&bezierSegment.point3);
                    bezierSegment.point3.x += origin.x;
                    bezierSegment.point3.y += origin.y;
                    pParent -> transformPoint(&pParent -> toDeviceSpace,&bezierSegment.point3,&bezierSegment.point3);
                    return;
                }
                void logPrimitive() {
                    if ( 0 == pParent -> pParent -> pIConnectionPoint -> CountConnections() )
                        return;
                    sprintf_s<1024>(Renderer::szLogMessage,"Cubic Bezier  at: %5.0f, %5.0f, Control Point1: %5.0f, %5.0f, Control Point2: %5.0f, %5.0f, End Point: %5.0f, %5.0f",
                                                vertex.x,vertex.y,bezierSegment.point1.x,bezierSegment.point1.y,bezierSegment.point2.x,bezierSegment.point2.y,
                                                   bezierSegment.point3.x,bezierSegment.point3.y);
                    pParent -> pParent -> pIConnectionPointContainer -> fire_LogNotification(Renderer::szLogMessage);
                }
                void *dataOne() { return (void *)&bezierSegment; }
                D2D1_BEZIER_SEGMENT bezierSegment{{0.0f,0.0f},{0.0f,0.0f},{0.0f,0.0f}};
            };

            struct quadraticBezierPrimitive : primitive {
                quadraticBezierPrimitive(GraphicElements *pp,POINTF *pCurrentPoint,D2D1_QUADRATIC_BEZIER_SEGMENT *pSegment) : primitive(pp,type::quadraticBezier) {
                    vertex.x = pCurrentPoint -> x;
                    vertex.y = pCurrentPoint -> y;
                    quadraticBezierSegment.point1.x = pSegment -> point1.x;
                    quadraticBezierSegment.point1.y = pSegment -> point1.y;
                    quadraticBezierSegment.point2.x = pSegment -> point2.x;
                    quadraticBezierSegment.point2.y = pSegment -> point2.y;
                    return;
                    }
                void transform() { 
                    if ( hasDownScale ) {
                        vertex.x /= downScale;
                        vertex.y /= downScale;
                        quadraticBezierSegment.point1.x /= downScale;
                        quadraticBezierSegment.point1.y /= downScale;
                        quadraticBezierSegment.point2.x /= downScale;
                        quadraticBezierSegment.point2.y /= downScale;
                    }
                    if ( hasXForm )
                        pParent -> transformPoint(&xForm,&vertex,&vertex);
                    if ( hasScale )
                        pParent -> transformPoint(&scaleXForm,&vertex,&vertex);
                    vertex.x += origin.x;
                    vertex.y += origin.y;
                    pParent -> transformPoint(&pParent -> toDeviceSpace,&vertex,&vertex);

                    if ( hasXForm )
                        pParent -> transformPoint(&xForm,&quadraticBezierSegment.point1,&quadraticBezierSegment.point1);
                    if ( hasScale )
                        pParent -> transformPoint(&scaleXForm,&quadraticBezierSegment.point1,&quadraticBezierSegment.point1);
                    quadraticBezierSegment.point1.x += origin.x;
                    quadraticBezierSegment.point1.y += origin.y;
                    pParent -> transformPoint(&pParent -> toDeviceSpace,&quadraticBezierSegment.point1,&quadraticBezierSegment.point1);

                    if ( hasXForm )
                        pParent -> transformPoint(&xForm,&quadraticBezierSegment.point2,&quadraticBezierSegment.point2);
                    if ( hasScale )
                        pParent -> transformPoint(&scaleXForm,&quadraticBezierSegment.point2,&quadraticBezierSegment.point2);
                    quadraticBezierSegment.point2.x += origin.x;
                    quadraticBezierSegment.point2.y += origin.y;
                    pParent -> transformPoint(&pParent -> toDeviceSpace,&quadraticBezierSegment.point2,&quadraticBezierSegment.point2);
                    return;
                }
                void logPrimitive() {
                    if ( 0 == pParent -> pParent -> pIConnectionPoint -> CountConnections() )
                        return;
                    sprintf_s<1024>(Renderer::szLogMessage,"Quadratic Bezier  at: %5.0f, %5.0f, Control Point: %5.0f, %5.0f, End Point: %5.0f, %5.0f",
                                                vertex.x,vertex.y,quadraticBezierSegment.point1.x,quadraticBezierSegment.point1.y,
                                                    quadraticBezierSegment.point2.x,quadraticBezierSegment.point2.y);
                    pParent -> pParent -> pIConnectionPointContainer -> fire_LogNotification(Renderer::szLogMessage);
                }
                void *dataOne() { return (void *)&quadraticBezierSegment; }
                D2D1_QUADRATIC_BEZIER_SEGMENT quadraticBezierSegment{{0.0f,0.0f},{0.0f,0.0f}};
            };

            struct path {
            public:

                path(Renderer *pr,GraphicElements *pp) : pRenderer(pr), pParent(pp) {};

                GraphicElements *pParent{NULL};
                Renderer *pRenderer{NULL};

                primitive *pFirstPrimitive{NULL};
                primitive *pLastPrimitive{NULL};

                boolean isFillPath{false};

                long hashCode{0L};
                char szLineSettings[256]{'\0'};

                path *pNextPath{NULL};
                path *pPriorPath{NULL};

                long occuranceInFile{-1L};

                void addPrimitive(primitive *p) {
                    if ( NULL == pFirstPrimitive ) {
                        pFirstPrimitive = p;
                        pLastPrimitive = p;
                        return;
                    }
                    p -> pPriorPrimitive = pLastPrimitive;
                    pLastPrimitive -> pNextPrimitive = p;
                    pLastPrimitive = p;
                    return;
                }

                void removePrimitive(primitive *p) {
                    if ( pFirstPrimitive == p ) {
                        pFirstPrimitive = p -> pNextPrimitive;
                        if ( ! ( NULL == p -> pNextPrimitive ) )
                            p -> pNextPrimitive -> pPriorPrimitive = pFirstPrimitive;
                    } else {
                        p -> pPriorPrimitive -> pNextPrimitive = p -> pNextPrimitive;
                        if ( ! ( NULL == p -> pNextPrimitive ) )
                            p -> pNextPrimitive -> pPriorPrimitive = p -> pPriorPrimitive;
                    }
                    delete p;
                    return;
                }

                boolean isClosed() {
                if ( NULL == pLastPrimitive )
                    return false;
                if ( ( GraphicElements::primitive::type::strokePathMarker == pLastPrimitive -> theType ) ||
                        ( GraphicElements::primitive::type::closePathMarker == pLastPrimitive -> theType ) ||
                        ( GraphicElements::primitive::type::fillPathMarker == pLastPrimitive -> theType ) ) 
                    return true;
                return false;
                }

                void clear() {
                    primitive *p = pFirstPrimitive;
                    while ( ! ( NULL == p ) ) {
                        primitive *pNext = p -> pNextPrimitive;
                        delete p;
                        p = pNext;
                    }
                    pFirstPrimitive = NULL;
                    pLastPrimitive = NULL;
                }

                enum pathAction {
                    none = 0,
                    stroke = 1,
                    close = 2,
                    fill = 3
                };

                enum pathAction apply(boolean doFill);

                class specializedSink : ID2D1SimplifiedGeometrySink {
                public:
                    specializedSink(path *pp,ID2D1GeometrySink *ps) : pParent(pp), pActiveSink(ps), refCount(1) {
                    pActiveSink -> AddRef();
                    }

                    ~specializedSink() {
                        pActiveSink -> Release();
                    }

                    // IUnknown

                    STDMETHOD_(ULONG, AddRef)() {
                    return ++refCount;
                    }

                    STDMETHOD_(ULONG, Release)()  {
                    if ( 1 == refCount ) {
                        delete this;
                        return 0;
                    }
                    return --refCount;;
                    }

                    STDMETHOD(QueryInterface)(REFIID riid, void **ppvObject) {
                    *ppvObject = NULL;

                    if (IID_IUnknown == riid) 
                        *ppvObject = static_cast<IUnknown*>(this);

                    else if ( IID_ID2D1SimplifiedGeometrySink == riid )
                        *ppvObject = static_cast<ID2D1SimplifiedGeometrySink *>(this);

                    if ( NULL == *ppvObject )
                        return E_NOINTERFACE;

                    AddRef();
                    return S_OK;
                    }

                    STDMETHOD_(void, AddBeziers)(const D2D1_BEZIER_SEGMENT *pSegment,UINT countItems) {
                    for ( UINT k = 0; k < countItems; k++ )
                        pActiveSink -> AddBezier(pSegment + k);
                    return;
                    }

                    STDMETHOD_(void, AddLines)(const D2D1_POINT_2F *pPoints, UINT pointsCount) {
                    for ( UINT k = 0; k < pointsCount; k++ )
                        pActiveSink -> AddLine(D2D1::Point2F(pPoints[k].x,pPoints[k].y));
                    return;
                    }

                    STDMETHOD_(void, BeginFigure)(D2D1_POINT_2F startPoint,D2D1_FIGURE_BEGIN figureBegin) {
                    pActiveSink -> BeginFigure(D2D1::Point2F(startPoint.x,startPoint.y),figureBegin);
                    return;
                    }

                    STDMETHOD_(void, EndFigure)(D2D1_FIGURE_END figureEnd) {
                    pActiveSink -> EndFigure(figureEnd);
                    return;
                    }

                    STDMETHOD_(void, SetFillMode)(D2D1_FILL_MODE fillMode) { }

                    STDMETHOD_(void, SetSegmentFlags)(D2D1_PATH_SEGMENT vertexFlags) { }

                    STDMETHOD(Close)() {
                    return S_OK;
                    }

                private:

                    uint32_t refCount{0};
                    path *pParent{NULL};
                    ID2D1GeometrySink *pActiveSink{NULL};

                };

            };

            struct image {
                image(HBITMAP hbm,long x,long y,long wp,long hp,long wi,long hi) :
                    hBitmap(hbm),
                    locPixels{x,y},
                    widthImage(wi),
                    heightImage(hi),
                    widthPixels(wp),
                    heightPixels(hp) {
                }
                POINTL locPixels{0,0};
                long widthImage{0};
                long heightImage{0};
                long widthPixels{0};
                long heightPixels{0};
                HBITMAP hBitmap{NULL};
                ~image() {
                if ( ! ( NULL == hBitmap ) ) 
                    DeleteObject(hBitmap);
                }
            };

            struct imageMask {
                imageMask(uint8_t *pb,COLORREF cc,long x,long y,long wp,long hp,long wi,long hi,boolean p) :
                    locPixels{x,y},
                    pbMaskingBytes(pb),
                    currentColor(cc),
                    widthImage(wi),
                    heightImage(hi),
                    widthPixels(wp),
                    heightPixels(hp),
                    polarity(p) {
                }
                ~imageMask() {
                    delete [] pbMaskingBytes;
                }
                POINTL locPixels{0L,0L};
                long widthImage{0};
                long heightImage{0};
                long widthPixels{0};
                long heightPixels{0};
                boolean polarity{true};
                COLORREF currentColor{RGB(0,0,0)};
                uint8_t *pbMaskingBytes{NULL};

            };

            void addPath(path *p) {
                if ( NULL == pFirstPath ) {
                    pFirstPath = p;
                    pCurrentPath = p;
                    pCurrentPath -> addPrimitive(new primitive(this,primitive::type::newPathMarker));
                    return;
                }
                p -> pPriorPath = pCurrentPath;
                pCurrentPath -> pNextPath = p;
                pCurrentPath = p;
                pCurrentPath -> addPrimitive(new primitive(this,primitive::type::newPathMarker));
                return;
            }

            void removePath(path *p) {
                pCurrentPath = NULL;
                p -> clear();
                if ( p == pFirstPath ) {
                    pFirstPath = NULL;
                    if ( ! ( NULL == p -> pNextPath ) ) 
                        p -> pNextPath -> pPriorPath = NULL;
                } else {
                    p -> pPriorPath -> pNextPath = p -> pNextPath;
                    if ( ! ( NULL == p -> pNextPath ) ) {
                        p -> pNextPath -> pPriorPath = p -> pPriorPath;
                        pCurrentPath = p -> pNextPath;
                    } else
                        pCurrentPath = p -> pPriorPath;
                }
                delete p;
                return;
            }

            void clearPaths();

            path *pFirstPath{NULL};
            path *pCurrentPath{NULL};

            POINTF currentPageSpacePoint{0,0};
            boolean isFigureStarted{false};

            XFORM toDeviceSpace{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};
            XFORM toDeviceInverse{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};

            void calcInverseTransform(XFORM *pInput,XFORM *pOutput);

            uint8_t *getPixelsFromJpeg(uint8_t *pJPEGData,long dataSize,long width,long height);

            std::list<image *> images;
            std::list<imageMask *> imageMasks;

            void scalePoint(XFORM *pXForm,FLOAT *px,FLOAT *py);
            void transformPoint(XFORM *pXForm,POINTF *ptIn,POINTF *ptOut);
            void transformPoint(XFORM *pXForm,D2D1_POINT_2F *ptIn,D2D1_POINT_2F *ptOut);

            Renderer *pParent{NULL};

            friend class Renderer;

        } *pIGraphicElements{NULL};

        // IGraphicParameters

        class GraphicParameters : public IUnknown {
        public:

            GraphicParameters(Renderer *pp) : pParent(pp) { }

            //   IUnknown

            STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
            STDMETHOD_(ULONG,AddRef()) { return pParent -> AddRef(); }
            STDMETHOD_(ULONG,Release()) { return pParent -> Release(); }

            ~GraphicParameters() { }

        private:

            STDMETHOD(put_LineWidth)(FLOAT lw);
            STDMETHOD(get_LineWidth)(FLOAT *plw);
            STDMETHOD(put_LineJoin)(long lj);
            STDMETHOD(get_LineJoin)(long *plj);
            STDMETHOD(put_LineCap)(long lc);
            STDMETHOD(get_LineCap)(long *plc);
            STDMETHOD(put_MiterLimit)(FLOAT ml);
            STDMETHOD(get_MiterLimit)(FLOAT *pml);

            STDMETHOD(put_LineDash)(FLOAT *pValues,long countValues,FLOAT offset);
            STDMETHOD(put_RGBColor)(COLORREF rgb);
            STDMETHOD(get_RGBColor)(COLORREF *pRGB);

            Renderer *pParent{NULL};

            struct values {
                values() {}
                values(values &rhs) {
                    lineWidth = rhs.lineWidth;
                    calculatedLineWidth = rhs.calculatedLineWidth;
                    miterLimit = rhs.miterLimit;
                    lineCap = rhs.lineCap;
                    lineJoin = rhs.lineJoin;
                    lineDashStyle = rhs.lineDashStyle;
                    countDashSizes = rhs.countDashSizes;
                    memcpy(lineDashSizes,rhs.lineDashSizes,sizeof(lineDashSizes));
                    lineDashOffset = rhs.lineDashOffset;
                    rgbColor = rhs.rgbColor;
                    origin = rhs.origin;
                    downScale = rhs.downScale;
                    toPageSpace = rhs.toPageSpace;
                    scaleXForm = rhs.scaleXForm;
                    hasXForm = rhs.hasXForm;
                    hasScale = rhs.hasScale;
                    hasDownScale = rhs.hasDownScale;
                    return;
                }
                GUID valuesId{GUID_NULL};
                FLOAT lineWidth{defaultLineWidth};
                FLOAT calculatedLineWidth{0.0f};
                FLOAT miterLimit{defaultMiterLimit};
                D2D1_CAP_STYLE lineCap{defaultLineCap};
                D2D1_LINE_JOIN lineJoin{defaultLineJoin};
                D2D1_DASH_STYLE lineDashStyle{defaultDashStyle};
                int countDashSizes{0};
                FLOAT lineDashSizes[16]{16 * 0.0f};
                FLOAT lineDashOffset{0.0f};
                COLORREF rgbColor{defaultRGBColor};
                POINTF origin{0.0f,0.0f};
                FLOAT downScale{1.0f};
                XFORM toPageSpace{1.0,0.0,0.0,1.0,0.0,0.0};
                XFORM scaleXForm{1.0,0.0,0.0,1.0,0.0,0.0};
                boolean hasXForm{false};
                boolean hasScale{false};
                boolean hasDownScale{false};
            };

            long hashCode(char *pszLineSettings);

            void setParameters(char *pszLineSettings);
            void resetParameters(char *pszLineSettings);

            static FLOAT defaultLineWidth;
            static FLOAT defaultMiterLimit;
            static D2D1_CAP_STYLE defaultLineCap;
            static D2D1_LINE_JOIN defaultLineJoin;
            static D2D1_DASH_STYLE defaultDashStyle;
            static COLORREF defaultRGBColor;

            friend class Renderer;
            friend class GraphicsStateManager;

        } *pIGraphicParameters{NULL};

#include "COM Interfaces/Renderer_EventsInterfaces.h"

        class GraphicsStateManager {
        public:

            GraphicsStateManager(Renderer *pp) : pParent(pp) {
                CoCreateGuid(&initialGUID);
                parametersStack.push(new Renderer::GraphicParameters::values());
                parametersStack.top() -> valuesId = initialGUID;
            }

            ~GraphicsStateManager();

            HRESULT Save();
            HRESULT Restore();

        private:

            std::stack<GraphicParameters::values *> parametersStack;

            void clear() {
                while ( 0 < parametersStack.size() ) {
                    Renderer::GraphicParameters::values *pValues = parametersStack.top();
                    parametersStack.pop();
                    delete pValues;
                }
                return;
            }

            Renderer *pParent;
            GUID initialGUID{GUID_NULL};

            friend class Renderer;
            friend class GraphicElements;
            friend class GraphicParameters;

        } *pGraphicsStateManager;

        void setupRenderer(HDC hdc,RECT *pDrawingRect);
        void shutdownRenderer();

        void setupPathAndSink();
        void shutdownPathAndSink();

        void closeSink();

        HRESULT fillRender(GraphicElements::path *pFirstPath);

        HRESULT applyImage(HDC hdc,GraphicElements::image *);
        HRESULT applyStencilMask(HDC hdc,GraphicElements::imageMask *);

        ID2D1Factory1 *pID2D1Factory1{NULL};

        ID2D1PathGeometry *pID2D1PathGeometry{NULL};
        ID2D1GeometrySink *pID2D1GeometrySink{NULL};

        ID2D1SolidColorBrush *pID2D1SolidColorBrush{NULL};
        ID2D1StrokeStyle1 *pID2D1StrokeStyle1{NULL};
        ID2D1DCRenderTarget *pID2D1DCRenderTarget{NULL};

        ULONG refCount{0};

        D2D1_TAG tag1{0UL};
        D2D1_TAG tag2{0UL};

        ULONG renderCount{0};
        boolean doRasterize{false};
        boolean lastRenderWasFilled{false};
        boolean renderLive{false};

        HDC hdcCurrent{NULL};
        RECT rcCurrent{0L,0L,0L,0L};

        static char szLogMessage[1024];
        static char szStatusMessage[1024];
        static char szErrorMessage[1024];

        friend class GraphicElements;
        friend class GraphicParameters;

    };

#ifdef DEFINE_DATA

    HMODULE hModule;
    wchar_t wstrModuleName[1024];
    char szModuleName[1024];

    double degToRad = 4.0 * atan(1.0) / 180.0;

#else

    extern HMODULE hModule;
    extern wchar_t wstrModuleName[];
    extern char szModuleName[];

    extern double degToRad;

#endif