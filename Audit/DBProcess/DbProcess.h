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


class CDbProcessApp : public CWinApp
{
public:

	CDbProcessApp();
	
	static CReturn dllExport RegisterProcess( CRouteList* io_route );

	static CReturn dllExport Db( CCommand* io_cmd );

	static CReturn dllExport DbCreate( CCommand* io_cmd );
	static CReturn dllExport DbDestroy( CCommand* io_cmd );
	static CReturn dllExport DbOpen( CCommand* io_cmd );
	static CReturn dllExport DbClose( CCommand* io_cmd );
	static CReturn dllExport DbTable( CCommand* io_cmd );
	static CReturn dllExport DbCount( CCommand* io_cmd );
	static CReturn dllExport DbMoveAbs( CCommand* io_cmd );
	static CReturn dllExport DbMoveIncr( CCommand* io_cmd );
	static CReturn dllExport DbMove( CCommand* io_cmd );
	static CReturn dllExport DbIntGet( CCommand* io_cmd );
	static CReturn dllExport DbDblGet( CCommand* io_cmd );
	static CReturn dllExport DbStrGet( CCommand* io_cmd );

	static CReturn dllExport QueryCreate( CCommand* io_cmd );
	static CReturn dllExport QueryDestroy( CCommand* io_cmd );
	static CReturn dllExport QueryExecute( CCommand* io_cmd );
	static CReturn dllExport QueryCount( CCommand* io_cmd );
	static CReturn dllExport QueryMoveIncr( CCommand* io_cmd );
	static CReturn dllExport QueryIntGet( CCommand* io_cmd );
	static CReturn dllExport QueryDblGet( CCommand* io_cmd );
	static CReturn dllExport QueryStrGet( CCommand* io_cmd );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDbProcessApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDbProcessApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:  // Methods

	static CDaoDB* DatabaseGet( int id );
	static CDaoQuery* QueryGet( int id );

private:  // Data

	static CRouteList m_dbRouter;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENTITYPROCESS_H__0A8E0D65_CDB6_11D3_919D_0040335A7818__INCLUDED_)
