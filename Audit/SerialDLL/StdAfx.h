// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__1149D29B_DD4C_4D4A_8EFA_098CB5E574BC__INCLUDED_)
#define AFX_STDAFX_H__1149D29B_DD4C_4D4A_8EFA_098CB5E574BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#ifdef __VISUAL_BASIC

#define BOOL_DLL	BOOL _stdcall
#define INT_DLL	int _stdcall
#define VOID_DLL	void _stdcall
#define DOUBLE_DLL	double _stdcall

#define DLL_DECL _stdcall

#else // Not visual basic

typedef const char* StringConst;

#define BOOL_DLL extern "C" BOOL __declspec(dllexport)
#define INT_DLL extern "C" int __declspec(dllexport)
#define VOID_DLL extern "C" void __declspec(dllexport)
#define DOUBLE_DLL extern "C" double __declspec(dllexport)
#define CONST_STRING extern "C" StringConst __declspec(dllexport)

#define DLL_DECL __declspec(dllexport)

#endif


#define dllImport	__declspec( dllimport )
#define dllExport	__declspec( dllexport )


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_STDAFX_H__1149D29B_DD4C_4D4A_8EFA_098CB5E574BC__INCLUDED_)
