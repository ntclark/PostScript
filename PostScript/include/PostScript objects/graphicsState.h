#pragma once

#include "job.h"

typedef float POINT_TYPE;

#include "PostScript objects/matrix.h"
#include "PostScript objects/binaryString.h"

    struct POINTD {
        POINTD() {};
        POINTD(POINT_TYPE *ptrX,POINT_TYPE *ptrY) : px(ptrX), py(ptrY) {};
        POINTD(POINT_TYPE x,POINT_TYPE y) {
            px = new POINT_TYPE;
            py = new POINT_TYPE;
            *px = x;
            *py = y;
        }
        POINTD(POINTD &rhs) {
            px = new POINT_TYPE;
            py = new POINT_TYPE;
            *px = *rhs.px;
            *py = *rhs.py;
        }
        ~POINTD() {
            delete px;
            delete py;
        }

        POINT_TYPE X() { return *px; }
        POINT_TYPE Y() { return *py; }

        POINT_TYPE *px{NULL};
        POINT_TYPE *py{NULL};
    };

    static POINTD &operator-=(POINTD &left,POINTD right) { 
        *left.px -= *right.px; 
        *left.py -= *right.py; 
        return left; 
    }

    static POINTD operator-(POINTD left,POINTD right) {
        return left -= right;
    }

    static POINTD &operator+=(POINTD &left,POINTD right) {
        *left.px += right.X();
        *left.py += right.Y();
        return left;
    }

    static POINTD operator+(POINTD left,POINTD right) {
        return left += right;
    }

    static POINTD &operator*=(POINTD &left,POINT_TYPE right) {
        *left.px *= right;
        *left.py *= right;
        return left; 
    }

    static POINTD operator*(POINT_TYPE left, POINTD right ) {
        return right *= left;
    }

    static POINTD operator*(POINTD left, POINT_TYPE right ) {
        return left *= right;
    }

    static POINT_TYPE dot( POINTD *left, POINTD *right ) {
    return left -> X() * right -> X() + left -> Y() * right -> Y();
    }

    static POINT_TYPE dot( POINTD &left, POINTD &right ) {
    return left.X() * right.X() + left.Y() * right.Y(); 
    }

    static POINTD perpendicular( POINTD that ) {
    return POINTD( -that.Y(), that.X() ); 
    }


   class graphicsState : public matrix {
   public:

      graphicsState(job *pJob,HWND hwndSurface);
      ~graphicsState();

      virtual void concat(matrix *);
      virtual void concat(float *);
      virtual void concat(XFORM &);

      void restored();

      void moveto();
      void moveto(POINT_TYPE x,POINT_TYPE y);
      void moveto(long x,long y);
      void moveto(POINTL ptl);
      void moveto(POINTD *pPointd);

      void rmoveto();
      void rmoveto(POINT_TYPE x,POINT_TYPE y);
      void rmoveto(long x,long y);
      void rmoveto(POINTL ptl);
      void rmoveto(POINTD *pPointd);

      void lineto();
      void lineto(POINT_TYPE x,POINT_TYPE y);
      void lineto(long x,long y);
      void lineto(POINTL ptl);
      void lineto(POINTD *pPointd);

      void rlineto();
      void rlineto(POINT_TYPE x,POINT_TYPE );
      void rlineto(long x,long y);
      void rlineto(POINTL ptl);
      void rlineto(POINTD *pPointd);

      void closepath();
      void show(char *);

      void translate(POINT_TYPE *pX,POINT_TYPE *pY,POINTL ptl,uint16_t ptCount);
      void scale(POINT_TYPE *pX,POINT_TYPE *pY,POINT_TYPE scaleX,POINT_TYPE scaleY,uint16_t ptCount);

      void setTextMatrix(float *);

      void setTextLeading(float v) { textLeading = v; };
      float getTextLeading() { return textLeading; };

      void setTextRise(float v) { textRise = v; };
      float getTextRise() { return textRise; };

      void setCharacterSpacing(float v) { characterSpacing = v; };
      float getCharacterSpacing() { return characterSpacing; };

      void setWordSpacing(float v) { wordSpacing = v; };
      float getWordSpacing() { return wordSpacing; };

      void setHorizontalTextScaling(float v) { horizontalTextScaling = v; };
      float getHorizontalTextScaling() { return horizontalTextScaling; };

      void setTextRenderingMode(long v) { textRenderingMode = v; };
      long getTextRenderingMode() { return textRenderingMode; };

      void setWritingMode(long v) { writingMode = v; };
      long getWritingMode() { return writingMode; };

      void setLineCap(long v) { lineCap = v; };
      long getLineCap() { return lineCap; };

      void setLineJoin(long v) { lineJoin = v; };
      long getLineJoin() { return lineJoin; };

      void translateTextMatrix(float tx,float ty);
      void translateTextMatrixTJ(float tx,float ty);
      void beginText();
      void addText(char *);
      void startLine(float tx,float ty);
      void endText();

      void setGraphicsStateDict(char *pszDictName);

      void setFont(class font *pFont);

      float measureText(char *pszText,RECT *pResult);

      void drawText(char *pszText);
      void drawGlyph(BYTE bGlyph);

   private:

      void drawConic(std::vector<double> &xPoints,std::vector<double> &yPoints);

      POINTD *lerp(POINTD *pStart,POINTD *pEnd,POINT_TYPE slope);
      void bezierCurve(POINTD *point_1,POINTD *control_1,POINTD *control_2,POINTD *point_2,POINT_TYPE angular);
      void addTessellation(POINTD *point_1,POINTD *control_1,POINTD *control_2,POINTD *point_2,POINT_TYPE angular,long limit);

      std::vector<POINTD *> thesePoints;

      HWND hwndSurface{NULL};
      HDC hdcSurface{NULL};

      XFORM *pTransform();
      XFORM *pTextTransform();

      void sendText();

      matrix textMatrix;
      matrix textMatrixIdentity;
      matrix textLineMatrix;
      matrix textMatrixPDF;
      matrix textMatrixPDFIdentity;
      matrix textLineMatrixPDF;
      matrix textMatrixRotation;

      float textLeading;
      float fontSize;
      float textRise;
      float characterSpacing;
      float wordSpacing;
      float horizontalTextScaling;
      float pdfRotation;
      long textRenderingMode;
      long writingMode;
      long lineCap;
      long lineJoin;

      class font *pCurrentFont{NULL};

      HGDIOBJ oldFont;

      long cxPDFPage,cyPDFPage;

      POINTL lastPoint;

      RECT rcTextObjectWindows;
      RECT rcTextObjectPDF;

      static char szCurrentText[4096];

   };
   