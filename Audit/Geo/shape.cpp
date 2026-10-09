
#include "stdafx.h"
#include "StringConst.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "GeoReducer.h"
#include "Solution.h"
#include "Shape.h"

static const double SHAPE_TOL = 1.e-4;
static const double ALIGNMENT_TOL = 1.e-3;

////////////////////////////////////////////////////////////////////////

CShape::CShape()
	: m_type( SHAPE_TERMINAL ),
	  m_fully_normalize( TRUE )
{
}

// Copy constructor (duh!)
CShape::CShape( const CShape& shape )
{
	(*this) = shape;
}

CShape::~CShape()
{
	m_curves.DestructiveFlush();
}

// When TRUE, the 'reduce' parameter causes 'like' arcs to be merged
// and colinear points to be eliminated.
//
void
CShape::Init( const CGeoCurveArray& curves, bool reduce )
{
	m_curves.DestructiveFlush();

	int count = curves.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CGeoCurve* curve = curves[indx];
		CGeoCurve* copy = dynamic_cast<CGeoCurve*>( curve->Clone( false ) );

		m_curves.Append( copy );
	}

	Init( reduce );
}

// When TRUE, the 'reduce' parameter causes 'like' arcs to be merged
// and colinear points to be eliminated.
//
void
CShape::Init( bool reduce )
{
	const double TOL = 1.e-4;
	int count, indx;

	m_lineCount = 0;
	m_arcCount = 0;

	if ( reduce )
	{
		CGeoReducer::ArcsReduce( m_curves, true, TOL );
		CGeoReducer::LinesReduce( m_curves, true, TOL );
	}

	count = m_curves.Count();
	for (indx = 0; indx < count; ++indx)
	{
		CGeoCurve* curve = m_curves[indx];

		if (curve->Type() == GEOLINE)
			++m_lineCount;
		else if (curve->Type() == GEOARC)
			++m_arcCount;
	}

	Normalize();
	Encode();
}

void
CShape::FullNormalization( bool fully_normalize )
{
	m_fully_normalize = fully_normalize;
}

// Assignment (copy) operator.
const CShape&
CShape::operator = ( const CShape& shape )
{
	const CGeoCurve*	orig;
	CGeoCurve*			copy;
	int					count, indx;

	m_type = shape.m_type;

	m_attribs = shape.m_attribs;

	count = shape.Count();
	for (indx = 0; indx < count; ++indx)
	{
		orig = dynamic_cast<const CGeoCurve*>( shape[indx] );
		copy = dynamic_cast<CGeoCurve*>( orig->Clone( false ) );

		// A minor deal, to conserve memory.
		if ( orig->HasAttrib() )
			(*(copy->pAttrib())) = orig->Attrib();

		m_curves.Append( copy );
	}

	m_box = shape.m_box;
	m_pc = shape.m_pc;

	m_lineCount = shape.m_lineCount;
	m_arcCount = shape.m_arcCount;

	return (*this);
}

const C2dCoord&
CShape::Center() const
{
	if ( !m_box.IsDefined() )
		UpdateBox();

	return m_pc;
}

C2dBox
CShape::Box2d() const
{
	if ( !m_box.IsDefined() )
		UpdateBox();

	return m_box;
}

CShape*
CShape::Clone() const
{
	CShape* clone = new CShape();

	clone->FullNormalization( m_fully_normalize );

	clone->Init( m_curves, FALSE );

	return clone;
}

// Apply the given transformation to the curves of this poly.
void
CShape::Transform( const C3x4Matrix& xform )
{
	int count = m_curves.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CGeoCurve* curve = m_curves[indx];
		curve->Xform( xform );
	}

	m_box.Invalidate();
}

// Reduce this poly to those curves whose normals
// dot positive with the given vector.
void
CShape::Cull(
			const C2dUnitVec&	vec,
			int					offsetDir )
{
	int count = m_curves.Count();

	bool cull = (count > 0);

	if ( cull )
	{
		if (count == 1)
		{
			// Avoid culling culling when we have a single
			// 360 degree arc representing a round punch
			cull = (dynamic_cast<CGeoArc*>( m_curves[0] ) == NULL);
		}
	}

	if ( cull )
	{
		C2dUnitVec norm;

		double rot = ((offsetDir > 0) ? HALFPI : -HALFPI);

		int indx = 0;
		while (indx < m_curves.Count())
		{
			CGeoCurve* curve = m_curves[indx];

			CGeoLine* line = dynamic_cast<CGeoLine*>( curve );
			CGeoArc* arc = dynamic_cast<CGeoArc*>( curve );

			double dot = UNDEFINED;

			if (line != NULL)
			{
				norm = curve->StartTan() + rot;
				dot = norm * vec;
			}
			else if (arc != NULL)
			{
				norm = arc->StartTan() + rot;
				dot = norm * vec;

				if (dot <= 0)
				{
					norm = arc->EndTan() + rot;
					dot = norm * vec;

					if (dot <= 0)
					{
						C2dCoord pt = arc->MidPt();
						norm = arc->TanAtPt( pt.X(), pt.Y() ) + rot;
						dot = norm * vec;
					}
				}
			}

			if (dot <= 0)
				delete m_curves.Remove( indx );
			else
				++indx;
		}
	}

	m_box.Invalidate();
}

// Reduce this poly to those curves that are in the beneath the tool.
//
// NOTE: This design assumes the tool is positioned such that its
// center is on the X-axis and is offset from the Y-axis by half
// the width of the tool in its current orientation (rotation).
//
// This is done because when we attempt to dress the part with
// the tool, we want to raise the tool using the part geometry
// that lays beneath the tool.  We do not want to raise the tool
// by part geometry that is above the tool, and we do not want
// to drop the tool on part geometry that is too far below the tool.
//
void
CShape::Cull( const C2dBox& box, int down )
{
	// Shrink the bounding box slightly.
	double xmin = box.Xmin() + SMALL;
	double xmax = box.Xmax() - SMALL;
	double ymin = box.Ymin();
	double ymax = box.Ymax();

	int indx = 0;
	while (indx < m_curves.Count())
	{
		CGeoCurve* curve = m_curves[indx];

		const C3dBox& cbox = curve->Box();
		double cxmin = cbox.Xmin();
		double cxmax = cbox.Xmax();
		double cymin = cbox.Ymin();
		double cymax = cbox.Ymax();

		bool remove = FALSE;

		if (cxmax < xmin || cxmin > xmax)
		{
			// Remove this curve from consideration because it is
			// laterally displaced from the bounding box of the tool.

			remove = TRUE;
		}
		else if (cymax < ymin || cymin > ymax)
		{
			// Remove this curve from consideration because it is
			// vertically displaced from the bounding box of the tool.

			remove = TRUE;
		}

		if ( remove )
			delete m_curves.Remove( indx );
		else
			++indx;
	}

	m_box.Invalidate();
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Normalizes CShape objects such that they have
// consistent direction and parameterization.  For objects
// containing lines, the longest line is used as the basis
// for parameterization and will appear as the first entity
// in the object.  Otherwise, the arc having the largest
// radius will be used as the basis.
//
// TODO: Perhaps colinear point removal prior to finding the basis entity?
//
// Ensure this poly has a CW direction and has the longest
// line segment at the 0th position in its list.
void
CShape::Normalize()
{
	double best = -UNDEFINED;
	int bindx = 0;

	if ( m_fully_normalize )
		Direction( CW );

	// Find the basis entity.
	int count = m_curves.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CGeoCurve* geoCurve = m_curves[indx];

		CGeoLine* geoLine = dynamic_cast<CGeoLine*>( geoCurve );
		CGeoArc* geoArc = dynamic_cast<CGeoArc*>( geoCurve );

		if (m_lineCount > 0 && geoLine != NULL)
		{
			double length = geoLine->Length2d();
			if (length > (best + SMALL))
			{
				best = length;
				bindx = indx;
			}
		}
		else if (geoArc != NULL)
		{
			double radius = geoArc->Radius();
			if (radius > (best + SMALL))
			{
				best = radius;
				bindx = indx;
			}
		}
	}

	if (m_lineCount == 2 && m_arcCount == 2)
	{
		// Assume we have either an obround or a keyhole.
		// A keyhole will have two different radii.
		// If we have a keyhole, make m_curves[1] be
		// the major-diameter arc.

		CGeoLine* line = dynamic_cast<CGeoLine*>( m_curves[bindx] );

		if (line == NULL)
		{
			bindx = ((bindx == 3) ? 0 : bindx+1);
		}
		
		int indxA = ((bindx == 0) ? 3 : bindx-1);
		int indxB = ((bindx == 3) ? 0 : bindx+1);

		CGeoArc* arcA = dynamic_cast<CGeoArc*>( m_curves[indxA] );
		CGeoArc* arcB = dynamic_cast<CGeoArc*>( m_curves[indxB] );

		if ((arcA != NULL) && (arcB != NULL))
		{
			bindx = ((arcA->Radius() > arcB->Radius()) ? indxA-1 : indxB-1);
			if (bindx < 0)
				bindx = 3;
		}
	}

	// Reorder the entities.
	while (bindx > 0)
	{
		CGeoCurve* geoCurve = m_curves.Remove( 0 );
		m_curves.Append( geoCurve );
		--bindx;
	}
}

// WARNING: This poly must be Normalize()'d before it is Encode()'d.
// 1) Assigns this poly a 'code' attribute that can be used to
//    identify its curve composition.
// 2) Assigns this poly a 'shape' attribute that serves a similar
//    purpose to the 'code' attribute.
// 3) Unrecognized shapes have a 'shape' attribute of 'Unknown'.  
// 4) Recognized shapes have additional attributes such as 'width',
//    'length' and 'cradius', as required by the parametric definition
//    of the shape.
//
// Attributes generated by Encode()
//
// Encoding scheme
//		L -- line
//		A -- CW arc
//		a -- CCW arc
//		T -- tangent
//		C -- corner
//
void
CShape::Encode()
{
	CString		code;
	CGeoCurve*	curveA;
	CGeoCurve*	curveB;
	int			count, indx;

	count = m_curves.Count();
	if (count == 2)
	{
		if (m_curves[0]->Type() == GEOARC)
		{
			// 2006.03.13 (PE)
			// Assume we have encountered a Single-D shape
			// and transpose the curves so that the result
			// is properly encoded.
			curveA = m_curves.Remove(0);
			m_curves.Append( curveA );
		}
	}

	for (indx = 0; indx < count; ++indx)
	{
		int jndx = ((indx+1 >= count) ? 0 : indx+1);

		curveA = m_curves[indx];
		curveB = m_curves[jndx];

		// Encode the entity information.
		if (curveA->Type() == GEOLINE)
		{
			code += 'L';
		}
		else if (curveA->Type() == GEOARC)
		{
			CGeoArc* arc = dynamic_cast<CGeoArc*>( curveA );
			code += ((arc->Dir() > 0) ? 'a' : 'A');
		}

		// Encode the turn between the entities.
		C2dUnitVec tanA = curveA->EndTan();
		C2dUnitVec tanB = curveB->StartTan();

		double turn = Turn( tanA, tanB );
		code += ((fabs(turn) < 1.e-4) ? 'T' : 'C' );
	}

	m_attribs.setString( STR_CODE, code );

	Parameterize();
}

void
CShape::Parameterize()
{
	CString string = m_attribs.getString( STR_CODE, "" );

	if (string.Compare( "LCLCLCLC" ) == 0 )
	{
		ParameterizeParallelogram();  // no corner radius
	}
	else if (string.Compare( "LTATLTATLTATLTAT" ) == 0)
	{
		ParameterizeParallelogram();  // with corner radius
	}
	else if (string.Compare( "LTATLTAT" ) == 0)
	{
		ParameterizeObround();
	}
	else if (string.Compare( "AT" ) == 0)
	{
		ParameterizeRound();
	}
	else if (string.Compare( "LCAC" ) == 0)
	{
		ParameterizeSingleD();
	}
	else if (string.Compare( "LCACLCAC" ) == 0)
	{
		ParameterizeDoubleD();
	}
	else if (string.Compare( "LCLCLCLCLCLC" ) == 0)
	{
		ParameterizeHexagon();
	}
	else if (string.Compare( "LCACLTAT" ) == 0)
	{
		ParameterizeKeyhole();
	}
	
	// Classify all of the unclassified shapes.
	CString shape =	m_attribs.getString( STR_SHAPE, "" );
	if ( shape.IsEmpty() )
		m_attribs.setString( STR_SHAPE, STR_UNKNOWN );
}

// Rectangle, Square, Diamond, Trapezoid?
// LCLCLCLC -- without corner radii
// LTATLTATLTATLTAT -- with corner radii
void
CShape::ParameterizeParallelogram()
{
	CGeoLine* line[4] = { NULL, NULL, NULL, NULL };
	CGeoArc* arc[4] = { NULL, NULL, NULL, NULL };

	double perim = 0.0;
	double orient = 0.0;
	double cradius = 0.0;
	double width = 0.0;
	double length = 0.0;
	bool orthogonal = TRUE;
	CString shape = STR_UNKNOWN;

	CString code = m_attribs.getString( STR_CODE, "" );

	int offset = ((code[2] == 'A') ? 2 : 1);
	if (offset == 2)
	{
		// The shape has corner radii.  We can only parameterize the shape
		// if it is regular, that is, that it has similar corner radii.

		arc[0] = dynamic_cast<CGeoArc*>( m_curves[1] );
		arc[1] = dynamic_cast<CGeoArc*>( m_curves[3] );
		arc[2] = dynamic_cast<CGeoArc*>( m_curves[5] );
		arc[3] = dynamic_cast<CGeoArc*>( m_curves[7] );

		if ( (fabs(arc[0]->Radius() - arc[1]->Radius()) > SHAPE_TOL) ||
			 (fabs(arc[0]->Radius() - arc[2]->Radius()) > SHAPE_TOL) ||
			 (fabs(arc[0]->Radius() - arc[3]->Radius()) > SHAPE_TOL) )
		{
			return;
		}

		cradius = arc[0]->Radius();
	}

	line[0] = dynamic_cast<CGeoLine*>( m_curves[0*offset] );
	line[1] = dynamic_cast<CGeoLine*>( m_curves[1*offset] );
	line[2] = dynamic_cast<CGeoLine*>( m_curves[2*offset] );
	line[3] = dynamic_cast<CGeoLine*>( m_curves[3*offset] );

	// Get orthogonality and perimeter.
	for (int indx = 0; indx < 4; ++indx)
	{
		CGeoLine* lineA = line[indx];
		CGeoLine* lineB = line[ ((indx == 3) ? 0 : indx+1) ];

		C2dUnitVec tanA = lineA->StartTan();
		C2dUnitVec tanB = lineB->StartTan();

		double cross = tanA ^ tanB;
		if ((1.0 - fabs(cross)) > SMALL)
			orthogonal = FALSE;

		perim += lineA->Length2d();
	}

	if ( orthogonal )
	{
		// Rectangle, square?
		double lenA = line[0]->Length2d();

		orient = (line[2]->StartTan().Radians() * RAD2DEG);

		if (fabs(perim / 4 - lenA) < SHAPE_TOL)
		{
			width  = lenA + (2 * cradius);
			length = width;
			shape  = "Square";
			m_type = SHAPE_SQUARE;

			// Correct the orientation for symmetry
			while (orient >= (90.0 - ALIGNMENT_TOL))
				orient -= 90.0;
		}
		else
		{
			double lenB = line[1]->Length2d();

			width  = ((lenA > lenB) ? lenB : lenA) + (2 * cradius);
			length = ((lenA > lenB) ? lenA : lenB) + (2 * cradius);
			shape  = "Rectangle";
			m_type = SHAPE_RECTANGLE;

			// Correct the orientation for symmetry
			if (orient >= (180.0 - ALIGNMENT_TOL))
				orient -= 180.0;
		}
	}
	else
	{
		// Diamond, trapezoid?
		double lenA = line[0]->Length2d();

		if (fabs(perim / 4 - lenA) < SHAPE_TOL)
		{
			C2dVec vecA;
			C2dVec vecB;
			C2dUnitVec uvec;
			double distA, distB;

			if (cradius < SMALL)
			{
				vecA = line[0]->StartPt() - line[2]->StartPt();
				distA = vecA.Length();
				vecB = line[0]->EndPt() - line[2]->EndPt();
				distB = vecB.Length();
				uvec = C2dUnitVec( ((distA > distB) ? vecA : vecB) );
			}
			else
			{
				vecA = arc[0]->CenterPt() - arc[2]->CenterPt();
				distA = vecA.Length() + 2 * cradius;
				vecB = arc[1]->CenterPt() - arc[3]->CenterPt();
				distB = vecB.Length() + 2 * cradius;
				uvec = C2dUnitVec( ((distA > distB) ? vecA : vecB) );
			}

			orient = uvec.Radians() * RAD2DEG;

			// Correct the orientation for symmetry
			if (orient >= (180.0 - ALIGNMENT_TOL))
				orient -= 180.0;

			width  = ((distA > distB) ? distB : distA);
			length = ((distA > distB) ? distA : distB);
			shape  = "Diamond";
			m_type = SHAPE_DIAMOND;
		}
		else
		{
			double lenB = line[2]->Length2d();
			if ((lenA - lenB) > SMALL)
			{
				C2dUnitVec tanA;
				C2dUnitVec tanB;
				double crossA, crossB;
				
				orient = (line[2]->StartTan().Radians() * RAD2DEG);

				tanA = line[0]->EndTan();
				tanB = line[1]->StartTan();
				crossA = tanA ^ tanB;

				tanA = line[3]->EndTan();
				tanB = line[0]->StartTan();
				crossB = tanA ^ tanB;

				if (fabs(crossA - crossB) < SMALL)
				{
					C3dCoord closestPt;
					double u;

					C3dCoord refPt = line[2]->StartPt();

					double dot = (tanA + PI) * tanB;
					double ang = 2 * (HALFPI - acos(dot));
					m_attribs.setReal( "angle", (ang * RAD2DEG) );

					width  = line[0]->PointClosest( refPt, &closestPt, &u );
					length = lenA * (2 + cradius);
					shape  = "Trapezoid";
					m_type = SHAPE_TRAPEZOID;
				}
			}
		}
	}

	if (shape.Compare( STR_UNKNOWN ) != 0)
	{
		C2dCoord ptA = line[0]->MidPt();
		C2dCoord ptB = line[2]->MidPt();
		C2dCoord pc( (0.5 * (ptA.X() + ptB.X())), (0.5 * (ptA.Y() + ptB.Y())) );

		m_attribs.setReal( STR_WIDTH, width );
		m_attribs.setReal( STR_LENGTH, length );
		m_attribs.setReal( STR_CRADIUS, cradius );

		m_attribs.setReal( STR_ORIENT, orient );

		m_attribs.setReal( STR_OX, pc.X() );
		m_attribs.setReal( STR_OY, pc.Y() );

		m_attribs.setString( STR_SHAPE, shape );
	}
}

// LTATLTAT
void
CShape::ParameterizeObround()
{
	CGeoLine* lineA = dynamic_cast<CGeoLine*>( m_curves[0] );
	CGeoLine* lineB = dynamic_cast<CGeoLine*>( m_curves[2] );

	double lenA = lineA->Length2d();
	double lenB = lineB->Length2d();

	if (fabs(lenA-lenB) < SHAPE_TOL)
	{
		CGeoArc* arcA = dynamic_cast<CGeoArc*>( m_curves[1] );
		CGeoArc* arcB = dynamic_cast<CGeoArc*>( m_curves[3] );

		const C3dCoord& pcA = arcA->CenterPt();
		const C3dCoord& pcB = arcB->CenterPt();
		C2dCoord pc( (0.5 * (pcA.X() + pcB.X())), (0.5 * (pcA.Y() + pcB.Y())) );

		double orient = lineB->StartTan().Radians() * RAD2DEG;
		double width = 2 * arcA->Radius();

		// Correct the orientation for symmetry
		while (orient >= (180.0 - ALIGNMENT_TOL))
			orient -= 180.0;

		m_attribs.setReal( STR_WIDTH, width );
		m_attribs.setReal( STR_LENGTH, (lenA + width) );
		m_attribs.setReal( STR_ORIENT, orient );

		m_attribs.setReal( STR_OX, pc.X() );
		m_attribs.setReal( STR_OY, pc.Y() );

		m_attribs.setString( STR_SHAPE, "Obround" );
		m_type = SHAPE_OBROUND;
	}
}

// AT
void
CShape::ParameterizeRound()
{
	CGeoArc* arc = dynamic_cast<CGeoArc*>( m_curves[0] );

	const C3dCoord& pc = arc->CenterPt();
	double radius = arc->Radius();;

	m_attribs.setReal( STR_DIAMETER, (2 * radius) );
	m_attribs.setReal( STR_ORIENT, 0. );

	m_attribs.setReal( STR_OX, pc.X() );
	m_attribs.setReal( STR_OY, pc.Y() );

	m_attribs.setString( STR_SHAPE, "Round" );
	m_type = SHAPE_ROUND;
}

// LCAC
void
CShape::ParameterizeSingleD()
{
	CGeoLine* line = dynamic_cast<CGeoLine*>( m_curves[0] );
	CGeoArc* arc   = dynamic_cast<CGeoArc*>( m_curves[1] );

	const C3dCoord& pc = arc->CenterPt();

	double dist = 0.5 * line->Length2d();
	double radius = arc->Radius();
	double theta = arc->IncludedAngle();
	double orient = (line->StartTan().Radians() * RAD2DEG) + 180.0;

	if (fabs(orient-360.) < 0.01)
		orient = 0.;

	dist = sqrt( radius*radius - dist*dist );
	if ((theta + 0.001) > PI)
		dist += radius;

	m_attribs.setReal( STR_DIAMETER, (2 * radius) );
	m_attribs.setReal( STR_WIDTH, dist );
	m_attribs.setReal( STR_ORIENT, orient );

	m_attribs.setReal( STR_OX, pc.X() );
	m_attribs.setReal( STR_OY, pc.Y() );

	m_attribs.setString( STR_SHAPE, "SingleD" );
	m_type = SHAPE_SINGLE_D;
}

// LCACLCAC
void
CShape::ParameterizeDoubleD()
{
	CGeoLine* lineA = dynamic_cast<CGeoLine*>( m_curves[0] );
	CGeoLine* lineB = dynamic_cast<CGeoLine*>( m_curves[2] );
	CGeoArc* arcA = dynamic_cast<CGeoArc*>( m_curves[1] );
	CGeoArc* arcB = dynamic_cast<CGeoArc*>( m_curves[3] );

	double lenA = lineA->Length2d();
	double lenB = lineB->Length2d();
	double radA = arcA->Radius();
	double radB = arcB->Radius();
	double angA = arcA->IncludedAngle();
	double angB = arcB->IncludedAngle();

	// TODO: May also have to compare arc centers.
	if ( (fabs(lenA-lenB) < SHAPE_TOL) &&
		 (fabs(radA-radB) < SHAPE_TOL) &&
		 (fabs(angA-angB) < 0.01) )
	{
		const C3dCoord& pc = arcA->CenterPt();

		double dist = 0.5 * lenA;
		dist = 2 * sqrt( radA*radA - dist*dist );

		double orient = lineB->StartTan().Radians() * RAD2DEG;

		// Correct the orientation for symmetry
		if (orient >= (180.0 - ALIGNMENT_TOL))
			orient -= 180.0;

		m_attribs.setReal( STR_DIAMETER, (2 * radA) );
		m_attribs.setReal( STR_WIDTH, dist );
		m_attribs.setReal( STR_ORIENT, orient );

		m_attribs.setReal( STR_OX, pc.X() );
		m_attribs.setReal( STR_OY, pc.Y() );

		m_attribs.setString( STR_SHAPE, "DoubleD" );
		m_type = SHAPE_DOUBLE_D;
	}
}

// LCLCLCLCLCLC
void
CShape::ParameterizeHexagon()
{
	double base = UNDEFINED;
	double perim = 0.0;
	double angle = 0.0;
	double orient = 0.0;

	for (int indx = 0; indx < 6; ++indx)
	{
		CGeoLine* lineA = dynamic_cast<CGeoLine*>( m_curves[indx] );
		CGeoLine* lineB = dynamic_cast<CGeoLine*>( m_curves[((indx+1 == 6) ? 0 : indx+1)] );

		perim += lineA->Length2d();
		if (base == UNDEFINED)
		{
			base = perim;
			orient = (lineA->StartTan().Radians() * RAD2DEG) + 30.0;
		}

		angle += Turn( lineA->StartTan(), lineB->StartTan() );
	}

	if ( (fabs(perim / 6 - base) < SHAPE_TOL) &&
		 ((360.0 - fabs(angle)) < 0.001) )
	{
		const C3dCoord& ptA = m_curves[0]->StartPt();
		const C3dCoord& ptB = m_curves[3]->StartPt();
		C2dCoord pc( (0.5 * (ptA.X() + ptB.X())), (0.5 * (ptA.Y() + ptB.Y())) );

		// After working through the math ....
		m_attribs.setReal( STR_DIAMETER, (2 * base) );

		// Correct the orientation for symmetry
		while (orient >= (60.0 - ALIGNMENT_TOL))
			orient -= 60.0;

		m_attribs.setReal( STR_ORIENT, orient );

		m_attribs.setReal( STR_OX, pc.X() );
		m_attribs.setReal( STR_OY, pc.Y() );

		m_attribs.setString( STR_SHAPE, "Hexagon" );
		m_type = SHAPE_HEXAGON;
	}
}

// LTATLCAC
// LCACLTAT
void
CShape::ParameterizeKeyhole()
{
	CGeoLine* lineA = dynamic_cast<CGeoLine*>( m_curves[0] );
	CGeoLine* lineB = dynamic_cast<CGeoLine*>( m_curves[2] );

	double lenA = lineA->Length2d();
	double lenB = lineB->Length2d();

	if (fabs(lenA-lenB) < SHAPE_TOL)
	{
		C2dUnitVec vecA = lineA->StartTan();
		C2dUnitVec vecB = lineB->StartTan();
		double dot = vecA * vecB;

		if (fabs(1.0 + dot) < SMALL)
		{
			CGeoArc* arcA = dynamic_cast<CGeoArc*>( m_curves[1] );
			CGeoArc* arcB = dynamic_cast<CGeoArc*>( m_curves[3] );

			C2dUnitVec uvec = lineB->StartTan();

			double radA = arcA->Radius();
			double radB = arcB->Radius();
			double dist = sqrt( fabs( radA*radA - radB*radB ) );
			double orient  = uvec.Radians() * RAD2DEG;
			double length  = lenA + radA + radB + dist;

			m_attribs.setReal( "Major_diameter", (2 * radA) );
			m_attribs.setReal( "Minor_diameter", (2 * radB) );
			m_attribs.setReal( STR_LENGTH, length );
			m_attribs.setReal( STR_ORIENT, orient );

			C2dCoord pc = arcA->CenterPt() + (uvec * (0.5 * length - radA));
			m_attribs.setReal( STR_OX, pc.X() );
			m_attribs.setReal( STR_OY, pc.Y() );

			m_attribs.setString( STR_SHAPE, "Keyhole" );
			m_type = SHAPE_KEYHOLE;
		}
	}
}

double
CShape::Turn( const C2dUnitVec& vecA, const C2dUnitVec& vecB )
{
	double dot = vecA * vecB;
	double cross = vecA ^ vecB;

	// if (fabs(cross) < VECTOR_SMALL)
	if ((1. - fabs(dot)) < (2 * SMALL))  // tangent within ~0.0001 degrees
		return ((dot > 0) ? 0.0 : 180.0);
	else
		return (acos( dot ) * SGN( cross ) * RAD2DEG);
}

void
CShape::UpdateBox() const
{
	int count = m_curves.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CGeoCurve* curve = m_curves[indx];

		const C3dBox& box = curve->Box();

		((CShape*) this)->m_box.X( box.Xmin() );
		((CShape*) this)->m_box.X( box.Xmax() );
		((CShape*) this)->m_box.Y( box.Ymin() );
		((CShape*) this)->m_box.Y( box.Ymax() );
	
		double xc = (m_box.Xmin() + m_box.Xmax()) / 2;
		double yc = (m_box.Ymin() + m_box.Ymax()) / 2;
		((CShape*) this)->m_pc.XY( xc, yc );
	}
}

bool
CShape::Direction( int dir )
{
	// BugID: 44 -- AutoIndexOffset offset direction gets confused.
	// This was because the winding direction was inappropriately
	// changed for a part profile consisting of a single line.

	bool reverse = FALSE;

	int count = m_curves.Count();

	if (count > 1 || (count == 1 && m_curves[0]->Type() == GEOARC))
	{
		double area = CSolution::Area( m_curves );
		
		reverse = (SGN( area ) != SGN( dir ));

		if ( reverse )
			Reverse();
	}

	return reverse;
}

void
CShape::Reverse()
{
	CGeoCurveList tmp;
	CGeoCurve* curve;
	CGeoArc* geoArc;
	int count, indx;

	count = m_curves.Count();

	for (indx = 0; indx < count; ++indx)
	{
		curve = m_curves.Remove( 0 );

		geoArc = dynamic_cast<CGeoArc*>( curve );
		if (geoArc != NULL)
		{
			// CAutoIndexRules::OutsideCut() requires us
			// to record the original winding direction.
			geoArc->IntSet( "dir", geoArc->Dir() );
		}

		curve->Reverse();

		tmp.Prepend( curve );
	}

	for (indx = 0; indx < count; ++indx)
	{
		curve = tmp[indx];
		m_curves.Append( curve );
	}
}


int CShape::AttribCount() const
{
	return (m_attribs.countVar());
}

int CShape::IntGet( const CString& name, int defval ) const
{
	return (m_attribs.getInt( name, defval ));
}

double CShape::DoubleGet( const CString& name, double defval ) const
{
	return (m_attribs.getReal( name, defval ));
}

CString CShape::StringGet( const CString& name, const CString& defval ) const
{
	return (m_attribs.getString( name, defval ));
}

void CShape::IntSet( const CString& name, int ival )
{
	m_attribs.setInt( name, ival );
}

void CShape::DoubleSet( const CString& name, double dval )
{
	m_attribs.setReal( name, dval );
}

void CShape::StringSet( const CString& name, const CString& sval )
{
	m_attribs.setString( name, sval );
}

void CShape::AttribsDelete()
{
	m_attribs.Reset();
}

void CShape::AttribDelete( const CString& name )
{
	m_attribs.deleteVar( name );
}
