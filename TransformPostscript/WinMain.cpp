
#include <Windows.h>
#include <stdio.h>

#include "CVPostscriptConverter_i.h"
#include "CVPostscriptConverter_i.c"

    int __stdcall WinMain(HINSTANCE hInst,HINSTANCE hInstancePrevious,LPSTR lpCmdLine,int nCmdShow) {

    WCHAR *pCommandLine = GetCommandLine();
    char **argv;
    int argc = 0;

    long n = (DWORD)wcslen(pCommandLine) + 1;
    BSTR bstrCommandLine = SysAllocStringLen(GetCommandLine(),n);
    BSTR *pArgs = CommandLineToArgvW(bstrCommandLine,&argc);
    argv = new char *[argc];

    for ( long k = 0; k < argc; k++ ) {
        argv[k] = new char[wcslen(pArgs[k]) + 1];
        memset(argv[k],0,(wcslen(pArgs[k]) + 1) * sizeof(char));
        WideCharToMultiByte(CP_ACP,0,pArgs[k],-1,argv[k],(DWORD)wcslen(pArgs[k]),0,0);
    }

    CoInitialize(NULL);

    ICVPostscriptConverter *pICVPostscriptConverter = NULL;

    CoCreateInstance(CLSID_CVPostscriptConverter,NULL,CLSCTX_ALL,IID_ICVPostscriptConverter,reinterpret_cast<void **>(&pICVPostscriptConverter));

    if ( 3 == argc )
        pICVPostscriptConverter -> ConvertToPS(argv[argc - 2]);
    else
        pICVPostscriptConverter -> ConvertToPDF(argv[argc - 1]);

    pICVPostscriptConverter -> Release();

    return 0;
    }
