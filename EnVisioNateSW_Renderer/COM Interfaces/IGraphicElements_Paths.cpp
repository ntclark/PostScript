/*
Copyright 2025 EnVisioNate LLC

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the “Software”), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is the MIT License
*/

#include "Renderer.h"


    HRESULT Renderer::GraphicElements::NewPath() {
/*
    newpath 
        – newpath –

    initializes the current path in the graphics state to an empty path. The current
    point becomes undefined.
*/

    if ( ! ( NULL == pCurrentPath ) ) {
        if ( ! pCurrentPath -> isClosed() ) {
            primitive *p = pCurrentPath -> pLastPrimitive;
            if ( p == pCurrentPath -> pFirstPrimitive -> pNextPrimitive && p -> theType == primitive::type::move ) {

                // This is a newpath - moveto, and no other primitives.
                // 
                // I *believe* this happens when there is a move to set the current point
                // and then a newpath. Example, a Type 3 font drawing procedure for a glyph.
                // It's first command is newpath, then moveto. So what would the point of
                // *THIS* moveto which occurs just before drawing the glyph ????
                //
                // I believe it makes sense. In my implementation of draw type 3 glyph
                // I set the current translation matrix to move to the current user space point
                // and in the sample '.ps document I see this happening
                //
                removePath(pCurrentPath);
            } else
                ClosePath();
        }
    }

    addPath(new path(pParent,this));

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

    primitive *p = pCurrentPath -> pLastPrimitive;

    while ( ! ( p == pCurrentPath -> pFirstPrimitive ) && ! ( p -> theType == primitive::type::move ) )
        p = p -> pPriorPrimitive;

    if ( p == pCurrentPath -> pFirstPrimitive ) {
        sprintf(Renderer::szErrorMessage,"Warning: a ClosePath was issued, however, the current path does not have a MoveTo location");
        pParent -> pIConnectionPointContainer -> fire_ErrorNotification(szErrorMessage);
        return E_UNEXPECTED;
    }

    if ( ! ( currentPageSpacePoint.x == p -> vertex.x ) || ! ( currentPageSpacePoint.y == p -> vertex.y ) ) {
        POINTF ptLine{p -> vertex.x,p -> vertex.y};
        pCurrentPath -> addPrimitive(new linePrimitive(this,&ptLine));
    }

    pCurrentPath -> addPrimitive(new primitive(this,primitive::type::closePathMarker));

    pCurrentPath -> isFillPath = false;
    pCurrentPath -> hashCode = pParent -> pIGraphicParameters -> hashCode(pCurrentPath -> szLineSettings,false);

    if ( ! pParent -> renderLive )
        return S_OK;

    if ( NULL == pParent -> pID2D1DCRenderTarget ) {
        sprintf(Renderer::szErrorMessage,"SetRenderLive has not been called. Call that method first");
        pParent -> pIConnectionPointContainer -> fire_ErrorNotification(szErrorMessage);
        return E_UNEXPECTED;
    }

    pParent -> setupPathAndSink();

    pParent -> pIGraphicParameters -> resetParameters(pCurrentPath -> szLineSettings);

    GraphicElements::path::pathAction pa = pCurrentPath -> apply(false);

    pParent -> closeSink();

    pParent -> strokeRender();

    removePath(pCurrentPath);

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

    if ( ! pParent -> renderLive )
        return S_OK;

    if ( NULL == pParent -> pID2D1DCRenderTarget ) {
        sprintf(Renderer::szErrorMessage,"SetRenderLive has not been called. Call this method first");
        pParent -> pIConnectionPointContainer -> fire_ErrorNotification(szErrorMessage);
        return E_UNEXPECTED;
    }

    pParent -> setupPathAndSink();

    pParent -> pIGraphicParameters -> resetParameters(pCurrentPath -> szLineSettings);

    GraphicElements::path::pathAction pa = pCurrentPath -> apply(false);

    pParent -> closeSink();

    pParent -> strokeRender();

    removePath(pCurrentPath);

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
*/

    if ( NULL == pCurrentPath )
        return E_UNEXPECTED;

    pCurrentPath -> addPrimitive(new primitive(this,primitive::type::fillPathMarker));

    pCurrentPath -> isFillPath = true;
    pCurrentPath -> hashCode = pParent -> pIGraphicParameters -> hashCode(pCurrentPath -> szLineSettings,true);

    if ( ! pParent -> renderLive ) {
        NewPath();
        return S_OK;
    }

    if ( NULL == pParent -> pID2D1DCRenderTarget ) {
        sprintf(Renderer::szErrorMessage,"SetRenderLive has not been called. Call this method first");
        pParent -> pIConnectionPointContainer -> fire_ErrorNotification(szErrorMessage);
        return E_UNEXPECTED;
    }

    pParent -> setupPathAndSink();

    pParent -> pIGraphicParameters -> resetParameters(pCurrentPath -> szLineSettings);

    GraphicElements::path::pathAction pa = pCurrentPath -> apply(true);

    pParent -> closeSink();

    pParent -> fillRender();

    removePath(pCurrentPath);

    NewPath();

    return S_OK;
    }


    Renderer::GraphicElements::path::pathAction Renderer::GraphicElements::path::apply(boolean doFill) {

    primitive *p = pFirstPrimitive;

    HRESULT hr = S_OK;

    while ( ! ( NULL == p ) ) {

        p -> transform();

        p -> logPrimitive();

        switch ( p -> theType) {

        case primitive::type::newPathMarker: {
            if ( pParent -> isFigureStarted )
                pRenderer -> pID2D1GeometrySink -> EndFigure(D2D1_FIGURE_END_CLOSED);
            pParent -> isFigureStarted = false;
            }
            break;

        case primitive::type::closePathMarker: {
            if ( pParent -> isFigureStarted )
                pRenderer -> pID2D1GeometrySink -> EndFigure(D2D1_FIGURE_END_CLOSED);
            pParent -> isFigureStarted = false;
            //return path::pathAction::close; //<--- causes fuzzy poor resolution !!!
            }
            break;

        case primitive::type::fillPathMarker:
            if ( pParent -> isFigureStarted )
                pRenderer -> pID2D1GeometrySink -> EndFigure(D2D1_FIGURE_END_CLOSED);
            pParent -> isFigureStarted = false;
            return path::pathAction::fill;

        case primitive::type::strokePathMarker:
            if ( pParent -> isFigureStarted )
                pRenderer -> pID2D1GeometrySink -> EndFigure(D2D1_FIGURE_END_OPEN);
            pParent -> isFigureStarted = false;
            return path::pathAction::stroke;

        case primitive::type::move:
            if ( pParent -> isFigureStarted )
                pRenderer -> pID2D1GeometrySink -> EndFigure(doFill ? D2D1_FIGURE_END_CLOSED : D2D1_FIGURE_END_OPEN);
            pRenderer -> pID2D1GeometrySink -> BeginFigure(D2D1::Point2F(p -> vertex.x,p -> vertex.y),doFill ? D2D1_FIGURE_BEGIN_FILLED : D2D1_FIGURE_BEGIN_HOLLOW );
            pParent -> isFigureStarted = true;
            break;

        case primitive::type::line:
            pRenderer -> pID2D1GeometrySink -> AddLine(D2D1::Point2F(p -> vertex.x,p -> vertex.y));
            break;

        case primitive::type::arc:
            pRenderer -> pID2D1GeometrySink -> AddArc((D2D1_ARC_SEGMENT *)p -> dataOne());
            break;

        case primitive::type::ellipse: {
            ID2D1EllipseGeometry *pEllipseGeometry = NULL;
            hr = pRenderer -> pID2D1Factory1 -> CreateEllipseGeometry((D2D1_ELLIPSE *)p -> dataOne(),&pEllipseGeometry);
            specializedSink *pSpecializedSink = new specializedSink(this,pRenderer -> pID2D1GeometrySink);
            D2D1_MATRIX_3X2_F theMatrix{0};
            theMatrix.m11 = 1.0f;
            theMatrix.m22 = 1.0f;
            hr = pEllipseGeometry -> Simplify(D2D1_GEOMETRY_SIMPLIFICATION_OPTION_CUBICS_AND_LINES,
                                                (D2D1_MATRIX_3X2_F *)&theMatrix,
                                                reinterpret_cast<ID2D1SimplifiedGeometrySink *>(pSpecializedSink));
            pSpecializedSink -> Release();
            }
            break;

        case primitive::type::cubicBezier:
            if ( ! pParent -> isFigureStarted ) {
                pRenderer -> pID2D1GeometrySink -> BeginFigure(D2D1::Point2F(p -> vertex.x,p -> vertex.y),doFill ? D2D1_FIGURE_BEGIN_FILLED : D2D1_FIGURE_BEGIN_HOLLOW );
                pParent -> isFigureStarted = true;
            }
            pRenderer -> pID2D1GeometrySink -> AddBezier((D2D1_BEZIER_SEGMENT *)p -> dataOne());
            break;

        case primitive::type::quadraticBezier:
            if ( ! pParent -> isFigureStarted ) {
                pRenderer -> pID2D1GeometrySink -> BeginFigure(D2D1::Point2F(p -> vertex.x,p -> vertex.y),doFill ? D2D1_FIGURE_BEGIN_FILLED : D2D1_FIGURE_BEGIN_HOLLOW );
                pParent -> isFigureStarted = true;
            }
            pRenderer -> pID2D1GeometrySink -> AddQuadraticBezier((D2D1_QUADRATIC_BEZIER_SEGMENT *)p -> dataOne());
            break;

        default:
MessageBox(NULL,"Unknown primitive","Error",MB_OK);
            break;
        }

        p = p -> pNextPrimitive;

    }

    return pathAction::none;
    }
