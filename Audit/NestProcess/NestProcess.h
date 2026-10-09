// NestProcess.h : main header file for the NESTPROCESS DLL
//

#if !defined(_NESTPROCESS_H)
#define _NESTPROCESS_H

#pragma once

#include "resource.h"		// main symbols

#include "Command.h"
#include "RouteList.h"
#include "ManualData.h"

#include "NestMgr.h"

enum EQuadrant
{
	QUADRANT_INVALID = 0,
	QUADRANT_ONE     = 1,
	QUADRANT_FOUR    = 2
};

class C3dBox;

/////////////////////////////////////////////////////////////////////////////
// CNestProcessApp
// See NestProcess.cpp for the implementation of this class
//

class CNestProcessApp : public CWinApp
{
public:
	CNestProcessApp();

	static CReturn dllExport RegisterProcess( CRouteList* io_route );

	static CReturn dllExport Nest( CCommand* io_cmd );
	static CReturn dllExport NestTrue( CCommand* io_cmd );
	static CReturn dllExport NestPanel( CCommand* io_cmd );
	static CReturn dllExport NestRepo( CCommand* io_cmd );

	static CReturn dllExport RepoMark( CCommand* io_cmd );
	static CReturn dllExport Wrap( CCommand* io_cmd );
	static CReturn dllExport Repo( CCommand* io_cmd );

	static CReturn dllExport PartSeqInit( CCommand* io_cmd );
	static CReturn dllExport PartSeqNext( CCommand* io_cmd );

	static CReturn dllExport Generate( CCommand* io_cmd );

	static CReturn dllExport Chain( CCommand* io_cmd );
	static CReturn dllExport RemnantCommit( CCommand* io_cmd );
	static CReturn dllExport RemnantStats( CCommand* io_cmd );

	static CReturn dllExport Test( CCommand* io_cmd );

	static CReturn dllExport FitTest( CCommand* io_cmd );

	static CReturn dllExport CutBack( CCommand* io_cmd );

	static CReturn dllExport ManualSheet( CCommand* io_cmd );
	static CReturn dllExport ManualPattern( CCommand* io_cmd );
	static CReturn dllExport ManualInstance( CCommand* io_cmd );
	static CReturn dllExport ManualSelection( CCommand* io_cmd );
	static CReturn dllExport ManualFile( CCommand* io_cmd );
	static CReturn dllExport ManualJumpTo( CCommand* io_cmd );
	static CReturn dllExport ManualBump( CCommand* io_cmd );
	static CReturn dllExport ManualPunch( CCommand* io_cmd );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNestProcessApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CNestProcessApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	static CReturn do_load_info(
								CNestConfig& config,
								CModel& model,
								CSheet& sheet,
								CPartBin& partbin,
								const CString& configdb,
								const CString& partdb );

	static CReturn do_simplify_parts(
								CNestConfig& config,
								CSheet& sheet,
								CPartBin& partbin );

	static CReturn do_make_nestable(
								CNestConfig& config,
								CPartBin& partbin,
								CViewMgr& view);

	static CReturn do_filter(
								CNestConfig& config,
								CPartBin& partbin,
								double filter_area,
								CViewMgr& view);

	static CReturn do_ordering(
								CNestConfig& config,
								CPartBin& partbin,
								CViewMgr& view);

	static CReturn do_resolution(
								CNestConfig& config,
								CPartBin& partbin,
								CViewMgr& view);

	static CReturn do_slice(
								CNestConfig& config,
								CPartBin& partbin,
								CViewMgr& view);
#if ORIGINAL_CODE
	static CReturn do_gridlink(
								CNestConfig& config,
								CPartBin& partbin,
								CViewMgr& view);
#else
	static CReturn do_gridlink(
								CNestConfig& config,
								CPartBin& partbin,
								CModel& model,
								CNestMgr& nest,
								CViewMgr& view);
#endif

	static CReturn do_prenest(
								CNestConfig& config,
								CPartBin& partbin,
								CModel& model,
								CNestMgr& nest,
								CViewMgr& view);

	static CReturn repo_explode(CModel& model);

	static CReturn MoveCopy(
								CDbPattern*	dbPattern,
								CDbCommand*	dbInstance,
								CCommand*	io_cmd );

	static int InstanceCount( CDbPattern* dbPattern );

	static CDbPattern* SimilarPatternFind(
								CDbPattern*	dbPattern,
								double		radians );

	static void PolyDataGenerate( CDbEntityArray& entities, CCommand* io_cmd );
	static void PolyDataGenerate( CGeoCurveArray& geoCurves, CCommand* io_cmd );
	
	static void CurvesTabulate(
		const CGeoCurveArray&	geoCurves,
		C3dCoordArray*			pts,
		C2dBox*					box );

	static void GetLinePts(
		const CGeoLine&	geoLine,
		C3dCoordArray*	pts,
		C2dBox*			box );

	static void GetArcPts(
		const CGeoArc&	geoArc,
		C3dCoordArray*	pts,
		C2dBox*			box );

	static void PointsMap(
		const CWnd&		wnd,
		const C2dBox&	box,
		C3dCoordArray*	pts );

	static void ShapeDraw(
		CDC*					dc,
		const C3dCoordArray&	pts );

	// Methods for remnant management.

	static void BucketDelete(
					CModel*			model,
					ID				remnant_layer_id,
					EDbEntityType	type );

	static double RemnantArea( const CModel& model );

	static CReturn RemnantStatistics(
					const CModel&	model,
					C3dBox*			box,
					double*			area );

	static CReturn HoldDownInit( CCommand* io_cmd );

	static CDbCommand* InstanceCreate(
					const CDbPattern&	dbPattern,
					double				ang,
					double				xp,
					double				yp );

	static CReturn BumpInit( CCommand* io_cmd );
	static CReturn BumpAnchor( CCommand* io_cmd );
	static CReturn BumpOrient( CCommand* io_cmd );
	static CReturn BumpDrag( CCommand* io_cmd );
	static CReturn BumpMoveCopy( CCommand* io_cmd );
	static CReturn BumpCancel( CCommand* io_cmd );

	static void ShiftInit( CCommand* io_cmd );
	static double RadiansAccumulate( double delta_radians );
	static double RadiansNormalize( double radians );


private:

	static CRouteList	m_router;
	static CRouteList	m_manual;

	static CManualData	m_mandata;

	// In support of Transform/Bump.
	static C2dCoord	m_handle;
	static C2dCoord	m_anchor;
	static C2dVec	m_vec;
	static double	m_radians;
	static double	m_delta_radians;
	static double	m_shift;  // for 4th quadrant pain
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NESTPROCESS_H__C12E1D25_4A45_11D3_B7F1_000039A6570C__INCLUDED_)
