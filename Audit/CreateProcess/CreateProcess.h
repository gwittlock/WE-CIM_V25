// CreateProcess.h : main header file for the CREATEPROCESS DLL
//

#if !defined(AFX_CREATEPROCESS_H__00D4DDA6_0F5A_11D3_B7F1_000039A6570C__INCLUDED_)
#define AFX_CREATEPROCESS_H__00D4DDA6_0F5A_11D3_B7F1_000039A6570C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "VarList.h"
#include "DbCurve.h"
#include "DbCurveList.h"
#include "DbPattern.h"
#include "WmChain.h"

// ==================================================================

#include "Command.h"
#include "RouteList.h"

class CGeoArc;
class CDbEntity;
class CDbTool;
class CDbWorkplane;
class CDbLine;
class CDbArc;
class CDbHole;
class CDbContainer;
class CDbFeature;
class CRaster;
class CSelector;

// ==================================================================
/////////////////////////////////////////////////////////////////////////////
// CCreateProcessApp
// See CreateProcess.cpp for the implementation of this class
//

class CCreateProcessApp : public CWinApp
{
public:

	CCreateProcessApp();

	static CReturn dllExport RegisterProcess( CRouteList* io_route );

	static CReturn dllExport Create( CCommand* io_cmd );
	static CReturn dllExport Plane( CCommand* io_cmd );
	static CReturn dllExport Layer( CCommand* io_cmd );
	static CReturn dllExport Tool( CCommand* io_cmd );

	static CReturn dllExport Point( CCommand* io_cmd );
	static CReturn dllExport Hole( CCommand* io_cmd );
	static CReturn dllExport Command( CCommand* io_cmd );
	
	// EXPERIMENTAL
	static CReturn dllExport Ellipse( CCommand* io_cmd );
	static CReturn dllExport EllipseApproximate( CCommand* io_cmd );

	static CReturn dllExport Clamp( CCommand* io_cmd );

	static CReturn dllExport BBPartOutline( CCommand* io_cmd );
	static CReturn dllExport CHPartOutline( CCommand* io_cmd );

	static CReturn dllExport Line( CCommand* io_cmd );
	static CReturn dllExport Arc( CCommand* io_cmd );
	static CReturn dllExport Split( CCommand* io_cmd );

	static CReturn dllExport Delete( CCommand* io_cmd );
	static CReturn dllExport Empty( CCommand* io_cmd );

	static CReturn dllExport Orphan( CCommand* io_cmd );
	static CReturn dllExport UpdateOrphan( CCommand* io_cmd );

	static CReturn dllExport Extract( CCommand* io_cmd );
	static CReturn dllExport OffsetCurve( CCommand* io_cmd );
	static CReturn dllExport OffsetProfile( CCommand* io_cmd );

	static CReturn dllExport Nibble( CCommand* io_cmd );

	static CReturn dllExport Transform( CCommand* io_cmd );
	static CReturn dllExport Move( CCommand* io_cmd );
	static CReturn dllExport Rotate( CCommand* io_cmd );
	static CReturn dllExport Scale( CCommand* io_cmd );
	static CReturn dllExport Mirror( CCommand* io_cmd );
	static CReturn dllExport MirrorLine( CCommand* io_cmd );
	static CReturn dllExport Grid( CCommand* io_cmd );

	static CReturn dllExport Profile( CCommand* io_cmd );
	static CReturn dllExport ProfileCreate( CCommand* io_cmd );
	static CReturn dllExport ProfileModify( CCommand* io_cmd );
	static CReturn dllExport ProfileIsClosed( CCommand* io_cmd );
	static CReturn dllExport ProfileGrow( CCommand* io_cmd );
	static CReturn dllExport ProfileExplode( CCommand* io_cmd );
	static CReturn dllExport ProfileSelected( CCommand* io_cmd );
	static CReturn dllExport ProfileReverse( CCommand* io_cmd );
	static CReturn dllExport ProfileSplit( CCommand* io_cmd );
	static CReturn dllExport ProfileBlend( CCommand* io_cmd );
	static CReturn dllExport ProfileChamfer( CCommand* io_cmd );
	static CReturn dllExport ProfileCount( CCommand* io_cmd );
	static CReturn dllExport ProfileCurve( CCommand* io_cmd );
	static CReturn dllExport ProfileAssociate( CCommand* io_cmd );
	static CReturn dllExport ProfileDisassociate( CCommand* io_cmd );
	static CReturn dllExport IsUsableCurve( CCommand* io_cmd );

	static CReturn dllExport Feature( CCommand* io_cmd );
	static CReturn dllExport FeatureCreate( CCommand* io_cmd );
	static CReturn dllExport FeatureModify( CCommand* io_cmd );
	static CReturn dllExport FeatureReorder( CCommand* io_cmd );
	static CReturn dllExport FeatureCount( CCommand* io_cmd );
	static CReturn dllExport FeatureEntity( CCommand* io_cmd );
	static CReturn dllExport FeatureToolAssociate( CCommand* io_cmd );

	static CReturn dllExport Pattern( CCommand* io_cmd );
	static CReturn dllExport PatternCreate( CCommand* io_cmd );
	static CReturn dllExport PatternModify( CCommand* io_cmd );
	static CReturn dllExport PatternCount( CCommand* io_cmd );
	static CReturn dllExport PatternEntity( CCommand* io_cmd );
	static CReturn dllExport PatternEdit( CCommand* io_cmd );
	static CReturn dllExport PatternInstance( CCommand* io_cmd );
	static CReturn dllExport PatternExplode( CCommand* io_cmd );

	static CReturn dllExport Toolpath( CCommand* io_cmd );
	static CReturn dllExport ToolpathSpiral( CCommand* io_cmd );
	static CReturn dllExport AutoTool( CCommand* io_cmd );
	static CReturn dllExport Disassociate( CCommand* io_cmd );

	static CReturn dllExport ConvexHullOffset( CCommand* io_cmd );
	static CReturn dllExport AutoIndexOffset( CCommand* io_cmd );
	static CReturn dllExport AutoPunch( CCommand* io_cmd );
	static CReturn dllExport PunchShapesCreate( CCommand* io_cmd );
	static CReturn dllExport Raster( CCommand* io_cmd );
	static CReturn dllExport Slit( CCommand* io_cmd );

	static CReturn dllExport DropStop( CCommand* io_cmd );

	static CReturn dllExport Lead( CCommand* io_cmd );
	static CReturn dllExport LeadPoint( CCommand* io_cmd );
	static CReturn dllExport LeadSelection( CCommand* io_cmd );
	static CReturn dllExport LeadModel( CCommand* io_cmd );

	static CReturn dllExport Boolean( CCommand* io_cmd );

	static CReturn dllExport Font( CCommand* io_cmd );
	static CReturn dllExport FontLoad( CCommand* io_cmd );
	static CReturn dllExport FontUnload( CCommand* io_cmd );
	static CReturn dllExport FontText( CCommand* io_cmd );

	static CReturn dllExport ZonesCount( CCommand* io_cmd );
	static CReturn dllExport ZoneGet( CCommand* io_cmd );
	static CReturn dllExport ZoneUpdate( CCommand* io_cmd );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateProcessApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCreateProcessApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private: // methods

	static void AutoPunchEntitiesGet(
								CModel&			model,
								int				profid,
								CDbEntityList*	dbEntities );

	static CReturn do_orphan( const CModel& model, CDbEntity* dbEntity );

	static CReturn ProfileGenerate(
						CModel* model,
						CDbWorkplane* dbWork,
						CDbTool* dbTool,
						CDbEntity* dbEntity,
						int dir,
						double dist,
						double sharpAngle,
						double zlevel,
						CDbFeature* dbFeature );  
						
	static CReturn ProfileGenerate2(
						CModel* model,
						CDbWorkplane* dbWork,
						CDbTool* dbTool,
						CDbEntity* dbEntity,
						bool climbCut,
						double dist,
						double sharpAngle,
						double zlevel,
						CDbFeature* dbFeature );

	// static CDbEntity* EntityGet( CCommand* io_cmd );

	static CDbContainer* ContainerFind( CModel& model, ID id );

	static void ProfileMidptsSplit(
		CDbProfile*		dbProfile,
		double			gap,
		bool			reparent );

	static void ProfileCornersSplit(
		CDbProfile*		dbProfile,
		double			gap,
		bool			reparent );

	static CReturn TextCreate(
						const CDbFeature&	dbFont,
						const CString&		text,
						double				size,
						double				radians,
						const C3dCoord&		pt,
						CModel*				model );

	static CDbFeature* CharFind(
						const CDbFeature&	dbFont,
						const char&			chr );

	static CDbFeature* FontFind( const CModel& model, const CString& fontName );

	static void FontLayer( CModel* model, CDbFeature* dbFont );


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Support methods for Toolpath:Raster: & Toolpath:Slit:
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	static CWmChain* ProfileConvert( const CDbEntity* boundary );

	static void RasterChainsCreate(
						CWmChain*		bdryChain,
						double			angle,
						double			step,
						CWmChainList*	rasterChains );

	static CDbFeature* ToolPathCreate(
						const CRaster&	raster,
						CDbTool*		dbTool,
						CModel*			model );

	static CReturn TrimmingChainsGet(
						CModel*			model,
						CWmChainList*	trimmingChains );

	static void FeatureAdditions( CSelector& selector, CDbEntityArray* dbEntities );

	static CDbEntity* HighestSelected( CDbEntity* dbEntity );

	static void ModelCopy(
					const CModel&	srcModel,
					CModel*			cpyModel );

	static void ClampsUpdate(
					const CModel&	model,
					CDbFeature*		workZone,
					bool			updateClampNum );

	static bool HaveClamps( const CModel& model );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Line Creation/Query (like duh)
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	static CReturn line_2pt( CCommand* io_cmd );
	static CReturn line_len_ang( CCommand* io_cmd );
	static CReturn line_ps_tan( CCommand* io_cmd );
	static CReturn line_tan_pe( CCommand* io_cmd );
	static CReturn line_tan_tan( CCommand* io_cmd );
	static CReturn line_query( CCommand* io_cmd );

	static CDbLine* GetLine( CCommand* io_cmd );
	static C3dCoord GetPt( CCommand* io_cmd, char what );
	static CReturn  OldLine( CCommand* io_cmd );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Arc Creation/Query (like duh)
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	static CReturn arc_pc_rad( CCommand* io_cmd );
	static CReturn arc_ps_pi_pe( CCommand* io_cmd );
	static CReturn arc_ps_pe_rad( CCommand* io_cmd );
	static CReturn arc_ps_pc_ang( CCommand* io_cmd );
	static CReturn arc_ps_pe_pc( CCommand* io_cmd );
	static CReturn arc_ps_tan( CCommand* io_cmd );
	static CReturn arc_tan_pe( CCommand* io_cmd );
	static CReturn arc_tan_tan( CCommand* io_cmd );
	static CReturn arc_query( CCommand* io_cmd );

	static CReturn circle_pc_diam( CCommand* io_cmd );
	static CReturn circle_query( CCommand* io_cmd );

	static CReturn arc_pt_tan(
						const C3dCoord&	ps,
						const CGeoArc&	geoArc,
						const C3dCoord&	hint,
						double			rad,
						int				dir,
						CGeoArc*		result );

	static CDbArc* GetArc( CCommand* io_cmd );

	static CReturn OldArc( CCommand* io_cmd );

	static void HoleAttribsApply( const CVarList& attribs, CDbHole* dbHole );

	static CReturn EntityReverse( CDbEntity* dbEntity );

	static CReturn EllipseApproximate(
		const C3dCoord&	pc,
		double			a,
		double			b,
		double			orient,
		int				narcs,
		C3dCoordArray*	pts );

	static CReturn EllipseApproximate(
		int				method,
		const C3dCoord&	pc,
		double			a,
		double			b,
		double			maxdev,
		double			orient,
		C3dCoordArray*	pts );

	static void ChordCalc(
		double a, double b, double maxdev,
		double xs, double ys, double* xe, double* ye );

	static void DeviationCalc(
		double a, double b,
		double xs, double ys,
		double xe, double ye,
		C3dCoord* result );

	static ID ProfileCurveSplit(
		CDbCurve*		dbCurve,
		const C3dCoord	pt,
		double			gap,
		bool			reparent);

	static CDbEntity* CurveSplit(
		CDbCurve*		dbCurve,
		const C3dCoord	pt,
		double			gap,
		bool			reparent );

	static CDbCurve* MidPointCandidateGet(
		const CDbProfile& dbProfile, int start_index, double gap, C3dCoord* midpt );

	static bool IsMidPointCandidate(
		const CDbCurve& dbCurve, double gap, C3dCoord* midpt );

	static void CornersSplit(
		const CDbEntityList& curves, double area, double gap );

	static void Repackage( const CDbEntityList& curves );

	static void TagsRemove( const CDbEntityList& curves );

private: // data

	static CRouteList m_createRouter;
	static CRouteList m_profileRouter;
	static CRouteList m_featureRouter;
	static CRouteList m_patternRouter;
	static CRouteList m_toolpathRouter;
	static CRouteList m_editRouter;
	static CRouteList m_transformRouter;
	static CRouteList m_leadRouter;
	static CRouteList m_fontRouter;
	static CRouteList m_zoneRouter;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATEPROCESS_H__00D4DDA6_0F5A_11D3_B7F1_000039A6570C__INCLUDED_)
