
#define NOMINMAX

#include "PostScript objects\graphicsState.h"

#include <math.h>
#include "utilities.h"

//#define CANVAS_ITY_IMPLEMENTATION
//#include "canvas_ity.hpp"
//#include <fstream>

#define NEXT_POINT { pX++; pY++; pFlag++; }

    char graphicsState::szCurrentText[4096];

    graphicsState::graphicsState(job *pj,HWND hwndSur) :

        matrix::matrix(pj),

        textMatrix(pj),
        textMatrixIdentity(pj),
        textLineMatrix(pj),
        textMatrixPDF(pj),
        textMatrixPDFIdentity(pj),
        textMatrixRotation(pj),

        textLineMatrixPDF(pj),

        hwndSurface(hwndSur)
    {

    hdcSurface = GetDC(hwndSurface);

    memset(&lastPoint,0,sizeof(POINTL));
    memset(&rcTextObjectWindows,0,sizeof(RECT));

    SetGraphicsMode(hdcSurface,GM_ADVANCED);
    SetMapMode(hdcSurface,MM_ANISOTROPIC);

    SetWorldTransform(hdcSurface,pTransform());

    return;
    }


    graphicsState::~graphicsState() {
    ReleaseDC(hwndSurface,hdcSurface);
    return;
    }


    void graphicsState::drawText(char *pszText) {
    DrawTextEx(hdcSurface,pszText,-1,NULL,0L,NULL);
    return;
    }


    void graphicsState::setTextMatrix(float *pValues) {

    sendText();

    textMatrix = textMatrixIdentity;
    textMatrixPDF = textMatrixPDFIdentity;

    textMatrix.concat(pValues);
    textMatrixPDF.concat(pValues);

    textLineMatrix = textMatrix;
    textLineMatrixPDF = textMatrixPDF;

    return;
    }


    void graphicsState::translateTextMatrix(float ptx,float pty) {
    float pValues[] = {1.0, 0.0, 0.0, 1.0, ptx, pty};
    textMatrix.concat(pValues);
    textMatrixPDF.concat(pValues);
    return;
    }


    void graphicsState::translateTextMatrixTJ(float ptx,float pty) {

    float pValues[] = {1.0, 0.0, 0.0, 1.0, -ptx * fontSize / 1000.0f, pty};

    textMatrix.concat(pValues);
    textMatrixPDF.concat(pValues);

    if ( 0.0f > ptx && 0.0f == pty ) {
        RECT rcText;
        float deltaX = measureText(" ",&rcText);
        if ( 0.0f == deltaX || 0.0f > deltaX )
            return;
        long count = (long)(pValues[4] / deltaX);
        if ( 0 == count && ( abs(pValues[4]) > 0.60 * abs(deltaX) ) )
            count = 1L;
        for ( long k = 0; k < count; k++ )
            sprintf(szCurrentText + strlen(szCurrentText)," ");
    }

    return;
    }


    void graphicsState::beginText() {
    sendText();
    textMatrix = textMatrixIdentity;
    textMatrixPDF = textMatrixPDFIdentity;
    textLineMatrix = textMatrix;
    textLineMatrixPDF = textMatrixPDF;
    return;
    }


    void graphicsState::addText(char *pszText) {
    sprintf(szCurrentText + strlen(szCurrentText),"%s",pszText);
    show(pszText);
    return;
    }


    void graphicsState::sendText() {
    return;
    }


    void graphicsState::startLine(float tx,float ty) {
    return;
    }


    void graphicsState::endText() {
    return;
    }


    XFORM *graphicsState::pTransform() {
    static XFORM theXForm;
    theXForm.eM11 = a();
    theXForm.eM12 = b();
    theXForm.eM21 = c();
    theXForm.eM22 = d();
    theXForm.eDx = tx();
    theXForm.eDy = ty();
    return &theXForm;
    }


    XFORM *graphicsState::pTextTransform() {
    static XFORM theXForm;
    theXForm.eM11 = textMatrix.a();
    theXForm.eM12 = textMatrix.b();
    theXForm.eM21 = textMatrix.c();
    theXForm.eM22 = textMatrix.d();
    theXForm.eDx = textMatrix.tx();
    theXForm.eDy = textMatrix.ty();
    return &theXForm;
    }


    void graphicsState::concat(matrix *pSource) {
    matrix::concat(pSource);
    if ( ! ( NULL == hdcSurface ) )
        SetWorldTransform(hdcSurface,pTransform());
    return;
    }


    void graphicsState::concat(XFORM &winXForm) {
    matrix::concat(winXForm);
    if ( ! ( NULL == hdcSurface ) )
        SetWorldTransform(hdcSurface,pTransform());
    return;
    }


    void graphicsState::concat(float *pValues) {
    matrix::concat(pValues);
    if ( ! ( NULL == hdcSurface ) )
        SetWorldTransform(hdcSurface,pTransform());
    return;
    }


    void graphicsState::restored() {
    SetWorldTransform(hdcSurface,pTransform());
    return;
    }


    void graphicsState::show(char *pszxText) {
    return;
    }


    float graphicsState::measureText(char *pszText,RECT *pResult) {
    return 0.0f;
    }


    void graphicsState::setGraphicsStateDict(char *pszDictName) {
Beep(2000,200);
    return;
    }


    void graphicsState::translate(POINT_TYPE *pX,POINT_TYPE *pY,POINTL ptl,uint16_t countPoints) {

    for ( long k = 0; k < countPoints; k++, pX++, pY++ ) {
        *pX += (POINT_TYPE)ptl.x;
        *pY += (POINT_TYPE)ptl.y;
    }

    return;
    }


    void graphicsState::scale(POINT_TYPE *pX,POINT_TYPE *pY,POINT_TYPE scaleX,POINT_TYPE scaleY,uint16_t countPoints) {

    for ( long k = 0; k < countPoints; k++, pX++, pY++ ) {
        *pX = *pX * scaleX;
        *pY = *pY * scaleY;
    }

    return;
    }

    POINTD *graphicsState::lerp( POINTD *pFrom,POINTD *pTo,POINT_TYPE ratio) {
    return new POINTD(pFrom -> X() + ratio * (pTo -> X() - pFrom -> X()),pFrom -> Y() + ratio * (pTo -> Y() - pFrom -> Y()));
    }


    void graphicsState::bezierCurve(POINTD *pPoint1,POINTD *pControl1,POINTD *pControl2,POINTD *pPoint2,POINT_TYPE angular) {

#if 0
canvas_ity::xy point_1(pPoint1 -> X(),pPoint1 -> Y());
canvas_ity::xy control_1(pControl1 -> X(),pControl1 -> Y());
canvas_ity::xy control_2(pControl2 -> X(),pControl2 -> Y());
canvas_ity::xy point_2(pPoint2 -> X(),pPoint2 -> Y());

static int const width = 256, height = 256;
canvas_ity::canvas context( width, height );

std::vector<canvas_ity::xy> morePoints;
context.bezierCurve(point_1,control_1,control_2,point_2,angular,&morePoints);
//for ( canvas_ity::xy xy : morePoints ) {
//thesePoints.push_back(new POINTD(xy.x,xy.y));
//}

#endif

    static POINT_TYPE const epsilon = 1.0E-04f;

    POINTD edge1 = *pControl1 - *pPoint1;
    POINTD edge2 = *pControl2 - *pControl1;
    POINTD edge3 = *pPoint2 - *pControl2;

    if ( 0.0 == dot(edge1,edge1) && 0.0 == dot(edge3,edge3) ) {
        thesePoints.push_back(pPoint2);
        return;
    }

    POINT_TYPE at[] = {0.0,1.0,0.0,0.0,0.0,0.0,0.0};
    int atCount = 2;

    POINTD extrema_a = -9.0 * edge2 + 3.0 * ( *pPoint2 - *pPoint1 );
    POINTD extrema_b = 6.0 * ( *pPoint1 + *pControl2 ) - 12.0 * *pControl1;
    POINTD extrema_c = 3.0 * edge1;

    if ( epsilon < fabs(extrema_a.X()) )
    {
        POINT_TYPE discriminant = extrema_b.X() * extrema_b.X() - 4.0f * extrema_a.X() * extrema_c.X();

        if ( 0.0 <= discriminant )
        {
            POINT_TYPE sign = extrema_b.X() > 0.0f ? 1.0f : -1.0f;
            POINT_TYPE term = -extrema_b.X() - sign * sqrt( discriminant );
            POINT_TYPE extremum_1 = term / ( 2.0f * extrema_a.X() );
            at[ atCount++ ] = extremum_1;
            at[ atCount++ ] = extrema_c.X() / ( extrema_a.X() * extremum_1 );
        }

    } else if ( epsilon < fabs(extrema_b.X()) )
        at[ atCount++ ] = -extrema_c.X() / extrema_b.X();

    if ( epsilon < fabs(extrema_a.Y()) )
    {
        POINT_TYPE discriminant = extrema_b.Y() * extrema_b.Y() - 4.0f * extrema_a.Y() * extrema_c.Y();

        if ( discriminant >= 0.0 )
        {
            POINT_TYPE sign = extrema_b.Y() > 0.0f ? 1.0f : -1.0f;
            POINT_TYPE term = -extrema_b.Y() - sign * sqrt( discriminant );
            POINT_TYPE extremum_1 = term / ( 2.0f * extrema_a.Y() );
            at[ atCount++ ] = extremum_1;
            at[ atCount++ ] = extrema_c.Y() / ( extrema_a.Y() * extremum_1 );
        }

    } else  if ( epsilon < fabs( extrema_b.Y() ) )
        at[atCount++] = -extrema_c.Y() / extrema_b.Y();

    POINT_TYPE determinant_1 = dot( perpendicular(edge1), edge2);
    POINT_TYPE determinant_2 = dot( perpendicular(edge1), edge3);
    POINT_TYPE determinant_3 = dot( perpendicular(edge2), edge3);

    POINT_TYPE curve_a = determinant_1 - determinant_2 + determinant_3;
    POINT_TYPE curve_b = -2.0f * determinant_1 + determinant_2;

    if ( epsilon < fabs(curve_a) && epsilon < fabs(curve_b) )
        at[atCount++] = -0.5f * curve_b / curve_a;

    for ( int index = 1; index < atCount; ++index )
    {
        POINT_TYPE value = at[ index ];
        int sorted = index - 1;

        for ( ; 0 <= sorted && value < at[ sorted ]; --sorted )
            at[ sorted + 1 ] = at[ sorted ];

        at[ sorted + 1 ] = value;
    }

    POINTD split_point_1 = *pPoint1;

    for ( long index = 0; index < atCount - 1; ++index )
    {
        if ( ! ( 0.0 <= at[index] && at[index + 1] <= 1.0 && at[ index ] != at[index + 1] ) )
            continue;

        POINT_TYPE ratio = at[ index ] / at[ index + 1 ];

        POINTD *partial_1 = lerp( pPoint1, pControl1, at[index + 1] );
        POINTD *partial_2 = lerp( pControl1, pControl2, at[ index + 1 ] );
        POINTD *partial_3 = lerp( pControl2, pPoint2, at[ index + 1 ] );
        POINTD *partial_4 = lerp( partial_1, partial_2, at[ index + 1 ] );
        POINTD *partial_5 = lerp( partial_2, partial_3, at[ index + 1 ] );
        POINTD *partial_6 = lerp( partial_1, partial_4, ratio );

        POINTD *split_point_2 = lerp( partial_4, partial_5, at[ index + 1 ] );
        POINTD *split_control_2 = lerp( partial_4, split_point_2, ratio );
        POINTD *split_control_1 = lerp( partial_6, split_control_2, ratio );

        addTessellation( &split_point_1, split_control_1,split_control_2, split_point_2,angular, 20 );

        *split_point_1.px = *split_point_2 -> px;
        *split_point_1.py = *split_point_2 -> py;
    }

    return;
    }


    void graphicsState::addTessellation(POINTD *pPoint1,POINTD *pControl1,POINTD *pControl2,POINTD *pPoint2,POINT_TYPE angular,long limit ) {

    static POINT_TYPE const tolerance = 0.125f;
    static POINT_TYPE const epsilon = 1.0E-04f;

    POINT_TYPE flatness = tolerance * tolerance;

    POINTD edge_1 = *pControl1 - *pPoint1;
    POINTD edge_2 = *pControl2 - *pControl1;
    POINTD edge_3 = *pPoint2 - *pControl2;
    POINTD segment = *pPoint2 - *pPoint1;

    POINT_TYPE squared_1 = dot( edge_1, edge_1 );
    POINT_TYPE squared_2 = dot( edge_2, edge_2 );
    POINT_TYPE squared_3 = dot( edge_3, edge_3 );
    POINT_TYPE length_squared = std::max( epsilon, dot( segment, segment ) );

    POINT_TYPE projection_1 = dot( edge_1, segment ) / length_squared;
    POINT_TYPE projection_2 = dot( edge_3, segment ) / length_squared;

    POINT_TYPE clamped_1 = std::min( std::max( projection_1, 0.0f ), 1.0f );
    POINT_TYPE clamped_2 = std::min( std::max( projection_2, 0.0f ), 1.0f );

    POINTD to_line_1 = *pPoint1 + clamped_1 * segment - *pControl1;
    POINTD to_line_2 = *pPoint2 - clamped_2 * segment - *pControl2;

    POINT_TYPE cosine = 1.0;

    if ( angular > -1.0 )
    {
        if ( ! ( 0.0 == squared_1 * squared_3 ) )
            cosine = dot( edge_1, edge_3 ) / sqrt( squared_1 * squared_3 );

        else if ( ! ( 0.0 == squared_1 * squared_2 ) ) 
            cosine = dot( edge_1, edge_2 ) / sqrt( squared_1 * squared_2 );

        else if ( ! ( 0.0 == squared_2 * squared_3 ) ) 
            cosine = dot( edge_2, edge_3 ) / sqrt( squared_2 * squared_3 );
    }

    if ( ( dot( to_line_1, to_line_1 ) <= flatness && dot( to_line_2, to_line_2 ) <= flatness && cosine >= angular ) || ! limit ) 
    {
        if ( -1.0 < angular && ! ( 0.0 == squared_1 ) )
            thesePoints.push_back( pControl1 );

        if ( -1.0 < angular && ! ( 0.0 == squared_2 ) )
            thesePoints.push_back( pControl2 );

        if ( -1.0 == angular || ! ( 0.0 == squared_3 ) )
            thesePoints.push_back( pPoint2 );

        return;
    }

    POINTD *pLeft1 = lerp( pPoint1, pControl1, 0.5 );
    POINTD *pMiddle = lerp( pControl1, pControl2, 0.5 );
    POINTD *pRight2 = lerp( pControl2, pPoint2, 0.5 );
    POINTD *pLeft2 = lerp( pLeft1, pMiddle, 0.5 );
    POINTD *pRight1 = lerp( pMiddle, pRight2, 0.5 );
    POINTD *split = lerp( pLeft2, pRight1, 0.5 );

    addTessellation( pPoint1, pLeft1, pLeft2, split, angular, limit - 1 );
    addTessellation( split, pRight1, pRight2, pPoint2, angular, limit - 1 );

    return;
    }

