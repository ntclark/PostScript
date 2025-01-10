#pragma once

#include "FontManager_i.h"

class job;
class font;
class graphicsState;

    class font : public dictionary {

    public:

        font(job *pJob,char *fontName);
        font(job *pJob,dictionary *,char *fontName);

        font(font &);

        ~font();

        char *fontName();

        void setFontMatrix(class matrix *pMatrix);

        XFORM *getMatrix();

        void SetCIDFont(boolean isCID) { isCIDFont = isCID; }

        void gSave() { pIFont -> SaveState(); }
        void gRestore() { pIFont -> RestoreState(); }

        static void setFont(font *pFont);
        static font *findFont(job *pJob,char *pszFamily);
        static void initialize();
        static font *CurrentFont();
        static font *makeFont(class matrix *pMatrix,font *pCopyFrom);
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
