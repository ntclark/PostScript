#include "job.h"

#include "pathParameters.h"

//#include <dwrite.h>

#include <algorithm>
#include <limits.h>

   int MxM(double *a,double *b,double *c);
/*
#define PT_CLOSEFIGURE      0x01
#define PT_LINETO           0x02
#define PT_BEZIERTO         0x04
#define PT_MOVETO           0x06
*/

    void pathParameters::rasterizePath() {

rasterizePathFreetype();
return;

    int cntVertices = GetPath(pPStoPDF -> GetDC(),NULL,NULL,0);

    if ( 0 == cntVertices ) 
        return;

    HRESULT hr = S_OK;

    if ( NULL == pID2D1Factory ) {

        D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED,&pID2D1Factory);

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_IGNORE),
                96,96,
                D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,D2D1_FEATURE_LEVEL_10);

        hr = pID2D1Factory -> CreateDCRenderTarget(&props, &pID2D1DCRenderTarget);

        RECT rcImage;
        GetClientRect(pPStoPDF -> HwndClient(),&rcImage);

        pID2D1DCRenderTarget -> BindDC(pPStoPDF -> GetDC(),&rcImage);
        pID2D1DCRenderTarget -> CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.0f),&pID2D1SolidColorBrush);
        pID2D1DCRenderTarget -> SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

    }

    D2D1_TAG tag1;
    D2D1_TAG tag2;

    POINT *pThePoints = new POINT[cntVertices];
    BYTE *pTheVertexTypes = new BYTE[cntVertices];

    GetPath(pPStoPDF -> GetDC(),pThePoints,pTheVertexTypes,cntVertices);

    ID2D1PathGeometry *pID2D1PathGeometry = NULL;
    ID2D1GeometrySink *pID2D1GeometrySink = NULL;

    pID2D1Factory -> CreatePathGeometry(&pID2D1PathGeometry);

    pID2D1PathGeometry -> Open(&pID2D1GeometrySink);

    pID2D1GeometrySink -> SetFillMode(D2D1_FILL_MODE_ALTERNATE);

    POINT *pPoint = pThePoints;
    POINT *pLastPoint = pThePoints + cntVertices;
    BYTE *pVertexType = pTheVertexTypes;

    boolean isFigureStarted = false;

    do {

        uint16_t delta = 1;

        if ( *pVertexType & PT_CLOSEFIGURE ) {
            pID2D1GeometrySink -> EndFigure(D2D1_FIGURE_END_CLOSED);
            isFigureStarted = false;
        }

        if ( PT_MOVETO == *pVertexType ) {

            if ( isFigureStarted )
                pID2D1GeometrySink -> EndFigure(D2D1_FIGURE_END_CLOSED);

            pID2D1GeometrySink -> BeginFigure(D2D1::Point2F((FLOAT)pPoint -> x,(FLOAT)pPoint -> y),D2D1_FIGURE_BEGIN_FILLED);
            isFigureStarted = true;

        } else if ( PT_LINETO == *pVertexType ) {

            pID2D1GeometrySink -> AddLine(D2D1::Point2F((FLOAT)pPoint -> x,(FLOAT)pPoint -> y));

        } else if ( PT_BEZIERTO == *pVertexType ) {

            pID2D1GeometrySink -> AddBezier(D2D1::BezierSegment(
                D2D1::Point2F((FLOAT)pPoint -> x,(FLOAT)pPoint -> y),
                D2D1::Point2F((FLOAT)(pPoint + 1) -> x,(FLOAT)(pPoint + 1) -> y),
                D2D1::Point2F((FLOAT)(pPoint + 2) -> x,(FLOAT)(pPoint + 2) -> y)));

            delta = 3;

        }

        pPoint += delta;
        pVertexType += delta;

    } while ( pPoint < pLastPoint);

    if ( isFigureStarted )
        pID2D1GeometrySink -> EndFigure(D2D1_FIGURE_END_CLOSED);

    pID2D1GeometrySink -> Close();
    pID2D1GeometrySink -> Release();
    pID2D1GeometrySink = NULL;

    D2D1_MATRIX_3X2_F unTrans;

    unTrans.m11 = (FLOAT)graphicsState::scalePointsToPixels;
    unTrans.m21 = 0.0f;
    unTrans.m12 = 0.0f;
    unTrans.m22 = (FLOAT)-graphicsState::scalePointsToPixels;

    unTrans.dx = 0.0f;
    unTrans.dy = (FLOAT)graphicsState::cyWindow;

    SetWorldTransform(pPStoPDF -> GetDC(),pIdentityTransform());

    pID2D1DCRenderTarget -> SetTransform(unTrans);

    pID2D1DCRenderTarget -> BeginDraw();
    pID2D1DCRenderTarget -> FillGeometry(pID2D1PathGeometry,(ID2D1Brush *)pID2D1SolidColorBrush);
    pID2D1DCRenderTarget -> EndDraw(&tag1,&tag2);

    pID2D1PathGeometry -> Release();

    SetWorldTransform(pPStoPDF -> GetDC(),pTransform());

    delete [] pThePoints;
    delete [] pTheVertexTypes;

    return;
    }

