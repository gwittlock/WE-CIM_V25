#if !defined(_VIEWSOLID_H)
#define _VIEWSOLID_H

// ==================================================================
//		View Solid
//
//	Acis-solids renderer
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

#include "Acis.h"

#include "ViewBase.h"

#include "DisplayEntity.h"

#include "GeoCurve.h"
#include "DbFeature.h"
#include "DbProfile.h"
#include "Profile.h"

#include "2dUnitVec.h"

// ==================================================================


// ==================================================================

typedef CArray< EDGE*, EDGE* >	CEdgeList;

class dllExport CViewSolid: public CViewBase
{
public:
	CViewSolid( ID in_id );
	virtual ~CViewSolid();

	virtual CReturn Create( HDC in_dc );
	virtual CReturn Create( HWND in_hwnd );

	virtual void Clear( void );

	virtual void	Refresh( void );
	virtual void	Refresh( ID in_id, BOOL in_marker );

	virtual void	Transform( void );
	virtual void	Transform( ID in_id );

	//
	// Don't even *ask* why so many regenerates
	//
	virtual void	Regenerate( const CModel& model,
								BOOL in_anim );
	virtual void	Regenerate( const CModel& model, 
								ID in_id );
			BOOL	Regenerate( const CModel& model, 
								BODY* io_stock,		
								CDbFeature* io_feature, 
								BOOL in_anim );
			BOOL	Regenerate( const CModel& model, 
								BODY* io_stock, 
								CDbProfile* io_profile, 
								BOOL in_anim );
			BOOL	Regenerate( const CModel& model,
								BODY*			io_stock,
								CDbEntity*		in_entity,
								CProfile*		in_prof,
								BOOL			in_anim );


	virtual CReturn	Move( const C3dCoord& in_mouse, BOOL snap, CCommand* io_cmd );
	virtual CReturn	Pick( const C3dCoord& in_mouse, CCommand* io_cmd );
	virtual CReturn	Highlight( ID in_id );

	virtual CReturn	Save( void );
	virtual CReturn	Next( const CModel& in_model );
	virtual CReturn	Prev( const CModel& in_model );

	virtual CReturn	Full( const CModel& in_model, BOOL regen );
	virtual CReturn	Window( const CModel& in_model, const C2dCoord& in_one, const C2dCoord& in_two, BOOL regen );
	virtual CReturn	Pan( const CModel& in_model, const C2dCoord& in_one, const C2dCoord& in_two );
	virtual CReturn	Zoom( const CModel& in_model, const C2dCoord& in_one, const C2dCoord& in_two );
	virtual CReturn	Rotate( const CModel& in_model, const C2dCoord& in_one, const C2dCoord& in_two );

	virtual CReturn	Angle( const CModel& in_model, double in_xy, double in_yz, double in_zx );
	virtual CReturn	Plane( const CModel& in_model, ID in_plane_id );

	virtual void Print( const CModel& in_model );

	virtual void		RubberStart( eRubber in_type, const C3dCoord& in_mouse, const CModel& in_model );
	virtual void		RubberStop( void );

	virtual void	mapRefToWorld( const C3dVec& in_ref, const CDbWorkplane& in_src, C3dVec* io_world ) const;
	virtual void	mapWorldToView( const C3dVec& in_world, C3dVec* io_view ) const;

	virtual void	mapRefToWorld( const C3dCoord& in_ref, const CDbWorkplane& in_src, C3dCoord* io_world ) const;
	virtual void	mapWorldToView( const C3dCoord& in_world, C3dCoord* io_view ) const;
	virtual void	mapViewToScreen( const C3dCoord& in_view, CPoint* io_screen ) const;

	virtual void	mapScreenToView( const CPoint& in_screen, C3dCoord* io_view ) const;
	virtual void	mapViewToWorld( const C3dCoord& in_view, C3dCoord* io_world ) const;
	virtual void	mapWorldToRef( const C3dCoord& in_world, const CDbWorkplane& in_dest, C3dCoord* io_ref ) const;

	virtual void	mapScreenToWorld( const CPoint& in_screen, C3dCoord* io_world ) const;
	virtual void	mapWorldToScreen( const C3dCoord& in_world, CPoint* io_screen ) const;

	virtual void	projectViewToRef( const C3dCoord& in_view, const CDbWorkplane& in_ref, C3dCoord* io_world) const;

protected:

private:
	// Disabled.
	CViewSolid( const CViewSolid& );
	const CViewSolid& operator = ( const CViewSolid& );
	int operator == ( const CViewSolid& ) const;
	int operator != ( const CViewSolid& ) const;

private:
	void	reset_context( void );
	BODY*	make_stock( double in_width, double in_height, double in_depth, DWORD in_color );
	BOOL	route_entity( BODY* io_stock, const CDisplayEntity& in_entity, BOOL in_anim );
	void	set_acis_color( ENTITY* in_ent, DWORD in_color );

	void	tool_sphere(	BODY* io_body, 
							const C3dCoord& in_at, 
							const C3dCoord& in_size );

	void	tool_cylinder(	BODY* io_body, 
							const C3dCoord& in_at, 
							const C3dCoord& in_size );

	void	tool_cone(	BODY* io_body, 
						const C3dCoord& in_at, 
						const C3dCoord& in_size );

	void	tool_torus(	BODY* io_body, 
						const C3dCoord& in_at, 
						const C3dCoord& in_size );

	BOOL	route_hole( const WCS& in_ref, 
						BODY* io_stock, 
						const BODY& in_body, 
						const C3dCoord& in_tip,
						BOOL  in_anim );

	BOOL	route_line( const WCS& in_ref, 
						BODY*			io_stock, 
						const BODY&		in_body, 
						const C3dCoord&	in_st,
						const C3dCoord&	in_en );

	BOOL	route_arc(	const WCS& in_ref, 
						BODY*			io_stock, 
						const BODY&		in_body, 
						const C3dCoord&	in_st,
						const C3dCoord&	in_en,
						const C3dCoord& in_ct,
						BOOL			in_cw );

	void	edge_line( 
						CEdgeList&	edge_list, 
						const C3dCoord&	at, 
						const C3dCoord&	to );
	void	edge_arc( 
						CEdgeList&	edge_list, 
						const C3dCoord&	at, 
						const C3dCoord&	to, 
						const C3dCoord&	ctr,
						BOOL		ccw );

	void	edge_extrude(
						const CEdgeList&	edge_list,
						BODY*&				body );




	BOOL	route_curve( const WCS&		in_ref, 
						BODY*			io_stock, 
						const BODY&		in_outline, 
						const ENTITY&	in_path );

	BODY*	cast_shadow( const BODY& in_body );

	BOOL	is_hidden( const CDbEntity* entity );

private:
	CAcis				m_acis;
	view_3d_MS*			m_view;
	gl_context*			m_context;

	eRubber				m_rubber_mode;
	rubberband_driver*	m_rubber;			// Rubber-band driver, as needed

	C3dCoord			m_tooltip;
	C3dCoord			m_stock_ext;
};

#endif

