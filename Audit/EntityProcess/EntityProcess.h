// EntityProcess.h : main header file for the ENTITYPROCESS DLL
//

#if !defined(AFX_ENTITYPROCESS_H__0A8E0D65_CDB6_11D3_919D_0040335A7818__INCLUDED_)
#define AFX_ENTITYPROCESS_H__0A8E0D65_CDB6_11D3_919D_0040335A7818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "Command.h"
#include "RouteList.h"

class CDbEntity;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


class CEntityProcessApp : public CWinApp
{
public:

	CEntityProcessApp();
	
	static CReturn dllExport RegisterProcess( CRouteList* io_route );

	static CReturn dllExport Entity( CCommand* io_cmd );
	static CReturn dllExport EntityType( CCommand* io_cmd );
	static CReturn dllExport EntityVBType( CCommand* io_cmd );
	static CReturn dllExport EntityName( CCommand* io_cmd );
	static CReturn dllExport EntityWork( CCommand* io_cmd );
	static CReturn dllExport EntityTool( CCommand* io_cmd );
	static CReturn dllExport EntityRefCnt( CCommand* io_cmd );
	static CReturn dllExport EntityBox( CCommand* io_cmd );

	static CReturn dllExport EntityIsLead( CCommand* io_cmd );

	static CReturn dllExport ViewBehavior( CCommand* io_cmd );

	static CReturn dllExport Hide( CCommand* io_cmd );
	static CReturn dllExport Show( CCommand* io_cmd );
	static CReturn dllExport IsHidden( CCommand* io_cmd );
	static CReturn dllExport System( CCommand* io_cmd );
	static CReturn dllExport User( CCommand* io_cmd );

	static CReturn dllExport RefsToInit( CCommand* io_cmd );
	static CReturn dllExport RefsToCount( CCommand* io_cmd );
	static CReturn dllExport RefsToGet( CCommand* io_cmd );
	static CReturn dllExport RefsToTerm( CCommand* io_cmd );

	static CReturn dllExport Container( CCommand* io_cmd );
	static CReturn dllExport ContainerCount( CCommand* io_cmd );
	static CReturn dllExport ContainerGet( CCommand* io_cmd );
	static CReturn dllExport ContainerSet( CCommand* io_cmd );
	static CReturn dllExport ContainerAppend( CCommand* io_cmd );
	static CReturn dllExport ContainerInsertBefore( CCommand* io_cmd );
	static CReturn dllExport ContainerInsertAfter( CCommand* io_cmd );
	static CReturn dllExport ContainerDisown( CCommand* io_cmd );
	static CReturn dllExport ContainerBenignFlush( CCommand* io_cmd );
	static CReturn dllExport ContainerDestructiveFlush( CCommand* io_cmd );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEntityProcessApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CEntityProcessApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:  // Methods

	static CDbEntity* EntityGet( CCommand* io_cmd );
	static bool IsLead( const CDbEntity& dbEntity, int type );

private:  // Data

	static CRouteList m_entityRouter;
	static CRouteList m_containerRouter;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENTITYPROCESS_H__0A8E0D65_CDB6_11D3_919D_0040335A7818__INCLUDED_)
