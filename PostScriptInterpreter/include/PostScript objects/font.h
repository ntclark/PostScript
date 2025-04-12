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

#pragma once

#include "FontManager_i.h"

#include "PostScript objects/matrix.h"

class job;
class graphicsState;

    class font : public dictionary {

    public:

        font(job *pJob,char *fontName);
        font(job *pJob,dictionary *,char *fontName);

        font(font &);

        ~font();

        char *fontName();

        void setFontMatrix(XFORM *pMatrix);

        XFORM *getFontMatrix();

        void SetCIDFont(boolean isCID) { isCIDFont = isCID; }

        void gSave() { pIFont -> SaveState(); }
        void gRestore() { pIFont -> RestoreState(); }

        void loadDictionary();

        static void setFont(font *pFont);
        static font *findFont(job *pJob,char *pszFamily);
        static font *CurrentFont();

        static font *makeFont(array *pArray,font *pCopyFrom);
        static font *makeFont(matrix *pMatrix,font *pCopyFrom);
        static font *makeFont(XFORM *pXForm,font *pCopyFrom);

        static font *scaleFont(FLOAT scaleFactor,font *pCopyFrom);

    private:

        void createDictionary();

        IFont_EVNSW *pIFont{NULL};

        boolean isCIDFont{false};

        long dupCount{0};

        dictionary *pCharStrings{NULL};
        array *pEncoding{NULL};
        array *pSfntsArray{NULL};

    };
