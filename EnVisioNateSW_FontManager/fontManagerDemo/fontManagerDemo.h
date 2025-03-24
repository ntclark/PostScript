#pragma once

#include <Windows.h>
#include <list>

#include "resource.h"

#include "Renderer_i.h"
#include "FontManager_i.h"

#define WHITE RGB(255,255,255)
#define GLYPH_TABLE_COLUMN_COUNT 20
#define GLYPH_TABLE_ROW_COUNT 20
#define GLYPH_TABLE_MARGIN_LEFT 32
#define GLYPH_TABLE_MARGIN_RIGHT 32
#define GLYPH_TABLE_MARGIN_TOP 32
#define GLYPH_TABLE_MARGIN_BOTTOM 64

    LRESULT CALLBACK frameHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
    LRESULT CALLBACK dialogHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
    LRESULT CALLBACK glyphTableDialogHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
    LRESULT CALLBACK glyphTableHostHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

    void setFont();
    void generateGlyphTable();
    void calcTransforms(HWND hwnd);

#ifdef DEFINE_DATA
    HWND hwndFrame = NULL;
    HWND hwndDialog = NULL;
    HWND hwndGlyphTableDialog = NULL;
    HWND hwndGlyphTableHost = NULL;

    HBITMAP hbmGlyphTable = NULL;
    HDC hdcGlyphTable = NULL;

    IRenderer *pIRenderer = NULL;
    IGraphicElements *pIGraphicElements = NULL;
    IFontManager *pIFontManager = NULL;
    IFont_EVNSW *pIFont = NULL;

    long cxRenderPane = 0;
    long cyRenderPane = 0;

    SCROLLINFO scrollInfo{0};

    FLOAT scalePDF = 1.0f;

    XFORM psXForm{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};
    XFORM gdiXForm{0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};

#include "Renderer_i.c"
#include "FontManager_i.c"

#else
    extern HWND hwndFrame;
    extern HWND hwndDialog;
    extern HWND hwndGlyphTableDialog;
    extern HWND hwndGlyphTableHost;

    extern HBITMAP hbmGlyphTable;
    extern HDC hdcGlyphTable;

    extern IRenderer *pIRenderer;
    extern IGraphicElements *pIGraphicElements;
    extern IFontManager *pIFontManager;
    extern IFont_EVNSW *pIFont;

    extern long cxRenderPane;
    extern long cyRenderPane;

    extern SCROLLINFO scrollInfo;

    extern FLOAT scalePDF;

    extern XFORM psXForm;
    extern XFORM gdiXForm;

#endif
