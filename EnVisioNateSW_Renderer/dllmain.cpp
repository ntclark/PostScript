
#include "Renderer.h"

    extern "C" BOOL WINAPI DllMain(HINSTANCE hI, DWORD dwReason, LPVOID) {

    switch ( dwReason ) {

    case DLL_PROCESS_ATTACH: {
        CoInitialize(NULL);
        hModule = hI;
        GetModuleFileName(hModule,szModuleName,1024);
        MultiByteToWideChar(CP_ACP, 0, szModuleName, -1, wstrModuleName, 1024);  
        }
        break;

    case DLL_PROCESS_DETACH:
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

    static Factory objectFactory;

    STDAPI DllCanUnloadNow(void) {
    return S_FALSE;
    }


    STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppObject) {
    *ppObject = NULL;
    if ( ! ( CLSID_Renderer == rclsid  ) )
        return CLASS_E_CLASSNOTAVAILABLE;
    return objectFactory.QueryInterface(riid,ppObject);
    }


    const char *OBJECT_NAME = "EnVisioNateSW.Renderer";
    const char *OBJECT_NAME_V = "EnVisioNateSW.Renderer.1";
    const char *OBJECT_VERSION = "1.0";
    const char *OBJECT_DESCRIPTION = "Renderer to render Graphics";

    GUID OBJECT_CLSID;

    STDAPI DllRegisterServer() {

    GUID OBJECT_LIBID;

    memcpy(&OBJECT_CLSID,&CLSID_Renderer,sizeof(GUID));
    memcpy(&OBJECT_LIBID,&LIBID_Renderer,sizeof(GUID));

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
    sprintf_s<256>(szTemp,OBJECT_DESCRIPTION);
    RegSetValueEx(clsidHandle,NULL,0,REG_SZ,(BYTE *)szTemp,(DWORD)strlen(szTemp));

    RegCreateKeyEx(clsidHandle,"Control",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)"",(DWORD)strlen(szTemp));

    RegCreateKeyEx(clsidHandle,"ProgID",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    sprintf(szTemp,OBJECT_NAME_V);
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szTemp,(DWORD)strlen(szTemp));

    RegCreateKeyEx(clsidHandle,"InprocServer",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szModuleName,(DWORD)strlen(szModuleName));

    RegCreateKeyEx(clsidHandle,"InprocServer32",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szModuleName,(DWORD)strlen(szModuleName));
    RegSetValueEx(keyHandle,"ThreadingModel",0,REG_SZ,(BYTE *)"Free",5);
    //RegSetValueEx(keyHandle,"ThreadingModel",0,REG_SZ,(BYTE *)"Apartment",9);

    RegCreateKeyEx(clsidHandle,"LocalServer",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szModuleName,(DWORD)strlen(szModuleName));
    
    RegCreateKeyEx(clsidHandle,"TypeLib",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);

    StringFromCLSID(OBJECT_LIBID,&oleString);
    WideCharToMultiByte(CP_ACP,0,oleString,-1,szTemp,256,0,0);
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szTemp,(DWORD)strlen(szTemp));

    RegCreateKeyEx(clsidHandle,"Version",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    sprintf_s<256>(szTemp,OBJECT_VERSION);
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szTemp,(DWORD)strlen(szTemp));

    RegCreateKeyEx(clsidHandle,"MiscStatus",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)"0",(DWORD)strlen(szTemp));

    RegCreateKeyEx(keyHandle,"1",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    sprintf_s<256>(szTemp,"%ld",
                 OLEMISC_ALWAYSRUN |
                 OLEMISC_ACTIVATEWHENVISIBLE | 
                 OLEMISC_RECOMPOSEONRESIZE | 
                 OLEMISC_INSIDEOUT |
                 OLEMISC_SETCLIENTSITEFIRST |
                 OLEMISC_CANTLINKINSIDE );
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szTemp,(DWORD)strlen(szTemp));

    RegCreateKeyEx(HKEY_CLASSES_ROOT,OBJECT_NAME,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    RegCreateKeyEx(keyHandle,"CurVer",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    sprintf_s<256>(szTemp,OBJECT_NAME_V);
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szTemp,(DWORD)strlen(szTemp));

    RegCreateKeyEx(HKEY_CLASSES_ROOT,OBJECT_NAME_V,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    RegCreateKeyEx(keyHandle,"CLSID",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&keyHandle,&disposition);
    RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szCLSID,(DWORD)strlen(szCLSID));

    return S_OK;
    }


    STDAPI DllUnregisterServer() {

    CoInitialize(NULL);

    memcpy(&OBJECT_CLSID,&CLSID_Renderer,sizeof(GUID));

    HKEY keyHandle;
    char szCLSID[256];
    LPOLESTR oleString;

    StringFromCLSID(OBJECT_CLSID,&oleString);
    WideCharToMultiByte(CP_ACP,0,oleString,-1,szCLSID,256,0,0);

    RegOpenKeyEx(HKEY_CLASSES_ROOT,"CLSID",0,KEY_CREATE_SUB_KEY,&keyHandle);

    HRESULT rc = SHDeleteKey(keyHandle,szCLSID);
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

    *ppv = NULL; 

    Renderer *pef = new Renderer(punkOuter);
    HRESULT hres = pef -> QueryInterface(riid,ppv);

    if ( ! ( S_OK == hres ) ) 
        delete pef;

    return hres;
    } 


    long __stdcall Factory::LockServer(int fLock) { 
    return S_OK; 
    }