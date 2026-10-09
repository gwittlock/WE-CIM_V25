 
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
#include "CodeUtil.h"
#include "CiClfile.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CCiClfile::CCiClfile()

	: CClfile(),
	  m_autoModDb(),
	  m_model( NULL ),
	  m_currGlobalPos( NULL ),
	  m_currLocalPos( NULL ),
	  m_currTool( NULL ),
	  m_clfile()
{
}

CCiClfile::~CCiClfile()
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
			while (jndx < attribs->countVar())
			{
				// Remove all temporary attributes.  In particular, remove
				// 'user commands' that were copied from containers to
				// lower-level entities.
				//
				// SEE ALSO:
				//     CCiClfile::UserCommandsProcess()
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
}

int
CCiClfile::Count() const
{
	return m_clfile.Count();
}

int
CCiClfile::CurrRecNo() const
{
	return m_clfile.Curr();
}

const ClfileRec*
CCiClfile::Fetch( int recNo ) const
{
	ClfileRec* rec = m_clfile[ recNo ];
	return rec;
}

int
CCiClfile::Read(
			long				recNo,
			long*				recInfo,
			long*				intArray,
			long				intArraySize,
			double*				dblArray,
			long				dblArraySize ) const
{
	if ( RecordOutOfRange( "CCiClfile::Read()", recNo ) )
		return -1;

	// Provide for 'logical const'
	CCiClfile* cast = ((CCiClfile*) this);

	ClfileRec* rec = m_clfile[ recNo ];

	const CDbEntity* dbEntity = rec->Entity();

	recInfo[0] = rec->RecType();
	// recInfo[1] = ((dbEntity == NULL) ? 0 : dbEntity->Id());
	recInfo[1] = ((dbEntity == NULL) ? 0 : dbEntity->IntGet( "_id", 0 ));

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

void
CCiClfile::DataTransfer( const ClfileRec& rec,
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
CCiClfile::Init( CModel* model, bool code_bottom )
{
	CReturn			status;
	CDbEntityArray	mainOrder;

	m_model = model;

	m_currGlobalPos = NULL;
	m_currLocalPos = NULL;
	m_currTool = NULL;
	m_clfile.DestructiveFlush();

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	status.Diagnostic("CCiClfile::CutOrderGenerate()");
	status = CutOrderGenerate( &mainOrder, code_bottom );

	status = MainBegin();

	status.Diagnostic("CCiClfile::ProgramStart()");
	if ( status.IsOk() )
		status = ProgramStart();

	status.Diagnostic("CCiClfile::ProgramBody()");
	if ( status.IsOk() )
		status = ProgramBody( mainOrder );

	status.Diagnostic("CCiClfile::ProgramEnd()");
	if ( status.IsOk() )
		status = ProgramEnd();

	if ( status.IsOk() )
		status = MainEnd();

	return status;
}

CReturn
CCiClfile::CutOrderGenerate( CDbEntityArray* mainOrder, bool code_bottom )
{
	CReturn	status;

	CDbEntityArray	temp;
	CDbIterator		iter;
	CDbProfile*		dbProfile;
	int				icnt, indx;
	int				jcnt, jndx;
	ID				tool_id;
	int				sncs;
	bool			okay;

	CDbEntity*	dbEntity;

	iter.Init( m_model->Db(), DBLINE );
	while (1)
	{
		dbEntity = iter();
		if (dbEntity == NULL || dbEntity->Type() > DBPROFILE)
			break;

		tool_id = dbEntity->IntGet( "_tool_id", 0 );
		if (tool_id > 0)
		{
			okay = true;

			if (dbEntity->Type() == DBHOLE)
				okay = (dbEntity->IntGet( "_seqnum", IUNDEFINED ) < IUNDEFINED);

			if (okay)
			{
				sncs =  dbEntity->IntGet( "_sncs", 0 );
				if (code_bottom == true)
					okay = ((sncs / 10000) == 3);
				else
					okay = ((sncs / 10000) != 3);
			}

			if (okay)
				temp.Append( dbEntity );
		}

		iter.Next();
	}

	temp.Qsort( CCiClfile::QSortExpseqCompare );

	icnt = temp.Count();
	for (indx = 0; indx < icnt; ++indx)
	{
		dbEntity = temp[indx];
		if (dbEntity->Type() == DBPROFILE)
		{
			dbProfile = (CDbProfile*) dbEntity;

			jcnt = dbProfile->Count();
			for (jndx = 0; jndx < jcnt; ++jndx)
			{
				dbEntity = (*dbProfile)[jndx]; 
				mainOrder->Append( dbEntity );
			}
		}
		else
		{
			mainOrder->Append( dbEntity );
		}
	}

	return status;
}

// Access to the header variables was moved here from ProgramStart()
// because attributes such as user-defined code generator variables
// must be accessible before the first clfile record is read.  This
// was discovered when someone tried to write a code generator whose
// output format and units were variable.
CReturn
CCiClfile::MainBegin()
{
	ClfileRec* rec;

	(* (pAttrib()) ) = m_model->Header();

	CReturn status = RecAppend( EV_MAIN_BEGIN, &rec );
	return status;
}

CReturn
CCiClfile::MainEnd()
{
	ClfileRec* rec;
	CReturn status = RecAppend( EV_MAIN_END, &rec );
	return status;
}

// TODO: Any data contained in StartProgram (currently empty)?
CReturn
CCiClfile::ProgramStart()
{
	ClfileRec* rec;
	CReturn status = RecAppend( EV_START_PROGRAM_BEGIN, &rec );

	if ( status.IsOk() )
		status = RecAppend( EV_START_PROGRAM_END, &rec );

	return status;
}

CReturn
CCiClfile::ProgramEnd()
{
	ClfileRec* rec;
	CReturn status = RecAppend( EV_END_PROGRAM_BEGIN, &rec );

	if ( status.IsOk() )
		status = RecAppend( EV_END_PROGRAM_END, &rec );

	return status;
}

CReturn
CCiClfile::ProgramBody( const CDbEntityArray& mainOrder )
{
	return ( CutOrderProcess( mainOrder ) );
}

CReturn
CCiClfile::CutOrderProcess( const CDbEntityArray& cutOrder )
{
	CReturn		status;
	C3dCoord	pt;
	int			count, indx;

	count = cutOrder.Count();
	for (indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = cutOrder[ indx ];

		//if (dbEntity->Tool() != NULL)
		{
			status = Transition( dbEntity );
			if ( !status.IsOk() )
				break;
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
	}

	return status;
}

CReturn
CCiClfile::Point( const CDbPoint* dbPoint )
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
CCiClfile::Line( const CDbLine* dbLine )
{
	ClfileRec* rec;
	CReturn status;

	if ( status.IsOk() )
	{
		CGeoLine* geoLine = dbLine->Line();

		status = RecAppend( EV_LINE, &rec );

		rec->Elem( geoLine );
		rec->Entity( dbLine );
		rec->Attrib( &(dbLine->Attrib()) );

		CurrPosUpdate( dbLine->EndPt(0), dbLine->EndPt() );
	}

	return status;
}

CReturn
CCiClfile::Arc( const CDbArc* dbArc )
{
	ClfileRec* rec;
	CReturn status;

	if ( status.IsOk() )
	{
		CGeoArc* geoArc = dbArc->Arc();

		int evType = ((dbArc->Dir() < 0) ? EV_CW_ARC : EV_CCW_ARC);

		status = RecAppend( evType, &rec );

		rec->Elem( dbArc->Arc() );
		rec->Entity( dbArc );
		rec->Attrib( &(dbArc->Attrib()) );

		CurrPosUpdate( dbArc->EndPt(0), dbArc->EndPt() );
	}


	return status;
}

CReturn
CCiClfile::Hole( const CDbHole* dbHole )
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
CCiClfile::Command( const CDbCommand* dbCommand )
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
CCiClfile::UserCommandsProcess( CDbEntity* dbEntity )
{
	CReturn		status;
	CVarList	emptyList;
	ClfileRec*	rec;
	double		xhold, yhold;
	double		xrepo, yrepo;
	double		delta;
	int			dropstop, processed;

	const CVarList& attribs = dbEntity->Attrib();

	int count = attribs.countVar();
	for (int indx = 0; indx < count; ++indx)
	{
		CVar* attrib = attribs.getVar( indx );

		if (attrib->getName().GetAt(0) > '_')
		{
			// Since the attributes are lexically ordered in
			// increasing order, and because 'system' attributes
			// start with an underscore, we can terminate the
			// loop as soon as we find an attribute whose name
			// does not start with an underscore.
			break;
		}

		EDbEntityType type = dbEntity->Type();

		if (type == DBFEATURE && ((CDbFeature*) dbEntity)->IsWorkZone())
		{
			processed = dbEntity->IntGet( "~processed", 0 );

			if ( processed )
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

				xrepo += delta;
				m_model->pHeader()->setReal( "_zone_repo", xrepo );

				yrepo = ((m_currLocalPos == NULL) ? UNDEFINED : m_currLocalPos->Y());

				// BugID: 652 -- a zero repo distance should not produce a
				// reposition event, but we do want to allow a negative
				// reposition (some people manually enter a negative value).
				if ((fabs(delta) > SMALL && delta < UNDEFINED) && yrepo < UNDEFINED)
				{
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
CCiClfile::ToolChange( const CDbEntity* dbEntity )
{
	CReturn status;
	bool	okay;

	switch (dbEntity->Type())
	{
	case DBHOLE:
		okay = (dbEntity->StringGet("stations","").GetLength() <= 0);
		break;

	case DBCOMMAND:
		okay = ((CDbCommand*) dbEntity)->IsTooledText();
		break;

	default:
		okay = (dbEntity->Type() <= DBHOLE);
		break;
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
CCiClfile::Transition( const CDbEntity* next )
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

		// Signal a retract.
		status = RecAppend( EV_RAPID, &rec );
		if ( status.IsOk() )
		{
			rec->Elem( new CGeoPoint( (*m_currLocalPos) ) );
			rec->Entity( next );
			rec->Attrib( &(next->Attrib()) );
		}

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
CCiClfile::CurrPosUpdate( const C3dCoord& globalPos, const C3dCoord& localPos )
{
	if (m_currGlobalPos == NULL)
		m_currGlobalPos = new C3dCoord();

	if (m_currLocalPos == NULL)
		m_currLocalPos = new C3dCoord();

	(*m_currGlobalPos) = globalPos;
	(*m_currLocalPos)  = localPos;
}

CReturn
CCiClfile::RecAppend( int recType, ClfileRec** rec )
{
	CReturn status;

	ClfileRec* tmp = new ClfileRec( recType );

	if (tmp == NULL)
		status.Fatal( IDS_MEM_ALLOC_FAILURE, "CCiClfile::RecAppend(#1)" );
	else
		m_clfile.Append( tmp );

	(*rec) = (( status.IsOk() ) ? tmp : NULL );

	return status;
}

CReturn
CCiClfile::RecInsertBefore( int indx, int recType, ClfileRec** rec )
{
	CReturn status;

	ClfileRec* tmp = new ClfileRec( recType );

	if (tmp == NULL)
		status.Fatal( IDS_MEM_ALLOC_FAILURE, "CCiClfile::RecInsertBefore(#1)" );
	else
		m_clfile.InsertBefore( indx, tmp );

	(*rec) = (( status.IsOk() ) ? tmp : NULL );

	return status;
}

CReturn
CCiClfile::RecInsertAfter( int indx, int recType, ClfileRec** rec )
{
	CReturn status;

	ClfileRec* tmp = new ClfileRec( recType );

	if (tmp == NULL)
		status.Fatal( IDS_MEM_ALLOC_FAILURE, "CCiClfile::RecInsertAfter(#1)" );
	else
		m_clfile.InsertAfter( indx, tmp );

	(*rec) = (( status.IsOk() ) ? tmp : NULL );

	return status;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

int
CCiClfile::RecordOutOfRange( const char* func, int recNo ) const
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
CCiClfile::TagOutOfRange( const char* func, int tag, int upperBound ) const
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

void
CCiClfile::CutOrderAppend(
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
CCiClfile::AttribsCopyAppend( const CVarList* from, CVarList* to ) const
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

void
CCiClfile::Dump( const CString& path )
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
CCiClfile::QSortExpseqCompare( const void* ptrA, const void* ptrB )
{
	CDbEntity* entityA = (*(CDbEntity**) ptrA);
	CDbEntity* entityB = (*(CDbEntity**) ptrB);

	int	expseqA = entityA->IntGet( "_seqnum", -1 );
	int	expseqB = entityB->IntGet( "_seqnum", -1 );

	return (expseqA - expseqB);
}


