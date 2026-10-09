#if !defined(_REPOZONE_H)
#define _REPOZONE_H

// ==================================================================
//		RepoZone
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "NestConfig.h"
#include "2dBox.h"

#include "Sheet.h"
#include "DbCommand.h"
#include "ToolConst.h"  // for enum eHoldType

// ==================================================================

class dllExport CRepoZone
{
public:
	CRepoZone();
	virtual ~CRepoZone();

	CReturn	InitRepo( 
				const CNestConfig&	config, 
				const CSheet&		sheet );
	CReturn	InitRepo( 
				double		sheet_length,
				double		sheet_width,
				double		mach_xmax,
				double		repo_overlap,
				double		repo_short,
				double		clamp_width,
				double		clamp_buffer );

	int		Count( void ) const			{ return m_zone_array.GetSize(); }
	C2dBox	Zone( int pass ) const		{ return m_zone_array[pass]; }

	int		HotRepo( void ) const		{ return m_hot_repo; }
	void	HotRepo( int num )			{ m_hot_repo = num; }

	int		DidRepo( void ) const		{ return m_did_repo; }

	CReturn	Mark( CSheet* sheet, int zone );
	CReturn	Mark( CModel* model, int zone );

	//
	// Clamp Maintenance
	//
	CReturn	ClampGrid( 
					const CNestConfig&	config, 
					CSheet&				sheet, 
					int					zone, 
					bool				on );

	CReturn	ClampMark( 
					const CNestConfig&	config, 
					CSheet*				sheet, 
					int					zone );

	CReturn ClampReMark( 
					CModel*				model,
					int					zone );

	CReturn	AnalyzeClamps( 
					CModel*	model );

	C2dBox	ClampBox( 
					const CNestConfig&	config, 
					int					clamp,
					double				zone_offset,
					const CSheet&		sheet );

	//
	// Hold-Down Maintenance
	//

	CReturn	HoldGrid( 
					CViewMgr&			view,
					const CNestConfig&	config, 
					CSheet&				sheet, 
					int					zone, 
					bool				on );

	CReturn	HoldMark( 
					const CNestConfig&	config, 
					CSheet&				sheet, 
					int					dest_zone,
					int					hold_zone,
					CDbCommand**		hold );


protected:

private:
	// Disabled.
	CRepoZone( const CRepoZone& );
	const CRepoZone& operator = ( const CRepoZone& );
	int operator == ( const CRepoZone& ) const;
	int operator != ( const CRepoZone& ) const;

	CReturn hold_locate( 
					CViewMgr&			view,
					const CNestConfig&	config, 
					CSheet&				sheet,
					int					zone );

	int		hold_score(
					CSheet&	sheet,
					int		at_x,
					int		at_y );
	int		hold_score_disk(
					CSheet&			sheet,
					const C2dCoord&	ctr,
					int				dia );
	int		hold_score_box(
					CSheet&		sheet,
					C2dBox&		box );

	void	hold_draw(
					CViewMgr&	view,
					CSheet&		sheet,
					int			at_x,
					int			at_y );

	void	hold_draw_disk(
					CViewMgr&	view,
					CSheet&			sheet,
					const C2dCoord&	ctr,
					int				dia );

	void	hold_draw_box(
					CViewMgr&	view,
					CSheet&		sheet,
					C2dBox&		box );


private:
	// Clamps and zones operate in real part space
	CArray<C2dBox, C2dBox>	m_zone_array;	
	CArray<C2dBox, C2dBox>*	m_clamp_array;	
	CArray<C2dCoord, C2dCoord> m_hold_array;

	int m_hot_repo;		// How many zones are used?
	int m_did_repo;		// How many did we actually mark?

	// Hold-downs operate in integer grid space
	eHoldType	m_hold_type;		// Shape of the hold-down(s)
	int			m_hold_x1;			// Left offset (given by user)
	int			m_hold_x2;			// Right offset (given by user)
	int			m_hold_y;			// Vertical offset (user)
	int			m_hold_dia;			// Diameter or width of hold-down (user)
	int			m_hold_dead;		// Dead area around edges
//	C2dCoord	m_hold_ctr;			// Placement (automatic)
};

#endif

