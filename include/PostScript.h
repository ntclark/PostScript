
#pragma once

#pragma warning(disable:4355)
#pragma warning(disable:4996)

#include <windows.h>
#include <olectl.h>
#include <ShlObj.h>
#include <stdio.h>
#include <float.h>
#include <RichEdit.h>
#include <process.h>

#include <map>
#include <list>
#include <queue>

#include "utilities.h"

#include "PdfEnabler_i.h"
#include "pdfEnabler\Page.h"
#include "PostScript_i.h"

#undef ADVANCE_THRU_EOL

#include "resourceIdentifiers.h"

#include "job.h"

//#define DO_RTF
#define PS_VERSION   3000L
#define PRODUCT_NAME "InnoVisioNate PostScript interpreter"

#define DRAW_TEXT       0

#define DRAW_RECTANGLES 1

#define DRAW_GRAPHICS   0

// Implementation rules:
//
// When parsing - the parsing procedure will be called with the first byte following the delimiter, which could be white space
//
   class PStoPDF : public IPostScript {

   public:

      PStoPDF(IUnknown *pUnknownOuter);
      ~PStoPDF();

      long queueLog(char *pszOutput,char *pEnd = NULL,bool isError = false);
      long clearLog();

      long displayStack();

      //   IUnknown

      STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
      STDMETHOD_ (ULONG, AddRef)();
      STDMETHOD_ (ULONG, Release)();

      // IPStoPDF

      STDMETHOD(Parse)(wchar_t *pszFileName);
      STDMETHOD(Convert)(wchar_t *pszFileName);
   
      STDMETHOD(ParseText)(char *pszPDFText,long length,void *pvIPdfPage,void *pvIPostScriptTakeText,HDC hdc,RECT *prcWindowsClip);

      STDMETHOD(GetLastError)(char **ppszError);

      // IOleObject 

      class _IOleObject : public IOleObject {

      public:

         _IOleObject(PStoPDF *pp) : pParent(pp) {};
         ~_IOleObject() {};

         STDMETHOD(QueryInterface)(REFIID riid,void **ppv);
         STDMETHOD_ (ULONG, AddRef)();
         STDMETHOD_ (ULONG, Release)();

      private:

         STDMETHOD(SetClientSite)(IOleClientSite *pClientSite);
         STDMETHOD(GetClientSite)(IOleClientSite **ppCLientSite);
         STDMETHOD(SetHostNames)(LPCOLESTR szContainerApp,LPCOLESTR szContainerObj);
         STDMETHOD(Close)(DWORD dwSaveOption);
         STDMETHOD(SetMoniker)(DWORD dwWhichMoniker, IMoniker *pmk);
         STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker,IMoniker **ppmk);
         STDMETHOD(InitFromData)(IDataObject *pDataObject,BOOL fCreation,DWORD dwReserved);
         STDMETHOD(GetClipboardData)(DWORD dwReserved,IDataObject **ppDataObject);
         STDMETHOD(DoVerb)(LONG iVerb,LPMSG lpmsg,IOleClientSite *pActiveSite,LONG lindex,HWND hwndParent,LPCRECT lprcPosRect);
         STDMETHOD(EnumVerbs)(IEnumOLEVERB **ppenumOleVerb);
         STDMETHOD(Update)();
         STDMETHOD(IsUpToDate)();
         STDMETHOD(GetUserClassID)(CLSID * pClsid);
         STDMETHOD(GetUserType)(DWORD dwFormOfType, LPOLESTR *pszUserType);
         STDMETHOD(SetExtent)(DWORD dwDrawAspect, LPSIZEL lpsizel);
         STDMETHOD(GetExtent)(DWORD dwDrawAspect, LPSIZEL lpsizel);
        
         STDMETHOD(Advise)(IAdviseSink *pAdvSink, DWORD * pdwConnection);
         STDMETHOD(Unadvise)(DWORD dwConnection);
         STDMETHOD(EnumAdvise)(IEnumSTATDATA **ppenumAdvise);
         STDMETHOD(GetMiscStatus)(DWORD dwAspect, DWORD * pdwStatus);        
        
         STDMETHOD(SetColorScheme)(LPLOGPALETTE lpLogpal);
   
         PStoPDF *pParent;

      } *pIOleObject;

      // IOleInPlaceObject

      class _IOleInPlaceObject : public IOleInPlaceObject {

      public:

         _IOleInPlaceObject(PStoPDF *pp) : pParent(pp) {};
         ~_IOleInPlaceObject() {};

         STDMETHOD(QueryInterface)(REFIID riid,void **ppv);
         STDMETHOD_ (ULONG, AddRef)();
         STDMETHOD_ (ULONG, Release)();

         // IOleWindow
   
         STDMETHOD (GetWindow)(HWND *);
         STDMETHOD (ContextSensitiveHelp)(BOOL);

         // IOleInPlaceObject

         STDMETHOD (InPlaceActivate)();
         STDMETHOD (InPlaceDeactivate)();
         STDMETHOD (UIDeactivate)();
         STDMETHOD (SetObjectRects)(LPCRECT,LPCRECT);
         STDMETHOD (ReactivateAndUndo)();

      private:

         PStoPDF *pParent;

      } *pIOleInPlaceObject;

      //      IConnectionPointContainer

      struct _IConnectionPointContainer : public IConnectionPointContainer {
      public:

         _IConnectionPointContainer(PStoPDF *pp) : pParent(pp) {};
         ~_IConnectionPointContainer() {};

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
        STDMETHOD_ (ULONG, AddRef)();
        STDMETHOD_ (ULONG, Release)();

        STDMETHOD(FindConnectionPoint)(REFIID riid,IConnectionPoint **);
        STDMETHOD(EnumConnectionPoints)(IEnumConnectionPoints **);

//        void fire_DataEvent(long status);

      private:

		  PStoPDF *pParent;

     } *pIConnectionPointContainer;

     struct _IConnectionPoint : IConnectionPoint {
	  public:

        _IConnectionPoint(PStoPDF *pp);
		  ~_IConnectionPoint();

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
        STDMETHOD_ (ULONG, AddRef)();
        STDMETHOD_ (ULONG, Release)();

        STDMETHOD (GetConnectionInterface)(IID *);
        STDMETHOD (GetConnectionPointContainer)(IConnectionPointContainer **ppCPC);
        STDMETHOD (Advise)(IUnknown *pUnk,DWORD *pdwCookie);
   	  STDMETHOD (Unadvise)(DWORD);
        STDMETHOD (EnumConnections)(IEnumConnections **ppEnum);

		  IUnknown *AdviseSink() { return adviseSink; };

     private:

        int getSlot();
        int findSlot(DWORD dwCookie);

		  IUnknown *adviseSink;
		  PStoPDF *pParent;
        DWORD nextCookie;
		  int countConnections,countLiveConnections;

        CONNECTDATA *connections;

     } *pIConnectionPoint;


	  struct _IEnumConnectionPoints : IEnumConnectionPoints {
	  public:

	     _IEnumConnectionPoints(PStoPDF *pp,_IConnectionPoint **cp,int connectionPointCount);
       ~_IEnumConnectionPoints();

        STDMETHOD (QueryInterface)(REFIID riid,void **ppv);
        STDMETHOD_ (ULONG, AddRef)();
        STDMETHOD_ (ULONG, Release)();

 	     STDMETHOD (Next)(ULONG cConnections,IConnectionPoint **rgpcn,ULONG *pcFetched);
        STDMETHOD (Skip)(ULONG cConnections);
        STDMETHOD (Reset)();
        STDMETHOD (Clone)(IEnumConnectionPoints **);

     private:

        int cpCount,enumeratorIndex;
		  PStoPDF *pParent;
		  _IConnectionPoint **connectionPoints;

     } *pIEnumConnectionPoints;

     struct _IEnumerateConnections : public IEnumConnections {

     public:

        _IEnumerateConnections(IUnknown* pParentUnknown,ULONG cConnections,CONNECTDATA* paConnections,ULONG initialIndex);
        ~_IEnumerateConnections();

         STDMETHODIMP         QueryInterface(REFIID, void **);
         STDMETHODIMP_(ULONG) AddRef();
         STDMETHODIMP_(ULONG) Release();
         STDMETHODIMP         Next(ULONG, CONNECTDATA*, ULONG*);
         STDMETHODIMP         Skip(ULONG);
         STDMETHODIMP         Reset();
         STDMETHODIMP         Clone(IEnumConnections**);

      private:

        ULONG refCount;
        IUnknown *pParentUnknown;
        ULONG enumeratorIndex;
        ULONG countConnections;
        CONNECTDATA *connections;

      } *pIEnumerateConnections;

      static CRITICAL_SECTION theQueueCriticalSection;

   private:

      int initWindows();

      IOleClientSite *pIOleClientSite;
      IOleInPlaceSite *pIOleInPlaceSite;

      HWND hwndHost;
      HWND hwndClient;
      HWND hwndLog;
      HWND hwndStack;

      job *pJob;

      long refCount;

      static char szErrorMessage[1024];

      static bool logPaintPending;
      static HANDLE hRichEditSemaphore;
      static WNDPROC nativeRichEditHandler;
      static LRESULT CALLBACK richEditHandler(HWND,UINT msg,WPARAM wParam,LPARAM lParam);

      static LRESULT CALLBACK handler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
      static DWORD __stdcall processLog(DWORD_PTR dwCookie,BYTE *pBuffer,LONG bufferSize,LONG *pBytesReturned);

   };

#define ADVANCE_THRU_EOL(p)               \
{                                         \
while ( *p != 0x0A && *p != 0x0D )        \
   p++;                                   \
if ( *p == 0x0A || *p == 0x0D )           \
   p++;                                   \
if ( *p == 0x0A || *p == 0x0D )           \
   p++;                                   \
}


#define ADVANCE_THRU_WHITE_SPACE(p) \
{                                   \
while ( *p && strchr(pszWhiteSpace,*p) ) { \
   p++;  \
}        \
}

#define ADVANCE_TO_WHITE_SPACE(p)         \
{                                         \
while ( *p && ! strchr(pszWhiteSpace,*p) )\
   p++;                                   \
}

#define ADVANCE_TO_END(p)                    \
{                                            \
while ( *p && ! strchr(pszWhiteSpace,*p) ) { \
   char *pz = (char *)p;                     \
   for ( long k = 0; 1; k++ ) {              \
      if ( ! psCollectionDelimiters[k][0] )  \
         break;                              \
      if ( 0 == strncmp(pz,psCollectionDelimiters[k],psCollectionDelimiterLength[k]) ) {   \
         pz = NULL; \
         break;     \
      }             \
   }                \
   if ( ! pz )      \
      break;        \
   pz = strchr(pszDelimiters,*p);  \
   if ( pz ) {                     \
      for ( long k = 0; 1; k++ ) { \
         if ( ! psDelimiters[k][0] ) \
            break;                   \
         if ( 0 == strncmp(pz,psDelimiters[k],psDelimiterLength[k]) ) { \
            pz = NULL;                                                  \
            break;                                                      \
         }          \
      }             \
      if ( ! pz )   \
         break;     \
   }                \
   p++;             \
}                   \
}

#define COMMENT_DELIMITER           "%"
#define DSC_DELIMITER               "%%"
#define LITERAL_DELIMITER           "/"
#define PROC_DELIMITER_BEGIN        "{"
#define PROC_DELIMITER_END          "}"
#define STRING_DELIMITER_BEGIN      "("
#define STRING_DELIMITER_END        ")"
#define HEX_STRING_DELIMITER_BEGIN  "<"
#define HEX_STRING_DELIMITER_END    ">"
#define HEX85_DELIMITER_BEGIN       "<~"
#define HEX85_DELIMITER_END         "~>"
#define DICTIONARY_DELIMITER_BEGIN  "<<"
#define DICTIONARY_DELIMITER_END    ">>"
#define ARRAY_DELIMITER_BEGIN       "["
#define ARRAY_DELIMITER_END         "]"

#define PIXELS_TO_HIMETRIC(x,ppli)  ( (2540*(x) + ((ppli) >> 1)) / (ppli) )
#define HIMETRIC_TO_PIXELS(x,ppli)  ( ((ppli)*(x) + 1270) / 2540 )

#define EOL "\x0A\x0D"

#define SYNCHRONOUS_CALL(theWindow,MSG,ARGUMENT) {                      \
   HANDLE hSemaphore = CreateSemaphore(NULL,0,1,NULL);                  \
   PostMessage((theWindow),(MSG),(WPARAM)(ARGUMENT),(LPARAM)hSemaphore);\
   WaitForSingleObject(hSemaphore,INFINITE);                            \
   CloseHandle(hSemaphore);                                             \
   }

#ifdef DEFINE_DATA

   PStoPDF *pPStoPDF = NULL;

   HMODULE hModule = NULL;
   char szModuleName[MAX_PATH];
   char szApplicationDataDirectory[MAX_PATH];
   char szUserDirectory[MAX_PATH];

   EDITSTREAM logStream;

   char psCollectionDelimiters[][8] = { "[","]","<<",">>","{","}",0};
   long psCollectionDelimiterLength[] = { 1,  1,   2,   2,  1,  1,0};

   char psDelimiters[][8] =   {"%","%%","(","/","<","<~",0};
   long psDelimiterLength[] = {  1,   2,  1,  1,  1,   2,0};

   char psOtherDelimiters[][8] = {"]",0};
   char pszDelimiters[] = {"%%%(/<~{"};
   char pszWhiteSpace[] = " \t\x0A\x0D\x0C";

#else

   extern PStoPDF *pPStoPDF;

   extern HMODULE hModule;
   extern char szModuleName[];
   extern char szApplicationDataDirectory[];
   extern char szUserDirectory[];

   extern EDITSTREAM logStream;

   extern char psCollectionDelimiters[][8];
   extern long psCollectionDelimiterLength[];
   extern char psDelimiters[][8];
   extern long psDelimiterLength[];
   extern char psOtherDelimiters[][8];
   extern char pszDelimiters[];
   extern char pszWhiteSpace[];

#endif
