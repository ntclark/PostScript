
#pragma once

#include <windows.h>

long HashCode(char *pszInput);
void ASCIIHexDecodeInPlace(char *pszInput);
void ASCIIHexEncode(char *pszInput,long valueSize,char **ppszResult);
int pixelsToHiMetric(SIZEL *pPixels,SIZEL *phiMetric);
int hiMetricToPixel(SIZEL *phiMetric,SIZEL *pPixels);

