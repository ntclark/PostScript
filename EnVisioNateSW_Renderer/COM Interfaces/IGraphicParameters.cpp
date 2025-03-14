#include "EnVisioNateSW_Renderer.h"

    FLOAT Renderer::GraphicParameters::defaultLineWidth = 1.0f;
    D2D1_CAP_STYLE Renderer::GraphicParameters::defaultLineCap = D2D1_CAP_STYLE_ROUND;//D2D1_CAP_STYLE_FLAT;
    D2D1_LINE_JOIN Renderer::GraphicParameters::defaultLineJoin = D2D1_LINE_JOIN_MITER;
    D2D1_DASH_STYLE Renderer::GraphicParameters::defaultDashStyle = D2D1_DASH_STYLE_SOLID;
    COLORREF Renderer::GraphicParameters::defaultRGBColor = RGB(0,0,0);


    HRESULT Renderer::GraphicParameters::QueryInterface(REFIID refIID,void **pvResult) {

    if ( ! pvResult )
        return E_POINTER;

    *pvResult = NULL;

    if ( IID_IUnknown == refIID ) 
        *pvResult = static_cast<IUnknown *>(this);

    else if ( IID_IGraphicParameters == refIID )
        *pvResult = static_cast<GraphicParameters *>(this);

    if ( * pvResult ) {
        AddRef();
        return S_OK;
    }

    return QueryInterface(refIID,pvResult);
    }


    HRESULT Renderer::GraphicParameters::SaveState() {
    valuesStack.push(new values(*valuesStack.top()));
    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::RestoreState() {
    if ( 1 == valuesStack.size() )
        return E_UNEXPECTED;
    values *pCurrent = valuesStack.top();
    valuesStack.pop();
    delete pCurrent;
    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_LineWidth(FLOAT lw) {
    valuesStack.top() -> lineWidth = lw; 
    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_LineJoin(long lj) {

    switch ( lj ) {
    default:
    case 0:
        valuesStack.top() -> lineJoin = D2D1_LINE_JOIN_MITER;
        break;

    case 1:
        valuesStack.top() -> lineJoin = D2D1_LINE_JOIN_BEVEL;
        break;

    case 2:
        valuesStack.top() -> lineJoin = D2D1_LINE_JOIN_ROUND;
        break;

    case 3:
        valuesStack.top() -> lineJoin = D2D1_LINE_JOIN_MITER_OR_BEVEL;
        break;

    }

    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_LineCap(long lc) {

    switch ( lc ) {
    default:
    case 0:
        valuesStack.top() -> lineCap = D2D1_CAP_STYLE_FLAT;
        break;
    case 1:
        valuesStack.top() -> lineCap = D2D1_CAP_STYLE_SQUARE;
        break;
    case 2:
        valuesStack.top() -> lineCap = D2D1_CAP_STYLE_ROUND;
        break;
    }

    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_LineDash(FLOAT *pDashValues,long countValues,FLOAT passedOffset) {

    values *pValues = valuesStack.top();

    if ( ! ( NULL == pDashValues ) ) {

        pValues -> countDashSizes = (int)min(16,countValues);
        pValues -> lineDashOffset = passedOffset;
        for ( long k = 0; k < pValues -> countDashSizes; k++ ) {
            POINTF ptValue{30.0f * 5.0f * pDashValues[k] / 9.0f,0.0f};
            pParent -> pIGraphicElements -> transformPoint(&ptValue,&ptValue);
            pValues -> lineDashSizes[k] = ptValue.x;
        }
        pValues -> lineDashStyle = D2D1_DASH_STYLE_CUSTOM;

    }  else {

        pValues -> countDashSizes = 0;
        pValues -> lineDashOffset = 0.0f;
        pValues -> lineDashStyle = D2D1_DASH_STYLE_SOLID;

    }

    return S_OK;
    }


    HRESULT Renderer::GraphicParameters::put_RGBColor(COLORREF color) {
    valuesStack.top() -> rgbColor = color;
    return S_OK;
    }


    void Renderer::GraphicParameters::setParameters(char *pszLineSettings,boolean doFill) {
    values *pValues = valuesStack.top();
    char szLDashes[128]{128 * '\0'};
    long cntPrint = 0;
    for ( int k = 0; k < 16; k++ )
        cntPrint += sprintf(szLDashes + cntPrint,"%05.2f,",pValues -> lineDashSizes[k]);
    szLDashes[cntPrint - 1] = '\0';
    sprintf_s(pszLineSettings,128,"%05.2f:%01d:%01d:%01d:%01d:%08ld:%c:%s",
            (FLOAT)valuesStack.top() -> lineWidth,pValues -> lineCap,pValues -> lineJoin,
                        pValues -> lineDashStyle,pValues -> countDashSizes,pValues -> rgbColor,doFill ? '1' : '0',szLDashes);
    return;
    }


    void Renderer::GraphicParameters::resetParameters(char *pszLineSettings) {

    values *pValues = valuesStack.top();

    char doFillChar;
    char szLDashes[128]{128 * '\0'};

    sscanf(pszLineSettings,"%05f:%01d:%01d:%01d:%01d:%08ld:%c:%s",&pValues -> lineWidth ,&pValues -> lineCap,&pValues -> lineJoin,
                        &pValues -> lineDashStyle,&pValues -> countDashSizes,&pValues -> rgbColor,&doFillChar,szLDashes);

    char *pszDash = strtok(szLDashes,",");

    for ( int k = 0; k < 16; k++ ) {
        pValues -> lineDashSizes[k] = (FLOAT)atof(pszDash);
        pszDash = strtok(NULL,",");
    }

    if ( ! ( NULL == pParent -> pID2D1StrokeStyle1 ) )
        pParent -> pID2D1StrokeStyle1 -> Release();

    pParent -> pID2D1Factory1 -> CreateStrokeStyle(
            D2D1::StrokeStyleProperties1(
                pValues -> lineCap,
                pValues -> lineCap,
                D2D1_CAP_STYLE_FLAT,
                pValues -> lineJoin,
                10.0f,
                pValues -> lineDashStyle,
                pValues -> lineDashOffset,
                D2D1_STROKE_TRANSFORM_TYPE_NORMAL ),//D2D1_STROKE_TRANSFORM_TYPE_FIXED),
                pValues -> lineDashSizes,
                pValues -> countDashSizes,
                    &pParent -> pID2D1StrokeStyle1);

    if ( ! ( NULL == pParent-> pID2D1SolidColorBrush ) )
        pParent-> pID2D1SolidColorBrush -> Release();

    FLOAT r = (FLOAT)GetRValue(pValues -> rgbColor) / 255.0f;
    FLOAT g = (FLOAT)GetGValue(pValues -> rgbColor) / 255.0f;
    FLOAT b = (FLOAT)GetBValue(pValues -> rgbColor) / 255.0f;

    pParent -> pID2D1DCRenderTarget -> CreateSolidColorBrush(D2D1::ColorF(r,g,b, 1.0f),&pParent -> pID2D1SolidColorBrush);

    return;
    }


    long Renderer::GraphicParameters::hashCode(char *szLineSettings,boolean doFill) {
    setParameters(szLineSettings,doFill);
    long hashCode = 0L;
    long part = 0L;
    long n = (DWORD)strlen(szLineSettings);
    char *psz = new char[n + 4];
    memset(psz,0,(n + 4) * sizeof(char));
    strcpy(psz,szLineSettings);
    char *p = psz;
    for ( long k = 0; k < n; k += 4 ) {
        memcpy(&part,p,4 * sizeof(char));
        hashCode ^= part;
        p += 4;
    }
    delete [] psz;
    return hashCode;
    }