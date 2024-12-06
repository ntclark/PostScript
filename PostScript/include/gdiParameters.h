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
         pColorSpace = pRHS -> pColorSpace;
         return;
      }

      ~gdiParameters() {
      if ( ! ( NULL == pLineStyles ) )
         delete [] pLineStyles;
      }

      void initialize();

      void setupDC();

      POINT_TYPE lineWidth{gdiParameters::defaultLineWidth};
      long lineCap{gdiParameters::defaultLineCap};
      long lineJoin{gdiParameters::defaultLineJoin};
      COLORREF rgbColor{gdiParameters::defaultRGBColor};
      DWORD *pLineStyles{NULL};
      DWORD countDashSizes{0};
      colorSpace *pColorSpace{NULL};

      void setLineCap(long v);
      void setLineJoin(long v);
      void setLineWidth(POINT_TYPE v);
      void setLineDash(array *pArray,POINT_TYPE offset);

      void setColorSpace(colorSpace *pColorSpace);
      colorSpace *getColorSpace();

      void setColor(colorSpace *pColorSpace);
      void setRGBColor(COLORREF rgb);
      void setRGBColor(POINT_TYPE r,POINT_TYPE g,POINT_TYPE b);

      static POINT_TYPE defaultLineWidth;
      static long defaultLineCap;
      static long defaultLineJoin;
      static COLORREF defaultRGBColor;
   };
