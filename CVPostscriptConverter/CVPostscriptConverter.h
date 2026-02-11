#pragma once

#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>
#include <shlobj.h>

#include "resource.h"

#include "CVPostscriptConverter_i.h"

#if __cplusplus

    class CVPostscriptConverter : public ICVPostscriptConverter {
    public:

        CVPostscriptConverter();
        ~CVPostscriptConverter();

        // IUnknown

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
        STDMETHOD_ (ULONG, AddRef)();
        STDMETHOD_ (ULONG, Release)();

        // ICVPostscriptConverter

        BOOL __stdcall ConvertToPDF(char *pszPostscriptFileName);
        BOOL __stdcall ConvertToPS(char *pszPDFFileName);

    private:

        long refCount{0L};
        void *gsInstance{NULL};

    };

#endif

extern "C" {
    int __stdcall Initialize(void *instance,char *pszPostscriptFileName);
    int __stdcall InitializeToPS(void *instance, char *pszPostscriptFileName);
    void __stdcall newInterpreter(void **,void *);
    void __stdcall gsapi_delete_instance(void *);
    void __stdcall gsapi_exit(void *);
    int __stdcall gsapi_init_with_args(void *,int,char **);
}

#ifdef DEFINE_DATA
   OLECHAR wstrModuleName[256];
   HMODULE hModule = NULL;
   char szModuleName[MAX_PATH];
#else
   extern OLECHAR wstrModuleName[];
   extern HMODULE hModule;
   extern char szModuleName[];
#endif
