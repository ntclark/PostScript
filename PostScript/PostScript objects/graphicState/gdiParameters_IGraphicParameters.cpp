#include "job.h"

#include "gdiParameters.h"


    HRESULT gdiParameters::GraphicParameters::put_LineWidth(POINT_TYPE v) { 
    pParent -> lineWidth = v; 
    put_LineDash(NULL,-1,0.0f);
    return S_OK;
    }


    HRESULT gdiParameters::GraphicParameters::put_LineJoin(long lj) {
    pParent -> lineJoin = lj;
    put_LineDash(NULL,-1,0.0f);
    return S_OK;
    }


    HRESULT  gdiParameters::GraphicParameters::put_LineCap(long lc) {
    pParent -> lineCap = lc;
    put_LineDash(NULL,-1,0.0f);
    return S_OK;
    }


    HRESULT gdiParameters::GraphicParameters::put_LineDash(FLOAT *pValues,long countValues,POINT_TYPE offset) {

    if ( ! ( -1 == countValues ) ) {

        if ( ! ( NULL == pValues ) ) {

            if ( ! ( NULL == pParent -> pLineStyles ) ) {
                delete [] pParent -> pLineStyles;
                pParent -> countDashSizes = 0;
            }

            pParent -> pLineStyles = NULL;

            if ( 0 < countValues ) {
                countValues = min(16,countValues);
                pParent -> pLineStyles = new DWORD[countValues];
                for ( long k = 0; k < countValues; k++ )
                    pParent -> pLineStyles[k] = (DWORD)(pValues[k]);
            }

            pParent -> countDashSizes = countValues;

        } else {

            if ( ! ( NULL == pParent -> pLineStyles ) ) {
                delete [] pParent -> pLineStyles;
                pParent -> pLineStyles = NULL;
            }

            pParent -> countDashSizes = 0;

        }

    }
/*
      0 Butt cap. The stroke is squared off at the endpoint of the path. There is no
      projection beyond the end of the path.

      1 Round cap. A semicircular arc with a diameter equal to the line width is
      drawn around the endpoint and filled in.

      2 Projecting square cap. The stroke continues beyond the endpoint of the
      path for a distance equal to half the line width and is then squared off.
*/

    LOGBRUSH logBrush{0};
    logBrush.lbColor = pParent -> rgbColor;
    logBrush.lbStyle = BS_SOLID;

    long penStyle = PS_GEOMETRIC;

    if ( ! ( NULL == pParent -> pLineStyles ) )
        penStyle |= PS_USERSTYLE;

    switch ( pParent -> lineCap ) {
    case 0:
        penStyle |= PS_ENDCAP_FLAT;
        break;
    case 1:
        penStyle |= PS_ENDCAP_ROUND;
        break;
    case 2:
        penStyle |= PS_ENDCAP_SQUARE;
        break;
    }

    HPEN hPen = ExtCreatePen(penStyle,(int)(pParent -> lineWidth),&logBrush,pParent -> countDashSizes,pParent -> pLineStyles);

    DeleteObject(SelectObject(pPStoPDF -> GetDC(),hPen));

    return S_OK;
    }


    HRESULT gdiParameters::GraphicParameters::put_RGBColor(COLORREF rgb) {
    pParent -> rgbColor = rgb;
    DeleteObject(SelectObject(pPStoPDF -> GetDC(),CreateSolidBrush(pParent -> rgbColor)));
    pParent -> setLineDash(NULL,0,0.0f);
    return S_OK;
    }