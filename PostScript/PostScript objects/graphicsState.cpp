
#include "PostScript objects\graphicsState.h"

#include "Fonts\font.h"

#include <math.h>
#include "utilities.h"

    char graphicsState::szCurrentText[4096];

    graphicsState::graphicsState(job *pj,RECT *prcWindowsClip) :

        matrix::matrix(pj),

        pFont(NULL),
        oldFont(NULL),
        textLeading(0.0f),
        fontSize(1.0f),
        textRise(0.0f),
        characterSpacing(0.0f),
        wordSpacing(0.0f),
        horizontalTextScaling(1.0),
        textRenderingMode(0L),
        writingMode(0L),
        lineCap(0L),
        lineJoin(0L),
        cxPDFPage(0L),
        cyPDFPage(0L),
        pdfRotation(0L),

        textMatrix(pj),
        textMatrixIdentity(pj),
        textLineMatrix(pj),
        textMatrixPDF(pj),
        textMatrixPDFIdentity(pj),
        textMatrixRotation(pj),

        textLineMatrixPDF(pj)
    {

    memset(&lastMove,0,sizeof(POINTL));
    memset(&lastPoint,0,sizeof(POINTL));
    memset(&rcTextObjectWindows,0,sizeof(RECT));

    if ( NULL == pJob || NULL ==  pJob -> pPdfPage ) 
        return;

    RECT rcPDFPage;
    double rotation = 0.0;

    pJob -> pPdfPage -> getPageSize(&rcPDFPage);

    pJob -> pPdfPage -> getRotation(&rotation);

    pdfRotation = (float)rotation;

    rotation = -rotation * atan(1.0) / 45.0;

    cxPDFPage = rcPDFPage.right - rcPDFPage.left;
    cyPDFPage = rcPDFPage.bottom - rcPDFPage.top;

    a = (float)(prcWindowsClip -> right - prcWindowsClip -> left) / (float)cxPDFPage;
    d = -(float)(prcWindowsClip -> bottom - prcWindowsClip -> top) / (float)cyPDFPage;

    tx = (float)(prcWindowsClip -> left);
    ty = (float)(prcWindowsClip -> bottom);

#if USE_ANISOTROPIC
    SetGraphicsMode(pJob -> GetDC(),GM_ADVANCED);
    SetMapMode(pJob -> GetDC(),MM_ANISOTROPIC);
    SetWorldTransform(pJob -> GetDC(),pTransform());
#endif

    textMatrixRotation.a = (float)cos(rotation);
    textMatrixRotation.b = (float)sin(rotation);
    textMatrixRotation.c = -textMatrixRotation.b;
    textMatrixRotation.d = textMatrixRotation.a;

    textMatrixIdentity = *static_cast<matrix *>(this);

    textMatrix = textMatrixIdentity;

    oldFont = SelectObject(pJob -> GetDC(),GetStockObject(DEFAULT_GUI_FONT));

    return;
    }


    graphicsState::~graphicsState() {
    if ( pFont )
        delete pFont;
    pFont = NULL;
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

    if ( ! szCurrentText[0] ) {
        rcTextObjectWindows.left = 0L;
        rcTextObjectWindows.top = 0L;
        return;
    }

    long cx = rcTextObjectWindows.right - rcTextObjectWindows.left;
    long cy = rcTextObjectWindows.bottom - rcTextObjectWindows.top;

    rcTextObjectWindows.left = (long)((double)(rcTextObjectWindows.left - textMatrixIdentity.tx) / textMatrixIdentity.a);
    rcTextObjectWindows.bottom = cyPDFPage - (long)((double)(rcTextObjectWindows.top - textMatrixIdentity.ty) / textMatrixIdentity.d);

    rcTextObjectWindows.right = rcTextObjectWindows.left + cx;
    rcTextObjectWindows.top = rcTextObjectWindows.bottom - cy;

    rcTextObjectPDF.right = rcTextObjectPDF.left + cx;
    rcTextObjectPDF.top = rcTextObjectPDF.bottom + cy;

    if ( 0.0 == textMatrix.a ) {

        long xOriginal = rcTextObjectWindows.left;
        long yOriginal = rcTextObjectWindows.bottom;

        long x = rcTextObjectWindows.left;
        rcTextObjectWindows.left = rcTextObjectWindows.top;
        rcTextObjectWindows.top = x;
        x = rcTextObjectWindows.right;
        rcTextObjectWindows.right = rcTextObjectWindows.bottom;
        rcTextObjectWindows.bottom = x;

        long cx = rcTextObjectWindows.right - xOriginal;
        rcTextObjectWindows.left -= cx;
        rcTextObjectWindows.right -= cx;

        long cy = rcTextObjectWindows.bottom - yOriginal;
        rcTextObjectWindows.top -= cy;
        rcTextObjectWindows.bottom -= cy;

    }

    if ( ! ( 0.0 == textMatrixRotation.b ) ) {
        long t = rcTextObjectPDF.left;
        rcTextObjectPDF.left = rcTextObjectPDF.top;
        rcTextObjectPDF.bottom = cxPDFPage - t;
        rcTextObjectPDF.right = rcTextObjectPDF.left + cy;
        rcTextObjectPDF.top = rcTextObjectPDF.bottom + cx;
    }

    if ( ! ( NULL == pJob -> pIPostScriptTakeText ) ) {

        long n = strlen(szCurrentText) + 1;

        char *pszValue = new char[n];
        static char pszEncodedValue[65535];//char *pszEncodedValue = NULL;

#if 1
        strncpy(pszValue,szCurrentText,n);
#else
        if ( ! ( NULL == pFont ) )
            strncpy(pszValue,pFont -> translateText(szCurrentText),255);
        else
            strncpy(pszValue,szCurrentText,n);
#endif

        RECT rc{0,0,0,0};

        measureText(szCurrentText,&rc);

        rcTextObjectPDF.top = rcTextObjectPDF.bottom + rc.bottom - rc.top;

        ASCIIHexEncodeToString(pszValue,(DWORD)n,pszEncodedValue,65535);//&pszEncodedValue);

        pJob -> pIPostScriptTakeText -> TakeText(&rcTextObjectPDF,pszEncodedValue);

        delete [] pszValue;
        //delete [] pszEncodedValue;

    }

    memset(szCurrentText,0,sizeof(szCurrentText));
    rcTextObjectWindows.left = 0L;
    rcTextObjectWindows.top = 0L;
    return;
    }


    void graphicsState::startLine(float tx,float ty) {
    sendText();
    textMatrix = textLineMatrix;
    textMatrixPDF = textLineMatrixPDF;
    translateTextMatrix(tx,ty);
    textLineMatrix = textMatrix;
    textLineMatrixPDF = textMatrixPDF;
    return;
    }


    void graphicsState::endText() {
    sendText();
    textMatrix = textMatrixIdentity;
    textMatrixPDF = textMatrixPDFIdentity;
    textLineMatrix = textMatrix;
    return;
    }


    XFORM *graphicsState::pTransform() {
    static XFORM theXForm;
    theXForm.eM11 = a;
    theXForm.eM12 = b;
    theXForm.eM21 = c;
    theXForm.eM22 = d;
    theXForm.eDx = tx;
    theXForm.eDy = ty;
    return &theXForm;
    }


    XFORM *graphicsState::pTextTransform() {
    static XFORM theXForm;
    theXForm.eM11 = textMatrix.a;
    theXForm.eM12 = textMatrix.b;
    theXForm.eM21 = textMatrix.c;
    theXForm.eM22 = textMatrix.d;
    theXForm.eDx = textMatrix.tx;
    theXForm.eDy = textMatrix.ty;
    return &theXForm;
    }


    void graphicsState::concat(matrix *pSource) {
    matrix::concat(pSource);
    if ( pJob && pJob -> GetDC() )
        SetWorldTransform(pJob -> GetDC(),pTransform());
    return;
    }


    void graphicsState::concat(XFORM &winXForm) {
    matrix::concat(winXForm);
    if ( pJob && pJob -> GetDC() )
        SetWorldTransform(pJob -> GetDC(),pTransform());
    return;
    }


    void graphicsState::concat(float *pValues) {
    matrix::concat(pValues);
    if ( pJob && pJob -> GetDC() )
        SetWorldTransform(pJob -> GetDC(),pTransform());
    return;
    }


    void graphicsState::restored() {
    SetWorldTransform(pJob -> GetDC(),pTransform());
    return;
    }

    void graphicsState::moveto() {
    if ( ! ( pJob && pJob -> GetDC() ) )
        return;
    object *pY = pJob -> pop();
    object *pX = pJob -> pop();
    lastMove.x = pX -> IntValue();
    lastMove.y = pY -> IntValue();
#if DRAW_GRAPHICS
    MoveToEx(pJob -> GetDC(),lastMove.x,lastMove.y,NULL);
#endif
    return;
    }


    void graphicsState::lineto() {
    if ( ! ( pJob && pJob -> GetDC() ) )
        return;
    object *pY = pJob -> pop();
    object *pX = pJob -> pop();
    lastPoint.x = pX -> IntValue();
    lastPoint.y = pY -> IntValue();
#if DRAW_GRAPHICS
    LineTo(pJob -> GetDC(),lastPoint.x,lastPoint.y);
#endif
    return;
    }


    void graphicsState::closepath() {
    if ( ! ( pJob && pJob -> GetDC() ) )
        return;
    if ( 0 == lastPoint.x )
        return;
#if DRAW_GRAPHICS
    LineTo(pJob -> GetDC(),lastMove.x,lastMove.y);
#endif
    return;
    }


    void graphicsState::show(char *pszxText) {

#if 0
    char *pszTranslatedText = pszxText;
    if ( pFont )
        pszTranslatedText = pFont -> translateText(pszxText);
#endif

    if ( 0 == rcTextObjectWindows.left && 0 == rcTextObjectWindows.top ) {

        rcTextObjectWindows.left = (long)textMatrix.tx;
        rcTextObjectWindows.top = (long)textMatrix.ty;
        rcTextObjectWindows.right = rcTextObjectWindows.left;
        rcTextObjectWindows.bottom = rcTextObjectWindows.top;

        rcTextObjectPDF.left = (long)textMatrixPDF.tx;
        rcTextObjectPDF.bottom = (long)textMatrixPDF.ty;
        rcTextObjectPDF.right = rcTextObjectPDF.left;
        rcTextObjectPDF.top = rcTextObjectPDF.bottom;

    }

    if ( 0.0 == textMatrix.a )
        textMatrix.c = -textMatrix.c;
    else
        textMatrix.d = -textMatrix.d;

    SetWorldTransform(pJob -> GetDC(),pTextTransform());

    if ( 0.0 == textMatrix.a )
        textMatrix.c = -textMatrix.c;
    else
        textMatrix.d = -textMatrix.d;

#if DRAW_TEXT
    DrawTextEx(hdcTarget,pszTranslatedText,-1,&rcText,DT_NOCLIP | DT_BOTTOM,NULL);
#endif

    RECT rcText = {0,0,0,0};
    float deltaX = measureText(pszxText,&rcText);

#if DRAW_RECTANGLES
    HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
    HGDIOBJ oldBrush = SelectObject(pJob -> GetDC(),hBrush);
    FrameRect(pJob -> GetDC(),&rcText,hBrush);
    DeleteObject(SelectObject(pJob -> GetDC(),oldBrush));
#endif

    float pValues[] = {1.0, 0.0, 0.0, 1.0, deltaX, 0.0f};

    textMatrix.concat(pValues);
//
// 10-27-2011: The matrix to track PDF Units *should* translate by the Widths value in the 
// font. This is (the only ?) place where widths (and height) are susceptable to the size of the
// drawn text which is in turn affected by the size of the host window. 
// This causes the small changes in width and height of the profile rectangles.
//
    textMatrixPDF.concat(pValues);

    rcTextObjectWindows.right += rcText.right - rcText.left;
    rcTextObjectWindows.bottom = rcTextObjectWindows.top + max(rcTextObjectWindows.bottom - rcTextObjectWindows.top,rcText.bottom - rcText.top);

    SetWorldTransform(pJob -> GetDC(),pTransform());

    return;
    }


    float graphicsState::measureText(char *pszText,RECT *pResult) {

    memset(pResult,0,sizeof(RECT));

    if ( 0.0 == textMatrix.a )
        textMatrix.c = -textMatrix.c;
    else
        textMatrix.d = -textMatrix.d;

    SetWorldTransform(pJob -> GetDC(),pTextTransform());

    if ( 0.0 == textMatrix.a )
        textMatrix.c = -textMatrix.c;
    else
        textMatrix.d = -textMatrix.d;

    DrawTextEx(pJob -> GetDC(),pszText,-1,pResult,DT_CALCRECT,NULL);
   
    long cy = pResult -> bottom - pResult -> top;
    pResult -> top -= cy;
    pResult -> bottom -= cy;
   
    SetWorldTransform(pJob -> GetDC(),pTransform());

    return (float)(pResult -> right - pResult -> left);
    }


    void graphicsState::setGraphicsStateDict(char *pszDictName) {

    PdfDictionary *pDict = pJob -> pPdfPage -> findResource(pszDictName,"ExtGState");

    if ( ! pDict )
        return;

    BYTE *pValue = NULL;
    PdfElement *pElement = pDict -> Element("OPM");

    if ( pElement ) {
        pValue = pElement -> Value();
        if ( pValue ) {
        }
    }

    pElement = pDict -> Element("SA");

    if ( pElement ) {
        pValue = pElement -> Value();
        if ( pValue ) {
        }
    }

    return;
    }