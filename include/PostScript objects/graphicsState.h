// Copyright 2017 InnoVisioNate Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include "PostScript objects\matrix.h"

#include "job.h"

class font;

   class graphicsState : public matrix {
   public:

      graphicsState(job *pJob,HDC hdc,RECT *prcWindowsClip);
      graphicsState(graphicsState &);
      ~graphicsState();

      virtual void concat(matrix *);
      virtual void concat(float *);
      virtual void concat(XFORM &);

      void restored();
      void moveto();
      void lineto();
      void closepath();
      void show(char *);

      void setTextMatrix(float *);

      void setTextLeading(float v) { textLeading = v; };
      float getTextLeading() { return textLeading; };

      void setFontSize(float v);
      float getFontSize() { return fontSize ; };

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
      void setFont(char *szFontName,float fontSize);

      float measureText(char *pszText,RECT *pResult);

   private:

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

      class font *pFont;

      job *pJob;
      HDC hdcTarget;
      HGDIOBJ oldFont;

      long cxPDFPage,cyPDFPage;

      POINTL lastMove,lastPoint;
      RECT rcTextObjectWindows;
      RECT rcTextObjectPDF;

      static char szCurrentText[4096];

   };
   