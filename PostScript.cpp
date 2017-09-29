
#include "PostScript.h"

#include "PostScript_i.c"

   CRITICAL_SECTION PStoPDF::theQueueCriticalSection;
   bool PStoPDF::logPaintPending = false;
   WNDPROC PStoPDF::nativeRichEditHandler = NULL;
   HANDLE PStoPDF::hRichEditSemaphore;

   char PStoPDF::szErrorMessage[1024];

   PStoPDF::PStoPDF(IUnknown *pUnknownOuter) :

      pIConnectionPointContainer(NULL),
      pIConnectionPoint(NULL),
      pIEnumConnectionPoints(NULL),
      pIEnumerateConnections(NULL),

      pIOleObject(NULL),
      pIOleInPlaceObject(NULL),
      pIOleClientSite(NULL),
      pIOleInPlaceSite(NULL),

      hwndHost(NULL),
      hwndClient(NULL),
      hwndLog(NULL),
      hwndStack(NULL),

      pJob(NULL),
      refCount(0)

   {

   pPStoPDF = this;

   pIOleObject = new _IOleObject(this);
   pIOleInPlaceObject = new _IOleInPlaceObject(this);

   pIConnectionPointContainer = new _IConnectionPointContainer(this);
   pIConnectionPoint = new _IConnectionPoint(this);

   INITCOMMONCONTROLSEX icex;
   icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
   icex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_TAB_CLASSES | ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS;
   InitCommonControlsEx(&icex);
   LoadLibrary("Msftedit.dll");

   InitializeCriticalSection(&theQueueCriticalSection);

   return;
   }


   PStoPDF::~PStoPDF() {

   delete pIOleObject;
   delete pIOleInPlaceObject;
   delete pIConnectionPointContainer;
   delete pIConnectionPoint;

   if ( pIOleInPlaceSite )
      pIOleInPlaceSite -> Release();

   if ( pIOleClientSite )
      pIOleClientSite -> Release();

   if ( pIEnumConnectionPoints )
      pIEnumConnectionPoints -> Release();

   if ( pIEnumerateConnections )
      pIEnumerateConnections -> Release();

   return;
   }


   int PStoPDF::initWindows() {
 
   pIOleInPlaceSite -> GetWindow(&hwndHost);

   WNDCLASS gClass;
   
   memset(&gClass,0,sizeof(WNDCLASS));
   gClass.style = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
   gClass.lpfnWndProc = PStoPDF::handler;
   gClass.cbClsExtra = 32;
   gClass.cbWndExtra = 32;
   gClass.hInstance = hModule;
   gClass.hIcon = NULL;
   gClass.hCursor = NULL;
   gClass.hbrBackground = 0;
   gClass.lpszMenuName = NULL;
   gClass.lpszClassName = "PStoPDF";
  
   RegisterClass(&gClass);

   hwndClient = CreateWindowEx(0L,"PStoPDF","",WS_CHILD,0,0,0,0,hwndHost,NULL,NULL,reinterpret_cast<void *>(this));

   hwndLog = CreateWindowEx(WS_EX_CLIENTEDGE,"RICHEDIT50W","",WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL,
                                       0,0,0,0,hwndClient,NULL,NULL,reinterpret_cast<void *>(this));

   hwndStack = CreateWindowEx(WS_EX_CLIENTEDGE,"RICHEDIT50W","",WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL,
                                          0,0,0,0,hwndClient,NULL,NULL,reinterpret_cast<void *>(this));

   nativeRichEditHandler = (WNDPROC)SetWindowLongPtr(hwndLog,GWLP_WNDPROC,(LONG_PTR)PStoPDF::richEditHandler);

   SetWindowLongPtr(hwndStack,GWLP_WNDPROC,(LONG_PTR)PStoPDF::richEditHandler);

   SetWindowLongPtr(hwndLog,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(this));

   CHARFORMAT charFormat;
   memset(&charFormat,0,sizeof(CHARFORMAT));

   charFormat.cbSize = sizeof(CHARFORMAT);
   charFormat.dwMask = CFM_FACE | CFM_SIZE;

   strcpy(charFormat.szFaceName,"Courier New");
   charFormat.yHeight = 10 * 20;

   SendMessage(hwndLog,EM_SETCHARFORMAT,(WPARAM)SCF_ALL,(LPARAM)&charFormat);
   SendMessage(hwndLog,EM_SHOWSCROLLBAR,(WPARAM)SB_VERT,(LPARAM)TRUE);
   SendMessage(hwndLog,EM_SHOWSCROLLBAR,(WPARAM)SB_HORZ,(LPARAM)TRUE);

   SETTEXTEX st;
   memset(&st,0,sizeof(SETTEXTEX));
   st.flags = ST_DEFAULT;
   st.codepage = CP_ACP;

#ifdef DO_RTF
   SendMessage(hwndLog,EM_SETTEXTMODE,(WPARAM)TM_RICHTEXT,(LPARAM)0L);
#else
   SendMessage(hwndLog,EM_SETTEXTMODE,(WPARAM)TM_PLAINTEXT,(LPARAM)0L);
#endif

   SendMessage(hwndLog,EM_SETTEXTEX,(WPARAM)&st,(LPARAM)L"");
   SendMessage(hwndLog,EM_EXLIMITTEXT,(WPARAM)0L,(LPARAM)(32768 * 65535));

   memset(&logStream,0,sizeof(EDITSTREAM));

   logStream.dwCookie = (DWORD_PTR)this;
   logStream.pfnCallback = processLog;

   return 0;
   }

#if 0

   long HashCode(char *pszInput) {
   long hashCode = 0L;
   long part = 0L;
   long n = strlen(pszInput);
   char *psz = new char[n + 4];
   memset(psz,0,(n + 4) * sizeof(char));
   strcpy(psz,pszInput);
   char *p = psz;
   for ( long k = 0; k < n; k += 4 ) {
      memcpy(&part,p,4 * sizeof(char));
      hashCode ^= part;
      p += 4;
   }
   delete [] psz;
   return hashCode;
   }

   void ASCIIHexDecodeInPlace(char *pszInput) {

   long n = strlen(pszInput);

   long j = 0;

   for ( long k = 0; k < n; j++, k += 2 ) {

      char c1 = pszInput[k];
      char c2 = pszInput[k + 1];

      BYTE v1 = 0x00;

      if ( '0' <= c1 && '9' >= c1 )
         v1 = c1 - '0';
      else if ( 'A' <= c1 && 'F' >= c1 )
         v1 = c1 - 'A' + 10;
      else if ( 'a' <= c1 && 'f' >= c1 )
         v1 = c1 - 'a' + 10;

      BYTE v2 = 0x00;

      if ( '0' <= c2 && '9' >= c2 )
         v2 = c2 - '0';
      else if ( 'A' <= c2 && 'F' >= c2 )
         v2 = c2 - 'A' + 10;
      else if ( 'a' <= c2 && 'f' >= c2 )
         v2 = c2 - 'a' + 10;

      pszInput[j] = (v1 << 4) + v2;

   }   

   pszInput[j] = '\0';

   return;
   }


   void ASCIIHexEncode(char *pszInput,long valueSize,char **ppszResult) {

   *ppszResult = new char[2 * valueSize + 1];
   memset(*ppszResult,0,(2 * valueSize + 1) * sizeof(char));

   char *p = pszInput;
   char *pEnd = p + valueSize;
   char *pTarget = *ppszResult;

   while ( p < pEnd ) {
  
      *pTarget = (*p & 0xF0) >> 4;
      *pTarget += (*pTarget > 9 ? 'a' - 10 : '0');

      pTarget++;

      *pTarget = (*p & 0x0F);
      *pTarget += (*pTarget > 9 ? 'a' - 10 : '0');
   
      pTarget++;

      p++;

   }

   return;
   }


   int pixelsToHiMetric(SIZEL *pPixels,SIZEL *phiMetric) {
   HDC hdc = GetDC(0);
   int pxlsX,pxlsY;
 
   pxlsX = GetDeviceCaps(hdc,LOGPIXELSX);
   pxlsY = GetDeviceCaps(hdc,LOGPIXELSY);
   ReleaseDC(0,hdc);
 
   phiMetric -> cx = PIXELS_TO_HIMETRIC(pPixels -> cx,pxlsX);
   phiMetric -> cy = PIXELS_TO_HIMETRIC(pPixels -> cy,pxlsY);
   return TRUE;
   }

   int hiMetricToPixel(SIZEL *phiMetric,SIZEL *pPixels) {
   HDC hdc = GetDC(0);
   int pxlsX,pxlsY;

   pxlsX = GetDeviceCaps(hdc,LOGPIXELSX);
   pxlsY = GetDeviceCaps(hdc,LOGPIXELSY);
   ReleaseDC(0,hdc);

   pPixels -> cx = HIMETRIC_TO_PIXELS(phiMetric -> cx,pxlsX);
   pPixels -> cy = HIMETRIC_TO_PIXELS(phiMetric -> cy,pxlsY);

   return TRUE;
   }
#endif

