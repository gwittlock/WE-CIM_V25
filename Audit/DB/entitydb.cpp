
#include "stdafx.h"
#include "cmn_resource.h"
#include "DbAllEntities.h"
#include "Undo.h"
#include "DbIterator.h"
#include "EntityDb.h"
#include "MathConst.h"
#include "StringConst.h"

static int IdCompareFunc( const void* myId, const void* arrayItem );
static int NameCompareFunc( const void* myName, const void* arrayItem );

int EntityDbIDCompare( const void* ptrA, const void* ptrB )
{
	CDbEntity* entityA = (*(CDbEntity**) ptrA);
	CDbEntity* entityB = (*(CDbEntity**) ptrB);

	ID idA = entityA->Id();
	ID idB = entityB->Id();

	return (idA - idB);
}


////////////////////////////////////////////////////////////////////////

CEntityDb::CEntityDb()
	: m_undo( NULL ),
	  m_nextId( 1 ),
//	  m_color( 1 ),
	  m_insert_pos( 1 )
{
	m_undo = new CUndo( this );
}

CEntityDb::~CEntityDb()
{
	Flush();

	delete m_undo;
}

CReturn
CEntityDb::Flush()
{
	CReturn status;

	m_undo->Flush();

	m_entityNames.DestructiveFlush();

	for (int indx = DBWORKPLANE; indx < DBTERMINAL; ++indx)
	{
		EDbEntityType type = (EDbEntityType) indx;

		int count = m_list[ type ].Count();

		for (int jndx = 0; jndx < count; ++jndx)
		{
			CDbEntity* dbEntity = m_list[ type ][ jndx ];

			// Since we're really done with this entity, we don't
			// care anymore about the integrity of the reference counts.
			dbEntity->ReferenceFlagClear();
		}

		m_list[ type ].DestructiveFlush();
	}

	m_nextId = 1;

	m_insert_pos = 1;

	m_defvar.Reset();

	m_reference_map.RemoveAll();

	return status;
}

int
CEntityDb::Count() const
{
	int total = 0;

	for (int tndx = DBWORKPLANE; tndx < DBTERMINAL; ++tndx)
	{
		EDbEntityType type = (EDbEntityType) tndx;

		total += Count( type );
	}

	return total;
}

int
CEntityDb::Count( EDbEntityType type ) const
{
	int total = 0;
	int count = m_list[ type ].Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = m_list[ type ][ indx ];

		if ( !dbEntity->IsDeleted() )
			++total;
	}

	return total;
}

// Obtain a pointer to the dbEntity having the given id.
// TODO:  Cache the entity pointer wrt. the ID, to speed repeat calls
CReturn
CEntityDb::Find(
				ID id,
				CDbEntity** dbEntity,
				EDbEntityType startType,
				EDbEntityType endType ) const
{
	CReturn status;

	(*dbEntity) = NULL;

	if (endType == DBTERMINAL)
		endType = (EDbEntityType) (DBTERMINAL - 1);

	for (int indx = startType; indx <= endType; ++indx)
	{
		EDbEntityType type = (EDbEntityType) indx;

		int jndx = EntityIndex( type, id );

		if (jndx >= 0)
		{
			CDbEntity* temp = m_list[ type ][ jndx ];

			(*dbEntity) = ((temp->IsDeleted()) ? NULL : temp);

			break;
		}
	}

	if ((*dbEntity) == NULL)
	{
		// Hmmm... finding an entity is one way of determining if it exists...
		// ... don't want a message for that here
		status.setStatus( STATUS_ERROR );
	}

	return status;
}

// Obtain a pointer to the dbEntity having the given name.
// TODO:  Cache the entity pointer wrt. the name, to speed repeat calls
CReturn
CEntityDb::Find(
				const CString& name,
				CDbEntity** dbEntity,
				EDbEntityType startType,
				EDbEntityType endType ) const
{
	CReturn status;

	(*dbEntity) = NULL;

	int typeA = (int) startType;
	int typeB = ((endType == DBTERMINAL) ? (int) endType : (int) endType + 1);

	for (int indx = typeA; indx < typeB; ++indx)
	{
		EDbEntityType type = (EDbEntityType) indx;

		int count = m_list[ type ].Count();

		for (int jndx = 0; jndx < count; ++jndx)
		{
			CDbEntity* temp = m_list[ type ][ jndx ];

			if ( !temp->IsDeleted() )
			{
				CString entityName = temp->Name();

				//	Case insensitivity is easier to manage.
				if ( !entityName.CompareNoCase( name ) )
				{
					(*dbEntity) = temp;
					return status;
				}
			}
		}
	}

	if ((*dbEntity) == NULL)
	{
		// Hmmm... finding an entity is one way of determining if it exists...
		// ... don't want a message for that here
		status.setStatus( STATUS_ERROR );
	}

	return status;
}


// Create an uninitialized dbEntity in the database.
// As this is a factory method, you must cast the
// dbEntity to the appropriate subclass, and then
// call one of its Init() methods.
CReturn
CEntityDb::Create( EDbEntityType type, CDbEntity** dbEntity )
{
	CReturn status;

	CDbEntity* temp = NULL;

	switch (type)
	{
	case DBLAYER:
#ifdef LAYER  // pre-Version 16.0
		temp = (CDbEntity*) new CDbLayer( this );
#else
		type = DBTOOL;
		temp = (CDbEntity*) new CDbTool( this );
#endif
		break;

	case DBWORKPLANE:
		temp = (CDbEntity*) new CDbWorkplane( this );
		break;

	case DBTOOL:
		// Nothing to do.  At first implementation, the tool is
		// an empty container that has attributes attached to it.
		temp = (CDbEntity*) new CDbTool( this );
		break;

	case DBPOINT:
		temp = (CDbEntity*) new CDbPoint( this );
		break;

	case DBLINE:
		temp = (CDbEntity*) new CDbLine( this );
		break;

	case DBARC:
		temp = (CDbEntity*) new CDbArc( this );
		break;

	case DBHOLE:
		temp = (CDbEntity*) new CDbHole( this );
		break;

	case DBPROFILE:
		temp = (CDbEntity*) new CDbProfile( this );
		break;

	case DBCOMMAND:
		temp = (CDbEntity*) new CDbCommand( this );
		break;

	case DBFEATURE:
		temp = (CDbEntity*) new CDbFeature( this );
		break;

	case DBSEQUENCE:
		temp = (CDbEntity*) new CDbSequence( this );
		break;

	case DBPATTERN:
		temp = (CDbPattern*) new CDbPattern( this );
		break;

	default:
		ASSERT( FALSE );  // Invalid case.
	}

	temp->Id( m_nextId );

	++m_nextId;

	m_list[ type ].Append( temp );

	(*dbEntity) = temp;

	return status;
}

CDbEntity*
CEntityDb::GeoConvert(
					const CGeoElem&	geoElem,
					CDbTool*		dbTool,
					CDbWorkplane*	dbWork )
{
	CReturn status;

	CDbEntity* dbEntity = NULL;

	if (geoElem.Type() == GEOPOINT)
	{
		// NOTE: See also: CAutoPuncher::AutoPunch()
		//
		// Currently, this branch is executed only by fab during AutoPunch()ing.
		// As such, the center of the hole should be located at Z0 and the hole
		// depth should be at thickness below the top of the material, regardless
		// of the Z of the profile that generated this hole.
		//
		// The hole diameter is defined here as zero because we
		// want to see only the tool shape, and not a round hole.
		//
		CDbHole* dbHole;
		status = Create( DBHOLE, (CDbEntity**) &dbHole );

		if ( status.IsOk() )
		{
			C3dCoord pc( geoElem.EndPt().X(), geoElem.EndPt().Y(), 0.0 );
			double depth = fabs( geoElem.EndPt().Z() );

			dbHole->Init( dbTool, dbWork, pc, 0.0, depth );
			dbEntity = dbHole;
		}
	}
	else if (geoElem.Type() == GEOLINE)
	{
		CGeoLine&	geoLine = (CGeoLine&) geoElem;
		double		len = geoLine.Length2d();

		if (len >= SMALL)
		{
			CDbLine* dbLine;
			status = Create( DBLINE, (CDbEntity**) &dbLine );

			if ( status.IsOk() )
				status = dbLine->Init( dbTool, dbWork, geoLine );

			if ( status.IsOk() )
				dbEntity = dbLine;
		}
	}
	else if (geoElem.Type() == GEOARC)
	{
		CGeoArc&	geoArc = (CGeoArc&) geoElem;
		double		len = geoArc.Length2d();

		if (len >= SMALL)
		{
			CDbArc* dbArc;
			status = Create( DBARC, (CDbEntity**) &dbArc );

			if ( status.IsOk() )
				status = dbArc->Init( dbTool, dbWork, geoArc );

			if ( status.IsOk() )
				dbEntity = dbArc;
		}
	}
	else
	{
		ASSERT( FALSE );
	}

	return dbEntity;
}

// ------------------------------------------------------------------
//		PrepareCopy
//		Copy
//
//	Copying a section of the database is an involved process -- it
//	requires the copied entities to have the same relationships to each
//	other as the masters.  For example, given three points and two lines,
//	with one of the points shared between the lines.  So, five entities
//	are to be copied.
//	Copy the three points -- easy, their contents are duplicated.
// But now copy the two lines... how do you copy a line?  Duplicate it's
//	end points!  But those points have already been duplicated.  To find
//	this out, the line looks into the reference map to see if it's points
//	have already been duplicated.  They have, so it duplicates it's common
//	stuff and then points to the new points.  This system would ALSO work
//	if the points were note explicitly duplicated... the line might find
//	no map, so it makes new points, adds them to the database and map, and
//	references them.
//
//	This technique should also work for profiles, groups... anything that
//	wants to keep reference relationships consistent in the copied set.
//
//	Before calling Copy on a batch of entities... you MUST call PrepareCopy()
//	to clear out the reference map, otherwise sick and unholy things may
//	occur.  The Copy system is not recursive, and should be used by one
//	system at a time.
//
CReturn CEntityDb::PrepareCopy( CEntityDb* io_dest )
{
	m_reference_map.RemoveAll();
	m_dest_db = io_dest;

	return CReturn( STATUS_OKAY );
}

CReturn CEntityDb::Copy( CDbEntity& in_entity, CDbEntity** dbEntity ) 
{
	CReturn status = in_entity.CopyTo( m_dest_db, &m_reference_map, dbEntity );

	return status;
}

bool CEntityDb::WasCopied( const CDbEntity& dbEntity ) const
{
	CDbEntity* result;

	return (m_reference_map.Lookup( (CDbEntity*)&dbEntity, result) != 0);
}

// Mark the given dbEntity as being deleted.
CReturn
CEntityDb::Delete( CDbEntity** dbEntity )
{
	CReturn status;

	EDbEntityType type = (*dbEntity)->Type();
	int count = m_list[ type ].Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* temp = m_list[ type ][ indx ];

		if (temp == (*dbEntity))
		{
			(*dbEntity)->Delete();
			break;
		}
	}

	(*dbEntity) = NULL;

	return status;
}

void CEntityDb::IdMaxInit()
{
	CDbIterator iter;

	m_nextId = 0;

	iter.Init( (*this), DBWORKPLANE );
	while (1)
	{
		const CDbEntity* dbEntity = iter();

		if (dbEntity == NULL)
			break;

		ID id = dbEntity->Id();

		if (id > m_nextId)
			m_nextId = id;

		iter.Next();
	}

	++m_nextId;
}

void
CEntityDb::DirtyEntities( CDbEntityList* list )
{
	// We are really only concerned with whether
	// any toolpath will have to be regenerated.

	CDbEntity* dbEntity;
	CDbEntity* listEntity;
	EDbEntityType type;
	int count, indx, jndx, tndx;

	// Build the list of base level entities that
	// have been modified.  We will subsequently
	// determine if these entities propogate their
	// dirt up to any toolpath.
	for (tndx = DBTOOL; tndx <= DBFEATURE; ++ tndx)
	{
		type = (EDbEntityType) tndx;

		count = m_list[ type ].Count();

		for (indx = 0; indx < count; ++indx)
		{
			CDbEntity* dbEntity = m_list[ type ][ indx ];

			if ( dbEntity->IsDeleted() )
				continue;  // Ignore this entity.

			if ( dbEntity->IsDirty() )
				list->Append( dbEntity );
		}
	}

	// TODO: Dirty DBTOOLs will might have to be managed
	// specially, depending on how they are referenced by
	// DBTOOLPATH entities.
	for (indx = 0; indx < list->Count(); ++ indx)
	{
		listEntity = (*list)[ indx ];

		for (tndx = (listEntity->Type() + 1); tndx <= DBFEATURE; ++tndx)
		{
			type = (EDbEntityType) tndx;

			count = m_list[ type ].Count();

			for (jndx = 0; jndx < count; ++jndx)
			{
				dbEntity = m_list[ type ][ jndx ];

				if ( dbEntity->IsDirty() )
					continue;  // The dbEntity will already be in the list.

				if ( dbEntity->HasRefTo( listEntity ) )
				{
					list->Append( dbEntity );
				}
			}
		}
	}
}

void
CEntityDb::ClearDirty()
{
	CDbIterator iter;

	iter.Init( (*this), DBWORKPLANE );
	while (1)
	{
		CDbEntity* dbEntity = iter();
		if (dbEntity == NULL)
			break;

		dbEntity->CreateFlag( false );
		dbEntity->ModifyFlag( false );

		iter.Next();
	}
}

// NOTE: Created specifically for macros.
// This method needs to behave like ::Count( EDbEntityType ).
// That is, it must avoid processing deleted entities.
CDbEntity*
CEntityDb::Get( EDbEntityType type, int indx )
{
	CDbEntity* theEntity = NULL;

	int ub = m_list[ type ].Count();
	int count = 0;
	for (int jndx = 0; jndx < ub; ++jndx)
	{
		CDbEntity* dbEntity = m_list[ type ][ jndx ];

		if ( !dbEntity->IsDeleted() )
		{
			if (count == indx)
			{
				theEntity = dbEntity;
				break;
			}

			++count;
		}
	}

	return theEntity;
}

// For forward migration of pre-V16 MM2 files.
// DbLayers are now superceded by DbTools and are
// implemented as DbTools.  As such, when reading
// a pre-V16 MM2 file, the entity ids are not likely
// to be arranged in increasing order. 
void
CEntityDb::SortByID( EDbEntityType type )
{
	m_list[type].Qsort( &EntityDbIDCompare );
}

static bool IsGapTool( const CDbEntity* dbEntity )
{
	return ((dbEntity->Type() == DBTOOL) && ((CDbTool*) dbEntity)->IsGapTool());
}

void
CEntityDb::RemoveRefs( CDbEntity* dbEntity )
{
	// TODO: Restructure as necessary to introduce optimization.
	// At first implementation, this method is not very efficient.
	// It can be optimized by restricting the search for references
	// to those classes of objects that might have references.  For
	// instance, a workplane will never reference anything, however,
	// most everything else references a workplane.  It would be
	// more OO to have each class of entity control the parameters
	// of searching.

	EDbEntityType startType;
	int count, indx, tndx;

	EDbEntityType type = dbEntity->Type();

	if ((type == DBWORKPLANE || type == DBTOOL) && dbEntity->IsRefd())
	{
		// As the 'rug', these entities can not be
		// delete 'out from under' other entities.
		// TODO: Avoid the assertion when running nesting because there
		// is (apparently) a case where the assertion is not relevant.
#if DISABLED_2017_06_24
		if ( !IsGapTool( dbEntity ) )
			ASSERT( FALSE );
#endif
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	if (type != DBTOOL && type != DBSEQUENCE)
	{
		// The remaining classes of entities all
		// reference both a workplane and a layer.

		for (tndx = DBWORKPLANE; tndx <= DBWORKPLANE; ++tndx)
		{
			type = (EDbEntityType) tndx;

			count = m_list[ type ].Count();

			for (indx = 0; indx < count; ++indx)
			{
				CDbEntity* thisEntity = m_list[ type ][ indx ];

				thisEntity->RemoveRef( dbEntity );
			}
		}
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	type = dbEntity->Type();

	switch (type)
	{
	case DBWORKPLANE:
		startType = DBTERMINAL;  // Nothing to do (at top of food chain).
		break;
	case DBTOOL:
		startType = DBFEATURE;
		break;
	case DBPOINT:
		startType = DBFEATURE;  // ASSUMPTION: user-defined point.
		break;
	case DBLINE:
	case DBARC:
		startType = DBPROFILE;
		break;
	case DBHOLE:
	case DBPROFILE:
		startType = DBFEATURE;
		break;
	case DBCOMMAND:
	case DBFEATURE:
//	case DBPATTERN: ??
		startType = DBFEATURE;
		break;
	case DBSEQUENCE:
		startType = DBTERMINAL;
		break;
	default:
		ASSERT( FALSE );  // Unknown type.
		break;
	}

	for (tndx = startType; tndx < DBTERMINAL; ++tndx)
	{
		type = (EDbEntityType) tndx;

		count = m_list[ type ].Count();

		for (indx = 0; indx < count; ++indx)
		{
			CDbEntity* thisEntity = m_list[ type ][ indx ];

			thisEntity->RemoveRef( dbEntity );
		}
	}
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Undo system management.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void
CEntityDb::UndoBufferActivate()
{
	m_undo->Activate();
}

void
CEntityDb::UndoBufferSuppress()
{
	m_undo->Suppress();
}

void
CEntityDb::UndoBufferFlush()
{
	m_undo->Flush();
}

void
CEntityDb::UndoBufferPrepare()
{
	m_undo->Open();
}

void
CEntityDb::UndoBufferCommit()
{
	m_undo->Close();
}

void
CEntityDb::UndoBufferRecord( CDbEntity* dbEntity )
{
	m_undo->Record( dbEntity );
}

CReturn
CEntityDb::Undo()
{
	CReturn status = m_undo->Undo();
	return status;
}

CReturn
CEntityDb::Redo()
{
	CReturn status = m_undo->Redo();
	return status;
}
int
CEntityDb::UndoBufferDepth() const
{
	return m_undo->Depth();
}

void
CEntityDb::UndoBufferDepth( int depth )
{
	m_undo->Depth( depth );
}

// Obtain a pointer to the dbEntity having the given id.
// While implementing changes for V16, it was discovered that
// CEntityDb::Find() was returning pointers to entities that
// were marked as deleted (which is a bad thing except when
// you are considering the undo system).  Hence, this method
// was introduced specifically for the undo system.
//
CReturn
CEntityDb::UndoFind(
				ID id,
				CDbEntity** dbEntity,
				EDbEntityType startType,
				EDbEntityType endType ) const
{
	CReturn			status;
	EDbEntityType	type;
	int				indx, jndx;

	(*dbEntity) = NULL;

	if (endType == DBTERMINAL)
		endType = (EDbEntityType) (DBTERMINAL - 1);

	for (indx = startType; indx <= endType; ++indx)
	{
		type = (EDbEntityType) indx;

		jndx = EntityIndex( type, id );

		if (jndx >= 0)
		{
			(*dbEntity) = m_list[ type ][ jndx ];
			break;
		}
	}

	if ((*dbEntity) == NULL)
	{
		status.setStatus( STATUS_ERROR );
	}

	return status;
}

CReturn
CEntityDb::Remove( const CDbEntity& dbEntity )
{
	CReturn status;

	EDbEntityType type = dbEntity.Type();
	ID id = dbEntity.Id();

	int indx = EntityIndex( type, id );

	ASSERT( (indx >= 0) );

	if (indx >= 0)
		m_list[ type ].Remove( indx );

	return status;
}

// NOTE: Entities will be in historical order (ie. increasing IDs)
int
CEntityDb::EntityIndex( EDbEntityType type, ID id ) const
{
	const CDbEntityArray* list = &m_list[type];

	int indx;
	bool found = list->BinarySearch( &id, &IdCompareFunc, &indx );

	return (found ? indx : -1);
}



// Check for the existence of a user-defined of system constant name.
int
CEntityDb::NameFind( const CString& name )
{
	int indx;
	bool found = m_entityNames.BinarySearch( (void*) &name, &NameCompareFunc, &indx );

	return (found ? indx : -1);
}

// Add a user-defined of system constant name.
// TODO: Improve efficiency?  We could incorporate a binary
// search with the insertion process.  The current implementation
// assumes that you have already called CEntityDb::NameFind().
int
CEntityDb::NameAdd( const CString& name )
{
	CString* newName = new CString( name );

	int count = m_entityNames.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CString* temp = m_entityNames[indx];

		if (name.CompareNoCase( (*temp) ) < 0)
		{
			m_entityNames.InsertBefore( indx, newName );
			return indx;
		}
	}

	return m_entityNames.Append( newName );
}

int
CEntityDb::NameRemove( const CString& name )
{
	int indx = NameFind( name );

	if (indx >= 0)
	{
		delete m_entityNames.Remove( indx );
	}

	return indx;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

int IdCompareFunc( const void* myId, const void* arrayItem )
{
	ID id = *((int*) myId);
	CDbEntity* dbEntity = ((CDbEntity*) arrayItem);

	ID entityId = dbEntity->Id();

	return (entityId - id);
}

int NameCompareFunc( const void* mtName, const void* arrayItem )
{
	CString* name = ((CString*) mtName);
	CString* entityName = ((CString*) arrayItem);

	return ( ISGN(name->CompareNoCase( (*entityName) )) );
}

// ==================================================================
//	DbToolMatch
//
//	Find a tool in the database that matches the provided tool,
//	using the *content* of the tools as criteria.
//
///	Returns the first matching tool.
CDbTool*
CEntityDb::DbToolMatch(
	const CDbTool&		src_tool )
{
	CDbIterator iter;
	int			high_score = 0;
	CDbTool*	high_tool = NULL;
	CDbTool*	test=NULL;

	CReturn ret;
	CString note;
	if (CReturn::Debug()>=5)
	{
		note.Format( "=== TOOL %d ===============", src_tool.IntGet( STR_NC_CODE_NUMBER, -1 ) );
		ret.Diagnostic(note);
	}

	iter.Init( (*this), DBTOOL );
	while (1)
	{
		test = dynamic_cast<CDbTool*>( iter() );
		if (test == NULL)
			break;
		iter.Next();

		int score = src_tool.Matches( *test );
		if ( score
			&& (score > high_score) )
		{
			high_score = score;
			high_tool = test;
		}
	}

if (CReturn::Debug()>=5)
{
if (!high_tool)
{ note.Format( "--- %d (%s) to null -------------", src_tool.IntGet( STR_NC_CODE_NUMBER, -1 ), src_tool.Name() ); }
else
{ note.Format( "--- %d (%s) to %d  (%s) ---------", src_tool.IntGet( STR_NC_CODE_NUMBER, -1 ), src_tool.Name(), high_tool->IntGet( STR_NC_CODE_NUMBER, -1 ), high_tool->Name() ); }
ret.Diagnostic(note);
}
	return high_tool;
}

// ==================================================================
// Introduced for Nesting, to 'create tool setups on-the-fly'.
// At first implementation, it was deemed sufficient (by Gary)
// to match tools by 'tool crib id'.
//
//	TODO:  Determine if anyone actually uses this!
CDbTool*
CEntityDb::ToolFindCreate( 
	const CDbTool&	dbTool,
	bool			create )
{
	int toolID = dbTool.IntGet( STR_TOOL_ID, IUNDEFINED );

	// First try to find a matching tool.
	CDbTool* result = DbToolMatch( dbTool );
	if (result == NULL)
	{
		CDbIterator iter;

		// Put a copy of the tool in the first empty station having the correct size.
		double reqdStationSize = dbTool.DoubleGet( STR_STATION_SIZE, 0.0 );

		iter.Init( (*this), DBTOOL );
		while (1)
		{
			CDbTool* candidate = dynamic_cast<CDbTool*>( iter() );
			if (candidate == NULL)
				break;
			iter.Next();

//			int type = candidate->IntGet( STR_TYPE_ID, IUNDEFINED );
//			if (type == TTYPE_OPEN )
			if (candidate->IsOpenStation())
			{
				// We've found an empty station.

				double stationSize = candidate->DoubleGet( STR_STATION_SIZE, 0. );
				if (fabs(reqdStationSize-stationSize) <= 1.e-3)
				{
					// Close enough for government work!
					result = candidate;
					break;
				}
			}
		}

		if ( create
			&& result )
		{
			// Copy the parameters of the reference tool, being careful
			// to retain the NC_Code_Number associated with the station.
			int ncCodeNum = result->IntGet( STR_NC_CODE_NUMBER, IUNDEFINED );
			(*(result->pAttrib())) = dbTool.Attrib();
			result->IntSet( STR_NC_CODE_NUMBER, ncCodeNum );
		}
	}

	return result;
}

