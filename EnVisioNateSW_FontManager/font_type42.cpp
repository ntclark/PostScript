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

#include "EnVisioNateSW_FontManager.h"

    HRESULT font::type42Load(BYTE *pbData) {

    tableDirectory.load(pbData);

    uint8_t missingTag[4];
    if ( ! tableDirectory.allRequiredSupplied(missingTag) ) {
        sprintf(FontManager::szFailureMessage,"font %s is invalid, it is missing the required table %c%c%c%c",InstalledFontName(),missingTag[0],missingTag[1],missingTag[2],missingTag[3]);
        FontManager::FireErrorNotification(FontManager::szFailureMessage);
        return NULL;
    }

    if ( ! ( NULL == tableDirectory.table("vhea") ) && ( NULL == tableDirectory.table("vmtx") ) ) {
        sprintf(FontManager::szFailureMessage,"font %s is invalid, it contains a vhea table, but does not contain a vmtx table",InstalledFontName());
        FontManager::FireErrorNotification(FontManager::szFailureMessage);
        return E_FAIL;
    }

    pGlyfTable = tableDirectory.table("glyf");
    if ( NULL == pGlyfTable )
        pGlyfTable = tableDirectory.table("glyx");

    if ( NULL == pGlyfTable ) {
        sprintf(FontManager::szFailureMessage,"font %s is invalid, it does not have a glyf (or glyx) table",InstalledFontName());
        FontManager::FireErrorNotification(FontManager::szFailureMessage);
        return E_FAIL;
    }

    pLocaTable = (uint8_t *)tableDirectory.table("loca");
    if ( NULL == pLocaTable )
        pLocaTable = (uint8_t *)tableDirectory.table("locx");

    if ( NULL == pLocaTable ) {
        sprintf(FontManager::szFailureMessage,"font %s is invalid, it does not have a loca (or locx ) table",InstalledFontName());
        FontManager::FireErrorNotification(FontManager::szFailureMessage);
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

    otCmapTable theCmapTable(tableDirectory.table("cmap"));

    boolean validPlatform = false;

    for ( long k = 0; k < theCmapTable.numTables; k++ ) {

        if ( ! ( 3 == theCmapTable.pEncodingRecords[k].platformID ) )
            continue;

        if ( ! ( 0 == theCmapTable.pEncodingRecords[k].encodingID ) &&
                ! ( 1 == theCmapTable.pEncodingRecords[k].encodingID ) && 
                    ! ( 10 == theCmapTable.pEncodingRecords[k].encodingID ) ) {
            sprintf(FontManager::szFailureMessage,"font %s is invalid. only cmap platformID = 3 and encodingID = [0 | 1 | 10] is supported.",InstalledFontName());
            FontManager::FireErrorNotification(FontManager::szFailureMessage);
            return E_FAIL;
        }

        validPlatform = true;

        BYTE *pLoad = (BYTE *)theCmapTable.pEncodingRecords[k].tableAddress;

        uint16_t format;
        BE_TO_LE_U16(pLoad,format)

        pLoad -= sizeof(uint16_t);

        if ( 0 == theCmapTable.pEncodingRecords[k].encodingID || 1 == theCmapTable.pEncodingRecords[k].encodingID ) {

            if ( ! ( 4 == format ) ) {
                sprintf(FontManager::szFailureMessage,"font %s is invalid. Only Cmap Subtable format 4 is supported when encodingID = 1.",InstalledFontName());
                FontManager::FireErrorNotification(FontManager::szFailureMessage);
                return E_FAIL;
            }

            pCmapSubtableFormat4 = new otCmapSubtableFormat4(&pLoad);

            pCmapSubtableFormat4 -> loadFormat4(&pLoad);

        } else if ( 10 == theCmapTable.pEncodingRecords[k].encodingID ) {

            if ( ! ( 12 == format ) ) {
                sprintf(FontManager::szFailureMessage,"font %s is invalid. Only Cmap Subtable format 12 is supported WHEN encodingID = 10.",InstalledFontName());
                FontManager::FireErrorNotification(FontManager::szFailureMessage);
                return E_FAIL;
            }

            sprintf(FontManager::szFailureMessage,"font %s is invalid. Only Cmap Subtable format 12 is not implemented yet.",InstalledFontName());
            FontManager::FireErrorNotification(FontManager::szFailureMessage);
            return E_NOTIMPL;

        }

        break;

    }

    if ( ! validPlatform ) {
        sprintf(FontManager::szFailureMessage,"font %s is invalid. only cmap platformID = 3 and encodingID = [0 | 1 | 10] is supported.",InstalledFontName());
        FontManager::FireErrorNotification(FontManager::szFailureMessage);
        return E_FAIL;
    }

    pPostTable = new otPostTable(tableDirectory.table("post"));

    isLoaded = true;

    return S_OK;
    }


