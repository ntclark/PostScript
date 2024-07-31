#pragma once

   struct gdiParameters {

      gdiParameters() {}
      gdiParameters(gdiParameters *pRHS) {
         lineWidth = pRHS -> lineWidth;
         lineCap = pRHS -> lineCap;
         lineJoin = pRHS -> lineJoin;
         rgbColor = pRHS -> rgbColor;
         if ( ! ( NULL == pRHS -> pLineStyles ) ) {
            pLineStyles = new DWORD[pRHS -> countDashSizes];
            memcpy(pLineStyles,pRHS -> pLineStyles,pRHS -> countDashSizes * sizeof(DWORD));
         }
         return;
      }

      void setupDC();

      POINT_TYPE lineWidth{0.0f};
      long lineCap{0L};
      long lineJoin{0L};
      boolean isPathActive{false};
      COLORREF rgbColor{RGB(0,0,0)};
      DWORD *pLineStyles{NULL};
      DWORD countDashSizes{0};

      void setLineCap(long v);
      void setLineJoin(long v);
      void setLineWidth(POINT_TYPE v);
      void setLineDash(array *pArray,POINT_TYPE offset);

      void setRGBColor(COLORREF rgb);
      void setRGBColor(POINT_TYPE r,POINT_TYPE g,POINT_TYPE b);

      ~gdiParameters() {
      if ( ! ( NULL == pLineStyles ) )
         delete [] pLineStyles;
      }
   };


