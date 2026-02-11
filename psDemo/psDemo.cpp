
#include <Windows.h>
#include <ocidl.h>

#include <stdio.h>
#include <conio.h>

#include "resource.h"

#include "PostScriptInterpreter_i.h"
#include "PostScriptInterpreter_i.c"

    IPostScriptInterpreter *pIPostScript{NULL};

    class IPostScriptEventsDemo : public IPostScriptInterpreterEvents {
    public:
        //   IUnknown

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv) {
            if ( ! ppv )
                return E_POINTER;
            if ( IID_IUnknown == riid )
                *ppv = static_cast<IUnknown *>(this);
            else if ( IID_IPostScriptInterpreterEvents == riid )
                *ppv = static_cast<IPostScriptInterpreterEvents *>(this);
            else
                return E_NOINTERFACE;
            return S_OK;
        }
        STDMETHOD_ (ULONG, AddRef)() { return 1; }
        STDMETHOD_ (ULONG, Release)() { return 1; }

        // IPostScriptInterpreterEvents

        HRESULT __stdcall RenderChar(POINT *pStartPoint,POINT *pEndPoint,char c) {
            printf("Character at: %d-%d, width: %d, height: %d: %c\n",pStartPoint -> x,pStartPoint -> y,pEndPoint -> x - pStartPoint -> x,pEndPoint -> y - pStartPoint -> y,c);
            return S_OK;
        }

        HRESULT __stdcall RenderString(POINT *pStartPoint,POINT *pEndPoint,char *pszString) {
            printf("String at: %d-%d, width: %d, height: %d: %s\n",pStartPoint -> x,pStartPoint -> y,pEndPoint -> x - pStartPoint -> x,pEndPoint -> y - pStartPoint -> y,pszString);
            return S_OK;
        }

        HRESULT __stdcall PageShown(POINT *) {
            return S_OK;
        }

        HRESULT __stdcall ErrorNotification(UINT_PTR pszString) {
            printf("There was an error: %s\n",(char *)pszString);
            return S_OK;
        }

    } postScriptEvents;


    extern "C" int main(int argc,char *argv[]) {

    HRESULT rc = CoInitializeEx(NULL,0);

    rc = CoCreateInstance(CLSID_PostScriptInterpreter,NULL,CLSCTX_ALL,IID_IPostScriptInterpreter,reinterpret_cast<void **>(&pIPostScript));

    IConnectionPointContainer *pIConnectionPointContainer = NULL;
    IConnectionPoint *pIConnectionPoint = NULL;

    rc = pIPostScript -> QueryInterface(IID_IConnectionPointContainer,reinterpret_cast<void **>(&pIConnectionPointContainer));

    rc = pIConnectionPointContainer -> FindConnectionPoint(IID_IPostScriptInterpreterEvents,&pIConnectionPoint);

    DWORD dwCookie = 0L;
    IUnknown *pIUnknownSink = NULL;

    postScriptEvents.QueryInterface(IID_IUnknown,reinterpret_cast<void **>(&pIUnknownSink));

    rc = pIConnectionPoint -> Advise(pIUnknownSink,&dwCookie);

    HRSRC hrsrc = FindResourceA(NULL,MAKEINTRESOURCE(ID_POSTSCRIPT_FILE),"#256");
    HGLOBAL hResource = LoadResource(NULL,hrsrc);
    SIZE_T sizeData = SizeofResource(NULL,hrsrc);

    char *pszString = new char[sizeData + 1];
    memcpy(pszString,(char *)LockResource(hResource),sizeData);
    pszString[sizeData] = '\0';

    pIPostScript -> ParseText(pszString);

    delete [] pszString;

    printf("press any key to continue");

    pIConnectionPoint -> Unadvise(dwCookie);

    pIConnectionPoint -> Release();
    pIConnectionPointContainer -> Release();

    pIPostScript -> Release();

    _getch();

    return 0;
    }