//=============================================================================
// glCanvas
//
//	OpenGL Canvas management
//	Hacked so it is probably tied in too tightly with oglViewWire.  Sorry.
//
//=============================================================================

#if !defined(_GLCANVAS_H)
#define _GLCANVAS_H
#pragma once

#include "GL/gl.h"
#include "GL/glu.h"

#include "common.h"
#include "2dCoord.h"
#include "2dBox.h"
#include "DbEntity.h"
#include "DisplayEntity.h"


//=============================================================================

class dllExport CglCanvas
{
public:

	CglCanvas( CWnd* output );

	~CglCanvas();

	CWnd* Window() const
		{ return m_output; }

	void Init();
	void LoadFont( CString name, int height );

	CRect Viewport();
	void Viewport( const CRect& screen, const C2dBox& world );

	const C2dBox& ViewExtent()
		{ return m_view_extent; }

	double ViewScale() const
		{ return m_scale; }

	const C2dCoord& ViewCenter()
		{ return m_view_center; }

	void ClearColor( COLORREF color );
	COLORREF ClearColor() const
		{ return m_clear_color; }

	bool Start();

	void BackBuffer( bool activate );
	void Show( GLint buffer );

	void ClearEnable( bool enable )
		{ m_clear_enabled = enable; }

	void ModelShowEnable( bool enable )
		{ m_model_enabled = enable; }

	void Color( COLORREF color );
	void Style( eDisplayStyle style );

	void WMF( bool on );
	void WMF_file( CString filepath );

	bool Fill();
	void Fill( bool enable );

	void QuadOpen();
	void Quad( const C2dCoord& p1, const C2dCoord& p2, const C2dCoord& p3, const C2dCoord& p4 );

	void LineOpen();
	void Line( const C2dCoord& p1, const C2dCoord& p2 );
	void Line( double x1, double y1, double x2, double y2 );

	void PolylineOpen( bool closed );

	void LineTo( double x2, double y2 )
		{ Vertex( x2, y2 ); }

	void FanOpen();

	void ModelBegin();
	void ModelEnd();
	bool IsModelActive() const;

	void SemiTempBegin();
	void SemiTempEnd();
	void SemiTempDelta( const C3dCoord& delta );

	void PureTempBegin();
	void PureTempEnd();
	bool IsTemporaryActive() const;

	void MoveTo( const C2dCoord& pt );
	void MoveTo( double x1, double y1 );

	void Vertex( double x2, double y2 );

	void Close();

	void PatternDraw( const C2dCoord& place );

	void Print( CString text, eDisplayTextPos pos, double angle, int size );

	double TextOWidth( const CString& text );
	double TextOHeight( const CString& text );
	C2dCoord TextOSize( const CString& text );
	CSize TextBSize( const CString& text );
	C2dCoord TextBSize( const CString& text, double scale );

	void OglReport();

	void Pan( const C2dVec& delta );
	void Scale( double factor, const C2dCoord& pt );

private:

	void EnableOpenGL( CDC* dc );

	void Palette();

	void load_font();

	void print_b( CString text, eDisplayTextPos pos );
	void print_o( CString text, eDisplayTextPos pos, double angle, double size );

	void moveto( CDC* dc, const C3dCoord& end );
	void lineto( CDC* dc, const C3dCoord& end );

	void error();
	void silent_error();

private:

	CWnd*		m_output;

	HGLRC		m_glrc;
	CPalette	m_palette;
	LOGFONT		m_fontdata;

	GLYPHMETRICSFLOAT	m_txtglyph[256];

	bool		m_open;


	// Track whether we are writing to a gl buffer.
	bool	m_model_active;
	bool	m_temp_active;

	// Argh!
	bool	m_clear_enabled;
	bool	m_model_enabled;

	// GDI stuff to track.
	bool	m_wmf;
	CString	m_wmf_file;

	// OGL stuff to track.
	GLdouble	m_model_mat[16];
	GLdouble	m_proj_mat[16];
	GLint		m_viewport[4];

	// Patterns (display lists)
	GLuint	m_glLists;
	GLuint	m_model_list;  // eg. model entities.
	GLuint	m_semi_list;   // eg. nesting kerf
	GLuint	m_pure_list;   // eg. rubberband

	GLUtesselator*	m_tess;
	bool			m_fill;

	// Generics?
	CMetaFileDC*	m_mfdc;

	int			m_font_height;

	C2dCoord	m_at;

	COLORREF	m_clear_color;

	C2dBox		m_view_extent;
	C2dCoord	m_view_center;
	C2dVec		m_delta;
	double		m_scale;

	C3dVec		m_semi_delta;  // (x,y,theta)
};
#endif