
#include "stdafx.h"
#include "StringConst.h"
#include "Return.h"
#include "Token.h"
#include "ToolConst.h"

#include "ToolSetup.h"
#include "PolicyConst.h"
#include "PunchMatcher.h"

// typedef bool (*TCompareFunc)( const CShape& shapeA, const CShape& shapeB );

const CString TOKEN_TYPES = "#@$";  // int, double, string

const double ALIGNMENT_TOL = 1.e-3;


////////////////////////////////////////////////////////////////////////

CPunchMatcher::CPunchMatcher()
	: CShapeMatcher(),
	  m_ptol( SMALL ),
	  m_mtol( SMALL )
{
}

CPunchMatcher::~CPunchMatcher()
{
}

void
CPunchMatcher::Tolerances( double plus, double minus )
{
	m_ptol = plus;
	m_mtol = minus;
}

int
CPunchMatcher::ExactMatch( const CToolSetup& toolSetup, TShapeArray& exemplars ) const
{
	int matches = 0;

	int count = exemplars.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CShape* exemplar = exemplars[indx];
		bool isMatched = (exemplar->Attribs().getInt( STR_SHAPEPTR, 0 ) > 0);

		if ( !isMatched )
		{
			TShapeArray candidateShapes;
			CShape* matchingShape;

			matchingShape = ExactMatch( toolSetup.TooledStationShapes(),(*exemplar) );

			if (matchingShape != NULL)
			{
				matchingShape->pAttribs()->setInt( STR_MATCH, EXACT_MATCH );
				candidateShapes.Append( matchingShape );
			}

			matchingShape = ExactMatch( toolSetup.UnpairedToolShapes(), (*exemplar) );

			if (matchingShape != NULL)
			{
				matchingShape->pAttribs()->setInt( STR_MATCH, EXACT_MATCH );
				candidateShapes.Append( matchingShape );
			}

			if (candidateShapes.Count() > 0)
			{
				double orient = exemplar->Attribs().getReal( STR_ORIENT, UNDEFINED );
				matchingShape = BestShapeFind( candidateShapes, orient );
				if (matchingShape != NULL)
				{
					AttribsSet( matchingShape, exemplar );
					++matches;
				}

				for (int jndx = 0; jndx < candidateShapes.Count(); ++jndx)
				{
					candidateShapes[jndx]->pAttribs()->deleteVar( STR_MATCH );
				}
			}
		}
	}

	return matches;
}

int
CPunchMatcher::AdequateMatch( const CToolSetup& toolSetup, TShapeArray& exemplars ) const
{
	int matches = 0;

	int count = exemplars.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CShape* exemplar = exemplars[indx];
		bool isMatched = (exemplar->Attribs().getInt( STR_SHAPEPTR, 0 ) > 0);

		if ( !isMatched )
		{
			TShapeArray candidateShapes;
			CShape* matchingShape;

			matchingShape = AdequateMatch( toolSetup.TooledStationShapes(), (*exemplar) );

			if (matchingShape != NULL)
			{
				matchingShape->pAttribs()->setInt( STR_MATCH, ADEQUATE_MATCH );
				candidateShapes.Append( matchingShape );
			}

			matchingShape = AdequateMatch( toolSetup.UnpairedToolShapes(), (*exemplar) );

			if (matchingShape != NULL)
			{
				matchingShape->pAttribs()->setInt( STR_MATCH, ADEQUATE_MATCH );
				candidateShapes.Append( matchingShape );
			}

			if (candidateShapes.Count() > 0)
			{
				double orient = exemplar->Attribs().getReal( STR_ORIENT, UNDEFINED );
				matchingShape = BestShapeFind( candidateShapes, orient );
				if (matchingShape != NULL)
				{
					AttribsSet( matchingShape, exemplar );
					++matches;
				}

				for (int jndx = 0; jndx < candidateShapes.Count(); ++jndx)
				{
					candidateShapes[jndx]->pAttribs()->deleteVar( STR_MATCH );
				}
			}
		}
	}

	return matches;
}

CShape*
CPunchMatcher::ExactMatch( const TSortedShapes& catalog, const CShape& exemplar ) const
{
	CShape* theMatchingShape = NULL;

	EShape type = exemplar.Type();

	if (type != SHAPE_TERMINAL)
	{
		const TShapeArray& candidates = catalog[type];

		theMatchingShape = ExactMatch( candidates, exemplar );
	}

	return theMatchingShape;
}

CShape*
CPunchMatcher::ExactMatch( const TShapeArray& candidateShapes, const CShape& exemplar ) const
{
	CReturn status;
	CShape* theMatchingShape = NULL;

	CString params;

	EShape type = exemplar.Type();

	const CVarList& attribs = exemplar.Attribs();

	// Just because these are fairly ubiquitous parameters ....
	double wid  = attribs.getReal( STR_WIDTH, UNDEFINED );
	double len  = attribs.getReal( STR_LENGTH, UNDEFINED );
	double crad = attribs.getReal( STR_CRADIUS, 0.0 );
	double orient = attribs.getReal( STR_ORIENT, UNDEFINED );

	if (type == SHAPE_SQUARE || type == SHAPE_RECTANGLE || type == SHAPE_DIAMOND)
	{
		params.Format( "@ width %f %f,@ length %f %f,@ cradius %f %f",
				wid-m_mtol, wid+m_ptol, len-m_mtol, len+m_ptol, crad-m_mtol, crad+m_ptol );
	}
	else if (type == SHAPE_TRAPEZOID)
	{
		double ang = attribs.getReal( "angle", UNDEFINED );
		params.Format( "@ width %f %f,@ length %f %f,@ angle %f %f,@ cradius %f %f",
				wid-m_mtol, wid+m_ptol, len-m_mtol, len+m_ptol, ang-0.1, ang+0.1, crad-m_mtol, crad+m_ptol );
	}
	else if (type == SHAPE_OBROUND)
	{
		params.Format( "@ width %f %f,@ length %f %f",
				wid-m_mtol, wid+m_ptol, len-m_mtol, len+m_ptol );
	}
	else if (type == SHAPE_ROUND || type == SHAPE_HEXAGON)
	{
		double diam = attribs.getReal( STR_DIAMETER, UNDEFINED );
		params.Format( "@ diameter %f %f", diam-m_mtol, diam+m_ptol );
	}
	else if (type == SHAPE_SINGLE_D || type == SHAPE_DOUBLE_D)
	{
		double diam = attribs.getReal( STR_DIAMETER, UNDEFINED );
		params.Format( "@ width %f %f,@ diameter %f %f",
				wid-m_mtol, wid+m_ptol, diam-m_mtol, diam+m_ptol );
	}
	else if (type == SHAPE_KEYHOLE)
	{
		double maj = attribs.getReal( "Major_diameter", UNDEFINED );
		double min = attribs.getReal( "Minor_diameter", UNDEFINED );
		params.Format( "@ Major_diameter %f %f,@ Minor_diameter %f %f,@ length %f %f",
				maj-m_mtol, maj+m_ptol, min-m_mtol, min+m_ptol, len-m_mtol, len+m_ptol );
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CPunchMatcher::ExactMatch()" );
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	if ( status.IsOk() )
	{
		TShapeArray matchingShapes;

		MatchingShapesFind( candidateShapes, params, &matchingShapes );

		theMatchingShape = BestShapeFind( matchingShapes, orient );
	}

	return theMatchingShape;
}

CShape*
CPunchMatcher::AdequateMatch( const TSortedShapes& catalog, const CShape& exemplar ) const
{
	CReturn status;
	CShape* theMatchingShape = NULL;

	TShapeArray matchingShapes;
	CString params;

	EShape etype = exemplar.Type();

	const CVarList& eattribs = exemplar.Attribs();
	double ewid   = eattribs.getReal( STR_WIDTH, UNDEFINED );
	double elen   = eattribs.getReal( STR_LENGTH, UNDEFINED );
	double ecrad  = eattribs.getReal( STR_CRADIUS, UNDEFINED );
	double orient = eattribs.getReal( STR_ORIENT, UNDEFINED );

	int parent = eattribs.getInt( "~original_shape", -1 );

	if (etype == SHAPE_RECTANGLE)
	{
		// Look for a shorter rectangle that can do the job.
		if (parent >= 0)
		{
			params.Format( "@ width %f %f,@ length %f %f",
					ewid-m_mtol, ewid+m_ptol, ewid-m_mtol, elen+m_ptol );
		}
		else
		{
			params.Format( "@ width %f %f,@ length %f %f,@ cradius %f %f",
					ewid-m_mtol, ewid+m_ptol, ewid-m_mtol, elen+m_ptol, ecrad-m_mtol, ecrad+m_ptol );
		}

		MatchingShapesFind( catalog[SHAPE_RECTANGLE], params, &matchingShapes );
		theMatchingShape = BestShapeFind( matchingShapes, orient );
	}

	if (etype == SHAPE_SQUARE || (etype == SHAPE_RECTANGLE && theMatchingShape == NULL))
	{
		// Look for a square that can do the job.

		if (parent >= 0)
		{
			params.Format( "@ width %f %f", ewid-m_mtol, ewid+m_ptol );
		}
		else
		{
			params.Format( "@ width %f %f,@ cradius %f %f",
					ewid-m_mtol, ewid+m_ptol, ecrad-m_mtol, ecrad+m_ptol );
		}

		MatchingShapesFind( catalog[SHAPE_SQUARE], params, &matchingShapes );
		theMatchingShape = BestShapeFind( matchingShapes, orient );
	}

	if (etype == SHAPE_OBROUND)
	{
		// Look for a shorter obround that can do the job.

		params.Format( "@ width %f %f,@ length %f %f",
				// ewid-m_mtol, ewid+m_ptol, (2*ewid), elen+m_ptol );
				ewid-m_mtol, ewid+m_ptol, ewid-m_ptol, elen+m_ptol );

		MatchingShapesFind( catalog[SHAPE_OBROUND], params, &matchingShapes );
		theMatchingShape = BestShapeFind( matchingShapes, orient );
	}

	return theMatchingShape;
}


//============================================================================

// params: search_item, search_item, .....
// search_item: type attrib_name lower_bound upper_bound
// type: $ | @ | #   (where $string, @double, #integer)
void
CPunchMatcher::MatchingShapesFind(
						const TShapeArray&	candidateShapes,
						const CString&		exemplarParams,
						TShapeArray*		matchingShapes ) const
{
	CToken tok;
	tok.addDelimit( ' ' );
	tok.addDelimit( ',' );
	tok.setBreak( TOKEN_BREAK_ALNUM );

	matchingShapes->BenignFlush();

	int count = candidateShapes.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CShape* candidate = candidateShapes[indx];
		const CVarList& candidateParams = candidate->Attribs();

		// 2004.12.25 (PE) -- We have recorded the Type_ID so that
		// MatchingShapesFind() can ignore Forming tools (Rittal).
		int type_id = candidateParams.getInt( STR_TYPE_ID, 0 );

		if (type_id == TTYPE_FORMING)
		{
			candidate = NULL;
		}
		else
		{
			tok.setLine( exemplarParams );
			while (1)
			{
				CString type = tok.getToken();
				if (type.IsEmpty())
					break;

				CString name = tok.getToken();
				if (name.IsEmpty())
					break;  // Error.

				CString lowerBound = tok.getToken();
				if (lowerBound.IsEmpty())
					break;  // Error.

				CString upperBound = tok.getToken();
				if (upperBound.IsEmpty())
					break;  // Error.

				CString value = candidateParams.getString( name, "" );

				if ( !ParamMatch( type, lowerBound, upperBound, value ) )
				{
					candidate = NULL;
					break;
				}
			}
		}

		if (candidate != NULL)
			matchingShapes->Append( candidate );
	}
}

bool
CPunchMatcher::ParamMatch(
						const CString&	type,
						const CString&	lowerBound,
						const CString&	upperBound,
						const CString&	value ) const
{
	bool match = FALSE;

	switch ( TOKEN_TYPES.Find( type ) )
	{
	case 0:  // # int
		{
			int lb = (lowerBound.IsEmpty() ? IUNDEFINED : atoi(lowerBound));
			int ub = (upperBound.IsEmpty() ? IUNDEFINED : atoi(upperBound));
			int val = (value.IsEmpty() ? IUNDEFINED : atoi(value));

			match = (lb <= val && ub >= val);
		}
		break;

	case 1:  // @ float
		{
			double lb = (lowerBound.IsEmpty() ? UNDEFINED : atof(lowerBound));
			double ub = (upperBound.IsEmpty() ? UNDEFINED : atof(upperBound));
			double val = (value.IsEmpty() ? UNDEFINED : atof(value));

			match = (lb <= val && ub >= val);
		}
		break;

	case 2:  // $ string
		match = (value.CompareNoCase( lowerBound ) == 0);
		break;
	}

	return match;
}

// Finds the longest tool that can be aligned with the
// given orientation.
CShape*
CPunchMatcher::BestShapeFind(
						const TShapeArray&	candidateShapes,
						double				orientation ) const
{
	const double TOL = 1.e-4;

	CShape* bestShape = NULL;

	int count = candidateShapes.Count();
	if (count > 0)
	{
		double size;
		double maxsize = -UNDEFINED;

		for (int indx = 0; indx < count; ++indx)
		{
			CShape* shape = candidateShapes[indx];

			const CVarList& attribs = shape->Attribs();

			switch ( shape->Type() )
			{
			case SHAPE_ROUND:
				size = attribs.getReal( STR_DIAMETER, 0.0 );
				break;
			case SHAPE_SQUARE:
				size = attribs.getReal( STR_WIDTH, 0.0 );
				break;
			default:
				size = attribs.getReal( STR_LENGTH, 0.0 );
				break;
			}

			if (size >= (maxsize - TOL))
			{
				int stationID = shape->Attribs().getInt( STR_STATION_ID, 0 );
				if (stationID > 0)
				{
					// The candidate shape represents a 'tooled station'.
					// Therefore, we must consider the tools orientation.

					double orient = attribs.getReal( STR_ORIENT, UNDEFINED );
					int indexable = attribs.getInt( STR_AUTO_INDEX, 0 );
					double diff = fabs(orientation-orient);

					if (diff < ALIGNMENT_TOL || fabs(diff - 180) < ALIGNMENT_TOL)
						diff = 0.0;

					if (fabs(diff) < SMALL || indexable)
					{
						maxsize = size;
						bestShape = shape;
					}
				}
				else if (size > (maxsize + TOL))
				{
					maxsize = size;
					bestShape = shape;
				}
			}
		}
	}

	return bestShape;
}

void
CPunchMatcher::AttribsSet( const CShape* matchingShape, CShape* exemplar ) const
{
	int matchType = matchingShape->Attribs().getInt( STR_MATCH, IUNDEFINED );

	if (matchType == EXACT_MATCH)
	{
		// NOTE: We are casting the address of the matching shape object!!!
		exemplar->pAttribs()->setInt( STR_SHAPEPTR, (int) (matchingShape) );
		exemplar->pAttribs()->setInt( STR_MATCH, matchType );
	}
	else if (matchType == ADEQUATE_MATCH)
	{
		double du, dv;

		EShape etype = exemplar->Type();
		EShape mtype = matchingShape->Type();

		double mwid  = matchingShape->Attribs().getReal( STR_WIDTH, UNDEFINED );
		double mlen  = matchingShape->Attribs().getReal( STR_LENGTH, UNDEFINED );
		double mcrad = matchingShape->Attribs().getReal( STR_CRADIUS, 0. );
	
		double ewid  = exemplar->Attribs().getReal( STR_WIDTH, UNDEFINED );
		double elen  = exemplar->Attribs().getReal( STR_LENGTH, UNDEFINED );
		int parent   = exemplar->Attribs().getInt( "~original_shape", -1 );

		if (etype == SHAPE_RECTANGLE)
		{
			mlen = ((mtype == SHAPE_RECTANGLE) ? mlen : mwid);
			mcrad = ((parent == SHAPE_OBROUND) ? mcrad : 0.);
			du = elen - mlen + (2* mcrad);
		}
		else if (etype == SHAPE_SQUARE)
		{
			mcrad = ((parent == SHAPE_OBROUND) ? mcrad : 0.);
			du = ewid - mwid + (2* mcrad);
		}
		else if (etype == SHAPE_OBROUND)
		{
			du = elen - mlen;
		}

		dv = (0.5 * ewid);

		// NOTE: We are casting the address of the matching shape object!!!
		exemplar->pAttribs()->setInt( STR_SHAPEPTR, (int) (matchingShape) );
		exemplar->pAttribs()->setInt( STR_MATCH, matchType );
		exemplar->pAttribs()->setReal( STR_DU, du );
		exemplar->pAttribs()->setReal( STR_DV, dv );
	}
	else
	{
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "CPunchMatcher::AttribsSet()" );
	}
}
