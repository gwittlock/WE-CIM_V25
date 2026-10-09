// ModelProcess.h : main header file for the ENTITYPROCESS DLL
//

#if !defined(AFX_MODELPROCESS_H__0A8E0D65_CDB6_11D3_919D_0040335A7818__INCLUDED_)
#define AFX_MODELPROCESS_H__0A8E0D65_CDB6_11D3_919D_0040335A7818__INCLUDED_

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
#include "AutoModDb.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


class CModelProcessApp : public CWinApp
{
public:

	CModelProcessApp();
	
	static CReturn dllExport RegisterProcess( CRouteList* io_route );

	static CReturn dllExport Model( CCommand* io_cmd );
	static CReturn dllExport ModelInit( CCommand* io_cmd );
	static CReturn dllExport ModelActiveTool( CCommand* io_cmd );
	static CReturn dllExport ModelActiveWorkplane( CCommand* io_cmd );
	static CReturn dllExport ModelEntityCount( CCommand* io_cmd );
	static CReturn dllExport ModelGet( CCommand* io_cmd );
	static CReturn dllExport ModelGet2( CCommand* io_cmd );
	static CReturn dllExport ModelToolGet( CCommand* io_cmd );
	static CReturn dllExport ModelNameCheck( CCommand* io_cmd );
	static CReturn dllExport ModelPartExtent( CCommand* io_cmd );
	static CReturn dllExport ModelMachineUpdate( CCommand* io_cmd );
	static CReturn dllExport ModelToolsUpdate( CCommand* io_cmd );
	static CReturn dllExport ModelStockUpdate( CCommand* io_cmd );
	static CReturn dllExport ModelIsRightHanded( CCommand* io_cmd );

	static CReturn dllExport ModelVarsLoad( CCommand* io_cmd );

	static CReturn dllExport ModelCTGRead( CCommand* io_cmd );
	static CReturn dllExport ModelCTGSave( CCommand* io_cmd );

	static CReturn dllExport ModelPurge( CCommand* io_cmd );

	static CReturn dllExport LayerMapApply( CCommand* io_cmd );

	static CReturn dllExport StatisticsGet( CCommand* io_cmd );

	static CReturn dllExport WorkZonesWrite( CCommand* io_cmd );
	static CReturn dllExport ClampsWrite( CCommand* io_cmd );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelProcessApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CModelProcessApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:  // Methods

	static void HoldDownsUpdate( const CModel& model );

private:

	static void DropDoorUpdate( const CAutoModDb& autoModDb, CModel* model );
	static void DropDoorValueRecord( const CString& name, double dval );
	static void DropDoorRecord( CModel* model );

private:  // Data

	static CRouteList m_modelRouter;

	// Max length & width of dropdoor.
	static double	m_dropdoor[2];
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELPROCESS_H__0A8E0D65_CDB6_11D3_919D_0040335A7818__INCLUDED_)
