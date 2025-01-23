#pragma once

#include "GlyphRenderer_i.h"
#include <vector>

    class GlyphRenderer : public IGlyphRenderer {
    public:

        GlyphRenderer(IUnknown *pUnkOuter);
        ~GlyphRenderer();

        //   IUnknown

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
        STDMETHOD_ (ULONG, AddRef)();
        STDMETHOD_ (ULONG, Release)();

    private:

        // IGlyphRenderer

        STDMETHOD(Prepare)(HDC hdc);
        STDMETHOD(put_TransformMatrix)(UINT_PTR /*XFORM */ pXformToDeviceSpace);
        STDMETHOD(get_IsPrepared)(BOOL *pIsPrepared);
        STDMETHOD(Render)();

        // IGraphicElements

        class GraphicElements : public IUnknown {
        public:

            GraphicElements(GlyphRenderer *pp) : pParent(pp) {}

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
                    quadraticBezier = 4
                };

                primitive(type t,POINTF *pPoint) : theType(t)  { 
                vertices[0].x = pPoint -> x;
                vertices[0].y = pPoint -> y;
                return;
                }

                primitive(type t,POINTF *pPoint1,POINTF *pPoint2,POINTF *pPoint3) : theType(t)  { 
                vertices[0].x = pPoint1 -> x;
                vertices[0].y = pPoint1 -> y;
                vertices[1].x = pPoint2 -> x;
                vertices[1].y = pPoint2 -> y;
                vertices[2].x = pPoint3 -> x;
                vertices[2].y = pPoint3 -> y;
                return;
                }

                primitive(D2D1_BEZIER_SEGMENT *pSegment) { 
                theType = cubicBezier;
                memcpy(&bezierSegment,pSegment,sizeof(D2D1_BEZIER_SEGMENT));
                return;
                }

                primitive(D2D1_QUADRATIC_BEZIER_SEGMENT  *pSegment) { 
                theType = quadraticBezier;
                memcpy(&quadraticBezierSegment,pSegment,sizeof(D2D1_QUADRATIC_BEZIER_SEGMENT));
                return;
                }

                type theType;

                POINTF vertices[3]{{0.0f,0.0f}, {0.0f,0.0f}, {0.0f,0.0f}};
                D2D1_BEZIER_SEGMENT bezierSegment{{0.0f,0.0f},{0.0f,0.0f},{0.0f,0.0f}};
                D2D1_QUADRATIC_BEZIER_SEGMENT quadraticBezierSegment{{0.0f,0.0f},{0.0f,0.0f}};

            };

            std::vector<primitive *> thePath;

            void applyPath(BOOL doFill);

            void clearPath() {
                for ( primitive *pPrimitive  : thePath )
                    delete pPrimitive;
                thePath.clear();
            }

            POINTF currentUserPoint{0,0};
            POINTF currentGDIPoint{0,0};
            BOOL isFigureStarted{false};
            BOOL savePath{false};

            XFORM toDeviceSpace{6 * 0.0f};
            void transformPoint(FLOAT *px,FLOAT *py);

            GlyphRenderer *pParent{NULL};

            friend class GlyphRenderer;

        } *pIGraphicElements{NULL};

        // IGraphicParameters

        class GraphicParameters : public IUnknown {
        public:

            GraphicParameters(GlyphRenderer *pp) : pParent(pp) {}

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

            GlyphRenderer *pParent{NULL};

            FLOAT lineWidth{defaultLineWidth};

            D2D1_CAP_STYLE lineCap;
            D2D1_LINE_JOIN lineJoin;

            COLORREF rgbColor{defaultRGBColor};
            FLOAT *pLineStyles{NULL};
            DWORD countDashSizes{0};

            BOOL doFill{false};

            static FLOAT defaultLineWidth;
            static long defaultLineCap;
            static long defaultLineJoin;
            static COLORREF defaultRGBColor;

            static ID2D1StrokeStyle1 *pID2D1StrokeStyle1;

            friend class GlyphRenderer;

        } *pIGraphicParameters{NULL};

        void setupRenderer();

        HDC hdc{NULL};

        ID2D1Factory1 *pID2D1Factory1{NULL};

        ID2D1PathGeometry *pID2D1PathGeometry{NULL};
        ID2D1GeometrySink *pID2D1GeometrySink{NULL};

        ID2D1SolidColorBrush *pID2D1SolidColorBrush{NULL};
        ID2D1DCRenderTarget *pID2D1DCRenderTarget{NULL};

        ULONG refCount{0};

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