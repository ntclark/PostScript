#pragma once

#include "Renderer_i.h"
#include <vector>

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
                theDashStyle = dashStyle;
                theDashOffset = offset;
                countFloats = 0;
                if ( ! ( NULL == pValues ) ) {
                    countFloats = countValues;
                    pvData = new BYTE[countValues * sizeof(FLOAT)];
                    memcpy(pvData,pValues,countValues * sizeof(FLOAT));
                }
                return;
                }

                primitive(GraphicElements *pp,type t,POINTF *pPoint) : pParent(pp), theType(t) {
                vertices[0].x = pParent -> pParent -> origin.x + pPoint -> x / pParent -> pParent -> downScale;
                vertices[0].y = pParent -> pParent -> origin.y + pPoint -> y / pParent -> pParent -> downScale;
                return;
                }

                primitive(GraphicElements *pp,type t,POINTF *pPoint1,POINTF *pPoint2,POINTF *pPoint3) : pParent(pp), theType(t) {
                vertices[0].x = pParent -> pParent -> origin.x + pPoint1 -> x / pParent -> pParent -> downScale;
                vertices[0].y = pParent -> pParent -> origin.y + pPoint1 -> y / pParent -> pParent -> downScale;
                vertices[1].x = pParent -> pParent -> origin.x + pPoint2 -> x / pParent -> pParent -> downScale;
                vertices[1].y = pParent -> pParent -> origin.y + pPoint2 -> y / pParent -> pParent -> downScale;
                vertices[2].x = pParent -> pParent -> origin.x + pPoint3 -> x / pParent -> pParent -> downScale;
                vertices[2].y = pParent -> pParent -> origin.y + pPoint3 -> y / pParent -> pParent -> downScale;
                return;
                }

                primitive(GraphicElements *pp,D2D1_BEZIER_SEGMENT *pSegment) : pParent(pp) { 
                theType = cubicBezier;
                bezierSegment.point1.x = pParent -> pParent -> origin.x + pSegment -> point1.x / pParent -> pParent -> downScale;
                bezierSegment.point1.y = pParent -> pParent -> origin.y + pSegment -> point1.y / pParent -> pParent -> downScale;
                bezierSegment.point2.x = pParent -> pParent -> origin.x + pSegment -> point2.x / pParent -> pParent -> downScale;
                bezierSegment.point2.y = pParent -> pParent -> origin.y + pSegment -> point2.y / pParent -> pParent -> downScale;
                bezierSegment.point3.x = pParent -> pParent -> origin.x + pSegment -> point3.x / pParent -> pParent -> downScale;
                bezierSegment.point3.y = pParent -> pParent -> origin.y + pSegment -> point3.y / pParent -> pParent -> downScale;
                return;
                }

                primitive(GraphicElements *pp,POINTF *pCurrentPoint,D2D1_QUADRATIC_BEZIER_SEGMENT *pSegment) : pParent(pp) { 
                vertices[0].x = pParent -> pParent -> origin.x + pCurrentPoint -> x / pParent -> pParent -> downScale;
                vertices[0].y = pParent -> pParent -> origin.y + pCurrentPoint -> y / pParent -> pParent -> downScale;
                theType = quadraticBezier;
                quadraticBezierSegment.point1.x = pParent -> pParent -> origin.x + pSegment -> point1.x / pParent -> pParent -> downScale;
                quadraticBezierSegment.point1.y = pParent -> pParent -> origin.y + pSegment -> point1.y / pParent -> pParent -> downScale;
                quadraticBezierSegment.point2.x = pParent -> pParent -> origin.x + pSegment -> point2.x / pParent -> pParent -> downScale;
                quadraticBezierSegment.point2.y = pParent -> pParent -> origin.y + pSegment -> point2.y / pParent -> pParent -> downScale;
                return;
                }

                ~primitive() {
                if ( ! ( NULL == pvData ) )
                    delete [] pvData;
                return;
                }

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
                D2D1_DASH_STYLE theDashStyle{GraphicParameters::defaultDashStyle};
                FLOAT theDashOffset{0.0f};

                POINTF vertices[3]{{0.0f,0.0f}, {0.0f,0.0f}, {0.0f,0.0f}};
                D2D1_BEZIER_SEGMENT bezierSegment{{0.0f,0.0f},{0.0f,0.0f},{0.0f,0.0f}};
                D2D1_QUADRATIC_BEZIER_SEGMENT quadraticBezierSegment{{0.0f,0.0f},{0.0f,0.0f}};

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

            POINTF currentUserPoint{0,0};
            POINTF currentGDIPoint{0,0};
            boolean isFigureStarted{false};

            XFORM toDeviceSpace{6 * 0.0f};
            void transformPoint(FLOAT *px,FLOAT *py);

            Renderer *pParent{NULL};

            friend class Renderer;

        } *pIGraphicElements{NULL};

        // IGraphicParameters

        class GraphicParameters : public IUnknown {
        public:

            GraphicParameters(Renderer *pp) : pParent(pp) {}

            //   IUnknown

            STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
            ULONG AddRef() { return pParent -> AddRef(); }
            ULONG Release() { return pParent -> Release(); }

        private:

            STDMETHOD(put_LineWidth)(FLOAT lw);
            STDMETHOD(put_LineJoin)(long lj);
            STDMETHOD(put_LineCap)(long lc);
            STDMETHOD(put_LineDash)(FLOAT *pValues,long countValues,FLOAT offset);
            STDMETHOD(put_RGBColor)(COLORREF rgb);

            Renderer *pParent{NULL};

            FLOAT lineWidth{defaultLineWidth};

            D2D1_CAP_STYLE lineCap{defaultLineCap};
            D2D1_LINE_JOIN lineJoin{defaultLineJoin};
            D2D1_DASH_STYLE lineDashStyle{defaultDashStyle};
            DWORD countDashSizes{0};

            COLORREF rgbColor{defaultRGBColor};

            boolean doFill{false};

            long hashCode(char *pszLineSettings);

            void resetParameters(char *pszLineSettings);

            static FLOAT defaultLineWidth;
            static D2D1_CAP_STYLE defaultLineCap;
            static D2D1_LINE_JOIN defaultLineJoin;
            static D2D1_DASH_STYLE defaultDashStyle;
            static COLORREF defaultRGBColor;

            static ID2D1StrokeStyle1 *pID2D1StrokeStyle1;

            friend class Renderer;

        } *pIGraphicParameters{NULL};

        void setupRenderer();
        HRESULT internalRender();

        HDC hdc{NULL};

        ID2D1Factory1 *pID2D1Factory1{NULL};

        ID2D1PathGeometry *pID2D1PathGeometry{NULL};
        ID2D1GeometrySink *pID2D1GeometrySink{NULL};

        ID2D1SolidColorBrush *pID2D1SolidColorBrush{NULL};
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