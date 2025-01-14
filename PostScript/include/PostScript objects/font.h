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

        static void setFont(font *pFont);
        static font *findFont(job *pJob,char *pszFamily);
        static void initialize();
        static font *CurrentFont();

        static font *makeFont(array *pArray,font *pCopyFrom);
        static font *makeFont(matrix *pMatrix,font *pCopyFrom);
        static font *makeFont(XFORM *pXForm,font *pCopyFrom);

        static font *scaleFont(FLOAT scaleFactor,font *pCopyFrom);

        static IFontManager *pIFontManager;

    private:

        void loadDictionary();

        IFont_EVNSW *pIFont{NULL};

        boolean isCIDFont{false};

        long dupCount{0};

        dictionary *pCharStrings{NULL};
        class array *pEncoding{NULL};

    };
