
#if !defined(AFX_SEQUENCEPROCESS_H__F0A0A786_4066_11D5_919D_0040335A7818__INCLUDED_)
#define AFX_SEQUENCEPROCESS_H__F0A0A786_4066_11D5_919D_0040335A7818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "Command.h"
#include "RouteList.h"

class CWorkPkg;


// ==================================================================

/////////////////////////////////////////////////////////////////////////////
// CSequenceProcessApp
// See SequenceProcess.cpp for the implementation of this class
//

class CSequenceProcessApp : public CWinApp
{
public:

	CSequenceProcessApp();

	static CReturn dllExport RegisterProcess( CRouteList* io_route );

	static CReturn dllExport Seq( CCommand* io_cmd );
	static CReturn dllExport Init( CCommand* io_cmd );
	static CReturn dllExport Wrap( CCommand* io_cmd );
	static CReturn dllExport Explode( CCommand* io_cmd );
	static CReturn dllExport Move( CCommand* io_cmd );
	static CReturn dllExport Optimize( CCommand* io_cmd );

	static CReturn dllExport InsertPosition( CCommand* io_cmd );

	static CReturn dllExport Iter_init( CCommand* io_cmd );
	static CReturn dllExport Iter_next( CCommand* io_cmd );
	static CReturn dllExport Iter_get( CCommand* io_cmd );

	static CReturn dllExport Create( CCommand* io_cmd );
	static CReturn dllExport Count( CCommand* io_cmd );
	static CReturn dllExport Get( CCommand* io_cmd );
	static CReturn dllExport Append( CCommand* io_cmd );
	static CReturn dllExport BenignFlush( CCommand* io_cmd );

	// For debugging.
	static CReturn dllExport Dump( CCommand* io_cmd );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSequenceProcessApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSequenceProcessApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	static CDbEntity* EntityFind( const CModel& model, ID id );
	static void CleanUp( CModel* model );
	static CReturn Explode( const CModel& model, ID id );

	static CReturn Flatten( const CSelector& selector, CWorkPkg* workPkg );

	static void Dump( const CModel& model );

private:

	static CRouteList m_seqRouter;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEQUENCEPROCESS_H__F0A0A786_4066_11D5_919D_0040335A7818__INCLUDED_)
