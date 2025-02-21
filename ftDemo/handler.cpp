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

        startPoint.x = 36.0f;
        startPoint.y = 792.0f - 36.0f;

            pIFontManager -> RenderGlyph(ps.hdc,8211,
                                        (UINT_PTR)&psXForm,(UINT_PTR)&gdiXForm,
                                            &startPoint,&endPoint);

            pIGlyphRenderer -> Render();

        long k = 1;

        for ( unsigned short c = 0x96; c < 0xfff; c++ ) {

            pIFontManager -> RenderGlyph(ps.hdc,c,
                                        (UINT_PTR)&psXForm,(UINT_PTR)&gdiXForm,
                                            &startPoint,&endPoint);

            pIGlyphRenderer -> Render();

            POINTL pt{startPoint.x,startPoint.y};
            FLOAT xx = pt.x;
            FLOAT yy = pt.y - 8;
            FLOAT xxx = gdiXForm.eM11 * xx + gdiXForm.eM12 * yy + gdiXForm.eDx;
            FLOAT yyy = gdiXForm.eM21 * xx + gdiXForm.eM22 * yy + gdiXForm.eDy;

            MoveToEx(ps.hdc,xxx,yyy,NULL);
            RECT rc{xxx - 8,yyy - 8,xxx + 12,yyy + 8};

            char szX[16];
            sprintf_s<16>(szX,"%03x",c);
            DrawTextEx(ps.hdc,szX,-1,&rc,DT_CENTER,NULL);

            startPoint = endPoint;

            k++;
            if ( 0 == ( k % 32 ) ) {
                startPoint.y -= 54.0f / 2.0f;
                startPoint.x = 36.0f;
                if ( 0 > startPoint.y ) {
                    FillRect(ps.hdc,&ps.rcPaint,(HBRUSH)(COLOR_WINDOW + 1));
                    startPoint.y = 792.0f - 36.0f;
                }
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