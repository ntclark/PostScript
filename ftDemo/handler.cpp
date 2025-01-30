#include <Windows.h>
#include <stdio.h>

#include "Renderer_i.h"
#include "FontManager_i.h"

#define ON_CURVE_POINT  0x01

    extern HWND hwndFrame;

    extern long cyWindow;

    extern "C" void callback(int,long x,long y,unsigned char tag);

    FLOAT scalePDF = (FLOAT)cyWindow / 792.0f;

    XFORM psXForm{1.0f,0.0f,0.0f,1.0f,0.0f,0.0f};
    XFORM gdiXForm{scalePDF,0.0f,0.0f,-scalePDF,0.0f,(FLOAT)cyWindow};

    extern IRenderer *pIGlyphRenderer;

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

RECT rc;
GetClientRect(hwnd,&rc);

cyWindow = rc.bottom - rc.top;//ps.rcPaint.bottom - ps.rcPaint.top;
scalePDF = (FLOAT)cyWindow / 792.0f;
psXForm = {0.5f,0.0f,0.0f,0.5f,0.0f,0.0f};
gdiXForm = {scalePDF,0.0f,0.0f,-scalePDF,0.0f,(FLOAT)cyWindow};

long fontSizes[]{288,144,72,36,18,9,4};

        long countVertices;

        char szText[]{"The quick brown fox jumped over the lazy dog"};

        POINTF startPoint,endPoint;

        startPoint.x = 0.0f;
        startPoint.y = 792.0f - 36.0f;

        long k = 1;

        for ( BYTE c = 0xA0; c < 0x1ff; c++ ) {

            pIFontManager -> RenderGlyph(ps.hdc,(BYTE)c,
                                        (UINT_PTR)&psXForm,(UINT_PTR)&gdiXForm,
                                            &startPoint,&endPoint);

            //MoveToEx(ps.hdc,)
            startPoint = endPoint;

            k++;
            if ( 0 == ( k % 32 ) ) {
                startPoint.y -= 36.0f / 2.0f;
                startPoint.x = 0.0f;
            }

        }

        EndPaint(hwnd,&ps);
        }
        break;

    default:
        break;
    }

    return DefWindowProc(hwnd,msg,wParam,lParam);
    }