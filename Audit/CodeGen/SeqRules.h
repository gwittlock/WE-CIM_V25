#if !defined(_SEQRULES_H)
#define _SEQRULES_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Type.h"
#include "VarList.h"
#include "3dCoord.h"
#include "2dUnitVec.h"
#include "2dBox.h"
#include "DbTool.h"
#include "Model.h"

// ==================================================================

#define PATH_RULE_XPOS_FACTOR		0x0001
#define PATH_RULE_YPOS_FACTOR		0x0002
#define PATH_RULE_XDIST_FACTOR		0x0004
#define PATH_RULE_YDIST_FACTOR		0x0008
#define PATH_RULE_TCHG_COST			0x0010
#define PATH_RULE_DROP_MIN			0x0020
#define PATH_RULE_FACE				0x0040
#define PATH_RULE_TREND				0x0080

enum ESeqMode
{
	PATH_BORE  = 0,
	PATH_ROUTE = 1,
	PATH_KEEP  = 2,
	PATH_TOOL  = 3,
	PATH_MIXED = 4,	// Obsolete?
	PATH_MODE_UNDEFINED
};

enum ESeqTrend
{
	LL_XPOS,
	LR_XNEG,
	UR_XNEG,
	UL_XPOS,
	LL_YPOS,
	LR_YPOS,
	UR_YNEG,
	UL_YNEG,
	OPT_UNDEFINED
};

enum ESeqToolOrder
{
	TOOL_ORDER_BY_ENCOUNTER,
	TOOL_ORDER_BY_JOB,
	TOOL_ORDER_UNDEFINED
};

enum ESeqProcess
{
	SEQUENCE_BY_TOOL_ORDER,
	SEQUENCE_BY_CONTAINMENT_ORDER,
	SEQUENCE_BY_FLOW_CHART,
	SEQUENCE_BY_PROGRESSIVE,
	SEQUENCE_UNDEFINED
};

// Note:  Duplicates ECTIterMethod in ContHierIterator.h
enum ECTIterMethod
{
	BY_GLOBAL_CONTAINMENT,
	BY_LOCAL_CONTAINMENT,
	BY_RECURSION,
	BY_PROGRESSIVE,
	ITER_METHOD_UNDEFINED
};

// ==================================================================

class dllExport CSeqRules
{
public:

	CSeqRules();

	void	Init( CModel* model, const CVarList& seqAttribs );

	void	WorkPkgSet( const C2dBox& box );

	void	PassCountSet( int count )		{ m_pass_num = count; }
	int		PassCountGet()					{ return m_pass_num; }

	CDbTool*	CurrTool()						{ return m_currTool; }
	void		CurrTool( CDbTool* dbTool )		{ m_currTool = dbTool; }

	void	TrendSet( bool forward_trend );

	const C3dCoord& Anchor() const			{ return m_anchor; }
	void Anchor( const C3dCoord& pt )		{ m_anchor = pt; }

	const C3dCoord& Buoy() const			{ return m_buoy; }
	void Buoy( const C3dCoord& pt )			{ m_buoy = pt; }

	void BuoyReset()						{ m_buoy = m_anchor; }

	const CModel&	Model()					{ return (*m_model); }
	CModel*			pModel()				{ return m_model; }

	virtual ~CSeqRules();


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Shortest path parameters
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	ESeqMode	mode;

	int			edges;
	bool		fixed_chunks;

	bool		bore;				// TRUE if we optimize bore elements
	bool		route;				// TRUE if we optimize route elements

	ID			tool_id;			// Current tooling in router
	ID			face_id;

	DWORD		flag;				// Bitflag determining which additional features to use

	int			linear_trend;

	double		pos_x_factor;		// Score factor based on ordinate position
	double		pos_y_factor;

	double		delta_x_factor;		// Score factore based on ordinate motion
	double		delta_y_factor;

	int			toolchange_cost;	// Fixed cost of router toolchange
	int			drop_min;			// If used, the minimum bore drops per route toolchange
	int			drop_cnt;			// Current drop count, between route toolchanges

	bool		face_1;				// TRUE if do only face-1 now


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Other parameters
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	bool			path_opt;
	bool			drill_opt;
	bool			grid_opt;
	bool			tsp_opt;
	bool			cut_avoid;
	bool			scribe_with_torch;

	ESeqToolOrder	toolOrder;
	ESeqProcess		gridSeqOption;

	C2dBox			extents;

	ESeqTrend		trend;
	C2dUnitVec		trend_vec;

	bool			bidirectional;
	bool			forward;

	bool			AllHolesFirst;
	ECTIterMethod	ByCompletePart;
	bool			HolesAcrossLocalNest;
	bool			HolesByToolOrder;

	CDbTool*		m_currTool;		// For sequencing by tool order

	double			y_origin;
	double			avoid_min;		// IF cut_avoid is true, holds the minimum size
									// of profiles to avoid


protected:

private:
	// Disabled.
	CSeqRules( const CSeqRules& );
	const CSeqRules& operator = ( const CSeqRules& );
	int operator == ( const CSeqRules& ) const;
	int operator != ( const CSeqRules& ) const;

private:

	CModel*		m_model;
	int			m_pass_num;

	C3dCoord	m_anchor;				// Anchor point, to determine traversal
	C3dCoord	m_buoy;
};

#endif

