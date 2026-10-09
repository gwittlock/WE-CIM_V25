
#include "stdafx.h"
#include "StringConst.h"
#include "Register.h"
#include "3x4Matrix.h"
#include "2dCoord.h"
#include "2dUnitVec.h"
#include "GeoPoint.h"
#include "GeoCurve.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "Int2d.h"
#include "AutoIndexRules.h"
#include "AutoIndex.h"


////////////////////////////////////////////////////////////////////////

CAutoIndex::CAutoIndex()
{
}

CAutoIndex::~CAutoIndex()
{
}

CReturn
CAutoIndex::Offset(
				const CShape&	part,
				const CShape&	tool,
				int				offsetDir,
				bool			use_long_side,
				CGeoElemList*	result )
{
	CReturn status;

	CAutoIndexRules rules;

	CShape partCopy = part;
	CShape toolCopy = tool;

	toolCopy.Direction( CW );

	rules.Init( partCopy, toolCopy, offsetDir, use_long_side );

	int stationID = tool.Attribs().getInt( STR_STATION_ID, 0 );
	int count = rules.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		const CAutoIndexRule& rule = (*(rules[indx]));

		C3dCoord ps = PositionCalculate(
			BACK_EDGE, rule, offsetDir, partCopy, toolCopy );

		C3dCoord pe = PositionCalculate(
			FRONT_EDGE, rule, offsetDir, partCopy, toolCopy );

		const CGeoCurve* pCurve = dynamic_cast<const CGeoCurve*>( partCopy[rule.m_partIndx] );
		double toolRotation = rule.m_toolRotation * RAD2DEG;

		CGeoElem* move = MoveCreate( ps, pe, pCurve, stationID, offsetDir, toolRotation );
		if (move != NULL)
			result->Append( move );
	}

	return status;
}

CGeoElem*
CAutoIndex::MoveCreate(
					const C3dCoord&		ps,
					const C3dCoord&		pe,
					const CGeoCurve*	pCurve,
					int					stationID,
					int					offsetDir,
					double				toolRotation )
{
	CGeoElem* move = NULL;

	if ( pe.WithinTol( ps, SMALL ) )
	{
		// Exact fit of tool face to part face.

		move = new CGeoPoint( ps );
	}
	else
	{
		const CGeoLine* pLine = dynamic_cast<const CGeoLine*>( pCurve );
		const CGeoArc* pArc = dynamic_cast<const CGeoArc*>( pCurve );

		if (pLine != NULL)
		{
			C2dUnitVec tanA = pLine->StartTan();
			C2dUnitVec tanB( (pe.X() - ps.X()), (pe.Y() - ps.Y()) );

			double dot = tanA * tanB;
			if (dot > 0)
			{
				// In other words, if the direction from ps to pe is opposite
				// the direction of the curve, then the tool could not be
				// properly positioned with respect to the current curve.

				move = new CGeoLine( ps, pe );
			}
		}
		else if (pArc != NULL)
		{
			C2dCoord pc = pArc->CenterPt();
			C2dVec vecA = ps - pc;
			C2dVec vecB = pe - pc;
			double diff = fabs(vecA.Length() - vecB.Length());

			if (diff < SMALL)
			{
				move = new CGeoArc( ps, pe, pc, pArc->Dir() );
			}
		}

	}

	if (move != NULL)
	{
		move->IntSet( STR_STATION_ID, stationID );
		move->IntSet( STR_CUTSIDE, offsetDir );
		move->DoubleSet( STR_ORIENT, toolRotation );
	}

	return move;
}

// Create copies of the part and tool such that
// 1) the part point is at 0,0 and the tangent of the associated
//    curve is aligned with the given orientation
// 2) the tool center is offset from the part point in the
//    given offset direction
//
// This way, the tool is already properly positioned relative
// to the transformed part to immediately find candidate curves
// in the shadow of the tool.
//
// TODO: Need class for CAutoIndexObject -- array of geo curves, etc ...
// ASSUMPTION: Tool geometry has CW orientation.
C3dCoord
CAutoIndex::PositionCalculate(
					EEdge					orient,
					const CAutoIndexRule&	rule,
					int						offsetDir,
					const CShape&	part,
					const CShape&	tool )
{
	CShape partCopy;
	C3x4Matrix xform;
	C3x4Matrix inverse;

	PartXformInit( orient, rule, &xform );

	partCopy = part;
	partCopy.Transform( xform );

	xform.InvertTo( &inverse );

	C3dCoord pos = ToolPositionGet( orient, rule, offsetDir, partCopy, tool );

	inverse.Transform( &pos );

	return pos;
}

C3dCoord
CAutoIndex::ToolPositionGet(
					EEdge					orient,
					const CAutoIndexRule&	rule,
					int						offsetDir,
					const CShape&	part,
					const CShape&	tool )
{
	CShape reducedPart;
	CShape toolCopy;
	C3x4Matrix xform;
	C2dBox box;
	C2dCoord pe;
	C3dCoord pc( 0., 0., 0. );
	const CGeoCurve* pCurve;
	const CGeoCurve* tCurve;
	double yc, alternateYc = UNDEFINED;


	pCurve = part[ rule.m_partIndx ];
	pe = ((orient == BACK_EDGE ) ? pCurve->StartPt() : pCurve->EndPt());

	// Initialize the transformation required to align
	// the tool face with the part face.
	ToolXformInit( orient, rule, &xform );

	// Since everything is relative to the center of the tool and
	// because the tool is defined with its center at the origin.
	xform.Transform( &pc );

	toolCopy = tool;
	toolCopy.Transform( xform );

	tCurve = toolCopy[ rule.m_toolIndx ];
	if (tCurve->Type() == GEOLINE)
	{
		// ASSUMPTIONS:
		// 1) The tool is designed such that its drive point is coincident
		// with the XY origin.  For symmetrical tools, this means the center
		// of the tool is the drive point.  This is the usual case (the only
		// known exception is a single-D whose width is less than its radius).
		// 2) The center of this transformed tool face is concident with the
		// origin of the transformed part.

		const CGeoArc* pArc = dynamic_cast<const CGeoArc*>( pCurve );
		if (pArc != NULL)
		{
			// First and foremost, we can not dress the inside of an arc
			// with a linear tool face.  This is determined by CAutoIndexRules.
			// Aside from that, if we are dressing the outside of the arc with
			// a linear tool face and the arc is 'free and clear', then the
			// mid-point of the line should be brought coincident with the
			// start point of the arc.

			alternateYc = 0.0;
		}
		else
		{
			alternateYc = fabs( tCurve->StartPt().Y() );
		}
	}

	box = toolCopy.Box2d();

	// Prevent consideration of entities not within
	// proximity of the tool.
	box.Ymax( pe.Y() + box.Dy() / 2 );

	// Reduce the tool to downward pointing faces.
	// ASSUMPTION: Tool has CW orientation
	toolCopy.Cull( C2dUnitVec( 0, -1 ), LEFT );

	// Reduce the part to upward pointing faces that
	// are beneath the tool.
	reducedPart = part;
	reducedPart.Cull( box, orient );
	reducedPart.Cull( C2dUnitVec( 0, 1 ), offsetDir );

	yc = ToolY( reducedPart, toolCopy, offsetDir );

	if (fabs(yc) < SMALL && alternateYc != UNDEFINED)
	{
		// ASSUMPTION: The tool was not raised by any entities
		// under the shadow of the tool.  Therefore, the part
		// face that is being dressed is 'free and clear'.

		if ( rule.m_oversizedToolFace )
		{
			// Center the tool face on the part face.

			yc = pCurve->Length2d() / 2;
		}
		else
		{
			// The leading/trailing end of the tool face should
			// be brought flush with the end of the part face.
			
			yc = alternateYc;
		}
	}

	return C3dCoord( pc.X(), yc, 0.0 );
}

// Check the tool against each element of the part that
// lays under the shadow of the tool.
double
CAutoIndex::ToolY(
					const CShape&	part,
					const CShape&	tool,
					int						offsetDir )
{
	double yc = 0.0;

	int count = part.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		const CGeoCurve* curve = part[indx];

		double yp = ToolY( (*curve), tool, offsetDir );

		if (yp > yc)
			yc = yp;
	}

	return yc;
}

// Check each face of the tool against the given part
// curve raising the tool to its highest position.
// The y-ordinate of the tool center is returned.
// TODO: Improve efficiency?
double
CAutoIndex::ToolY(
				const CGeoCurve&		partCurve,
				const CShape&	tool,
				int						offsetDir )
{
	const C3dBox& pbox = partCurve.Box();

	double yc = 0.0;

	int count = tool.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		const CGeoCurve* toolCurve = tool[indx];

		const C3dBox& tbox = toolCurve->Box();

		if ( Overlap( pbox, tbox ) )
		{
			const CGeoLine* partLine = dynamic_cast<const CGeoLine*>( &partCurve );
			const CGeoArc* partArc = dynamic_cast<const CGeoArc*>( &partCurve );

			const CGeoLine* toolLine = dynamic_cast<const CGeoLine*>( toolCurve );
			const CGeoArc* toolArc = dynamic_cast<const CGeoArc*>( toolCurve );
	
			double yp = -UNDEFINED;

			if (partLine && toolLine)
				yp = ToolY( (*partLine), (*toolLine), offsetDir );
			else if (partLine && toolArc)
				yp = ToolY( (*partLine), (*toolArc), offsetDir );
			else if (partArc && toolLine)
				yp = ToolY( (*partArc), (*toolLine), offsetDir );
			else if (partArc && toolArc)
				yp = ToolY( (*partArc), (*toolArc), offsetDir );

			if (yp > yc)
				yc = yp;
		}
	}

	return yc;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//                 The 'raising' methods.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// ASSUMPTIONS:
// 1) Vertical part faces have been culled.
// 2) We have a tool with CW orientation.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// The tool is positioned with its center at Y0.  Check the tool
// geometry against the part geometry that is in the 'shadow' of
// the tool.  Determine how far to raise the tool such that it
// will not interfere with the part.
double
CAutoIndex::ToolY(
				const CGeoLine&		partLine,
				const CGeoLine&		toolLine,
				int					offsetDir )
{
	C2dUnitVec tan;
	double slope;
	double dx0, dx1;

	double dyFinal = 0.0;
	double dyThis = -UNDEFINED;

	C2dCoord pps = partLine.StartPt();
	C2dCoord ppe = partLine.EndPt();

	C2dCoord tps = toolLine.StartPt();
	C2dCoord tpe = toolLine.EndPt();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Check the end points of the tool against the line.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	tan = partLine.StartTan();
	slope = tan.Y() / tan.X();

	dx0 = tps.X() - pps.X();
	dx1 = tps.X() - ppe.X();

	if ((dx0 * dx1) < SMALL)
	{
		// The tool point is in the shadow of the part.

		dyThis = (pps.Y() + (dx0 * slope)) - tps.Y();
		
		if (dyThis > dyFinal)
			dyFinal = dyThis;
	}

	dx0 = tpe.X() - pps.X();
	dx1 = tpe.X() - ppe.X();

	if ((dx0 * dx1) < SMALL)
	{
		// The tool point is in the shadow of the part.

		dyThis = (pps.Y() + (dx0 * slope)) - tpe.Y();
		
		if (dyThis > dyFinal)
			dyFinal = dyThis;
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Check the end points of the line against the tool.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	tan = toolLine.StartTan();
	slope = tan.Y() / tan.X();

	dx0 = pps.X() - tps.X();
	dx1 = pps.X() - tpe.X();

	if ((dx0 * dx1) < SMALL)
	{
		// The part point is in the shadow of the tool.

		dyThis = pps.Y() - (tps.Y() + (dx0 * slope));
		
		if (dyThis > dyFinal)
			dyFinal = dyThis;
	}

	dx0 = ppe.X() - tps.X();
	dx1 = ppe.X() - tpe.X();

	if ((dx0 * dx1) < SMALL)
	{
		// The part point is in the shadow of the tool.

		dyThis = ppe.Y() - (tps.Y() + (dx0 * slope));
		
		if (dyThis > dyFinal)
			dyFinal = dyThis;
	}

	return dyFinal;
}

double
CAutoIndex::ToolY(
				const CGeoLine&	partLine,
				const CGeoArc&	toolArc,
				int				offsetDir )
{
	double dx0, dx1, slope;
	C2dCoord tpt;
	C2dCoord ppt;

	double dyFinal = 0.0;
	double dyThis = -UNDEFINED;

	C2dCoord pps = partLine.StartPt();
	C2dCoord ppe = partLine.EndPt();

	// Find the tangency point on the tool.
	C2dUnitVec tan = partLine.StartTan();
	C2dUnitVec antiNorm = tan - (offsetDir * HALFPI);

	tpt = toolArc.CenterPt() + (antiNorm * toolArc.Radius());

	// Find the corresponding point on the part.
	slope = tan.Y() / tan.X();
	dx0 = tpt.X() - pps.X();

	ppt.XY( tpt.X(), (pps.Y() + (dx0 * slope)) );

	if ( partLine.PointOnSeg( ppt ) && toolArc.PointOnSeg( tpt ) )
	{
		// The tangency point is the solution.

		dyThis = ppt.Y() - tpt.Y();

		if (dyThis > dyFinal)
			dyFinal = dyThis;
	}
	else
	{
		// The tangency point is not the solution.

		C2dCoord tps = toolArc.StartPt();
		C2dCoord tpe = toolArc.EndPt();

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Check the end points of the tool against the line.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		dx0 = tps.X() - pps.X();
		dx1 = tps.X() - ppe.X();

		if ((dx0 * dx1) < SMALL)
		{
			// The tool point is in the shadow of the part.

			dyThis = (pps.Y() + (dx0 * slope)) - tps.Y();

			if (dyThis > dyFinal)
				dyFinal = dyThis;
		}

		dx0 = tpe.X() - pps.X();
		dx1 = tpe.X() - ppe.X();

		if ((dx0 * dx1) < SMALL)
		{
			// The tool point is in the shadow of the part.

			dyThis = (pps.Y() + (dx0 * slope)) - tpe.Y();

			if (dyThis > dyFinal)
				dyFinal = dyThis;
		}

		if (dyThis == -UNDEFINED)
		{
			// The tool end points could not contact the part.

			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// Check the end points of the part against the tool.
			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	
			const C3dBox& box = toolArc.Box();
			double txmin = box.Xmin();
			double txmax = box.Xmax();

			dx0 = pps.X() - txmin;
			dx1 = pps.X() - txmax;

			if ((dx0 * dx1) < SMALL)
			{
				// The part point is in the shadow of the tool.

				dyThis = ToolY( pps, toolArc, TRUE );
				if (dyThis > dyFinal)
					dyFinal = dyThis;
			}

			dx0 = ppe.X() - txmin;
			dx1 = ppe.X() - txmax;

			if ((dx0 * dx1) < SMALL)
			{
				// The part point is in the shadow of the tool.

				dyThis = ToolY( ppe, toolArc, TRUE );
				if (dyThis > dyFinal)
					dyFinal = dyThis;
			}
		}
	}

	return dyFinal;
}

double
CAutoIndex::ToolY(
				const CGeoArc&	partArc,
				const CGeoLine&	toolLine,
				int				offsetDir )
{
	double dx0, dx1;
	C2dCoord tpt;
	C2dCoord ppt;

	double dyFinal = 0.0;
	double dyThis = -UNDEFINED;

	C2dCoord tps = toolLine.StartPt();
	C2dCoord tpe = toolLine.EndPt();

	// Find the tangency point on the part
	C2dUnitVec tan = toolLine.StartTan();
	C2dUnitVec vec = tan - HALFPI;

	C2dCoord pctr = partArc.CenterPt();
	double prad = partArc.Radius();

	ppt = pctr + (vec * prad);

	// Find the corresponding point on the tool.
	double slope = tan.Y() / tan.X();
	dx0 = ppt.X() - tps.X();

	tpt.XY( ppt.X(), (tps.Y() + (dx0 * slope)) );

	if ( partArc.PointOnSeg( ppt ) && toolLine.PointOnSeg( tpt ) )
	{
		// The tangency point is the solution.

		dyThis = ppt.Y() - tpt.Y();

		if (dyThis > dyFinal)
			dyFinal = dyThis;
	}
	else
	{
		// The tangency point is not the solution.

		C2dCoord pps = partArc.StartPt();
		C2dCoord ppe = partArc.EndPt();

		double slope = tan.Y() / tan.X();

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Check the end points of the tool against the part.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		dyThis = ToolY( tps, partArc, FALSE );
		if (dyThis > dyFinal)
			dyFinal = dyThis;

		dyThis = ToolY( tpe, partArc, FALSE );
		if (dyThis > dyFinal)
			dyFinal = dyThis;


		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Check the end points of the arc against the tool.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		dx0 = pps.X() - tps.X();
		dx1 = pps.X() - tpe.X();

		if ((dx0 * dx1) < SMALL)
		{
			// The part point is in the shadow of the tool.

			tpt.XY( pps.X(), (tps.Y() + (dx0 * slope)) );

			dyThis = pps.Y() - tpt.Y();

			if (dyThis > dyFinal)
				dyFinal = dyThis;
		}

		dx0 = ppe.X() - tps.X();
		dx1 = ppe.X() - tpe.X();

		if ((dx0 * dx1) < SMALL)
		{
			// The part point is in the shadow of the tool.

			tpt.XY( ppe.X(), (tps.Y() + (dx0 * slope)) );

			dyThis = ppe.Y() - tpt.Y();

			if (dyThis > dyFinal)
				dyFinal = dyThis;
		}
	}

	return dyFinal;
}

double
CAutoIndex::ToolY(
				const CGeoArc&	partArc,
				const CGeoArc&	toolArc,
				int				offsetDir )
{
	double dyFinal = 0.0;
	double dyThis = -UNDEFINED;

	C2dCoord tctr = toolArc.CenterPt();
	double trad   = toolArc.Radius();

	C2dCoord pctr = partArc.CenterPt();
	double prad   = partArc.Radius();

	C2dVec vecA = tctr - pctr;
	double dist = vecA.Length();

	if (fabs( dist - (prad + trad) ) < SMALL ||
		fabs( dist - (prad - trad) ) < SMALL )
	{
		// Nothing to do, already tangent

		return dyFinal;
	}

	// ASSUMPTION: An earlier bounding box test has ensured these arcs overlap.
	double radius = prad + trad;
	double dx     = tctr.X() - pctr.X();
	double delta  = radius * radius - dx * dx;

	// Find the tangency point on the tool.

	C2dUnitVec vecB( dx, sqrt( delta ) );

	C2dCoord tpt = tctr - (vecB * trad);
	C2dCoord ppt = pctr + (vecB * prad);

	if ( partArc.PointOnSeg( ppt ) && toolArc.PointOnSeg( tpt ) )
	{
		// The tangency point is the solution.

		dyThis = ppt.Y() - tpt.Y();

		if (dyThis > dyFinal)
			dyFinal = dyThis;
	}
	else
	{
		// The tangency point is not the solution.

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Check the end points of the tool against the arc.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		C2dCoord tps = toolArc.StartPt();
		C2dCoord tpe = toolArc.EndPt();

		dyThis = ToolY( tps, partArc, FALSE );
		if (dyThis > dyFinal)
			dyFinal = dyThis;

		dyThis = ToolY( tpe, partArc, FALSE );
		if (dyThis > dyFinal)
			dyFinal = dyThis;


		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Check the end points of the arc against the tool.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		C2dCoord pps = partArc.StartPt();
		C2dCoord ppe = partArc.EndPt();

		dyThis = ToolY( pps, toolArc, TRUE );
		if (dyThis > dyFinal)
			dyFinal = dyThis;

		dyThis = ToolY( ppe, toolArc, TRUE );
		if (dyThis > dyFinal)
			dyFinal = dyThis;
	}

	return dyFinal;
}

double
CAutoIndex::ToolY( const C2dCoord& pt, const CGeoArc& arc, bool ptMinusInt )
{
	CInt2d intersector;

	CGeoLine line( pt.X(), UNDEFINED, pt.X(), -UNDEFINED );

	intersector.ArcSeg( arc, line );
	if (intersector.Count() > 0)
	{
		// The arc point is in the shadow of the arc.
		const C2dCoord& ipt = intersector.Point( 0 );
		return (ptMinusInt ? (pt.Y() - ipt.Y()) : (ipt.Y() - pt.Y()));
	}

	return (-UNDEFINED);
}

bool
CAutoIndex::Overlap( const C3dBox& pbox, const C3dBox& tbox )
{
	double pmin = pbox.Xmin();
	double pmax = pbox.Xmax();

	double tmin = tbox.Xmin();
	double tmax = tbox.Xmax();

	if ( InRange( pmin, pmax, tmin ) )
		return TRUE;

	if ( InRange( pmin, pmax, tmax ) )
		return TRUE;

	if ( InRange( tmin, tmax, pmin ) )
		return TRUE;

	if ( InRange( tmin, tmax, pmax ) )
		return TRUE;

	return FALSE;
}

bool
CAutoIndex::InRange( double min, double max, double val )
{
	double dx0 = val - min;
	double dx1 = val - max;
	return ((dx0 * dx1) <= 0.0);
}


// Create the part transform such that
// 1) the part point will be at 0,0
// 2) the tangent of the associated curve will be aligned with the given orientation
//
void
CAutoIndex::PartXformInit(
					EEdge					orient,
					const CAutoIndexRule&	rule,
					C3x4Matrix*				xform )
{
	C3x4Matrix to_origin;
	const C3dVec* trans;
	double radians;

	if (orient == BACK_EDGE )
	{
		trans = &(rule.m_partBackEdgeTrans);
		radians = rule.m_partBackEdgeRot;
	}
	else
	{
		trans = &(rule.m_partFrontEdgeTrans);
		radians = rule.m_partFrontEdgeRot;
	}

	to_origin.Shift( (*trans) );
	xform->setXYAngle( radians );
	to_origin.Transform( xform );
}

// Create the tool transform such that
// 1) the primary edge of the tool will be aligned with the given orientation
// 2) the center of the tool will be offset from 0,0 to the proper side of the
//    orientation vector.
//
// Note: The tool will have a CW orientation when the offset direction is RIGHT
// and will have a CCW orientation when the offset direction is LEFT.
//
void
CAutoIndex::ToolXformInit(
					EEdge					orient,
					const CAutoIndexRule&	rule,
					C3x4Matrix*				xform )
{
	C3x4Matrix to_origin;
	const C3dVec* trans;
	double radians;

	if (orient == BACK_EDGE )
	{
		trans = &(rule.m_toolBackEdgeTrans);
		radians = rule.m_toolBackEdgeRot;
	}
	else
	{
		trans = &(rule.m_toolFrontEdgeTrans);
		radians = rule.m_toolFrontEdgeRot;
	}

	to_origin.Shift( (*trans) );
	xform->setXYAngle( radians );
	to_origin.Transform( xform );
}


