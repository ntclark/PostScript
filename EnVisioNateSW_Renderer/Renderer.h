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

#include "Renderer_i.h"

#define MY_EVENT_INTERFACE IID_IRendererNotifications
#define MY_EVENT_CLASS IRendererNotifications

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

        STDMETHOD(put_TransformMatrix)(UINT_PTR /*XFORM */ pXformToDeviceSpace);
        STDMETHOD(put_DownScale)(FLOAT ds);
        STDMETHOD(put_Origin)(POINTF ptOrigin);

        STDMETHOD(SetRenderLive)(HDC hdc,RECT *pDrawingRect);
        STDMETHOD(UnSetRenderLive)();
        STDMETHOD(Render)(HDC hdc,RECT *pDrawingRect);

        STDMETHOD(Discard)();
        STDMETHOD(ClearRect)(HDC hdc,RECT *pREct,COLORREF theColor);
        STDMETHOD(WhereAmI)(long xPixels,long yPixels,FLOAT *pX,FLOAT *pY);
        STDMETHOD(Reset)() { origin.x = 0.0f; origin.y = 0.0f; downScale = 1.0f; return S_OK; }

        STDMETHOD(GetParametersBundle)(UINT_PTR **pptrBundleStorage);
        STDMETHOD(SetParametersBundle)(UINT_PTR *ptrBundlestorage);

        // IGraphicElements

        class GraphicElements : public IUnknown {
        public:

            GraphicElements(Renderer *pp) : pParent(pp) {}

            //   IUnknown

            STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
            ULONG AddRef() { return pParent -> AddRef(); }
            ULONG Release() { return pParent -> Release(); }

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

            STDMETHOD(Image)(HDC hdc,HBITMAP hbmImage,UINT_PTR /*xForm*/ pPSCurrentCTM,FLOAT width,FLOAT height);
            STDMETHOD(NonPostScriptImage)(HDC hdc,HBITMAP hBitmap,FLOAT x0,FLOAT y0,FLOAT width,FLOAT height);

            struct primitive {

                enum type {
                    move = 0,
                    line = 1,
                    arc = 2,
                    ellipse = 3,
                    cubicBezier = 4,
                    quadraticBezier = 5,
#if 0
                    colorSet = 6,
                    lineStyleSet = 7,
                    lineWidthSet = 8,
#endif
                    newPathMarker = 24,
                    closePathMarker = 25,
                    strokePathMarker = 26,
                    fillPathMarker = 27
                };

                primitive(GraphicElements *pp,type t) : pParent(pp), theType(t) {}

                ~primitive() {}

                virtual void transform() {}
                virtual void *dataOne() { return (void *)&vertex; }
                virtual void *dataTwo() { return NULL; }
                virtual void logPrimitive() {
                    if ( 0 == pParent -> pParent -> pIConnectionPoint -> CountConnections() )
                        return;
                    switch ( theType ) {
#if 0
                    case colorSet:
                        sprintf_s<256>(Renderer::szLogMessage,"SetColor");
                        break;
                    case lineStyleSet:
                        sprintf_s<256>(Renderer::szLogMessage,"LineStyleSet");
                        break;
                    case lineWidthSet:
                        sprintf_s<256>(Renderer::szLogMessage,"LineWidthSet");
                        break;
#endif
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

                GraphicElements *pParent{NULL};

                POINTF vertex{0.0f,0.0f};

            };

            struct movePrimitive : primitive {
                movePrimitive(GraphicElements *pp,POINTF *pPoint) : primitive(pp,type::move) {
                    vertex.x = pPoint -> x;
                    vertex.y = pPoint -> y;
                    origin = pParent -> pParent -> origin;
                    downScale = pParent -> pParent -> downScale;
                    return;
                    }
                void transform() { 
                    vertex.x /= downScale;
                    vertex.y /= downScale;
                    vertex.x += origin.x;
                    vertex.y += origin.y;
                    pParent -> transformPoint(&vertex,&vertex);
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
                    origin = pParent -> pParent -> origin;
                    downScale = pParent -> pParent -> downScale;
                    return;
                    }
                void transform() { 
                    vertex.x /= downScale;
                    vertex.y /= downScale;
                    vertex.x += origin.x;
                    vertex.y += origin.y;
                    pParent -> transformPoint(&vertex,&vertex);
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
                    origin = pParent -> pParent -> origin;
                    downScale = pParent -> pParent -> downScale;
                    return;
                }
                void transform() {
                    arcSegment.point.x /= downScale;
                    arcSegment.point.y /= downScale;
                    arcSegment.size.width /= downScale;
                    arcSegment.size.height /= downScale;
                    pParent -> transformPoint(&arcSegment.point,&arcSegment.point);
                    pParent -> scalePoint(&arcSegment.size.width,&arcSegment.size.height);
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
                    origin = pParent -> pParent -> origin;
                    downScale = pParent -> pParent -> downScale;
                    return;
                }
                void transform() {
                    ellipseSegment.point.x /= downScale;
                    ellipseSegment.point.y /= downScale;
                    ellipseSegment.radiusX /= downScale;
                    ellipseSegment.radiusY /= downScale;
                    pParent -> transformPoint(&ellipseSegment.point,&ellipseSegment.point);
                    pParent -> scalePoint(&ellipseSegment.radiusX,&ellipseSegment.radiusY);
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
                    origin = pParent -> pParent -> origin;
                    downScale = pParent -> pParent -> downScale;
                    return;
                }
                void transform() { 
                    vertex.x /= downScale;
                    vertex.y /= downScale;
                    bezierSegment.point1.x /= downScale;
                    bezierSegment.point1.y /= downScale;
                    bezierSegment.point2.x /= downScale;
                    bezierSegment.point2.y /= downScale;
                    bezierSegment.point3.x /= downScale;
                    bezierSegment.point3.y /= downScale;
                    vertex.x += origin.x;
                    vertex.y += origin.y;
                    bezierSegment.point1.x += origin.x;
                    bezierSegment.point1.y += origin.y;
                    bezierSegment.point2.x += origin.x;
                    bezierSegment.point2.y += origin.y;
                    bezierSegment.point3.x += origin.x;
                    bezierSegment.point3.y += origin.y;
                    pParent -> transformPoint(&vertex,&vertex);
                    pParent -> transformPoint(&bezierSegment.point1,&bezierSegment.point1);
                    pParent -> transformPoint(&bezierSegment.point2,&bezierSegment.point2);
                    pParent -> transformPoint(&bezierSegment.point3,&bezierSegment.point3);
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
                    origin = pParent -> pParent -> origin;
                    downScale = pParent -> pParent -> downScale;
                    return;
                    }
                void transform() { 
                    vertex.x /= downScale;
                    vertex.y /= downScale;
                    quadraticBezierSegment.point1.x /= downScale;
                    quadraticBezierSegment.point1.y /= downScale;
                    quadraticBezierSegment.point2.x /= downScale;
                    quadraticBezierSegment.point2.y /= downScale;
                    quadraticBezierSegment.point1.x += origin.x;
                    quadraticBezierSegment.point1.y += origin.y;
                    quadraticBezierSegment.point2.x += origin.x;
                    quadraticBezierSegment.point2.y += origin.y;
                    pParent -> transformPoint(&vertex,&vertex);
                    pParent -> transformPoint(&quadraticBezierSegment.point1,&quadraticBezierSegment.point1);
                    pParent -> transformPoint(&quadraticBezierSegment.point2,&quadraticBezierSegment.point2);
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
                char szLineSettings[128]{'\0'};

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

            path *pFirstPath{NULL};
            path *pCurrentPath{NULL};

            POINTF currentPageSpacePoint{0,0};
            boolean isFigureStarted{false};

            XFORM toDeviceSpace{6 * 0.0f};
            XFORM toUserSpace{6 * 0.0f};

            void calcInverseTransform();

            void scalePoint(FLOAT *px,FLOAT *py);
            void transformPoint(FLOAT *px,FLOAT *py);
            void unTransformPoint(float *px,FLOAT *py);
            void transformPoint(POINTF *ptIn,POINTF *ptOut);
            void transformPoint(D2D1_POINT_2F *ptIn,D2D1_POINT_2F *ptOut);

            Renderer *pParent{NULL};

            friend class Renderer;

        } *pIGraphicElements{NULL};

        // IGraphicParameters

        class GraphicParameters : public IUnknown {
        public:

            GraphicParameters(Renderer *pp) : pParent(pp) {
                valuesStack.push(new values());
            }

            //   IUnknown

            STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
            ULONG AddRef() { return pParent -> AddRef(); }
            ULONG Release() { return pParent -> Release(); }

            ~GraphicParameters() { }

        private:

            STDMETHOD(SaveState)();
            STDMETHOD(RestoreState)();

            STDMETHOD(put_LineWidth)(FLOAT lw);
            STDMETHOD(put_LineJoin)(long lj);
            STDMETHOD(put_LineCap)(long lc);
            STDMETHOD(put_LineDash)(FLOAT *pValues,long countValues,FLOAT offset);
            STDMETHOD(put_RGBColor)(COLORREF rgb);

            Renderer *pParent{NULL};

            struct values {
                values() {}
                values(values &rhs) {
                    lineWidth = rhs.lineWidth;
                    lineCap = rhs.lineCap;
                    lineJoin = rhs.lineJoin;
                    lineDashStyle = rhs.lineDashStyle;
                    countDashSizes = rhs.countDashSizes;
                    memcpy(lineDashSizes,rhs.lineDashSizes,sizeof(lineDashSizes));
                    lineDashOffset = rhs.lineDashOffset;
                    rgbColor = rhs.rgbColor;
                    return;
                }
                FLOAT lineWidth{defaultLineWidth};
                D2D1_CAP_STYLE lineCap{defaultLineCap};
                D2D1_LINE_JOIN lineJoin{defaultLineJoin};
                D2D1_DASH_STYLE lineDashStyle{defaultDashStyle};
                int countDashSizes{0};
                FLOAT lineDashSizes[16]{16 * 0.0f};
                FLOAT lineDashOffset{0.0f};
                COLORREF rgbColor{defaultRGBColor};
            };

            long hashCode(char *pszLineSettings,boolean doFill);

            void setParameters(char *pszLineSettings,boolean doFill);
            void resetParameters(char *pszLineSettings);

            std::stack<values *> valuesStack;

            static FLOAT defaultLineWidth;
            static D2D1_CAP_STYLE defaultLineCap;
            static D2D1_LINE_JOIN defaultLineJoin;
            static D2D1_DASH_STYLE defaultDashStyle;
            static COLORREF defaultRGBColor;

            friend class Renderer;

        } *pIGraphicParameters{NULL};

        // IConnectionPointContainer

        struct _IConnectionPointContainer : public IConnectionPointContainer {
        public:

            _IConnectionPointContainer(Renderer *pp) : pParent(pp) {};
            ~_IConnectionPointContainer() {};

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
        STDMETHOD_ (ULONG, AddRef)();
        STDMETHOD_ (ULONG, Release)();

        STDMETHOD(FindConnectionPoint)(REFIID riid,IConnectionPoint **);
        STDMETHOD(EnumConnectionPoints)(IEnumConnectionPoints **);

        void fire_ErrorNotification(char *pszError);
        void fire_StatusNotification(char *pszStatus);
        void fire_LogNotification(char *pszLog);
        void fire_Clear();

        private:

            uint32_t refCount{0};
            Renderer *pParent{NULL};

        } *pIConnectionPointContainer{NULL};

        // IConnectionPoint

        struct _IConnectionPoint : IConnectionPoint {
        public:

            _IConnectionPoint(Renderer *pp);
            ~_IConnectionPoint();

            STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
            STDMETHOD_ (ULONG, AddRef)();
            STDMETHOD_ (ULONG, Release)();

            STDMETHOD (GetConnectionInterface)(IID *);
            STDMETHOD (GetConnectionPointContainer)(IConnectionPointContainer **ppCPC);
            STDMETHOD (Advise)(IUnknown *pUnk,DWORD *pdwCookie);
            STDMETHOD (Unadvise)(DWORD);
            STDMETHOD (EnumConnections)(IEnumConnections **ppEnum);

            IUnknown *AdviseSink() { return adviseSink; }

            int CountConnections() { return countConnections; }

        private:

            int getSlot();
            int findSlot(DWORD dwCookie);

            IUnknown *adviseSink{NULL};
            Renderer *pParent{NULL};
            DWORD nextCookie{0L};
            int countConnections{0};
            int countLiveConnections{0};

            uint32_t refCount{0};
            CONNECTDATA *connections{NULL};

        } *pIConnectionPoint{NULL};

        // IEnumConnectionPoints

        struct _IEnumConnectionPoints : IEnumConnectionPoints {
        public:

            _IEnumConnectionPoints(Renderer *pp,_IConnectionPoint **cp,int connectionPointCount);
            ~_IEnumConnectionPoints();

            STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
            STDMETHOD_ (ULONG, AddRef)();
            STDMETHOD_ (ULONG, Release)();

            STDMETHOD (Next)(ULONG cConnections,IConnectionPoint **rgpcn,ULONG *pcFetched);
            STDMETHOD (Skip)(ULONG cConnections);
            STDMETHOD (Reset)();
            STDMETHOD (Clone)(IEnumConnectionPoints **);

        private:

        int cpCount{0};
        int enumeratorIndex{0};
        Renderer *pParent{NULL};
        _IConnectionPoint **connectionPoints{NULL};

        } *pIEnumConnectionPoints{NULL};

        // IEnumerateConnections

        struct _IEnumerateConnections : public IEnumConnections {
        public:

        _IEnumerateConnections(IUnknown* pParentUnknown,ULONG cConnections,CONNECTDATA* paConnections,ULONG initialIndex);
        ~_IEnumerateConnections();

            STDMETHODIMP QueryInterface(REFIID, void **);
            STDMETHODIMP_(ULONG) AddRef();
            STDMETHODIMP_(ULONG) Release();
            STDMETHODIMP Next(ULONG, CONNECTDATA*, ULONG*);
            STDMETHODIMP Skip(ULONG);
            STDMETHODIMP Reset();
            STDMETHODIMP Clone(IEnumConnections**);

        private:

        ULONG refCount{0L};
        IUnknown *pParentUnknown{NULL};
        ULONG enumeratorIndex{0L};
        ULONG countConnections{0L};
        CONNECTDATA *connections{NULL};

        } *pIEnumerateConnections{NULL};

        void setupRenderer(HDC hdc,RECT *pDrawingRect);
        void shutdownRenderer();

        void setupPathAndSink();
        void shutdownPathAndSink();

        void closeSink();

        HRESULT fillRender();
        HRESULT strokeRender();

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

        POINTF origin{0.0f,0.0f};
        FLOAT downScale{1.0f};

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