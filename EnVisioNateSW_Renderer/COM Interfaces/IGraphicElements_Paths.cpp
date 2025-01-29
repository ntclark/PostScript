
#include "EnVisioNateSW_Renderer.h"


    HRESULT Renderer::GraphicElements::NewPath() {
/*
    newpath 
        – newpath –

    initializes the current path in the graphics state to an empty path. The current
    point becomes undefined.
*/

    if ( ! ( NULL == pCurrentPath ) && 0 == pCurrentPath -> hashCode ) 
        pCurrentPath -> hashCode = pParent -> pIGraphicParameters -> hashCode(pCurrentPath -> szLineSettings,pCurrentPath -> isFillPath);

    addPath(new path(pParent,this));

    pCurrentPath -> addPrimitive(new primitive(this,primitive::type::newPathMarker));

    return S_OK;
    }


    HRESULT Renderer::GraphicElements::ClosePath() {
/*
    closepath 
        – closepath –

    closes the current subpath by appending a straight line segment connecting the
    current point to the subpath’s starting point, which is generally the point most recently
    specified by moveto (see Section 4.4, “Path Construction”).
    closepath terminates the current subpath; appending another segment to the current
    path will begin a new subpath, even if the new segment begins at the endpoint
    reached by the closepath operation. If the current subpath is already closed
    or the current path is empty, closepath does nothing.
*/

    if ( NULL == pCurrentPath )
        return E_UNEXPECTED;

    pCurrentPath -> addPrimitive(new primitive(this,primitive::type::closePathMarker));

    return S_OK;
    }


    HRESULT Renderer::GraphicElements::StrokePath() {
/*
    stroke 
        – stroke –

    paints a line centered on the current path, with sides parallel to the path segments.
    The line’s graphical properties are defined by various parameters of the graphics
    state. Its thickness is determined by the current line width parameter (see
    setlinewidth) and its color by the current color (see setcolor). The joints between
    connected path segments and the ends of open subpaths are painted with the current
    line join (see setlinejoin) and the current line cap (see setlinecap), respectively.
    The line is either solid or broken according to the dash pattern established by
    setdash. Uniform stroke width can be ensured by enabling automatic stroke adjustment
    (see setstrokeadjust). All of these graphics state parameters are consulted
    at the time stroke is executed; their values during the time the path is being
    constructed are irrelevant.

    If a subpath is degenerate (consists of a single-point closed path or of two or more
    points at the same coordinates), stroke paints it only if round line caps have been
    specified, producing a filled circle centered at the single point. If butt or projecting
    square line caps have been specified, stroke produces no output, because the
    orientation of the caps would be indeterminate. A subpath consisting of a singlepoint
    open path produces no output.

    After painting the current path, stroke clears it with an implicit newpath operation. 
    To preserve the current path across a stroke operation, use the sequence

        gsave
        fill
        grestore
*/

    if ( NULL == pCurrentPath )
        return E_UNEXPECTED;

    pCurrentPath -> addPrimitive(new primitive(this,primitive::type::strokePathMarker));

    pCurrentPath -> isFillPath = false;
    pCurrentPath -> hashCode = pParent -> pIGraphicParameters -> hashCode(pCurrentPath -> szLineSettings,false);

    //pParent -> Render();

    return S_OK;
    }


    HRESULT Renderer::GraphicElements::FillPath() {
/*
    fill 
        – fill –

    paints the area inside the current path with the current color. The nonzero winding
    number rule is used to determine what points lie inside the path
    (see “Nonzero Winding Number Rule” on page 195).

    fill implicitly closes any open subpaths of the current path before painting. Any
    previous contents of the filled area are obscured, so an area can be erased by filling
    it with the current color set to white.

    After filling the current path, fill clears it with an implicit newpath operation. To
    preserve the current path across a fill operation, use the sequence
*/

    if ( NULL == pCurrentPath )
        return E_UNEXPECTED;

    pCurrentPath -> addPrimitive(new primitive(this,primitive::type::fillPathMarker));

    pCurrentPath -> isFillPath = true;
    pCurrentPath -> hashCode = pParent -> pIGraphicParameters -> hashCode(pCurrentPath -> szLineSettings,true);

    //pParent -> Render();

    return S_OK;
    }


    void Renderer::GraphicElements::path::apply(boolean doFill,boolean firstPass,FILE *fDebug) {

    if ( firstPass )
        pRenderer -> pIGraphicParameters -> resetParameters(szLineSettings);

    pParent -> isFigureStarted = false;

    primitive *p = pFirstPrimitive;

    while ( ! ( NULL == p ) ) {

        p -> transform();

        switch ( p -> theType) {

        case primitive::type::newPathMarker: {
            if ( pParent -> isFigureStarted )
                pRenderer -> pID2D1GeometrySink -> EndFigure(D2D1_FIGURE_END_CLOSED);
            pParent -> isFigureStarted = false;
            if ( ! ( NULL == fDebug ) )
                fprintf(fDebug,"newpath\n");
            }
            break;

        case primitive::type::closePathMarker: {
            if ( pParent -> isFigureStarted )
                pRenderer -> pID2D1GeometrySink -> EndFigure(D2D1_FIGURE_END_CLOSED);
            pParent -> isFigureStarted = false;
            if ( ! ( NULL == fDebug ) )
                fprintf(fDebug,"closepath\n");
            }
            break;

        case primitive::type::fillPathMarker: {
            if ( pParent -> isFigureStarted )
                pRenderer -> pID2D1GeometrySink -> EndFigure(D2D1_FIGURE_END_CLOSED);
            pParent -> isFigureStarted = false;
            if ( ! ( NULL == fDebug ) )
                fprintf(fDebug,"fillpath\n");
            }
            break;

        case primitive::type::strokePathMarker:
            if ( pParent -> isFigureStarted )
                pRenderer -> pID2D1GeometrySink -> EndFigure(D2D1_FIGURE_END_OPEN);
            pParent -> isFigureStarted = false;
            if ( ! ( NULL == fDebug ) )
                fprintf(fDebug,"strokepath\n");
            break;

        case primitive::type::move:
            if ( pParent -> isFigureStarted )
                pRenderer -> pID2D1GeometrySink -> EndFigure(doFill ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
            pRenderer -> pID2D1GeometrySink -> BeginFigure(D2D1::Point2F(p -> vertices[0].x,p -> vertices[0].y),doFill ? D2D1_FIGURE_BEGIN_FILLED : D2D1_FIGURE_BEGIN_HOLLOW );
            pParent -> isFigureStarted = true;
            if ( ! ( NULL == fDebug ) )
                fprintf(fDebug,"%f  %f  moveto\n",p -> vertices[0].x,p -> vertices[0].y);
            break;

        case primitive::type::line:
            pRenderer -> pID2D1GeometrySink -> AddLine(D2D1::Point2F(p -> vertices[0].x,p -> vertices[0].y));
            if ( ! ( NULL == fDebug ) )
                fprintf(fDebug,"%f  %f  lineto\n",p -> vertices[0].x,p -> vertices[0].y);
            break;

        case primitive::type::arc:
MessageBox(NULL,"Not implemented","Error",MB_OK);
            break;

        case primitive::type::cubicBezier:
            pRenderer -> pID2D1GeometrySink -> AddBezier(&p -> bezierSegment);
            if ( ! ( NULL == fDebug ) )
                fprintf(fDebug,"%f  %f  %f %f %f %f curveto %%cubic\n",
                                                p -> bezierSegment.point1.x,p -> bezierSegment.point1.y,
                                                p -> bezierSegment.point2.x,p -> bezierSegment.point2.y,
                                                p -> bezierSegment.point3.x,p -> bezierSegment.point3.y);
            break;

        case primitive::type::quadraticBezier:
            pRenderer -> pID2D1GeometrySink -> AddQuadraticBezier(&p -> quadraticBezierSegment);
            if ( ! ( NULL == fDebug ) )
                fprintf(fDebug,"%f  %f  %f %f quadcurveto %%quadratic\n",
                                                p -> quadraticBezierSegment.point1.x,p -> quadraticBezierSegment.point1.y,
                                                p -> quadraticBezierSegment.point2.x,p -> quadraticBezierSegment.point2.y);
            break;

        case primitive::type::colorSet: {
            pRenderer -> pID2D1SolidColorBrush -> Release();
            FLOAT r = (FLOAT)GetRValue(p -> theColor) / 255.0f;
            FLOAT g = (FLOAT)GetGValue(p -> theColor) / 255.0f;
            FLOAT b = (FLOAT)GetBValue(p -> theColor) / 255.0f;
            pRenderer -> pID2D1DCRenderTarget -> CreateSolidColorBrush(D2D1::ColorF(r,g,b, 1.0f),&pRenderer -> pID2D1SolidColorBrush);
            }
            break;

        case primitive::type::lineStyleSet:
            if ( ! ( NULL == pRenderer -> pID2D1StrokeStyle1 ) )
                pRenderer -> pID2D1StrokeStyle1 -> Release();

            pRenderer -> pID2D1Factory1 -> CreateStrokeStyle(
                    D2D1::StrokeStyleProperties1(
                        p -> theLineCap,
                        p -> theLineCap,
                        p -> theLineCap,
                        p -> theLineJoin,
                        10.0f,
                        p -> theLineDashStyle,
                        p -> theLineDashOffset,
                        D2D1_STROKE_TRANSFORM_TYPE_FIXED),
                        (FLOAT *)p -> pvData,
                        p -> countFloats,
                            &pRenderer -> pID2D1StrokeStyle1);
            break;

        default:
            break;
        }

        p = p -> pNext;

    }

    if ( pParent -> isFigureStarted )
        pRenderer -> pID2D1GeometrySink -> EndFigure(doFill ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);

    pParent -> isFigureStarted = false;

    return;
    }
