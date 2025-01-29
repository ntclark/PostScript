#pragma once

#include "Renderer_i.h"
#include <vector>
#include <stack>

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

        STDMETHOD(Prepare)(HDC hdc);
        STDMETHOD(put_TransformMatrix)(UINT_PTR /*XFORM */ pXformToDeviceSpace);
        STDMETHOD(get_IsPrepared)(BOOL *pIsPrepared);
        STDMETHOD(put_DownScale)(FLOAT ds);
        STDMETHOD(put_Origin)(POINTF ptOrigin);
        STDMETHOD(Render)();
        STDMETHOD(Reset)() { origin.x = 0.0f; origin.y = 0.0f; downScale = 1.0f; return S_OK; }

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
            STDMETHOD(ArcTo)(FLOAT xCenter,FLOAT yCenter,FLOAT radius,FLOAT angle1,FLOAT angle2);
            STDMETHOD(CubicBezierTo)(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2,FLOAT x3,FLOAT y3);
            STDMETHOD(QuadraticBezierTo)(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2);

            STDMETHOD(Image)(HBITMAP hbmImage,UINT_PTR /*xForm*/ pPSCurrentCTM,FLOAT width,FLOAT height);

            struct primitive {

                enum type {
                    move = 0,
                    line = 1,
                    arc = 2,
                    cubicBezier = 3,
                    quadraticBezier = 4,
                    colorSet = 5,
                    lineStyleSet = 6,
                    lineWidthSet = 7,
                    newPathMarker = 24,
                    closePathMarker = 25,
                    strokePathMarker = 26,
                    fillPathMarker = 27
                };

                primitive(GraphicElements *pp,type t) : pParent(pp), theType(t) {}

                primitive(GraphicElements *pp,COLORREF color) : pParent(pp), theType(colorSet), theColor(color) {}

                primitive(GraphicElements *pp,FLOAT lw) : pParent(pp), theType(lineWidthSet), theLineWidth(lw) {};

                primitive(GraphicElements *pp,D2D1_CAP_STYLE lineCap,D2D1_LINE_JOIN lineJoin,D2D1_DASH_STYLE dashStyle,
                                FLOAT offset,FLOAT *pValues,long countValues) : pParent(pp), theType(lineStyleSet) {
                theLineCap = lineCap;
                theLineJoin = lineJoin;
                theLineDashStyle = dashStyle;
                theLineDashOffset = offset;
                countFloats = 0;
                if ( ! ( NULL == pValues ) ) {
                    countFloats = countValues;
                    pvData = new BYTE[countValues * sizeof(FLOAT)];
                    memcpy(pvData,pValues,countValues * sizeof(FLOAT));
                }
                return;
                }

                ~primitive() {
                if ( ! ( NULL == pvData ) )
                    delete [] pvData;
                return;
                }

                virtual void transform() {}

                type theType;

                primitive *pNext{NULL};
                primitive *pPrior{NULL};

                GraphicElements *pParent{NULL};

                long countFloats{0};
                void *pvData{NULL};

                COLORREF theColor{GraphicParameters::defaultRGBColor};
                FLOAT theLineWidth{GraphicParameters::defaultLineWidth};
                D2D1_CAP_STYLE theLineCap{GraphicParameters::defaultLineCap};
                D2D1_LINE_JOIN theLineJoin{GraphicParameters::defaultLineJoin};
                D2D1_DASH_STYLE theLineDashStyle{GraphicParameters::defaultDashStyle};
                FLOAT theLineDashOffset{0.0f};

                POINTF vertices[3]{{0.0f,0.0f}, {0.0f,0.0f}, {0.0f,0.0f}};
                D2D1_BEZIER_SEGMENT bezierSegment{{0.0f,0.0f},{0.0f,0.0f},{0.0f,0.0f}};
                D2D1_QUADRATIC_BEZIER_SEGMENT quadraticBezierSegment{{0.0f,0.0f},{0.0f,0.0f}};

            };

            struct movePrimitive : primitive {
                movePrimitive(GraphicElements *pp,POINTF *pPoint) : primitive(pp,type::move) {
                    vertices[0].x = pPoint -> x;
                    vertices[0].y = pPoint -> y;
                    return;
                    }
                void transform() { 
                    vertices[0].x /= pParent -> pParent -> downScale;
                    vertices[0].y /= pParent -> pParent -> downScale;
                    vertices[0].x += pParent -> pParent -> origin.x;
                    vertices[0].y += pParent -> pParent -> origin.y;
                    pParent -> transformPoint(&vertices[0],&vertices[0]);
                }
            };

            struct linePrimitive : primitive {
                linePrimitive(GraphicElements *pp,POINTF *pPoint) : primitive(pp,type::line) {
                    vertices[0].x = pPoint -> x;
                    vertices[0].y = pPoint -> y;
                    return;
                    }
                void transform() { 
                    vertices[0].x /= pParent -> pParent -> downScale;
                    vertices[0].y /= pParent -> pParent -> downScale;
                    vertices[0].x += pParent -> pParent -> origin.x;
                    vertices[0].y += pParent -> pParent -> origin.y;
                    pParent -> transformPoint(&vertices[0],&vertices[0]);
                 }
            };

            struct bezierPrimitive : primitive {
                bezierPrimitive(GraphicElements *pp,D2D1_BEZIER_SEGMENT *pSegment) : primitive(pp,type::cubicBezier) { 
                    bezierSegment.point1.x = pSegment -> point1.x;
                    bezierSegment.point1.y = pSegment -> point1.y;
                    bezierSegment.point2.x = pSegment -> point2.x;
                    bezierSegment.point2.y = pSegment -> point2.y;
                    bezierSegment.point3.x = pSegment -> point3.x;
                    bezierSegment.point3.y = pSegment -> point3.y;
                    return;
                }
                void transform() { 
                    bezierSegment.point1.x /= pParent -> pParent -> downScale;
                    bezierSegment.point1.y /= pParent -> pParent -> downScale;
                    bezierSegment.point2.x /= pParent -> pParent -> downScale;
                    bezierSegment.point2.y /= pParent -> pParent -> downScale;
                    bezierSegment.point3.x /= pParent -> pParent -> downScale;
                    bezierSegment.point3.y /= pParent -> pParent -> downScale;
                    bezierSegment.point1.x += pParent -> pParent -> origin.x;
                    bezierSegment.point1.y += pParent -> pParent -> origin.y;
                    bezierSegment.point2.x += pParent -> pParent -> origin.x;
                    bezierSegment.point2.y += pParent -> pParent -> origin.y;
                    bezierSegment.point3.x += pParent -> pParent -> origin.x;
                    bezierSegment.point3.y += pParent -> pParent -> origin.y;
                    pParent -> transformPoint(&bezierSegment.point1,&bezierSegment.point1);
                    pParent -> transformPoint(&bezierSegment.point2,&bezierSegment.point2);
                    pParent -> transformPoint(&bezierSegment.point3,&bezierSegment.point3);
                    return;
                }

            };

            struct quadraticBezierPrimitive : primitive {
                quadraticBezierPrimitive(GraphicElements *pp,POINTF *pCurrentPoint,D2D1_QUADRATIC_BEZIER_SEGMENT *pSegment) : primitive(pp,type::quadraticBezier) {
                    vertices[0].x = pCurrentPoint -> x;
                    vertices[0].y = pCurrentPoint -> y;
                    theType = quadraticBezier;
                    quadraticBezierSegment.point1.x = pSegment -> point1.x;
                    quadraticBezierSegment.point1.y = pSegment -> point1.y;
                    quadraticBezierSegment.point2.x = pSegment -> point2.x;
                    quadraticBezierSegment.point2.y = pSegment -> point2.y;
                    return;
                    }
                void transform() { 
                    quadraticBezierSegment.point1.x /= pParent -> pParent -> downScale;
                    quadraticBezierSegment.point1.y /= pParent -> pParent -> downScale;
                    quadraticBezierSegment.point2.x /= pParent -> pParent -> downScale;
                    quadraticBezierSegment.point2.y /= pParent -> pParent -> downScale;
                    quadraticBezierSegment.point1.x += pParent -> pParent -> origin.x;
                    quadraticBezierSegment.point1.y += pParent -> pParent -> origin.y;
                    quadraticBezierSegment.point2.x += pParent -> pParent -> origin.x;
                    quadraticBezierSegment.point2.y += pParent -> pParent -> origin.y;
                    pParent -> transformPoint(&quadraticBezierSegment.point1,&quadraticBezierSegment.point1);
                    pParent -> transformPoint(&quadraticBezierSegment.point2,&quadraticBezierSegment.point2);
                    return;
                }
            };

            struct path {
            public:

                path(Renderer *pr,GraphicElements *pp) : pRenderer(pr), pParent(pp) {};

                GraphicElements *pParent{NULL};
                Renderer *pRenderer{NULL};

                primitive *pFirstPrimitive{NULL};
                primitive *pCurrentPrimitive{NULL};

                boolean isFillPath{false};

                long hashCode{0L};
                char szLineSettings[128]{'\0'};

                path *pNext{NULL};
                path *pPrior{NULL};

                void addPrimitive(primitive *p) {
                    if ( NULL == pFirstPrimitive ) {
                        pFirstPrimitive = p;
                        pCurrentPrimitive = p;
                        return;
                    }
                    p -> pPrior = pCurrentPrimitive;
                    pCurrentPrimitive -> pNext = p;
                    pCurrentPrimitive = p;
                    return;
                }

                void clear() {
                    primitive *p = pFirstPrimitive;
                    while ( ! ( NULL == p ) ) {
                        primitive *pNext = p -> pNext;
                        delete p;
                        p = pNext;
                    }
                    pFirstPrimitive = NULL;
                    pCurrentPrimitive = NULL;
                }

                void apply(boolean doFill,boolean firstPass,FILE *fDebug = NULL);

            };

            void addPath(path *p) {
                if ( NULL == pFirstPath ) {
                    pFirstPath = p;
                    pCurrentPath = p;
                    return;
                }
                p -> pPrior = pCurrentPath;
                pCurrentPath -> pNext = p;
                pCurrentPath = p;
            return;
            }

            path *pFirstPath{NULL};
            path *pCurrentPath{NULL};

            POINTF currentPageSpacePoint{0,0};
            boolean isFigureStarted{false};

            XFORM toDeviceSpace{6 * 0.0f};

            void scalePoint(FLOAT *px,FLOAT *py);
            void transformPoint(FLOAT *px,FLOAT *py);
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

            void addColorPrimitive() {
            if ( NULL == pParent -> pIGraphicElements -> pCurrentPath )
                return;
            pParent -> pIGraphicElements -> pCurrentPath -> addPrimitive(new GraphicElements::primitive(pParent -> pIGraphicElements,valuesStack.top() -> rgbColor));
            return;
            }


            void addLineStylePrimitive() {
            if ( NULL == pParent -> pIGraphicElements -> pCurrentPath )
                return;
            pParent -> pIGraphicElements -> pCurrentPath -> addPrimitive(
                new GraphicElements::primitive(pParent -> pIGraphicElements,
                        valuesStack.top() -> lineCap,valuesStack.top() -> lineJoin,
                            valuesStack.top() -> lineDashStyle,valuesStack.top() -> offset,
                            valuesStack.top() -> lineStyles,valuesStack.top() -> countDashSizes));
            }

            struct values {
                values() {}
                values(values &rhs) {
                    lineWidth = rhs.lineWidth;
                    lineCap = rhs.lineCap;
                    lineJoin = rhs.lineJoin;
                    lineDashStyle = rhs.lineDashStyle;
                    countDashSizes = rhs.countDashSizes;
                    memcpy(lineStyles,rhs.lineStyles,sizeof(lineStyles));
                    offset = rhs.offset;
                    rgbColor = rhs.rgbColor;
                    return;
                }
                FLOAT lineWidth{defaultLineWidth};
                D2D1_CAP_STYLE lineCap{defaultLineCap};
                D2D1_LINE_JOIN lineJoin{defaultLineJoin};
                D2D1_DASH_STYLE lineDashStyle{defaultDashStyle};
                DWORD countDashSizes{0};
                FLOAT lineStyles[16]{16 * 0.0f};
                FLOAT offset{0.0f};
                COLORREF rgbColor{defaultRGBColor};
            };

            long hashCode(char *pszLineSettings,boolean doFill);

            void resetParameters(char *pszLineSettings);

            std::stack<values *> valuesStack;

            static FLOAT defaultLineWidth;
            static D2D1_CAP_STYLE defaultLineCap;
            static D2D1_LINE_JOIN defaultLineJoin;
            static D2D1_DASH_STYLE defaultDashStyle;
            static COLORREF defaultRGBColor;

            friend class Renderer;

        } *pIGraphicParameters{NULL};

        void setupRenderer();
        HRESULT internalRender();

        HDC hdc{NULL};

        ID2D1Factory1 *pID2D1Factory1{NULL};

        ID2D1PathGeometry *pID2D1PathGeometry{NULL};
        ID2D1GeometrySink *pID2D1GeometrySink{NULL};

        ID2D1SolidColorBrush *pID2D1SolidColorBrush{NULL};
        ID2D1StrokeStyle1 *pID2D1StrokeStyle1{NULL};
        ID2D1DCRenderTarget *pID2D1DCRenderTarget{NULL};

        ULONG refCount{0};

        ULONG renderCount{0};
        boolean doRasterize{false};
        boolean lastRenderWasFilled{false};

        POINTF origin{1.0f,1.0f};
        FLOAT downScale{1.0f};

        friend class GraphicElements;
        friend class GraphicParameters;

    };

#ifdef DEFINE_DATA

    HMODULE hModule;
    wchar_t wstrModuleName[1024];
    char szModuleName[1024];

#else

    extern HMODULE hModule;
    extern wchar_t wstrModuleName[];
    extern char szModuleName[];

#endif