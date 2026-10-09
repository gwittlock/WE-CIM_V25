// Civd.h : main header file for the CADPROCESS DLL
//

#if !defined(AFX_CADPROCESS_H__4C1A3C57_0B58_4A27_8AA8_6F239DFA64B2__INCLUDED_)
#define AFX_CADPROCESS_H__4C1A3C57_0B58_4A27_8AA8_6F239DFA64B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "VarList.h"
#include "CiType.h"
#include "Civd_i.h"

#include "vddocument.h"
#include "vdcommand.h"

#include "vdFigure.h"
//#include "vdEntityList.h"
#include "vdEntities.h"

// ie. is valid vdraw object
#define IsVdObject( obj ) ((obj).m_lpDispatch != NULL)


/////////////////////////////////////////////////////////////////////////////
// CCivd
// See CadProcess.cpp for the implementation of this class
//

class CCivd : public CWinApp
{
public:

	CCivd();

	static dllExport BOOL Init(
							LPDISPATCH	docObject,
							LPDISPATCH	cmdObject,
							eCiHookup	which_obj );

	static dllExport BOOL Terminate();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Low-level VDraw object access.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	static dllExport CvdEntities GetEntities( eCiHookup which_obj );

	static dllExport CvdLayers GetLayers( eCiHookup which_obj );

	static dllExport CvdSelections GetSelections( eCiHookup which_obj );

	static dllExport void SetActiveLayer( eCiHookup which_obj, const CString& name );

	static dllExport CvdFigure GetFromHandle( eCiHookup which_obj, const CString& handle );

	// Obtain an entity from the VDraw document.
	static dllExport CvdFigure* EntityGet( eCiHookup which_obj, const CString& handle );

	// Convert a VDraw type to an enumerated type.
	static dllExport eCiType CiType( const CString& vdrawType );

	static dllExport VARIANT XYZToVariant( double x, double y, double z );

	static dllExport BOOL VariantToXYZ( const VARIANT FAR& variant, double* x, double* y, double* z );

	static dllExport void XpropRemove( CvdPrimary& prim, const CString& name );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Very low-level VDraw object access.
	// NOTE: It's probably best not to use these methods.  This
	// became apparent during the upgrade process from v3.6.7.7
	// to v4.0.2
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Obtain the VDraw document.
	static dllExport CvdDocument& DocumentGet( eCiHookup which_obj );

	// Obtain the VDraw command object.
	static dllExport CvdCommand& CommandGet( eCiHookup which_obj );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCivd)
		virtual BOOL InitInstance();
		virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCivd)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	BOOL InitATL();

private:

#if ORIGINAL_CODE
	// Main VD control
	static CvdDocument* m_doc;
	static CvdCommand* m_cmd;

	// Auxillary VD control.
	static CvdDocument* m_aux_doc;
	static CvdCommand* m_aux_cmd;
#else
	static CvdDocument*	m_doc[3];
	static CvdCommand*	m_cmd[3];
#endif

	static CVarList	m_types;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CADPROCESS_H__4C1A3C57_0B58_4A27_8AA8_6F239DFA64B2__INCLUDED_)
