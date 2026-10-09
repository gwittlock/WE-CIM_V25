
#include "stdafx.h"
#include "StringConst.h"
#include "cmn_resource.h"
#include "GeoPoint.h"
#include "GeoLine.h"
#include "DbTool.h"
#include "Shape.h"

#include "PolicyConst.h"
#include "AutoTool.h"


////////////////////////////////////////////////////////////////////////

CAutoTool::CAutoTool()
{
}

CAutoTool::~CAutoTool()
{
}



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// toolSetup       -- the reference tool setup
// buildSetup      -- (true) builds/modifies the toolSetup
//                    (false) only uses tooled stations in the toolSetup
// theShape        -- the shape that we trying to realize
// shapeMatcher    -- the shape matcher for finding tools to realize theShape
// shapeDecomposer -- the shape decomposer for reducing theShape to more primitive shapes
// geometry        -- the 'hit locations' for the matching tooling to realize theShape
//                    the client is responsible for managing the lifetimes of the contained entities
//
CReturn
CAutoTool::ToolIt(
					CToolSetup*				toolSetup,
					bool					buildSetup,
					EMatchingRigour			rigour,
					const CShape&			theShape,
					const CShapeMatcher&	shapeMatcher,
					const CShapeDecomposer&	shapeDecomposer,
					CGeoElemList*			geometry )
{
	CReturn status;

	TShapeArray exemplars;
	int matches = 0;
	int iteration = 0;

	exemplars.Append( theShape.Clone() );

	while (1)
	{
		// TODO: Exact matches should be tagged as such so that
		//       the shape can be realized with a single hit.

		matches += shapeMatcher.ExactMatch( (*toolSetup), exemplars );

		if (matches < exemplars.Count())
		{
			matches += shapeMatcher.AdequateMatch( (*toolSetup), exemplars );
		}

		if (matches < exemplars.Count())
		{
			// Decompose the shape into more primitive shapes.

			shapeDecomposer.Decompose( theShape, iteration, &exemplars );
			if (exemplars.Count() == 0)
				break;  // process complete, no matches.
		}
		else
		{
			if ( buildSetup )
			{
				ToolSetupModify( toolSetup, exemplars );
			}

			RealizeShape( exemplars, geometry );
			break;  // process complete, all matches.
		}

		matches = 0;
		++iteration;
	}

	exemplars.DestructiveFlush();

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// For each shape that is not associated with a tooled station, create a
// tooled station by either 1) associating the tool with an empty station,
// or 2) replacing an unreferenced tool in a tooled station.
//
CReturn
CAutoTool::ToolSetupModify( CToolSetup* toolSetup, const TShapeArray& exemplars )
{
	CReturn status;

	int count = exemplars.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CShape* exemplar = exemplars[indx];
		CShape* tool = MatchingShape( (*exemplar) );

		if (tool == NULL)
		{
			status.Internal( IDS_INTERNAL_ERROR, "CAutoTool::ToolSetupModify()" );
		}
		else
		{
			CShape* tooledStation = NULL;

			int stationID = tool->Attribs().getInt( STR_STATION_ID, 0 );

			if (stationID == 0)
			{
				// This tool is not associated with a station.

				double orientation = exemplar->Attribs().getReal( STR_ORIENT, 0. );
				double reqdStationSize = tool->Attribs().getReal( STR_REQD_STATION_SIZE, 0. );
				bool reqAutoIndex = (tool->Attribs().getInt( STR_REQD_AUTO_INDEX, FALSE ) != FALSE);

				CShape* station = toolSetup->EmptyFind( reqdStationSize, orientation, reqAutoIndex );

				if (station == NULL)
					station = toolSetup->UnrefdFind( reqdStationSize, orientation, reqAutoIndex );

				if (station != NULL)
					tooledStation = Collate( toolSetup, station, tool );
			}
			else
			{
				tooledStation = tool;
			}

			if (tooledStation == NULL)
			{
				// TODO: Log the failure?
			}
			else
			{
				// By incrementing the reference count of this tooled station,
				// we prevent the tool setup from identifying this tooled station
				// as one that is 'unreferenced'.
				int refcnt = tooledStation->Attribs().getInt( STR_REFCNT, 0 );
				tooledStation->pAttribs()->setInt( STR_REFCNT, ++refcnt );
			}
		}
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// TODO: CAutoTool::RealizeShape()
// See also c:\Bbi\AdvMach\Toolpath\Autotool.cpp as a reference.
//
CReturn
CAutoTool::RealizeShape( const TShapeArray& exemplars, CGeoElemList* geometry )
{
	CReturn status;

	int count = exemplars.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		const CShape* theShape = exemplars[indx];
		CShape* theMatchingShape = MatchingShape( (*theShape) );

		if (theShape != NULL && theMatchingShape != NULL)
		{
			int match = theShape->Attribs().getInt( STR_MATCH, IUNDEFINED );
			double orient = theShape->Attribs().getReal( STR_ORIENT, 0.0 );

			if (match == EXACT_MATCH)
			{
				double ox = theShape->Attribs().getReal( STR_OX, 0.0 );
				double oy = theShape->Attribs().getReal( STR_OY, 0.0 );
				C3dCoord pc = C3dCoord( ox, oy, 0. );

				HitCreate( pc, (*theShape), (*theMatchingShape), geometry );
			}
			else if (match == ADEQUATE_MATCH)
			{
				double du = theShape->Attribs().getReal( STR_DU, 0. );
				double dv = theShape->Attribs().getReal( STR_DV, 0. );

				HitsCreate( (*theShape), (*theMatchingShape), du, dv, geometry );
			}
		}
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Get the tool shape that was found to match this shape
//   (by CPunchMatcher::ExactMatch(), for instance)
//
CShape* 
CAutoTool::MatchingShape( const CShape& shape )
{
	int match = shape.Attribs().getInt( STR_SHAPEPTR, 0 );
	if (match == 0)
	{
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "CAutoTool::MatchingShape()" );
	}
	return ((CShape*) match);
}

void
CAutoTool::HitCreate(
				const C3dCoord&		pc,
				const CShape&		theShape,
				const CShape&		theMatchingShape,
				CGeoElemList*		result )
{
	double orient = theShape.Attribs().getReal( STR_ORIENT, 0.0 );
	int stationID = theMatchingShape.Attribs().getInt( STR_STATION_ID, 0 );
	int autoIndex = theMatchingShape.Attribs().getInt( STR_AUTO_INDEX, 0 );

	CGeoPoint* pt = new CGeoPoint( pc );

	// See also CModelUtil::PunchedFeatureCreate()
	pt->IntSet( STR_STATION_ID, stationID );
	pt->DoubleSet( STR_ORIENT, (autoIndex ? orient : 0.0) );

	result->Append( pt );
}

void
CAutoTool::HitsCreate(
				const CShape&		theShape,
				const CShape&		theMatchingShape,
				double				du,
				double				dv,
				CGeoElemList*		result )
{
	const CGeoLine* line = dynamic_cast<const CGeoLine*>( theShape[0] );
	if (line != NULL)
	{
		C2dUnitVec vec = line->StartTan();
		C2dCoord pc = line->MidPt() + ((vec  - HALFPI) * dv);

		C3dCoord ps = pc - (vec * (0.5 * du));
		C3dCoord pe = pc + (vec * (0.5 * du));

		ps.Z( 0. );
		pe.Z( 0. );

		if ( ps.WithinTol( pe, SMALL ) )
		{
			HitCreate( ps, theShape, theMatchingShape, result );
		}
		else
		{
			double orient = theShape.Attribs().getReal( STR_ORIENT, 0.0 );
			int stationID = theMatchingShape.Attribs().getInt( STR_STATION_ID, 0 );
			int autoIndex = theMatchingShape.Attribs().getInt( STR_AUTO_INDEX, 0 );

			CGeoLine* move = new CGeoLine( ps, pe );

			// See also CConversion::PunchedFeatureCreate()
			move->IntSet( STR_STATION_ID, stationID );
			move->DoubleSet( STR_ORIENT, (autoIndex ? orient : 0.0) );

			result->Append( move );
		}
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Goes through the necessary gyrations to adjust the tool setup.
//
CShape*
CAutoTool::Collate( CToolSetup* toolSetup, CShape* station, CShape* tool )
{
	CShape* tooledStation = NULL;

	if ( IsReusable( (*station) ) )
		toolSetup->Divorce( station );

	tooledStation = toolSetup->Marry( station, tool );

	return tooledStation;
}

// Returns true when given a 'tooled station' that is not a 'fixed station'.
bool
CAutoTool::IsReusable( const CShape& station )
{
	int toolID = station.Attribs().getInt( STR_TOOL_ID, 0 );
	int fixedStation = station.Attribs().getInt( STR_FIXED_STATION, 0 );
	return (toolID > 0 && fixedStation == 0);
}

EShape
CAutoTool::ShapeType( int typeID )
{
	switch (typeID)
	{
	case TTYPE_SQUARE:		return SHAPE_SQUARE;
	case TTYPE_RECTANGLE:	return SHAPE_RECTANGLE;
	case TTYPE_OBROUND:		return SHAPE_OBROUND;
	case TTYPE_DIAMOND:		return SHAPE_DIAMOND;
	case TTYPE_SINGLE_D:	return SHAPE_SINGLE_D;
	case TTYPE_DOUBLE_D:	return SHAPE_DOUBLE_D;
	case TTYPE_TRAPEZOID:	return SHAPE_TRAPEZOID;
	case TTYPE_KEYHOLE:		return SHAPE_KEYHOLE;
	case TTYPE_HEXAGON:		return SHAPE_HEXAGON;
	default:				return SHAPE_ROUND;
	}
}
