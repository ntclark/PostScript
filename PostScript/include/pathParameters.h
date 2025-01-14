#pragma once

#include <d2d1.h>
#include "GlyphRenderer_i.h"

    struct pathParameters {

    public:

        pathParameters();
        pathParameters(pathParameters *pRHS);
        ~pathParameters();

        void initialize();

        void initMatrix(HWND hwndClient,long pageNumber,long pageHeightPoints);

        // Graphics primitives

        void moveto(GS_POINT *pPt);
        void moveto(FLOAT x,FLOAT y);

        void rmoveto(GS_POINT *pPt);
        void rmoveto(FLOAT x,FLOAT y);

        void lineto(GS_POINT *pPt);
        void lineto(FLOAT x,FLOAT y);

        void rlineto(GS_POINT *pPt);
        void rlineto(FLOAT x,FLOAT );

        void arcto(FLOAT xCenter,FLOAT yCenter,FLOAT radius,FLOAT angle1,FLOAT angle2);

        void curveto(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2,FLOAT x3,FLOAT y3);

        void dot(GS_POINT at,FLOAT radius);

        void openGeometry();
        void closeGeometry();
        void renderGeometry();

        static void RenderGeometry();

        void outlinePage(long pageWidth,long pageHeight);

        GS_POINT *CurrentDevicePoint() { return &currentDevicePoint; }

        // Coordinate systems

        static XFORM *ToDeviceSpace() { return &toDeviceSpace; }

        void transformPoint(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2);
        void transformPointInPlace(FLOAT x,FLOAT y,FLOAT *pX2,FLOAT *pY2);

        void untransformPoint(FLOAT x,FLOAT y,FLOAT *x2,FLOAT *y2);
        void untransformPointInPlace(FLOAT x,FLOAT y,FLOAT *x2,FLOAT *y2);

        void scalePoint(FLOAT x,FLOAT y,FLOAT *px2,FLOAT *py2);

        void transform(GS_POINT *pPoints,uint16_t pointCount);
        void transformInPlace(GS_POINT *pPoints,uint16_t pointCount);

        void translate(GS_POINT *pPoints,uint16_t pointCount,GS_POINT *pToPoint);
        void scale(GS_POINT *pPoints,uint16_t pountCount,FLOAT scale);

        // Paths

        void newpath();
        void stroke();
        void closepath();
        void fillpath();
        void eofillpath();

        boolean setDefaultToRasterize(boolean doRasterization) { 
            boolean oldVal = defaultDoRasterization ; 
            defaultDoRasterization = doRasterization; 
            return oldVal; 
        }

        void forwardToRenderer();
        void revertToLocal();

    private:

        class GraphicElements : IGraphicElements {
        public:

            GraphicElements(pathParameters *pp);

            //   IUnknown

            STDMETHOD (QueryInterface)(REFIID riid,void **ppv) { return E_NOTIMPL; }
            STDMETHOD_ (ULONG, AddRef)() { return 0; }
            STDMETHOD_ (ULONG, Release)() { return 0; }

        private:

            STDMETHOD(NewPath)();
            STDMETHOD(StrokePath)();
            STDMETHOD(FillPath)();

            STDMETHOD(MoveTo)(FLOAT x,FLOAT y);
            STDMETHOD(MoveToRelative)(FLOAT x,FLOAT y);
            STDMETHOD(LineTo)(FLOAT x,FLOAT y);
            STDMETHOD(LineToRelative)(FLOAT x,FLOAT y);
            STDMETHOD(ArcTo)(FLOAT xCenter,FLOAT yCenter,FLOAT radius,FLOAT angle1,FLOAT angle2);
            STDMETHOD(CubicBezierTo)(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2,FLOAT x3,FLOAT y3);
            STDMETHOD(QuadraticBezierTo)(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2);

            pathParameters *pParent{NULL};

            friend struct pathParameters;

        };

        IGraphicElements *pIGraphicElements_Local{NULL};
        IGraphicElements *pIGraphicElements{NULL};

        void rasterizePath();
        void rasterizePathFreetype();

        void getGeometry(long *pCountVertices,BYTE **pVertexTypes,POINT **pVertexPoints);

        FLOAT totalRotation{0.0};

        boolean defaultDoRasterization{false};

        GS_POINT currentUserPoint POINT_TYPE_NAN_POINT;
        GS_POINT currentDevicePoint POINT_TYPE_NAN_POINT;
        GS_POINT userSpaceDomain POINT_TYPE_NAN_POINT;

        POINT currentGDIPoint{-1L,-1L};

        static boolean isPathActive;
        static long cxClient;
        static long cyClient;
        static long cyWindow;
        static long displayResolution;

        static FLOAT scalePointsToPixels;
        static FLOAT renderingHeight;

        static XFORM toDeviceSpace;
        static XFORM toDeviceSpaceInverse;

    };
