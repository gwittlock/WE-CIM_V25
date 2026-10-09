#pragma once

// ==================================================================
//		Base View
//
//	A single view, base class.  Specific view child-classes
//	know how to draw entities in particular ways.  Each view,
//	however, has a view transform and a destination DC.
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

#include "Type.h"
#include "Return.h"

#include "3dCoord.h"
#include "3dVec.h"
#include "3x4Matrix.h"

#include "ViewXform.h"
#include "ViewStack.h"
#include "GeoCurve.h"

#include "Model.h"

#include "Command.h"

#include "DBWorkplane.h"

class CDisplayEntity;

enum EDisplayList
{
	MODEL_LIST = 0x01,
	STEMP_LIST = 0x02,
	PTEMP_LIST = 0x04,
	TEMP_LISTS = 0x06,
	ALL_LISTS =  0x07
};

enum EViewBuffer
{
	BACK_BUFFER =  0x01,
	FRONT_BUFFER = 0x02
};

// ==================================================================


enum eRubber
{
	RUBBER_OFF		 = 0x00,
	RUBBER_LINE		 = 0x01,
	RUBBER_BOX		 = 0x02,
	RUBBER_CIRCLE	 = 0x03,
	RUBBER_SELECTION = 0x04,
	RUBBER_HOLD		 = 0x05
};

enum eColor
{
	COLOR_BG,
	COLOR_FG,
	COLOR_HOT,
	COLOR_SHEET,
	COLOR_PATTERN
};

// ==================================================================

class dllExport CViewBase
{
public:

	CViewBase( ID in_id );

	virtual ~CViewBase();

	void terminate();

	virtual CReturn Create( HDC in_dc );
	virtual CReturn Create( HWND in_hwnd );

	virtual void ModelSet( CModel& in_model ) = 0;

	ID getID()		{ return m_id; }

	CWnd* getCWnd()	{ return m_cwnd; }

	bool Show( bool show );

	static bool		GDI( void );
	static void		GDI( bool on );

	static void		Markers( bool in_mark );	// System markers
	static bool		Markers( void );

	static void		Stock(bool show);
	static bool		Stock();

	static void		ZoneMarkers(bool in_mark);	// Zone markers
	static bool		ZoneMarkers(void);

	static void		MarkerScale( int in_mark );
	static int		MarkerScale( void );

	static void		HotDot( bool dot );
	static bool		HotDot( void );

	static void		ToolDash( bool dash );
	static bool		ToolDash( void );

	static void		PrintText( bool text );
	static bool		PrintText( void );

	static bool		PrintText( const CDbEntity* dbEntity );

	static void		PrintLegend( bool legend );
	static bool		PrintLegend( void );

	static void		PrintVerbatim( bool active );
	static bool		PrintVerbatim( void );

	static void		Monochrome( bool mono );
	static bool		Monochrome( void );

	static void		Nibble( bool nibble );
	static bool		Nibble( void );

	virtual void	Color( eColor idx, int rgb ) = NULL;
	virtual int		Color( eColor idx ) = NULL;

	static void		PickTol( int pixels );
	static int		PickTol( void );

	static void		GridTol( double tol );
	static double	GridTol( void );

	static void		ViewLimit( double tol );
	static double	ViewLimit( void );

	static void		ViewFactor( double factor );
	static double	ViewFactor( void );

	static void		HighContainers(bool high);
	static bool		HighContainers(void);

	static void		EndPoints(bool high);
	static bool		EndPoints();

	static void		Rapids(bool high);
	static bool		Rapids();

	virtual void Resize()  { }

	virtual void	Clear( EDisplayList which )		{ which;  /* compiler fodder, do nothing */ }

	virtual void DisplayListBegin( EDisplayList which )		{ which;  /* compiler fodder, do nothing */ }
	virtual void DisplayListEnd( EDisplayList which )		{ which;  /* compiler fodder, do nothing */ }

	// NOTE: BufferShow() clears the canvas on every call unless
	// clearing has be suppressed by a call to ClearEnable().
	virtual void BufferShow( EViewBuffer which )			{ which;  /* compiler fodder, do nothing */ }

	// Argh!
	virtual	void ClearEnable( bool enable )			{ enable;  /* compiler fodder, do nothing */ }
	virtual	void ModelShowEnable( bool enable )		{ enable;  /* compiler fodder, do nothing */ }


	virtual void	Refresh( bool regen ) = NULL;
	virtual void	Refresh( ID in_id, bool in_marker ) = NULL;

	virtual void	Export( FILE* f ) = NULL;

	virtual void	Transform( void ) = NULL;
	virtual void	Transform( ID in_id ) = NULL;

	virtual void	Print( const CModel& in_model, bool quiet, bool wmf, CString file ) = NULL;

	virtual CReturn	Move( const C3dCoord& in_mouse, bool snap, CCommand* io_cmd ) = NULL;
	virtual CReturn	Pick( const C3dCoord& in_mouse, CCommand* io_cmd ) = NULL;

	virtual CReturn	Highlight( ID in_id ) = NULL;
	virtual CReturn	Erase( ID in_id ) = NULL;
	virtual CReturn	Draw( ID in_id ) = NULL;

	virtual CReturn	Save( bool reset ) = NULL;
	virtual CReturn	Next( const CModel& in_model ) = NULL;
	virtual CReturn	Prev( const CModel& in_model ) = NULL;

	virtual CReturn	Full( const CModel& in_model, bool regen ) = NULL;
	virtual CReturn MaterialExtents( const CModel& in_model, bool regen ) = NULL;
	virtual CReturn	Window( const C2dCoord& in_one, const C2dCoord& in_two ) = NULL;
	virtual CReturn	Pan( const CModel& in_model, const C2dCoord& in_one, const C2dCoord& in_two ) = NULL;
	virtual CReturn	Zoom( const CModel& in_model, const C2dCoord pt, double scale ) = NULL;
	virtual CReturn	Rotate( const CModel& in_model, const C2dCoord& in_one, const C2dCoord& in_two ) = NULL;

	virtual CReturn	Angle( const CModel& in_model, double in_xy, double in_yz, double in_zx ) = NULL;
	virtual CReturn	Plane( const CModel& in_model, ID in_plane_id ) = NULL;

	virtual void	RubberStart( eRubber in_type, const C3dCoord& in_mouse, const CModel& in_model ) = NULL;
	virtual void	RubberStop( void ) = NULL;

	virtual void	mapWorldToView( const C3dCoord& in_world, C3dCoord* io_view ) const = NULL;
	virtual void	mapViewToScreen( const C3dCoord& in_view, CPoint* io_screen ) const = NULL;

	virtual void	mapRefToWorld( const C3dCoord& in_ref, const CDbWorkplane* in_src, C3dCoord* io_world ) const = NULL;

	virtual void	mapScreenToView( const CPoint& in_screen, C3dCoord* io_view ) const = NULL;
	virtual void	mapViewToWorld( const C3dCoord& in_view, C3dCoord* io_world ) const = NULL;
	virtual void	mapWorldToRef( const C3dCoord& in_world, const CDbWorkplane* in_dest, C3dCoord *io_ref ) const = NULL;

	virtual void	mapScreenToWorld( const CPoint& in_screen, C3dCoord* io_world ) const = NULL;
	virtual void	mapWorldToScreen( const C3dCoord& in_world, CPoint* io_screen ) const = NULL;

	virtual void XorEnable( bool enable ) = NULL;

	virtual void DebugLine( C3dCoord& st, C3dCoord& en ) = NULL;
	virtual void RapidLine( C3dCoord& st, C3dCoord& en, int clr ) = NULL;

	virtual void DrawAtColor( eDisplayColor color ) = NULL;
	virtual void DrawAtStyle( eDisplayStyle style ) = NULL;

	virtual void DrawCurve( const CGeoCurve* curve, int draw ) = NULL;
	virtual void DrawGeoAt( const CGeoElem& geo, const C3dCoord& delta ) = NULL;

	virtual CReturn	Annotate( CString*text ) = NULL;

	bool Solid( void )			{ return m_solid; }
	void Solid( bool solid )	{ m_solid = solid; }

	static void Fill( bool enable );
	static bool Fill();

	virtual CModel* ActiveModel() const = NULL;

	virtual COLORREF BackGroundColor() const = NULL;

	virtual void DrawDirect( CDisplayEntity* disp_ent ) = 0;

	virtual void CenterPt( double* xc, double* yc )
		{ (*xc) = 0.;  (*yc) = 0.; }

	virtual void InstanceDraw( CDbCommand* dbInstance ) = 0;
	virtual void DrawPatternAt( CDbPattern* dbPattern, C3x4Matrix* xform ) = 0;
	virtual void SemiTempDelta( const C3dCoord& C3dCoord ) = 0;

protected:

private:
	// Disabled.
	CViewBase( const CViewBase& );
	const CViewBase& operator = ( const CViewBase& );
	int operator == ( const CViewBase& ) const;
	int operator != ( const CViewBase& ) const;

protected:
	CViewStack	m_view_stack;	// Stack of view transforms...
	bool		m_solid;		// Solid view
	int			m_animate_speed;	// Solid view animation speed

private:
	ID		m_id;				// View identifier

	// TODO:  Move modes into bitflag
	static bool m_stock;
	static bool	m_markers;			// TRUE if we want to display all the markers
	static bool	m_zonemarkers;		// TRUE if we want to display all the zone stuff
	static int	m_markscale;		// Marker scale 0..9 (4 is factor of 1.0)
	static bool	m_hotdot;			// TRUE if we want Hot Dots to be active
	static bool	m_tooldash;			// TRUE if we want tool dashes
	static bool m_text;				// TRUE if we want to print text
	static bool m_legend;			// TRUE if we want to print the nesting legend
	static bool m_mono;				// TRUE for B&W display
	static bool m_verbatim;			// TRUE if we want to leave text entities in place
	static bool m_nibble;			// TRUE to display wireframe curve nibbles
	static bool m_gdi;				// TRUE for GDI graphics override
	static bool m_highcontainers;	// TRUE if we allow highlighting of containers
	static bool m_rapids;			// TRUE if we display rapids during code view
	static bool m_endpts;			// TRUE if we want to display end-point markers
	static bool m_fill;

	static int  m_pick_tol;
	static double m_grid_tol;		// Affects resolution of mouse movement in world
	static double m_view_limit;		// Affects limits of zoom-in
	static double m_view_factor;	// Border for view full

	CWnd*	m_cwnd;				// View window
	CPoint	m_carat;
};
