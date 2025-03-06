#pragma once

#include <Windows.h>
#include <stdio.h>

#include <objbase.h>
#include <ShlObj.h>
#include <objsafe.h>
#include <process.h>
#include <olectl.h>

#include "resource.h"

#include "Properties_i.h"
#include "PostScriptInterpreter_i.h"


class ParsePSHost : public IUnknown {
public:

    ParsePSHost();
    ~ParsePSHost();

    //   IUnknown

    STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
    STDMETHOD_ (ULONG, AddRef)();
    STDMETHOD_ (ULONG, Release)();

    HWND hwndFrame{NULL};
    HWND hwndPSHost{NULL};

    IPostScript *pIPostScript{NULL};

#define EMBEDDED_OBJECT_EMBEDDER_CLASS  ParsePSHost

#include "interfacesToSupportAnEmbeddedObject.h"

    IOleObject *pIOleObject_HTML{NULL};
    IOleInPlaceObject *pIOleInPlaceObject_HTML{NULL};
    IOleInPlaceActiveObject *pIOleInPlaceActiveObject_HTML{NULL};

    unsigned long refCount{0L};

    static LRESULT CALLBACK frameHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

    static LRESULT CALLBACK clientHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

};

#define PIXELS_TO_HIMETRIC(x,ppli)  ( (2540*(x) + ((ppli) >> 1)) / (ppli) )
#define HIMETRIC_TO_PIXELS(x,ppli)  ( ((ppli)*(x) + 1270) / 2540 )

int pixelsToHiMetric(SIZEL *pPixels,SIZEL *phiMetric);
int hiMetricToPixels(SIZEL *phiMetric,SIZEL *pPixels);

int GetLocation(HWND hwnd,long key,char *szFolderLocation);


