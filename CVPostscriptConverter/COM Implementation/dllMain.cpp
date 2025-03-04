
#define DEFINE_DATA

#include "CVPostscriptConverter.h"

#include "olectl.h"

#include "CVPostscriptConverter_i.c"

   extern "C" BOOL WINAPI DllMain(HINSTANCE hI, DWORD dwReason, LPVOID) {

   switch ( dwReason ) {

   case DLL_PROCESS_ATTACH: {

      CoInitialize(NULL);

      hModule = hI;

      GetModuleFileName(hModule,szModuleName,1024);
      memset(wstrModuleName,0,sizeof(wstrModuleName));

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
   if ( CLSID_CVPostscriptConverter != rclsid  ) 
      return CLASS_E_CLASSNOTAVAILABLE;
   return objectFactory.QueryInterface(riid,ppObject);
   }
  
   char *OBJECT_NAME;
   char *OBJECT_NAME_V;
   GUID OBJECT_CLSID;


   STDAPI DllRegisterServer() {

   char *OBJECT_VERSION;
   GUID OBJECT_LIBID;
   char *OBJECT_DESCRIPTION;

   OBJECT_NAME = (char *)"EnVisioNateSW.CVPostscriptConverter";
   OBJECT_NAME_V = (char *)"EnVisioNateSW.CVPostscriptConverter.1";
   OBJECT_VERSION = (char *)"1.0";

   memcpy(&OBJECT_CLSID,&CLSID_CVPostscriptConverter,sizeof(GUID));
   memcpy(&OBJECT_LIBID,&LIBID_CVPostscriptConverter,sizeof(GUID));

   OBJECT_DESCRIPTION = (char *)"A COM object for Converting Postscript";

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
      sprintf(szTemp,OBJECT_DESCRIPTION);
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
//      RegSetValueEx(keyHandle,"ThreadingModel",0,REG_SZ,(BYTE *)"Free",5);
      RegSetValueEx(keyHandle,"ThreadingModel",0,REG_SZ,(BYTE *)"Apartment",9);
  
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
//      sprintf(szTemp,"%s, 1",szModuleName);
//      RegSetValueEx(keyHandle,NULL,0,REG_SZ,(BYTE *)szTemp,(DWORD)strlen(szModuleName));
  
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
  
#if false
   ICatRegister *pICatRegister;

   rc = CoCreateInstance(CLSID_StdComponentCategoriesMgr,NULL,CLSCTX_ALL,IID_ICatRegister,reinterpret_cast<void **>(&pICatRegister));

   CATID categoryId = IID_ICursiVisionBackEnd;

   pICatRegister -> RegisterClassImplCategories(CLSID_CursiVisionNamingBackEnd,1,&categoryId);

   pICatRegister -> Release();
#endif

   return S_OK;
   }
  
  
   STDAPI DllUnregisterServer() {

   CoInitialize(NULL);

   OBJECT_NAME = (char *)"EnVisioNateSW.CVPostscriptConverter";
   OBJECT_NAME_V = (char *)"EnVisioNateSW.CVPostscriptConverter.1";
   memcpy(&OBJECT_CLSID,&CLSID_CVPostscriptConverter,sizeof(GUID));

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

   HRESULT hres;

   *ppv = NULL; 

   CVPostscriptConverter *pef = new CVPostscriptConverter();

   hres = pef -> QueryInterface(riid,ppv);

   if ( ! *ppv ) 
      delete pef;

   return hres;
   } 
  
  
   long __stdcall Factory::LockServer(int fLock) { 
   return S_OK; 
   }