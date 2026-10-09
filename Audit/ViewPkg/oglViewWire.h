
#pragma once

// ==================================================================
//		View Wire
//
//	Wire-frame renderer
//
// ==================================================================

#include "GL/gl.h"
#include "GL/glu.h"

#include "glCanvas.h"

#include "GeoArc.h"
#include "Nibbler.h"

#include "ViewBase.h"
#include "AnimateDialog.h"

#include "3dCoord.h"
#include "3x4Matrix.h"


// ==================================================================

// ==================================================================

class CoglViewWire : public CViewBase
{
public:

	CoglViewWire( ID in_id );
	virtual ~CoglViewWire();

	virtual CReturn Create( HDC in_dc );
	virtual CReturn Create( HWND in_hwnd );

	virtual void ModelSet( CModel& in_model );

	virtual void Resize();
	virtual void Clear( EDisplayList which );

	virtual void DisplayListBegin( EDisplayList which );
	virtual void DisplayListEnd( EDisplayList which );

	virtual void BufferShow( EViewBuffer which );

	// Argh!
	virtual void ClearEnable( bool enable )
		{ if (m_canvas != NULL) m_canvas->ClearEnable( enable ); }

	virtual void ModelShowEnable( bool enable )
		{ if (m_canvas != NULL) m_canvas->ModelShowEnable( enable ); }

	virtual void Color( eColor idx, int rgb );
	virtual int Color( eColor idx );

	virtual void Refresh( bool regen );
	virtual void Refresh( ID in_id, bool in_marker );

	virtual void Export( FILE* f );

	virtual void Transform( void )				{ return; }
	virtual void Transform( ID in_id )			{ return; }

	virtual CReturn	Move( const C3dCoord& in_mouse, bool snap, CCommand* io_cmd );
	virtual CReturn	Pick( const C3dCoord& in_mouse, CCommand* io_cmd );

	virtual CReturn	Highlight( ID in_id );
	virtual CReturn	Erase( ID in_id );
	virtual CReturn	Draw( ID in_id );

	virtual CReturn	Save( bool reset );
	virtual CReturn	Next( const CModel& in_model );
	virtual CReturn	Prev( const CModel& in_model );

	virtual CReturn	Full( const CModel& in_model, bool regen );
	virtual CReturn MaterialExtents( const CModel& model, bool regen );
	virtual CReturn	Window( const C2dCoord& in_one, const C2dCoord& in_two );

	C2dBox AdjustedExtent( double xll, double yll, double xur, double yur );

	virtual CReturn	Pan( const CModel& model, const C2dCoord& in_one, const C2dCoord& in_two );
	virtual CReturn	Zoom( const CModel& model, const C2dCoord world, double factor );
	virtual CReturn	Rotate( const CModel& model, const C2dCoord& in_one, const C2dCoord& in_two );

	virtual CReturn	Angle( const CModel& in_model, double in_xy, double in_yz, double in_zx );
	virtual CReturn	Plane( const CModel& in_model, ID in_plane_id );

	virtual void RubberStart( eRubber in_type, const C3dCoord& in_mouse, const CModel& in_model );
	virtual void RubberStop( void );

	virtual void mapRefToWorld( const C3dVec& in_ref, const CDbWorkplane* in_src, C3dVec* io_world ) const;
	virtual void mapWorldToView( const C3dVec& in_world, C3dVec* io_view ) const;

	virtual void mapRefToWorld( const C3dCoord& in_ref, const CDbWorkplane* in_src, C3dCoord* io_world ) const;
	virtual void mapWorldToView( const C3dCoord& in_world, C3dCoord* io_view ) const;
	virtual void mapViewToScreen( const C3dCoord& in_view, CPoint* io_screen ) const;

	virtual void mapScreenToView( const CPoint& in_screen, C3dCoord* io_view ) const;
	virtual void mapViewToWorld( const C3dCoord& in_view, C3dCoord* io_world ) const;
	virtual void mapWorldToRef( const C3dCoord& in_world, const CDbWorkplane* in_dest, C3dCoord* io_ref ) const;

	virtual void mapScreenToWorld( const CPoint& in_screen, C3dCoord* io_world ) const;
	virtual void mapWorldToScreen( const C3dCoord& in_world, CPoint* io_screen ) const;

	virtual void projectViewToRef( const C3dCoord& in_view, const CDbWorkplane& in_ref, C3dCoord* io_world) const;

	virtual void XorEnable( bool enable );

	virtual void DebugLine( C3dCoord& st, C3dCoord& en );
	virtual void RapidLine( C3dCoord& st, C3dCoord& en, int clr );

	virtual void DrawAtColor( eDisplayColor color );
	virtual void DrawAtStyle( eDisplayStyle line_style );

	virtual void DrawCurve( const CGeoCurve* curve, int draw);
	virtual void DrawGeoAt( const CGeoElem& geo, const C3dCoord& delta );

	virtual CReturn	Annotate( CString* text );

	virtual void Print( const CModel& model, bool quiet, bool wmf, CString file );
	void PrintWMF( const CModel& model, CString file );

	virtual CModel* ActiveModel() const	{ return m_model; }

	virtual void	dot( const C3dCoord& pt, bool hot );

	double Scale()
		{ return m_canvas->ViewScale(); }

	virtual COLORREF BackGroundColor() const
		{ return m_canvas->ClearColor(); }

	virtual void DrawDirect( CDisplayEntity* disp_ent );

	virtual void CenterPt( double* xc, double* yc );

	virtual void InstanceDraw( CDbCommand* dbInstance );
	virtual void DrawPatternAt( CDbPattern* dbPattern, C3x4Matrix* xform );
	virtual void SemiTempDelta( const C3dCoord& C3dCoord );


private:
	void	ogl_purge(void);
	void	ogl_init(void);
	void	ogl_setupPixelFormat( CDC* DC );
	void	ogl_setupPalette( CDC* DC );
	void	ogl_setupFont( CDC* context );
	void	ogl_test_error( const char* where );

	int		decode_text( CDisplayEntity* disp_ent, int cmd_idx, CString* text );

	void	not_hot( void );

	void refresh_setup( CRect& screen, const C2dBox& view_extent );

	void refresh_id(ID in_id, bool marker );
	void refresh_entity( CDbEntity* dbEntity, bool marker );

	void	draw_entities( const CModel* in_model, double xclip );
	void	draw_entity( CDbContainer* container, double xclip );
	void	draw_entity( CDbEntity* entity, bool in_marker, double txt_xclip );
	void	draw_entitytool( CDbEntity*	db_ent, C3dCoord*	hitpt, C2dUnitVec&	tanvec );
	void	draw_display( CDisplayEntity* disp_ent, bool in_marker, double txt_xclip, bool selected );

	void	wire_entity(CDbEntity* db_ent, bool in_marker, double txt_xclip, double tolerance);
	void	burn_curve(CDbCurve* curve, double tolerance);
	void	nibble_entity(CDbEntity* db_ent, double tolerance);

	C3dCoord draw_coord( C3x4Matrix* xform, const C3dCoord& pt );

	void	start_drawing( int type );
	void	stop_drawing( void );

	void	moveto( const C3dCoord& start, int mode );
	void	lineto( const C3dCoord& end );
	void	arcctr( const C3dCoord& pc, int dir );
	void	arcto( const C3dCoord& end, int dir );
	void	hot_dot( const C3dCoord& pt, bool new_dot );
	void	draw_dot( const C3dCoord& pt );
	void TargetDraw( const C3dCoord& world );

	void	moveto_print( const C3dCoord& start );
	void	lineto_print( const C3dCoord& end );
	void	arcto_print( const C3dCoord& end, int dir );
	void	dot_print( const C3dCoord& pt, bool hot );

	void	hot_arrow( const CModel& in_model, const C3dCoord& in_snap );
	void	arrow( const C3dCoord& in_tip, const C2dUnitVec& in_vec, double in_len );
	void	circle( double in_tolerance, const CGeoArc& in_arc );

	void	rubber( const C3dCoord&	end );

	void	text( C3dCoord& coord, const CString& text );
	void	text_print( C3dCoord& coord, const CString& text );

	void	color( int color );
	int		ColorCorrection( int color );

	void	style( eDisplayStyle in_style );
	void	text_angle( double angle )				{ m_txtang = angle; }
	void	text_size( int size )					{ m_txtsize = size; }
	void	text_pos( eDisplayTextPos pos )			{ m_txtpos = pos; }

	void	pen_select(void);

	// Because ogl scales values [0..1]
	double red( int color ) const
		{ return ((color & 0xff) / 255.); }

	double green( int color) const
		{ return (((color >> 8) & 0xff) / 255.); }

	double blue( int color ) const
		{ return (((color >> 16) & 0xff) / 255.); }

	bool IsSnappable( const CDbEntity* dbEntity );
	bool IsHotDottable( const CDbEntity* dbEntity );
	bool SupportsInteraction( const CDbEntity* dbEntity );

	CReturn	find_closest( 
		double			dist, 
		const C2dCoord&	pnt, 
		CDbEntity**		dbEntity, 
		bool*			io_dot, 
		C3dCoord*		io_snap, 
		bool*			io_end );


	void	extract_labels( const CModel&	model,
							CStringArray&	label_array,
							CString&		label_max,
							double			xclip );
	int		extract_text_xclip( CString&		text,
								CDisplayEntity*	disp_ent,
								double			xclip );
	CString	print_titles(	const CModel&	model,
							int&			title_x, 
							int&			title_y );

	void DrawPointAt( const C3dCoord& pt );
	void DrawLineAt( const CGeoLine& line, const C3dCoord& delta );
	void DrawArcAt( const CGeoArc& arc, const C3dCoord& delta );
	void DrawCurveAt( const CGeoCurve& curve, const C3dCoord& delta );
	void DrawPolyAt( const CGeoPoly& poly, const C3dCoord& delta );

	void ViewPortAdjustmentsApply( double delta, double& dx, double& dy );

	bool RegisterWindowClass();

private:

	// Disabled.
	CoglViewWire( const CoglViewWire& );
	const CoglViewWire& operator = ( const CoglViewWire& );
	int operator == ( const CoglViewWire& ) const;
	int operator != ( const CoglViewWire& ) const;

private:
	CModel*			m_model;

	eDisplayColor	m_DrawAt_color;
	eDisplayStyle	m_DrawAt_style;

	CglCanvas*	m_canvas;
	int			m_regen;

	int			m_background_color;
	int			m_foreground_color;
	int			m_hot_color;
	int			m_pattern_color;
	int			m_black;
	int			m_white;
	int			m_sheet_color;
	int			m_font_height;
	CString		m_font_name;

	CNibbler	m_teeth;

	bool		m_drawing;
	int			m_drawtype;
	DWORD		m_timer;

	C3dCoord			m_origin;
	C3dCoord			m_at;
	C3dCoord			m_ctr;
	C3dCoord			m_tooltip;

	// m_scrnctr is needed for the CoglViewWire::map.*() functions
	CPoint				m_scrnctr;

	eDisplayTextPos		m_txtpos;	// text position (justification)
	double				m_txtang;	// text angle
	int					m_txtsize;
	C2dCoord			m_carat;

	ID					m_high_id;

	// 2008.07.17 (PE) -- Introduced m_hot_entity in an attempt
	// to improve graphics performance. Its intent is to cache
	// the 'hot entity', thereby minimizing database searches
	// (which is how m_hot_id was formerly used).
	CDbEntity*			m_hot_entity;
	ID					m_hot_id;

	ID					m_pat_id;
	bool				m_pat_selected;
	bool				m_hot_dot;
	bool				m_hot_arrow;
	C3dCoord			m_hot_tip;
	C2dUnitVec			m_hot_vec;

	bool				m_old_dot;
	bool				m_old_arrow;
	C3dCoord			m_old_tip;
	C2dUnitVec			m_old_vec;

	eRubber				m_rubber_mode;
	C3dCoord			m_rubber_start;
	C3dCoord			m_rubber_end;

	double				m_delay_count;

	// Special printing information
	bool		m_printing;
	CDC			m_pdc;
	GLdouble	m_model_mat[16];
	GLdouble	m_proj_mat[16];
	GLint		m_viewport[4];
	double		m_printscale;
	int			m_printer_top;
	CPen*		m_pen;
	CPen*		m_old_pen;

	eDisplayStyle	m_style;
	int				m_color;
	eDisplayMeta	m_meta;

	FILE* m_export_file;
};

