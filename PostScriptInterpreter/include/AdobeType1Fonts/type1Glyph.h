#pragma once

#include <Windows.h>
#include <stdint.h>

class dictionary;
class array;

    struct type1Glyph {
    public:

        type1Glyph(dictionary *pFontDict);

        enum paintType {
            fill = 0,
            stroke = 2
        };

        FLOAT leftSideBearing[2]{0.0f,0.0f};
        FLOAT characterWidth[2]{0.0f,0.0f};

        int32_t countRandomBytes{-1};

        boolean firstRMove{true};
        array *pFontMatrixArray{NULL};
        enum paintType thePaintType{paintType::fill};
        dictionary *pDictionary{NULL};
        dictionary *pPrivateDictionary{NULL};
    };