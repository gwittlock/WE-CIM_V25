
#include "stdafx.h"
#include "MathConst.h"
#include "StringConst.h"
#include "ToolConst.h"
#include "3dVec.h"
#include "3x4Matrix.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "Solution.h"
#include "ToolShape.h"
#include "ToolShapeExtruder.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


CToolShapeExtruder::CToolShapeExtruder()
{
}

CToolShapeExtruder::~CToolShapeExtruder()
{
}

CReturn
CToolShapeExtruder::Extrude(
			const CGeoCurve&	geoCurve,
			const CVarList&		descrip,
			CGeoPoly*			geoPoly )
{
	CReturn status;

	int type_id = descrip.getInt( STR_TYPE_ID, IUNDEFINED );

	// 2004.07.04 (PE) -- At first implementation (experimental),
	// all tools are treated as if they are auto-indexable.  This
	// may yield erroneous results.  If so, we can extend the
	// implementation to handle appropriately oriented tools.

	/*
	int autoIndex = descrip.getInt( STR_AUTO_INDEX, IUNDEFINED );
	double indexAngle = descrip.getReal( STR_INDEX_ANGLE, UNDEFINED );
	*/

	geoPoly->Flush();

	switch (type_id)
	{
	case TTYPE_ROUND:
		// Probably never called because nesting treats
		// round tools as if they were a burning tool.
		extrude_2d_round( geoCurve, descrip, geoPoly );
		break;

	case TTYPE_SQUARE:
		extrude_2d_square( geoCurve, descrip, geoPoly );
		break;

	case TTYPE_RECTANGLE:
	case TTYPE_FORMING:
		extrude_2d_rect( geoCurve, descrip, geoPoly );
		break;

	case TTYPE_OBROUND:
		extrude_2d_obround( geoCurve, descrip, geoPoly );
		break;

	case TTYPE_DOUBLE_D:
		extrude_2d_dd( geoCurve, descrip, geoPoly );
		break;

	case TTYPE_TRAPEZOID:
		extrude_2d_trapezoid( geoCurve, descrip, geoPoly );
		break;

	case TTYPE_HEXAGON:
		extrude_2d_hexagon( geoCurve, descrip, geoPoly );
		break;
	}

	return status;
}


void
CToolShapeExtruder::extrude_2d_round(
			const CGeoCurve&	geoCurve,
			const CVarList&		descrip,
			CGeoPoly*			geoPoly )
{
	if (geoCurve.Type() == GEOLINE)
	{
		double radius, crvlen;

		crvlen = geoCurve.Length2d();

		if ( CToolShape::IsRoundTool( descrip ) )
			radius = CToolShape::EffectiveDiameter( descrip ) / 2.0;
		else
			radius = descrip.getReal( STR_WIDTH, 5. ) / 2.0;

		C3dCoord ct( 0., 0., 0. );

		C2dUnitVec	perp( 0., 1. );
		C2dUnitVec	back( -perp.Y(), perp.X() );

		double x_axis = descrip.getReal( STR_LENGTH, (2*radius) ) + (0.5 * crvlen);

		double half_x = x_axis / 2.0;
		double xflat = x_axis - (2 * radius);
		double half_xflat = xflat / 2.0;

		C3dVec	perp_long = perp * radius;
		C3dVec	back_short = back * half_xflat;


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

		C3x4Matrix	xform;
		xform.setXYAngle( geoCurve.StartTan().Radians() );
		xform.setT( geoCurve.StartPt() );

		CToolShape::Convert( m_type, m_pt, geoPoly );
		geoPoly->Xform( xform );
	}
}


void
CToolShapeExtruder::extrude_2d_square(
			const CGeoCurve&	geoCurve,
			const CVarList&		descrip,
			CGeoPoly*			geoPoly )
{
	if (geoCurve.Type() == GEOLINE)
	{
		C3dCoord ct( 0., 0., 0. );

		C2dUnitVec	perp( 0., 1. );
		C2dUnitVec	back( -perp.Y(), perp.X() );

		double crvlen = geoCurve.Length2d();

		double width = descrip.getReal( STR_WIDTH, 0. );

		double radius = descrip.getReal( "Corner_radius", 0. );

		double halfwide = width / 2.0;
		double flat = width - 2*radius;
		double halfflat = flat / 2.0;

		C3dVec	perp_long = perp * halfwide;
		C3dVec	perp_short = perp * halfflat;
		C3dVec	back_long = back * (halfwide + (0.5 * crvlen));
		C3dVec	back_short = back * (halfflat + (0.5 * crvlen));

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

		CToolShape::Convert( m_type, m_pt, geoPoly );

		C3x4Matrix	xform;
		xform.setXYAngle( geoCurve.StartTan().Radians() );
		xform.setT( geoCurve.MidPt() );
		geoPoly->Xform( xform );
	}
}

void
CToolShapeExtruder::extrude_2d_rect(
			const CGeoCurve&	geoCurve,
			const CVarList&		descrip,
			CGeoPoly*			geoPoly )
{
	if (geoCurve.Type() == GEOLINE)
	{
		C3dCoord ct( 0., 0., 0. );

		C2dUnitVec	perp( 0., 1. );
		C2dUnitVec	back( -perp.Y(), perp.X() );

		double crvlen = geoCurve.Length2d();

		double y_axis = descrip.getReal( STR_WIDTH, 0. );

		double x_axis = descrip.getReal( STR_LENGTH, y_axis );

		double radius = descrip.getReal( "Corner_radius", 0. );

		double half_y = y_axis / 2.0;
		double yflat = y_axis - 2*radius;
		double half_yflat = yflat / 2.0;

		double half_x = x_axis / 2.0;
		double xflat = x_axis - 2*radius;
		double half_xflat = xflat / 2.0;

		C3dVec	perp_long = perp * half_y;
		C3dVec	perp_short = perp * half_yflat;
		C3dVec	back_long = back * (half_x + (0.5 * crvlen));
		C3dVec	back_short = back * (half_xflat + (0.5 * crvlen));

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
	
		C3x4Matrix	xform;
		xform.setXYAngle( geoCurve.StartTan().Radians() );
		xform.setT( geoCurve.MidPt() );

		CToolShape::Convert( m_type, m_pt, geoPoly );
		geoPoly->Xform( xform );
	}
}

void
CToolShapeExtruder::extrude_2d_obround(
			const CGeoCurve&	geoCurve,
			const CVarList&		descrip,
			CGeoPoly*			geoPoly )
{
	if (geoCurve.Type() == GEOLINE)
	{
		C3dCoord ct( 0., 0., 0. );

		C2dUnitVec	perp( 0., 1. );
		C2dUnitVec	back( -perp.Y(), perp.X() );

		double crvlen = geoCurve.Length2d();

		double y_axis = descrip.getReal( STR_WIDTH, 0. );

		double x_axis = descrip.getReal( STR_LENGTH, y_axis ) + crvlen;

		double half_y = y_axis / 2.0;

		double half_x = x_axis / 2.0;
		double xflat = x_axis - y_axis;
		double half_xflat = xflat / 2.0;

		C3dVec	perp_long = perp * half_y;
		C3dVec	back_short = back * half_xflat;

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
	
		C3x4Matrix	xform;
		xform.setXYAngle( geoCurve.StartTan().Radians() );
		xform.setT( geoCurve.MidPt() );

		CToolShape::Convert( m_type, m_pt, geoPoly );
		geoPoly->Xform( xform );
	}
}

void
CToolShapeExtruder::extrude_2d_dd(
			const CGeoCurve&	geoCurve,
			const CVarList&		descrip,
			CGeoPoly*			geoPoly )
{
	if (geoCurve.Type() == GEOLINE)
	{
		C3dCoord ct( 0., 0., 0. );

		C2dUnitVec	perp( 0., 1. );
		C2dUnitVec	back( -perp.Y(), perp.X() );

		double crvlen = geoCurve.Length2d();

		double	width = descrip.getReal( STR_WIDTH, 0. );

		double diameter = descrip.getReal( STR_DIAMETER, 0. );

		double radius = diameter / 2.0;
		double rise = width/2.0;//radius - halfwide;
		double base = sqrt( radius*radius - rise*rise ) + (0.5 * crvlen);

		C3dVec	perp_rise = perp * rise;
		C3dVec	back_base = back * base;

		m_type.SetSize( 7, 10 );
		m_pt.SetSize( 7, 10 );

		m_type[0] = 1;
		m_pt[0] = ct + perp_rise - back_base;
		m_type[1] = 2;
		m_pt[1] = ct;
		m_pt[1].X( 0.5 * crvlen );
		m_type[2] = 1;
		m_pt[2] = ct - perp_rise - back_base;

		m_type[3] = 1;
		m_pt[3] = ct - perp_rise + back_base;
		m_type[4] = 2;
		m_pt[4] = ct;
		m_pt[4].X( -(0.5 * crvlen) );
		m_type[5] = 1;
		m_pt[5] = ct + perp_rise + back_base;

		m_type[6] = 1;
		m_pt[6] = m_pt[0];
	
		C3x4Matrix	xform;
		xform.setXYAngle( geoCurve.StartTan().Radians() );
		xform.setT( geoCurve.MidPt() );

		CToolShape::Convert( m_type, m_pt, geoPoly );
		geoPoly->Xform( xform );
	}
}

void
CToolShapeExtruder::extrude_2d_trapezoid(
			const CGeoCurve&	geoCurve,
			const CVarList&		descrip,
			CGeoPoly*			geoPoly )
{
	if (geoCurve.Type() == GEOLINE)
	{
		C3dCoord ct( 0., 0., 0. );

		C2dUnitVec	perp( 0., 1. );
		C2dUnitVec	back( -perp.Y(), perp.X() );

		double crvlen = geoCurve.Length2d();

		double	y_axis = descrip.getReal( STR_WIDTH, 0. );

		double x_axis =	descrip.getReal( STR_LENGTH, y_axis );

		double radius = descrip.getReal( "Corner_radius", 0. );

		double angle = descrip.getReal( "Angle", 0. );
		angle *= DEG2RAD;

		double half_y = y_axis / 2.0;
		double half_x = x_axis / 2.0;

		double top = half_x - (y_axis * tan( HALFPI - angle ));

		C3dVec	perp_long = perp * half_y;
		C3dVec	back_long = back * (half_x + (0.5 * crvlen));
		C3dVec	back_short = back * (top + (0.5 * crvlen));


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
	
		C3x4Matrix	xform;
		xform.setXYAngle( geoCurve.StartTan().Radians() );
		xform.setT( geoCurve.MidPt() );

		CToolShape::Convert( m_type, m_pt, geoPoly );
		geoPoly->Xform( xform );
	}
}

void
CToolShapeExtruder::extrude_2d_hexagon(
			const CGeoCurve&	geoCurve,
			const CVarList&		descrip,
			CGeoPoly*			geoPoly )
{
	if (geoCurve.Type() == GEOLINE)
	{
		C3dCoord ct( 0., 0., 0. );
		C2dUnitVec perp( 0., 1. );
		double dx, dy;

		double delta = geoCurve.Length2d() / 2.;

		//double stang = perp.Radians();
		double stang = 0.;

		double diameter = descrip.getReal( STR_DIAMETER, 1. );
		double radius = diameter / 2.0;

		m_type.SetSize( 7, 10 );
		m_pt.SetSize( 7, 10 );

		for (int idx=0; idx<7; idx++)
		{
			dx = radius * cos( stang + (idx*(TWOPI/6.0)) );
			dy = radius * sin( stang + (idx*(TWOPI/6.0)) );

			dx += (((idx > 1) && (idx < 5)) ? -delta : delta);

			m_type[idx] = 1;
			m_pt[idx].XYZ( dx, dy, 0. );
		}

		C3x4Matrix	xform;
		xform.setXYAngle( geoCurve.StartTan().Radians() );
		xform.setT( geoCurve.MidPt() );

		CToolShape::Convert( m_type, m_pt, geoPoly );
		geoPoly->Xform( xform );
	}
}
