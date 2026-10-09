#if !defined(_VIEWMGR_H)
#define _VIEWMGR_H

// ==================================================================
//		View Manager
//
//
//	Keeps track of the various views.  Routes commands to the
//	active view as needed.  Manages the view transforms.
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

#include "ViewBase.h"
#include "glCanvas.h"

// ==================================================================

class CViewMgr;

typedef CViewBase*	(*pNewSolid)(ID);
typedef void		(*pDeleteSolid)(CViewBase*);

// ==================================================================

#include "Return.h"
#include "Type.h"

#include "Var.h"
#include "VarList.h"
#include "Command.h"
#include "RouteList.h"

#include "3dCoord.h"
#include "3dVec.h"
#include "3x4Matrix.h"

#include "ViewBase.h"

#include "DisplayEntity.h"


// ==================================================================

enum eViewType
{
	VIEW_WIRE = 1,
	VIEW_SOLID = 2,
	VIEW_OGLWIRE = 3
};

// ==================================================================

class dllExport CViewMgr
{
public:
	CViewMgr();
	virtual ~CViewMgr();

	void	Reset( void );

	CReturn	Register( CRouteList* io_route );

	static CReturn Enable( CCommand* io_cmd );

	static CReturn Execute( CCommand* io_cmd );

	static CReturn Create( CCommand* io_cmd );
	static CReturn Delete( CCommand* io_cmd );
	static CReturn Select( CCommand* io_cmd );

	static CReturn Mode( CCommand* io_cmd );
	static CReturn ModeGet( CCommand* io_cmd );
	static CReturn ModeDlg( CCommand* io_cmd );

	static CReturn Resize( CCommand* io_cmd );
	static CReturn Show( CCommand* io_cmd );
	static CReturn Clear( CCommand* io_cmd );

	static CReturn Refresh( CCommand* io_cmd );
	static CReturn Export( CCommand* io_cmd );

	static CReturn Save( CCommand* io_cmd );
	static CReturn Next( CCommand* io_cmd );
	static CReturn Prev( CCommand* io_cmd );

	static CReturn Full( CCommand* io_cmd );
	static CReturn MaterialExtents( CCommand* io_cmd );
	static CReturn Window( CCommand* io_cmd );
	static CReturn Zoom( CCommand* io_cmd );
	static CReturn Pan( CCommand* io_cmd );
	static CReturn Rotate( CCommand* io_cmd );

	static CReturn Angle( CCommand* io_cmd );
	static CReturn Plane( CCommand* io_cmd );

	static CReturn RubberStart( CCommand* io_cmd );
	static CReturn RubberStop( CCommand* io_cmd );

	static CReturn Move( CCommand* io_cmd );
	static CReturn Pick( CCommand* io_cmd );

	static CReturn Highlight( CCommand* io_cmd );
	static CReturn Erase( CCommand* io_cmd );
	static CReturn Draw( CCommand* io_cmd );

	static CReturn Print( CCommand* io_cmd );

	static CReturn RGB_Value( CCommand* io_cmd );

	// Argh!  I can't take it anymore!  I added this!  I'm sorry!  eww
	static CViewBase*	ActiveView( void );

	// ---------------------------------------------------------------
	//		Non-command interfaces, for internal use
	//
	// Argh!
	static CReturn ClearEnable( bool enable );
	static CReturn ModelShowEnable( bool enable );

	static CReturn Clear( void );

	static CReturn	Refresh( bool regen );
	static CReturn	Refresh( ID in_id, bool in_marker );

	static CReturn	ModelSet( CCommand* io_cmd );
	static CReturn	ModelSet( CModel& model );

	void	mapScreenToWorld( const CPoint& in_screen, C3dCoord* io_view ) const;
	void	mapWorldToRef( const C3dCoord& world, C3dCoord* ref ) const;
	void	mapRefToWorld( const C3dCoord& ref, C3dCoord* world ) const;
	void	mapWorldToScreen( const C3dCoord& world, CPoint* io_screen ) const;
	void	mapViewToScreen( const C3dCoord& view, CPoint* io_screen ) const;

	void	DebugLine( C3dCoord& st, C3dCoord& en );
	void	RapidLine( C3dCoord& st, C3dCoord& en, int clr );

	static CReturn	Annotate( CString* text );

	static CglCanvas*	GetCanvas( void );
	static void			KillCanvas( void );


public:  // for debugging

	static CViewBase* ViewBaseGet();

	// Debugging
	static CWnd* getCWnd( void );

private:
	static CReturn view_transform( const C3x4Matrix& in_xform );

	static CViewBase* ViewFind( ID view_id );

	// Disabled.
	CViewMgr( const CViewMgr& );
	const CViewMgr& operator = ( const CViewMgr& );
	int operator == ( const CViewMgr& ) const;
	int operator != ( const CViewMgr& ) const;

private:
	static ID								m_next_id;

	static CRouteList						m_router;

	static CArray<CViewBase*, CViewBase*>	m_view_list;
	static CViewBase*						m_active_view;

	//
	// Debug view canvas
	//
	static CglCanvas*	m_debug_canvas;

	// Introduced for CI (to conditionally suppress graphics during nesting).
	static bool m_viewmgr_enabled;
};


class dllExport CGeoRenderer : public CBaseGeoRenderer
{
public:

	CGeoRenderer();
	virtual ~CGeoRenderer();

	void DisplayListOpen();
	void DisplayListClose();
	void Draw();

	eDisplayColor DrawColorSet( eDisplayColor color );

	void WindowSet( const C2dBox& box );

	void OriginSet( double x, double y, double z );
	void OriginSet( const C3dCoord& pt );

	virtual void DrawGeo( const CGeoElem& geo );
	virtual void DrawGeo( const CGeoElemList& list );
	virtual void DrawGeo( const CGeoElemArray& array );

private:

	bool m_open;
	eDisplayColor m_color;

	C3dCoord m_origin;

};

#endif

