#pragma once

#include <d2d1.h>
#include "GlyphRenderer_i.h"

//int Mx3Inverse(double *sourceMatrix,double *targetMatrix);

    struct pathParameters {

    public:

        pathParameters();
        pathParameters(pathParameters &rhs);
        ~pathParameters();

        void initialize();

        void initMatrix(HWND hwndClient,long pageNumber,long pageHeightPoints);

        // Graphics primitives

        void moveto(GS_POINT *pPt);
        void moveto(POINT_TYPE x,POINT_TYPE y);

        void rmoveto(GS_POINT *pPt);
        void rmoveto(POINT_TYPE x,POINT_TYPE y);

        void lineto(GS_POINT *pPt);
        void lineto(POINT_TYPE x,POINT_TYPE y);

        void rlineto(GS_POINT *pPt);
        void rlineto(POINT_TYPE x,POINT_TYPE );

        void arcto(POINT_TYPE xCenter,POINT_TYPE yCenter,POINT_TYPE radius,POINT_TYPE angle1,POINT_TYPE angle2);

        void curveto(FLOAT x1,FLOAT y1,FLOAT x2,FLOAT y2,FLOAT x3,FLOAT y3);

        void dot(GS_POINT at,POINT_TYPE radius);

        void openGeometry();
        void closeGeometry();
        void renderGeometry();

        static void RenderGeometry();

        void outlinePage(long pageWidth,long pageHeight);

        GS_POINT *CurrentDevicePoint() { return &currentDevicePoint; }

        // Coordinate systems

        static POINT_TYPE *ToDeviceSpace() { return toDeviceSpace; }

        void transformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2);
        void transformPointInPlace(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2);

        void untransformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *x2,POINT_TYPE *y2);
        void untransformPointInPlace(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *x2,POINT_TYPE *y2);

        void scalePoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *px2,POINT_TYPE *py2);

        void transform(GS_POINT *pPoints,uint16_t pointCount);
        void transformInPlace(GS_POINT *pPoints,uint16_t pointCount);

        void translate(GS_POINT *pPoints,uint16_t pointCount,GS_POINT *pToPoint);
        void scale(GS_POINT *pPoints,uint16_t pountCount,POINT_TYPE scale);

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
        void revertToGDI();

    private:

        class GraphicElements : IGraphicElements {
        public:

            GraphicElements(pathParameters *pp) : pParent(pp) {}

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

        IGraphicElements *pIGraphicElements_GDI{NULL};
        IGraphicElements *pIGraphicElements{NULL};

        void rasterizePath();
        void rasterizePathFreetype();

        void getGeometry(long *pCountVertices,BYTE **pVertexTypes,POINT **pVertexPoints);

        static POINT_TYPE toDeviceSpace[6];
        static POINT_TYPE toDeviceSpaceInverse[6];

        POINT_TYPE totalRotation{0.0};

        static boolean isPathActive;
        boolean defaultDoRasterization{false};

        GS_POINT currentUserPoint POINT_TYPE_NAN_POINT;
        GS_POINT currentDevicePoint POINT_TYPE_NAN_POINT;
        GS_POINT userSpaceDomain POINT_TYPE_NAN_POINT;

        POINT_TYPE scalePointsToPixels{1.0};
        POINT_TYPE renderingHeight{0.0};

        POINT currentGDIPoint{-1L,-1L};

        static long cxClient;
        static long cyClient;
        static long cyWindow;
        static long displayResolution;

    };
