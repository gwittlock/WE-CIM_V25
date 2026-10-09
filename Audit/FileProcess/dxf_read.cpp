//========================================================================
//		DXF File Interface
//
//		Read Methods
//
//		NOTE: DXF ortho-ref planes are numbered as such:
//
//			Generic XY -- 0
//
//			OCS	0, 0, 1			0		OCS_XY_POS
//				0, 0, -1		1		OCS_XY_NEG
//				1, 0, 0			2		OCS_YZ_POS
//				-1, 0, 0		3		OCS_YZ_NEG
//				0, 1, 0			4		OCS_ZX_POS
//				0, -1, 0		5		OCS_ZX_NEG
//
// ============================================================================

#include "stdafx.h"

#include "dxf.h"
#include "3dVec.h"
#include "token.h"

#include "MathConst.h"
#include "StringConst.h"

#include "GeoPoint.h"
#include "GeoLine.h"
#include "GeoArc.h"

#include "DbWorkplane.h"
#include "DbTool.h"
#include "DbLine.h"
#include "DbArc.h"
#include "DbHole.h"
#include "DbPoint.h"
#include "AutoModDb.h"
#include "ModelUtil.h"

#include "CadLayer.h"
#include "CadEntity.h"

static CString XY_POS( "XY_POS" );
static CString XY_NEG( "XY_NEG" );
static CString YZ_POS( "YZ_POS" );
static CString YZ_NEG( "YZ_NEG" );
static CString ZX_POS( "ZX_POS" );
static CString ZX_NEG( "ZX_NEG" );


//========================================================================
//		read_pair
//
//		Read two lines from the DXF file -- the first line
//		holds the integer Group code, the second an arbitrary string
//		that will get interpreted elsewhere...
//
CReturn
CDxf::read_pair( void )
{
	CString	group_str;

	group_str = m_file.ReadLine();
	m_group = atoi(group_str);

	m_value = m_file.ReadLine();
	++m_lineNo;

	return CReturn( STATUS_OKAY );
}

//========================================================================
//		read_section
//
//		This is the largest chunk allowed in DXF.  For sections that we
//		recognize, will call down to specific sub-reads.  Otherwise,
//		reads and IGNORES until an end-section marker is found.
//
CReturn
CDxf::read_section( void )
{
	CReturn ret = read_pair();

	while ( !m_file.isEOF() && ret.isOkay() )
	{
		if (m_group == 0)
		{
			if (!m_value.CompareNoCase( "ENDSEC" ))
				break;
			else if (!m_value.CompareNoCase("BLOCK"))
				ret += read_entities("ENDBLK");
			else
				ret += read_pair();
		}
		else if (m_group == 2)
		{
			if (m_value.CompareNoCase("HEADER") == 0)
				ret += read_header();
			else if (m_value.CompareNoCase("TABLES") == 0)
				ret += read_tables();
			else if (m_value.CompareNoCase("ENTITIES") == 0)
				ret += read_entities("ENDSEC");
			else
				ret += read_pair();
		}
		else
		{
			ret += read_pair();
		}
	}

	return ret;
}

//========================================================================
//		read_header
//
//		Tables are one section type.  There are many types of tables,
//		and we read and ignore most of them.  If we find a table type
//		we understand, calls down to a specific read.
//
CReturn
CDxf::read_header( void )
{
	CReturn ret = read_pair();

	while ( !m_file.isEOF() && ret.isOkay() )
	{
		if (m_group == 9)
		{
			if (!m_value.CompareNoCase( "$UCSORG" ))
				ret += read_ucs();
			else
				ret = read_pair();
		}
		else if (m_group == 0)
		{
			break;  // ENDSEC
		}
		else
		{
			ret = read_pair();
		}
	}

	return ret;
}

CReturn
CDxf::read_ucs( void )
{
	CReturn ret = read_pair();

	while ( !m_file.isEOF() && ret.isOkay() )
	{
		switch (m_group)
		{
		case 10:	m_xorg = atof(m_value);		break;
		case 20:	m_yorg = atof(m_value);		break;
		case 30:	m_zorg = atof(m_value);		break;
		case  9:	return ret;
		}
		ret += read_pair();
	}

	return ret;
}

//========================================================================
//		read_tables
//
//		Tables are one section type.  There are many types of tables,
//		and we read and ignore most of them.  If we find a table type
//		we understand, calls down to a specific read.
//
CReturn
CDxf::read_tables( void )
{
	CReturn ret = read_pair();

	while ( !m_file.isEOF() && ret.isOkay() )
	{
		if (m_group == 0)
		{
			if (!m_value.CompareNoCase( "ENDSEC" ))
				break;
			else if (!m_value.CompareNoCase( "TABLE" ))
				ret += read_one_table();
			else
				ret = read_pair();
		}
		else
		{
			ret = read_pair();
		}
	}

	return ret;
}

//========================================================================
//		read_one_table
//
//		Dummy table wrapper
//
CReturn
CDxf::read_one_table( void )
{
	CReturn ret = read_pair();

	while ( !m_file.isEOF()	&& ret.isOkay() )
	{
		if (m_group == 0)
		{
			if ( (m_value.CompareNoCase("ENDTAB") == 0) ||
				 (m_value.CompareNoCase("ENDSEC") == 0) )
			{
				break;
			}
		}

		ret += read_pair();
	}

	return ret;
}


//========================================================================
//		read_layers
//
//		Read layer information; a table.  Stores it specially; how it is
//		interpreted is a future problem.
//
//		TABLE
//			LAYER
//			  *
//			  *
//			LAYER
//			  *
//			  *
//		ENDTAB
//
CReturn
CDxf::read_layers( void )
{
	CString layerName;
	int layerColor = -1;

	CReturn ret = read_pair();

	while ( !m_file.isEOF() && ret.isOkay() )
	{
		switch (m_group)
		{
		case 0:
			if ( (m_value.CompareNoCase("LAYER") == 0)  ||
				 (m_value.CompareNoCase("ENDTAB") == 0) ||
				 (m_value.CompareNoCase("ENDSEC") == 0) )
			{
				if (layerColor >= 0)
				{
					// We've encountered a visible layer.
					// And though we should never encounter
					// an empty layer name ....
					if ( !layerName.IsEmpty() )
						m_importUtil.LayerAdd( layerName );
				}

				if (m_value.CompareNoCase("LAYER") != 0)
					return ret;
			}
			break;

		case 2:  // Layer name
			layerName = m_value;
			break;

		case 62:  // Layer color
			layerColor = atoi( m_value );
			break;
		}

		ret += read_pair();
	}

	return ret;
}

//========================================================================
//		read_entities
//
//		Reads the entity section.  Ignores many entity types, and calls
//		down to utility routines for entities we understand.
//
CReturn
CDxf::read_entities( const CString& terminal )
{
	CReturn ret = read_pair();

	while ( !m_file.isEOF() && ret.isOkay() )
	{
		if (m_group == 0)
		{
			if (m_value.CompareNoCase(terminal) == 0)
				break;
			else if (m_value.CompareNoCase("LINE") == 0)
				ret += read_line();
			else if (m_value.CompareNoCase("POLYLINE") == 0)
				ret += read_polyline();
			else if (m_value.CompareNoCase("ARC") == 0)
				ret += read_arc();
			else if (m_value.CompareNoCase("CIRCLE") == 0)
				ret += read_circle();
			else if (m_value.CompareNoCase("POINT") == 0)
				ret += read_point();
			else if (m_value.CompareNoCase("TEXT") == 0)
				ret += read_text();
			else
				ret += read_pair();
		}
		else
		{
			ret += read_pair();
		}
	}

	return ret;
}

//========================================================================
//		parse_common
//
//	Interpret common GROUP fields... and adjust the active
//	state to match.
//
bool
CDxf::parse_common( void )
{
	switch (m_group)
	{
	case 8:
		m_layer = m_value;
		return TRUE;
	default:
		return FALSE;
	}
}

//========================================================================
//		read_line
//
//		Read a line entity, and convert to a Line element in the part
//
CReturn
CDxf::read_line( void )
{
	CReturn		ret;
	CCadEntity*	cadEntity;
	CGeoLine*	geoLine;
	C3dCoord	ps( 0.0, 0.0, 0.0 );
	C3dCoord	pe( 0.0, 0.0, 0.0 );
	C3dVec		normal( 0.0, 0.0, 1.0 );
	double		depth = 0.0;

	// --------------------------------------------------------------

	ret += read_pair();
	while ( !m_file.isEOF() && ret.isOkay() )
	{
		if (!parse_common())
		{
			switch (m_group)
			{
			case 0:

				if (m_acceptedLayers.Find( m_layer ) >= 0)
				{
					geoLine = new CGeoLine( ps, pe );
					cadEntity = new CCadEntity( 0, m_layer, normal, geoLine );
					m_entities.Append( cadEntity );
				}
				return ret;

			case 39:
				depth = atof(m_value);
				break;

			case 10:
				ps.X( atof(m_value) - m_xorg );
				break;
			case 20:
				ps.Y( atof(m_value) - m_yorg );
				break;
			case 30:
				ps.Z( atof(m_value) - m_zorg );
				break;

			case 11:
				pe.X( atof(m_value) - m_xorg );
				break;
			case 21:
				pe.Y( atof(m_value) - m_yorg );
				break;
			case 31:
				pe.Z( atof(m_value) - m_zorg );
				break;
			}
		}
		ret += read_pair();
	}

	return ret;
}


//========================================================================
//		read_polyline
//
//		Read a polyline entity, and convert to a series of Line elements
//		in the part
//
CReturn
CDxf::read_polyline( void )
{
	CReturn		ret;
	CCadEntity*	cadEntity;
	CGeoLine*	geoLine;
	CGeoArc*	geoArc;
	C3dCoord	vertexA( 0.0, 0.0, 0.0 );
	C3dCoord	vertexB( 0.0, 0.0, 0.0 );
	C3dCoord	anchor;
	C3dVec		normal( 0.0, 0.0, 1.0 );
	double		depth = 0.0;
	double		inc_ang;
	double		pending_inc_ang;
	bool		anchored = FALSE;
	bool		closed = FALSE;

	// --------------------------------------------------------------

	ret = read_pair();
	while ( !m_file.isEOF() && ret.isOkay() )
	{
		bool read = TRUE;

		if (!parse_common())
		{
			switch (m_group)
			{
			case 0:
				if (m_value.CompareNoCase("VERTEX") == 0)
				{
					if (m_acceptedLayers.Find( m_layer ) >= 0)
					{

						if ( !anchored )
						{
							read_vertex( &anchor, &inc_ang );
							vertexB = anchor;

							anchored = TRUE;
						}
						else
						{
							vertexA = vertexB;
							pending_inc_ang = inc_ang;

							read_vertex( &vertexB, &inc_ang );

							if (fabs(pending_inc_ang) > SMALL)
							{
								geoArc = AcadArc( vertexA, vertexB, pending_inc_ang );
								cadEntity = new CCadEntity( 0, m_layer, normal, geoArc );
							}
							else
							{
								geoLine = new CGeoLine( vertexA, vertexB );
								cadEntity = new CCadEntity( 0, m_layer, normal, geoLine );
							}

							m_entities.Append( cadEntity );
						}
						read = FALSE;
					}
				}
				else if (m_value.CompareNoCase("SEQEND") == 0)
				{
					if ( closed )
					{
						geoLine = new CGeoLine( vertexB, anchor );
						cadEntity = new CCadEntity( 0, m_layer, normal, geoLine );
						m_entities.Append( cadEntity );
					}
					return ret;
				}
				else
				{
					return ret;
				}
				
				break;

			case 39:
				depth = atof(m_value);
				break;

			case 70:
				if (atoi(m_value) == 1)
					closed = TRUE;
				break;
			}
		}

		if ( read )
			read_pair();
	}

	return ret;
}

//========================================================================
//		read_vertex
//
//		Read a single vertex and related attributes into storage
//
CReturn
CDxf::read_vertex( C3dCoord* vertex, double* inc_ang )
{
	CReturn ret = read_pair();

	vertex->X( UNDEFINED );
	vertex->Y( UNDEFINED );
	vertex->Z( 0. );

	(*inc_ang) = 0.;

	while ( !m_file.isEOF() && ret.isOkay() )
	{
		if (!parse_common())
		{
			switch (m_group)
			{
			case 0:
				return ret;

			case 39:
				// Thickness;
				break;

			case 42:
				(*inc_ang) = 4. * atan( atof(m_value) );
				break;

			case 10:
				vertex->X( atof(m_value) - m_xorg );
				break;
			case 20:
				vertex->Y( atof(m_value) - m_yorg );
				break;
			case 30:
				vertex->Z( atof(m_value) - m_zorg);
				break;
			}
		}
		ret = read_pair();
	}

	return ret;
}

//========================================================================
//		read_arc
//
//		Read an arc entity, and convert to an Arc element in the part
//
CReturn
CDxf::read_arc( void )
{
	CReturn		ret;
	CCadEntity*	cadEntity;
	CGeoArc*	geoArc;
	int			flip = 0;
	C3dCoord	ps( 0.0, 0.0, 0.0 );
	C3dCoord	pe( 0.0, 0.0, 0.0 );
	C3dCoord	pc( 0.0, 0.0, 0.0 );
	double		radius;
	double		st_ang;
	double		en_ang;
	C2dUnitVec	st_vect;
	C2dUnitVec	en_vect;
	C3dVec		normal( 0.0, 0.0, 1.0 );
	double		depth = 0.0;

	// --------------------------------------------------------------

	ret += read_pair();
	while ( !m_file.isEOF() && ret.isOkay() )
	{
		if (!parse_common())
		{
			switch (m_group)
			{
			case 0:

				if (m_acceptedLayers.Find( m_layer ) >= 0)
				{
					st_vect = C2dUnitVec( st_ang );
					en_vect = C2dUnitVec( en_ang );

					ps = C3dCoord( pc.X() + st_vect.X() * radius,
								   pc.Y() + st_vect.Y() * radius,
								   pc.Z() );

					pe = C3dCoord( pc.X() + en_vect.X() * radius,
								   pc.Y() + en_vect.Y() * radius,
								   pc.Z() );

					if ( !ps.WithinTolXY( pe, SMALL ) )  // ie. a non-zero length arc
					{
						// NOTE: AutoCad always defines arcs a CCW.
						if ( flip )
						{
							ps.X( -ps.X() );
							pe.X( -pe.X() );
							pc.X( -pc.X() );
							geoArc = new CGeoArc( ps, pe, pc, CW );
						}
						else
						{
							geoArc = new CGeoArc( ps, pe, pc, CCW );
						}

						cadEntity = new CCadEntity( 0, m_layer, normal, geoArc );
						m_entities.Append( cadEntity );
					}
				}
				return ret;

			case 39:
				depth = atof(m_value);
				break;

			case 10:
				pc.X( atof(m_value) - m_xorg );
				break;
			case 20:
				pc.Y( atof(m_value) - m_yorg );
				break;
			case 30:
				pc.Z( atof(m_value) - m_zorg );
				break;

			case 40:
				radius = atof(m_value);
				break;

			case 50:
				st_ang = DEG2RAD * atof(m_value);
				break;
			case 51:
				en_ang = DEG2RAD * atof(m_value);
				break;

			case 230:
				flip = (atof(m_value) < 0.0);
				break;
			}
		}
		ret += read_pair();
	}

	return ret;
}

//========================================================================
//		read_circle
//
//		Read a circle entity, and convert to an Arc element in the part
//
CReturn
CDxf::read_circle( void )
{
	CReturn		ret;
	CCadEntity*	cadEntity;
	CGeoArc*	geoArc;
	C3dCoord	ps( 0.0, 0.0, 0.0 );
	C3dCoord	pe( 0.0, 0.0, 0.0 );
	C3dCoord	pc( 0.0, 0.0, 0.0 );
	C3dVec		normal;
	double		radius;
	double		depth = 0.0;
	int			flip = 0;

	// --------------------------------------------------------------

	ret += read_pair();
	while ( !m_file.isEOF() && ret.isOkay() )
	{
		if (!parse_common())
		{
			switch (m_group)
			{
			case 0:

				if (m_acceptedLayers.Find( m_layer ) >= 0)
				{
					ps = C3dCoord( (pc.X() + radius), pc.Y(), pc.Z() );
					pe = C3dCoord( (pc.X() + radius), pc.Y(), pc.Z() );

					normal.Init( 0., 0., (flip ? -1. : 1.) );
					geoArc = new CGeoArc( ps, pe, pc, CCW );
					cadEntity = new CCadEntity( 0, m_layer, normal, geoArc );
					m_entities.Append( cadEntity );
				}
				return ret;

			case 39:
				depth = atof(m_value);
				break;

			case 10:
				pc.X( atof(m_value) - m_xorg );
				break;
			case 20:
				pc.Y( atof(m_value) - m_yorg );
				break;
			case 30:
				pc.Z( atof(m_value) - m_zorg );
				break;

			case 40:
				radius = atof(m_value);
				break;

			case 230:
				flip = (atof(m_value) < 0.0);
				break;
			}
		}
		ret += read_pair();
	}

	return ret;
}

//========================================================================
//		read_point
//
//		Read a point entity, and convert to a Hole element in the part
//
// It was decided that we should ignore points unless a tool is associated
// with the target layer (per Tom & Gary 07/27/2000)
//
// See also: CAutoMod::HoledFeaturesCreate( CDbTool* dbTool )
//
CReturn
CDxf::read_point( void )
{
	CReturn		ret;
	CCadEntity*	cadEntity;
	CGeoPoint*	geoPoint;
	C3dCoord	ps( 0.0, 0.0, 0.0 );
	double		depth = 0.0;

	// --------------------------------------------------------------

	ret += read_pair();
	while ( !m_file.isEOF() && ret.isOkay() )
	{
		if (!parse_common())
		{
			switch (m_group)
			{
			case 0:

				if (m_acceptedLayers.Find( m_layer ) >= 0)
				{
					geoPoint = new CGeoPoint( ps );
					cadEntity = new CCadEntity( 0, m_layer, m_normal, geoPoint );
					m_entities.Append( cadEntity );
				}
				return ret;

			case 39:
				depth = atof(m_value);
				break;

			case 10:
				ps.X( atof(m_value) - m_xorg );
				break;
			case 20:
				ps.Y( atof(m_value) - m_yorg );
				break;
			case 30:
				ps.Z( atof(m_value) - m_zorg );
				break;
			}
		}
		ret += read_pair();
	}

	return ret;
}

//========================================================================
//		read_text
//
//		Read a text entity, and convert to a Command element in the part
//
CReturn
CDxf::read_text( void )
{
	CReturn		ret;
	CString		text;
	CCadEntity*	cadEntity;
	CGeoPoint*	geoPoint;
	C3dCoord	ps( 0.0, 0.0, 0.0 );
	double		depth = 0.0;
	double		orient = 0.0;
	bool		okay = true;  // default for preview.

	// --------------------------------------------------------------

	ret += read_pair();
	while ( !m_file.isEOF() && ret.isOkay() )
	{
		if (!parse_common())
		{
			switch (m_group)
			{
			case 0:

				if (m_autoModDb != NULL)
				{
					 okay = (m_autoModDb->LayerSetupAttributes().getInt( "ProcessText", FALSE ) != FALSE);
				}

				if ( okay )
				{
					if (m_acceptedLayers.Find( m_layer ) >= 0)
					{
						geoPoint = new CGeoPoint( ps );
						geoPoint->StringSet( "text", text );
						geoPoint->DoubleSet( "orient", orient );
						cadEntity = new CCadEntity( 0, m_layer, m_normal, geoPoint );
						m_entities.Append( cadEntity );
					}
				}
				return ret;

			case 1:
				text = m_value;
				break;

			case 10:
				ps.X( atof(m_value) - m_xorg );
				break;
			case 20:
				ps.Y( atof(m_value) - m_yorg );
				break;
			case 50:
				orient = atof(m_value);
				break;
			}
		}
		ret += read_pair();
	}

	return ret;
}

CReturn
CDxf::read_layer_table()
{
	CReturn status = read_pair();

	while ( !m_file.isEOF()	&& status.isOkay() )
	{
		if ((m_group == 2) && (m_value.CompareNoCase("LAYER") == 0))
		{
			status = read_layers();
			// break;
		}
		else if (m_group == 8)
		{
			// We have this branch because some dxf files are
			// written without a layer table.  This way, we
			// can collect layer names on-the-fly from each
			// entity (because each entity refers to its
			// layer by name).
			if ( !m_value.IsEmpty() )
				m_importUtil.LayerAdd( m_value );
		}

		status = read_pair();
	}

	return status;
}

int
CDxf::LayerCount()
{
	return m_importUtil.LayerCount();
}

CString
CDxf::LayerName( int index )
{
	return m_importUtil.LayerName( index );
}

CGeoArc*
CDxf::AcadArc( const C3dCoord& ps, const C3dCoord& pe, double inc_ang )
{
	C2dUnitVec	vec;
	C3dCoord	pc;
	double		half_ang;
	double		dist;
	double		radius;
	double		dx, dy;

	half_ang = 0.5 * inc_ang;

	dx = pe.X() - ps.X();
	dy = pe.Y() - ps.Y();
	dist = sqrt( dx*dx + dy*dy );

	radius = (0.5 * dist) / sin( fabs(half_ang) );

	vec.Init( dx, dy );
	vec += (SGN(half_ang) * (HALFPI - fabs(half_ang)));

	pc.X( ps.X() + (radius * vec.X()) );
	pc.Y( ps.Y() + (radius * vec.Y()) );
	pc.Z( ps.Z() );

	return ( new CGeoArc( ps, pe, pc, SGN(inc_ang) ) );
}
