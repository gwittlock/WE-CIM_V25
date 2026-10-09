// AttribProcess.h : main header file for the ATTRIBPROCESS DLL
//

#if !defined(AFX_ATTRIBPROCESS_H__0A8E0D65_CDB6_11D3_919D_0040335A7818__INCLUDED_)
#define AFX_ATTRIBPROCESS_H__0A8E0D65_CDB6_11D3_919D_0040335A7818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#ifndef _TYPE_H
#include "Type.h"
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "Command.h"
#include "RouteList.h"

class CDbEntity;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


class CAttribProcessApp : public CWinApp
{
public:

	CAttribProcessApp();
	
	static CReturn dllExport RegisterProcess( CRouteList* io_route );

	static CReturn dllExport Attrib( CCommand* io_cmd );

	static CReturn dllExport GetAtt( CCommand* io_cmd );
	static CReturn dllExport SetAtt( CCommand* io_cmd );
	static CReturn dllExport DelAtt( CCommand* io_cmd );
	static CReturn dllExport SetMulti( CCommand* io_cmd );

	static CReturn dllExport SetChainAtt( CCommand* io_cmd );

	static CReturn dllExport UpdateAtt( CCommand* io_cmd );
	static CReturn dllExport UpdateHide( CCommand* io_cmd );
	static CReturn dllExport UpdateShow( CCommand* io_cmd );
	static CReturn dllExport UpdateFeatureAtt( CCommand* io_cmd );
	static CReturn dllExport UpdateLayerAtt( CCommand* io_cmd );
	static CReturn dllExport UpdateDelAtt( CCommand* io_cmd );

	static CReturn dllExport UpdateExternal( CCommand* io_cmd );

	static CReturn dllExport DefaultGetAtt( CCommand* io_cmd );
	static CReturn dllExport DefaultSetAtt( CCommand* io_cmd );
	static CReturn dllExport DefaultDelAtt( CCommand* io_cmd );

	static CReturn dllExport HeaderCountAtt( CCommand* io_cmd );
	static CReturn dllExport HeaderGetAtt( CCommand* io_cmd );
	static CReturn dllExport HeaderSetAtt( CCommand* io_cmd );
	static CReturn dllExport HeaderDelAtt( CCommand* io_cmd );

	static CReturn dllExport HeaderVarsetExtract( CCommand* io_cmd );
	static CReturn dllExport HeaderVarNameGet( CCommand* io_cmd );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//     Introduced for used-defined attribute lists via Java

	static CReturn dllExport List( CCommand* io_cmd );
	static CReturn dllExport AttribListNew( CCommand* io_cmd );
	static CReturn dllExport AttribListDestroy( CCommand* io_cmd );
	static CReturn dllExport AttribListFlush( CCommand* io_cmd );
	static CReturn dllExport AttribListCount( CCommand* io_cmd );
	static CReturn dllExport AttribListName( CCommand* io_cmd );
	static CReturn dllExport AttribListGet( CCommand* io_cmd );
	static CReturn dllExport AttribListSet( CCommand* io_cmd );
	static CReturn dllExport AttribListDel( CCommand* io_cmd );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//		Introduced for the V16 Edit Attributes dialog.

	static CReturn dllExport SelectedCount( CCommand* io_cmd );
	static CReturn dllExport SelectedGet( CCommand* io_cmd );
	static CReturn dllExport SelectedSet( CCommand* io_cmd );
	static CReturn dllExport SelectedDelete( CCommand* io_cmd );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttribProcessApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAttribProcessApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:  // Methods

	static CReturn do_get_att( const CVarList& in_att, const CString& in_name, CCommand* io_cmd );
	static CReturn do_set_att( CVarList* io_att, const CString& in_name, CCommand* io_cmd );
	static CReturn do_del_att( CVarList* io_att, const CString& in_name );
	static CReturn do_feature_att( const CModel& model, CDbEntity* io_ent, ID in_feature );

	static CReturn do_set_chain(CDbEntity* db_ent, int val);

	static CVarList VarsetExtract( const CVarList& header, const CString& className );

private:  // Data

	static CRouteList m_attribRouter;
	static CRouteList m_listRouter;
	static CRouteList m_selectedRouter;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIBPROCESS_H__0A8E0D65_CDB6_11D3_919D_0040335A7818__INCLUDED_)
