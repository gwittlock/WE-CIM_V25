
#include "stdafx.h"
#include "MathConst.h"
#include "StringConst.h"
#include "3dVec.h"
#include "3x4Matrix.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "Solution.h"
#include "ToolShape.h"

const double DEFAULT_TOOL_LENGTH = 2.0;
const double DEFAULT_TOOL_CUT_HEIGHT = 1.0;
const double DEFAULT_TOOL_DIAMETER = 0.5;


////////////////////////////////////////////////////////////////////////

CToolShape::CToolShape()
{
}

CToolShape::~CToolShape()
{
	m_type.RemoveAll();
	// m_type.FreeExtra();

	m_pt.RemoveAll();
	// m_pt.FreeAll();
}

void
CToolShape::Init( const CVarList& attribs )
{
	int type_id = attribs.getInt( STR_TYPE_ID, IUNDEFINED );
	int autoIndex = attribs.getInt( STR_AUTO_INDEX, IUNDEFINED );
	double indexAngle = attribs.getReal( STR_INDEX_ANGLE, UNDEFINED );

	switch (type_id)
	{
	case TTYPE_SQUARE:
		gen_2d_square( attribs );
		break;
	case TTYPE_RECTANGLE:
	case TTYPE_FORMING:
		gen_2d_rect( attribs );
		break;
	case TTYPE_OBROUND:
		gen_2d_obround( attribs );
		break;
	case TTYPE_DIAMOND:
		gen_2d_diamond( attribs );
		break;
	case TTYPE_CORNER_RADIUS:
		gen_2d_crad( attribs );
		break;
	case TTYPE_SINGLE_D:
		gen_2d_d( attribs );
		break;
	case TTYPE_DOUBLE_D:
		gen_2d_dd( attribs );
		break;
	case TTYPE_TRAPEZOID:
		gen_2d_trapezoid( attribs );
		break;
	case TTYPE_KEYHOLE:
		gen_2d_keyhole( attribs );
		break;
	case TTYPE_HEXAGON:
		gen_2d_hexagon( attribs );
		break;
	case TTYPE_CUSTOM:
		gen_2d_custom( attribs );
		break;
	default:
		gen_2d_round( attribs );
		break;
	}

	if (autoIndex == 0 && (indexAngle > 0.0 && indexAngle < UNDEFINED))
	{
		// The shape must be rotated into its defined orientation.
		C3x4Matrix xform;
		xform.setXYAngle( (indexAngle * DEG2RAD) );
		
		int count = m_type.GetSize();
		for (int indx = 0; indx < count; ++indx)
		{
			xform.Transform( &(m_pt[indx]) );
		}
	}
}

void
CToolShape::InitXZ( const CVarList& attribs )
{
#if (_CI || _NST)
	int type_id = attribs.getInt( STR_TYPE_ID, IUNDEFINED );

	switch (type_id)
	{
	case TTYPE_ROUTER_BIT:
		gen_xz_straight( attribs );
		break;
	case TTYPE_CORE_BOX:
		gen_xz_core_box( attribs );
		break;
	case TTYPE_ROUND_OVER:
		gen_xz_round_over( attribs );
		break;
	case TTYPE_POINT_CUTTING_ROUND_OVER:
		gen_xz_pt_round_over( attribs );
		break;
	case TTYPE_OGEE:
		gen_xz_ogee( attribs );
		break;
	case TTYPE_RAISED_PANEL:
		gen_xz_raised_panel( attribs );
		break;
	case TTYPE_V_GROOVE:
		gen_xz_v_groove( attribs );
		break;
	case TTYPE_DRILL:
		gen_xz_drill( attribs );
		break;
	case TTYPE_SAW:
		gen_xz_saw( attribs );
		break;
	case TTYPE_CUSTOM:
		gen_xz_custom( attribs );
		break;
	default:
		gen_xz_round( attribs );
		break;
	}
#endif
}


void
CToolShape::Init(
			CArray<int,int>& type,
			CArray<C3dCoord, C3dCoord>& pt )
{
	int	count, indx;

	count = type.GetSize();  // should be same size as pt.GetSize()

	m_type.SetSize( count, 10 );
	m_pt.SetSize( count, 10 );

	for (indx = 0; indx < count; ++indx)
	{
		m_type.SetAt( indx, type[indx] );
		m_pt.SetAt( indx, pt[indx] );
	}
}

CReturn
CToolShape::InitFromCTG( const CString& ctg_path )
{
	CReturn		status;
	C3dCoord	pt;
	float		dval1, dval2;
	int			ival;
	FILE*		f;

	m_type.SetSize( 0, 16 );
	m_pt.SetSize( 0, 16 );

	f = fopen( ctg_path, "r" );
	if (f == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CToolShape::InitFromCTG()" );
	}
	else
	{
		while (fscanf( f, "%d%f%f", &ival, &dval1, &dval2 ) != EOF)
		{
			m_type.Add( ival );
			pt.XYZ( dval1, dval2, 0. );
			m_pt.Add( pt );
		}

		fclose( f );
	}

	return status;
}

void
CToolShape::Invalidate()
{
	m_type.SetSize( 0, 10 );
	m_pt.SetSize( 0, 10 );
}

// In lieu of BNF, examples of m_type[] sequences are:
//   1,1,1,1,1    -- interpreted as line,line,line,line
//   1,1,2,1,2,1  -- interpreted as line,arc,arc
//
// NOTE: the sequence must always start with type 1.
void
CToolShape::Convert( CGeoCurveArray* geoCurves ) const
{
	Convert( m_type, m_pt, geoCurves );
}

void
CToolShape::Convert( CGeoPoly* geoPoly) const
{
	Convert( m_type, m_pt, geoPoly );
}

// Type values:
//   0 -- nop (rapid)
//   1 -- end point
//   2 -- cw arc center point
//   3 -- ccw arc center point
//
// In lieu of BNF, examples of m_type[] sequences are:
//   1,1,1,1,1    -- interpreted as line,line,line,line
//   1,1,2,1,2,1  -- interpreted as line,arc,arc
//
// NOTE: the sequence must always start with type either 0 or 1.
void
CToolShape::Convert(
				const CArray<int,int>& pt_type,
				const CArray<C3dCoord, C3dCoord>& pt,
				CGeoCurveArray* geoCurves )
{
	int count = pt_type.GetSize();
	int indx = 1;

	while (indx < count)
	{
		int type = pt_type[indx];

		if (type == 1)
		{
			CGeoLine* line = new CGeoLine( pt[indx-1], pt[indx] );

			if (line->Length2d() >= SMALL)
				geoCurves->Append( line );
			else
				delete line;
		}
		else if (type != 0) // ie. (2) G02 / (3) G03
		{
			CGeoArc* arc = new CGeoArc(
				pt[indx-1], pt[indx+1], pt[indx], ((pt_type[indx] == 2) ? -1 : 1) );

			if (arc->Radius() >= SMALL)
				geoCurves->Append( arc );
			else
				delete arc;

			++indx;
		}

		++indx;
	}
}

void
CToolShape::Convert(
				const CArray<int,int>& pt_type,
				const CArray<C3dCoord, C3dCoord>& pt,
				CGeoPoly* geoPoly)
{
	int count = pt_type.GetSize();
	int indx = 1;

	while (indx < count)
	{
		int type = pt_type[indx];

		if (type == 1)
		{
			CGeoLine line( pt[indx-1], pt[indx] );

			if (line.Length2d() >= SMALL)
				geoPoly->CopyAppend( line );
		}
		else  // ie. (2) G02 / (3) G03
		{
			CGeoArc arc( pt[indx-1], pt[indx+1], pt[indx], ((pt_type[indx] == 2) ? -1 : 1) );

			if (arc.Radius() >= SMALL)
				geoPoly->CopyAppend( arc );

			++indx;
		}

		++indx;
	}
}


double
CToolShape::EffectiveDiameter( const CVarList& attribs )
{
	double diam;

#if (_CI || _NST)
	diam = attribs.getReal( STR_DIAMETER, 0.5 );
#else
	int type_id = attribs.getInt( STR_TYPE_ID, IUNDEFINED );

	// TODO: Non-round PUNCH tools
	switch (type_id)
	{
	case TTYPE_BURNER:
	case TTYPE_DISC_SAW:
	case TTYPE_SCRIBE:
	case TTYPE_WATERJET:
	case TTYPE_LASER:
	case TTYPE_POWDER_MARK:
		diam = attribs.getReal( "Kerf", 0.5 );
		break;

	case TTYPE_MARKING:
		// Don't ask me, I didn't do it. eww
		return EffectiveLength( attribs );

	default:
		diam = attribs.getReal( STR_DIAMETER, 0.5 );
		break;
	}
#endif

	return diam;
}

double
CToolShape::EffectiveLength( const CVarList& attribs )
{
	// Like EffectiveDiameter, but reports on lengths, too
	// TODO:  Merge with EffectiveDiameter???

	int type_id = attribs.getInt( STR_TYPE_ID, IUNDEFINED );

	double diam = 5.0;  // Default, to avoid brain pain
	switch (type_id)
	{
	case TTYPE_BURNER:
	case TTYPE_DISC_SAW:
	case TTYPE_SCRIBE:
	case TTYPE_WATERJET:
	case TTYPE_LASER:
	case TTYPE_POWDER_MARK:
		return attribs.getReal( "Kerf", diam );
	}

	double length = attribs.getReal( "Length", 0.0 );
	double width = attribs.getReal( "Width", 0.0 );

	diam = attribs.getReal( STR_DIAMETER, 0.0 );
	return max( diam, max( length, width ) );
}



bool
CToolShape::IsHoleTool( const CVarList& attribs )
{
	int	type_id = attribs.getInt( STR_TYPE_ID, IUNDEFINED );

	switch (type_id)
	{
	case TTYPE_BRAD_POINT:
	case TTYPE_LANCE_BIT:
	case TTYPE_DRILL:
	case TTYPE_TAP:
	case TTYPE_COUNTER_SINK:
	case TTYPE_ROUND:
	case TTYPE_SQUARE:
	case TTYPE_RECTANGLE:
	case TTYPE_OBROUND:
	case TTYPE_DIAMOND:
	case TTYPE_CORNER_RADIUS:
	case TTYPE_SINGLE_D:
	case TTYPE_DOUBLE_D:
	case TTYPE_KEYHOLE:
	case TTYPE_TRAPEZOID:
	case TTYPE_HEXAGON:
	case TTYPE_CENTER_PUNCH:
	case TTYPE_FORMING:
	case TTYPE_MARKING:
		return TRUE;
	}

	return FALSE;
}

bool
CToolShape::IsRoundTool( const CVarList& attribs )
{
	// All non-hole tools are considered "round" tools; though Disk Saw
	// may eventually be an exception.
	if ( !IsHoleTool( attribs ) )
		return TRUE;

	// *some* hole tools are also "round"
	int type_id = attribs.getInt( STR_TYPE_ID, IUNDEFINED );

	switch (type_id)
	{
	case TTYPE_BRAD_POINT:
	case TTYPE_LANCE_BIT:
	case TTYPE_DRILL:
	case TTYPE_TAP:
	case TTYPE_COUNTER_SINK:
	case TTYPE_ROUND:
	case TTYPE_CENTER_PUNCH:
	case TTYPE_MARKING:
		return TRUE;
	}

	return FALSE;
}

bool
CToolShape::IsFormTool( const CVarList& attribs )
{
	return (attribs.getInt( STR_TYPE_ID, IUNDEFINED ) == TTYPE_FORMING);
}

bool
CToolShape::IsProfilingTool( const CVarList& attribs )
{
	int	type_id = attribs.getInt( STR_TYPE_ID, IUNDEFINED );

	switch (type_id)
	{
	case TTYPE_ROUTER_BIT:
	case TTYPE_DISC_SAW:
	case TTYPE_BURNER:
	case TTYPE_SCRIBE:
	case TTYPE_LASER:
	case TTYPE_WATERJET:
	case TTYPE_ROUND:
	case TTYPE_SQUARE:
	case TTYPE_RECTANGLE:
	case TTYPE_OBROUND:
	case TTYPE_DIAMOND:
	case TTYPE_CORNER_RADIUS:
	case TTYPE_SINGLE_D:
	case TTYPE_DOUBLE_D:
	case TTYPE_TRAPEZOID:
	case TTYPE_KEYHOLE:
	case TTYPE_HEXAGON:
	case TTYPE_END_MILL:
		return TRUE;
	}

	return FALSE;
}


//================================================================

void
CToolShape::gen_2d_round( const CVarList& attribs ) 
{
	double radius;
	if ( IsRoundTool( attribs ) )
		radius = EffectiveDiameter( attribs ) / 2.0;
	else
		radius = attribs.getReal( STR_WIDTH, 5. ) / 2.0;

	m_type.SetSize( 3, 10 );
	m_pt.SetSize( 3, 10 );
	
	m_type[0] = 1;
	m_pt[0] = C3dCoord( radius, 0., 0. );

	m_type[1] = 3;
	m_pt[1] = C3dCoord( 0., 0., 0. );

	m_type[2] = 1;
	m_pt[2] = C3dCoord( radius, 0., 0. );
}


void
CToolShape::gen_2d_square( const CVarList& attribs )
{
	C3dCoord ct( 0., 0., 0. );

	C2dUnitVec	perp( 0., 1. );
	C2dUnitVec	back( -perp.Y(), perp.X() );

	double width = attribs.getReal( STR_WIDTH, 0. );

	double radius = attribs.getReal( "Corner_radius", 0. );

	double halfwide = width / 2.0;
	double flat = width - 2*radius;
	double halfflat = flat / 2.0;

	C3dVec	perp_long = perp*halfwide;
	C3dVec	perp_short = perp*halfflat;
	C3dVec	back_long = back*halfwide;
	C3dVec	back_short = back*halfflat;


	m_type.SetSize( 13, 10 );
	m_pt.SetSize( 13, 10 );

	m_type[0] = 1;
	m_pt[0] = ct + perp_long + back_short;

	m_type[1] = 1;
	m_pt[1] = ct + perp_long - back_short;
	m_type[2] = 2;
	m_pt[2] = ct + perp_short - back_short;
	m_type[3] = 1;
	m_pt[3] = ct - back_long + perp_short;

	m_type[4] = 1;
	m_pt[4] = ct - back_long - perp_short;
	m_type[5] = 2;
	m_pt[5] = ct - back_short - perp_short;
	m_type[6] = 1;
	m_pt[6] = ct - perp_long - back_short;

	m_type[7] = 1;
	m_pt[7] = ct - perp_long + back_short;
	m_type[8] = 2;
	m_pt[8] = ct - perp_short + back_short;
	m_type[9] = 1;
	m_pt[9] = ct + back_long - perp_short;

	m_type[10] = 1;
	m_pt[10] = ct + back_long + perp_short;
	m_type[11] = 2;
	m_pt[11] = ct + back_short + perp_short;
	m_type[12] = 1;
	m_pt[12] = m_pt[0];
}

void
CToolShape::gen_2d_rect( const CVarList& attribs )
{
	C3dCoord ct( 0., 0., 0. );

	C2dUnitVec	perp( 0., 1. );
	C2dUnitVec	back( -perp.Y(), perp.X() );

	double y_axis = attribs.getReal( STR_WIDTH, 0. );

	double x_axis = attribs.getReal( STR_LENGTH, y_axis );

	double radius = attribs.getReal( "Corner_radius", 0. );

	double half_y = y_axis / 2.0;
	double yflat = y_axis - 2*radius;
	double half_yflat = yflat / 2.0;

	double half_x = x_axis / 2.0;
	double xflat = x_axis - 2*radius;
	double half_xflat = xflat / 2.0;

	C3dVec	perp_long = perp*half_y;
	C3dVec	perp_short = perp*half_yflat;
	C3dVec	back_long = back*half_x;
	C3dVec	back_short = back*half_xflat;


	m_type.SetSize( 13, 10 );
	m_pt.SetSize( 13, 10 );

	m_type[0] = 1;
	m_pt[0] = ct + perp_long + back_short;

	m_type[1] = 1;
	m_pt[1] = ct + perp_long - back_short;
	m_type[2] = 2;
	m_pt[2] = ct + perp_short - back_short;
	m_type[3] = 1;
	m_pt[3] = ct - back_long + perp_short;

	m_type[4] = 1;
	m_pt[4] = ct - back_long - perp_short;
	m_type[5] = 2;
	m_pt[5] = ct - back_short - perp_short;
	m_type[6] = 1;
	m_pt[6] = ct - perp_long - back_short;

	m_type[7] = 1;
	m_pt[7] = ct - perp_long + back_short;
	m_type[8] = 2;
	m_pt[8] = ct - perp_short + back_short;
	m_type[9] = 1;
	m_pt[9] = ct + back_long - perp_short;

	m_type[10] = 1;
	m_pt[10] = ct + back_long + perp_short;
	m_type[11] = 2;
	m_pt[11] = ct + back_short + perp_short;
	m_type[12] = 1;
	m_pt[12] = m_pt[0];
}

void
CToolShape::gen_2d_obround( const CVarList& attribs )
{
	C3dCoord ct( 0., 0., 0. );

	C2dUnitVec	perp( 0., 1. );
	C2dUnitVec	back( -perp.Y(), perp.X() );

	double	y_axis = attribs.getReal( STR_WIDTH, 0. );

	double x_axis = attribs.getReal( STR_LENGTH, y_axis );

	double half_y = y_axis / 2.0;

	double half_x = x_axis / 2.0;
	double xflat = x_axis - y_axis;
	double half_xflat = xflat / 2.0;

	C3dVec	perp_long = perp*half_y;
	C3dVec	back_short = back*half_xflat;


	m_type.SetSize( 7, 10 );
	m_pt.SetSize( 7, 10 );

	m_type[0] = 1;
	m_pt[0] = ct + perp_long + back_short;

	m_type[1] = 1;
	m_pt[1] = ct - back_short + perp_long;
	m_type[2] = 2;
	m_pt[2] = ct - back_short;
	m_type[3] = 1;
	m_pt[3] = ct - back_short - perp_long;

	m_type[4] = 1;
	m_pt[4] = ct + back_short - perp_long;
	m_type[5] = 2;
	m_pt[5] = ct + back_short;
	m_type[6] = 1;
	m_pt[6] = m_pt[0];
}

void
CToolShape::gen_2d_crad( const CVarList& attribs )
{
	C3dCoord ct( 0., 0., 0. );

	C2dUnitVec	perp( 0., 1. );
	C2dUnitVec	back( -perp.Y(), perp.X() );

	double width = attribs.getReal( STR_WIDTH, 0. );

	double web = attribs.getReal( "Web", 0. );

	double radius = attribs.getReal( "Radius", 0. );

	double halfwide = width / 2.0;
	double halfweb = web / 2.0;
	double ctr = halfweb + radius;

	C3dVec	perp_long = perp*halfwide;
	C3dVec	back_long = back*halfwide;

	C3dVec	perp_web = perp*halfweb;
	C3dVec	back_web = back*halfweb;

	C3dVec	perp_ctr = perp*ctr;
	C3dVec	back_ctr = back*ctr;


	m_type.SetSize( 21, 10 );
	m_pt.SetSize( 21, 10 );

	m_type[0] = 1;
	m_pt[0] = ct + perp_long - back_web;

	m_type[1] = 1;
	m_pt[1] = ct + perp_ctr - back_web;
	m_type[2] = 3;
	m_pt[2] = ct + perp_ctr - back_ctr;
	m_type[3] = 1;
	m_pt[3] = ct + perp_web - back_ctr;

	m_type[4] = 1;
	m_pt[4] = ct + perp_web - back_long;
	m_type[5] = 1;
	m_pt[5] = ct - perp_web - back_long;

	m_type[6] = 1;
	m_pt[6] = ct - perp_web - back_ctr;
	m_type[7] = 3;
	m_pt[7] = ct - perp_ctr - back_ctr;
	m_type[8] = 1;
	m_pt[8] = ct - perp_ctr - back_web;

	m_type[9] = 1;
	m_pt[9] = ct - perp_long - back_web;
	m_type[10] = 1;
	m_pt[10] = ct - perp_long + back_web;

	m_type[11] = 1;
	m_pt[11] = ct - perp_ctr + back_web;
	m_type[12] = 3;
	m_pt[12] = ct - perp_ctr + back_ctr;
	m_type[13] = 1;
	m_pt[13] = ct - perp_web + back_ctr;

	m_type[14] = 1;
	m_pt[14] = ct - perp_web + back_long;
	m_type[15] = 1;
	m_pt[15] = ct + perp_web + back_long;

	m_type[16] = 1;
	m_pt[16] = ct + perp_web + back_ctr;
	m_type[17] = 3;
	m_pt[17] = ct + perp_ctr + back_ctr;
	m_type[18] = 1;
	m_pt[18] = ct + perp_ctr + back_web;

	m_type[19] = 1;
	m_pt[19] = ct + perp_long + back_web;
	m_type[20] = 1;
	m_pt[20] = m_pt[0];
}

// The shape dimensions are the physical dimensions of the tool.
void
CToolShape::gen_2d_diamond( const CVarList& attribs )
{
	C3dCoord ct( 0., 0., 0. );

	double dy = attribs.getReal( STR_WIDTH, 0. );
	double dx = attribs.getReal( STR_LENGTH, dy );

	double rad = attribs.getReal( "Corner_radius", 0. );

	C3dVec vx( ((dx / 2) - rad), 0., 0. );
	C3dVec vy( 0., ((dy / 2) - rad), 0. );

	m_type.SetSize( 13, 10 );
	m_pt.SetSize( 13, 10 );

	m_type[0] = 1;
	m_pt[0] = ct + vy;

	m_type[1] = 1;
	m_pt[1] = ct + vx;
	m_type[2] = 2;
	m_pt[2] = m_pt[1];
	m_type[3] = 1;
	m_pt[3] = m_pt[1];

	m_type[4] = 1;
	m_pt[4] = ct - vy;
	m_type[5] = 2;
	m_pt[5] = m_pt[4];
	m_type[6] = 1;
	m_pt[6] = m_pt[4];

	m_type[7] = 1;
	m_pt[7] = ct - vx;
	m_type[8] = 2;
	m_pt[8] = m_pt[7];
	m_type[9] = 1;
	m_pt[9] = m_pt[7];

	m_type[10] = 1;
	m_pt[10] = m_pt[0];
	m_type[11] = 2;
	m_pt[11] = m_pt[10];

	if (rad > SMALL)
	{
		// Calculate the end points of the arcs.

		LineOffset( &m_pt[0], &m_pt[1], rad );
		LineOffset( &m_pt[3], &m_pt[4], rad );
		LineOffset( &m_pt[6], &m_pt[7], rad );
		LineOffset( &m_pt[9], &m_pt[10], rad );
	}

	m_type[12] = 1;
	m_pt[12] = m_pt[0];
}

void
CToolShape::gen_2d_d( const CVarList& attribs )
{
	C3dCoord ct( 0., 0., 0. );

	C2dUnitVec	perp( 0., 1. );
	C2dUnitVec	back( -perp.Y(), perp.X() );

	double	width = attribs.getReal( STR_WIDTH, 0. );

	double diameter = attribs.getReal( STR_DIAMETER, 0. );

	double radius = diameter / 2.0;
	double rise = radius-width; //width/2.0;//radius - width;
	double base = sqrt( radius*radius - rise*rise );

	C3dVec	perp_rise = perp*rise;
	C3dVec	back_base = back*base;


	m_type.SetSize( 4, 10 );
	m_pt.SetSize( 4, 10 );

	m_type[0] = 1;
	m_pt[0] = ct + perp_rise - back_base;
	m_type[1] = 1;
	m_pt[1] = ct + perp_rise + back_base;
	m_type[2] = 2;
	m_pt[2] = ct;
	m_type[3] = 1;
	m_pt[3] = ct + perp_rise - back_base;
}

void
CToolShape::gen_2d_dd( const CVarList& attribs )
{
	C3dCoord ct( 0., 0., 0. );

	C2dUnitVec	perp( 0., 1. );
	C2dUnitVec	back( -perp.Y(), perp.X() );

	double	width = attribs.getReal( STR_WIDTH, 0. );

	double diameter = attribs.getReal( STR_DIAMETER, 0. );

	double radius = diameter / 2.0;
	double rise = width/2.0;//radius - halfwide;
	double base = sqrt( radius*radius - rise*rise );

	C3dVec	perp_rise = perp*rise;
	C3dVec	back_base = back*base;

	m_type.SetSize( 7, 10 );
	m_pt.SetSize( 7, 10 );

	m_type[0] = 1;
	m_pt[0] = ct + perp_rise - back_base;
	m_type[1] = 2;
	m_pt[1] = ct;
	m_type[2] = 1;
	m_pt[2] = ct - perp_rise - back_base;

	m_type[3] = 1;
	m_pt[3] = ct - perp_rise + back_base;
	m_type[4] = 2;
	m_pt[4] = ct;
	m_type[5] = 1;
	m_pt[5] = ct + perp_rise + back_base;

	m_type[6] = 1;
	m_pt[6] = m_pt[0];
}

void
CToolShape::gen_2d_trapezoid( const CVarList& attribs )
{
	C3dCoord ct( 0., 0., 0. );

	C2dUnitVec	perp( 0., 1. );
	C2dUnitVec	back( -perp.Y(), perp.X() );

	double	y_axis = attribs.getReal( STR_WIDTH, 0. );

	double x_axis =	attribs.getReal( STR_LENGTH, y_axis );

	double radius = attribs.getReal( "Corner_radius", 0. );

	double angle = attribs.getReal( "Angle", 0. );
	angle *= DEG2RAD;

	double half_y = y_axis / 2.0;
	double half_x = x_axis / 2.0;

	double top = half_x - (y_axis * tan( HALFPI - angle ));

	C3dVec	perp_long = perp*half_y;
	C3dVec	back_long = back*half_x;
	C3dVec	back_short = back*top;


	m_type.SetSize( 13, 10 );
	m_pt.SetSize( 13, 10 );

	m_type[0] = 1;
	m_pt[0] = ct + perp_long + back_short;

	m_type[1] = 1;
	m_pt[1] = ct + perp_long - back_short;
	m_type[2] = 2;
	m_pt[2] = m_pt[1];			// center calculated later
	m_type[3] = 1;
	m_pt[3] = m_pt[1];

	m_type[4] = 1;
	m_pt[4] = ct - perp_long - back_long;
	m_type[5] = 2;
	m_pt[5] = m_pt[4];			// center calculated later
	m_type[6] = 1;
	m_pt[6] = m_pt[4];

	m_type[7] = 1;
	m_pt[7] = ct -perp_long + back_long;
	m_type[8] = 2;
	m_pt[8] = m_pt[7];			// center calculated later
	m_type[9] = 1;
	m_pt[9] = m_pt[7];

	m_type[10] = 1;
	m_pt[10] = m_pt[0];
	m_type[11] = 2;
	m_pt[11] = m_pt[10];		// center calculated later
	m_type[12] = 1;
	m_pt[12] = m_pt[0];

	if (radius > SMALL)
	{
		// Now, find the arc center points by blending the sets of lines
		//	Also trims back the lines...
		//
		//	TODO: Find a direct arc solution, to avoid use of Blend;
		//           probably wrt. the angle between the lines
		//
		CGeoLine line[4];

		int idx;
		for (idx=0; idx<12; idx+=3)
			line[idx/3] = CGeoLine( m_pt[idx], m_pt[idx+1] );

		for (idx=0; idx<4; idx++)
		{
			int next = (idx+1) & 0x03;
			CGeoArc* arc = CSolution::Blend( line[idx], line[next], radius, SMALL );
			if (arc)
			{
				int st_idx = idx*3 + 1;
				int ct_idx = st_idx + 1;
				int en_idx = st_idx + 2;

				m_pt[st_idx] = arc->StartPt();
				m_pt[en_idx] = arc->EndPt();
				m_pt[ct_idx] = arc->CenterPt();

				delete arc;
			}
		}
	}

	m_pt[0] = m_pt[12];
}

void
CToolShape::gen_2d_keyhole( const CVarList& attribs )
{
	C3dCoord ct( 0., 0., 0. );

	C2dUnitVec	perp( 0., 1. );
	C2dUnitVec	back( -perp.Y(), perp.X() );

	double major = attribs.getReal( "Major_diameter", 0. );

	double minor = attribs.getReal( "Minor_diameter", 0. );

	double length = attribs.getReal( STR_LENGTH, 0. );

	double halflen = length / 2.0;
	double major_rad = major / 2.0;
	double minor_rad = minor / 2.0;

	double base = sqrt( major_rad*major_rad - minor_rad*minor_rad );

	C3dVec	back_ctr = back * (halflen - major_rad);
	C3dVec	fore_ctr = back * -(halflen - minor_rad);
	C3dVec	fore_base = back * -base;

	C3dVec	perp_minor = perp * minor_rad;


	m_type.SetSize( 7, 10 );
	m_pt.SetSize( 7, 10 );

	m_type[0] = 1;
	m_pt[0] = ct + back_ctr + fore_base - perp_minor;
	m_type[1] = 2;
	m_pt[1] = ct + back_ctr;
	m_type[2] = 1;
	m_pt[2] = ct + back_ctr + fore_base + perp_minor;

	m_type[3] = 1;
	m_pt[3] = ct + fore_ctr + perp_minor;
	m_type[4] = 2;
	m_pt[4] = ct + fore_ctr;
	m_type[5] = 1;
	m_pt[5] = ct + fore_ctr - perp_minor;

	m_type[6] = 1;
	m_pt[6] = m_pt[0];
}

void
CToolShape::gen_2d_hexagon( const CVarList& attribs )
{
	C3dCoord ct( 0., 0., 0. );
	C2dUnitVec perp( 0., 1. );

	double stang = perp.Radians();

	double diameter = attribs.getReal( STR_DIAMETER, 1. );
	double radius = diameter / 2.0;


	m_type.SetSize( 7, 10 );
	m_pt.SetSize( 7, 10 );

	for (int idx=0; idx<7; idx++)
	{
		C2dUnitVec	vec(stang + (idx*(TWOPI/6.0)));

		m_type[idx] = 1;
		m_pt[idx] = ct + vec*radius;
	}
}

void
CToolShape::gen_2d_custom( const CVarList& attribs )
{
	CString		path;
	C3dCoord	pt;
	float		dval1, dval2;
	int			ival;
	FILE*		f;

	path = attribs.getString( "filename", "" );
	f = (path.IsEmpty() ? NULL : fopen( path, "r" ));

	if (f == NULL)
	{
		// Because CfgMan provides length & width.
		gen_2d_rect( attribs );
	}
	else
	{
		m_type.SetSize( 0, 16 );
		m_pt.SetSize( 0, 16 );

		while (fscanf( f, "%d%f%f", &ival, &dval1, &dval2 ) != EOF)
		{
			pt.XYZ( dval1, dval2, 0. );
			m_type.Add( ival );
			m_pt.Add( pt );
		}

		fclose( f );
	}
}

void
CToolShape::LineOffset( C3dCoord* ptA, C3dCoord* ptB, double dist )
{
	CGeoLine lineA( (*ptA), (*ptB) );
	CGeoLine* lineB = (CGeoLine*) lineA.Offset( 1, dist );

	(*ptA) = lineB->StartPt();
	(*ptB) = lineB->EndPt();

	delete lineB;
}



// ============================================================================

void
CToolShape::gen_xz_straight( const CVarList& attribs ) 
{
	double	tool_dia = attribs.getReal( "Tool_Dia", DEFAULT_TOOL_DIAMETER );
	double	tool_radius = 0.5 * tool_dia;

	double	tool_shank_radius = 0.5 * attribs.getReal( "Tool_Shank_Size", (0.75 * tool_dia) );
	double	tool_corner_radius = attribs.getReal( "Tool_Corner_Radius", 0.0 );

	double	tool_length = attribs.getReal( "Tool_Length", DEFAULT_TOOL_LENGTH );
	double	tool_cut_height = attribs.getReal( "Tool_Cut_Height", DEFAULT_TOOL_CUT_HEIGHT );

	m_type.SetSize( 13, 10 );
	m_pt.SetSize( 13, 10 );
	
	m_type[0] = 1;
	m_pt[0] = C3dCoord( tool_shank_radius, tool_length, 0.0 );
	
	m_type[1] = 1;
	m_pt[1] = C3dCoord( -tool_shank_radius, tool_length, 0.0 );

	m_type[2] = 1;
	m_pt[2] = C3dCoord( -tool_shank_radius, tool_cut_height, 0.0 );

	m_type[3] = 1;
	m_pt[3] = C3dCoord( -tool_radius, tool_cut_height, 0.0 );

	m_type[4] = 1;
	m_pt[4] = C3dCoord( -tool_radius, tool_corner_radius, 0.0 );

	m_type[5] = 3;
	m_pt[5] = C3dCoord( -(tool_radius - tool_corner_radius), tool_corner_radius, 0.0 );

	m_type[6] = 1;
	m_pt[6] = C3dCoord( -(tool_radius - tool_corner_radius), 0.0, 0.0 );

	m_type[7] = 1;
	m_pt[7] = C3dCoord( (tool_radius - tool_corner_radius), 0.0, 0.0 );

	m_type[8] = 3;
	m_pt[8] = C3dCoord( (tool_radius - tool_corner_radius), tool_corner_radius, 0.0 );
	
	m_type[9] = 1;
	m_pt[9] = C3dCoord( tool_radius, tool_corner_radius, 0.0 );
	
	m_type[10] = 1;
	m_pt[10] = C3dCoord( tool_radius, tool_cut_height, 0.0 );

	m_type[11] = 1;
	m_pt[11] = C3dCoord( tool_shank_radius, tool_cut_height, 0.0 );

	m_type[12] = 1;
	m_pt[12] = C3dCoord( tool_shank_radius, tool_length, 0.0 );
}

void
CToolShape::gen_xz_core_box( const CVarList& attribs ) 
{
	double	tool_dia = attribs.getReal( "Tool_Dia", DEFAULT_TOOL_DIAMETER );
	double	tool_radius = 0.5 * tool_dia;

	double	tool_shank_radius = 0.5 * attribs.getReal( "Tool_Shank_Size", (0.75 * tool_dia) );
	double	tool_corner_radius = attribs.getReal( "Tool_Corner_Radius", tool_radius );

	double	tool_length = attribs.getReal( "Tool_Length", DEFAULT_TOOL_LENGTH );
	double	tool_cut_height = attribs.getReal( "Tool_Cut_Height", DEFAULT_TOOL_CUT_HEIGHT );

	m_type.SetSize( 13, 10 );
	m_pt.SetSize( 13, 10 );
	
	m_type[0] = 1;
	m_pt[0] = C3dCoord( tool_shank_radius, tool_length, 0.0 );
	
	m_type[1] = 1;
	m_pt[1] = C3dCoord( -tool_shank_radius, tool_length, 0.0 );

	m_type[2] = 1;
	m_pt[2] = C3dCoord( -tool_shank_radius, tool_cut_height, 0.0 );

	m_type[3] = 1;
	m_pt[3] = C3dCoord( -tool_radius, tool_cut_height, 0.0 );

	m_type[4] = 1;
	m_pt[4] = C3dCoord( -tool_radius, tool_corner_radius, 0.0 );

	m_type[5] = 3;
	m_pt[5] = C3dCoord( -(tool_radius - tool_corner_radius), tool_corner_radius, 0.0 );

	m_type[6] = 1;
	m_pt[6] = C3dCoord( -(tool_radius - tool_corner_radius), 0.0, 0.0 );

	m_type[7] = 1;
	m_pt[7] = C3dCoord( (tool_radius - tool_corner_radius), 0.0, 0.0 );

	m_type[8] = 3;
	m_pt[8] = C3dCoord( (tool_radius - tool_corner_radius), tool_corner_radius, 0.0 );
	
	m_type[9] = 1;
	m_pt[9] = C3dCoord( tool_radius, tool_corner_radius, 0.0 );
	
	m_type[10] = 1;
	m_pt[10] = C3dCoord( tool_radius, tool_cut_height, 0.0 );

	m_type[11] = 1;
	m_pt[11] = C3dCoord( tool_shank_radius, tool_cut_height, 0.0 );

	m_type[12] = 1;
	m_pt[12] = C3dCoord( tool_shank_radius, tool_length, 0.0 );
}

void
CToolShape::gen_xz_round_over( const CVarList& attribs ) 
{
	double	tool_dia = attribs.getReal( "Tool_Major_Dia", DEFAULT_TOOL_DIAMETER );
	double	tool_radius = 0.5 * tool_dia;

	double	tool_shank_radius = 0.5 * attribs.getReal( "Tool_Shank_Size", (0.75 * tool_dia) );
	double	tool_corner_radius = attribs.getReal( "Tool_Corner_Radius", tool_radius );

	double	tool_length = attribs.getReal( "Tool_Length", DEFAULT_TOOL_LENGTH );
	double	tool_cut_height = attribs.getReal( "Tool_Cut_Height", DEFAULT_TOOL_CUT_HEIGHT );
	double	tool_shoulder_height = attribs.getReal( "Tool_Shoulder_Height", (tool_cut_height / 4.0) );

	m_type.SetSize( 15, 10 );
	m_pt.SetSize( 15, 10 );
	
	m_type[0] = 1;
	m_pt[0] = C3dCoord( tool_shank_radius, tool_length, 0.0 );
	
	m_type[1] = 1;
	m_pt[1] = C3dCoord( -tool_shank_radius, tool_length, 0.0 );

	m_type[2] = 1;
	m_pt[2] = C3dCoord( -tool_shank_radius, tool_cut_height, 0.0 );

	m_type[3] = 1;
	m_pt[3] = C3dCoord( -tool_radius, tool_cut_height, 0.0 );

	m_type[4] = 1;
	m_pt[4] = C3dCoord( -tool_radius, (tool_cut_height - tool_shoulder_height), 0.0 );

	m_type[5] = 2;
	m_pt[5] = C3dCoord( -tool_radius, (tool_cut_height - tool_shoulder_height - tool_corner_radius), 0.0 );

	m_type[6] = 1;
	m_pt[6] = C3dCoord( -(tool_radius - tool_corner_radius), (tool_cut_height - tool_shoulder_height - tool_corner_radius), 0.0 );

	m_type[7] = 1;
	m_pt[7] = C3dCoord( -(tool_radius - tool_corner_radius), 0.0, 0.0 );

	m_type[8] = 1;
	m_pt[8] = C3dCoord( (tool_radius - tool_corner_radius), 0.0, 0.0 );

	m_type[9] = 1;
	m_pt[9] = C3dCoord( (tool_radius - tool_corner_radius), (tool_cut_height - tool_shoulder_height - tool_corner_radius), 0.0 );

	m_type[10] = 2;
	m_pt[10] = C3dCoord( tool_radius, (tool_cut_height - tool_shoulder_height - tool_corner_radius), 0.0 );
	
	m_type[11] = 1;
	m_pt[11] = C3dCoord( tool_radius, (tool_cut_height - tool_shoulder_height), 0.0 );

	m_type[12] = 1;
	m_pt[12] = C3dCoord( tool_radius, tool_cut_height, 0.0 );

	m_type[13] = 1;
	m_pt[13] = C3dCoord( tool_shank_radius, tool_cut_height, 0.0 );

	m_type[14] = 1;
	m_pt[14] = C3dCoord( tool_shank_radius, tool_length, 0.0 );
}

void
CToolShape::gen_xz_pt_round_over( const CVarList& attribs ) 
{
	double	tool_dia = attribs.getReal( "Tool_Dia", DEFAULT_TOOL_DIAMETER );
	double	tool_radius = 0.5 * tool_dia;

	double	tool_shank_radius = 0.5 * attribs.getReal( "Tool_Shank_Size", (0.75 * tool_dia) );
	double	tool_corner_radius = attribs.getReal( "Tool_Corner_Radius", tool_radius );

	double	tool_length = attribs.getReal( "Tool_Length", DEFAULT_TOOL_LENGTH );
	double	tool_cut_height = attribs.getReal( "Tool_Cut_Height", DEFAULT_TOOL_CUT_HEIGHT );

	m_type.SetSize( 13, 10 );
	m_pt.SetSize( 13, 10 );
	
	m_type[0] = 1;
	m_pt[0] = C3dCoord( tool_shank_radius, tool_length, 0.0 );
	
	m_type[1] = 1;
	m_pt[1] = C3dCoord( -tool_shank_radius, tool_length, 0.0 );

	m_type[2] = 1;
	m_pt[2] = C3dCoord( -tool_shank_radius, tool_cut_height, 0.0 );

	m_type[3] = 1;
	m_pt[3] = C3dCoord( -tool_radius, tool_cut_height, 0.0 );

	m_type[4] = 1;
	m_pt[4] = C3dCoord( -tool_radius, tool_corner_radius, 0.0 );

	m_type[5] = 2;
	m_pt[5] = C3dCoord( -tool_radius, 0.0, 0.0 );

	m_type[6] = 1;
	m_pt[6] = C3dCoord( -(tool_radius - tool_corner_radius), 0.0, 0.0 );

	m_type[7] = 1;
	m_pt[7] = C3dCoord( (tool_radius - tool_corner_radius), 0.0, 0.0 );

	m_type[8] = 2;
	m_pt[8] = C3dCoord( tool_radius, 0.0, 0.0 );
	
	m_type[9] = 1;
	m_pt[9] = C3dCoord( tool_radius, tool_corner_radius, 0.0 );

	m_type[10] = 1;
	m_pt[10] = C3dCoord( tool_radius, tool_cut_height, 0.0 );

	m_type[11] = 1;
	m_pt[11] = C3dCoord( tool_shank_radius, tool_cut_height, 0.0 );

	m_type[12] = 1;
	m_pt[12] = C3dCoord( tool_shank_radius, tool_length, 0.0 );
}

#define STANDARD 0
#define ROMAN 1
void
CToolShape::gen_xz_ogee( const CVarList& attribs ) 
{
	double	tool_dia = attribs.getReal( "Tool_Major_Dia", DEFAULT_TOOL_DIAMETER );
	double	tool_radius = 0.5 * tool_dia;

	double	tool_shank_radius = 0.5 * attribs.getReal( "Tool_Shank_Size", (0.75 * tool_dia) );
	double	tool_corner_radius = attribs.getReal( "Tool_Corner_Radius", (0.5 * tool_radius) );

	double	tool_length = attribs.getReal( "Tool_Length", DEFAULT_TOOL_LENGTH );
	double	tool_cut_height = attribs.getReal( "Tool_Cut_Height", tool_radius );

	// (0) Standard / (1) Roman
	int	tool_style = attribs.getInt( "Tool_Style", STANDARD );

	m_type.SetSize( 17, 10 );
	m_pt.SetSize( 17, 10 );
	
	m_type[0] = 1;
	m_pt[0] = C3dCoord( tool_shank_radius, tool_length, 0.0 );
	
	m_type[1] = 1;
	m_pt[1] = C3dCoord( -tool_shank_radius, tool_length, 0.0 );

	m_type[2] = 1;
	m_pt[2] = C3dCoord( -tool_shank_radius, tool_cut_height, 0.0 );

	m_type[3] = 1;
	m_pt[3] = C3dCoord( -tool_radius, tool_cut_height, 0.0 );

	m_type[4] = 1;
	m_pt[4] = C3dCoord( -tool_radius, (2.0 * tool_corner_radius) , 0.0 );

	if (tool_style == ROMAN)
	{
		m_type[5] = 2;
		m_pt[5] = C3dCoord( -tool_radius, tool_corner_radius, 0.0 );
	}
	else
	{
		m_type[5] = 3;
		m_pt[5] = C3dCoord( -(tool_radius - tool_corner_radius), (2.0 * tool_corner_radius), 0.0 );
	}

	m_type[6] = 1;
	m_pt[6] = C3dCoord( -(tool_radius - tool_corner_radius), tool_corner_radius, 0.0 );


	if (tool_style == ROMAN)
	{
		m_type[7] = 3;
		m_pt[7] = C3dCoord( -(tool_radius - (2.0 * tool_corner_radius)), tool_corner_radius, 0.0 );
	}
	else
	{
		m_type[7] = 2;
		m_pt[7] = C3dCoord( -(tool_radius - tool_corner_radius), 0., 0.0 );
	}

	m_type[8] = 1;
	m_pt[8] = C3dCoord( -(tool_radius - (2.0 * tool_corner_radius)), 0.0, 0.0 );

	m_type[9] = 1;
	m_pt[9] = C3dCoord( (tool_radius - (2.0 * tool_corner_radius)), 0.0, 0.0 );


	if (tool_style == ROMAN)
	{
		m_type[10] = 3;
		m_pt[10] = C3dCoord( (tool_radius - (2.0 * tool_corner_radius)), tool_corner_radius, 0.0 );
	}
	else
	{
		m_type[10] = 2;
		m_pt[10] = C3dCoord( (tool_radius - tool_corner_radius), 0., 0.0 );
	}

	m_type[11] = 1;
	m_pt[11] = C3dCoord( (tool_radius - tool_corner_radius), tool_corner_radius, 0.0 );

	if (tool_style == ROMAN)
	{
		m_type[12] = 2;
		m_pt[12] = C3dCoord( tool_radius, tool_corner_radius, 0.0 );
	}
	else
	{
		m_type[12] = 3;
		m_pt[12] = C3dCoord( (tool_radius - tool_corner_radius), (2.0 * tool_corner_radius), 0.0 );
	}

	m_type[13] = 1;
	m_pt[13] = C3dCoord( tool_radius, (2.0 * tool_corner_radius) , 0.0 );

	m_type[14] = 1;
	m_pt[14] = C3dCoord( tool_radius, tool_cut_height, 0.0 );

	m_type[15] = 1;
	m_pt[15] = C3dCoord( tool_shank_radius, tool_cut_height, 0.0 );

	m_type[16] = 1;
	m_pt[16] = C3dCoord( tool_shank_radius, tool_length, 0.0 );
}

void
CToolShape::gen_xz_raised_panel( const CVarList& attribs ) 
{
	double	tool_maj_radius = attribs.getReal( "Tool_Major_Dia", DEFAULT_TOOL_DIAMETER ) / 2.0;
	double	tool_min_radius = attribs.getReal( "Tool_Minor_Dia", 0.0 ) / 2.0;
	double	tool_shank_radius = attribs.getReal( "Tool_Shank_Size", 0.5 ) / 2.0;

	double	tool_length = attribs.getReal( "Tool_Length", DEFAULT_TOOL_LENGTH );
	double	tool_cut_height = attribs.getReal( "Tool_Cut_Height", DEFAULT_TOOL_CUT_HEIGHT );
	double	tool_shoulder_height = attribs.getReal( "Tool_Shoulder_Height", (tool_cut_height / 4.0) );

	m_type.SetSize( 11, 10 );
	m_pt.SetSize( 11, 10 );
	
	m_type[0] = 1;
	m_pt[0] = C3dCoord( tool_shank_radius, tool_length, 0.0 );
	
	m_type[1] = 1;
	m_pt[1] = C3dCoord( -tool_shank_radius, tool_length, 0.0 );

	m_type[2] = 1;
	m_pt[2] = C3dCoord( -tool_shank_radius, tool_cut_height, 0.0 );

	m_type[3] = 1;
	m_pt[3] = C3dCoord( -tool_maj_radius, tool_cut_height, 0.0 );

	m_type[4] = 1;
	m_pt[4] = C3dCoord( -tool_maj_radius, (tool_cut_height - tool_shoulder_height), 0.0 );

	m_type[5] = 1;
	m_pt[5] = C3dCoord( -tool_min_radius, 0.0, 0.0 );

	m_type[6] = 1;
	m_pt[6] = C3dCoord( tool_min_radius, 0.0, 0.0 );

	m_type[7] = 1;
	m_pt[7] = C3dCoord( tool_maj_radius, (tool_cut_height - tool_shoulder_height), 0.0 );

	m_type[8] = 1;
	m_pt[8] = C3dCoord( tool_maj_radius, tool_cut_height, 0.0 );

	m_type[9] = 1;
	m_pt[9] = C3dCoord( tool_shank_radius, tool_cut_height, 0.0 );

	m_type[10] = 1;
	m_pt[10] = C3dCoord( tool_shank_radius, tool_length, 0.0 );
}

void
CToolShape::gen_xz_v_groove( const CVarList& attribs ) 
{
	double	tool_dia = attribs.getReal( "Tool_Dia", DEFAULT_TOOL_DIAMETER );
	double	tool_radius = 0.5 * tool_dia;

	double	tool_shank_radius = 0.5 * attribs.getReal( "Tool_Shank_Size", (0.75 * tool_dia) );

	double	tool_length = attribs.getReal( "Tool_Length", DEFAULT_TOOL_LENGTH );
	double	tool_cut_height = attribs.getReal( "Tool_Cut_Height", DEFAULT_TOOL_CUT_HEIGHT );
	double	tool_angle = attribs.getReal( "Tool_Angle", 60.0 ) / 2.0;
	double	dy = tool_radius * tan( (90.0 - tool_angle) * DEG2RAD );

	m_type.SetSize( 10, 10 );
	m_pt.SetSize( 10, 10 );
	
	m_type[0] = 1;
	m_pt[0] = C3dCoord( tool_shank_radius, tool_length, 0.0 );
	
	m_type[1] = 1;
	m_pt[1] = C3dCoord( -tool_shank_radius, tool_length, 0.0 );
	
	m_type[2] = 1;
	m_pt[2] = C3dCoord( -tool_shank_radius, tool_cut_height, 0.0 );
	
	m_type[3] = 1;
	m_pt[3] = C3dCoord( -tool_radius, tool_cut_height, 0.0 );
	
	m_type[4] = 1;
	m_pt[4] = C3dCoord( -tool_radius, dy, 0.0 );
	
	m_type[5] = 1;
	m_pt[5] = C3dCoord( 0.0, 0.0, 0.0 );
	
	m_type[6] = 1;
	m_pt[6] = C3dCoord( tool_radius, dy, 0.0 );
	
	m_type[7] = 1;
	m_pt[7] = C3dCoord( tool_radius, tool_cut_height, 0.0 );
	
	m_type[8] = 1;
	m_pt[8] = C3dCoord( tool_shank_radius, tool_cut_height, 0.0 );
	
	m_type[9] = 1;
	m_pt[9] = C3dCoord( tool_shank_radius, tool_length, 0.0 );
}

void
CToolShape::gen_xz_drill( const CVarList& attribs ) 
{
	double	tool_radius = attribs.getReal( "Tool_Dia", DEFAULT_TOOL_DIAMETER ) / 2.0;
	double	tool_length = attribs.getReal( "Tool_Length", DEFAULT_TOOL_LENGTH );
	double	tool_angle = attribs.getReal( "Tool_Angle", 60.0 ) / 2.0;
	double	dy = tool_radius * tan( (90.0 - tool_angle) * DEG2RAD );

	m_type.SetSize( 6, 10 );
	m_pt.SetSize( 6, 10 );
	
	m_type[0] = 1;
	m_pt[0] = C3dCoord( tool_radius, tool_length, 0.0 );
	
	m_type[1] = 1;
	m_pt[1] = C3dCoord( -tool_radius, tool_length, 0.0 );
	
	m_type[2] = 1;
	m_pt[2] = C3dCoord( -tool_radius, dy, 0.0 );
	
	m_type[3] = 1;
	m_pt[3] = C3dCoord( 0.0, 0.0, 0.0 );
	
	m_type[4] = 1;
	m_pt[4] = C3dCoord( tool_radius, dy, 0.0 );
	
	m_type[5] = 1;
	m_pt[5] = C3dCoord( tool_radius, tool_length, 0.0 );
}

void
CToolShape::gen_xz_saw( const CVarList& attribs ) 
{
	double	tool_radius = 0.5 * attribs.getReal( "Saw_Kerf", 0.125 );
	double	tool_length = attribs.getReal( "Saw_Dia", 1.0 );

	m_type.SetSize( 5, 10 );
	m_pt.SetSize( 5, 10 );
	
	m_type[0] = 1;
	m_pt[0] = C3dCoord( tool_radius, tool_length, 0.0 );
	
	m_type[1] = 1;
	m_pt[1] = C3dCoord( -tool_radius, tool_length, 0.0 );
	
	m_type[2] = 1;
	m_pt[2] = C3dCoord( -tool_radius, 0.0, 0.0 );
	
	m_type[3] = 1;
	m_pt[3] = C3dCoord( tool_radius, 0.0, 0.0 );
	
	m_type[4] = 1;
	m_pt[4] = C3dCoord( tool_radius, tool_length, 0.0 );
}

// TODO: gen_xz_custom() -- read pts from ctg ?
void
CToolShape::gen_xz_custom( const CVarList& attribs ) 
{
	CString		ctg_folder;
	CString		tool_name;
	CString		fpath;
	C3dCoord	pt;
	float		dval1, dval2;
	int			ival;
	FILE*		f;

	ctg_folder = attribs.getString( "ctg_folder", "" );
	tool_name = attribs.getString( "Tool_Description", "" );
	fpath.Format( "%s\\ctg\\%s.ctg", ctg_folder, tool_name );
	f = (fpath.IsEmpty() ? NULL : fopen( fpath, "r" ));

	if (f == NULL)
	{
		m_type.SetSize( 4, 16 );
		m_pt.SetSize( 4, 16 );

		dval1 = (float) (2. * cos( QUARTERPI ));

		m_type[0] = 1;
		m_pt[0] = C3dCoord( dval1, dval1, 0.);

		m_type[1] = 2;
		m_pt[1] = C3dCoord( 0., 0., 0.);

		m_type[2] = 1;
		m_pt[2] = C3dCoord( dval1, dval1, 0.);

		m_type[3] = 1;
		m_pt[3] = C3dCoord( -dval1, -dval1, 0.);
	}
	else
	{
		m_type.SetSize( 0, 16 );
		m_pt.SetSize( 0, 16 );

		while (fscanf( f, "%d%f%f", &ival, &dval1, &dval2 ) != EOF)
		{
			pt.XYZ( dval1, dval2, 0. );
			m_type.Add( ival );
			m_pt.Add( pt );
		}

		fclose( f );
	}
}

void
CToolShape::gen_xz_round( const CVarList& attribs ) 
{
	double radius = attribs.getReal( "Tool_Dia", DEFAULT_TOOL_DIAMETER ) / 2.0;
	double z_axis = attribs.getReal( "Tool_Length", DEFAULT_TOOL_LENGTH );

	m_type.SetSize( 6, 10 );
	m_pt.SetSize( 6, 10 );
	
	m_type[0] = 1;
	m_pt[0] = C3dCoord(radius, radius, 0.0);

	m_type[1] = 1;
	m_pt[1] = C3dCoord(radius, z_axis, 0.0);

	m_type[2] = 1;
	m_pt[2] = C3dCoord(-radius, z_axis, 0.0);

	m_type[3] = 1;
	m_pt[3] = C3dCoord(-radius, radius, 0.0);

	m_type[4] = 3;
	m_pt[4] = C3dCoord(0.0, radius, 0.0);

	m_type[5] = 1;
	m_pt[5] = C3dCoord(radius, radius, 0.0);
}
