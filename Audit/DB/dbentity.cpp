
#include "stdafx.h"
#include "cmn_resource.h"
#include "StringConst.h"

#include "AppConst.h"

#include "Command.h"
#include "EntityDb.h"
#include "DbIterator.h"
#include "DbWorkplane.h"
#include "DbTool.h"
#include "DbCurve.h"
#include "DbHole.h"
#include "DbContainer.h"
#include "DbFeature.h"
#include "DbSequence.h"
#include "DbEntity.h"
#include "DisplayEntity.h"


////////////////////////////////////////////////////////////////////////

int CDbEntity::m_new_action = 0;
bool CDbEntity::m_flip_cutside = true;
bool CDbEntity::m_target_draw = TRUE;
bool CDbEntity::m_legend_draw = TRUE;
bool CDbEntity::m_handles_draw = TRUE;

DWORD CDbEntity::m_override_color = 0;
bool CDbEntity::m_use_override_color = false;

C3dVec ZERO_SHIFT(0,0,0);

////////////////////////////////////////////////////////////////////////

CDbEntity::CDbEntity()
	
	: m_db( NULL ),
	  m_id( 0 ),
	  m_name(),
	  m_flags( 0 ),
	  m_action(0),
	  m_workplane( NULL ),
	  m_tool( NULL ),
	  m_refcnt( 0 ),
	  m_owner( NULL ),
	  m_seq( NULL ),
	  m_attribs()
{
	CreateFlag( true );
	ReferenceFlagSet();
	SelectableFlag( true );

	// TODO:  Move the workid set and access functions, plus the cache set/get into methods
	m_box_workid = -2;
	m_action = -1;

	m_disp = NULL;
}

CDbEntity::CDbEntity( CEntityDb* db )
	
	: m_db( db ),
	  m_id( 0 ),
	  m_name(),
	  m_flags( 0 ),
	  m_action(0),
	  m_workplane( NULL ),
	  m_tool( NULL ),
	  m_refcnt( 0 ),
	  m_owner( NULL ),
	  m_seq( NULL ),
	  m_attribs()
{
	//
	// Other initialization
	//
	CreateFlag( true );
	ReferenceFlagSet();
	SelectableFlag( true );

	m_box_workid = -2;

	m_disp = NULL;
}

CDbEntity::CDbEntity( const CDbEntity& dbEntity )
	
	: m_db( dbEntity.m_db ),
	  m_id( dbEntity.m_id ),
	  m_name( dbEntity.m_name ),
	  m_flags( dbEntity.m_flags ),
	  m_action(0),
	  m_workplane( dbEntity.m_workplane ),
	  m_tool( dbEntity.m_tool ),
	  m_refcnt( dbEntity.m_refcnt ),
	  m_owner( dbEntity.m_owner ),
	  m_seq( dbEntity.m_seq ),
	  m_attribs( dbEntity.m_attribs )
{
	SelectFlag( false );

	m_box_workid = -2;

	m_disp = NULL;
}

CDbEntity::~CDbEntity(void)
{
	if ( CDbEntity::IsReferencing() )
	{
		ASSERT( (m_refcnt == 0) );
	}

	delete m_disp;

}

const CDbEntity&
CDbEntity::operator = ( const CDbEntity& dbEntity )
{
	CommonCopy( dbEntity );
	return (*this);
}

CDisplayEntity*
CDbEntity::DisplayEntityGet( bool* did_allocate ) const
{
	(*did_allocate) = (m_disp == NULL);
	if ( *did_allocate )
		((CDbEntity*) this)->m_disp = new CDisplayEntity( this );

	return m_disp;
}

CString
CDbEntity::Name() const
{
	if (m_name.GetLength() > 0)
		return m_name;

	return DerivedName();
}

CString
CDbEntity::SystemName() const
{
	return m_name;
}


bool
CDbEntity::Name( const CString& name )
{
	if ( !m_name.CompareNoCase( name ) )
		return TRUE;  // okay, nothing to do.

	if (name.GetLength() > 0)
	{
		if ( !NameCheck( name ) )
			return FALSE;  // the name is not lexically acceptable.

#if REQUIRED
		// Historical note: duplicate name checking was introduced to prevent
		// users (who write Java macros) from creating entities having duplicate
		// names.  Allowing duplicate names corrupts entity searches by name.
		//
		// V16 introduced named work-zones (eg. WorkZone1, WorkZone2, etc...)
		// Since the work-zone number indicates it processing order, we must
		// shuffle work-zone names.  This means there will be times when
		// two work-zones will temporarily have the same name.
		//
		// If the duplicate namee check needs to be enforced, then we
		// can introduce a method for supressing/activing the check.
		//
		if (m_db->NameFind( name ) >= 0)
			return FALSE;  // the name is already in use.
#endif
	}

	// Now that we've passed the integrity checks, we
	// know that a change will occur and therefore
	// must record the current state of this entity.

	Record();

	if (m_name.GetLength() > 0)
		m_db->NameRemove( m_name );  // Remove the existing user-defined name.

	if (name.GetLength() > 0)
		m_db->NameAdd( name );  // add the new name.

	m_name = name;

	return TRUE;
}


bool
CDbEntity::SystemName( const CString& name )
{
	if ( !m_name.CompareNoCase( name ) )
		return TRUE;  // okay, nothing to do.

	if (name.GetLength() > 0)
	{
		if ( !SystemNameCheck( name ) )
			return FALSE;  // the name is not lexically acceptable.

		if (m_db->NameFind( name ) >= 0)
			return FALSE;  // the name is already in use.
	}

	// Now that we've passed the integrity checks, we
	// know that a change will occur and therefore
	// must record the current state of this entity.

	Record();

	if (m_name.GetLength() > 0)
		m_db->NameRemove( m_name );  // Remove the existing user-defined name.

	if (name.GetLength() > 0)
		m_db->NameAdd( name );  // add the new name.

	m_name = name;

	return TRUE;
}

// Entity names must adhere to Java variable naming convention.
// Though the configuration manager should enforce these rules,
// this safety net patches a common problem.  Note that when
// an invalid name is provided, the entity will be created with
// a 'derived' name which will, in turn, cause failures in things
// like chaining of curves into profiles.
CString
CDbEntity::NameConvert( const CString& name )
{
	CString result;

	int len = name.GetLength();
	for (int indx = 0; indx < len; ++indx)
	{
		char ch = name[indx];
		if ( isalnum( ch ) )
			result += ch;
		else // if (result.GetLength() > 0)
			result += '_';

		// NOTE:  Allow preceding underbar, 25 Feb eww
	}

	return result;
}

bool
CDbEntity::NameCheck( const CString& name )
{
	int len = name.GetLength();

	// BEWARE: Prior to this change, entity names had to start
	// with an alpha character.  This was a requirement for the
	// following reasons 1) the use of '_' as the first character
	// is reserved by our sofware, and 2) when dumping the model
	// as a Java macro, the entity name must conform to the Java
	// variable naming convention (which excludes the first
	// character from being a numeric character).
	//
	// HOWEVER: Said restriction causes many file import functions
	// to fail because layer names (particularly DXF and DWG) often
	// start with a numeric character (eg. 5MMFACE_15).
	//
	// DECISION: We will allow entity names to start with numeric
	// characters, but Mr.Customer will have to edit his Java macro.
	//
	//     bool okay = (len > 0 && isalpha( name[0] ));
	bool okay = (len > 0 && isalnum( name[0] ));

	if ( okay )
	{
		for( int indx = 1; indx < len; ++ indx)
		{
			char ch = name[indx];

			okay = (ch == '_' || isalnum( ch ));

			if ( !okay )
				break;
		}
	}

	return okay;
}

bool
CDbEntity::SystemNameCheck( const CString& name )
{
	int len = name.GetLength();

	// JUST LIKE NameCheck, but allows leading '_'
	bool okay = (len > 0);
	if ( okay )
	{
		for( int indx = 0; indx < len; ++ indx)
		{
			char ch = name[indx];

			okay = (ch == '_' || isalnum( ch ));

			if ( !okay )
				break;
		}
	}

	return okay;
}

void
CDbEntity::CommonCopy( const CDbEntity& dbEntity )
{
	m_db     = dbEntity.m_db;
	m_id     = dbEntity.m_id;
	m_name   = dbEntity.m_name;
	m_flags  = dbEntity.m_flags;
	m_workplane = dbEntity.m_workplane;
	m_tool   = dbEntity.m_tool;
	m_refcnt = dbEntity.m_refcnt;
	m_owner  = dbEntity.m_owner;
	m_seq    = dbEntity.m_seq;
	m_attribs = dbEntity.m_attribs;

	SelectFlag( false );
	m_box_workid = -2;
	m_box_cache.Invalidate();
}

// virtual C2dBox Box( ID workplaneId = ~0 ) const;
C3dBox
CDbEntity::Box( ID workplaneId ) const
{
	C3dBox box;  // ctor creates as undefined.
	return box;
}

// const unknownDisplayRepresentation() const = 0

bool
CDbEntity::IsRefd() const
{
	return (m_refcnt > 0);
}

int
CDbEntity::RefCnt() const
{
	return m_refcnt;
}

void
CDbEntity::RefInc()
{
	Record();
	++m_refcnt;
}

void
CDbEntity::RefDec()
{
	Record();
	--m_refcnt;
	ASSERT( (m_refcnt >= 0) );
}

void
CDbEntity::RefdBy( CDbEntityList* list ) const
{
	CDbIterator iterator;
	EDbEntityType type;
	
	type = (EDbEntityType) (Type() + 1);

	iterator.Init( (*m_db), type );
	while (1)
	{
		CDbEntity* dbEntity = (CDbEntity*) iterator();
		if (!dbEntity)	// YO! Gotta break the loop eventually...
			break;

		if ( dbEntity->HasRefTo( this ) )
			list->Append( dbEntity );

		iterator.Next();
	}
}

bool
CDbEntity::HasRefTo( const CDbEntity* refdEntity ) const
{
	if ( CDbEntity::IsDeleted() )
		return FALSE;

	if (m_workplane == refdEntity)
		return TRUE;

	if (m_tool == refdEntity)
		return TRUE;

	return FALSE;
}

void
CDbEntity::Orphan()
{
	CDbEntity::RefDec();  // NOTE: this statement also Record()'s this entity's state
	m_owner = NULL;
}

void
CDbEntity::OrphanSeq()
{
	CDbEntity::RefDec();    // NOTE: this statement also Record()'s this entity's state
	m_seq = NULL;
}

void
CDbEntity::Owner( CDbEntity* owner )
{
	CDbContainer* dbContainer;

	if (owner != NULL)
	{
		// This entity is going to have a new owner.
		dbContainer = dynamic_cast<CDbContainer*>( owner );
		if (dbContainer == NULL)
		{
			// An entity that is not a container can
			// not take ownership of another entity.
			CReturn status;
			status.Internal( IDS_INTERNAL_ERROR, "CDbEntity::Owner()" );
			return;
		}
	}

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Now for the meaty stuff.

	dbContainer = dynamic_cast<CDbContainer*>( m_owner );
	if (dbContainer == NULL)
	{
		// This entity does not yet have an owner.
		if (owner != m_owner)
		{
			// There will be a net change in ownership.
			// ASSUMPTION: The new owner has, or will
			// soon have, this entity in its storage.
			if (owner == NULL)
				CDbEntity::RefDec();
			else
				CDbEntity::RefInc();
		}
	}
	else
	{
		// This entity is currently owned by something like
		// a CDbProfile or a CDbFeature.  Change ownership.
		dbContainer->Disown( this );

		if (owner != NULL)
			CDbEntity::RefInc();
	}

	m_owner = owner;
}

// Associates this entity with a 'sequence container'.
void
CDbEntity::Sequence( CDbSequence* seq )
{
	if (seq != m_seq)
	{
		CDbEntity::Record();
		CDbEntity::CreateFlag( false );

		if (m_seq != NULL)
			m_seq->Disown( this );

		m_seq = seq;
		if (seq != NULL)
			RefInc();
	}
}

CDbFeature*
CDbEntity::WorkZone() const
{
	CDbEntity*	dbEntity;
	CDbEntity*	owner;
	int			zonenum;
	
	dbEntity = ((CDbEntity*) this);
	while (1)
	{
		owner = dbEntity->Owner();
		if (owner == NULL)
			break;

		dbEntity = owner;

		zonenum = dbEntity->IntGet( "_zone_num", IUNDEFINED );
		if (zonenum != IUNDEFINED)
			return ( dynamic_cast<CDbFeature*>( dbEntity ) );
	}

	return NULL;
}

// Introduced to support ITI label generation.
// ASSUMPTION: The ITI solution is totally automated and so it is
// unlikely that a user will manipulate a nested part. As such, the
// immediate owner of any lead-curve will be a feature.
bool
CDbEntity::IsChildOfLead() const
{
	CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( Owner() );
	return ((dbFeature != NULL) && dbFeature->IsLead());
}

// virtual void RefsTo( CDbEntityList* list ) const = 0;

// virtual bool HasRefTo( const CDbEntity* refdEntity ) const = 0;

// Attribute management.

int CDbEntity::AttribCount() const
{
	return (m_attribs.countVar());
}

int CDbEntity::IntGet( const CString& name, int defval ) const
{
	return (m_attribs.getInt( name, defval ));
}

double CDbEntity::DoubleGet( const CString& name, double defval ) const
{
	return (m_attribs.getReal( name, defval ));
}

CString CDbEntity::StringGet( const CString& name, const CString& defval ) const
{
	return (m_attribs.getString( name, defval ));
}

void CDbEntity::IntSet( const CString& name, int ival )
{
	m_attribs.setInt( name, ival );
}

void CDbEntity::DoubleSet( const CString& name, double dval )
{
	m_attribs.setReal( name, dval );
}

void CDbEntity::StringSet( const CString& name, const CString& sval )
{
	m_attribs.setString( name, sval );
}

void CDbEntity::AttribsDelete()
{
	m_attribs.Reset();
}

void CDbEntity::AttribDelete( const CString& name )
{
	m_attribs.deleteVar( name );
}

CReturn CDbEntity::setMulti( const CString& multi_str )
{
	// Parse the attribute string
	CCommand multi_att;
	CReturn ret = multi_att.setCommand( multi_str, NULL, NULL );

	// Set the attribute(s)...
	const CVarList& multi_var = multi_att.VarList();
	int num = multi_var.countVar();
	for (int idx=0; idx<num; idx++)
	{
		CVar* var = multi_var.getVar( idx );

		switch (var->getType())
		{
		case VAR_INT:
			m_attribs.setInt( var->getName(), var->getInt());
			break;

		case VAR_REAL:
			m_attribs.setReal( var->getName(), var->getReal() );
			break;

		case VAR_STRING:
			m_attribs.setString( var->getName(), var->getString() );
			break;
		}
	}

	return ret;
}

void
CDbEntity::ColorSet( int color )
{
	pAttrib()->setColor( color );
}

// virtual -- default behavior
int
CDbEntity::ColorGet( int defaultColor ) const
{
	int color;
	if ( m_use_override_color )
	{
		color = m_override_color;
	}
	else
	{
		// By default, an entity's color is defined by its tool's color.
		// However, the entity's color can be directly assigned/overridden
		// (usually) via a Java macro. In the worst case, we punt and
		// simply return the default color.
		color = Attrib().getColor( -1 );
		if (color < 0)
		{
			CDbTool* dbTool = Tool();
			if (dbTool != NULL)
				color = dbTool->ColorGet( -1 );
		}
	}

	return ((color < 0) ?  defaultColor  : color);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Notes about user-commands (copied from UserCommands.doc)
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// User-commands are encoded as:
//    Name    Value
//    @cmd    "@cmd:a= ,b= ,c= "
// where:
//    name is something like @STOP or @DROP
//    value is something like "@DROP:x=1.,y=2."
//
// Using the '@' character ensures that user-command attributes appear as
// the first attributes in the list, and also upholds lexical ordering
// requirements of an attribute list.
//
// Encoding the value as a Portal command allows us to easily parse parameters
// from the value.
//
// User-commands are seperated into two groups 1) system-recognized and
// 2) user-defined.  The former adheres to the aforementioned conventions
// and also have associated display characteristics.
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Gets the count of 'user-command' attributes attached to this entity.
int
CDbEntity::UserCommandCount() const
{
	int count = 0;

	int varCount = m_attribs.countVar();
	for (int indx = 0; indx < varCount; ++indx)
	{
		CVar* var = m_attribs.getVar( indx );

		const CString& name = var->getName();

		if (name[0] != '_')
			continue;

		++count;
	}

	return count;
}

ID
CDbEntity::WorkplaneId() const
{
	return ((m_workplane == NULL) ? 0 : m_workplane->Id());
}

ID
CDbEntity::ToolId() const
{
	return ((m_tool == NULL) ? 0 : m_tool->Id());
}

bool
CDbEntity::IsToolpath() const
{
	CDbTool* dbTool = Tool();
	if (dbTool == NULL)
	{
		// Either we have an entity that does not reference
		// a tool, or the model is corrupt because the
		// entity should reference a tool.

		return FALSE;
	}

	return ( !dbTool->IsLayer() );
}

// See also CDbTool::IndexAngle()
void
CDbEntity::TransientOrientationSet() const
{
	if (m_tool != NULL && m_tool->IsIndexable())
	{
		double orient = DoubleGet( STR_ORIENT, UNDEFINED );
		if (orient != UNDEFINED)
			m_tool->DoubleSet( "~orient", orient );
	}
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
bool
CDbEntity::IsSelected() const
{
	return ((m_flags & DBSELECTED) != 0);
}

void
CDbEntity::SelectFlag( bool set )
{
	if ( set )
		m_flags = (m_flags | DBSELECTED);
	else
		m_flags = (m_flags & ~DBSELECTED);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
bool
CDbEntity::IsSelectable() const
{
	return ((m_flags & DBSELECTABLE) != 0);
}

void
CDbEntity::SelectableFlag( bool set )
{
	if ( set )
		m_flags = (m_flags | DBSELECTABLE);
	else
		m_flags = (m_flags & ~DBSELECTABLE);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
bool CDbEntity::IsHidden() const
{
	return ((m_flags & DBHIDDEN) != 0);
}

void CDbEntity::Hide()
{
	m_flags = (m_flags | DBHIDDEN);
}

void CDbEntity::Seek()
{
	m_flags = (m_flags & ~DBHIDDEN);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
bool CDbEntity::IsSystem() const
{
	return ((m_flags & DBSYSTEM) != 0);
}

void CDbEntity::SystemFlag( bool set )
{
	if ( set )
		m_flags = (m_flags | DBSYSTEM);
	else
		m_flags = (m_flags & ~DBSYSTEM);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
bool CDbEntity::WasCreated() const
{
	return ((m_flags & DBCREATED) != 0);
}

void CDbEntity::CreateFlag( bool set )
{
	if ( set )
		m_flags = (m_flags | DBCREATED);
	else
		m_flags = (m_flags & ~DBCREATED);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
bool CDbEntity::IsDeleted() const
{
	return ((m_flags & DBDELETED) != 0);
}

void CDbEntity::DeleteFlag( bool set )
{
	if ( set )
		m_flags = (m_flags | DBDELETED);
	else
		m_flags = (m_flags & ~DBDELETED);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
bool CDbEntity::IsDirty() const
{
	return ((m_flags & DBMODIFIED) != 0);
}

void CDbEntity::ModifyFlag( bool set )
{
	if ( set )
		m_flags = (m_flags | DBMODIFIED);
	else
		m_flags = (m_flags & ~DBMODIFIED);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
bool CDbEntity::IsFilled() const
{
	return ((m_flags & DBFILLED) != 0);
}

void CDbEntity::FilledFlag( bool set )
{
	if ( set )
		m_flags = (m_flags | DBFILLED);
	else
		m_flags = (m_flags & ~DBFILLED);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Applicable to tools (layers) only.
bool CDbEntity::IsSnappable() const
{
	return ((m_flags & DBSNAP) == DBSNAP);
}

void CDbEntity::SnappableFlag( bool set )
{
	if ( set )
		m_flags = (m_flags | DBSNAP);
	else
		m_flags = (m_flags & ~DBSNAP);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Applicable to tools (layers) only.
bool CDbEntity::IsHotDot() const
{
	return ((m_flags & DBHOTDOT) == DBHOTDOT);
}

void CDbEntity::HotDotFlag( bool set )
{
	if ( set )
		m_flags = (m_flags | DBHOTDOT);
	else
		m_flags = (m_flags & ~DBHOTDOT);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

CReturn CDbEntity::Find(
				ID id,
				CDbEntity** dbEntity,
				EDbEntityType startType,
				EDbEntityType endType ) const
{
	CReturn status = m_db->Find( id, dbEntity, startType, endType );

	return status;
}

CReturn CDbEntity::Create( EDbEntityType type, CDbEntity** dbEntity )
{
	CReturn status = m_db->Create( type, dbEntity);

	return status;
}

void CDbEntity::Delete( CDbEntity** dbEntity )
{
	CString name = (*dbEntity)->Name();
	if (name.GetLength() > 0 && name[0] != '_')
	{
		// We have a user-defined name ( '_' is reserved for derived names )
		m_db->NameRemove( name );
	}

	m_db->Delete( dbEntity );
}

void CDbEntity::Remove( const CDbEntity& dbEntity )
{
	m_db->Remove( dbEntity );
}

void CDbEntity::RemoveRefs()
{
	ASSERT( (Type() != DBPOINT) );
	m_db->RemoveRefs( this );
}

void CDbEntity::Workplane( CDbWorkplane* workplane )
{
	if (workplane != m_workplane)
	{
		CDbEntity::Record();
		CDbEntity::CreateFlag( false );

		if (m_workplane != NULL)
			m_workplane->RefDec();

		m_workplane = workplane;

		if (m_workplane != NULL)
			m_workplane->RefInc();
	}
}

void CDbEntity::Tool( CDbTool* tool )
{
//	ASSERT(tool);
if (tool == NULL)
{ bool stop=true; }

	if (tool != m_tool)
	{
		CDbEntity::Record();
		CDbEntity::CreateFlag( false );

		if (m_tool != NULL)
			m_tool->RefDec();

		m_tool = tool;

		if (m_tool != NULL)
			m_tool->RefInc();
	}
}

FLAGS CDbEntity::Flags() const
{
	return m_flags;
}

void CDbEntity::Flags( FLAGS flags )
{
	m_flags = flags;
}

void CDbEntity::Record()
{
	m_db->UndoBufferRecord( this );
}

bool CDbEntity::IsReferencing() const
{
	return ((m_flags & DBREFERENCE) != 0);
}

void CDbEntity::ReferenceFlagSet()
{
	m_flags = (m_flags | DBREFERENCE);
}

void CDbEntity::ReferenceFlagClear()
{
	m_flags = (m_flags & ~DBREFERENCE);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Copy this into dbEntity... mostly... constructs valid db entry
//	Will return previous copy of the entity, if it exists...
//
CReturn CDbEntity::CopyTo(
	CEntityDb*		io_dest,
	tDbEntityMap*	io_refmap, 
	CDbEntity**		dbEntity )
{
	CReturn ret;

	if (!io_refmap->Lookup( this, *dbEntity ))
	{
		ret += io_dest->Create( Type(), dbEntity );

		if (ret.isOkay())
		{
			// TODO: Slip94 -- may need to restore this call --> CDbEntity::Record();
			//
			// CommonCopy stuff...
			//
			(*dbEntity)->SystemFlag( IsSystem() );
			(*dbEntity)->SelectFlag( false );

			(*dbEntity)->m_attribs = m_attribs;
			(*dbEntity)->m_name = m_name;

			// NOTE: Workplane() will always return NULL for some entity types.
			CDbWorkplane* workplane = Workplane();
			if (workplane != NULL)
			{
				CDbWorkplane* remapped_workplane = workplane;

				if (io_dest != Db())
				{
					//
					// 2 Nov 99 eww -- FIND PLANE BY NAME
					// ... in case the recipient model defined its own.
					//
					CDbEntity*	tmp;
					ret += io_dest->Find( workplane->Name(), &tmp, DBWORKPLANE, DBWORKPLANE );
					if (!tmp)
						ret += workplane->CopyTo( io_dest, io_refmap, &tmp );

					remapped_workplane = dynamic_cast<CDbWorkplane*>(tmp);
				}

				(*dbEntity)->Workplane( remapped_workplane );
			}

			// NOTE: Workplane() will always return NULL for some entity types.
			CDbTool* tool = Tool();
			if (tool != NULL)
			{
				CDbTool* remapped_tool = tool;

				if (io_dest != m_db)
				{
					// Tool re-mapping across databasesCDbTool*
					// Note: the only time we seem to be copying across databases is
					// for nesting.  This new ModelUtil find function deals with
					// empty stations.
					// NOTE:  The "find" aspect is totally lame, but Gary says that is okay for now

					if (tool->IsLayer())
					{ 
						CDbEntity* tmp;
						io_dest->Find( tool->Name(), &tmp, DBTOOL, DBTOOL);
						if (!tmp)
							ret += tool->CopyTo( io_dest, io_refmap, &tmp );

						remapped_tool = dynamic_cast<CDbTool*>(tmp);
					}
					else
					{
						remapped_tool = io_dest->ToolFindCreate( (*tool), TRUE );
					}
				}

				(*dbEntity)->Tool( remapped_tool );
			}

			io_refmap->SetAt( this, *dbEntity );
		}
	}

	return ret;
}

void CDbEntity::CommonSubsetCopy( const CDbEntity& dbEntity )
{
	SystemFlag( dbEntity.IsSystem() );

	SelectFlag( false );

	if ( dbEntity.IsHidden() )
		Hide();

	m_attribs = dbEntity.m_attribs;
	m_name = dbEntity.m_name;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//		Describe2d
//
//	Describe yourself, using lines.
//	See CDisplayEntity for details...
//
void CDbEntity::Describe2d( 
	int			regen,
	C3dCoord*	io_tooltip,		// In world... internal special
	double		tolerance ) const
{
	if ( ZERO(tolerance) )
		return;  // Nothing to do.

	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );

	int	dropstop, chain, indx;
	int	color, style, erase;

	//
	// Entities may carry attributes that describe machine events or objects.
	// Here we must find and draw those attributes.
	// This replaces our earlier job or reading commands.  Commands now take care
	// of themselves.
	//
	dropstop = m_attribs.getInt( "_dropstop", 0 );
	
	//If we are compiling for API we should use this line
	//chain = 1; //m_attribs.getInt("chain", 0) != 0;
	//else we use this line
	chain = m_attribs.getInt("chain", 0) != 0;

	erase = IntGet( "Erase", 0 );

	// -----------------------------------------

	if ( dropstop || chain )
	{
		if (IsSelected() && !erase)
		{
			color = DCOLOR_SELECT;
			style = DSTYLE_SELECT;
		}
		else
		{
			color = ColorGet( DCOLOR_BLUE );
			style = DSTYLE_SOLID;
		}

		dispent->CommandAppend( DCMD_COLOR, (DWORD) color );
	}

	if (dropstop > 0)
	{
		// USES the default STYLE sometimes
		// FORCES a STYLE someimtes
		//
		DescribeDropStop( regen, dropstop, io_tooltip, tolerance );
	}

	if (chain)
	{
		CString display("@");

		indx = dispent->CoordAppend( *io_tooltip );
		dispent->CommandAppend( DCMD_TEXT, (DWORD) indx );

		int num = CDisplayEntity::CommandTextNum( display );
		for (int idx=0; idx<num; idx++)
		{
			dispent->CommandAppend( display, idx );
		}
	}
}

// static
void CDbEntity::CanDrawTarget( bool active )
	{ CDbEntity::m_target_draw = active; }

// static
bool CDbEntity::CanDrawTarget()
	{ return CDbEntity::m_target_draw; }

// static
void CDbEntity::CanDrawLegend( bool active )
	{ CDbEntity::m_legend_draw = active; }

// static
bool CDbEntity::CanDrawLegend()
	{ return CDbEntity::m_legend_draw; }

// static
void CDbEntity::CanDrawHandles( bool active )
	{ CDbEntity::m_handles_draw = active; }

// static
bool CDbEntity::CanDrawHandles()
	{ return CDbEntity::m_handles_draw; }

// (More statics)
//  Ugh. Color override introduced for Pattern/Bump while dragging parts.
void CDbEntity::OverrideColorSet( DWORD color )
{
	CDbEntity::m_override_color = color;
	CDbEntity::m_use_override_color = true;
}

void CDbEntity::OverrideColorClear()
{
	CDbEntity::m_override_color = 0;
	CDbEntity::m_use_override_color = false;
}
 
bool CDbEntity::UseOverrideColor() 
	{ return CDbEntity::m_use_override_color; }

// ----------------------------------------------------------------------------
void CDbEntity::DescribeTarget(
	int				regen,
	const CString&	display,
	C3dCoord*		io_tooltip,
	double			tolerance ) const
{
	if ( !CDbEntity::CanDrawHandles() )
		return;  // Nothing to do.

	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );

	int	indx;


	// @todo Turn off targets if they get too big relative to the view.  Test against tolerance?

	C3dCoord pt = *io_tooltip;
	double	space = tolerance * 1;
	double	mark = tolerance * 3;

	indx = dispent->CoordAppend( pt );
	dispent->CommandAppend( DCMD_SYSDOT, (DWORD) indx );

	if ( CDbEntity::CanDrawTarget() )
	{
		indx = dispent->CoordAppend( pt );
		dispent->CommandAppend( DCMD_TARGET, (DWORD) indx );
	}

	if (display.GetLength() > 2)
	{
		// -----------------------------------------------------
		//
		// The text
		//
		indx = dispent->CoordAppend( *io_tooltip );
		dispent->CommandAppend( DCMD_HOTTEXT, (DWORD) indx );

		int num = CDisplayEntity::CommandTextNum( display );
		for (int idx=0; idx<num; idx++)
		{
			dispent->CommandAppend( display, idx );
		}
	}
}

// ----------------------------------------------------------------------------
#if ORIGINAL_CODE
void
CDbEntity::DescribeDropStop( 	
	int				regen,
	int				dropstop,
	C3dCoord*		io_tooltip,
	double			tolerance ) const
{
	CString		text;
	C3dCoord	pt;

	text = ((dropstop == DROP_CONST) ? "DROP" : "STOP");

	switch (Type())
	{
	case DBLINE:
	case DBARC:
		pt = ((CDbCurve*) this)->EndPt(0);

		DescribeTarget( regen, text, &pt, tolerance );
		break;

	case DBHOLE:
		pt = ((CDbHole*) this)->Center(0);

		DescribeTarget( regen, text, &pt, tolerance );
		break;

	default:
		break;
	}
}
#else
void CDbEntity::DescribeDropStop( 	
	int				regen,
	int				dropstop,
	C3dCoord*		io_tooltip,
	double			tolerance ) const
{
	C3dCoord	pt;

	CString text = ((dropstop == DROP_CONST) ? "DROP" : "STOP");

	switch (Type())
	{
	case DBLINE:
	case DBARC:
		{
		pt = ((CDbCurve*) this)->EndPt(0);
		DescribeTarget( regen, text, &pt, tolerance );
		}
		break;

	case DBHOLE:
		pt = ((CDbHole*) this)->Center(0);

		DescribeTarget( regen, text, &pt, tolerance );
		break;

	default:
		break;
	}
}
#endif

void CDbEntity::draw_2d( 
	const CToolShape&	shape,
	tCmdArray*			io_cmd, 
	tPnt3Array*			io_pnt, 
	const C3x4Matrix&	ref2world,		// Local plane of reference
	double				tolerance,		// Tolerance to explode arcs to
	const C3dCoord&		ct,				// Punch origin
	double				radians,
	bool				fill )
{
	C3x4Matrix local;
	C3dCoord pnt[MAX_TOOL_PNT];
	C3dCoord world[MAX_TOOL_PNT];
	C3dCoord wct;
	int type, indx;

	local.setXYAngle( radians );

	int count = shape.Count();

	// V16.5, increased max from 32 to 1024 (for custom tools, like cluster punches)
	// Also capped upper bound because overrun would cause really hard crash.
	if (count > MAX_TOOL_PNT)
		count = MAX_TOOL_PNT;

	for (indx = 0; indx < count; ++indx)
	{
		// Rotate the tool about its origin.
		local.TransformTo( shape.Point( indx ), &pnt[indx] );

		// Translate the tool so its origin coincides with the given point.
		pnt[indx] += ct;

		ref2world.TransformTo( pnt[indx], &world[indx] );
	}

	ref2world.TransformTo( ct, &wct );

	if ( fill )
	{
		io_cmd->Add( CDisplayEntity::Command( DCMD_META, (DWORD)META_FILL ) );

		io_cmd->Add( CDisplayEntity::Command( DCMD_MOVETO, (DWORD)io_pnt->Add( world[0] ) ) );
		indx = 1;
		while (indx < count)
		{
			type = shape.Type( indx );

			if ((type == 0) || (type == 1))
			{
				// Simply add a vertex.
				io_cmd->Add( CDisplayEntity::Command(
					DCMD_MOVETO, (DWORD)io_pnt->Add( world[indx] ) ) );
			}
			else  // ie. (2) G02 / (3) G03
			{
				CGeoArc arc( world[indx-1], world[indx+1], world[indx], ((type == 2) ? -1 : 1) );

				double radius = arc.Radius();
				if (radius >= SMALL)
				{
					double theta = TWOPI / 64.;  // ie. 32 chords
					double chord_tol = radius * (1. - cos( theta ));

					C3dCoordArray pts;
					arc.Tabulate( chord_tol, ZERO_SHIFT, &pts );

					int pcnt = pts.Count();
					for (int pndx = 0; pndx < pcnt; ++pndx)
					{
						io_cmd->Add( CDisplayEntity::Command(
							DCMD_MOVETO, (DWORD)io_pnt->Add( *pts[pndx] ) ) );
					}

					pts.DestructiveFlush();
				}

				++indx;
			}

			++indx;
		}
	}
	else
	{
		io_cmd->Add( CDisplayEntity::Command( DCMD_META, (DWORD)META_TOOL ) );
		io_cmd->Add( CDisplayEntity::Command( DCMD_TOOLHITCTR, (DWORD)io_pnt->Add( wct ) ) );

		io_cmd->Add( CDisplayEntity::Command( DCMD_MOVETO, (DWORD)io_pnt->Add( world[0] ) ) );
		indx = 1;
		while (indx < count)
		{
			type = shape.Type( indx );

			if (type == 0)
			{
				// Line to this point...
				io_cmd->Add( CDisplayEntity::Command(
					DCMD_MOVETO, (DWORD)io_pnt->Add( world[indx] ) ) );
			}
			else if (type == 1)
			{
				// Line to this point...
				io_cmd->Add( CDisplayEntity::Command(
					DCMD_LINETO, (DWORD)io_pnt->Add( world[indx] ) ) );
			}
			else  // ie. (2) G02 / (3) G03
			{
				CGeoArc arc( world[indx-1], world[indx+1], world[indx], ((type == 2) ? -1 : 1) );

				io_cmd->Add( CDisplayEntity::Command( DCMD_MOVETO, (DWORD)io_pnt->Add( arc.StartPt() ) ) );
				io_cmd->Add( CDisplayEntity::Command( DCMD_ARCCTR, (DWORD)io_pnt->Add( arc.CenterPt()	) ) );
				if (arc.Dir() == CW)
				{
					io_cmd->Add( CDisplayEntity::Command( DCMD_ARCCWTO, (DWORD)io_pnt->Add( arc.MidPt() ) ) );
					io_cmd->Add( CDisplayEntity::Command( DCMD_ARCCWTO, (DWORD)io_pnt->Add( arc.EndPt() ) ) );
				}
				else // CCW
				{
					io_cmd->Add( CDisplayEntity::Command( DCMD_ARCCCTO, (DWORD)io_pnt->Add( arc.MidPt() ) ) );
					io_cmd->Add( CDisplayEntity::Command( DCMD_ARCCCTO, (DWORD)io_pnt->Add( arc.EndPt() ) ) );
				}

				++indx;
			}

			++indx;
		}
	}

	io_cmd->Add( CDisplayEntity::Command( DCMD_META, (DWORD)META_NONE ) );
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//		Transform
//
void CDbEntity::Transform( const C3x4Matrix&	xform )
{
	double	ix, jy;
	int		cutside;
	bool	mirror;

	// PREVIOUSLY: Default transformation for an entity is to... do nothing
	// NOW: The base class method handles the common behavior.
	if (DidAction() || IsDeleted())
		return;

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	DoAction();

	m_box_workid = -2;
	m_box_cache.Invalidate();

	// Adjust the cutside as necessary.
	ix = xform.getI().X();
	jy = xform.getJ().Y();

	mirror = (((ix * jy) - SMALL) <= -1.);
	if (mirror)
	{
		// Flip the cutside.
		cutside = IntGet( STR_CUTSIDE, 0 );
		if (cutside != 0)
			IntSet( STR_CUTSIDE, -cutside );
	}

	if (m_tool != NULL && m_tool->IsPunchTool())
	{
		double	orient, angle;
		double	dx, dy, iy;

		// See also CDbTool::IndexAngle()
		orient = Attrib().getReal( STR_ORIENT, UNDEFINED );
		if (orient < UNDEFINED)
		{
			// 2004.08.06 (PE) -- Rittal reported that mirroring
			// and auto-indexable tool about the Y-axis adversly
			// affected the tool orientation.  Now, we check for
			// a mirroring transformation by inspecting the
			// cross-product of the transform matrix.
			iy = xform.getI().Y();

			if (mirror)
			{
				dx = ix * cos( DEG2RAD * orient );
				dy = jy * sin( DEG2RAD * orient );

				angle = RAD2DEG * atan2( dy, dx );
			}
			else
			{
				// rotation
				angle = orient + (RAD2DEG * atan2( iy, ix ));
			}

			DoubleSet( STR_ORIENT, angle );
		}
	}
}


CString CDbEntity::DerivedName() const
{
	CString name;

	switch ( Type() )
	{
	case DBWORKPLANE:	name.Format( "_work%d", m_id );		break;
	case DBTOOL:		name.Format( "_tool%d", m_id );		break;
	case DBPOINT:		name.Format( "_point%d", m_id );	break;
	case DBLINE:		name.Format( "_line%d", m_id );		break;
	case DBARC:			name.Format( "_arc%d", m_id );		break;
	case DBHOLE:		name.Format( "_hole%d", m_id );		break;
	case DBPROFILE:		name.Format( "_profile%d", m_id );	break;
	case DBCOMMAND:		name.Format( "_command%d", m_id );	break;
	case DBFEATURE:		name.Format( "_feature%d", m_id );	break;
	case DBSEQUENCE:	name.Format( "_sequence%d", m_id );	break;
	case DBPATTERN:		name.Format( "_pattern%d", m_id );	break;
	
	default:			ASSERT( FALSE );		break;
	}

	return name;
}


// Must put these here to be able to access static int m_new_action;

int CDbEntity::NewAction()				
	{ return ++m_new_action; }

void CDbEntity::DoAction()						
	{ m_action = m_new_action; }

bool CDbEntity::DidAction() const	
	{ return (m_action == m_new_action); }

bool CDbEntity::is_hidden() const
{
	if ( IsDeleted() )
		return TRUE;

	if ( IsHidden() )
		return TRUE;

	// Note: Profile and Feature get tool by proxy.
#if V16_ORIGINAL
	if (Tool() && Tool()->IsHidden())
		return TRUE;
#else
	// TODO: Aaaargh!  Now that profiles and features are
	// containers that are not associated with a tool (layer)
	// we have issues with hiding instances and work-zones.
	// With 'this' change, we can at least hide tools and layers
	// without making everything disappear.
	if (Type() != DBFEATURE)
	{
		CDbTool* dbTool = Tool();
		if (dbTool && dbTool->IsHidden())
			return TRUE;
	}
#endif

	return FALSE;
}

void CDbEntity::FlipCutside( bool flip_cutside )
{
	m_flip_cutside = flip_cutside;
}

bool CDbEntity::FlipCutside()
{
	return m_flip_cutside;
}

void CDbEntity::TempWorkplane( CDbWorkplane* workplane )
{
	m_workplane = workplane;
}
