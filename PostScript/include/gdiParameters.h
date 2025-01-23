#pragma once

#ifdef USE_RENDERER
#include "Renderer_i.h"
#else
#include "GlyphRenderer_i.h"
#endif

    class gdiParameters {
    public:

        gdiParameters();
        gdiParameters(gdiParameters *pRHS);

        ~gdiParameters();

        void initialize();

        void setupDC();

#ifdef USE_RENDERER
#else
        void forwardToRenderer();

        void revertToGDI() {
            pIGraphicParameters = pIGraphicParameters_Local;
        }

        class GraphicParameters : IGraphicParameters {
        public:

            GraphicParameters(gdiParameters *pp) : pParent(pp) {}

            //   IUnknown

            STDMETHOD (QueryInterface)(REFIID riid,void **ppv) { return E_NOTIMPL; }
            STDMETHOD_ (ULONG, AddRef)() { return 0; }
            STDMETHOD_ (ULONG, Release)() { return 0; }

        private:

            STDMETHOD(put_LineWidth)(FLOAT lw);
            STDMETHOD(put_LineJoin)(long lj);
            STDMETHOD(put_LineCap)(long lc);
            STDMETHOD(put_LineDash)(FLOAT *pValues,long countValues,FLOAT offset);
            STDMETHOD(put_RGBColor)(COLORREF rgb);

            gdiParameters *pParent{NULL};

            friend class gdiParameters;

        } *pIGraphicParameters_Local{NULL};

#endif

        IGraphicParameters *pIGraphicParameters{NULL};

        FLOAT lineWidth{gdiParameters::defaultLineWidth};
        long lineCap{gdiParameters::defaultLineCap};
        long lineJoin{gdiParameters::defaultLineJoin};
        COLORREF rgbColor{gdiParameters::defaultRGBColor};
        DWORD *pLineStyles{NULL};
        DWORD countDashSizes{0};
        colorSpace *pColorSpace{NULL};

        void setLineCap(long v);
        void setLineJoin(long v);
        void setLineWidth(FLOAT v);
        void setLineDash(FLOAT *pValues,long countValues,FLOAT offset);

        void setColorSpace(colorSpace *pColorSpace);
        colorSpace *getColorSpace();

        void setColor(colorSpace *pColorSpace);
        void setRGBColor(COLORREF rgb);
        void setRGBColor(FLOAT r,FLOAT g,FLOAT b);

        static FLOAT defaultLineWidth;
        static long defaultLineCap;
        static long defaultLineJoin;
        static COLORREF defaultRGBColor;

    };
