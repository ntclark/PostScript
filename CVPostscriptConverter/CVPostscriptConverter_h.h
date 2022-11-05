

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for COM Implementation\CVPostscriptConverter.odl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0628 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __CVPostscriptConverter_h_h__
#define __CVPostscriptConverter_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

#ifndef __ICVPostscriptConverter_FWD_DEFINED__
#define __ICVPostscriptConverter_FWD_DEFINED__
typedef interface ICVPostscriptConverter ICVPostscriptConverter;

#endif 	/* __ICVPostscriptConverter_FWD_DEFINED__ */


#ifndef __CVPostscriptConverter_FWD_DEFINED__
#define __CVPostscriptConverter_FWD_DEFINED__

#ifdef __cplusplus
typedef class CVPostscriptConverter CVPostscriptConverter;
#else
typedef struct CVPostscriptConverter CVPostscriptConverter;
#endif /* __cplusplus */

#endif 	/* __CVPostscriptConverter_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __CVPostscriptConverter_LIBRARY_DEFINED__
#define __CVPostscriptConverter_LIBRARY_DEFINED__

/* library CVPostscriptConverter */
/* [version][control][lcid][helpstring][uuid] */ 


EXTERN_C const IID LIBID_CVPostscriptConverter;

#ifndef __ICVPostscriptConverter_INTERFACE_DEFINED__
#define __ICVPostscriptConverter_INTERFACE_DEFINED__

/* interface ICVPostscriptConverter */
/* [object][helpstring][uuid] */ 


EXTERN_C const IID IID_ICVPostscriptConverter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D734A734-368C-4C8D-ACF2-E5CCFCE1C7E1")
    ICVPostscriptConverter : public IUnknown
    {
    public:
        virtual BOOL STDMETHODCALLTYPE ConvertToPDF( 
            unsigned char *pszPostscriptFilename) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ICVPostscriptConverterVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICVPostscriptConverter * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICVPostscriptConverter * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICVPostscriptConverter * This);
        
        DECLSPEC_XFGVIRT(ICVPostscriptConverter, ConvertToPDF)
        BOOL ( STDMETHODCALLTYPE *ConvertToPDF )( 
            ICVPostscriptConverter * This,
            unsigned char *pszPostscriptFilename);
        
        END_INTERFACE
    } ICVPostscriptConverterVtbl;

    interface ICVPostscriptConverter
    {
        CONST_VTBL struct ICVPostscriptConverterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICVPostscriptConverter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICVPostscriptConverter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICVPostscriptConverter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICVPostscriptConverter_ConvertToPDF(This,pszPostscriptFilename)	\
    ( (This)->lpVtbl -> ConvertToPDF(This,pszPostscriptFilename) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICVPostscriptConverter_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_CVPostscriptConverter;

#ifdef __cplusplus

class DECLSPEC_UUID("95FDBDF1-67D5-4DF6-AC84-8D5B080DF21A")
CVPostscriptConverter;
#endif
#endif /* __CVPostscriptConverter_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


