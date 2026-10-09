#if !defined(_LEAD_H)
#define _LEAD_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Common.h"
#include "Model.h"
#include "LeadData.h"

#include "DbProfile.h"
#include "DbCurve.h"

#include "GeoArc.h"
#include "GeoLine.h"
#include "DbHole.h"

#include "LeadPoint.h"

// ==================================================================

class CLeadCandidate
{
public:

	CLeadCandidate();
	~CLeadCandidate();

	CString ParamsName()  { return m_leadInParams.Name(); }

	void IsClosedSoln( bool is_closed )  { m_is_closed = is_closed; }
	bool IsClosedSoln() const            { return m_is_closed; }

	void BasisSet( bool is_basis )  { m_is_basis = is_basis; }
	
	bool IsUsable() const;

	bool IsExternal() const;

	void Pt( const C3dCoord& pt)    { m_pt = pt; }
	const C3dCoord& Pt() const      { return m_pt; }

	void CurveIndex( int index )    { m_index = index; }
	int CurveIndex() const          { return m_index; }

	void Radians( double radians )  { m_radians = radians; }
	double Radians() const          { return m_radians; }

	void RefDbCurve( const CDbCurve* dbCurve )  { m_dbcurve = dbCurve; }
	const CDbCurve* RefDbCurve() const  { return m_dbcurve; }

	// NOTE: Setting the lead params is defered to LeadDataGet()!!!!
	void LeadInParamsSet( const CLeadData& leadInParams );
	void LeadOutParamsSet( const CLeadData& leadOutParams );
	void BaseParamsOverride( const CLeadData& baseParams );

	const CLeadData& LeadInParamsGet() const   { return m_leadInParams; }
	const CLeadData& LeadOutParamsGet() const  { return m_leadOutParams; }

	// TODO: Might need to use CGeoElemArray because of pierce holes.
	void LeadInGeoSet( CGeoCurveArray* geoCurves );
	void LeadOutGeoSet( CGeoCurveArray* geoCurves );

	const CGeoCurveArray& LeadInGeoGet() const   { return m_leadInGeo; }
	const CGeoCurveArray& LeadOutGeoGet() const  { return m_leadOutGeo; }

	void StartPointSet( const C3dCoord& pt )  { m_ps = pt; }
	const C3dCoord& StartPointGet() const     { return m_ps; }

	void EndPointSet( const C3dCoord& pt )    { m_pe = pt; }
	const C3dCoord& EndPointGet() const       { return m_pe; }

public:

	static int IncreasingDelta( const void* ptrA, const void* ptrB );
	static int DecreasingDelta( const void* ptrA, const void* ptrB );

private:

	C3dCoord m_pt;
	int m_index;

	bool m_is_basis;
	bool m_is_closed;

	// 'm_radians' is used for ordering candidate solutions
	double m_radians;

	CLeadData m_leadInParams;
	CLeadData m_leadOutParams;

	CGeoCurveArray m_leadInGeo;
	CGeoCurveArray m_leadOutGeo;

	C3dCoord m_ps;
	C3dCoord m_pe;

	// A dirty little hack introduced for v21.0.181.0
	// The model entity at the position where this lead would be
	// placed if used. This data is necessary because it helps
	// the leads to be created with the correct tool, workplane, etc.
	const CDbCurve* m_dbcurve;
};

typedef CDynamicArray<CLeadCandidate*> TLeadCandidates;


// ==================================================================

class dllExport CLead
{
public:

	CLead( CModel& model );
	virtual ~CLead();

	// So that nesting can generate meaningful messages ....
	void PartName( const CString& name )  { m_part_name = name; }

	CReturn	Manual( 
				ID					id,
				int					side,
				const CLeadData*	lead_in,
				const CLeadData*	lead_out );

	CReturn	SemiAuto(
				const CString&	configdb, 
				int				profid,
				int				setup,
				bool			exterior,
				double			min_overlap );

	CReturn	Auto(
				const CString&	configdb, 
				int				setup,
				const CSelector& selector,
				bool			mark_int,
				bool			split,
				int				restrictions );

	CReturn	AutoOpen(
				const CString&	configdb,
				int				setup,
				bool			use_winding_flag,
				CSelector*		selector );

	double	LeadWidth(
				const CString&	configdb,
				int				setup );

	CReturn	ReduceOpen( 
				CSelector*		selector, 
				bool			use_winding_flag,
				CDbEntityList*	dstlist );

	CDbTool*	PierceTool( 
							const CString&	configdb,
							int				setup,
							int				context,
							CDbWorkplane*	dbwork,
							double*			out_dia = NULL );

	CDbTool*	PierceTool( 
							const CLeadData& data,
							CDbWorkplane*	dbwork,
							double*			out_dia = NULL );

	CReturn		Locate( CDbProfile* db_prof, 
						int dir, 
						double split, 
						CLeadPointArray* lead_at );

	CDbContainer*	Root() const		{ return m_root; }

	// Introduced in V20 (per ITI request) to make lead placement
	// more flexible. In particular, the leads should be placed
	// so they don't overlap any part geometry.
	CReturn LeadExperiment( CDbProfile* dbProfile, bool split );

public:

	// 2012.04.28 (PE) -- A complete hack.
	static void MasterModel( CModel* masterModel );
	static CModel* MasterModel();

	// TODO:  Move these somewhere else; used by both LeadProcess and AttribProcess
	static CReturn Reduce(
		const CSelector& selector, int restrictions, CDbEntityList* proflist );
	
	static bool IsClosedForLeads( const CDbProfile& dbProfile );

private: // Disabled.

	CLead();
	CLead( const CLead& );
	const CLead& operator = ( const CLead& );
	int operator == ( const CLead& ) const;
	int operator != ( const CLead& ) const;

private:

	CReturn	load_setup( const CString& configdb, int setup );
	CReturn	load_parameter( const CString& configdb, CLeadData* data, int parameter );

	bool CanApplyLeads( const CDbProfile& dbProfile, int restrictions );

	// Lead operations, on Selection Set
	CReturn gen_lead( CDbEntityList* proflist, bool split, int restrictions );

	// Lead operations, on Profile
	// TODO: Consider for more general placement in other libraries
	CReturn	autosplit_prof( CDbProfile* dbprof, const CLeadData& data );
	CDbCurve* outside_corner( CDbProfile* dbprof, CDbCurve* start, int winding );
	CReturn	reorder_prof( CDbProfile* prof, const CDbCurve* curve );
	CReturn	autolead_prof( CDbProfile* dbprof, bool split );
	CReturn lead_prof( CDbProfile* dbprof, const CLeadData& data, int context );
	CReturn lead_curve( CDbCurve* dbcurve, const CLeadData& data, int context );
	CReturn lead_junction( CDbProfile* dbprof, const CLeadData&	data );

	CReturn	split_candidates(
				CLeadPointArray* candidate,
				CDbProfile*	db_prof,
				int dir );
	CReturn	profile_candidates(
				CLeadPointArray* candidate,
				CDbProfile*	db_prof );
	CReturn	reduce_candidates(
				CLeadPointArray* candidate,
				CDbProfile*	db_prof,
				double split );
	bool is_outside_corner( 
				CGeoCurve* first,
				CGeoCurve* second,
				int winding );
	CReturn sort_candidates(
				CLeadPointArray* candidate,
				CDbProfile*	db_prof,
				int dir ); 





	// Lead operations, geometry
	CGeoArc* gen_lead_arc( 
		const CGeoCurve&	curve, 
		int					context, 
		eLeadSide			lead_side, 
		double				angle, 
		double				radius );

	CGeoLine* gen_lead_line( 
		const CGeoCurve&	curve, 
		int					context, 
		eLeadSide			lead_side, 
		double				angle, 
		double				length );

	CGeoLine* gen_lead_ramp( 
		const CGeoCurve&	curve, 
		int					context, 
		eLeadSide			lead_side, 
		double				angle,
		bool				tilt );

	CGeoArc* gen_lead_pierce( 
		const CGeoCurve&	curve, 
		const CLeadData&	data );

	CDbHole* gen_lead_pierce( 
							const CGeoCurve&	curve, 
							CDbWorkplane*		dbwork,
							int					context, 
							const CLeadData&	data);

	void AttribsApply(
		const CDbEntity&	src_dbent,
		const CDbTool*		dbTool,
		CDbEntity*			dst_dbent );

	int		get_winding( const CDbProfile& db_prof );
	int		get_cutside( const CDbProfile& db_prof );

	CGeoLine ClockVector( const CDbProfile& dbProfile, int dir );

	CDbCurve* ActualStartCurve( const CDbProfile& dbProfile ) const;
	CDbCurve* ActualEndCurve( const CDbProfile& dbProfile ) const;

	void LeadInRemove( CDbFeature* dbFeature, const CDbProfile* dbProfile );
	void LeadOutRemove( CDbFeature* dbFeature, const CDbProfile* dbProfile );
	
	// ==================================================================

	void ProfileToGeopoly( const CDbProfile& dbProfile, CGeoPoly* geoPoly );
	void PolyReorder( const CLeadCandidate& candidate, CGeoPoly* geoPoly );
	void LeadsPolyModify( const CLeadCandidate& candidate, CGeoPoly* geoPoly );

	CReturn LeadsCreate(
		const CGeoPoly& geoPoly, const CLeadCandidate& candidate, CDbProfile* dbProfile );

	CReturn ContainerAppend(
		const CGeoCurveArray& geoCurves,
		const CDbCurve& dbRefCurve,
		CDbContainer* dbContainer );

	double ClockToRadians( int clockPosition );
	bool IsGapCurve( const CGeoCurve& geoCurve );

	void CandidatePositionsCalc( const CGeoPoly& geoPoly, TLeadCandidates* candidates );
	CLeadCandidate* LeadBasisGet( const CGeoLine& basisLine, const CGeoPoly& geoPoly );
	void ClosedPositionsCalc( const CGeoPoly& geoPoly, TLeadCandidates* candidates );
	void OpenPositionsCalc( const CGeoPoly& geoPoly, TLeadCandidates* candidates );

	const CLeadData BaseLeadParamsGet( const CGeoPoly& geoPoly );
	const CLeadData LeadInParamsGet(
		const CGeoPoly& geoPoly, const CLeadCandidate& candidate );
	const CLeadData LeadOutParamsGet(
		const CGeoPoly& geoPoly, const CLeadCandidate& candidate );
	void ConditionalSplitParamOverride( const CGeoPoly& geoPoly, CLeadData* leadParams );

	void LeadDataGet( const CGeoPoly& geoPoly, CLeadCandidate* candidate );
	bool HaveCompatibleLeads(
		const CLeadData& leadInParams, const CLeadData& leadOutParams );

	void VirtualLeadsCreate( const CGeoPoly& geoPoly, CLeadCandidate* candidate );
	bool IsValidJunction( const CGeoCurve& scrv, const CGeoCurve& ecrv );
	void VirtualLeadJunction( const CGeoPoly& geoPoly, CLeadCandidate* candidate );
	void VirtualLeadInCreate( const CGeoPoly& geoPoly, CLeadCandidate* candidate );
	void VirtualLeadOutCreate( const CGeoPoly& geoPoly, CLeadCandidate* candidate );

	bool IsLeadOkay( const CGeoPoly& geoPoly, const CLeadCandidate& candidate );
	int get_cutside( const CGeoPoly& geoPoly );

	bool IsPierceHoleArc( const CGeoCurve& geoCurve );
	CDbHole* PierceHoleCreate( const CGeoCurveArray& geoCurves );

	void ProfileAttribsModify( const CLeadCandidate& candidate, CDbProfile* dbProfile );

	bool IsNestingContext() const  { return ( !m_part_name.IsEmpty() ); }
	CDbProfile* LeadHullCreate( const CDbEntityArray& chInput, const CDbCurve& dbRefCurve );

private:

	// 2012.04.28 (PE) -- A complete hack.
	static CModel* m_masterModel;

private:

	CModel*	m_model;
	CString	m_part_name;

	int				m_setup;
	CLeadData		m_context[LEAD_CONTEXT_NUM];
	CDbContainer*	m_root;
};

#endif

