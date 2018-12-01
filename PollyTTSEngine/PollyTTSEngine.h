

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for PollyTTSEngine.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
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


#ifndef __PollyTTSEngine_h__
#define __PollyTTSEngine_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __PollyTTSEngine_FWD_DEFINED__
#define __PollyTTSEngine_FWD_DEFINED__

#ifdef __cplusplus
typedef class PollyTTSEngine PollyTTSEngine;
#else
typedef struct PollyTTSEngine PollyTTSEngine;
#endif /* __cplusplus */

#endif 	/* __PollyTTSEngine_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "sapiddk.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_PollyTTSEngine_0000_0000 */
/* [local] */ 

typedef struct VOICEITEM
    {
    LPCWSTR pText;
    ULONG ulTextLen;
    ULONG ulNumAudioBytes;
    BYTE *pAudio;
    } 	VOICEITEM;



extern RPC_IF_HANDLE __MIDL_itf_PollyTTSEngine_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_PollyTTSEngine_0000_0000_v0_0_s_ifspec;


#ifndef __POLLYTTSENGLib_LIBRARY_DEFINED__
#define __POLLYTTSENGLib_LIBRARY_DEFINED__

/* library POLLYTTSENGLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_POLLYTTSENGLib;

EXTERN_C const CLSID CLSID_PollyTTSEngine;

#ifdef __cplusplus

class DECLSPEC_UUID("698312A0-4B71-46AE-867F-FF45D9835EE9")
PollyTTSEngine;
#endif
#endif /* __POLLYTTSENGLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


