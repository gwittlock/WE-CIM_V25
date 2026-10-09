 
#include "stdafx.h"
#include "ColorConst.h"
#include "MathConst.h"
#include "StringConst.h"
#include "cmn_resource.h"
#include "AppConst.h"

#include "Return.h"
#include "Register.h"
#include "Msg.h"
#include "3dBox.h"
#include "ClfileConsts.h"
#include "GeoPoint.h"
#include "DbAllEntities.h"
#include "DbIterator.h"
#include "Model.h"
#include "SpeedCalc.h"
#include "ModelClfile.h"
#include "WorkPkg.h"

#include "CodeUtil.h"
#include "Optimizer.h"
#include "Profile.h"
#include "Worm.h"
#include "Nibbler.h"

#include "ShortestPath.h"

#define BEFORE	0
#define AFTER	1


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CModelClfile::CModelClfile()

	: CClfile(),
	  m_autoModDb(),
	  m_model( NULL ),
	  m_currGlobalPos( NULL ),
	  m_currLocalPos( NULL ),
	  m_currTool( NULL ),
	  m_currEntity( NULL ),
	  m_speedramp( TRUE ),  // TODO: set as given by io_cmd
	  m_nibble( FALSE ),
	  m_clfile()
{
	m_record_order = false;
}

CModelClfile::~CModelClfile()
{
	int count = m_clfile.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = (CDbEntity*) m_clfile[indx]->Entity();

		if (dbEntity != NULL)
		{
			// NOTE: The entity pointer associated with a clfile record can
			// be NULL when the record event is EV_MAIN_BEGIN, for instance.

			CVarList* attribs = dbEntity->pAttrib();

			if (dbEntity->Type() != DBTOOL)
			{
				// Remove any station data that was attached to the database
				// entities as a result of applying toolpath optimization.

				attribs->deleteVar( STR_NC_CODE_NUMBER );
			}

			int jndx = 0;
			while (jndx < dbEntity->Attrib().countVar())
			{
				// Remove all temporary attributes.  In particular, remove
				// 'user commands' that were copied from containers to
				// lower-level entities.
				//
				// SEE ALSO:
				//     CModelClfile::UserCommandsProcess()
				//     COptimizer::prep_container()

				CVar* attrib = attribs->getVar( jndx );
				CString name = attrib->getName();

				if (name[0] == '~')
					attribs->deleteVar( jndx );
				else
					++jndx;
			}
		}
	}
	m_clfile.DestructiveFlush();

	delete m_currGlobalPos;
	delete m_currLocalPos;

	m_order.BenignFlush();
}

int
CModelClfile::Count() const
{
	return m_clfile.Count();
}

int
CModelClfile::CurrRecNo() const
{
	return m_clfile.Curr();
}

const ClfileRec*
CModelClfile::Fetch( int recNo ) const
{
	ClfileRec* rec = m_clfile[ recNo ];
	return rec;
}

int
CModelClfile::Read( long				recNo,
				    long*				recInfo,
				    long*				intArray,
				    long				intArraySize,
				    double*				dblArray,
				    long				dblArraySize ) const
{
	if ( RecordOutOfRange( "CModelClfile::Read()", recNo ) )
		return -1;

	// Provide for 'logical const'
	CModelClfile* cast = ((CModelClfile*) this);

	ClfileRec* rec = m_clfile[ recNo ];

	const CDbEntity* dbEntity = rec->Entity();

	recInfo[0] = rec->RecType();
	recInfo[1] = ((dbEntity == NULL) ? 0 : dbEntity->Id());

	DataTransfer( (*rec),
		intArray, intArraySize,
		dblArray, dblArraySize );

	AttribsCopyAppend( rec->Attrib(), cast->pAttrib() );

	if (rec->Elem() != NULL)
		AttribsCopyAppend( rec->Elem()->pAttrib(), cast->pAttrib() );

	if (dbEntity != NULL)
	{
		// In v12, an entity's name was stored as a attribute.
		// In v13, an entity's name was moved to CDbEntity but
		// we still need to support access to code generators.
		cast->pAttrib()->setString( "name", dbEntity->Name() );

		CDbEntity* owner = dbEntity->Owner();
		if (owner != NULL)
		{
			// Record the feature name.
			// Remember, the hierarchy is one of
			//
			//			Feature		Feature
			//				Entity		Profile
			//								Entity
			//
			CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( owner );

			if (dbFeature == NULL)
				dbFeature = dynamic_cast<CDbFeature*>( owner->Owner() );

			if (dbFeature != NULL)
				cast->pAttrib()->setString( "feature_name", dbFeature->Name() );
		}
	}

	return recNo;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE: We can get here via either Seq:Init: or CodeGen:Optimize:
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// The Seq:Init: Portal command is called upon opening the Seq_Order
// dialog.  In the case, new work-zones (and hence, new sequence
// objects) may be introduced.  We must add new 'insert markers'
// to the new sequence objects.
//
// The CodeGen:Optimize:id=0 Portal command is called from the
// Seq_Order dialog when the 'root' node is selected and the Optimize
// button is pressed.  In this case, the work-zone features are
// 'flattened' and their entities are added to new/existing sequence
// objects as required.  We must add new 'insert markers' to the new
// sequence objects.
//
// This method also gets called via the CodeGen:Generate: Portal
// command when you disable optimization.  In this case, we can
// forego adding insert markers.
//
CReturn 
CModelClfile::SequenceInit( CModel* model, bool addInsertMarkers ) 
{
	CReturn			status;
	COptimizer		opti;
	CWorkPkgArray	sheetWorkPkgs;
	CWorkPkgArray	subdefWorkPkgs;
	CDbSequence*	rootseq;
	CDbSequence*	subseq;
	CDbEntity*		dbEntity;
	CString			name;
	int				wcnt, wndx;
	int				ecnt, endx;
	int				zoneNum;

	bool inhibit = (model->Header().getInt( "inhibit", FALSE ) != FALSE);

	// NOTE: At first implementation, we will not have subdefs here.
	status = opti.PrepModel( model, &sheetWorkPkgs, &subdefWorkPkgs );
	if ( status.IsOk() )
	{
		model->EntityFind( "RootSequence", (CDbEntity**) &rootseq, DBSEQUENCE, DBSEQUENCE );
		if (rootseq == NULL)
		{
			model->EntityCreate( DBSEQUENCE, (CDbEntity**) &rootseq );

			rootseq->Name( "RootSequence" );
			rootseq->StringSet( STR_TYPE, "_root" );
			rootseq->IntSet( "_insert_ba", IUNDEFINED );
			rootseq->IntSet( "_insert_id", 0 );
		}

		wcnt = sheetWorkPkgs.Count();
		for (wndx = 0; wndx < wcnt; ++wndx)
		{
			CWorkPkg*		workPkg = sheetWorkPkgs[wndx];
			CDbEntityArray&	wpents = workPkg->Entities();
			CDbFeature*		workZone = workPkg->Feature();

			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// Create and prep a subsequence as necessary.
			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

			zoneNum = workZone->IntGet( "_zone_num", IUNDEFINED );

			name.Format( "WorkPkg%d", zoneNum );
			model->EntityFind( name, (CDbEntity**) &subseq, DBSEQUENCE, DBSEQUENCE );
			if (subseq == NULL)
			{
				// We have not yet established a sequence for this work zone.
				model->EntityCreate( DBSEQUENCE, (CDbEntity**) &subseq );

				subseq->Name( name );
				rootseq->StringSet( STR_TYPE, "_workzone" );
				subseq->IntSet( "_zone_num", zoneNum );
				subseq->IntSet( "_insert_ba", AFTER );
				subseq->IntSet( "_insert_id", IUNDEFINED );

				rootseq->Append( subseq );
			}

			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// Put the workzone entities into the subsequence.
			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

			ecnt = wpents.Count();
			for (endx = 0; endx < ecnt; ++endx)
			{
				dbEntity = wpents[endx];

				if (dbEntity->Sequence() == NULL)
				{
					// This entity has not yet been sequenced.

					subseq->Append( dbEntity );
				}
			}
		}

		if ( addInsertMarkers )
			InsertMarkersUpdate( model );
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CModelClfile::SequenceInit()" );
	}

	sheetWorkPkgs.DestructiveFlush();
	subdefWorkPkgs.DestructiveFlush();

	return status;
}

void
CModelClfile::InsertMarkersUpdate( CModel* model )
{
	CDbIterator		iter;
	CString			name;
	CDbSequence*	dbSequence;
	CDbCommand*		dbCommand;
	CDbWorkplane*	dbWork;
	CDbTool*		dbTool;
	int				zoneNum;
	ID				insertID;

	model->EntityFind( STR_TOP, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	model->EntityFind( STR_STOCK, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );

	// Add the insert markers to the sequence objects.
	iter.Init( model->Db(), DBSEQUENCE );
	while (1)
	{
		dbSequence = dynamic_cast<CDbSequence*>( iter() );
		if (dbSequence == NULL)
			break;

		// NOTE: The RootSequence object has an '_insert_id=0'.
		insertID = dbSequence->IntGet( "_insert_id", IUNDEFINED );

		if (insertID == IUNDEFINED)
		{
			// ASSUMPTION: We are processing a newly created sequence object
			// that was introduced via either Seq:Init: or CodeGen:Optimize:

			zoneNum = dbSequence->IntGet( "_zone_num", IUNDEFINED );

			// Create a new insert object.
			model->EntityCreate( DBCOMMAND, (CDbEntity**) &dbCommand );
			dbCommand->SystemFlag( true );
			dbCommand->Init( dbTool, dbWork, 0, 0, 0, "" );

			// Redundant, but makes life easier in VB
			dbCommand->StringSet( STR_TYPE, "_insert" );

			dbCommand->IntSet( "_insert_ba", BEFORE );
			dbCommand->IntSet( "_zone_num", zoneNum );

			name.Format( "insert%d", dbCommand->Id() );
			dbCommand->Name( name );  // so it is easily recognized in dumped model!
			dbCommand->ColorSet( DCOLOR_RED );

			dbSequence->Append( dbCommand );
			dbSequence->IntSet( "_insert_ba", BEFORE );
			dbSequence->IntSet( "_insert_id", dbCommand->Id() );
		}
		else if (insertID > 0)
		{
			// ASSUMPTION: We are processing an existing sequence object.

			model->EntityFind( insertID, (CDbEntity**) &dbCommand, DBCOMMAND, DBCOMMAND );
			if (dbCommand->Sequence() == NULL)
			{
				// We have gotten here via CodeGen:Optimize: because it
				// flushes a sequence object before resequencing the
				// entire work-zone.

				dbSequence->Append( dbCommand );
			}
			else
			{
				// Do nothing because the 'insert marker' should
				// already be in its correct location.
			}
		}

		iter.Next();
	}
}

void
CModelClfile::DataTransfer( const ClfileRec& rec,
						    long*        intArray,
						    long         intArraySize,
						    double*      dblArray,
						    long         dblArraySize ) const
{
	const CGeoElem* elem = rec.Elem();
	if (elem != NULL)
	{
		const C3dCoord& ps = elem->StartPt();
		const C3dCoord& pe = elem->EndPt();

		dblArray[DBL_XS] = ps.X();
		dblArray[DBL_YS] = ps.Y();
		dblArray[DBL_ZS] = ps.Z();

		dblArray[DBL_XE] = pe.X();
		dblArray[DBL_YE] = pe.Y();
		dblArray[DBL_ZE] = pe.Z();

		const CGeoArc* arc = dynamic_cast<const CGeoArc*>( elem );
		if (arc != NULL)
		{
			const C3dCoord& pc = arc->CenterPt();

			dblArray[DBL_XC] = pc.X();
			dblArray[DBL_YC] = pc.Y();
			dblArray[DBL_ZC] = pc.Z();
		}

		const CDbHole* hole = dynamic_cast<const CDbHole*>( rec.Entity() );
		if (hole != NULL)
		{
			dblArray[DBL_DEPTH] = hole->Depth();
		}
	}
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CReturn
CModelClfile::Init( CSeqRules* seqRules )
{
	CReturn			status;
	CDbEntityArray	mainOrder;
	CDbEntityArray	subdefOrder;

	m_model = seqRules->pModel();

	m_currGlobalPos = NULL;
	m_currLocalPos = NULL;
	m_currTool = NULL;
	m_clfile.DestructiveFlush();

	m_speedramp = (m_model->Header().getInt( "speedramp", FALSE ) != FALSE);
	m_nibble = (m_model->Header().getInt( "Ind_Hits", FALSE ) != FALSE);

	if ( m_speedramp )
	{
		// TODO: Perhaps get the database name from io_cmd instead?
		CString databaseName = CRegister::StringGetV( "ConfigurationManager", "Database", "<error>" );

		CString machineName = "";
		CString toolSetupName = m_model->Header().getString( "MachCfg", "" );
		CString materialName = m_model->Header().getString( "MatCfg", "" );

		status = m_autoModDb.Init(
			databaseName, machineName, toolSetupName, materialName, FALSE );

		m_speedramp = status.IsOk();
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	status = CutOrderGenerate( seqRules, &mainOrder, &subdefOrder );

	status = MainBegin();

	if ( status.IsOk() )
		status = SafeZoneInfo();

	if ( status.IsOk() )
		status = ProgramStart();

	if ( status.IsOk() )
		status = ProgramBody( mainOrder );

	if ( status.IsOk() )
		status = ProgramEnd();

	if ( status.IsOk() )
		status = MainEnd();

	if ( status.IsOk() )
		status = Subdefs( subdefOrder );

	if ( status.IsOk() && m_speedramp )
		AdditionalFormatting();

	return status;
}

// Access to the header variables was moved here from ProgramStart()
// because attributes such as user-defined code generator variables
// must be accessible before the first clfile record is read.  This
// was discovered when someone tried to write a code generator whose
// output format and units were variable.
CReturn
CModelClfile::MainBegin()
{
	ClfileRec* rec;

	(* (pAttrib()) ) = m_model->Header();

	CReturn status = RecAppend( EV_MAIN_BEGIN, &rec );
	return status;
}

CReturn
CModelClfile::MainEnd()
{
	ClfileRec* rec;
	CReturn status = RecAppend( EV_MAIN_END, &rec );
	return status;
}

// Allow the code generator to track the clamp positions.  This
// allows the code generator to create clamp-avoidance moves.
CReturn
CModelClfile::SafeZoneInfo()
{
	CReturn		status;
	CDbIterator	iter;

	CDbFeature*	dbFeature;
	ClfileRec*	rec;
	int			zoneNum;
	int			clampCount;

	/*
	From the dump of a model:
	(r)_clamp1_x="15.000000",(r)_clamp1_y="48.000000",
	(r)_clamp2_x="40.000000",(r)_clamp2_y="48.000000",
	(r)_clamp_lldx="-7.800000",(r)_clamp_lldy="-5.750000",
	(i)_clamp_num="2",(r)_clamp_trdx="7.800000",
	(r)_clamp_trdy="0.000000",(i)_hold="1",
	(r)_hold_c1dx="-8.400000",(r)_hold_c1dy="-4.800000",
	(r)_hold_c2dx="14.400000",(r)_hold_c2dy="-4.800000",
	(r)_hold_dia="3.000000",(r)_hold_x="19.200000",
	(r)_hold_y="24.000000",(r)_zone_bottom="48.000000",
	(r)_zone_left="0.000000",(i)_zone_num="1",
	(r)_zone_repo="36.000000",(r)_zone_right="60.000000",
	(r)_zone_top="0.000000",(s)Type="_zone",
	*/

	iter.Init( m_model->Db(), DBFEATURE );
	while (1)
	{
		dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
			break;

		iter.Next();

		// Work-zones are 1-based.
		zoneNum = dbFeature->IntGet( "_zone_num", 0 );
		if (zoneNum == 1)
		{
			clampCount = dbFeature->IntGet( "_clamp_num", 0 );
			if (clampCount > 0)
			{
				status = RecAppend( EV_CLAMP_INFO, &rec );
				if ( status.IsOk() )
				{
					// This allows us to access the _clamp.* attributes
					rec->Entity( dbFeature );
					rec->AttribsCopy( dbFeature->Attrib() );
				}
			}

			// The clamp information should be identical
			// in each work-zone.  That is why we can
			// bail after processing the first work-zone.
			break;
		}
	}

	return status;
}

// TODO: Any data contained in StartProgram (currently empty)?
CReturn
CModelClfile::ProgramStart()
{
	ClfileRec* rec;
	CReturn status = RecAppend( EV_START_PROGRAM_BEGIN, &rec );

	if ( status.IsOk() )
		status = RecAppend( EV_START_PROGRAM_END, &rec );

	return status;
}

CReturn
CModelClfile::ProgramEnd()
{
	ClfileRec* rec;
	CReturn status = RecAppend( EV_END_PROGRAM_BEGIN, &rec );

	if ( status.IsOk() )
		status = RecAppend( EV_END_PROGRAM_END, &rec );

	return status;
}

CReturn
CModelClfile::ProgramBody( const CDbEntityArray& mainOrder )
{
	return ( CutOrderProcess( mainOrder, FALSE ) );
}

CReturn
CModelClfile::Subdefs( const CDbEntityArray& subdefOrder )
{
	return ( CutOrderProcess( subdefOrder, TRUE ) );
}

bool
CModelClfile::HasEvenContainmentParity( const CDbEntity* dbEntity )
{
	bool evenParity = false;

	// 2011.12.31 (PE) -- The following conditional is a hack. The
	// reported problem says that, when using Process Flowchart,
	// every instance of a hole causes a part to be added to the
	// label list. Though the solution here resolves the problem
	// it is less-than-desirable. A better solution would be to
	// clearly identify the outermost profile and only add its
	// associated part when it is processed.
	if (dbEntity->Type() != DBHOLE)
	{
		// Get the containment-depth (as set by CContHier::Init()).
		int cd = dbEntity->IntGet( STR_CD, -1 );
		if ((cd >= 0) && ((cd & 0x01) == 0))
			evenParity = (dbEntity->IsChildOfLead() == false);
	}

	return evenParity;
}

CReturn
CModelClfile::CutOrderProcess( const CDbEntityArray& cutOrder, bool subdefs )
{
	CReturn		status;
	CString		msg;
	C3dCoord	pt;
	int			upper_bound, indx;

	msg.Format( "CutOrderProcess m_record_order <%s>",
		(m_record_order ? "TRUE" : "FALSE") );

	status.Diagnostic( msg );

	// The model header is used to track the world repo.
	// See also CModelClfile::UserCommandsProcess().
	m_model->pHeader()->setReal( "_zone_repo", 0. );

	upper_bound = cutOrder.Count() - 1;
	for (indx = 0; indx <= upper_bound; ++indx)
	{
		CDbEntity* dbEntity = cutOrder[ indx ];

		if ( subdefs )
		{
			ConditionalSubdefBegin( dbEntity );
		}

		if (dbEntity->Tool() != NULL)
		{
			status = Transition( dbEntity );
			if ( !status.IsOk() )
				break;
		}

		if ( m_record_order )
		{
			// As part of label generation for ITI.
			if ( HasEvenContainmentParity( dbEntity ) )
			{
				CDbFeature* part = PartGet( dbEntity );
				if ((part != NULL) && (part->IntGet( "~labeled", 0 ) == 0))
				{
					part->IntSet( "~labeled", 1 );

					msg.Format( "entity id <%d>", dbEntity->Id() );
					status.Diagnostic( msg );

					// And now the underlying pattern.
					CDbPattern* dbPattern = PatternGet( dbEntity );
					if (dbPattern != NULL)
						m_order.Append( dbPattern );
				}
			}
		}

		switch (dbEntity->Type())
		{
		case DBPOINT:
			status = Point( (const CDbPoint*) dbEntity );
			break;
		case DBLINE:
			status = Line( (const CDbLine*) dbEntity );
			break;
		case DBARC:
			status = Arc( (const CDbArc*) dbEntity );
			break;
		case DBHOLE:
			status = Hole( (const CDbHole*) dbEntity );
			break;
		case DBCOMMAND:
			status = Command( (const CDbCommand*) dbEntity );
			break;
		default:
			// status.Internal( IDS_ERROR );
			break;
		}

		if ( !status.IsOk() )
			break;

		if ( subdefs )
		{
			ConditionalSubdefEnd( dbEntity );
		}

		status = UserCommandsProcess( dbEntity, (indx < upper_bound) );
		if ( !status.IsOk() )
			break;

		m_currEntity = dbEntity;
	}

	return status;
}

void
CModelClfile::ConditionalSubdefBegin( const CDbEntity* dbEntity )
{
	int subdef = dbEntity->IntGet( STR_SUBDEF_BEGIN, 0 );
	if (subdef != 0)
	{
		ClfileRec*	rec;
		C3dCoord	pt;

		pt = CCodeUtil::StartPoint( dbEntity );

		RecAppend( EV_SUBDEF_BEGIN, &rec );
		rec->Elem( new CGeoPoint( pt ) );
		rec->Entity( dbEntity );
		rec->Attrib( &(dbEntity->Attrib()) );
	}
}

void
CModelClfile::ConditionalSubdefEnd( const CDbEntity* dbEntity )
{
	int subdef = dbEntity->IntGet( STR_SUBDEF_END, 0 );
	if (subdef != 0)
	{
		ClfileRec*	rec;

		RecAppend( EV_SUBDEF_END, &rec );
		rec->Entity( dbEntity );
	}
}

CReturn
CModelClfile::Point( const CDbPoint* dbPoint )
{
	ClfileRec* rec;
	CReturn status = RecAppend( EV_POINT, &rec );

	if ( status.IsOk() )
	{
		// Get the geometry in the local workplane.
		C3dCoord pt = dbPoint->Coord();

		rec->Elem( new CGeoPoint( pt ) );
		rec->Entity( dbPoint );
		rec->Attrib( &(dbPoint->Attrib()) );

		CurrPosUpdate( dbPoint->Coord(0), pt );
	}

	return status;
}

CReturn
CModelClfile::Line( const CDbLine* dbLine )
{
	ClfileRec* rec;
	CReturn status;

	if ( status.IsOk() )
	{
		CGeoLine* geoLine = dbLine->Line();

		if ( m_nibble && dbLine->Tool()->IsPunchTool() )
		{
			CNibbler nibbler;
			C3dCoordList pts;
			double feed;
			int count, indx;

			feed = CNibbler::DefaultFeedrate( (*dbLine) );
			
			nibbler.Nibble(	(*geoLine), feed, NIBBLE_BALANCED, &pts );

			count = pts.Count();
			for (indx = 0; indx < count; ++indx)
			{
				status += RecAppend( EV_DRILL_HOLE, &rec );
				rec->Elem( new CGeoPoint( (*(pts[indx])) ) );
				rec->Entity( dbLine );
				rec->Attrib( &(dbLine->Attrib()) );
			}
		}
		else
		{
			status = RecAppend( EV_LINE, &rec );

			rec->Elem( geoLine );
			rec->Entity( dbLine );
			rec->Attrib( &(dbLine->Attrib()) );
		}

		CurrPosUpdate( dbLine->EndPt(0), dbLine->EndPt() );

		dogleg(dbLine);
	}

	return status;
}

CReturn
CModelClfile::Arc( const CDbArc* dbArc )
{
	ClfileRec* rec;
	CReturn status;

	if ( status.IsOk() )
	{
		CGeoArc* geoArc = dbArc->Arc();

		if ( m_nibble && dbArc->Tool()->IsPunchTool() )
		{
			CNibbler nibbler;
			C3dCoordList pts;
			double feed;
			int count, indx;

			feed = CNibbler::DefaultFeedrate( (*dbArc) );
			
			nibbler.Nibble(	(*geoArc), feed, NIBBLE_BALANCED, &pts );

			count = pts.Count();
			for (indx = 0; indx < count; ++indx)
			{
				status += RecAppend( EV_DRILL_HOLE, &rec );
				rec->Elem( new CGeoPoint( (*(pts[indx])) ) );
				rec->Entity( dbArc );
				rec->Attrib( &(dbArc->Attrib()) );
			}
		}
		else
		{
			int evType = ((dbArc->Dir() < 0) ? EV_CW_ARC : EV_CCW_ARC);

			status = RecAppend( evType, &rec );

			rec->Elem( dbArc->Arc() );
			rec->Entity( dbArc );
			rec->Attrib( &(dbArc->Attrib()) );
		}

		CurrPosUpdate( dbArc->EndPt(0), dbArc->EndPt() );

		dogleg(dbArc);
	}


	return status;
}

CReturn
CModelClfile::Hole( const CDbHole* dbHole )
{
	CReturn		status;
	ClfileRec*	rec;

	status = RecAppend( EV_DRILL_HOLE, &rec );

	if ( status.IsOk() )
	{
		// Get the geometry in the local workplane.
		C3dCoord top;
		C3dCoord bot;

		top = dbHole->Center();

		bot = top;
		bot.Z( top.Z() - dbHole->Depth() );

		rec->Elem( new CGeoLine( top, bot ) );
		rec->Entity( dbHole );
		rec->Attrib( &(dbHole->Attrib()) );

		CurrPosUpdate( dbHole->Center(0), dbHole->Center() );
	}

	return status;
}

CReturn
CModelClfile::Command( const CDbCommand* dbCommand )
{
	CReturn		status;
	ClfileRec*	rec;
//	CDbTool*	dbTool;

	if ( dbCommand->IsInstance() )
	{
		status = RecAppend( EV_SUBCALL, &rec );

		// Get the geometry in the local workplane.

		rec->Elem( new CGeoPoint( dbCommand->Coord() ) );
		rec->Entity( dbCommand );
		rec->Attrib( &(dbCommand->Attrib()) );

		CurrPosUpdate( dbCommand->Coord(0), dbCommand->Coord() );
	}
	else if ( dbCommand->IsTooledText() )
	{
		status = RecAppend( EV_TEXT_COMMAND, &rec );

		// Get the geometry in the local workplane.

		rec->Elem( new CGeoPoint( dbCommand->Coord() ) );
		rec->Entity( dbCommand );
		rec->Attrib( &(dbCommand->Attrib()) );

		CurrPosUpdate( dbCommand->Coord(0), dbCommand->Coord() );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Processes the 'system' attributes that have corresponding events in
// the clfile.
// NOTE: Clamp information has already been processed by SafeZoneInfo()
CReturn
CModelClfile::UserCommandsProcess( CDbEntity* dbEntity, bool allow_repo )
{
	CReturn			status;
	CVarList		emptyList;
	CVar*			attrib;
	ClfileRec*		rec;
	EDbEntityType	type;
	double			xhold, yhold;
	double			xrepo, yrepo;
	double			delta;
	int				count, indx;
	int				dropstop, processed;

	const CVarList& attribs = dbEntity->Attrib();

	count = attribs.countVar();
	for (indx = 0; indx < count; ++indx)
	{
		attrib = attribs.getVar( indx );

		if (attrib->getName().GetAt(0) > '_')
		{
			// Since the attributes are lexically ordered in
			// increasing order, and because 'system' attributes
			// start with an underscore, we can terminate the
			// loop as soon as we find an attribute whose name
			// does not start with an underscore.
			break;
		}

		type = dbEntity->Type();

		if (type == DBFEATURE && ((CDbFeature*) dbEntity)->IsWorkZone())
		{
			processed = dbEntity->IntGet( "~processed", 0 );

			// NOTE: Prevent hold-down/repo events if they are the
			// very last event (ie. there are no more cutting events)
			// at the end of the program.
			// 
			if ( processed && allow_repo )
			{
				xhold = attribs.getReal( "_hold_x", UNDEFINED );
				yhold = attribs.getReal( "_hold_y", UNDEFINED );

				if (xhold < UNDEFINED && yhold < UNDEFINED)
				{
					RecAppend( EV_HOLD_COMMAND, &rec );
					rec->Elem( new CGeoPoint( xhold, yhold, 0. ) );
					rec->Entity( dbEntity );
					rec->Attrib( &attribs );
				}

				xrepo = m_model->Header().getReal( "_zone_repo", 0. );
				delta = attribs.getReal( "_zone_repo", UNDEFINED );

				yrepo = ((m_currLocalPos == NULL) ? UNDEFINED : m_currLocalPos->Y());

				// BugID: 652 -- a zero repo distance should not produce a
				// reposition event, but we do want to allow a negative
				// reposition (some people manually enter a negative value).
				if ((fabs(delta) > SMALL && delta < UNDEFINED) && yrepo < UNDEFINED)
				{
					xrepo += delta;
					m_model->pHeader()->setReal( "_zone_repo", xrepo );

					RecAppend( EV_REPO_COMMAND, &rec );
					rec->Elem( new CGeoPoint( xrepo, yrepo, 0. ) );
					rec->Entity( dbEntity );
					rec->Attrib( &attribs );
				}
			}
			else
			{
				// Used only during manual sequencing of cutting entities.
				RecAppend( EV_WORKZONE_INFO, &rec );
				rec->Entity( dbEntity );
	
				dbEntity->IntSet( "~processed", 1 );
			}
		}
		else if (type >= DBLINE && type <= DBHOLE)
		{
			dropstop = attribs.getInt( "_dropstop", 0 );

			if (dropstop > 0)
			{
				dropstop = ((dropstop == DROP_CONST) ? EV_DROP_COMMAND : EV_STOP_COMMAND);

				RecAppend( dropstop, &rec );
				if (type == DBLINE)
					rec->Elem( ((CDbLine*) dbEntity)->Line() );
				else if (type == DBARC)
					rec->Elem( ((CDbArc*) dbEntity)->Arc() );
				else if (type == DBHOLE)
					rec->Elem( new CGeoPoint( ((CDbHole*) dbEntity)->Center() ) );

				rec->Entity( dbEntity );
				rec->Attrib( &attribs );
			}
		}

		break;
	}

	return status;
}

CReturn
CModelClfile::ToolChange( const CDbEntity* dbEntity )
{
	CReturn status;
	bool	okay;

	okay = (dbEntity->Type() <= DBHOLE);

	if ( !okay )
	{
		okay = (dbEntity->Type() == DBCOMMAND && ((CDbCommand*) dbEntity)->IsTooledText());
	}

	if ( !okay )
		return status;  // tools are attached only to cutting entities.

	CDbTool* dbTool = dbEntity->Tool();
	if (dbTool == m_currTool)
		return status;

	m_currTool = dbTool;

	// This serves as an event, but it also causes the
	// clfile to be updated with the tool attributes
	ClfileRec* rec;
	status = RecAppend( EV_TOOL_CHANGE_BEGIN, &rec );
	if ( status.IsOk() )
	{
		rec->Entity( dbTool );
		rec->Attrib( dbTool->pAttrib() );
	}

	// Capture the relevant 'speed ramping' data.
	if ( m_speedramp )
	{
		CSpeedCalc calc;
		CVarList sfparams;

		double diam = dbTool->EffectiveDiameter();
		int toolType = dbTool->IntGet( STR_TYPE_ID, 0 );

		if ( m_autoModDb.SpeedFeed( toolType, diam, &sfparams ).IsOk() )
		{
			if ( calc.Init( sfparams ) )
			{
				status = RecAppend( EV_SPEED_RAMP_INFO, &rec );
				if ( status.IsOk() )
					rec->AttribsCopy( sfparams );
			}
		}
	}

	// This serves as an event, but it also causes the clfile file to be
	// updated with the default attributes like feed, speed, doff, etc.
	if ( status.IsOk() )
	{
		// Providing the start point of the first entity in
		// the feature reduces the need to do entity 'look ahead'.

		C3dCoord ps = CCodeUtil::StartPoint( dbEntity );

		status = RecAppend( EV_TOOL_CHANGE_END, &rec );

		if ( status.IsOk() )
		{
			rec->Elem( new CGeoPoint( ps ) );
			rec->Entity( dbEntity );
			rec->Attrib( &(dbEntity->Attrib()) );

			CurrPosUpdate( ps, ps );
		}
	}

	return status;
}

CReturn
CModelClfile::Transition( const CDbEntity* next )
{
	CReturn status;

	status = ToolChange( next );

	if ( !status.IsOk() )
		return status;

	if (m_currGlobalPos == NULL)
		return status;  // Can't do anything yet.


	C3dCoord nextGlobalPos = (*m_currGlobalPos);
	C3dCoord nextLocalPos = (*m_currLocalPos);

	switch (next->Type())
	{
	case DBPOINT:
		nextGlobalPos = ((const CDbPoint*) next)->Coord(0);
		break;
	case DBLINE:
		nextGlobalPos = ((const CDbLine*) next)->StartPt(0);
		break;
	case DBARC:
		nextGlobalPos = ((const CDbArc*) next)->StartPt(0);
		break;
	case DBHOLE:
		break;
	case DBPROFILE:
		break;
	case DBCOMMAND:
		break;
	case DBFEATURE:
		// status = ToolChange( (const CDbFeature&) next );
		break;
	default:
		status.Internal( IDS_ERROR );
		break;
	}

	if (status.IsOk() && !nextGlobalPos.WithinTol( (*m_currGlobalPos), SMALL ))
	{
		ClfileRec* rec;

		ID work = next->WorkplaneId();

		switch (next->Type())
		{
		case DBPOINT:
			nextLocalPos = ((const CDbPoint*) next)->Coord( work );
			break;
		case DBLINE:
			nextLocalPos = ((const CDbLine*) next)->StartPt( work );
			break;
		case DBARC:
			nextLocalPos = ((const CDbArc*) next)->StartPt( work );
			break;
		}

#if BEFORE_V19
		// Signal a retract.
		status = RecAppend( EV_RAPID, &rec );
		if ( status.IsOk() )
		{
			rec->Elem( new CGeoPoint( (*m_currLocalPos) ) );
			rec->Entity( next );
			rec->Attrib( &(next->Attrib()) );
		}
#else
		if (m_currEntity != NULL)
		{
			// EV_DISENGAGE was introduced soley to support the display of
			// clamp avoidance movements. To that end, all the command
			// really provides is an indication that we are about to rapid
			// traverse and (most importantly) a pointer to the entity
			// immediately preceding the disengage event. Since the clamp
			// avoidance movements are generated by the code generator, it
			// is the code generator that adds new CCodeGeoRec's that record
			// these events. Critically, rapid movements associated with these
			// events must 1) be drawn using the correct tool, 2) appear before
			// 'the usual' rapid traversal and 3) and rapid traversal sequence
			// must culminate at 'the usual' terminal position (ie. the
			// position prescribed by the subsequent EV_RAPID).
			status = RecAppend( EV_DISENGAGE, &rec );
			if ( status.IsOk() )
			{
				rec->Elem( new CGeoPoint( (*m_currLocalPos) ) );
				rec->Entity( m_currEntity );
				rec->Attrib( &(m_currEntity->Attrib()) );
			}
		}
#endif

		// Signal a rapid traversal.
		status = RecAppend( EV_RAPID, &rec );
		if ( status.IsOk() )
		{
			rec->Elem( new CGeoPoint( nextLocalPos ) );
			rec->Entity( next );
			rec->Attrib( &(next->Attrib()) );
		}
	}

	return status;
}

void
CModelClfile::CurrPosUpdate( const C3dCoord& globalPos, const C3dCoord& localPos )
{
	if (m_currGlobalPos == NULL)
		m_currGlobalPos = new C3dCoord();

	if (m_currLocalPos == NULL)
		m_currLocalPos = new C3dCoord();

	(*m_currGlobalPos) = globalPos;
	(*m_currLocalPos)  = localPos;
}

CReturn
CModelClfile::RecAppend( int recType, ClfileRec** rec )
{
	CReturn status;

	ClfileRec* tmp = new ClfileRec( recType );

	if (tmp == NULL)
		status.Fatal( IDS_MEM_ALLOC_FAILURE, "CModelClfile::RecAppend(#1)" );
	else
		m_clfile.Append( tmp );

	(*rec) = (( status.IsOk() ) ? tmp : NULL );

	return status;
}

CReturn
CModelClfile::RecInsertBefore( int indx, int recType, ClfileRec** rec )
{
	CReturn status;

	ClfileRec* tmp = new ClfileRec( recType );

	if (tmp == NULL)
		status.Fatal( IDS_MEM_ALLOC_FAILURE, "CModelClfile::RecInsertBefore(#1)" );
	else
		m_clfile.InsertBefore( indx, tmp );

	(*rec) = (( status.IsOk() ) ? tmp : NULL );

	return status;
}

CReturn
CModelClfile::RecInsertAfter( int indx, int recType, ClfileRec** rec )
{
	CReturn status;

	ClfileRec* tmp = new ClfileRec( recType );

	if (tmp == NULL)
		status.Fatal( IDS_MEM_ALLOC_FAILURE, "CModelClfile::RecInsertAfter(#1)" );
	else
		m_clfile.InsertAfter( indx, tmp );

	(*rec) = (( status.IsOk() ) ? tmp : NULL );

	return status;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

int
CModelClfile::RecordOutOfRange( const char* func, int recNo ) const
{
	if (recNo < 0 || recNo >= Count())
	{
		const char* tmplt = "Diagnostic from %s\n" \
							"    Record index <%d> out of range.";

		CString errMsg;
		errMsg.Format( tmplt, func, recNo );

		MsgDisplay( errMsg );
		return -1;
	}
	else
	{
		return 0;
	}
}

int
CModelClfile::TagOutOfRange( const char* func, int tag, int upperBound ) const
{
	if (tag < 0 || tag > upperBound)
	{
		const char* tmplt = "Diagnostic from %s\n" \
							"    Tag value <%d> out of range.";

		CString errMsg;
		errMsg.Format( tmplt, func, tag );

		MsgDisplay( errMsg );
		return -1;
	}
	else
	{
		return 0;
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CReturn
CModelClfile::CutOrderGenerate(
					CSeqRules*		seqRules,
					CDbEntityArray*	mainOrder,
					CDbEntityArray*	subdefOrder )
{
	CReturn status;

	if (seqRules->path_opt || seqRules->grid_opt )  // || seqRules->drill_opt )
	{
		OptiCutOrder( seqRules, mainOrder, subdefOrder );
	}
	else
	{
		ExplicitCutOrder( seqRules, mainOrder );
	}

	return status;
}

CReturn
CModelClfile::OptiCutOrder(
					CSeqRules*		seqRules,
					CDbEntityArray*	mainOrder,
					CDbEntityArray*	subdefOrder )
{
	CReturn			status;
	COptimizer		opti;
	CWorkPkgArray	sheetWorkPkgs;
	CWorkPkgArray	subdefWorkPkgs;
	bool			codeSubs;
	CModel*			model;

	status.Diagnostic( "*CModelClfile::OptiCutOrder(#1)" );

	model = seqRules->pModel();

	// Flatten the database (see also COptimizer::PrepModel())
	status = opti.PrepModel( model, &sheetWorkPkgs, &subdefWorkPkgs );

	if ( status.IsOk() )
	{
		OptiCutOrder( sheetWorkPkgs, seqRules, mainOrder );

		// NOTE: This is set in CCodeGenProcessApp::Generate().
		codeSubs = (m_model->Header().getInt( "subs", FALSE ) != FALSE);

		if (codeSubs && subdefWorkPkgs.Count() > 0)
		{
			OptiCutOrder( subdefWorkPkgs, seqRules, subdefOrder );
			
			// Update the location at which each subcall is made.
			SubcallsResolve( (*mainOrder) );
		}
	}

	sheetWorkPkgs.DestructiveFlush();
	subdefWorkPkgs.DestructiveFlush();

	status.Diagnostic( "*CModelClfile::OptiCutOrder(#2)" );

	return status;
}

CReturn
CModelClfile::OptiCutOrder(
						const CWorkPkgArray&	workPkgs,
						CSeqRules*				seqRules,
						CDbEntityArray*			cutOrder )
{
	CReturn			status;
	COptimizer		opti;
	CShortestPath	shortest;
	CDbEntityArray	optimizedEntities;
	CDbEntityArray	dbToolList;
	CDbFeature*		topLevelFeature;
	CModel*			model;
	int				count, indx;

	status.Diagnostic( "*-- CModelClfile::OptiCutOrder(#1)" );

	model = seqRules->pModel();

	count = workPkgs.Count();
	for (indx = 0; indx < count; ++indx)
	{
		CWorkPkg*		currWorkPkg = workPkgs[indx];
		CDbEntityArray&	workPkgEntities = currWorkPkg->Entities();

		// (V14.5 - 4/17/2001 - PE) Guard against 'empty' work zones.
		// Otherwise, the dx of the bounding box is 2 * UNDEFINED which,
		// in turn, appears to cause an infinite loop when doing grid opt.
		//
		if (workPkgEntities.Count() > 0)
		{
			// If TSP, don't step on the m_anchor, and don't care about the
			// trend vectors, etc.
			if (!seqRules->tsp_opt)
			{ seqRules->WorkPkgSet( currWorkPkg->Box() ); }

			if ( currWorkPkg->IsSubdef() && IsExplicitlySequenced( workPkgEntities ) )
			{
				CopyAndSort( workPkgEntities, &optimizedEntities );
			}
			else
			{
				status += opti.PrepToolOrder( seqRules->toolOrder, FALSE,
					(*model), workPkgEntities, &dbToolList );

				if (seqRules->toolOrder == TOOL_ORDER_BY_JOB)
				{
					seqRules->PassCountSet( dbToolList.Count() );
				}

				if (seqRules->cut_avoid)
				{
					status = opti.SequenceByAvoidance(
								seqRules, workPkgEntities, dbToolList, &optimizedEntities );
				}
				else
				if ( seqRules->path_opt )
				{
					status = shortest.OptimizePath(
								seqRules, dbToolList, workPkgEntities, &optimizedEntities );
				}
				else
				if ( seqRules->grid_opt )
				{
					if (seqRules->gridSeqOption == SEQUENCE_BY_TOOL_ORDER)
					{
						status = opti.SequenceByToolOrder(
									seqRules, workPkgEntities, dbToolList, &optimizedEntities );
					}
					else if (seqRules->gridSeqOption == SEQUENCE_BY_FLOW_CHART)
					{
						status = opti.SequenceByFlowChart(
									seqRules, workPkgEntities, dbToolList, &optimizedEntities );
					}
					else
					if (seqRules->gridSeqOption == SEQUENCE_BY_PROGRESSIVE)
					{
						status = opti.SequenceByProgressive(
									seqRules, workPkgEntities, dbToolList, &optimizedEntities );
					}
				}
			}

			topLevelFeature = currWorkPkg->Feature();

			if ( currWorkPkg->IsSubdef() )
			{
				SubdefUpdate( currWorkPkg, &optimizedEntities );
			}

			// Move this bucket of entities to the final cut order.
			CutOrderAppend( topLevelFeature, &optimizedEntities, cutOrder );

			dbToolList.BenignFlush();
		}
	}

	status.Diagnostic( "*-- CModelClfile::OptiCutOrder(#2)" );

	return status;
}

CReturn
CModelClfile::ExplicitCutOrder( CSeqRules* seqRules, CDbEntityArray* cutOrder )
{
	CReturn status;
	CDbSequence* rootSequence;

	CModel* model = seqRules->pModel();

	// We must manage the undo buffer because SequenceInit() creates entities.
	// model->UndoBufferPrepare();
	status = SequenceInit( model, FALSE );
	// model->UndoBufferCommit();
	if ( status.IsOk() )
		model->EntityFind( "RootSequence", (CDbEntity**) &rootSequence, DBSEQUENCE, DBSEQUENCE );

	if (rootSequence != NULL && status.IsOk())
	{
		rootSequence->Flatten( (DBSEQ_RECURSE | DBSEQ_WORKZONE), cutOrder );
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CModelClfile::ExplicitCutOrder()" );
	}

	return status;
}

void
CModelClfile::SubdefUpdate( CWorkPkg* workPkg, CDbEntityArray* optimizedEntities )
{
	C3dCoord	ps;
	CDbPattern*	dbPattern;
	CDbEntity*	subdefStart;
	CDbEntity*	subdefEnd;
	double		subx, suby;
	double		subu, subv;
	int			count;

	count = optimizedEntities->Count();

	subdefStart = (*optimizedEntities)[0];
	subdefEnd = (*optimizedEntities)[count-1];

	dbPattern = SubdefFind( (*subdefStart) );

	// Provide tags for CModelClfile::CutOrderProcess() to
	// generate EV_SUBDEF_BEGIN and EV_SUBDEF_END events.

	subdefStart->IntSet( STR_SUBDEF_BEGIN, 1 );
	subdefEnd->IntSet( STR_SUBDEF_END, 1 );

	// Get the temporary handle of the subroutine as
	// recorded by SubdefsPrep().
	subx = dbPattern->DoubleGet( STR_SUBX, 0. );
	suby = dbPattern->DoubleGet( STR_SUBY, 0. );

	// Get the actual starting point of the subdef
	// (established by optimizing the subdef).
	ps = CCodeUtil::StartPoint( subdefStart );

	subu = ps.X();
	subv = ps.Y();

#ifdef V16_ORIGINAL
	// Use the starting point of the part as the local origin
	// of the subroutine.  Attaching the STR_SUBU & STR_SUBV
	// attributes to the first entity in the subroutine allows
	// the code generator to output the subroutine definition
	// with the starting point at (0,0).
	subdefStart->DoubleSet( STR_SUBU, subu );
	subdefStart->DoubleSet( STR_SUBV, subv );

	// Attaching the STR_SUBU & STR_SUBV attributes to the
	// work package allows SubcallsResolve() to update the
	// call-out location (by adding the vector (u-x,v-y)
	// to the temporary handle point for the call out.

	dbPattern->DoubleSet( STR_SUBU, subu );
	dbPattern->DoubleSet( STR_SUBV, subv );
#else
	// NOTE: This was done for Modular Services.
	// Use the lower left corner of the part bounding box as
	// the local origin of the subroutine.  Attaching the
	// STR_SUBU & STR_SUBV attributes to the first entity
	// in the subroutine allows the code generator to output
	// the subroutine definition with the starting point at (0,0).
	subdefStart->DoubleSet( STR_SUBU, subu );
	subdefStart->DoubleSet( STR_SUBV, subv );

	// Attaching the STR_SUBU & STR_SUBV attributes to the
	// work package allows SubcallsResolve() to update the
	// call-out location (by adding the vector (u-x,v-y)
	// to the temporary handle point for the call out.

	dbPattern->DoubleSet( STR_SUBU, subu );
	dbPattern->DoubleSet( STR_SUBV, subv );
#endif
}

// Update the location at which each subcall is made.
// NOTE: Does not affect cutOrder but does affect contained entities.
void
CModelClfile::SubcallsResolve(
						const CDbEntityArray&	mainOrder )
{
#if V16_ORIGINAL
	CDbPattern*	dbPattern;
	CDbCommand*	dbCommand;
	C3dCoord	anchor;
	double		subx, suby;
	double		subu, subv;
	int			count, indx;
	ID			patid;

	count = mainOrder.Count();

	for (indx = 0; indx < count; ++indx)
	{
		dbCommand = dynamic_cast<CDbCommand*>( mainOrder[indx] );
		if (dbCommand != NULL && dbCommand->IsInstance())
		{
			patid = (ID) dbCommand->IntGet( "patid", 0 );
			dbCommand->Db()->Find( patid, (CDbEntity**) &dbPattern, DBPATTERN, DBPATTERN );

			subx = dbPattern->DoubleGet( STR_SUBX, UNDEFINED );
			suby = dbPattern->DoubleGet( STR_SUBY, UNDEFINED );

			subu = dbPattern->DoubleGet( STR_SUBU, UNDEFINED );
			subv = dbPattern->DoubleGet( STR_SUBV, UNDEFINED );

			anchor = dbCommand->Coord();

			anchor.X( anchor.X() + (subu - subx) );
			anchor.Y( anchor.Y() + (subv - suby) );

#if REQUIRED
			dbCommand->Init(
				dbCommand->Layer(),
				dbCommand->Workplane(),
				anchor,
				dbCommand->Diam(),
				dbCommand->Depth() );
#endif
		}
	}
#endif
}

CDbPattern*
CModelClfile::SubdefFind( const CDbEntity& dbEntity )
{
	CDbPattern*	dbPattern;
	CDbEntity*	owner;

	owner = dbEntity.Owner();
	while (1)
	{
		if (owner == NULL)
			break;

		if (owner->Type() == DBPATTERN)
		{
			dbPattern = dynamic_cast<CDbPattern*>(owner);
			break;
		}

		owner = owner->Owner();
	}

	return dbPattern;
}
void
CModelClfile::CutOrderAppend(
						CDbFeature* topLevelFeature,
						CDbEntityArray* dbEntities,
						CDbEntityArray* cutOrder )
{
	if (topLevelFeature != NULL)
	{
		// Allow UserCommandsProcess() to flag zone information so that
		// SequenceInit() can put entities into the correct sequence object.
		cutOrder->Append( topLevelFeature );
	}

	if (dbEntities != NULL)
	{
		int count = dbEntities->Count();
		for (int indx = 0; indx < count; ++indx)
		{
			cutOrder->Append( (*dbEntities)[indx] );
		}
		dbEntities->BenignFlush();
	}

	if (topLevelFeature != NULL)
	{
		// Allow UserCommandsProcess() to output such things as hold-down
		// and repositioning events.  These events must occur before we
		// can processes the next work-zone.
		cutOrder->Append( topLevelFeature );
	}
}

// clfileAttribs is used to track the 'clfile state' of the attributes.
// NOTE: The variables must be copied as strings!  The reason for
// this is too complicated to explain here, but prior to this change,
// when dealing with 'mulitple tool drops', the clfileAttribs
// 'station' attribute was an integer variable, as it is on
// tool entities.
void
CModelClfile::AttribsCopyAppend( const CVarList* from, CVarList* to ) const
{
	if ((from == NULL) || (to == NULL))
		return;

	int count = from->countVar();
	for (int indx = 0; indx < count; ++indx)
	{
		CVar* var = from->getVar( indx );

		const CString& name = var->getName();
		CString value = var->getString();

		to->setString( name, value );
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE: The attribute "_expseq" (ie. explicit sequence number) was
// introduced to support Modular Service's Cincinatti laser.  In this
// application, there is sometimes a need to explicitly sequence the
// cutting entities within a part.  This is done to prevent to the tool
// from crossing an area which has already been cut (which would otherwise
// cause the laser head to collide with an already cut area).
//
// The _expseq attribute is attached to the entities via the Java macro
// ExplicitSequence.java.  This macro must be executed after the user
// manually sequences entities using the Sequence/Sequence_Order menu
// functionality.  Though the results of Sequence_Order are persistent,
// they are not preserved by Nesting.  The _expseq attribute is preserved
// by Nesting, however.
//
// In the event that ExplicitSequence.java is lost, it may be resurrected
// by restoring the Java code seen at the bottom of this file.
//
bool
CModelClfile::IsExplicitlySequenced( const CDbEntityArray& workPkgEntities )
{
	CDbEntity*	dbEntity = workPkgEntities[0];
	int			expseq = dbEntity->IntGet( "_expseq", -1 );

	return (expseq >= 0);
}

void
CModelClfile::CopyAndSort( const CDbEntityArray& workPkgEntities, CDbEntityArray* optimizedEntities )
{
	CDynamicArray<CDbEntity*>	tmp;
	int		count, indx;

	count = workPkgEntities.Count();
	for (indx = 0; indx < count; ++indx)
	{
		tmp.Append( workPkgEntities[indx] );
	}

	tmp.Qsort( &QSortExpseqCompare );

	for (indx = 0; indx < count; ++indx)
	{
		optimizedEntities->Append( tmp[indx] );
	}
}

void
CModelClfile::Dump( const CString& path )
{
	ClfileRec*			rec;
	const CDbEntity*	dbEntity;
	FILE*				f;
	int					count, indx;

	f = fopen( path, "w" );
	if (f != NULL)
	{
		count = m_clfile.Count();
		for (indx = 0; indx < count; ++indx)
		{
			rec = m_clfile[indx];

			dbEntity = rec->Entity();

			fprintf( f, "%d <%s>\n", rec->RecType(), ((dbEntity == NULL) ? "null" : dbEntity->Name()) );
		}

		fclose(f);
	}
}

// static
int
CModelClfile::QSortExpseqCompare( const void* ptrA, const void* ptrB )
{
	CDbEntity* entityA = (*(CDbEntity**) ptrA);
	CDbEntity* entityB = (*(CDbEntity**) ptrB);

	int	expseqA = entityA->IntGet( "_expseq", -1 );
	int	expseqB = entityB->IntGet( "_expseq", -1 );

	return (expseqA - expseqB);
}


/*

import Weng.System.*;
import Weng.Access.*;
import Weng.Math.*;
import Weng.Geometry.*;
import Weng.Modeler.*;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Establishes the cutting sequence of a part.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Proper use of this macro requires the following:
//
// 1. You must first manually sequence the cutting entities using the
//    Sequence/Sequence_Order menu feature.
//
// 2. Run this macro after step 1
//
// The macro the attaches the "_expseq" attribute to all cutting
// entities in the part based upon the sequence order of each entity.
// CModelClfile::OptiCutOrder() then takes special action if it
// encounters a "_expseq" attribute.
//
public class ExplicitSequence implements WengMacro
{
	static private final boolean DEBUG = false; // Set to "true" to test!

	public void main()
	{
		DbEntity	dbSequence;
		int			count, indx;
		
		try
		{
			Selector.Restrictions( true );
			Selector.All( false );
			Selector.Sequence( true );
			Selector.AddAll();
			
			count = Selector.Count();
			for (indx = 0; indx < count; ++indx)
			{
				dbSequence = Selector.Get(indx);
				if ( dbSequence.Name().equalsIgnoreCase("WorkPkg1") )
				{
					SequenceNumbersAssign( dbSequence );
					break;
				}
			}
			
		}
		catch( Exception e )
		{
			ExceptionPrinter.StackTracePrint( e );
		}
	}

	private void SequenceNumbersAssign( DbEntity dbSequence )
	{
		DbEntity	dbEntity;
		String		msg;
		int			seqnum;
		
		msg = DbSeqIterator.Init( dbSequence.Id() );
		
		if (msg == null)
		{
			seqnum = 0;
			
			while (true)
			{
				dbEntity = DbSeqIterator.Get();
				if (dbEntity == null)
					break;
				
				dbEntity.IntSet( "_expseq", seqnum );
				++seqnum;
				
				DbSeqIterator.Next();
			}
		}
		else
		{
			Msg.Display( msg );
		}
	}
}
*/

// ============================================================================
//		dogleg
//
//	For cut avoidance (Modular), the last curve in a profile may have "dogleg"
// attributes attached to it.  These indicate rapid waypoints that must be traversed
// to avoid already-cut geometry.
//
CReturn
CModelClfile::dogleg(
	const CDbEntity* db_ent )
{
	ClfileRec* rec;
	CReturn status;
	CString name;

	const CVarList& attrib = db_ent->Attrib();
	int num = attrib.getInt( "_dog_num", 0 );

	if (num > 0)
	{
		for (int idx=1; idx<=num; idx++)
		{
			name.Format( "_dog_x%d", idx);
			double dx = attrib.getReal( name, 0.0 );
			name.Format( "_dog_y%d", idx);
			double dy = attrib.getReal( name, 0.0 );

			C3dCoord end(dx, dy, 0.0);	// Put in a real Z value?
			//
			//
			CEntityDb* db = ((CDbEntity*)db_ent)->Db();
			CDbPoint* pt;
			db->Create(DBPOINT, (CDbEntity**)&pt );
			pt->Init(db_ent->Tool(), db_ent->Workplane(), end);

			status = RecAppend( EV_RAPID, &rec );
			rec->Elem( new CGeoPoint(end) );
			rec->Entity( pt );
			rec->Attrib( &(db_ent->Attrib()) );
		}
	}

	return status;
}

CDbPattern* CModelClfile::PatternGet( const CDbEntity* dbEntity )
{
	CDbPattern*	dbPattern = NULL;

	while (1)
	{
		CDbEntity* owner = dbEntity->Owner();
		if (owner == NULL)
			break;

		int pattern_id = owner->IntGet( "exp", 0 );
		if (pattern_id > 0)
		{
			m_model->EntityFind( pattern_id,
				(CDbEntity**) &dbPattern, DBPATTERN, DBPATTERN );
			break;
		}

		dbEntity = owner;
	}

	return dbPattern;
}

CDbFeature* CModelClfile::PartGet( const CDbEntity* dbEntity )
{
	CDbFeature*	dbFeature = NULL;

	while (1)
	{
		CDbEntity* owner = dbEntity->Owner();
		if (owner == NULL)
			break;

		CDbFeature* tmp = dynamic_cast<CDbFeature*>( owner );
		if ((tmp != NULL) && tmp->IsPart())
		{
			dbFeature = tmp;
			break;
		}

		dbEntity = owner;
	}

	return dbFeature;
}
