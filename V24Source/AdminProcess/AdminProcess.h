// AdminProcess.h : main header file for the ADMINPROCESS DLL
//

#if !defined(AFX_ADMINPROCESS_H__EE20D1F1_2251_11D3_B7F1_000039A6570C__INCLUDED_)
#define AFX_ADMINPROCESS_H__EE20D1F1_2251_11D3_B7F1_000039A6570C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "Command.h"
#include "RouteList.h"

class CDbFeature;


/////////////////////////////////////////////////////////////////////////////
// CAdminProcessApp
// See AdminProcess.cpp for the implementation of this class
//

class CAdminProcessApp : public CWinApp
{
public:
	CAdminProcessApp();

	static CReturn dllExport RegisterProcess( CRouteList* io_route );

	static CReturn dllExport Admin( CCommand* io_cmd );

	static CReturn dllExport New( CCommand* io_cmd );
	static CReturn dllExport System( CCommand* io_cmd );
	static CReturn dllExport User( CCommand* io_cmd );
	static CReturn dllExport Prepare( CCommand* io_cmd );
	static CReturn dllExport Commit( CCommand* io_cmd );
	static CReturn dllExport Cancel( CCommand* io_cmd );
	static CReturn dllExport Undo( CCommand* io_cmd );
	static CReturn dllExport Redo( CCommand* io_cmd );

	// Introduced to support RTLs.
	static CReturn dllExport Quit( CCommand* io_cmd );

	static CReturn dllExport GlobalModel( CCommand* io_cmd );

	static CReturn dllExport IsRegenPending( CCommand* io_cmd );
	static CReturn dllExport Regen( CCommand* io_cmd );

	static CReturn dllExport MacroCompile( CCommand* io_cmd );
	static CReturn dllExport MacroRun( CCommand* io_cmd );

	static CReturn dllExport TreeInit( CCommand* io_cmd );
	static CReturn dllExport TreeCount( CCommand* io_cmd );
	static CReturn dllExport TreeGet( CCommand* io_cmd );

	static CReturn dllExport Lookup( CCommand* io_cmd );

	static CReturn dllExport JavaVarsExtract( CCommand* io_cmd );
	static CReturn dllExport JavaVarGet( CCommand* io_cmd );

	static CReturn dllExport RegGet( CCommand* io_cmd );
	static CReturn dllExport RegPut( CCommand* io_cmd );

	static CReturn dllExport Config( CCommand* io_cmd );
	static CReturn dllExport Errors( CCommand* io_cmd );
	static CReturn dllExport EWM( CCommand* io_cmd );

	// RTL commands
	static CReturn dllExport RTL( CCommand* io_cmd );
	static CReturn dllExport RtlDir( CCommand* io_cmd );

	// VarSys commands
	static CReturn dllExport VarSys( CCommand* io_cmd );
	static CReturn dllExport VarSysCount( CCommand* io_cmd );
	static CReturn dllExport VarSysFlush( CCommand* io_cmd );
	static CReturn dllExport VarSysName( CCommand* io_cmd );
	static CReturn dllExport VarSysDel( CCommand* io_cmd );
	static CReturn dllExport VarSysStrGet( CCommand* io_cmd );
	static CReturn dllExport VarSysStrPut( CCommand* io_cmd );

	// Sequencing commands
	static CReturn dllExport Sequence( CCommand* io_cmd );
	static CReturn dllExport SequenceSet( CCommand* io_cmd );
	static CReturn dllExport SequenceGet( CCommand* io_cmd );
	static CReturn dllExport SequenceNext( CCommand* io_cmd );
	static CReturn dllExport SequenceReset( CCommand* io_cmd );

	static CReturn dllExport TimersDump( CCommand* io_cmd );
	static CReturn dllExport TimerCreate( CCommand* io_cmd );
	static CReturn dllExport TimerStart( CCommand* io_cmd );
	static CReturn dllExport TimerStop( CCommand* io_cmd );
	static void pDump( void );
	static int Profile( const CString&	name );
	static void In( int idx );
	static void Out( int idx );

private:	// data

	static CRouteList m_adminRouter;
	static CRouteList m_seqRouter;
	static CRouteList m_rtlRouter;
	static CRouteList m_varsysRouter;
	static CRouteList m_timerRouter;

private:

	static CString CommandAssemble( const CDbFeature& dbFeature );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdminProcessApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CAdminProcessApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADMINPROCESS_H__EE20D1F1_2251_11D3_B7F1_000039A6570C__INCLUDED_)
