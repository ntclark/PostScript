#pragma once

#include <Windows.h>
#include <stdint.h>

#include <stdio.h>
#include <commctrl.h>

#include "Renderer_i.h"
#include "FontManager_i.h"

#include "resource.h"

LRESULT CALLBACK frameHandler(HWND,UINT,WPARAM,LPARAM);
LRESULT dialogHandler(HWND,UINT,WPARAM,LPARAM);

void crossHairs(HDC hdc);
void drawText(HDC hdc,char *pszText,FLOAT x,FLOAT y,uint8_t flag);

#ifdef DEFINE_DATA

    HWND hwndFrame = NULL;
    HWND hwndStatus = NULL;
    HWND hwndDialog = NULL;

    IRenderer *pIRenderer = NULL;
    IGraphicElements *pIGraphicElements = NULL;
    IGraphicParameters *pIGraphicParameters = NULL;

    IFontManager *pIFontManager = NULL;
    IFont_EVNSW *pIFont = NULL;

    XFORM xFormToPixels{1.0f,0.0f,0.0f,-1.0f,0.0f};
    XFORM xFormToPoints{1.0f,0.0f,0.0f,1.0f,0.0f};

    long ticCount = 11;

    COLORREF graphBackgroundColor = RGB(0xF0,0xFF,0xFF);

#include "Renderer_i.c"
#include "FontManager_i.c"

#else

    extern HWND hwndFrame;
    extern HWND hwndStatus;
    extern HWND hwndDialog;

    extern IRenderer *pIRenderer;
    extern IGraphicElements *pIGraphicElements;
    extern IGraphicParameters *pIGraphicParameters;

    extern IFontManager *pIFontManager;
    extern IFont_EVNSW *pIFont;

    extern XFORM xFormToPixels;
    extern XFORM xFormToPoints;

    extern long ticCount;

    extern COLORREF graphBackgroundColor;

#endif