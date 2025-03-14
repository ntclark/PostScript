
#include <Windows.h>
#include <ocidl.h>

#include <stdio.h>
#include <conio.h>

#include "resource.h"

#include "PostScriptInterpreter_i.h"
#include "PostScriptInterpreter_i.c"

    IPostScript *pIPostScript{NULL};

    class IPostScriptEventsDemo : public IPostScriptEvents {
    public:
        //   IUnknown

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv) {
            if ( ! ppv )
                return E_POINTER;
            if ( IID_IUnknown == riid )
                *ppv = static_cast<IUnknown *>(this);
            else if ( IID_IPostScriptEvents == riid )
                *ppv = static_cast<IPostScriptEvents *>(this);
            else
                return E_NOINTERFACE;
            return S_OK;
        }
        STDMETHOD_ (ULONG, AddRef)() { return 1; }
        STDMETHOD_ (ULONG, Release)() { return 1; }

        // IPostScriptEvents

        HRESULT __stdcall RenderChar(POINT *pPoint,char c) {
            printf("Character at %d,%d: %c\n",pPoint -> x,pPoint -> y,c);
            return S_OK;
        }

        HRESULT __stdcall RenderString(POINT *pPoint,char *pszString) {
            printf("String at %d,%d: %s\n",pPoint -> x,pPoint -> y,pszString);
            return S_OK;
        }

    } postScriptEvents;


    extern "C" int main(int argc,char *argv[]) {

    HRESULT rc = CoInitializeEx(NULL,0);

    rc = CoCreateInstance(CLSID_PostScriptInterpreter,NULL,CLSCTX_ALL,IID_IPostScript,reinterpret_cast<void **>(&pIPostScript));

    IConnectionPointContainer *pIConnectionPointContainer = NULL;

    rc = pIPostScript -> QueryInterface(IID_IConnectionPointContainer,reinterpret_cast<void **>(&pIConnectionPointContainer));

    IConnectionPoint *pIConnectionPoint = NULL;

    rc = pIConnectionPointContainer -> FindConnectionPoint(IID_IPostScriptEvents,&pIConnectionPoint);

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

    pIPostScript -> ParseText(pszString,sizeData,NULL,NULL,NULL,NULL);

    delete [] pszString;

    printf("press any key to continue");

    pIConnectionPoint -> Unadvise(dwCookie);

    _getch();

    return 0;
    }