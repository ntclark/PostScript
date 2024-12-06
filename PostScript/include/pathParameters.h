#pragma once

#include <d2d1.h>

    struct savedPrimitive {
        savedPrimitive(GS_POINT pt) {
            vertices.push_back(new GS_POINT(pt));
        }
        savedPrimitive(GS_POINT pt1,GS_POINT pt2,GS_POINT pt3) {
            vertices.push_back(new GS_POINT(pt1));
            vertices.push_back(new GS_POINT(pt2));
            vertices.push_back(new GS_POINT(pt3));
        }
        std::list<GS_POINT *> vertices;
        ~savedPrimitive() {
            for ( GS_POINT *pPt : vertices )
                delete pPt;
            vertices.clear();
        }
    };

    struct savedPath {
        void addMove(GS_POINT m) {
            primitiveType.push_back('m');
            thePrimitive.push_back(new savedPrimitive(m));
        }
        void addLine(GS_POINT m) {
            primitiveType.push_back('l');
            thePrimitive.push_back(new savedPrimitive(m));
        }
        void addCurve(GS_POINT pt1,GS_POINT pt2,GS_POINT pt3) {
            primitiveType.push_back('c');
            thePrimitive.push_back(new savedPrimitive(pt1,pt2,pt3));
        }
        std::list<char> primitiveType;
        std::list<savedPrimitive *> thePrimitive;
    };


    struct pathParameters {

    public:

        pathParameters();
        pathParameters(pathParameters &rhs);
        ~pathParameters();

        void initialize();

        // Graphics primitives

        void moveto(POINT_TYPE x,POINT_TYPE y);
        void moveto(GS_POINT *pPt);

        void rmoveto(POINT_TYPE x,POINT_TYPE y);
        void rmoveto(GS_POINT pt);

        void lineto(POINT_TYPE x,POINT_TYPE y);
        void lineto(GS_POINT *pPt);

        void rlineto(POINT_TYPE x,POINT_TYPE );
        void rlineto(GS_POINT);

        void curveto(GS_POINT *pPoints);

        void arcto(POINT_TYPE xCenter,POINT_TYPE yCenter,POINT_TYPE radius,POINT_TYPE angle1,POINT_TYPE angle2);

        void dot(GS_POINT at,POINT_TYPE radius);

        // Coordinate systems

        void setMatrix(POINT_TYPE *pValues);
        void currentMatrix();
        void revertMatrix();

        void concat(matrix *pSource);
        void concat(class array *pSource);
        void concat(POINT_TYPE *);
        void concat(XFORM &);

        void translate(POINT_TYPE x,POINT_TYPE y);
        void rotate(POINT_TYPE angle);
        void scale(POINT_TYPE scaleX,POINT_TYPE scaleY);

        void transformPoint(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2);
        void transformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2);
        void transformPointInPlace(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2);
        void transformPointInPlace(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *pX2,POINT_TYPE *pY2);

        void untransformPoint(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *x2,POINT_TYPE *y2);
        void untransformPoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *x2,POINT_TYPE *y2);
        void untransformPointInPlace(class matrix *pMatrix,POINT_TYPE x,POINT_TYPE y,POINT_TYPE *x2,POINT_TYPE *y2);
        void untransformPointInPlace(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *x2,POINT_TYPE *y2);

        void scalePoint(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *px2,POINT_TYPE *py2);

        void transform(GS_POINT *pPoints,uint16_t pointCount);
        void transformInPlace(GS_POINT *pPoints,uint16_t pointCount);

        void translate(GS_POINT *pPoints,uint16_t pointCount,GS_POINT *pToPoint);
        void scale(GS_POINT *pPoints,uint16_t pountCount,POINT_TYPE scale);

        // Paths

        void newpath(POINT_TYPE x = POINT_TYPE_NAN,POINT_TYPE y = POINT_TYPE_NAN);
        void stroke();
        void closepath();
        void fillpath(boolean doRasterization = false);
        void eofillpath(boolean doRasterization = false);

        boolean savePath(boolean doSave);

        boolean setDefaultToRasterize(boolean doRasterization) { 
            boolean oldVal = defaultDoRasterization ; 
            defaultDoRasterization = doRasterization; 
            return oldVal; 
        }

    private:

        void rasterizePath();
        void rasterizePathFreetype();

        static XFORM *pTransform();
        static XFORM *pIdentityTransform();

        static void untransformGDI(POINT_TYPE x,POINT_TYPE y,POINT_TYPE *px,POINT_TYPE *py);

        static matrix *pUserSpaceToDeviceSpace;

        static double untransformGDIMatrix[3][3];
        static double untransformPDFMatrix[3][3];

        matrix *pToUserSpace{NULL};

        POINT_TYPE totalRotation{0.0};

        boolean isPathActive{false};
        boolean defaultDoRasterization{false};
        boolean saveThePath{false};

        savedPath *pSavedPath{NULL};

        GS_POINT pathBeginPoint POINT_TYPE_NAN_POINT;
        GS_POINT currentUserSpacePoint POINT_TYPE_NAN_POINT;
        GS_POINT currentPointsPoint POINT_TYPE_NAN_POINT;
        GS_POINT userSpaceDomain POINT_TYPE_NAN_POINT;

        POINT currentGDIPoint{-1L,-1L};

        static ID2D1Factory *pID2D1Factory;
        static ID2D1SolidColorBrush *pID2D1SolidColorBrush;
        static ID2D1DCRenderTarget *pID2D1DCRenderTarget;

        friend class graphicsState;

    };
