#include <Windows.h>

#ifdef USE_RENDERER
#include "Renderer_i.h"
#else
#include "GlyphRenderer_i.h"
#endif
#include "FontManager_i.h"

#define ON_CURVE_POINT  0x01

    extern HWND hwndFrame;

    extern long cyWindow;

    extern "C" void callback(int,long x,long y,unsigned char tag);

    FLOAT scalePDF = (FLOAT)cyWindow / 792.0f;

    XFORM psXForm{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};
    XFORM gdiXForm{scalePDF,0.0f,0.0f,-scalePDF,0.0f,(FLOAT)cyWindow};

#ifdef USE_RENDERER
    extern IRenderer *pIGlyphRenderer;
#else
    extern IGlyphRenderer *pIGlyphRenderer;
#endif

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

FillRect(ps.hdc,&ps.rcPaint,(HBRUSH) (COLOR_WINDOW + 1));

cyWindow = ps.rcPaint.bottom - ps.rcPaint.top;
scalePDF = (FLOAT)cyWindow / 792.0f;
psXForm = {0.5f,0.0f,0.0f,0.5f,0.0f,0.0f};
gdiXForm = {scalePDF,0.0f,0.0f,-scalePDF,0.0f,(FLOAT)cyWindow};

        long countVertices;

        char szText[]{"The quick brown fox jumped over the lazy dog"};

        POINTF startPoint,endPoint;

        FLOAT fontSize = 288.0f;

        for ( long k = 0; k < 10; k++ ) {

            fontSize /= 1.5f;

            pIFontManager -> put_PointSize(fontSize);

            startPoint.x = 0.0f;
            startPoint.y = 8 * 72.0 - (FLOAT) k * 72.0 / 2.0f;

            char *p = szText;

            while ( *p ) {
                pIFontManager -> RenderGlyph(ps.hdc,(BYTE)*p,
                                            (UINT_PTR)&psXForm,(UINT_PTR)&gdiXForm,
                                                NULL,//pIGlyphRenderer,
                                                &startPoint,&endPoint);
                startPoint = endPoint;
                p++;
            }

#if 0
            p = szText;

            startPoint.x = 2.0f;
            startPoint.y = 5 * 72.0 - (FLOAT) k * 72.0;

            while ( *p ) {
                pIFontManager -> RenderGlyph(ps.hdc,(BYTE)*p,
                                            (UINT_PTR)&psXForm,(UINT_PTR)&gdiXForm,
                                                pIGlyphRenderer,
                                                &startPoint,&endPoint);

                startPoint = endPoint;
                p++;
            }
#endif
        }

        EndPaint(hwnd,&ps);
        }
        break;

    default:
        break;
    }

    return DefWindowProc(hwnd,msg,wParam,lParam);
    }