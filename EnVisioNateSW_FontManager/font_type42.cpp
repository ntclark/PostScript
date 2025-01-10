
#include "EnVisioNateSW_FontManager.h"

    HRESULT font::type42Load(BYTE *pbData) {

    //if ( ! ( NULL == pSfntsArray ) ) {
    //    pSfntsTable = reinterpret_cast<class binaryString *>(pSfntsArray -> getElement(0)) -> getData();
    //    tableDirectory.load(pSfntsTable);
    //} else
        tableDirectory.load(pbData);

    if ( NULL == tableDirectory.table("maxp") ) {
        sprintf(font::szFailureMessage,"font %s is invalid, it does not have a maxp table",szFamily);
        return E_FAIL;
    }

    if ( NULL == tableDirectory.table("head") ) {
        sprintf(font::szFailureMessage,"font %s is invalid, it does not have a head table",szFamily);
        return E_FAIL;
    }

    if ( NULL == tableDirectory.table("hhea") ) {
        sprintf(font::szFailureMessage,"font %s is invalid, it does not have a hhea table",szFamily);
        return E_FAIL;
    }

    if ( NULL == tableDirectory.table("hmtx") ) {
        sprintf(font::szFailureMessage,"font %s is invalid, it does not have a hmtx table",szFamily);
        return E_FAIL;
    }

    if ( ! ( NULL == tableDirectory.table("vhea") ) && ( NULL == tableDirectory.table("vmtx") ) ) {
        sprintf(font::szFailureMessage,"font %s is invalid, it contains a vhea table, but does not contain a vmtx table",szFamily);
        return E_FAIL;
    }

#if 0
    if ( NULL == pSfntsArray && NULL == tableDirectory.table("cmap") ) {
        sprintf(szMessage,"font %s is invalid, it does not contain a cmap table",szFamily);
        throw invalidfont(szMessage);
        return;
    }
#endif

    pGlyfTable = tableDirectory.table("glyf");
    if ( NULL == pGlyfTable )
        pGlyfTable = tableDirectory.table("glyx");

    if ( NULL == pGlyfTable ) {
        sprintf(font::szFailureMessage,"font %s is invalid, it does not have a glyf (or glyx) table",szFamily);
        return E_FAIL;
    }

    pLocaTable = tableDirectory.table("loca");
    if ( NULL == pLocaTable )
        pLocaTable = tableDirectory.table("locx");

    if ( NULL == pLocaTable ) {
        sprintf(font::szFailureMessage,"font %s is invalid, it does not have a loca (or locx ) table",szFamily);
        return E_FAIL;
    }

    pMaxProfileTable = new otMaxProfileTable(tableDirectory.table("maxp"));

    countGlyphs = pMaxProfileTable -> numGlyphs;

    pHeadTable = new otHeadTable(tableDirectory.table("head"));

    pHorizHeadTable = new otHorizHeadTable(tableDirectory.table("hhea"));

    pHorizontalMetricsTable = new otHorizontalMetricsTable(countGlyphs,pHorizHeadTable -> numberOfHMetrics,tableDirectory.table("hmtx"));

    if ( ! ( NULL == tableDirectory.table("vhea") ) ) {
        pVertHeadTable = new otVertHeadTable(tableDirectory.table("vhea"));
        pVerticalMetricsTable = new otVerticalMetricsTable(countGlyphs,pVertHeadTable -> numOfLongVerMetrics,tableDirectory.table("vmtx"));
    }

    if ( ! ( NULL == tableDirectory.table("os2") ) )
        pOS2Table = new otOS2Table(tableDirectory.table("os2"));

    scaleFUnitsToPoints = 1.0f / (float)pHeadTable -> unitsPerEm;

    if ( ! ( NULL == pOS2Table ) )
        scaleFUnitsToPoints = 1.0f / (float)(pOS2Table -> sTypoAscender - pOS2Table -> sTypoDescender);

    glyphIDMap.clear();

    if ( ! ( NULL == tableDirectory.table("cmap") ) ) {

        otCmapTable theCmapTable(tableDirectory.table("cmap"));

        for ( long k = 0; k < theCmapTable.numTables; k++ ) {

            if ( ! ( 3 == theCmapTable.pEncodingRecords[k].platformID ) )
                continue;

            BYTE *pLoad = (BYTE *)theCmapTable.pEncodingRecords[k].tableAddress;

            otCmapSubtableFormat4 theCmapSubtable(&pLoad);

            if ( ! ( 4 == theCmapSubtable.format ) ) {
                sprintf(font::szFailureMessage,"operator: findfont. font %s is invalid. only Cmap Subtable format 4 is supported.",szFamily);
                return E_FAIL;
            }

            theCmapSubtable.loadFormat4(&pLoad);

            /*
                Each segment is described by a startCode and endCode, along with an idDelta and an 
                idRangeOffset, which are used for mapping the character codes in the segment. 
                The segments are sorted in order of increasing endCode values, and the segment
                values are specified in four parallel arrays. You search for the first endCode
                that is greater than or equal to the character code you want to map. 
            */

            for ( uint16_t charCode = 0; charCode < 256; charCode++ ) {

                char szCharCode[8];
                sprintf_s<8>(szCharCode,"%ld",charCode);

                /*
                You search for the first endCode that is greater than or equal 
                to the character code you want to map
                */

                uint16_t endCodeIndex = 0xFFFF;
                for ( uint16_t k = 0; 1; k++ ) {
                    if ( 0xFFFF == theCmapSubtable.pEndCode[k] || theCmapSubtable.pEndCode[k] >= charCode ) {
                        endCodeIndex = k;
                        break;
                    }
                }

                uint16_t startCode = theCmapSubtable.pStartCode[endCodeIndex];
                uint16_t idRangeOffset = theCmapSubtable.pIdRangeOffsets[endCodeIndex];
                uint16_t idDelta = theCmapSubtable.pIdDelta[endCodeIndex];

                /*
                If the corresponding startCode is less than or equal to the 
                character code, then you use the corresponding idDelta and 
                idRangeOffset to map the character code to a glyph index 
                (otherwise, the missingGlyph is returned). 
                */

                uint16_t glyphId = 0;

                if ( startCode <= charCode ) {

                    if ( 0 == idRangeOffset ) {

                        /*
                        If the idRangeOffset is 0, the idDelta value is added directly 
                        to the character code offset (i.e. idDelta[i] + c) to get the 
                        corresponding glyph index. Again, the idDelta arithmetic is modulo 65536. 
                        If the result after adding idDelta[i] + c is less than zero, add 65536 
                        to obtain a valid glyph ID.
                        */

                        glyphId = idDelta + charCode;

                        if ( 0 > glyphId )
                            glyphId += (uint16_t)65536;

                    } else {

                        /*
                        If the idRangeOffset value for the segment is not 0, the mapping of character codes 
                        relies on glyphIdArray. The character code offset from startCode is added to the 
                        idRangeOffset value. This sum is used as an offset from the current location 
                        within idRangeOffset itself to index out the correct glyphIdArray value.
                        This obscure indexing trick works because glyphIdArray immediately follows 
                        idRangeOffset in the font file. The C expression that yields the glyph index is:

                        glyphId = *(idRangeOffset[i] / 2 + (c - startCode[i]) + &idRangeOffset[i])
                        */

                        uint16_t *pbGlyphId = &theCmapSubtable.pIdRangeOffsets[endCodeIndex];
                        pbGlyphId += idRangeOffset / 2;
                        pbGlyphId += (charCode - startCode);

                        glyphId = *(uint16_t *)pbGlyphId;

                        /*
                        The value c is the character code in question, and i is the segment index in which c appears. 
                        If the value obtained from the indexing operation is not 0 (which indicates missingGlyph), 
                        idDelta[i] is added to it to get the glyph index. The idDelta arithmetic is modulo 65536.
                        */

                        if ( ! ( 0 == glyphId ) ) {
                            glyphId += idDelta;
                            if ( 0 > glyphId )
                                glyphId += (uint16_t)65536;
                        }

                    }

                }

                glyphIDMap[charCode] = glyphId;

#if 0
                pCharStrings -> put(szCharCode,new (pJob -> CurrentObjectHeap()) object(pJob,(long)glyphId));
#endif

            }

        }

    }

    if ( ! ( NULL == tableDirectory.table("post") ) ) 
        pPostTable = new otPostTable(tableDirectory.table("post"));

    isLoaded = true;

    return S_OK;
    }


