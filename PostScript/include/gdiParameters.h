#pragma once

    class gdiParameters {
    public:

        gdiParameters();

        gdiParameters(gdiParameters *pRHS);

        ~gdiParameters();

        void initialize();

        void setupDC();

        void forwardToRenderer();

        void revertToGDI() {
            pIGraphicParameters = pIGraphicParameters_GDI;
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

        } *pIGraphicParameters_GDI{NULL};

        IGraphicParameters *pIGraphicParameters{NULL};

        POINT_TYPE lineWidth{gdiParameters::defaultLineWidth};
        long lineCap{gdiParameters::defaultLineCap};
        long lineJoin{gdiParameters::defaultLineJoin};
        COLORREF rgbColor{gdiParameters::defaultRGBColor};
        DWORD *pLineStyles{NULL};
        DWORD countDashSizes{0};
        colorSpace *pColorSpace{NULL};

        void setLineCap(long v);
        void setLineJoin(long v);
        void setLineWidth(POINT_TYPE v);
        void setLineDash(POINT_TYPE *pValues,long countValues,POINT_TYPE offset);

        void setColorSpace(colorSpace *pColorSpace);
        colorSpace *getColorSpace();

        void setColor(colorSpace *pColorSpace);
        void setRGBColor(COLORREF rgb);
        void setRGBColor(POINT_TYPE r,POINT_TYPE g,POINT_TYPE b);

        static POINT_TYPE defaultLineWidth;
        static long defaultLineCap;
        static long defaultLineJoin;
        static COLORREF defaultRGBColor;

    };
