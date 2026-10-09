//=============================================================================
// glCanvas
//
//	OpenGL Canvas management
//
//	Hacked so it is probably tied in too tightly with oglViewWire.  Sorry.
//
//=============================================================================

#include "stdafx.h"

#include "common.h"
#include "Register.h"
#include "2dUnitVec.h"

#include "glCanvas.h"

CReturn g_viewDiagnostic;

PIXELFORMATDESCRIPTOR g_bestMatch_pfd;

//=============================================================================

GLuint OFONT_LIST_BASE = 0;  // base of outline font display lists
GLuint BFONT_LIST_BASE = 0;  // base of bitmap font display lists


// Because ogl scales values [0..1]
float red( COLORREF color )
	{ return (float) ((color & 0xff) / 255.); }

float green( COLORREF color)
	{ return (float) (((color >> 8) & 0xff) / 255.); }

float blue( COLORREF color )
	{ return (float) (((color >> 16) & 0xff) / 255.); }


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Support items for OpenGL polygon tesselation.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

static double COINCIDENT_TOL = (SMALL * SMALL);

// We cache the color to minimize memory requirements.
// NOTE: All of the examples that I found illustrating polygon tesselation
// used an array of 6 double to represent each vertex of the polygon. The
// first three elements represented the coordinate and the last three
// elements represented the color components. In some cases, this was done
// because the polygon was rendered using color-averaging via a user-defined
// callback combineCallback(). We do not have that requirement because each
// polygon is drawn using a solid color.
static COLORREF g_curr_color;
static GLdouble g_poly_color[3] = { 0., 0., 0 };

// After much struggling, I discovered the polygon data must be persistent
// for the life of the tesselation process. We could cache this data in
// other ways (but still allow dynamic amounts of data). However, I have
// chosen to use a fixed array so as to reduce memory thrashing.
const int MAX_POLY = 1024;
static GLdouble g_poly[MAX_POLY][3];
static int g_npoly = 0;

static GLvoid CALLBACK beginCallback( GLenum which );
static GLvoid CALLBACK endCallback();
static GLvoid CALLBACK vertexCallback( GLvoid* vertex );
static GLvoid CALLBACK errorCallback( GLenum errorCode );


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CglCanvas::CglCanvas( CWnd* output )//CDC* context )
	: m_output( output ),
	  m_glrc( NULL ),
	  m_scale( 1 ),
	  m_open( false ),
	  m_font_height( 0 ),
	  m_mfdc( NULL ),
	  m_wmf( false ),
	  m_wmf_file( "printout.wmf" )
{

	m_view_extent.Update( 0, 0, 0, 0 );
	m_view_center.XY( m_view_extent.Xc(), m_view_extent.Yc() );
	m_delta.Init( 0, 0 );
	m_scale = 1.;

	m_tess = gluNewTess();
	// Set callback functions
	//   gluTessCallback( m_tess, GLU_TESS_VERTEX, (GLvoid (*) ()) &vertexCallback );
	gluTessCallback( m_tess, GLU_TESS_VERTEX, (GLvoid (CALLBACK*) ()) &vertexCallback );
	gluTessCallback( m_tess, GLU_TESS_BEGIN, (GLvoid (CALLBACK*) ()) &beginCallback );
	gluTessCallback( m_tess, GLU_TESS_END, (GLvoid (CALLBACK*) ()) &endCallback );
	//   gluTessCallback( m_tess, GLU_TESS_COMBINE, (GLvoid (*) ())&combineCallback );
	gluTessCallback( m_tess, GLU_TESS_ERROR, (GLvoid (CALLBACK*) ()) &errorCallback );

	m_fill = false;
	m_clear_enabled = true;
	m_model_enabled = true;
}

CglCanvas::~CglCanvas( void )
{
	gluDeleteTess( m_tess );

	if (m_mfdc)
	{ 
		HENHMETAFILE mfh = m_mfdc->CloseEnhanced();
		::DeleteEnhMetaFile( mfh );

		// m_output->ReleaseDC( m_mfdc );
	}

	if (m_glrc)
	{
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( m_glrc );
	}
	m_palette.DeleteObject();

	m_model_active = false;
	m_temp_active = false;
}

//=============================================================================

void
CglCanvas::Init()
{
	CDC* dc;

	if (m_wmf)
	{
		m_mfdc = new CMetaFileDC();
		CString name = m_wmf_file; 
		if (name.GetLength() < 2)
		{
			name.Format( "printout.wmf" );
		}

		// bool good = m_mfdc->CreateEnhanced( NULL, name, NULL, "WE-CIM" );
		BOOL good = m_mfdc->CreateEnhanced( NULL, name, NULL, NULL );

		if (CReturn::Debug()>=1)
		{
			if (!good)
			{
				CReturn ret;
				ret.Diagnostic( "ERROR creating WMF 'printout.wmf'" );
				ret.SystemError();
			}
			else
			{
				CReturn ret;
				ret.Diagnostic( "Created WMF 'printout.wmf'" );
			}
		}
	}

	// Does this fix our printing woes?
	dc = (m_wmf ? m_mfdc : m_output->GetDC());
	if (m_wmf)
		dc->SetMapMode(MM_TEXT); 

	EnableOpenGL( dc );
	Palette();  // do we need this?

	m_glrc = wglCreateContext( dc->m_hDC );
	wglMakeCurrent( dc->m_hDC, m_glrc );

	// ------------

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glEnable( GL_LINE_STIPPLE );
	glHint( GL_LINE_SMOOTH_HINT, GL_FASTEST );
	glDisable( GL_LINE_SMOOTH );

	glHint( GL_POINT_SMOOTH_HINT, GL_FASTEST );
	glDisable( GL_POINT_SMOOTH );

	glHint( GL_POLYGON_SMOOTH_HINT, GL_FASTEST );
	glDisable( GL_POLYGON_SMOOTH );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glShadeModel( GL_FLAT );

	glDisable( GL_ALPHA_TEST );
	glDisable( GL_BLEND );

	// ------------

	m_glLists = glGenLists(3);

	m_model_list = m_glLists;
	m_semi_list = m_model_list + 1;
	m_pure_list = m_model_list + 2;

	m_model_active = false;
	m_temp_active = false;

	if (!m_wmf)
		ReleaseDC(m_output->m_hWnd, dc->m_hDC);
}

// NOTE: Useful data at:
//   http://www.opengl.org/resources/code/rendering/mjktips/win32/pixlfrmt.html
//   http://www.opengl.org/resources/faq/technical/mswindows.htm
void
CglCanvas::EnableOpenGL( CDC* dc )
{
	PIXELFORMATDESCRIPTOR pfd;
	int format;

	// --- wecim stuff ---
	CReturn note;
	CString str;

	CString key = "Preferences\\ViewOptions";

	bool double_buffer = CRegister::BoolGetV( key, "DoubleBuffer", true );

	// An attempt to eliminate the dreaded "black blob".
	bool gdi_mode = CRegister::BoolGetV( key, "GDI", false );
	bool accelerated = CRegister::BoolGetV( key, "Accelerated", true );

	// --- wecim stuff ---

	
	HDC hDC = dc->m_hDC;
	
	// set the pixel format for the DC - first make sure everything is cleared
	ZeroMemory( &pfd, sizeof( pfd ) );

	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	// pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.dwFlags = (PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL);
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	// --- wecim stuff ---
	// PFD_GENERIC_FORMAT = 64 -- The pixel format is supported by the GDI
	//   software implementation, which is also known as the generic implementation.
	//   If this bit is clear, the pixel format is supported by a device driver or
	//   hardware.
	//
	// PFD_GENERIC_ACCELERATED = 4096 -- The pixel format is supported by a device
	//   driver that accelerates the generic implementation. If this flag is clear
	//   and the PFD_GENERIC_FORMAT flag is set, the pixel format is supported by
	//   the generic implementation only.
	//
	// From http://www.opengl.org/resources/faq/technical/mswindows.htm
	//   5.040 How do I know my program is using hardware acceleration on a Wintel card?
	//   OpenGL doesn't provide a direct query to determine hardware acceleration usage.
	//   However, this can usually be inferred by using indirect methods. If you are
	//   using the Win32 interface (as opposed to GLUT), call DescribePixelFormat()
	//   and check the returned dwFlags bitfield. If PFD_GENERIC_ACCELERATED is clear
	//   and PFD_GENERIC_FORMAT is set, then the pixel format is only supported by the
	//   generic implementation. Hardware acceleration is not possible for this format.
	//   For hardware acceleration, you need to choose a different format.
	//
	//   If glGetString(GL_VENDOR) returns something other than "Microsoft Corporation",
	//   it means you're using the board's ICD. If it returns "Microsoft Corporation",
	//   this implies you chose a pixel format that your device can't accelerate.
	//   However, glGetString(GL_VENDOR) also returns this if your device has an MCD
	//   instead of an ICD, which means you might still be hardware accelerated in this
	//   case.
	//
	if (gdi_mode)
	{
		// When no other choice ...
		pfd.dwFlags |= PFD_SUPPORT_GDI;
	}
	else
	{
		if (double_buffer)
		{
			pfd.dwFlags |= PFD_DOUBLEBUFFER;
		}
		else
		{
			// Disable double buffering for Intel graphics cards because
			// the graphics will flash as you move the mouse.
		}
	}

	// NOTE: No reference to these constants appear in the SmartCAM implementation.
	// pfd.dwFlags |= ((accelerated) ? PFD_GENERIC_ACCELERATED : PFD_GENERIC_FORMAT);
	// --- wecim stuff ---

	format = ChoosePixelFormat( hDC, &pfd );
	if (format == 0)
	{
		// notice_error( rsrc_getstr( "GlRendPixErr" ), NULL );
		return;
	}

	DescribePixelFormat( hDC, format, sizeof(pfd), &g_bestMatch_pfd );

	if( !(g_bestMatch_pfd.dwFlags & PFD_SUPPORT_OPENGL) || !(g_bestMatch_pfd.dwFlags & PFD_DOUBLEBUFFER) )
	{
		// notice_error( rsrc_getstr( "GlRendPixNone" ), NULL );
		return;
	}

	if ( (g_bestMatch_pfd.dwFlags & PFD_NEED_PALETTE) )
	{
		// notice_error( rsrc_getstr( "GlRendNoPalet" ), NULL );
		return;
	}
	
	// bool res = SetPixelFormat( hDC, format, &pfd );
	BOOL res = SetPixelFormat( hDC, format, &g_bestMatch_pfd );
	if (res == 0)
	{
		note.User( IDS_INTERNAL_ERROR, "SetPixelFormat() failed" );
	}
}

//=============================================================================

void
CglCanvas::Palette( void )
{
	CDC* dc = m_output->GetDC();
	int pixelFormat = GetPixelFormat( dc->m_hDC );
	PIXELFORMATDESCRIPTOR pfd;
	LOGPALETTE* pPal;
	int paletteSize;

	DescribePixelFormat( dc->m_hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd );

	if (pfd.dwFlags & PFD_NEED_PALETTE)
		paletteSize = 1 << pfd.cColorBits;
	else
		return;


	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) + paletteSize * sizeof(PALETTEENTRY) );
	pPal->palVersion = 0x300;
	pPal->palNumEntries = paletteSize;

	// Build a simple RGB palette
	{
		int redMask = (1 << pfd.cRedBits) - 1;
		int greenMask = (1 << pfd.cGreenBits) - 1;
		int blueMask = (1 << pfd.cBlueBits) - 1;
		int idx;

		for (idx=0; idx<paletteSize; idx++)
		{
			pPal->palPalEntry[idx].peRed = (((idx >> pfd.cRedShift) & redMask) * 255) / redMask;
			pPal->palPalEntry[idx].peGreen = (((idx >> pfd.cGreenShift) & greenMask) * 255) / greenMask;
			pPal->palPalEntry[idx].peBlue = (((idx >> pfd.cBlueShift) & blueMask) * 255) / blueMask;
		}
	}


	if (m_palette.CreatePalette( pPal ))
	{
		dc->SelectPalette( &m_palette, FALSE );
		dc->RealizePalette();
	}
	else
		error();

	if (dc)
		ReleaseDC(m_output->m_hWnd, dc->m_hDC);

	free( pPal );
}


CRect
CglCanvas::Viewport()
{
	GLint maxdim[2];
	glGetIntegerv( GL_MAX_VIEWPORT_DIMS, maxdim );

	return CRect( 0, 0, maxdim[0], maxdim[1] );
}

void
CglCanvas::Viewport( const CRect& screen, const C2dBox& world )
{
	m_view_extent = world;
	m_view_center.XY( m_view_extent.Xc(), m_view_extent.Yc() );

	// Otherwise, m_scale becomes an undefined number and
	// then the application hangs when rendering arcs.
	if ((m_view_extent.Dx() > SMALL) && (m_view_extent.Dy() > SMALL))
	{
		glViewport( 0, 0, screen.Width(), screen.Height() );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();

		glOrtho( m_view_extent.Xmin(), m_view_extent.Xmax(),
				m_view_extent.Ymin(), m_view_extent.Ymax(),
				-1.0, 1.0 );

		glGetDoublev(GL_MODELVIEW_MATRIX, m_model_mat );
		glGetDoublev(GL_PROJECTION_MATRIX, m_proj_mat );
		glGetIntegerv(GL_VIEWPORT, m_viewport );

		m_scale = min( (double)screen.Width() / m_view_extent.Dx(),
						(double)screen.Height() / m_view_extent.Dy() );
	}
}

//=============================================================================

void
CglCanvas::ClearColor( COLORREF color )
{
	if (m_open)
		Close();

	// Clear color and depth buffer
	glClearColor( red(color), green(color), blue(color), 0. );
	m_clear_color = color;
}

//=============================================================================

bool CglCanvas::Start()
{
	bool success = false;  // assume failure

	CDC* dc = m_output->GetDC();
	if (dc != NULL)
	{
		if (m_open)
			Close();

		m_output->UnlockWindowUpdate();

		wglMakeCurrent( dc->m_hDC, m_glrc );

		CRect screen;
		m_output->GetClientRect( &screen );
		glViewport( screen.left, screen.top, screen.Width(), screen.Height() );

		if (dc)
			ReleaseDC(m_output->m_hWnd, dc->m_hDC);

		success = true;
	}

	return success;
}

void
CglCanvas::BackBuffer( bool activate )
{
	glDrawBuffer( (activate ? GL_BACK : GL_FRONT) );
}

/*
http://www.opengl.org/resources/faq/technical/miscellaneous.htm

24.130 What's the difference between glFlush() and glFinish() and
why would I want to use these routines?

The OpenGL spec allows an implementation to store commands and data
in buffers, which are awaiting execution. glFlush() causes these buffers
to be emptied and executed. Thus, any pending rendering commands will be
executed, but glFlush() may return before their execution is complete.
glFinish() instructs an implementation to not return until the effects of
all commands are executed and updated.

A typical use of glFlush() might be to ensure rendering commands are exected
when rendering to the front buffer.

glFinish() might be particularly useful if an app draws using both OpenGL
and the window system's drawing commands. Such an application would first
draw OpenGL, then call glFinish() before proceeding to issue the window
system's drawing commands.
*/
void
CglCanvas::Show( GLint buffer )
{
	if (m_open)
		Close();

	glDrawBuffer( buffer );  // GL_BACK, GL_FRONT

	// 2011.02.06 (PE)
	// TODO: Argh! I just don't understand why/how these next calls
	// are necessary. Without these calls (particularly GL_PROJECTION),
	// if the model contains rotated command text, all of the other model
	// geometry is drawn in a tiny space in the lower left corner as if
	// there were a scaling problem.
	//
	// Aha!. The opengl documentation says:
	//   glOrtho() describes a transformation that produces a parallel projection.
    //   The current matrix (see glMatrixMode) is multiplied by this matrix and
    //   the result replaces the current matrix, as if glMultMatrix were called
	//   with the following matrix as its argument ....
	//
	//   Typically, the matrix mode is GL_PROJECTION, and (left,bottom,-nearVal)
	//   and (right,top,-nearVal) specify the points on the near clipping plane
	//   that are mapped to the lower left and upper right corners of the window,
    //   respectively, assuming that the eye is located at (0, 0, 0). -farVal
	//   specifies the location of the far clipping plane. Both nearVal and farVal
	//   can be either positive or negative.
	//
	//   See also http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
        
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	// 2008.07.06 (PE) -- I suppose the traditional way of
	// managing the view transformation is to apply the
	// following functions.  However, I discovered that
	// using glOrtho() is sufficient for our purposes, and
	// may actually be easier to use.
	//    glScaled( m_scale, m_scale, 1. );
	//    glTranslated( m_delta.X(), m_delta.Y(), 0. );

	glOrtho( m_view_extent.Xmin(), m_view_extent.Xmax(),
		m_view_extent.Ymin(), m_view_extent.Ymax(),
		-1.0, 1.0 );

	// Give us a clean slate as necessary.
	if ( m_clear_enabled )
		glClear( GL_COLOR_BUFFER_BIT );

	if (buffer == GL_FRONT)
	{
		// 2009.10.06 (PE) -- Code Previewer was not maintaining
		// highlight color because the model was being redrawn.
		if ( m_model_enabled )
			glCallList( m_model_list );

		// Draw temporary entities like parts while nesting is active.
		glCallList( m_pure_list );
	
		// This branch is intended for temporary
		// graphics like entity highlighting.
		// NOTE: These graphics are lost when
		// the window is moved/resized.
		glTranslated( m_semi_delta.X(), m_semi_delta.Y(), 0. );
		glRotated( (m_semi_delta.Z() * RAD2DEG), 0.0, 0.0, 1.0 );
		glCallList( m_semi_list );
		glRotated( -(m_semi_delta.Z() * RAD2DEG), 0.0, 0.0, 1.0 );
		glTranslated( -m_semi_delta.X(), -m_semi_delta.Y(), 0. );

		glFinish();
	}
	else
	{
		// This is the default branch.

		// Draw the model entities.
		glCallList( m_model_list );

		// Draw temporary entities like nesting kerf, etc.
		glCallList( m_semi_list );

		// Draw temporary entities like rubberband, etc.
		glCallList( m_pure_list );

		glFinish();

		CDC* dc = m_output->GetDC();

		SwapBuffers( dc->GetSafeHdc() );

		if (dc)
			ReleaseDC(m_output->m_hWnd, dc->m_hDC);
	}
}

void
CglCanvas::Pan( const C2dVec& delta )
{
	m_view_extent.Shift( delta.X(), delta.Y() );
	m_view_center.XY( m_view_extent.Xc(), m_view_extent.Yc() );
	m_delta += delta;
}

void
CglCanvas::Scale( double factor, const C2dCoord& pt )
{
	C2dCoord screen_pc;
	screen_pc.X( (int) (0.5 * m_viewport[2]) );
	screen_pc.Y( (int) (0.5 * m_viewport[3]) );

	C2dCoord bl( m_view_extent.BL() );
	C2dCoord tr( m_view_extent.TR() );

	// Map the reference point to the screen.
	C2dCoord screen_pt;
	screen_pt.X( (int) ((pt.X() - m_view_center.X()) * m_scale) + screen_pc.X() );
	screen_pt.Y( (int) ((pt.Y() - m_view_center.Y()) * m_scale) + screen_pc.Y() );

	// Shift to the origin (0,0) and scale about it.
	C3x4Matrix xform;
	xform.Shift( C3dVec( -pt.X(), -pt.Y(), 0. ) );
	xform.Scale( factor, factor, 1. );

	xform.Transform( &bl );
	xform.Transform( &tr );

	// Shift back to the reference point.
	xform.setUnit();
	xform.Shift( C3dVec( pt.X(), pt.Y(), 0. ) );

	xform.Transform( &bl );
	xform.Transform( &tr );

	m_view_extent.Update( bl.X(), bl.Y(), tr.X(), tr.Y() );
	m_view_center.XY( m_view_extent.Xc(), m_view_extent.Yc() );
	m_scale /= factor;

	// Map the screen point back to world.
	C2dCoord pt2;
	pt2.X( ((screen_pt.X() - screen_pc.X()) / m_scale) + m_view_center.X() );
	pt2.Y( ((screen_pt.Y() - screen_pc.Y()) / m_scale) + m_view_center.Y() );

	// Shift everything so the reference point is
	// back in its original position on the screen.
	Pan( pt - pt2 );
}


//=============================================================================

void
CglCanvas::Color( COLORREF color )
{
	if (m_open)
		Close();

	g_curr_color = color;
	glColor3d( red(color), green(color), blue(color) );
}

void
CglCanvas::Style( eDisplayStyle	disp_style )
{
	if (m_open)
		Close();

	switch (disp_style)
	{
	case DSTYLE_SOLID:
		glLineStipple( 1, 0xffff );
		break;

	case DSTYLE_SELECT:
		glLineStipple( 2, 0xAAAA );
		break;

	case DSTYLE_CENTER:
	case DSTYLE_DASH:
		glLineStipple( 6, 0xAAAA );
		break;

	case DSTYLE_TOOL:
		glLineStipple( 3, 0xd6d6 );
		break;
	}
}

void CglCanvas::WMF( bool on )
{
	m_wmf = on;
}


//=============================================================================

void
CglCanvas::QuadOpen( void )
{
	if (m_open)
		Close();

	glBegin( GL_QUADS );
	m_open = TRUE;
}

void
CglCanvas::Quad( 
	const C2dCoord& p1, 
	const C2dCoord& p2, 
	const C2dCoord& p3, 
	const C2dCoord& p4 )
{
	glVertex2d( p1.X(), p1.Y() );
	glVertex2d( p2.X(), p2.Y() );
	glVertex2d( p3.X(), p3.Y() );
	glVertex2d( p4.X(), p4.Y() );
}

//=============================================================================

void
CglCanvas::FanOpen( void )
{
	if (m_open)
		Close();

	glBegin( GL_TRIANGLE_FAN );
	m_open = TRUE;
}

//=============================================================================

void
CglCanvas::LineOpen( void )
{
	if (m_open)
		Close();

	glBegin( GL_LINES );
	m_open = TRUE;
}

void
CglCanvas::Line( const C2dCoord& p1, const C2dCoord& p2 )
{
	glVertex2d( p1.X(), p1.Y() );
	glVertex2d( p2.X(), p2.Y() );
}

void
CglCanvas::Line( 
	double x1, double y1,
	double x2, double y2 )
{
	C2dCoord p1( x1, y1 );
	C2dCoord p2( x2, y2 );

	Line( p1, p2 );
}


void
CglCanvas::PolylineOpen( bool closed )
{
	if (m_open)
		Close();

	glBegin( (closed ? GL_LINE_LOOP : GL_LINE_STRIP) );
	m_open = TRUE;
}

void
CglCanvas::MoveTo( double x1, double y1 )
{
	m_at = C2dCoord( x1, y1 );
	MoveTo( m_at );
}

void
CglCanvas::MoveTo( const C2dCoord& pt )
{
	m_at = pt;
}

void
CglCanvas::Vertex( double x2, double y2 )
{
	if ( m_fill )
	{
		bool okay = (g_npoly < MAX_POLY);
		if ( okay )
		{
			okay = (g_npoly == 0);
			if ( !okay )
			{
				// Polygon tesselation does not appear to be sensitive to coincident
				// points but, regardless, we prevent them from accumulating.
				double dx = x2 - g_poly[g_npoly-1][0];
				double dy = y2 - g_poly[g_npoly-1][1];
				okay = ((dx*dx + dy*dy) >= COINCIDENT_TOL);
			}

			if ( okay )
			{
				g_poly[g_npoly][0] = x2;
				g_poly[g_npoly][1] = y2;
				g_poly[g_npoly][2] = 0.;
				++g_npoly;
			}
		}
	}
	else
	{
		glVertex2d( x2, y2 );
	}
}

void
CglCanvas::Close( void )
{
	if (!m_open)
	{
		MessageBox( NULL, "Closing a context without opening.", NULL, MB_OK );
	}

	glEnd();
	silent_error();
	m_open = FALSE;
}

bool
CglCanvas::Fill()
{
	return m_fill;
}

void
CglCanvas::Fill( bool enable )
{
	m_fill = enable;
	if ( m_fill )
	{
		// ASSUMPTION: Tool shapes have a CW winding direction.
		gluTessProperty( m_tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE );
		gluTessBeginPolygon( m_tess, NULL );
		gluTessBeginContour( m_tess );
	}
	else
	{
		for (int indx = 0; indx < g_npoly; ++indx)
		{
			gluTessVertex( m_tess, g_poly[indx], g_poly[indx] );
		}

		gluTessEndContour( m_tess );
		gluTessEndPolygon( m_tess );

		g_npoly = 0;
	}
}

void
CglCanvas::LoadFont( CString name, int height )
{
	m_font_height = height;

	CDC* dc = m_output->GetDC();

	LOGFONT lf;
#if ORIGINAL_CODE
	memset( &lf, 0, sizeof(lf) );
	lstrcpy( lf.lfFaceName, name );

	lf.lfHeight = -MulDiv(height, dc->GetDeviceCaps(LOGPIXELSY), 72);
	lf.lfWeight = FW_BOLD;
#else
	lf.lfHeight = -MulDiv(height, dc->GetDeviceCaps(LOGPIXELSY), 72);
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = lf.lfEscapement;
    // lf.lfWeight = FW_NORMAL;
	lf.lfWeight = FW_HEAVY;  // looks cleaner (more uniform) wrt rotated text
    lf.lfItalic = FALSE;
    lf.lfUnderline = FALSE;
    lf.lfStrikeOut = FALSE;
    lf.lfCharSet = ANSI_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	// lf.lfOutPrecision = OUT_DEVICE_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
	// lf.lfQuality = PROOF_QUALITY;
    lf.lfPitchAndFamily = FF_DONTCARE|DEFAULT_PITCH;
    lstrcpy(lf.lfFaceName, name);
#endif

	CFont* font = new CFont();
	font->CreateFontIndirect( &lf );

	CFont* old_font = dc->SelectObject( font );

	OFONT_LIST_BASE = glGenLists(255);

	wglUseFontOutlines( 
						dc->m_hDC, 
						0, 
						255, 
						OFONT_LIST_BASE, 
						0.0f, 
						0.0f, 
						WGL_FONT_POLYGONS,
						m_txtglyph );

	BFONT_LIST_BASE = glGenLists(255);

	wglUseFontBitmaps(
						dc->m_hDC,
						0,
						255,
						BFONT_LIST_BASE );

	dc->SelectObject( old_font );

	if (dc)
		ReleaseDC(m_output->m_hWnd, dc->m_hDC);

	delete font;
}

void
CglCanvas::Print( 
	CString			text, 
	eDisplayTextPos	pos, 
	double			angle,
	int				size )
{
	if (m_open)
		Close();

// Why bother differentiating except that the bitmap font looks a
// little cleaner. This is most noticeable wrt a nested sheet's legend.
#if ORIGINAL_CODE
	if ( ZERO(angle) )
		print_b( text, pos  );
	else
		print_o( text, pos, angle );
#else
	print_o( text, pos, angle, size );
#endif
}

void
CglCanvas::print_b( CString text, eDisplayTextPos pos )
{
	CDC* dc = m_output->GetDC();

	glListBase( BFONT_LIST_BASE );

	// Break the text down into individual lines
	CStringArray	txt_array;
	CString			maxstr = "";

	CString	block = text;
	while (block.GetLength()>0)
	{
		CString sub;
		sub = block.SpanExcluding( "\n" );

		txt_array.Add( sub );

		int len = sub.GetLength();
		if (len > maxstr.GetLength())
			maxstr = sub;

		if (len >= block.GetLength())
			block = "";
		else
			block = block.Mid( len+1 );
	}
	int num = txt_array.GetSize();

	// How tall is text right now?  
	//	Set scale to a constant for variable-size text
	CSize size = dc->GetTextExtent( maxstr );

	C2dUnitVec right_dir( 1.0, 0.0);
	C2dVec left = right_dir * -((double)(size.cx/2) / m_scale);

	C2dUnitVec down_dir( 0.0, -1.0 );
	C2dVec down = down_dir * ((double)size.cy / m_scale);
	C2dVec up = down_dir * -((double)size.cy / m_scale);

	switch (pos)
	{
	case TEXTPOS_BTMCTR:
		m_at.X( m_at.X() + down.X()*num + left.X() );
		m_at.Y( m_at.Y() + down.Y()*num + left.Y() );
		break;

	case TEXTPOS_TOPCTR:
		m_at.X( m_at.X() + up.X()*(num-1) + left.X() );
		m_at.Y( m_at.Y() + up.Y()*(num-1) + left.Y() );
		break;

	default:
		break;	// Keep it boring
	}

	// Now, print the multiple lines
	for (int idx=0; idx<num; idx++)
	{
		glRasterPos2d( m_at.X(), m_at.Y() );

		glPushAttrib(GL_LIST_BIT);
		glCallLists( txt_array[idx].GetLength(), GL_UNSIGNED_BYTE, (LPCTSTR)txt_array[idx]);
		glPopAttrib();

		m_at.X( m_at.X() + down.X() );
		m_at.Y( m_at.Y() + down.Y() );
	}

	if (dc)
		ReleaseDC(m_output->m_hWnd, dc->m_hDC);
}

void
CglCanvas::print_o( CString text, eDisplayTextPos pos, double angle, double size )
{
	glListBase( OFONT_LIST_BASE );
	glMatrixMode( GL_MODELVIEW );

	// Break the text down into individual lines
	CStringArray	txt_array;
	CString			maxstr = "";

	CString	block = text;
	while (block.GetLength()>0)
	{
		CString sub;
		sub = block.SpanExcluding( "\n" );

		txt_array.Add( sub );

		int len = sub.GetLength();
		if (len > maxstr.GetLength())
			maxstr = sub;

		if (len >= block.GetLength())
			block = "";
		else
			block = block.Mid( len+1 );
	}
	int num = txt_array.GetSize();

	// How tall is text right now?  
	//	Set scale to a constant for variable-size text
#if BEFORE_V21_0_182_1
	static double FACTOR = 16.;
#else
	double FACTOR = ((size < 10) ? 10 : size);
#endif
	double scale = FACTOR / m_scale;
	double width = TextOWidth( maxstr ) * scale;

	C2dUnitVec right_dir( DEG2RAD*angle );
	C2dVec left = right_dir * -(width/2.0);

	C2dUnitVec down_dir( DEG2RAD*(angle-90.0) );
	C2dVec down = down_dir * scale;
	C2dVec up = down_dir * -scale;

	switch (pos)
	{
	case TEXTPOS_BTMCTR:
		m_at.X( m_at.X() + down.X()*num + left.X() );
		m_at.Y( m_at.Y() + down.Y()*num + left.Y() );
		break;

	case TEXTPOS_TOPCTR:
		m_at.X( m_at.X() + up.X()*(num-1) + left.X() );
		m_at.Y( m_at.Y() + up.Y()*(num-1) + left.Y() );
		break;

	default:
		break;	// Keep it boring
	}

	// Now, print the multiple lines
	for (int idx=0; idx<num; idx++)
	{
		glPushMatrix();
		glLoadIdentity();
		glTranslated( m_at.X(), m_at.Y(), 0.0 );
		glRotated( angle, 0.0, 0.0, 1.0 );
		glScaled( scale, scale, 1.0 );

		glPushAttrib(GL_LIST_BIT);
		glCallLists( txt_array[idx].GetLength(), GL_UNSIGNED_BYTE, (LPCTSTR)txt_array[idx]);
		glPopAttrib();

		m_at.X( m_at.X() + down.X() );
		m_at.Y( m_at.Y() + down.Y() );

		glPopMatrix();
	}
}

double
CglCanvas::TextOWidth( const CString& text )
{
	double	step = 0.0;

	int num = text.GetLength();
	for (int idx=0; idx<num; idx++)
	{
		step += m_txtglyph[text[idx]].gmfCellIncX;
	}

	return step;
}

double
CglCanvas::TextOHeight( const CString& text )
{
	double	step = 0.0;

	int num = text.GetLength();
	for (int idx=0; idx<num; idx++)
		step = max( step, m_txtglyph[text[idx]].gmfBlackBoxY);


	return step;
}

C2dCoord 
CglCanvas::TextOSize( 
	const CString& text )
{
	double	stepx = 0.0;
	double	stepy = 0.0;

	int num = text.GetLength();
	for (int idx=0; idx<num; idx++)
	{
		stepx += m_txtglyph[text[idx]].gmfCellIncX;
		stepy = max( stepy, m_txtglyph[text[idx]].gmfBlackBoxY );
	}

	return C2dCoord( stepx, stepy);
}


CSize
CglCanvas::TextBSize( const CString& text )
{
	CDC* dc = m_output->GetDC();
	CSize size = dc->GetTextExtent( text );

	if (dc)
		ReleaseDC(m_output->m_hWnd, dc->m_hDC);

	return size;
}

C2dCoord
CglCanvas::TextBSize( const CString& text, double scale )
{
	CSize size = TextBSize( text );

	C2dCoord coord( size.cx / scale, size.cy / scale );
	return coord;
}

void		
CglCanvas::moveto( CDC* dc, const C3dCoord& end )
{
	double px, py, pz;

	if ((end.X() >= LARGE) || (end.Y() >= LARGE))
		return;

	glGetDoublev(GL_MODELVIEW_MATRIX, m_model_mat );
	glGetDoublev(GL_PROJECTION_MATRIX, m_proj_mat );
	glGetIntegerv(GL_VIEWPORT, m_viewport );


	gluProject( end.X(), end.Y(), end.Z(),
				m_model_mat,
				m_proj_mat,
				m_viewport,
				&px, &py, &pz );

	m_at = end;

//	m_pdc.MoveTo( (int)(px*m_printscale), (int)((m_viewport[3]-py)*m_printscale)+m_printer_top );
	dc->MoveTo( int(px), int(m_viewport[3]-py) );
}

void		
CglCanvas::lineto( CDC* dc, const C3dCoord& end )
{
	double px, py, pz;

	if ((end.X() >= LARGE) || (end.Y() >= LARGE))
		return;

	gluProject( end.X(), end.Y(), end.Z(),
				m_model_mat,
				m_proj_mat,
				m_viewport,
				&px, &py, &pz );

	m_at = end;

	dc->LineTo( int(px), int(m_viewport[3]-py) );
}

void
CglCanvas::ModelBegin()
{
	glNewList( m_model_list, GL_COMPILE );
	m_model_active = true;
}

void
CglCanvas::ModelEnd()
{
	if (m_open)
		Close();

	glEndList();
	m_model_active = false;
}

bool
CglCanvas::IsModelActive() const
{
	return m_model_active;
}

void
CglCanvas::SemiTempBegin()
{
	glNewList( m_semi_list, GL_COMPILE );
	m_temp_active = true;
}

void
CglCanvas::SemiTempEnd()
{
	if (m_open)
		Close();

	glEndList();
	m_temp_active = false;
}

void
CglCanvas::SemiTempDelta( const C3dCoord& delta )
{
	m_semi_delta = delta;
}

void
CglCanvas::PureTempBegin()
{
	glNewList( m_pure_list, GL_COMPILE );
	m_temp_active = true;
}

void
CglCanvas::PureTempEnd()
{
	if (m_open)
		Close();

	glEndList();
	m_temp_active = false;
}

bool
CglCanvas::IsTemporaryActive() const
{
	return m_temp_active;
}

void
CglCanvas::PatternDraw( const C2dCoord& at )
{
	if (m_open)
		Close();

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

	glTranslatef( (float)at.X(), (float)at.Y(), 0.0 );
	glCallList( m_model_list );
//	glFlush();

	glPopMatrix();
} 

void		
CglCanvas::error( void )
{
	int glerr = glGetError();
	if (glerr != GL_NO_ERROR)
	{
		MessageBox( NULL, (char*)gluErrorString( glerr ), NULL, MB_OK );
	}
}

void		
CglCanvas::silent_error( void )
{
	int glerr = glGetError();
	// TODO:  Figure out why I keep getting errors
}

void	
CglCanvas::WMF_file( CString filepath )
{
	m_wmf_file = filepath;
}

void
CglCanvas::OglReport()
{
	if (CReturn::Debug() > 1)
	{
		CReturn note;
		CString str;
		
		str = "Mode:";
		if (g_bestMatch_pfd.dwFlags & PFD_DRAW_TO_WINDOW)
			str += " WINDOW";
		if (g_bestMatch_pfd.dwFlags & PFD_SUPPORT_GDI)
			str += " GDI";
		if (g_bestMatch_pfd.dwFlags & PFD_SUPPORT_OPENGL)
			str += " OGL";
		if (g_bestMatch_pfd.dwFlags & PFD_GENERIC_FORMAT)
			str += " GENERIC";
		if (g_bestMatch_pfd.dwFlags & PFD_DOUBLEBUFFER)
			str += " DOUBLE-BUF";
		if (g_bestMatch_pfd.dwFlags & PFD_SWAP_LAYER_BUFFERS)
			str += " SWAP-LAYER";
		if (g_bestMatch_pfd.dwFlags & PFD_NEED_PALETTE)
			str += " NEED-PALETTE";
		note.Diagnostic( str );

		if (g_bestMatch_pfd.dwFlags & PFD_SWAP_COPY)
			str = "Swap COPY";
		else
		if (g_bestMatch_pfd.dwFlags & PFD_SWAP_EXCHANGE)
			str = "Swap EXCHANGE";
		else
			str = "Swap unknown";
		note.Diagnostic( str );

		str.Format( "%s at %d bits", (g_bestMatch_pfd.iPixelType==PFD_TYPE_RGBA)?"RGBA":"Palette", g_bestMatch_pfd.cColorBits );
		note.Diagnostic( str );

		str.Format( "OGL Vendor: %s\n", (char*)glGetString( GL_VENDOR ) );
		note.Diagnostic( str );

		str.Format( "OGL Renderer: %s\n", (char*)glGetString( GL_RENDERER) );
		note.Diagnostic( str );

		str.Format( "OGL Version: %s\n", (char*)glGetString( GL_VERSION ) );
		note.Diagnostic( str );

#if 0
		// This can be a *very* long string which (currently) doesn't provide much useful information.
		str.Format( "OGL Extensions: %s\n", (char*)glGetString( GL_EXTENSIONS ) );
		note.Diagnostic( str );
#endif
		str.Format( "GLU Version: %s\n", (char*)gluGetString( GLU_VERSION ) );
		note.Diagnostic( str );

		str.Format( "GLU Extensions: %s\n", (char*)gluGetString( GLU_EXTENSIONS ) );
		note.Diagnostic( str );
	}
}

// ----------------------------------------------------------

#if NECESSARY

// FROM http://www.opengl.org/resources/faq/technical/weight.cpp
// Ron Fosner's code for weighting pixel formats and forcing software.



/*===================================================================
This is your weighting function. Customize it to suit your needs.
You write it so that it examines the PFD passed in and returns a weighting
factor. You then select the largest weight (which is supposedly the best
pixel format). The optional bForceSoftwareFlag will eliminate any hardware
accelerated format from consideration (i.e. it'll force software rendering)
The currentColorDepth param is the current desktop color depth. Formats
that are in this color depth are preferred over others. If the color depth
isn't better that 256 color mode, we don't select and pixel format.
===================================================================*/

unsigned WeightPixelFormats(
	PIXELFORMATDESCRIPTOR&	pfd,
	unsigned				currentColorDepth,
	bool					bForceSoftware )

{
	unsigned weight = 0;

	// if it's not the same color depth, double buffered in a Window,
	// and in RGBA mode with a z buffer we don't want it.
	if ( currentColorDepth != pfd.cColorBits  || 
		!(pfd.dwFlags & PFD_SUPPORT_OPENGL) ||
		!(pfd.dwFlags & PFD_DRAW_TO_WINDOW) ||
		( pfd.cDepthBits <= 8 ) ||
		!(pfd.iPixelType == PFD_TYPE_RGBA)  )
			return(0);

	// (1) it's usable / (2) we like double buffered.
	weight = ((pfd.dwFlags & PFD_DOUBLEBUFFER) ? 2 : 1);


	// we like smaller (but nonzero depth buffers)
	// so weight smaller better - assume smallest is 16 bits, else ignore it
	if (16 <= pfd.cDepthBits)
	{
		// assume that the depth buffer will never >= 128 bits
		weight += 128 - pfd.cDepthBits;
		// assert( pfd.cDepthBits <= 128 );
	}

	// only good for ICD's or generic, don't worry about mini-client drivers
	if ( pfd.dwFlags & PFD_GENERIC_FORMAT )
	{
		// software implementation
	}
	else
	{
		// hardware implementation
		if ( true == bForceSoftware )
			weight = 0; // kill it
		else
			weight += 1024; 
	}

	return weight;
}



/*===================================================================
Use this function instead of ChoosePixelFormat, You pass in the Window
DC, the current desktop color depth, and a flag that's true if you only
want software rendering only. It will return the pixel format that will
be best suited (as selected by the WeightPixelFormats function). You then
use the returned value for CreatePixelFormat.
Sugggestions/bugs report to Ron Fosner - ron@directx.com
===================================================================*/

int EnumPixelFormats( HDC hdc, unsigned colorDepth, bool bSoftware )
{
	int  iPixelFormat; 
	int  iCount; 
	unsigned weight = 0;

	PIXELFORMATDESCRIPTOR pfd2;


	PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),// size of this pfd
		1,                      // version number
		PFD_DRAW_TO_WINDOW |    // support window
		PFD_SUPPORT_OPENGL |  // support OpenGL
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,          // RGBA type
		32,
		0, 0, 0, 0, 0, 0,       // color bits ignored
		0,                      // no alpha buffer
		0,                      // shift bit ignored
		0,                      // no accumulation buffer
		0, 0, 0, 0,             // accum bits ignored
		16,                     // 16-bit z-buffer
		0,                      // no stencil buffer
		0,                      // no auxiliary buffer
		PFD_MAIN_PLANE,         // main layer
		0,                      // reserved
		0, 0, 0                 // layer masks ignored
	};


	// assert( NULL != hdc );
	iPixelFormat = 1;

	pfd2 = pfd;

	// obtain detailed information about 
	// the device context's first pixel format 
	iCount = 1 + ::DescribePixelFormat( hdc, iPixelFormat,  
		sizeof(PIXELFORMATDESCRIPTOR), &pfd2 ); 

	// use stupid OS function to select one by default in case we
	// can't come up with something better   
	iPixelFormat = ::ChoosePixelFormat( hdc, &pfd );
	if (0 == iPixelFormat)
	{
		// TRACE( "No OpenGL pixel formats found\n");
		return 0;
	}

	// TRACE("%d OpenGL pixel formats found\n", iCount-1);

	// pfd counts are 1 based (not 0)
	while ( --iCount )
	{
		::DescribePixelFormat( hdc, iCount, sizeof(PIXELFORMATDESCRIPTOR), &pfd ); 

		unsigned w = WeightPixelFormats( pfd, colorDepth, bSoftware );

		if ( w > weight )
		{
			 // we like this one better?
			weight = w;
			iPixelFormat = iCount;
		}
	}

#if defined (_DEBUG)
	// ::DescribePixelFormat( hdc, iPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd ); 
	// TRACE("Pixel format %d selected\n",iPixelFormat);
	// TRACE("Pixel format has %d color & %d depth bits\n",(int)pfd.cColorBits, (int)pfd.cDepthBits );
#endif

	return iPixelFormat;
}

#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Support items for OpenGL polygon tesselation.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

GLvoid CALLBACK beginCallback( GLenum which )
{
   glBegin(which);
}

GLvoid CALLBACK endCallback()
{
   glEnd();
}

GLvoid CALLBACK vertexCallback( GLvoid* vertex )
{
	GLdouble* v = (GLdouble*) vertex;
	// glColor3dv(v+3);
	g_poly_color[0] = red(g_curr_color);
	g_poly_color[1] = green(g_curr_color);
	g_poly_color[2] = blue(g_curr_color);
	glColor3dv(g_poly_color);
	glVertex3dv(v);

	TRACE( "vertexCallback: (%f,%f,%f)\n", v[0], v[1], v[2] ) ;
}

GLvoid CALLBACK errorCallback( GLenum errorCode )
{
	const GLubyte* estring = gluErrorString(errorCode);
	if (estring != NULL)
	{
		CReturn status;
		CString msg;

		msg.Format( "Tessellation Error: %s", estring );
		status.Diagnostic( msg );
	}
}
