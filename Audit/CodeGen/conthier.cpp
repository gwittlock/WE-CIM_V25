
#include "stdafx.h"
#include "assert.h"
#include "cmn_resource.h"
#include "StringConst.h"
#include "Return.h"
#include "Register.h"
#include "GeoPoly.h"
#include "DbAllEntities.h"
#include "ModelUtil.h"
#include "ContHierNode.h"
#include "ContHier.h"

#include "SeqRules.h"
#include "ShortestPath.h"
#include "ToolSequencer.h"

bool DEEP_FIRST = true;

#define DECREASING_ORDER	FALSE
#define INCREASING_ORDER	TRUE



double CContHier::m_gap_tol = SMALL;

////////////////////////////////////////////////////////////////////////

CContHier::CContHier()
{
	m_phenolicToolOrder = NULL;
	m_phenolicTrend = OPT_UNDEFINED;
}

CContHier::~CContHier()
	{ m_nodes.DestructiveFlush(); }
	
void
CContHier::PhenolicInit( const CDbEntityArray* toolOrder, ESeqTrend trend )
{
	m_phenolicToolOrder = toolOrder;
	m_phenolicTrend = trend;
}

void CContHier::Debug() const
{
	if ( EWMDiagnosticAllow() )
	{
		CString msg;

		EWMDiagnostic( "--- CContHier::Debug() ---" );

		for (int indx = 0; indx < m_nodes.Count(); ++indx)
		{
			CContHierNode* node = m_nodes[indx];
			CDbEntity* dbEntity = node->Root();

			msg = ((dbEntity == NULL) ? "<null>" : dbEntity->Name());

			int count = node->Contained().Count();
			for (int jndx = 0; jndx < count; ++jndx)
			{
				if (jndx == 0)
					msg += " contains";

				dbEntity = node->Contained()[jndx];

				msg += (" " + dbEntity->Name());

				if ((jndx + 1) < count)
					msg += ",";
			}

			EWMDiagnostic( (LPCSTR) msg );
		}

		EWMDiagnostic( "" );
	}
}

// The basis for this method was copied from CModelUtil::MarkIntExt()
CReturn
CContHier::Init( const CDbEntityArray& entityPool )
{
	CReturn			status;
	CGeoPolyArray	polyArray;
	CDbEntityArray	entityArray;
	CContHierNode*	hierNode;
	CDbEntity*		entityA;
	CDbEntity*		entityB;
	CGeoPoly*		polyA;
	CGeoPoly*		polyB;
	int				count, indxA, indxB;
	int				depth, cntB;
	bool			encloses;

	EWMFile( FILE_INFO );

	m_nodes.DestructiveFlush();
	m_finalOrder.BenignFlush();

	if ( IsPhenolicProject() )
	{
		status = PhenolicHierInit( entityPool );
		return status;
	}

	// Create Poly versions of each arc/hole/profile
	PolysCreate( entityPool, &polyArray, &entityArray );

	count = polyArray.Count();
	if (count == 0)
		return status;

	// Test each poly against the others, for inclusion
	for (indxA = 0; indxA < count; indxA++)
	{
		hierNode = new CContHierNode();
		m_nodes.Append( hierNode );

		entityA = entityArray[indxA];
		hierNode->Root( entityA );

		for (indxB = 0; indxB < count; indxB++)
		{
			if (indxB == indxA)
				continue;

			// 2006.12.02 (PE) -- Till now, code was never produced for
			// either hit when we encountered a double hit. Now we punt.
			entityB = entityArray[indxB];
			if ( IsDoubleHit( (*entityA), (*entityB) ) )
			{
				int depthA = entityA->IntGet( STR_CD, 0 );
				int depthB = entityB->IntGet( STR_CD, 0 );

				if (depthA != depthB)
				{
					if (depthA > depthB)
						entityB->IntSet( STR_CD, depthA );
					else
						entityA->IntSet( STR_CD, depthB );
				}

				continue;
			}

			polyA = polyArray[indxA];
			polyB = polyArray[indxB];

			// 2006.10.28 (PE) -- Hard lesson. Prior to now, containment
			// checks were allowed when polyA was an open profile. That
			// solution caused some entities to remain uncut because they
			// were pruned from the tree (because their containment parity
			// was out-of-whack). See also CContHier::Sort() and
			// CContHierNode::Reduce().
			//
			// In particular, the containment depth values indicated a conflict
			// of ownership (that is, who really contained what). This became
			// particularly evident during an experiment where two C-shaped polys
			// contained each others start points.
			encloses = FALSE;
			if ( IsClosed( polyA ) )
			{
				cntB = polyB->Count();
				encloses = (polyA->PtInPoly( (*polyB)[0].StartPt() ) ||
							polyA->PtInPoly( (*polyB)[cntB-1].EndPt() ));
			}

			if ( encloses )
			{
				// Each time a profile is enclosed, it value is flipped
				// ... a sort of enclosure parity.  Seems to work!
				depth = entityB->IntGet( STR_CD, 0 );

				depth++;
				entityB->IntSet( STR_CD, depth );

				hierNode->Append( entityB );
			}
		}
	}

	// Sort the nodes on increasing order of containment.
	Sort( INCREASING_ORDER, &m_nodes );

	polyArray.DestructiveFlush();

	EWMFile( FILE_INFO );

	return status;
}

CReturn
CContHier::Sequence( CSeqRules* seqRules )
{
	CReturn	status;

	if (seqRules->cut_avoid)
	{
		status = SequenceByAvoidance(seqRules);
	}
	else
	if (seqRules->gridSeqOption == SEQUENCE_BY_PROGRESSIVE)
	{
		status = SequenceByProgressive( seqRules );
	}
	else
	if ( seqRules->ByCompletePart == BY_LOCAL_CONTAINMENT)
	{
		status = SequenceByLocalContainment( seqRules );
	}
	else if (seqRules->ByCompletePart == BY_GLOBAL_CONTAINMENT)
	{
		status = SequenceByGlobalContainment( seqRules );
	}
	else if (seqRules->ByCompletePart == BY_RECURSION)
	{
		status = SequenceByRecursion( seqRules );
	}

	return status;
}

CReturn
CContHier::SequenceByRecursion( CSeqRules* seqRules )
{
	CReturn			status;
	CString			regRoot;
	CToolSequencer	toolSeq;
	CDbEntityArray	toolOrder;
	CContHierNode*	root;
	CDbTool*		currTool;
	CDbEntity*		rootEntity;
	int				indxA, indxB;

	EWMFile( FILE_INFO );

	regRoot = CRegister::RootPathV("\\CodeGeneration");
	DEEP_FIRST = CRegister::BoolGetV( "CodeGeneration", "DeepFirst", true );

	CDbEntity::NewAction();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// This bit of trickery is done for the sake of
	// the first call to RecursiveOrder().
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	{
		if (seqRules->grid_opt == SEQUENCE_BY_TOOL_ORDER)
			currTool = seqRules->CurrTool();
		else
			currTool = NULL;

		// Find the range of level-zero containment entities.
		indxA = 0;
		indxB = NodesScan( m_nodes, indxA );

		root = new CContHierNode();
		while (indxA < indxB)
		{
			rootEntity = m_nodes[indxA]->Root();

			if (currTool == NULL)
			{
				root->Append( rootEntity );
			}
			else if (rootEntity->Tool() == currTool)
			{
				root->Append( rootEntity );
			}

			++indxA;
		}

		m_nodes.Prepend( root );
	}

	toolSeq.ToolSetupOrder( seqRules->Model(), FALSE, &toolOrder );

	RecursiveOrder( seqRules, toolOrder, root->Contained(), &m_finalOrder );

	EWMFile( FILE_INFO );

	return status;
}

// BACKGROUND: Containment order processing is done mostly on burners
// because the processing method provides maximum material support.
// That is, many burners support the material on cones.  By burning
// the largest regions last, we provide maximum support for the longest
// time.
//
CReturn
CContHier::SequenceByGlobalContainment( CSeqRules* seqRules )
{
	CReturn			status;
	CToolSequencer	toolSeq;
	CDbEntityArray	toolOrder;
	CDbEntityArray	rawHoles;
	CDbEntityArray	rawShallow;
	CDbEntityArray	rawDeep;
	CDbEntityArray	cutOrder;
	CShortestPath	shortest;
	int				indxA, indxB;

	EWMFile( FILE_INFO );

	toolSeq.ToolSetupOrder( seqRules->Model(), FALSE, &toolOrder );

	CDbEntity::NewAction();

	indxA = 0;
	while (1)
	{
		// Find the range of nodes [indxA..indxB-1] having the same
		// containment level.
		indxB = NodesScan( m_nodes, indxA );
		if (indxB < 0)
			break;

		// Collect all the relevant entities at this containment level.
		EntitiesCollect( m_nodes, indxA, indxB, &rawHoles, &rawShallow, &rawDeep );

		// ASSUMPTION: Regions containing other parts (rawDeep) have the largest
		// containment areas.  As such, they are processed last during coding.

		seqRules->BuoyReset();
		status = shortest.OptimizePath( seqRules, toolOrder, rawHoles, &cutOrder );
		rawHoles.BenignFlush();

		status = shortest.OptimizePath( seqRules, toolOrder, rawShallow, &cutOrder );
		rawShallow.BenignFlush();

		status = shortest.OptimizePath( seqRules, toolOrder, rawDeep, &cutOrder );
		rawDeep.BenignFlush();

		SpecialInsert( cutOrder, &m_finalOrder );

		cutOrder.BenignFlush();

		indxA = indxB;
	}

	EWMFile( FILE_INFO );

	return status;
}

CReturn
CContHier::SequenceByLocalContainment( CSeqRules* seqRules )
{
	CReturn			status;
	CToolSequencer	toolSeq;
	CDbEntityArray	toolOrder;
	CDbEntityArray	rawHoles;
	CDbEntityArray	rawOrder;
	CDbEntityArray	cutOrder;
	CShortestPath	shortest;
	int				indxA, indxB;

	EWMFile( FILE_INFO );

	toolSeq.ToolSetupOrder( seqRules->Model(), FALSE, &toolOrder );

	CDbEntity::NewAction();

	indxA = 0;
	while (1)
	{
		// Find the range of nodes [indxA..indxB-1] having the same
		// containment level.
		indxB = NodesScan( m_nodes, indxA );
		if (indxB < 0)
			break;

		if (indxA == 0)
		{
			// Generate the shortest path between parts at depth zero.
			// This data requires special processing because of its format.

			EntitiesCollect( m_nodes, indxA, indxB, &rawHoles, &rawOrder, &rawOrder );

			seqRules->BuoyReset();
			status = shortest.OptimizePath( seqRules, toolOrder, rawHoles, &cutOrder );
			status = shortest.OptimizePath( seqRules, toolOrder, rawOrder, &cutOrder );

			NodesReorder( indxA, indxB, cutOrder );
		}

		indxA = indxB;
	}

	indxB = NodesScan( m_nodes, 0 );
	for (indxA = 0; indxA < indxB; ++indxA)
	{
		CContHierNode*	hier = m_nodes[indxA];
		LocalOrder( seqRules, toolOrder, hier, &m_finalOrder );
	}

	EWMFile( FILE_INFO );

	return status;
}

CReturn
CContHier::SequenceByProgressive( CSeqRules* seqRules )
{
	CReturn			status;
	CDbEntityArray	punch;
	CDbEntityArray	torchShallow;
	CDbEntityArray	torchDeep;
	CDbEntityArray	cutOrder;
	CShortestPath	shortest;
	int				idxA, idxB;

	EWMFile( FILE_INFO );

	SortProgressive( &m_nodes );

	CDbEntity::NewAction();

	idxA = 0;
	while (1)
	{
		// Find the range of nodes [indxA..indxB-1] having the same
		// containment level.
		idxB = NodesScan( m_nodes, idxA );
		if (idxB < 0)
			break;

		// Collect all the relevant entities at this containment level.
		ProgressiveCollect( seqRules, m_nodes, idxA, idxB, &punch, &torchShallow, &torchDeep );

		status = shortest.OptimizeTSP( seqRules, NULL, punch, 0, punch.Count()-1, &cutOrder );
		punch.BenignFlush();

		status = shortest.OptimizeTSP( seqRules, NULL, torchShallow, 0, torchShallow.Count()-1, &cutOrder );
		torchShallow.BenignFlush();

		status = shortest.OptimizeTSP( seqRules, NULL, torchDeep, 0, torchDeep.Count()-1, &cutOrder );
		torchDeep.BenignFlush();

		SpecialInsert( cutOrder, &m_finalOrder );

		cutOrder.BenignFlush();

		idxA = idxB;
	}

	EWMFile( FILE_INFO );

	return status;
}


CReturn
CContHier::SequenceByAvoidance( CSeqRules* seqRules )
{
	CReturn			status;
	CDbEntityArray	deep;
	CDbEntityArray	smallShallow;
	CDbEntityArray	largeShallow;
	CDbEntityArray	preCutOrder;
	CDbEntityArray	cutOrder;
	CShortestPath	shortest;
	int				idxA, idxB;

	EWMFile( FILE_INFO );

	SortProgressive( &m_nodes );

	CDbEntity::NewAction();

	idxA = 0;
	while (1)
	{
		// Find the range of nodes [indxA..indxB-1] having the same
		// containment level.
		idxB = NodesScan( m_nodes, idxA );
		if (idxB < 0)
			break;

		// Collect all the relevant entities at this containment level.
		AvoidanceCollect( seqRules, m_nodes, idxA, idxB, &deep, &smallShallow, &largeShallow );

		status = shortest.OptimizeTSP( seqRules, NULL, smallShallow, 0, smallShallow.Count()-1, &preCutOrder );
		smallShallow.BenignFlush();

		status = shortest.OptimizeTSP( seqRules, NULL, largeShallow, 0, largeShallow.Count()-1, &cutOrder );
		largeShallow.BenignFlush();

		shortest.CutAvoidance(&cutOrder);

		status = shortest.OptimizeTSP( seqRules, NULL, deep, 0, deep.Count()-1, &cutOrder );
		deep.BenignFlush();

		SpecialInsert( cutOrder, &m_finalOrder );
		SpecialInsert( preCutOrder, &m_finalOrder );

		preCutOrder.BenignFlush();
		cutOrder.BenignFlush();

		idxA = idxB;
	}

	EWMFile( FILE_INFO );

	return status;
}



// Create Poly versions of each arc/hole/profile (all assumed to be closed)
void
CContHier::PolysCreate(
				const CDbEntityArray& entityPool,
				CGeoPolyArray* polyArray,
				CDbEntityArray* entityArray )
{
	int count = entityPool.Count();
	if (count < 1)
		return;

	CGeoPoly* geoPoly = NULL;
	CGeoCurve* geoCurve = NULL;

	for (int indx = 0; indx < count; indx++)
	{
		CDbEntity* dbEntity = entityPool[indx];
		CDbEntity* owner = dbEntity->Owner();

		// First, un-mark the profile; that is, assume to be exterior
		dbEntity->IntSet( STR_CD, 0 );

		CDbHole* dbHole = dynamic_cast<CDbHole*>( dbEntity );
		CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbEntity );
		CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( dbEntity );
		CDbCommand*	dbCommand = dynamic_cast<CDbCommand*>( dbEntity );

		// Then, convert to poly
		geoPoly = NULL;

		if (dbHole != NULL)
		{
			CDbTool* dbTool = dbHole->Tool();
			if (dbTool != NULL)
			{
				geoPoly = new CGeoPoly();

				double diam = 1.e-3;  // ASSUMPTION: nothing nested in hole

				C3dCoord pc = dbHole->Center();
				C3dCoord ps = pc + C3dVec( (0.5 * diam), 0.0, 0.0 );

				geoCurve = new CGeoArc( ps, ps, pc, CCW );
				geoPoly->CopyAppend( *geoCurve );
				delete geoCurve;
			}
		}
		else if (dbCurve != NULL)
		{
			// Since are processing toolpath, a curve will always have an owner.

			CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( owner );
			if (dbFeature != NULL)
			{
				if ( CModelUtil::IsLeadEntity( dbCurve ) )
				{
					// We want to avoid considering lead in/out entities during
					// contaimnent analysis.  As they are 'connected' to other
					// curves/profiles, they will be considered later in the
					// processes by CPartitionSequencer::EntitiesTransfer()
					dbCurve = NULL;
				}
			}
			else
			{
				dbProfile = dynamic_cast<CDbProfile*>( owner );
			}
		}
		else if (dbCommand != NULL)
		{
			if (dbCommand->IsInstance() || dbCommand->IsTooledText())
			{
				geoPoly = new CGeoPoly();

				double diam = 1.e-3;

				C3dCoord pc = dbCommand->Coord();
				C3dCoord ps = pc + C3dVec( (0.5 * diam), 0.0, 0.0 );

				geoCurve = new CGeoArc( ps, ps, pc, CCW );
				geoPoly->CopyAppend( *geoCurve );
				delete geoCurve;
			}
		}

		if (dbProfile != NULL)
		{
			geoPoly = PolyFromExplicitProfile( (*dbProfile), &indx );
		}
		else if (dbCurve != NULL)
		{
			geoPoly = PolyFromImplicitProfile( entityPool, &indx );
		}

		if (geoPoly != NULL)
		{
			polyArray->Append( geoPoly );
			entityArray->Append( dbEntity );
		}
	}
}

void
CContHier::Sort( bool increasingOrder, CCHNArray* nodes )
{
	CContHierNode*	hierNode;
	int				count, indx;

	if ( increasingOrder )
		nodes->Qsort( CContHier::IncreasingOrderCompare );
	else
		nodes->Qsort( CContHier::DecreasingOrderCompare );

	count = nodes->Count();
	for (indx = 0; indx < count; ++indx)
	{
		hierNode = (*nodes)[indx];
		hierNode->Reduce();
	}
}

void
CContHier::SortProgressive( CCHNArray* nodes )
{
	CContHierNode*	hierNode;
	int				count, indx;

	count = nodes->Count();
	for (indx = 0; indx < count; ++indx)
	{
		hierNode = (*nodes)[indx];
		//
		// Force the freaking depth on holes and commands
		//
		CDbEntity* entity = hierNode->Root();
		if ( (entity->Type() == DBHOLE)
			|| (entity->Type() == DBCOMMAND) )
		{
			entity->IntSet( STR_CD, 1 );
		}
	}

	nodes->Qsort( CContHier::IncreasingOrderCompare );

	count = nodes->Count();
	for (indx = 0; indx < count; ++indx)
	{
		hierNode = (*nodes)[indx];
		hierNode->Reduce();
	}
}

// Starting at m_nodes[indxA], find the last index of a node
// having the containment level as the node m_nodes[indxA].
//
int
CContHier::NodesScan( const CCHNArray& nodes, int indxA )
{
	int	indxB;
	int	level;
	int	depth;
	int	count;
	
	count = nodes.Count();
	if (indxA >= count)
		return -1;

	level = ContainmentDepth( nodes, indxA );

	for (indxB = indxA; indxB < count; ++indxB)
	{
		depth = ContainmentDepth( nodes, indxB );
		if (depth < 0)
		{
			indxB = -1;	// premature exit
			break;
		}

		if (depth != level)
			break;
	}

	return indxB;
}

int
CContHier::ContainmentDepth( const CCHNArray& nodes, int indx )
{
	return ( nodes[indx]->RootDepth() );
}

void
CContHier::EntitiesCollect(
					const CCHNArray&	nodes,
					int					indxA,
					int					indxB,
					CDbEntityArray*		rawHole,
					CDbEntityArray*		rawShallow,
					CDbEntityArray*		rawDeep )
{
	CContHierNode*	hierNode;
	CDbEntity*		dbEntity;

	while (indxA < indxB)
	{
		hierNode = nodes[indxA];

		dbEntity = hierNode->Root();

		if (!dbEntity->DidAction())
		{
		if (dbEntity->Type() == DBHOLE)
		{
			rawHole->Append( dbEntity );
		}
		else if (hierNode->Contained().Count() > 0)
		{
			rawDeep->Append( dbEntity );

			if (dynamic_cast<CDbCurve*>( dbEntity ) != NULL)
				SiblingsAppend( dbEntity, rawDeep );
		}
		else
		{
			rawShallow->Append( dbEntity );

			if (dynamic_cast<CDbCurve*>( dbEntity ) != NULL)
				SiblingsAppend( dbEntity, rawShallow );
		}
			dbEntity->DoAction();
		}

		++indxA;
	}
}

void
CContHier::ProgressiveCollect(
					CSeqRules*			seqRules,
					const CCHNArray&	nodes,
					int					indxA,
					int					indxB,
					CDbEntityArray*		punch,
					CDbEntityArray*		torchShallow,
					CDbEntityArray*		torchDeep )
{
	CContHierNode*	hierNode;
	CDbEntity*		dbEntity;

	while (indxA < indxB)
	{
		hierNode = nodes[indxA];

		dbEntity = hierNode->Root();

		if (!dbEntity->DidAction())
		{
		if (dbEntity->Type() == DBHOLE)
		{
			punch->Append( dbEntity );
		}
		else
		if (dbEntity->Type() == DBCOMMAND)
		{
			if (seqRules->scribe_with_torch)
			{ 
				torchShallow->Append( dbEntity );
			}
			else
			{
				punch->Append( dbEntity );
			}
		}
		else 
		if (hierNode->Contained().Count() > 0)
		{
			torchDeep->Append( dbEntity );

			if (dynamic_cast<CDbCurve*>( dbEntity ) != NULL)
			{ SiblingsAppend( dbEntity, torchDeep ); }
		}
		else
		{
			torchShallow->Append( dbEntity );

			if (dynamic_cast<CDbCurve*>( dbEntity ) != NULL)
			{ SiblingsAppend( dbEntity, torchShallow ); }
		}
			dbEntity->DoAction();
		}

		++indxA;
	}
}

void
CContHier::AvoidanceCollect(
					CSeqRules*			seqRules,
					const CCHNArray&	nodes,
					int					indxA,
					int					indxB,
					CDbEntityArray*		deep,
					CDbEntityArray*		smallShallow,
					CDbEntityArray*		largeShallow )
{
	CContHierNode*	hierNode;
	CDbEntity*		dbEntity;

	while (indxA < indxB)
	{
		hierNode = nodes[indxA];

		dbEntity = hierNode->Root();

		if (!dbEntity->DidAction())
		{
		if (hierNode->Contained().Count() > 0)
		{
			deep->Append( dbEntity );

			if (dynamic_cast<CDbCurve*>( dbEntity ) != NULL)
			{ SiblingsAppend( dbEntity, deep); }
		}
		else
		{
			CDbProfile* dbProf = dynamic_cast<CDbProfile*>(dbEntity->Owner());

			CDbEntityArray* large_small = smallShallow;
			if (dbProf)
			{
				C3dBox extent = dbProf->Box();
				if ( (extent.Dx() > seqRules->avoid_min)
					|| (extent.Dy() > seqRules->avoid_min) )
				{ large_small = largeShallow; }
			}

			large_small->Append( dbEntity );

			if (dynamic_cast<CDbCurve*>( dbEntity ) != NULL)
			{ SiblingsAppend( dbEntity, large_small); }
		}
			dbEntity->DoAction();
		}

		++indxA;
	}
}


void
CContHier::SiblingsAppend( CDbEntity* dbEntity, CDbEntityArray* dbEntities )
{
	CDbContainer*	dbContainer;
	CDbCurve*		dbCurve;
	int				count, indx;

	dbContainer = dynamic_cast<CDbContainer*>( dbEntity->Owner() );
	if (dbContainer == NULL)
		return;

	count = dbContainer->Count();
	for (indx = 1; indx < count; ++indx)
	{
		dbCurve = dynamic_cast<CDbCurve*>( (*dbContainer)[indx] );
		if ( (dbCurve != NULL)
			&& !dbCurve->DidAction()
			)
		{
			dbEntities->Append( dbCurve );
			dbCurve->DoAction();
		}
	}
}

void
CContHier::NodesReorder( int indxA, int indxB, const CDbEntityArray& dbEntities )
{
	CContHierNode*	hierNode;
	CDbEntity*		dbEntity;
	int				count, indx;
	int				jndx;

	jndx = indxA;
	count = dbEntities.Count();
	int ncount = m_nodes.Count();
	// Entities count may be MUCH LARGER than nodes count...
	//
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = dbEntities[indx];

		hierNode = (CContHierNode*) dbEntity->IntGet( STR_HIER, 0 );

		if (hierNode != NULL)
		{
			// WHY IS THIS GETTING AN OVERRUN?
			if (jndx < ncount)
			{
				m_nodes.Replace( jndx, hierNode );
				++jndx;
			}
		}
	}
}

#if V16_ORIGINAL
void
CContHier::RecursiveOrder(
					CSeqRules*				seqRules,
					const CDbEntityArray&	toolOrder,
					const CDbEntityArray&	contained,
					CDbEntityArray*			finalOrder )
{
	CReturn			status;
	CContHierNode*	hierNode;
	CDbEntity*		parent;
	CDbEntityArray	rawHoles;
	CDbEntityArray	rawShallow;
	CDbEntityArray	rawDeep;
	CDbEntityArray	cutOrder;
	CShortestPath	shortest;
	int				count, jcnt, jndx;
	int				depth;

	count = contained.Count();
	if (count > 0)
	{
		if (DEEP_FIRST)
		{
			// Standard mode
			EntitiesCollect( contained, &rawHoles, &rawShallow, &rawDeep );
		}
		else
		{
			// Special mode turned on via registry.
			// Affects ordering of level-zero containment only.
			depth = contained[0]->IntGet( STR_CD, -1 );

			if (depth == 0)
				EntitiesCollect( contained, &rawHoles, &rawDeep, &rawDeep );
			else
				EntitiesCollect( contained, &rawHoles, &rawShallow, &rawDeep );
		}

		seqRules->BuoyReset();
		status = shortest.OptimizePath( seqRules, toolOrder, rawDeep, &cutOrder );
		rawDeep.BenignFlush();

		if ( seqRules->HolesAcrossLocalNest ||
			 (rawHoles.Count() > 0) )	// ugh. possible when coding subroutines.
		{
			// Process all holes at the current containment level.
			status = shortest.OptimizePath( seqRules, toolOrder, rawHoles, &cutOrder );
			rawHoles.BenignFlush();
		}

		status = shortest.OptimizePath( seqRules, toolOrder, rawShallow, &cutOrder );
		rawShallow.BenignFlush();

		jcnt = cutOrder.Count();
		for (jndx = 0; jndx < jcnt; ++jndx)
		{
			parent = cutOrder[jndx];

			hierNode = (CContHierNode*) parent->IntGet( STR_HIER, 0 );
			if (hierNode != NULL)
			{
				if ( !seqRules->AllHolesFirst && !seqRules->HolesAcrossLocalNest )
				{
					LocalHolesProcess( seqRules, toolOrder, parent, finalOrder );
				}

				RecursiveOrder( seqRules, toolOrder, hierNode->Contained(), finalOrder );
			}

			finalOrder->Append( parent );
		}

		cutOrder.BenignFlush();
	}
}
#else
void
CContHier::RecursiveOrder(
					CSeqRules*				seqRules,
					const CDbEntityArray&	toolOrder,
					const CDbEntityArray&	contained,
					CDbEntityArray*			finalOrder )
{
	CReturn			status;
	CContHierNode*	hierNode;
	CDbEntity*		parent;
	CDbEntityArray	rawHoles;
	CDbEntityArray	rawShallow;
	CDbEntityArray	rawDeep;
	CDbEntityArray	cutOrder;
	CShortestPath	shortest;
	int				count, jcnt, jndx;
	int				depth;

	EWMFile( FILE_INFO );

	count = contained.Count();
	if (count > 0)
	{
		if (DEEP_FIRST)
		{
			// Standard mode
			EntitiesCollect( contained, &rawHoles, &rawDeep, &rawDeep );
		}
		else
		{
			// Special mode turned on via registry.
			// Affects ordering of level-zero containment only.
			depth = contained[0]->IntGet( STR_CD, -1 );

			if (depth == 0)
				EntitiesCollect( contained, &rawHoles, &rawDeep, &rawDeep );
			else
				EntitiesCollect( contained, &rawHoles, &rawShallow, &rawDeep );
		}

		if ( seqRules->HolesAcrossLocalNest ||
			 (rawHoles.Count() > 0) )	// ugh. possible when coding subroutines.
		{
			// Process all holes at the current containment level.
			seqRules->BuoyReset();
			status = shortest.OptimizePath( seqRules, toolOrder, rawHoles, &cutOrder );
			rawHoles.BenignFlush();
		}

		seqRules->BuoyReset();
		status = shortest.OptimizePath( seqRules, toolOrder, rawDeep, &cutOrder );
		rawShallow.BenignFlush();

		jcnt = cutOrder.Count();
		for (jndx = 0; jndx < jcnt; ++jndx)
		{
			parent = cutOrder[jndx];

			hierNode = (CContHierNode*) parent->IntGet( STR_HIER, 0 );
			if (hierNode != NULL)
			{
				if ( !seqRules->AllHolesFirst && !seqRules->HolesAcrossLocalNest )
				{
					LocalHolesProcess( seqRules, toolOrder, parent, finalOrder );
				}

				RecursiveOrder( seqRules, toolOrder, hierNode->Contained(), finalOrder );
			}

			finalOrder->Append( parent );
		}

		cutOrder.BenignFlush();
	}

	EWMFile( FILE_INFO );
}
#endif

void
CContHier::EntitiesCollect(
					const CDbEntityArray&	dbEntities,
					CDbEntityArray*			rawHoles,
					CDbEntityArray*			rawShallow,
					CDbEntityArray*			rawDeep )
{
	CContHierNode*	hierNode;
	CDbEntity*		dbEntity;
	int				count, indx;

	count = dbEntities.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = dbEntities[indx];

		hierNode = (CContHierNode*) dbEntity->IntGet( STR_HIER, 0 );
		if (hierNode != NULL)  // had better not be NULL!
		{
			if (dbEntity->Type() == DBHOLE)
			{
				if ( !dbEntity->DidAction() )
					rawHoles->Append( dbEntity );
			}
			else if (hierNode->Contained().Count() > 0)
			{
				if ( !dbEntity->DidAction() )
					rawDeep->Append( dbEntity );

				if (dynamic_cast<CDbCurve*>( dbEntity ) != NULL)
					SiblingsAppend( dbEntity, rawDeep );
			}
			else
			{
				if ( !dbEntity->DidAction() )
					rawShallow->Append( dbEntity );

				if (dynamic_cast<CDbCurve*>( dbEntity ) != NULL)
					SiblingsAppend( dbEntity, rawShallow );
			}
		}
	}
}

void
CContHier::LocalOrder(
				CSeqRules*				seqRules,
				const CDbEntityArray&	toolOrder,
				CContHierNode*			hier,
				CDbEntityArray*			finalOrder )
{
	CReturn			status;
	CCHNArray		nodes;
	CDbEntityArray	rawHoles;
	CDbEntityArray	rawShallow;
	CDbEntityArray	rawDeep;
	CDbEntityArray	cutOrder;
	CShortestPath	shortest;
	int				indxA, indxB;

	EWMFile( FILE_INFO );

	if (hier->Contained().Count() > 0)
	{
		NodesCollect( hier, &nodes );

		// Sort on decreasing order of containment.
		Sort( DECREASING_ORDER, &nodes );

		indxA = 0;
		while (1)
		{
			// Find the range of nodes [indxA..indxB-1] having the same
			// containment level.
			indxB = NodesScan( nodes, indxA );
			if (indxB < 0)
				break;

			EntitiesCollect( nodes, indxA, indxB, &rawHoles, &rawShallow, &rawDeep );

			if ( seqRules->HolesAcrossLocalNest )
			{
				// Process all holes at this containment before cutting anything.
				status = shortest.OptimizePath( seqRules, toolOrder, rawHoles, &cutOrder );
				rawHoles.BenignFlush();
			}

			seqRules->BuoyReset();
			status = shortest.OptimizePath( seqRules, toolOrder, rawShallow, &cutOrder );
			rawShallow.BenignFlush();

			status = shortest.OptimizePath( seqRules, toolOrder, rawDeep, &cutOrder );
			rawDeep.BenignFlush();

			// SpecialInsert( cutOrder, finalOrder );
			{
				int bar = cutOrder.Count();
				for (int foo = 0; foo < bar; ++foo)
				{
					finalOrder->Append( cutOrder[foo] );
				}
			}

			cutOrder.BenignFlush();

			indxA = indxB;
		}
	}
	else
	{
		finalOrder->Append( hier->Root() );
	}

	if ( !seqRules->HolesAcrossLocalNest )
	{
		// Process all holes across the level-zero containment before cutting anything.

		status = shortest.OptimizePath( seqRules, toolOrder, rawHoles, &cutOrder );
		rawHoles.BenignFlush();

		SpecialInsert( cutOrder, finalOrder );

		cutOrder.BenignFlush();
	}

	EWMFile( FILE_INFO );
}

// Recursively collect all children (nodes) encompased by the given containment entity.
void
CContHier::NodesCollect(
					CContHierNode*	parent,
					CCHNArray*		nodes )
{
	CDbEntity*		dbEntity;
	CContHierNode*	child;
	int				count, indx;

	const CDbEntityArray& contained = parent->Contained();

	nodes->Append( parent );

	count = contained.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = contained[indx];
		child = (CContHierNode*) dbEntity->IntGet( STR_HIER, 0 );

		NodesCollect( child, nodes );
	}
}

// Recursively collect all holes encompased by the given containment entity.
void
CContHier::HolesCollect(
				CDbEntity*		root,
				CDbEntityArray*	rawHoles )
{
	CContHierNode*	hierNode;
	CDbEntity*		dbEntity;
	int				count, indx;

	hierNode = (CContHierNode*) root->IntGet( STR_HIER, 0 );
	if (hierNode != NULL)
	{
		const CDbEntityArray& contained = hierNode->Contained();

		if (rawHoles->Count() > 0)
		{
			if (root->Type() == DBHOLE)
			{
				rawHoles->Append( root );
			}
		}

		count = contained.Count();
		for (indx = 0; indx < count; ++indx)
		{
			dbEntity = contained[indx];

			if (dbEntity->Type() == DBHOLE)
			{
				rawHoles->Append( dbEntity );
			}

			HolesCollect( dbEntity, rawHoles );
		}
	}
}

// Collect and sequence all holes encompassed by the given level-zero containment entity.
void
CContHier::LocalHolesProcess(
					CSeqRules*				seqRules,
					const CDbEntityArray&	toolOrder,
					CDbEntity*				parent,
					CDbEntityArray*			finalOrder )
{
	CReturn			status;
	CDbEntityArray	rawHoles;
	CDbEntityArray	cutOrder;
	CShortestPath	shortest;
	CDbEntity*		dbEntity;
	int				count, indx;
	int				level;

	level = parent->IntGet( STR_CD, -1 );
	if (level == 0)
	{
		// Process all holes across the part.
		HolesCollect( parent, &rawHoles );

		status = shortest.OptimizePath( seqRules, toolOrder, rawHoles, &cutOrder );

		count = cutOrder.Count();
		for (indx = 0; indx < count; ++indx)
		{
			dbEntity = cutOrder[indx];
			if (dbEntity->IntGet( STR_HIER, 0 ) > 0)
			{
				finalOrder->Append( dbEntity );
			}
		}
	}
}


		// Trickery to get the correct cut order.
		// We are processing the containment levels from 0 to N
		// but we want the cut order to be N to 0, without
		// altering the results of shortest path optimization.
void
CContHier::SpecialInsert(
					const CDbEntityArray&	cutOrder,
					CDbEntityArray*			finalOrder )
{
	CDbEntity*	dbEntity;
	int			count, indx, jndx;

	jndx = 0;
	count = cutOrder.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbEntity = cutOrder[indx];

		if (dbEntity->IntGet( STR_HIER, 0 ) > 0)
		{
			if (jndx == 0)
				m_finalOrder.Prepend( dbEntity );
			else
				m_finalOrder.InsertAfter( jndx-1, dbEntity );

			++jndx;
		}
	}
}

CGeoPoly*
CContHier::PolyFromExplicitProfile( const CDbProfile& dbProfile, int* indx )
{
	CGeoPoly*	geoPoly;
	CGeoCurve*	geoCurve;
	int			count, jndx;

	geoPoly = new CGeoPoly();

	count = dbProfile.Count();
	for (jndx = 0; jndx < count; jndx++)
	{
		geoCurve = ((CDbCurve*) dbProfile[jndx])->Curve();
		geoPoly->CopyAppend( *geoCurve );
		delete geoCurve;
	}

	(*indx) += (count - 1);

	// 2008.03.05 (PE) -- The gap tolerance is used by code generation
	// to identify profiles that are 'logically closed'. In turn, this
	// helps avoid containment parity issues. We may have to extend this
	// check beyond just the start/end boundary.
	//
	// NOTE: SMALL is added to work around a floating point issue.
	// In particular, a failure was encountered when the m_gap_tol was
	// set via CCodeGenProcessApp::Generate() as 0.05 but appeared here
	// as 0.05000000001 (in the debugger). As such, a known gap of 0.05
	// failed to identified as being closed.
	//
	//   See also CSheet::Save()
	if ((m_gap_tol > SMALL) && geoPoly->IsClosed( m_gap_tol + SMALL ))
		geoPoly->GapsClose();

	return geoPoly;
}

// It is most likely the profile is a single full circle, but is possible that
// it represents some other closed shape that was drawn in "with tool" mode.
// Failing to return this latter case as a profile causes grievous harm downstream.
CGeoPoly*
CContHier::PolyFromImplicitProfile( const CDbEntityArray& entityPool, int* indx )
{
	CGeoPoly*		geoPoly;
	CDbCurve*		dbCurve;
	CGeoCurveArray	geoCurves;
	CGeoCurve*		geoCurve;
	CGeoCurve*		prevCurve;
	int				count, jndx;

	geoPoly = new CGeoPoly();

	count = entityPool.Count();
	for (jndx = (*indx); jndx < count; ++jndx)
	{
		dbCurve = dynamic_cast<CDbCurve*>( entityPool[jndx] );
		if (dbCurve == NULL)
			break;

		if ( CModelUtil::IsLeadEntity( dbCurve ) )
			break;

		geoCurve = dbCurve->Curve();

		geoCurves.Append( geoCurve );

		if (jndx > (*indx))
		{
			prevCurve = geoCurves[ geoCurves.Count()-2 ];
			if ( !prevCurve->EndPt().WithinTol( geoCurve->StartPt(), SMALL ) )
				break;
		}

		geoPoly->CopyAppend( *geoCurve );
	}

	geoCurves.DestructiveFlush();

	(*indx) = (--jndx);

	// 2008.03.05 (PE) -- The gap tolerance is used by code generation
	// to identify profiles that are 'logically closed'. In turn, this
	// helps avoid containment parity issues. We may have to extend this
	// check beyond just the start/end boundary.
	//   See also CSheet::Save()
	if ((m_gap_tol > SMALL) && geoPoly->IsClosed( m_gap_tol ))
		geoPoly->GapsClose();

	return geoPoly;
}

// For sorting the hier on increasing order of containment.
int
CContHier::IncreasingOrderCompare( const void* ptrA, const void* ptrB )
{
	CContHierNode* nodeA = ( *(CContHierNode**) ptrA);
	CContHierNode* nodeB = ( *(CContHierNode**) ptrB);

	CDbEntity* entityA = nodeA->Root();
	CDbEntity* entityB = nodeB->Root();

	int depthA = entityA->IntGet( STR_CD, 0 );
	int depthB = entityB->IntGet( STR_CD, 0 );

	int diff = (depthA - depthB);
	if (diff == 0)
	{
		if (entityB->Type() == DBHOLE && entityA->Type() != DBHOLE)
		{
			// Slip 214 -- outer profile was burned before pierce hole was punched.
			diff = 1;
		}
	}

	return diff;
}

// For sorting the hier on increasing order of containment.
int
CContHier::DecreasingOrderCompare( const void* ptrA, const void* ptrB )
{
	CContHierNode* nodeA = ( *(CContHierNode**) ptrA);
	CContHierNode* nodeB = ( *(CContHierNode**) ptrB);

	CDbEntity* entityA = nodeA->Root();
	CDbEntity* entityB = nodeB->Root();

	int depthA = entityA->IntGet( STR_CD, 0 );
	int depthB = entityB->IntGet( STR_CD, 0 );

	int diff = (depthB - depthA);
	if (diff == 0)
	{
		if (entityB->Type() == DBHOLE && entityA->Type() != DBHOLE)
		{
			// Slip 214 -- outer profile was burned before pierce hole was punched.
			diff = 1;
		}
	}

	return diff;
}

bool
CContHier::IsDoubleHit( const CDbEntity& entityA, const CDbEntity& entityB )
{
	bool is_double_hit = false;

	if ((entityA.Type() == DBHOLE) && (entityB.Type() == DBHOLE))
	{
		// 2007.02.24 (PE) -- Prior to this date, there was no proximity test.
		// This caused total failure of hole sequencing across local nested parts.
		if (entityA.Tool() == entityB.Tool())
		{
			C3dCoord pcA = ((const CDbHole&) entityA).Center();
			C3dCoord pcB = ((const CDbHole&) entityB).Center();

			is_double_hit = pcA.WithinTolXY( pcB, 1.e-3 );  // arbitrary tolerance.
		}
	}

	return is_double_hit;
}

bool
CContHier::IsClosed( CGeoPoly* poly )
{
	// Get the cached value (if any).
	int	closed = poly->IntGet( "closed", -1 );

	if (closed < 0)
	{
		// Check for trivial closure (arbitrary tolerance).
		closed = (poly->IsClosed( 1.e-3 ) ? 1 : 0);
		if ( !closed )
		{
			int count = poly->Count();
			if (count > 1)
			{
				// Assume the winding of the profile exceeds 360 degrees. One way
				// this is possible is if leads were applied automatically using
				// the overlap option. If this is the case, then the end point of
				// the terminal curve will (most likely) lay on the starting curve
				// of the profile. There may, of course, be other cases where this
				// condition is true but where the geometric configuration differs.
				// For the purpose of sequencing for code generation, we treat
				// this condition as representing a closed profile so that our
				// containment-parity test behaves as desired.

				// Check for the condition where the winding exceeds 360 degrees.
				// ASSUMPTION: This typically happens because the leads were created using overlap.
				C3dCoord	closestPt;
				double		u, dist;

				const CGeoCurve& curveA = (*poly)[0];
				const CGeoCurve& curveB = (*poly)[count-1];

				dist = curveA.PointClosest( curveB.EndPt(), &closestPt, &u );
				
				closed = (((dist < 1.e-3) && ((u > 0.) && (u <= 1.))) ? 2 : 0);
			}
			else
			{
				// Nothing else to do. Whether the poly represents a line or an arc,
				// it is definitely an open profile containing a single (or no) curve.
			}
		}

		// Cache the value to reduce future computation.
		poly->IntSet( "closed", closed );
	}


	return (closed > 0);
}

void
CContHier::GapTolSet( double gap_tol )
{
	m_gap_tol = gap_tol;
}


CReturn
CContHier::PhenolicHierInit( const CDbEntityArray& entityPool )
{
	CReturn status;
	CDbEntityArray cuts;

	int indx = 0;
	while (1)
	{
		int jndx = PhenolicCutsCollect( entityPool, indx, &cuts );
		if (jndx < 0)
			break;

		PhenolicCutsOrder( &cuts );

		PhenolicOrderSet( cuts );

		cuts.BenignFlush();
		indx = jndx + 1;
	}

	// Sort the nodes on increasing order of containment.
	Sort( INCREASING_ORDER, &m_nodes );

	return status;
}

int
CContHier::PhenolicCutsCollect(
	const CDbEntityArray&	entityPool,
	int						indx,
	CDbEntityArray*			cuts )
{
	int jndx = -1;  // assume failure

	int count = entityPool.Count();
	if (indx < count)
	{
		CDbEntity* entity = entityPool.GetAt(indx);

		int partId = entity->IntGet( "~pp", 0 );
		if (partId > 0)  // this should be an assertion?
		{
			cuts->Append( entity );

			// Loop until we find a part-id mismatch.
			for (jndx = indx+1; jndx < count; ++jndx)
			{
				entity = entityPool.GetAt(jndx);

				int tempId = entity->IntGet( "~pp", 0 );
				if (tempId != partId)
					break;

				cuts->Append( entity );
			}
		}
	}

	return (jndx - 1);
}

// Sort tool indices highest to lowest.
int ToolIndexSort( const void* ptrA, const void* ptrB )
{
	CDbEntity* entityA = (*(CDbEntity**) ptrA);
	CDbEntity* entityB = (*(CDbEntity**) ptrB);

	int indxA = entityA->Id();
	int indxB = entityB->Id();

	return (indxB - indxA);
}

void
CContHier::PhenolicCutsOrder( CDbEntityArray* cuts )
{
	assert( m_phenolicToolOrder != NULL );
	int tcnt = m_phenolicToolOrder->Count();
	if (tcnt > 0)
	{
		int count = cuts->Count();
		for (int indx = 0; indx < count; ++indx)
		{
			CDbEntity* entity = cuts->GetAt( indx );
			CDbTool* dbTool = entity->Tool();

			int tndx = m_phenolicToolOrder->Find( dbTool );
			assert( tndx >= 0 );
			if (tndx >= 0)
			{
				// "~pt" mean phenolic-tool.
				entity->IntSet( "~pt", tndx );
			}
		}
	}

	cuts->Qsort( &ToolIndexSort );
}

// Set the "containment relationships" of the cuts for a part.
// NOTE: There is no real sense of containment because (at first
// implementation) all cuts are open profiles. Since the cuts
// have been ordered by tool-order, we assign a Nth-tool cut the
// containment-depth of zero. All subsequent cuts become childern
// of that cut.
void
CContHier::PhenolicOrderSet( const CDbEntityArray& cuts )
{
	int count = cuts.Count();
	if (count > 0)  // not really necessary but ....
	{
		int bestIndx = PhenolicBestSeedGet( cuts );

		CDbEntity* bestCut = cuts.GetAt( bestIndx );
		CDbTool* nthTool = bestCut->Tool();

		// Our surrogate containment profile.
		CDbProfile* outer = dynamic_cast<CDbProfile*>( bestCut->Owner() );

		CContHierNode* hierNode = new CContHierNode();
		m_nodes.Append( hierNode );

		bestCut->IntSet( STR_CD, 0 );
		hierNode->Root( bestCut );

		for (int indx = 0; indx < count; ++indx)
		{
			CDbEntity* cut = cuts.GetAt( indx );
			CDbTool* dbTool = cut->Tool();

			if (dbTool == nthTool)
			{
				// Move all curves associated with the Nth-tool into the
				// 'outer' profile, in essence treating the profile as if
				// it is outermost containment profile.
				cut->IntSet( STR_CD, 0 );
				CDbContainer* owner = dynamic_cast<CDbContainer*>( cut->Owner() );
				if (owner != outer)
				{
					outer->Append( cut, FALSE );
				}
			}
			else
			{
				cut->IntSet( STR_CD, 1 );

				hierNode->Append( cut );

				CContHierNode* siblingNode = new CContHierNode();
				siblingNode->Root( cut );
				m_nodes.Append( siblingNode );
			}
		}
	}
}

// Find the "best" Nth-tool cut to be used as the seed for the
// optimizing trend (progression direction).
// ASSUMPTION: The entities in 'cuts' have been sorted on tool-order.
int
CContHier::PhenolicBestSeedGet( const CDbEntityArray& cuts )
{
	C3dCoord bestPt;  // ctor defaults to an undefined-point.
	int bestIndx = -1;

	int count = cuts.Count();

	int indx = count - 1;
	CDbEntity* dbEntity = cuts.GetAt( indx );

	CDbTool* nthTool = dbEntity->Tool();
	while (1)
	{
		if (indx < 0)
			break;

		dbEntity = cuts.GetAt( indx );

		CDbTool* dbTool = dbEntity->Tool();
		if (dbTool != nthTool)
			break;  // exhausted nthTool cuts

		// ASSUMPTION: Any part that was tooled-up during the import
		// process will contain only curves having an owning-profile.
		CDbProfile* owner = dynamic_cast<CDbProfile*>( dbEntity->Owner() );
		if (owner != NULL)
		{
			CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbEntity );

			// We are interested only in the 0th curve of any profile.
			int position = owner->Position( dbCurve );
			if (position == 0)
			{
				// We've encounter the 0th curve of an Nth-cut profile.
				C3dCoord ps = dbCurve->StartPt();

				if ((bestIndx < 0) || IsBetterSeed( bestPt, ps ))
				{
					bestPt = ps;
					bestIndx = indx;
				}
			}
		}

		--indx;
	}

	// In the unlikely case that we've failed to find a candidate, punt.
	if (bestIndx < 0)
		bestIndx = count - 1;

	return bestIndx;
}

// NOTE: IsBetterSeed() may require refinement as we learn more.
bool
CContHier::IsBetterSeed( const C3dCoord& best, const C3dCoord& pt )
{
	bool isBetter = false;

	switch (m_phenolicTrend)
	{
	case LL_XPOS:
	case LL_YPOS:
		// isBetter = ((pt.X() <= best.X()) && (pt.Y() <= best.Y()));
		isBetter = (pt.X() <= best.X());
		break;

	case LR_XNEG:
	case LR_YPOS:
		// isBetter = ((pt.X() >= best.X()) && (pt.Y() <= best.Y()));
		isBetter = (pt.X() >= best.X());
		break;

	case UR_XNEG:
	case UR_YNEG:
		// isBetter = ((pt.X() >= best.X()) && (pt.Y() >= best.Y()));
		isBetter = (pt.X() >= best.X());
		break;

	case UL_XPOS:
	case UL_YNEG:
		// isBetter = ((pt.X() <= best.X()) && (pt.Y() >= best.Y()));
		isBetter = (pt.X() <= best.X());
		break;
	}

	return isBetter;
}
