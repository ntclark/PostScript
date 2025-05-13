
#include "job.h"

#include "type1Glyph.h"
#include "PostScript objects/dictionary.h"

    type1Glyph::type1Glyph(dictionary *pFontDict) {

    pDictionary = pFontDict;
    countRandomBytes = 4;
    object *pPrivateObj = pDictionary -> retrieve("Private");
    if ( ! ( NULL == pPrivateObj ) ) {
        pPrivateDictionary = reinterpret_cast<dictionary *>(pPrivateObj);
        object *pLenIVObject = pPrivateDictionary -> retrieve("lenIV");
        if ( ! ( NULL == pLenIVObject ) ) 
            countRandomBytes = pLenIVObject -> IntValue();
        object *pPaintType = pPrivateDictionary -> retrieve("PaintType");
        if ( ! ( NULL == pPaintType ) )
            thePaintType = (enum paintType)pPaintType -> IntValue();
    }

    pFontMatrixArray = reinterpret_cast<array *>(pDictionary -> retrieve("FontMatrix"));

    return;
    }