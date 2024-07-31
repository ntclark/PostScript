
#include "PostScript objects/font.h"

    static char szMessage[1024];

    void font::type42Load(BYTE *pbData) {

    if ( ! ( NULL == pSfntsArray ) ) {
        pSfntsTable = reinterpret_cast<class binaryString *>(pSfntsArray -> getElement(0)) -> getData();
        tableDirectory.load(pSfntsTable);
    } else
        tableDirectory.load(pbData);

    if ( NULL == tableDirectory.table("maxp") ) {
        sprintf(szMessage,"font %s is invalid, it does not have a maxp table",szFamily);
        throw invalidfont(szMessage);
        return;
    }

    if ( NULL == tableDirectory.table("head") ) {
        sprintf(szMessage,"font %s is invalid, it does not have a head table",szFamily);
        throw invalidfont(szMessage);
        return;
    }

    if ( NULL == tableDirectory.table("hhea") ) {
        sprintf(szMessage,"font %s is invalid, it does not have a hhea table",szFamily);
        throw invalidfont(szMessage);
        return;
    }

    if ( NULL == tableDirectory.table("hmtx") ) {
        sprintf(szMessage,"font %s is invalid, it does not have a hmtx table",szFamily);
        throw invalidfont(szMessage);
        return;
    }

    if ( ! ( NULL == tableDirectory.table("vhea") ) && ( NULL == tableDirectory.table("vmtx") ) ) {
        sprintf(szMessage,"font %s is invalid, it contains a vhea table, but does not contain a vmtx table",szFamily);
        throw invalidfont(szMessage);
        return;
    }

    if ( NULL == pSfntsArray && NULL == tableDirectory.table("cmap") ) {
        sprintf(szMessage,"font %s is invalid, it does not contain a cmap table",szFamily);
        throw invalidfont(szMessage);
        return;
    }

    pGlyfTable = tableDirectory.table("glyf");
    if ( NULL == pGlyfTable )
        pGlyfTable = tableDirectory.table("glyx");

    if ( NULL == pGlyfTable ) {
        sprintf(szMessage,"font %s is invalid, it does not have a glyf (or glyx) table",szFamily);
        throw invalidfont(szMessage);
        return;
    }

    pLocaTable = tableDirectory.table("loca");
    if ( NULL == pLocaTable )
        pLocaTable = tableDirectory.table("locx");

    if ( NULL == pLocaTable ) {
        sprintf(szMessage,"font %s is invalid, it does not have a loca (or locx ) table",szFamily);
        throw invalidfont(szMessage);
        return;
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

    scaleFUnitsToPoints = pointSize / (float)pHeadTable -> unitsPerEm;

    if ( ! ( NULL == pOS2Table ) )
        scaleFUnitsToPoints = scaleFUnitsToPoints * (float)pHeadTable -> unitsPerEm / (float)(pOS2Table -> sTypoAscender - pOS2Table -> sTypoDescender);

    if ( ! exists(pJob -> pFontTypeLiteral -> Name()) )
        put(pJob -> pFontTypeLiteral -> Name(),"42");

    if ( ! exists(pJob -> pFontNameLiteral -> Name()) )
        put(pJob -> pFontNameLiteral -> Name(),szFamily);

    if ( ! exists(pJob -> pFontMatrixLiteral -> Name()) )
        put(pJob -> pFontMatrixLiteral -> Name(),new (pJob -> CurrentObjectHeap()) class matrix(pJob));

    if ( ! exists(pJob -> pFontBoundingBoxLiteral -> Name()) ) {
        class array *pArray = new (pJob -> CurrentObjectHeap()) class array(pJob,4);
        pArray -> putElement(0,0);
        pArray -> putElement(1,0);
        pArray -> putElement(2,0);
        pArray -> putElement(3,0);
        put(pJob -> pFontBoundingBoxLiteral -> Name(),pArray);
    }

    glyphIDMap.clear();

    if ( ! exists(pJob -> pCharStringsLiteral -> Name() ) ) {
        pCharStrings = new (pJob -> CurrentObjectHeap()) dictionary(pJob,pJob -> pCharStringsLiteral -> Name());
        put(pJob -> pCharStringsLiteral -> Name(),pCharStrings);
    }

    if ( ! ( NULL == tableDirectory.table("cmap") ) ) {

        otCmapTable theCmapTable(tableDirectory.table("cmap"));

        for ( long k = 0; k < theCmapTable.numTables; k++ ) {

            if ( ! ( 3 == theCmapTable.pEncodingRecords[k].platformID ) )
                continue;

            BYTE *pLoad = (BYTE *)theCmapTable.pEncodingRecords[k].tableAddress;

            otCmapSubtableFormat4 theCmapSubtable(&pLoad);

            if ( ! ( 4 == theCmapSubtable.format ) ) {
                sprintf(szMessage,"operator: findfont. font %s is invalid. only Cmap Subtable format 4 is supported.",szFamily);
                throw invalidfont(szMessage);
                return;
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

                // You search for the first endCode that is greater than or equal 
                // to the character code you want to map

                uint16_t endCodeIndex = 0xFFFF;
                for ( uint16_t k = 0; 1; k++ ) {
                    if ( 0xFFFF == theCmapSubtable.pEndCode[k] )
                        break;
                    if ( theCmapSubtable.pEndCode[k] >= charCode ) {
                        endCodeIndex = k;
                        break;
                    }
                }

                if ( 0xFFFF == endCodeIndex ) {
                    pCharStrings -> put(szCharCode,pJob -> pZeroConstant);
                    continue;
                }

                uint16_t startCode = theCmapSubtable.pStartCode[endCodeIndex];

                /*
                If the corresponding startCode is less than or equal to the character code, then 
                you use the corresponding idDelta and idRangeOffset to map the character code to a 
                glyph index
                */

                uint16_t glyphId = 0;

                if ( startCode <= charCode ) {

                    if ( 0 == theCmapSubtable.pIdRangeOffsets[endCodeIndex] ) {

                        /*
                        If the idRangeOffset is 0, the idDelta value is added directly to 
                        the character code offset (i.e. idDelta[i] + c) to get the corresponding 
                        glyph index. Again, the idDelta arithmetic is modulo 65536.
                        */
                        glyphId = theCmapSubtable.pIdDelta[endCodeIndex] + charCode;

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

                        BYTE *pbGlyphId = (BYTE *)&theCmapSubtable.pIdRangeOffsets[endCodeIndex];
                        pbGlyphId += theCmapSubtable.pIdRangeOffsets[endCodeIndex];
                        pbGlyphId += (charCode - startCode);

                        glyphId = *(uint16_t *)pbGlyphId;

                        /*
                        The value c is the character code in question, and i is the segment index in which c appears. 
                        If the value obtained from the indexing operation is not 0 (which indicates missingGlyph), 
                        idDelta[i] is added to it to get the glyph index. The idDelta arithmetic is modulo 65536.
                        */
                        if ( ! ( 0 == glyphId ) )
                            glyphId += theCmapSubtable.pIdDelta[endCodeIndex] % 65536;

                    }

                }

                glyphIDMap[charCode] = glyphId;

                pCharStrings -> put(szCharCode,new (pJob -> CurrentObjectHeap()) object(pJob,(long)glyphId));

            }

        }

    }

    if ( ! exists(pJob -> pEncodingArrayLiteral -> Name() ) ) {

        pEncoding = new (pJob -> CurrentObjectHeap()) class array(pJob,pJob -> pEncodingArrayLiteral -> Name(),256);
        put(pJob -> pEncodingArrayLiteral -> Name(),pEncoding);

        for ( long k = 0; k < 256; k++ )
            pEncoding -> putElement(k,pJob -> pNotdefLiteral);

        dictionary *pCharStrings = reinterpret_cast<dictionary *>(retrieve(pJob -> pCharStringsLiteral -> Name()));

        for ( uint16_t charCode = 0; charCode < 256; charCode++ ) {
            char szCharCode[8];
            sprintf_s<8>(szCharCode,"%ld",charCode);
            pEncoding -> putElement(charCode,pCharStrings -> retrieveKey(szCharCode));
        }

    }

    if ( ! ( NULL == tableDirectory.table("post") ) ) 
        pPostTable = new otPostTable(tableDirectory.table("post"));

    isLoaded = true;

    return;
    }


