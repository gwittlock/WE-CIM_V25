
#pragma once

#include "GeoArc.h"
#include "Nibbler.h"

#include "ViewBase.h"
#include "AnimateDialog.h"

#include "3dCoord.h"
#include "3x4Matrix.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class CViewBogus : public CViewBase
{
public:

	CViewBogus() : CViewBase(0)  { /* do nothing */ }
	virtual ~CViewBogus()  { /* do nothing */ }

	virtual CReturn Create( HDC in_dc )  { return STATUS_OKAY; }
	virtual CReturn Create( HWND in_hwnd )  { return STATUS_OKAY; }

	virtual void ModelSet( CModel& in_model )  { /* do nothing */ }

	virtual void Resize()  { /* do nothing */ }
	virtual void Clear( EDisplayList which )  { /* do nothing */ }

	virtual void DisplayListBegin( EDisplayList which )  { /* do nothing */ }
	virtual void DisplayListEnd( EDisplayList which )  { /* do nothing */ }

	virtual void BufferShow( EViewBuffer which )  { /* do nothing */ }

	// Argh!
	virtual void ClearEnable( bool enable )  { /* do nothing */ }

	virtual void ModelShowEnable( bool enable )  { /* do nothing */ }

	virtual void Color( eColor idx, int rgb )  { /* do nothing */ }
	virtual int Color( eColor idx )  { return 0; }

	virtual void Refresh( bool regen )  { /* do nothing */ }
	virtual void Refresh( ID in_id, bool in_marker )  { /* do nothing */ }

	virtual void Export( FILE* f )  { /* do nothing */ }

	virtual void Transform( void )  { /* do nothing */ }
	virtual void Transform( ID in_id )  { /* do nothing */ }

	virtual void Print( const CModel& in_model, bool quiet, bool wmf, CString file )  { /* do nothing */ }

	virtual CReturn	Move( const C3dCoord& in_mouse, bool snap, CCommand* io_cmd )  {  return STATUS_OKAY;  }
	virtual CReturn	Pick( const C3dCoord& in_mouse, CCommand* io_cmd )  {  return STATUS_OKAY;  }

	virtual CReturn	Highlight( ID in_id )  {  return STATUS_OKAY;  }
	virtual CReturn	Erase( ID in_id )  {  return STATUS_OKAY;  }
	virtual CReturn	Draw( ID in_id )  {  return STATUS_OKAY;  }

	virtual CReturn	Save( bool reset )  {  return STATUS_OKAY;  }
	virtual CReturn	Next( const CModel& in_model )  {  return STATUS_OKAY;  }
	virtual CReturn	Prev( const CModel& in_model )  {  return STATUS_OKAY;  }

	virtual CReturn	Full( const CModel& in_model, bool regen )  {  return STATUS_OKAY;  }
	virtual CReturn MaterialExtents( const CModel& model, bool regen )  {  return STATUS_OKAY;  }
	virtual CReturn	Window( const C2dCoord& in_one, const C2dCoord& in_two )  {  return STATUS_OKAY;  }

	C2dBox AdjustedExtent( double xll, double yll, double xur, double yur )  { /* do nothing */ }

	virtual CReturn	Pan( const CModel& model, const C2dCoord& in_one, const C2dCoord& in_two )  {  return STATUS_OKAY;  }
	virtual CReturn	Zoom( const CModel& model, const C2dCoord world, double factor )  {  return STATUS_OKAY;  }
	virtual CReturn	Rotate( const CModel& model, const C2dCoord& in_one, const C2dCoord& in_two )  {  return STATUS_OKAY;  }

	virtual CReturn	Angle( const CModel& in_model, double in_xy, double in_yz, double in_zx )  {  return STATUS_OKAY;  }
	virtual CReturn	Plane( const CModel& in_model, ID in_plane_id )  {  return STATUS_OKAY;  }

	virtual void RubberStart( eRubber in_type, const C3dCoord& in_mouse, const CModel& in_model )  { /* do nothing */ }
	virtual void RubberStop( void )  { /* do nothing */ }

	virtual void mapRefToWorld( const C3dVec& in_ref, const CDbWorkplane* in_src, C3dVec* io_world ) const  { /* do nothing */ }
	virtual void mapWorldToView( const C3dVec& in_world, C3dVec* io_view ) const  { /* do nothing */ }

	virtual void mapRefToWorld( const C3dCoord& in_ref, const CDbWorkplane* in_src, C3dCoord* io_world ) const  { /* do nothing */ }
	virtual void mapWorldToView( const C3dCoord& in_world, C3dCoord* io_view ) const  { /* do nothing */ }
	virtual void mapViewToScreen( const C3dCoord& in_view, CPoint* io_screen ) const  { /* do nothing */ }

	virtual void mapScreenToView( const CPoint& in_screen, C3dCoord* io_view ) const  { /* do nothing */ }
	virtual void mapViewToWorld( const C3dCoord& in_view, C3dCoord* io_world ) const  { /* do nothing */ }
	virtual void mapWorldToRef( const C3dCoord& in_world, const CDbWorkplane* in_dest, C3dCoord* io_ref ) const  { /* do nothing */ }

	virtual void mapScreenToWorld( const CPoint& in_screen, C3dCoord* io_world ) const  { /* do nothing */ }
	virtual void mapWorldToScreen( const C3dCoord& in_world, CPoint* io_screen ) const  { /* do nothing */ }

	virtual void projectViewToRef( const C3dCoord& in_view, const CDbWorkplane& in_ref, C3dCoord* io_world) const  { /* do nothing */ }

	virtual void XorEnable( bool enable )  { /* do nothing */ }

	virtual void DebugLine( C3dCoord& st, C3dCoord& en )  { /* do nothing */ }
	virtual void RapidLine( C3dCoord& st, C3dCoord& en, int clr )  { /* do nothing */ }

	virtual void DrawAtColor( eDisplayColor color )  { /* do nothing */ }
	virtual void DrawAtStyle( eDisplayStyle line_style )  { /* do nothing */ }

	virtual void DrawCurve( const CGeoCurve* curve, int draw)  { /* do nothing */ }
	virtual void DrawGeoAt( const CGeoElem& geo, const C3dCoord& delta )  { /* do nothing */ }

	virtual CReturn	Annotate( CString*text )  { return STATUS_OKAY; }

	// virtual void Print( const CModel& model, bool quiet, bool wmf, CString file )  { /* do nothing */ }
	void PrintWMF( const CModel& model, CString file )  { /* do nothing */ }

	virtual CModel* ActiveModel() const	{ return NULL; }

	virtual void dot( const C3dCoord& pt, bool hot )  { /* do nothing */ }

	double Scale()  { return 1.; }

	virtual COLORREF BackGroundColor() const  { return 0; }

	virtual void DrawDirect( CDisplayEntity* disp_ent )  { /* do nothing */ }

	virtual void CenterPt( double* xc, double* yc )  { /* do nothing */ }

	virtual void InstanceDraw( CDbCommand* dbInstance )  { /* do nothing */ }
	virtual void DrawPatternAt( CDbPattern* dbPattern, C3x4Matrix* xform )  { /* do nothing */ }
	virtual void SemiTempDelta( const C3dCoord& C3dCoord )  { /* do nothing */ }

private:

	// Disabled.
	CViewBogus( const CViewBogus& );
	const CViewBogus& operator = ( const CViewBogus& );
	int operator == ( const CViewBogus& ) const;
	int operator != ( const CViewBogus& ) const;
};

