
#include "stdafx.h"

#include "MathConst.h"
#include "StringConst.h"

#include "cmn_resource.h"
#include "Register.h"
#include "GeoLine.h"
#include "DbCurve.h"
#include "DbHole.h"
#include "DbFeature.h"
#include "DbProfile.h"
#include "DbIterator.h"
#include "DbEntityVisitor.h"
#include "ModelUtil.h"
#include "ContHier.h"

#include "CodeUtil.h"
#include "Optimizer.h"
#include "SeqRules.h"
#include "ShortestPath.h"
#include "Portal.h"

const double PROGRESSIVE_X_TOL = 0.001;

////////////////////////////////////////////////////////////////////////

	class CFeatureTagger : public CDbEntityVisitor
	{
	public:

		CFeatureTagger()
		{
		}
		
		virtual ~CFeatureTagger()
		{
		}

		virtual void Visit( CDbEntity* dbEntity )
		{
			CDbFeature*	dbFeature = dynamic_cast<CDbFeature*>( dbEntity );
			if (dbFeature != NULL)
				dbFeature->DoAction();
		}
	};

////////////////////////////////////////////////////////////////////////

CReturn
COptimizer::SequenceByToolOrder(
						CSeqRules*				seqRules,
						const CDbEntityArray&	dbEntityList,
						const CDbEntityArray&	dbToolOrder,
						CDbEntityArray*			cutOrder )
{
	CReturn			status;
	CShortestPath	shortest;

	CDbEntityArray	masterCopy;
	CDbEntityArray	rawOrder;
	CDbEntityArray	emptyToolList;
	CContHier		hier;
	CDbTool*		dbTool;
	int				tcnt, tndx;

	status.Diagnostic( "*COptimizer::SequenceByToolOrder(#1)" );
	
	EntitiesCopy( dbEntityList, &masterCopy );


	tcnt = dbToolOrder.Count();
	for (tndx = 0; tndx < tcnt; ++tndx)
	{
		dbTool = dynamic_cast<CDbTool*>( dbToolOrder[tndx] );

		TooledEntitiesTransfer( dbTool, &masterCopy, &rawOrder );

		if (rawOrder.Count() > 0)
		{
			seqRules->CurrTool( dbTool );
			seqRules->TrendSet( TRUE );

			// Build the containment hierarchy.
			hier.Init( rawOrder );
			hier.Sequence( seqRules );

			EntitiesTransfer( hier.FinalOrder(), &rawOrder, cutOrder );

			rawOrder.BenignFlush();  // just in case
		}
	}

	status.Diagnostic( "*COptimizer::SequenceByToolOrder(#2)" );

	return status;
}

CReturn
COptimizer::SequenceByFlowChart(
						CSeqRules*				seqRules,
						const CDbEntityArray&	masterPool,
						const CDbEntityArray&	toolOrder,
						CDbEntityArray*			cutOrder )
{
	CReturn				status;
	CContHier			hier;
	CDbEntityArray		masterCopy;

	status.Diagnostic( "*COptimizer::SequenceByFlowChart(#1)" );

	EntitiesCopy( masterPool, &masterCopy );

	if ( seqRules->AllHolesFirst )
	{
		CDbEntityArray	interPool;
		bool			memo;

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Holes will be processed across the entire material before
		// processing any profiles.  Once the holes have been processed,
		// they will have been removed from further consideration.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		// Move all holes from the master entity
		//  pool to an intermediate entity pool.
		
		HolesTransfer( &masterCopy, &interPool );

		memo = seqRules->HolesAcrossLocalNest;
		seqRules->HolesAcrossLocalNest = TRUE;
		SequenceByToolOrder( seqRules, interPool, toolOrder, cutOrder );
		seqRules->HolesAcrossLocalNest = memo;

		seqRules->TrendSet( TRUE );
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Build the containment hierarchy.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	if ( IsPhenolicProject() )
		hier.PhenolicInit( &toolOrder, seqRules->trend );

	hier.Init( masterCopy );
	hier.Sequence( seqRules );

	if ( CRegister::Debug("ContHier") )
		hier.Debug();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	EntitiesTransfer( hier.FinalOrder(), &masterCopy, cutOrder );

	AttributesClear( masterPool );

	status.Diagnostic( "*COptimizer::SequenceByFlowChart(#2)" );

	return status;
}

#if ORIGINAL_PROGESSIVE
CReturn
COptimizer::SequenceByProgressive(
							CSeqRules*				seqRules,
							const CDbEntityArray&	masterPool,
							const CDbEntityArray&	toolOrder,
							CDbEntityArray*			cutOrder )
{
	CReturn				status;
	CContHier			hier;
	CDbEntityArray		masterCopy;
	CShortestPath		shortest;

	EntitiesCopy( masterPool, &masterCopy );

	hier.Init( masterCopy );
	hier.Sequence(seqRules);

	if ( CRegister::Debug("ContHier") )
		hier.Debug();

	EntitiesTransfer( hier.FinalOrder(), &masterCopy, cutOrder );

	AttributesClear( masterPool );

	//
	//
	return status;
}
#else
CReturn
COptimizer::SequenceByProgressive(
							CSeqRules*				seqRules,
							const CDbEntityArray&	masterPool,
							const CDbEntityArray&	toolOrder,
							CDbEntityArray*			cutOrder )
{
	CReturn			status;
	tGeoPointArray	initial;
	tGeoPointArray	temp;
	CGeoPoint*		geo_pt;
	C3dCoord		pt;
	CDbEntity*		dbEntity;
	double			offset;
	int				count, indx;
	int				jndx, lb, ub;

	status.Diagnostic( "*COptimizer::SequenceByProgressive(#1)" );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Collect the set of points representing the starting
	// location of each operation.  Each point records a
	// back-pointer to its associated entity.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	count = masterPool.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = masterPool[indx];

		// NOTE: Station_Location_X represents the offset
		// between the punch station and the current station.
		offset = dbEntity->Tool()->DoubleGet("Station_Location_X", 0.);

		switch (dbEntity->Type())
		{
		case DBLINE:
		case DBARC:
			// Get the span of curves having C0 continuity from this
			// starting location (which is most likely the start of a lead-in)
			jndx = SpanGet( masterPool, indx );
			pt = ((CDbCurve*) dbEntity)->StartPt(0);
			pt.X( pt.X() + offset );
			geo_pt = new CGeoPoint( pt );
			geo_pt->IntSet( "_ptr", (int) dbEntity );
			geo_pt->IntSet( "_lb", indx );
			geo_pt->IntSet( "_ub", jndx );
			initial.Append( geo_pt );
			indx = jndx - 1;
			break;

		case DBHOLE:
			// NOTE: The offset should be zero, but just in case ...
			pt = ((CDbHole*) dbEntity)->Coord(0);
			pt.X( pt.X() + offset );
			geo_pt = new CGeoPoint( pt );
			geo_pt->IntSet( "_ptr", (int) dbEntity );
			initial.Append( geo_pt );
			break;

		case DBCOMMAND:
			pt = ((CDbCommand*) dbEntity)->Coord(0);
			pt.X( pt.X() + offset );
			geo_pt = new CGeoPoint( pt );
			geo_pt->IntSet( "_ptr", (int) dbEntity );
			initial.Append( geo_pt );
			break;
		}
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Sort the points on X & Y, and record the results.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	initial.Qsort( ProgressiveXsort );

	count = initial.Count();
	indx = 0;
	while (1)
	{
		// Find a set of points having the same X value.
		jndx = ProgressiveRange( initial, indx );
		while (indx < jndx)
		{
			temp.Append( initial.GetAt(indx) );
			++indx;
		}

		// Sort the set on Y.
		temp.Qsort( ProgressiveYsort );

		// Record the results.
		for (indx = 0; indx < temp.Count(); ++indx)
		{
			geo_pt = temp.GetAt(indx);
			dbEntity = (CDbEntity*) geo_pt->IntGet( "_ptr", 0 );

			switch (dbEntity->Type())
			{
			case DBLINE:
			case DBARC:
				lb = geo_pt->IntGet( "_lb", 0 );
				ub = geo_pt->IntGet( "_ub", 0 );
				while (lb < ub)
				{
					cutOrder->Append( masterPool.GetAt(lb) );
					++lb;
				}
				break;
			case DBHOLE:
			case DBCOMMAND:
				cutOrder->Append( dbEntity );
				break;
			}
		}

		// Prepare for the next batch.
		temp.BenignFlush();

		if (jndx >= count)
			break;

		indx = jndx;
	}

	initial.DestructiveFlush();

	status.Diagnostic( "*COptimizer::SequenceByProgressive(#2)" );

	return status;
}
#endif

CReturn
COptimizer::SequenceByAvoidance(
							CSeqRules*				seqRules,
							const CDbEntityArray&	masterPool,
							const CDbEntityArray&	toolOrder,
							CDbEntityArray*			cutOrder )
{
	CReturn				status;
	CContHier			hier;
	CDbEntityArray		masterCopy;
	CShortestPath		shortest;

	status.Diagnostic( "*COptimizer::SequenceByAvoidance(#1)" );

	EntitiesCopy( masterPool, &masterCopy );

	hier.Init( masterCopy );
	hier.Sequence(seqRules);

	if ( CRegister::Debug("ContHier") )
		hier.Debug();

	EntitiesTransfer( hier.FinalOrder(), &masterCopy, cutOrder );

	AttributesClear( masterPool );

	//

	status.Diagnostic( "*COptimizer::SequenceByAvoidance(#2)" );
	//
	return status;
}


void
COptimizer::HolesTransfer( CDbEntityArray* entityPool, CDbEntityArray* holes )
{
	int indx = 0;
	while (indx < entityPool->Count())
	{
		CDbHole* dbHole = dynamic_cast<CDbHole*>( (*entityPool)[indx] );
		if (dbHole != NULL)
		{
			entityPool->Remove( indx );
			holes->Append( dbHole );
		}
		else
		{
			++indx;
		}
	}
}

void
COptimizer::CommandsTransfer( CDbEntityArray* entityPool, CDbEntityArray* holes )
{
	int indx = 0;
	while (indx < entityPool->Count())
	{
		CDbCommand* dbCmd = dynamic_cast<CDbCommand*>( (*entityPool)[indx] );
		if (dbCmd != NULL)
		{
			entityPool->Remove( indx );
			holes->Append( dbCmd );
		}
		else
		{
			++indx;
		}
	}
}


void
COptimizer::ProgressiveTransfer( CDbEntityArray* entityPool, CDbEntityArray* hits )
{
	int indx = 0;
	while (indx < entityPool->Count())
	{
		CDbCommand* dbCmd = dynamic_cast<CDbCommand*>( (*entityPool)[indx] );
		CDbHole* dbHole = dynamic_cast<CDbHole*>( (*entityPool)[indx] );

		if (dbCmd != NULL)
		{
			entityPool->Remove( indx );
			hits->Append( dbCmd );
		}
		else
		if (dbHole != NULL)
		{
			entityPool->Remove( indx );
			hits->Append( dbHole );
		}
		else
		{
			++indx;
		}
	}
}

void
COptimizer::DepthTransfer( CDbEntityArray* entityPool, CDbEntityArray* deepHits, CDbEntityArray* shallowHits )
{
	while (entityPool->Count())
	{
		CDbEntity* ent = (*entityPool)[0];
		int depth = ent->IntGet(STR_CD, -1);

CString note;
CReturn ret;
note.Format( "Depth %d", depth);
ret.Diagnostic(note);
depth = 0;

		if (depth >= 1)
			deepHits->Append(ent);
		else
			shallowHits->Append(ent);

		entityPool->Remove(0);
	}
}


// Slip 248 -- Flowchart method does not process complete outer profile.
// This happened because this method assumed C0 continuity, but the problem
// profile contained tab gaps.  This method now considers tab gaps.
//
// TODO: Though this method can handle sequences such as LeadIn Profile LeadOut, it
// can not handle cases like LeadIn P0 LeadOut LeadIn P1 LeadOut...LeadIn Pn LeadOut
// where P0..Pn are sequences within a given profile.  As of V14, the software is
// not capable of generating such as sequence, but such a sequence may be desirable
// if a user wants to AutoLead a profile containing tab gaps.  In this case, you
// might expect to see a lead-in & lead-out on each side of the tab gap.
//
void
COptimizer::EntitiesTransfer(
						const CDbEntityArray& order,
						CDbEntityArray* masterPool,
						CDbEntityArray* interPool )
{
	int indx, jndx;
	int indxA, indxB;
	int orderCount, masterCount;
	C3dCoord ptA, ptB;
	CDbEntity* dbEntity;
	CDbCurve* dbCurveA;
	CDbCurve* dbCurveB;

	orderCount = order.Count();

	for (indx = 0; indx < orderCount; ++indx)
	{
		dbEntity = order[indx];

		masterCount = masterPool->Count();

		jndx = masterPool->Find( dbEntity );
		if (jndx >= 0)
		{
			if (dbEntity->Type() == DBHOLE || dbEntity->Type() == DBCOMMAND)
			{
				interPool->Append( dbEntity );
			}
			else
			{
				// Find the actual start of the sequence.
				// Whereas this curve may be the starting curve of a profile,
				// the curve may be preceeded and/or followed by a lead in/out.
				dbCurveA = dynamic_cast<CDbCurve*>( dbEntity );
				indxA = jndx;

				while (1)
				{
					--indxA;
					if (indxA < 0)
						break;

					dbCurveB = dynamic_cast<CDbCurve*>( (*masterPool)[indxA] );
					if (dbCurveB == NULL)
						break;

					ptA = dbCurveA->StartPt();
					ptB = dbCurveB->EndPt();
					if ( !ptA.WithinTol( ptB, SMALL ) )
					{
						// We've encountered a break in C0 continuity.
						break;
					}

					dbCurveA = dbCurveB;
				}
				++indxA;

				// Find the actual end of the sequence.
				dbCurveA = dynamic_cast<CDbCurve*>( dbEntity );
				indxB = jndx;

				while (1)
				{
					++indxB;
					if (indxB >= masterCount)
						break;

					dbCurveB = dynamic_cast<CDbCurve*>( (*masterPool)[indxB] );
					if (dbCurveB == NULL)
						break;

					ptA = dbCurveA->EndPt();
					ptB = dbCurveB->StartPt();
					if ( !ptA.WithinTol( ptB, SMALL ) )
					{
						// We've encountered a break in C0 continuity.
						// We must consider tab gaps within a profile, however.

						if (dbCurveA->Owner() != dbCurveB->Owner())
							break;
						
						if (dynamic_cast<CDbProfile*>( dbCurveA->Owner() ) == NULL)
							break;
					}

					dbCurveA = dbCurveB;
				}
				--indxB;

				for (jndx = indxA; jndx <= indxB; ++jndx)
				{
					dbEntity = masterPool->Remove( indxA );
					interPool->Append( dbEntity );
				}
			}
		}
	}
}

void
COptimizer::EntitiesCopy( const CDbEntityArray& entities, CDbEntityArray* copies )
{
	int count = entities.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		copies->Append( entities[indx] );
	}
}

// Remove the temporary 'processing attributes'
void
COptimizer::AttributesClear( const CDbEntityArray& masterPool )
{
	// 2009.04.05 (PE) -- Prior to V17 (?) we removed this attribute
	// so as to prevent detritus from being saved with the model. In
	// later versions, the model was copied (as I remember ... that
	// was done to support subroutines) and so the original model
	// is not affected by the attribute and, hence, will not be
	// saved with the original model when it is written to disk.
#if ORIGINAL_CODE
	int count = masterPool.Count();
	for (int indx = 0; indx < count; ++ indx)
	{
		CDbEntity* dbEntity = masterPool[indx];

		CVarList* attribs = dbEntity->pAttrib();
		attribs->deleteVar( STR_CD );
	}
#endif
}

void
COptimizer::TooledEntitiesTransfer(
							const CDbTool*	dbTool,
							CDbEntityArray*	source,
							CDbEntityArray*	result )
{
	CDbEntity*	dbEntity;
	int			indx;

	indx = 0;
	while (indx < source->Count())
	{
		dbEntity = (*source)[indx];

		if (dbEntity->Type() == DBCOMMAND)
		{
			if ( ((CDbCommand*) dbEntity)->IsInstance() ||
				 ((CDbCommand*) dbEntity)->IsTooledText() )
			{
				source->Remove( indx );
				result->Append( dbEntity );
			}
			else
			{
				++indx;
			}
		}
		else
		{
			if (dbEntity->Tool() == dbTool)
			{
				source->Remove( indx );
				result->Append( dbEntity );
			}
			else
			{
				++indx;
			}
		}
	}
}


int
COptimizer::SpanGet( const CDbEntityArray& ents, int indx )
{
	CDbCurve*	dbCurveA;
	CDbCurve*	dbCurveB;
	int			count, jndx;

	count = ents.Count();

	while (1)
	{
		jndx = indx + 1;
		if (jndx >= count)
			break;

		dbCurveA = dynamic_cast<CDbCurve*>( ents.GetAt(indx) );

		dbCurveB = dynamic_cast<CDbCurve*>( ents.GetAt(jndx) );
		if (dbCurveB == NULL)
			break;

		if (dbCurveA->Tool() != dbCurveB->Tool())
			break;

		if ( !dbCurveB->StartPt().WithinTol( dbCurveA->EndPt(), SMALL ) )
			break;

		++indx;
	}

	return jndx;
}

int
COptimizer::ProgressiveRange( const tGeoPointArray& pts, int indx )
{
	CGeoPoint*	geo_ptA;
	CGeoPoint*	geo_ptB;
	double		diff;
	int			count, jndx;

	geo_ptA = pts.GetAt(indx);

	count = pts.Count();
	jndx = indx;
	while (1)
	{
		++jndx;
		if (jndx >= count)
			break;

		geo_ptB = pts.GetAt(jndx);

		diff = geo_ptB->StartPt().X() - geo_ptA->StartPt().X();
		if (diff > PROGRESSIVE_X_TOL)
			break;
	}

	return jndx;
}

int
COptimizer::ProgressiveXsort( const void* ptrA, const void* ptrB )
{
	CGeoPoint* geoPtA = ( *(CGeoPoint**) ptrA);
	CGeoPoint* geoPtB = ( *(CGeoPoint**) ptrB);

	return (((geoPtB->StartPt().X() - geoPtA->StartPt().X()) > PROGRESSIVE_X_TOL) ? -1 : 1);;
}

int
COptimizer::ProgressiveYsort( const void* ptrA, const void* ptrB )
{
	CGeoPoint* geoPtA = ( *(CGeoPoint**) ptrA);
	CGeoPoint* geoPtB = ( *(CGeoPoint**) ptrB);

	return (((geoPtB->StartPt().Y() - geoPtA->StartPt().Y()) > PROGRESSIVE_X_TOL) ? -1 : 1);;
}
