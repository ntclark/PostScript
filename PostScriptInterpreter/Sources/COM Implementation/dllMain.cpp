/*
Copyright 2025 EnVisioNate LLC

Permission is hereby granted, free of charge, to any person obtaining a 
copy of this software and associated documentation files (the �Software�), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the 
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included 
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This is the MIT License
*/

#include <windows.h>

#include <olectl.h>
#include <stdio.h>

#define OBJECT_NAME "EnVisioNateSW.PostScriptInterpreter"
#define OBJECT_NAME_V "EnVisioNateSW.PostScriptInterpreter.1"
#define OBJECT_VERSION "1.0"
#define OBJECT_CLSID CLSID_PostScriptInterpreter
#define OBJECT_LIBID LIBID_PostScriptInterpreter

#define DEFINE_DATA

#include "utilities.h"

#include "PostScriptInterpreter.h"
#include "PostScriptInterpreter_i.c"
#include "Renderer_i.c"

#include "PostScript objects/object.h"

   OLECHAR wstrModuleName[256];

    extern "C" BOOL WINAPI DllMain(HINSTANCE hI, DWORD dwReason, LPVOID) {

    switch ( dwReason ) {

    case DLL_PROCESS_ATTACH: {

        hModule = reinterpret_cast<HMODULE>(hI);

        GetModuleFileName(hModule,szModuleName,256);
        GetModuleFileNameW(hModule,wstrModuleName,256);

        GetCommonAppDataLocation(NULL,szApplicationDataDirectory);
        strcat(szApplicationDataDirectory,"\\EnVisioNateSW");

        CreateDirectory(szApplicationDataDirectory,NULL);

        LoadLibrary("Riched20.dll");

        }

        break;
  
    case DLL_PROCESS_DETACH:
        DeleteCriticalSection(&PostScriptInterpreter::theQueueCriticalSection);
        break;
  
    }
  
    return TRUE;
    }


    class Factory : public IClassFactory {
    public:

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
        STDMETHOD_ (ULONG, AddRef)();
        STDMETHOD_ (ULONG, Release)();
        STDMETHOD (CreateInstance)(IUnknown *punkOuter, REFIID riid, void **ppv);
        STDMETHOD (LockServer)(BOOL fLock);

        Factory() : refCount(0) {};
        ~Factory() {};

    private:
        int refCount;
    };


    static Factory factory;

    STDAPI DllCanUnloadNow() {
    return S_OK;
    }


    STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppObject) {
    *ppObject = NULL;
    if ( rclsid != OBJECT_CLSID ) 
        return CLASS_E_CLASSNOTAVAILABLE;
    return factory.QueryInterface(riid,ppObject);
    }


    STDAPI DllRegisterServer() {

    HRESULT rc = S_OK;
    ITypeLib *ptLib;
    HKEY keyHandle,clsidHandle;
    DWORD disposition;
    char szTemp[256],szCLSID[256];
    LPOLESTR oleString;

    StringFromCLSID(OBJECT_CLSID,&oleString);
    WideCharToMultiByte(CP_ACP,0,oleString,-1,szCLSID,256,0,0);

    if ( S_OK != LoadTypeLib(wstrModuleName,&ptLib) )
        rc = ResultFromScode(SELFREG_E_TYPELIB);
    else
        if ( S_OK != RegisterTypeLib(ptLib,wstrModuleName,NULL) )
            rc = ResultFromScode(SELFREG_E_TYPELIB);

    RegOpenKeyEx(HKEY_CLASSES_ROOT,"CLSID",0,KEY_CREATE_SUB_KEY,&keyHandle);

    RegCreateKeyEx(keyHandle,szCLSID,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&clsidHandle,&disposition);
    sprintf(szTemp,"EnVisioNateSW PostScript Interpreter Object");
    RegSetValueEx(clsidHandle,NULL,0,REG_SZ,(BYTE *)szTemp,(DWORD)strlen(szTemp));

    sprintf(szTemp,"Control");
    RegCreateKeyEx(clsidHandle,szTemp,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    sprintf(szTemp,"");
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szTemp,(DWORD)strlen(szTemp));

    sprintf(szTemp,"ProgID");
    RegCreateKeyEx(clsidHandle,szTemp,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    sprintf(szTemp,OBJECT_NAME_V);
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szTemp,(DWORD)strlen(szTemp));

    sprintf(szTemp,"InprocServer");
    RegCreateKeyEx(clsidHandle,szTemp,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szModuleName,(DWORD)strlen(szModuleName));

    sprintf(szTemp,"InprocServer32");
    RegCreateKeyEx(clsidHandle,szTemp,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szModuleName,(DWORD)strlen(szModuleName));
    RegSetValueEx(keyHandle,"ThreadingModel",0,REG_SZ,(BYTE *)"Both",4);

    sprintf(szTemp,"LocalServer");
    RegCreateKeyEx(clsidHandle,szTemp,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szModuleName,(DWORD)strlen(szModuleName));

    sprintf(szTemp,"TypeLib");
    RegCreateKeyEx(clsidHandle,szTemp,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);

    StringFromCLSID(OBJECT_LIBID,&oleString);
    WideCharToMultiByte(CP_ACP,0,oleString,-1,szTemp,256,0,0);
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szTemp,(DWORD)strlen(szTemp));
        
    sprintf(szTemp,"ToolboxBitmap32");
    RegCreateKeyEx(clsidHandle,szTemp,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);

    sprintf(szTemp,"Version");
    RegCreateKeyEx(clsidHandle,szTemp,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    sprintf(szTemp,OBJECT_VERSION);
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szTemp,(DWORD)strlen(szTemp));

    sprintf(szTemp,"MiscStatus");
    RegCreateKeyEx(clsidHandle,szTemp,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    sprintf(szTemp,"0");
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szTemp,(DWORD)strlen(szTemp));

    sprintf(szTemp,"1");
    RegCreateKeyEx(keyHandle,szTemp,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    sprintf(szTemp,"%ld",
                OLEMISC_ALWAYSRUN |
                OLEMISC_ACTIVATEWHENVISIBLE | 
                OLEMISC_RECOMPOSEONRESIZE | 
                OLEMISC_INSIDEOUT |
                OLEMISC_SETCLIENTSITEFIRST |
                OLEMISC_CANTLINKINSIDE );

    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szTemp,(DWORD)strlen(szTemp));

    RegCreateKeyEx(HKEY_CLASSES_ROOT,OBJECT_NAME,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
        RegCreateKeyEx(keyHandle,"CurVer",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
        sprintf(szTemp,OBJECT_NAME_V);
        RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szTemp,(DWORD)strlen(szTemp));

    RegCreateKeyEx(HKEY_CLASSES_ROOT,OBJECT_NAME_V,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
        RegCreateKeyEx(keyHandle,"CLSID",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
        RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szCLSID,(DWORD)strlen(szCLSID));

    return S_OK;
    }


    STDAPI DllUnregisterServer() {

    HRESULT rc = S_OK;
    HKEY keyHandle;
    char szCLSID[256];
    LPOLESTR oleString;

    StringFromCLSID(OBJECT_CLSID,&oleString);
    WideCharToMultiByte(CP_ACP,0,oleString,-1,szCLSID,256,0,0);

    RegOpenKeyEx(HKEY_CLASSES_ROOT,"CLSID",0,KEY_CREATE_SUB_KEY,&keyHandle);
    rc = SHDeleteKey(keyHandle,szCLSID);
    rc = SHDeleteKey(HKEY_CLASSES_ROOT,OBJECT_NAME);
    rc = SHDeleteKey(HKEY_CLASSES_ROOT,OBJECT_NAME_V);
    return S_OK;
    }


    long __stdcall Factory::QueryInterface(REFIID iid, void **ppv) { 
    *ppv = NULL; 
    if ( iid == IID_IUnknown || iid == IID_IClassFactory ) 
        *ppv = this; 
    else 
        return E_NOINTERFACE; 
    AddRef(); 
    return S_OK; 
    } 


    unsigned long __stdcall Factory::AddRef() { 
    return ++refCount; 
    } 


    unsigned long __stdcall Factory::Release() { 
    return --refCount;
    } 


    HRESULT STDMETHODCALLTYPE Factory::CreateInstance(IUnknown *punkOuter, REFIID riid, void **ppv) { 
    HRESULT hres;
    *ppv = NULL; 
    PostScriptInterpreter *pef = new PostScriptInterpreter(punkOuter);
    hres = pef -> QueryInterface(riid,ppv);
    if ( !*ppv ) delete pef;
    return hres;
    } 


    long __stdcall Factory::LockServer(int fLock) { 
    return S_OK; 
    }
