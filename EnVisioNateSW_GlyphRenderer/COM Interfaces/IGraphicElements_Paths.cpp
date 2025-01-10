#include "EnVisioNateSW_GlyphRenderer.h"


    HRESULT GlyphRenderer::GraphicElements::NewPath() {
    savePath = true;
    pParent -> pIGraphicElements -> isFigureStarted = false;
    return S_OK;
    }


    HRESULT GlyphRenderer::GraphicElements::StrokePath() {
    applyPath(false);
    clearPath();
    savePath = false;
    return S_OK;
    }


    HRESULT GlyphRenderer::GraphicElements::FillPath() {
    applyPath(true);
    clearPath();
    savePath = false;
    return S_OK;
    }


    void GlyphRenderer::GraphicElements::applyPath(BOOL doFill) {

    //HPEN hPen = CreatePen(PS_SOLID,0,RGB(255,0,0));
    //HGDIOBJ oldObj = SelectObject(hdc,hPen);

    for ( GraphicElements::primitive *pPrimitive : thePath ) {

        switch ( pPrimitive -> theType) {
        case primitive::type::move:
            if ( pParent -> pIGraphicElements -> isFigureStarted )
                pParent -> pID2D1GeometrySink -> EndFigure(D2D1_FIGURE_END_CLOSED);
            pParent -> pID2D1GeometrySink -> BeginFigure(D2D1::Point2F(pPrimitive -> vertices[0].x,pPrimitive -> vertices[0].y),doFill ? D2D1_FIGURE_BEGIN_FILLED : D2D1_FIGURE_BEGIN_HOLLOW );
            pParent -> pIGraphicElements -> isFigureStarted = true;
            continue;

        case primitive::type::line:
            pParent -> pID2D1GeometrySink -> AddLine(D2D1::Point2F(pPrimitive -> vertices[0].x,pPrimitive -> vertices[0].y));
            continue;

        case primitive::type::arc:
            continue;

        case primitive::type::cubicBezier:
            pParent -> pID2D1GeometrySink -> AddBezier(&pPrimitive -> bezierSegment);
            continue;

        case primitive::type::quadraticBezier:
            pParent -> pID2D1GeometrySink -> AddQuadraticBezier(&pPrimitive -> quadraticBezierSegment);
            continue;

        default:
            continue;
        }

    }

    if ( pParent -> pIGraphicElements -> isFigureStarted )
        pParent -> pID2D1GeometrySink -> EndFigure(D2D1_FIGURE_END_CLOSED);

    pParent -> pIGraphicElements -> isFigureStarted = false;

    return;
    }
