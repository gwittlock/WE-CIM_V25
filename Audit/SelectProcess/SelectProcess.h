// SelectProcess.h : main header file for the SELECTPROCESS DLL
//

#if !defined(AFX_SELECTPROCESS_H__956AF5C6_2A0E_11D3_919D_0040335A7818__INCLUDED_)
#define AFX_SELECTPROCESS_H__956AF5C6_2A0E_11D3_919D_0040335A7818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "Command.h"
#include "RouteList.h"

/////////////////////////////////////////////////////////////////////////////
// CSelectProcessApp
// See SelectProcess.cpp for the implementation of this class
//

class CSelectProcessApp : public CWinApp
{
public:

	CSelectProcessApp();

	static CReturn dllExport RegisterProcess( CRouteList* io_route );

	static CReturn dllExport Selector( CCommand* io_cmd );

	static CReturn dllExport IsSelected( CCommand* io_cmd );
	static CReturn dllExport IsSelectable( CCommand* io_cmd );

	static CReturn dllExport Count( CCommand* io_cmd );
	static CReturn dllExport Get( CCommand* io_cmd );

	static CReturn dllExport Select( CCommand* io_cmd );
	static CReturn dllExport Unselect( CCommand* io_cmd );

	static CReturn dllExport Empty( CCommand* io_cmd );

	static CReturn dllExport Add( CCommand* io_cmd );
	static CReturn dllExport Remove( CCommand* io_cmd );

	static CReturn dllExport SelectAllRefsTo( CCommand* io_cmd );

	static CReturn dllExport SelectBox( CCommand* io_cmd );
	static CReturn dllExport SelectAll( CCommand* io_cmd );

	static CReturn dllExport SelectionsClear( CCommand* io_cmd );
	static CReturn dllExport SelectionFilter( CCommand* io_cmd );

	static CReturn dllExport Restrictions( CCommand* io_cmd );
	static CReturn dllExport System( CCommand* io_cmd );

	static CReturn dllExport SelectorPush( CCommand* io_cmd );
	static CReturn dllExport SelectorPop( CCommand* io_cmd );

	static CReturn SelectorSystemFlag( CCommand* io_cmd );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectProcessApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSelectProcessApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	static CRouteList m_selectorRouter;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTPROCESS_H__956AF5C6_2A0E_11D3_919D_0040335A7818__INCLUDED_)
