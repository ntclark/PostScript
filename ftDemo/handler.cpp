#include <Windows.h>

#include "GlyphRenderer_i.h"
#include "FontManager_i.h"

#define ON_CURVE_POINT  0x01

    extern HWND hwndFrame;

    extern long cyWindow;

    extern "C" void callback(int,long x,long y,unsigned char tag);

    FLOAT scalePDF = (FLOAT)cyWindow / 792.0f;

    XFORM psXForm{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};
    XFORM gdiXForm{scalePDF,0.0f,0.0f,-scalePDF,0.0f,(FLOAT)cyWindow};

    extern IGlyphRenderer *pIGlyphRenderer;
    extern IGraphicElements *pIGraphicElements;
    extern IFontManager *pIFontManager;

    LRESULT CALLBACK handler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {

    switch ( msg ) {

    case WM_CREATE: {
        CREATESTRUCT *pc = reinterpret_cast<CREATESTRUCT *>(lParam);
        }
        return (LRESULT)0;

    case WM_SYSCOMMAND: {
        if ( wParam == SC_CLOSE ) {
            PostQuitMessage(0);
            return (LRESULT)0;
        }
        }
        break;

    case WM_PAINT: {

        PAINTSTRUCT ps;

        BeginPaint(hwnd,&ps);

cyWindow = ps.rcPaint.bottom - ps.rcPaint.top;
scalePDF = (FLOAT)cyWindow / 792.0f;
psXForm = {1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};
gdiXForm = {scalePDF,0.0f,0.0f,-scalePDF,0.0f,(FLOAT)cyWindow};

        long countVertices;

        char szText[]{"eThe quick brown fox jumped over the lazy dog"};

        POINTF startPoint,endPoint;

        startPoint.x = 36.0f;//612.0f / 2.0f;
        startPoint.y = 792.0f / 2.0f;

        char *p = szText;

        while ( *p ) {
            pIFontManager -> RenderGlyph(ps.hdc,(BYTE)*p,
                                        (UINT_PTR)&psXForm,(UINT_PTR)&gdiXForm,
                                            pIGlyphRenderer,
                                            &startPoint,&endPoint);
            startPoint = endPoint;
            p++;
        }

        pIGlyphRenderer -> Render();

        EndPaint(hwnd,&ps);
        }
        break;

    default:
        break;
    }

    return DefWindowProc(hwnd,msg,wParam,lParam);
    }