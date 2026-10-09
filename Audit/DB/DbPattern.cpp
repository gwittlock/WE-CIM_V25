
#include "stdafx.h"
#include "StringConst.h"
#include "CommonFlags.h"
#include "EntityDb.h"
#include "DbEntityVisitor.h"
#include "DbAllEntities.h"
#include "EntityCopier.h"
#include "DisplayEntity.h"

#include "DbPattern.h"

////////////////////////////////////////////////////////////////////////

CDbPattern::CDbPattern( CEntityDb* db )
	: CDbContainer( db ),
	  m_list()
{
	// Copy the default attributes from the database.
	// See also comment in CDbEntity::CDbEntity()
	(*pAttrib()) = CDbEntity::Db()->Default();
}

// CAREFUL!  This ctor creates a shallow copy!  This ctor was
// introduced to support the undo buffer.  As features can be
// nested, operations on features are applied via recursive
// descent.  In turn, each feature will record itself in the
// undo buffer.  Hence, we only need a shallow copy.
CDbPattern::CDbPattern( const CDbPattern& dbPattern )
	: CDbContainer( dbPattern ),
	  m_list()
{
	int count = dbPattern.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = dbPattern[ indx ];
		m_list.Append( dbEntity );
	}
}

CDbPattern::~CDbPattern()
{
	if ( CDbEntity::IsReferencing() )
		CDbEntity::Remove( (*this) );
}

int CDbPattern::Count() const
	{ return m_list.Count(); }

CDbEntity* CDbPattern::GetAt( int indx ) const
	{ return m_list[indx]; }

CDbEntity* CDbPattern::operator[]( int indx ) const
	{ return m_list[indx]; }

CDbEntity* CDbPattern::ReplaceAt( int indx, CDbEntity* dbEntity )
{
	CDbEntity* prev = NULL;

	if (dbEntity == NULL)
	{
		CReturn status;
		status.Diagnostic( "CDbPattern::ReplaceAt()" );
	}
	else
	{
		// NOTE: We don't bother to check if the entity is
		// already in this list but we probably should.
		prev = m_list.GetAt( indx );
		if (prev != NULL)
		{
			prev = m_list.Replace( indx, dbEntity );  // 'prev' should still point to same
			dbEntity->Owner( this );
			prev->Orphan();
		}
	}

	return prev;
}

CReturn CDbPattern::Prepend( CDbEntity* dbEntity, bool copy )
{
	CReturn status;

	if ( copy )
	{
		tDbEntityMap refmap;
		CDbEntity* dbCopy = NULL;
		
		status = dbEntity->CopyTo( m_db, &refmap, &dbCopy );

		if ( !status.IsOk() )
			return status;

		dbEntity = dbCopy;
	}

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	dbEntity->Owner( this );

	// TODO: Do we have to prevent 'cyclic inclusion'?  If someone deletes
	// a feature, are all entities owned by the feature also deleted?  If
	// so, are we going to encounter problems multiply deleting the same
	// dbEntity?

	m_list.Prepend( dbEntity );

	return status;
}

CReturn CDbPattern::Append( CDbEntity* dbEntity, bool copy )
{
	CReturn status;

	if ( copy )
	{
		tDbEntityMap refmap;
		CDbEntity* dbCopy = NULL;
		
		status = dbEntity->CopyTo( m_db, &refmap, &dbCopy );

		if ( !status.IsOk() )
			return status;

		dbEntity = dbCopy;
	}

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	dbEntity->Owner( this );

	// TODO: Do we have to prevent 'cyclic inclusion'?  If someone deletes
	// a feature, are all entities owned by the feature also deleted?  If
	// so, are we going to encounter problems multiply deleting the same
	// dbEntity?

	m_list.Append( dbEntity );

	return status;
}

int
CDbPattern::InsertBefore( CDbEntity* refEntity, CDbEntity* newEntity )
{
	int indx = m_list.Find( refEntity );

	if (indx >= 0)
		InsertBefore( indx, newEntity );

	return indx;
}

int
CDbPattern::InsertBefore( int indx, CDbEntity* newEntity )
{
	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	newEntity->Owner( this );

	m_list.InsertBefore( indx, newEntity );

	return indx;
}

int
CDbPattern::InsertAfter( CDbEntity* refEntity, CDbEntity* newEntity )
{
	int indx = m_list.Find( refEntity );

	if (indx >= 0)
		InsertAfter( indx, newEntity );

	return indx;
}

int
CDbPattern::InsertAfter( int indx, CDbEntity* newEntity )
{
	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	newEntity->Owner( this );

	m_list.InsertAfter( indx, newEntity );

	return indx;
}

int
CDbPattern::Position( const CDbEntity* refEntity ) const
{
	int indx = m_list.Find( (CDbCurve*) refEntity );
	return indx;
}

// NOTE: Mutually recursive with CDbEntity::Owner( CDbEntity* )
bool
CDbPattern::Disown( CDbEntity* dbEntity )
{
	int indx = m_list.Find( dbEntity );

	if (indx < 0)
		return FALSE;

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	m_list.Remove( indx );


	// Previously, CDbEntity::Owner( CDbEntity* ) was mutually recursive
	// with CDbContainer::Disown( CDbEntity* ).  Resolving ownership in
	// that manner is clearly problematic.  Though we could have used
	// entity flags to manage this (such as CDbEntity::TagSet()) that
	// method introduces its own set of problems.  In the end, I decided
	// to employ an encapsulated, non-recursive solution that leverages
	// the class friend construct.
	//
	//      dbEntity->Owner( NULL );
	//
	dbEntity->Orphan();

	return TRUE;
}

CReturn
CDbPattern::DestructiveFlush()
{
	CReturn status;

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	int count = m_list.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = m_list[0];
		//Disown( dbEntity );
		dbEntity->Delete();
	}

	return status;
}

void
CDbPattern::BenignFlush()
{
	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	int count = m_list.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = m_list[0];
		Disown( dbEntity );
	}
}

void
CDbPattern::RefsTo( CDbEntityList* list ) const
{
	int count = m_list.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		list->Append( m_list[indx] );
	}
}

void
CDbPattern::Subordinates( CDbEntityList* list ) const
{
	int count = m_list.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		list->Append( m_list[indx] );
	}
}

// Get the atomic entities.
void CDbPattern::Flatten( CDbEntityList* entities ) const
{
	// 2012.06.16 (PE) -- Can't see any reason to implement at this time.
}

bool
CDbPattern::HasRefTo( const CDbEntity* refdEntity ) const
{
	if ( CDbEntity::IsDeleted() )
		return FALSE;

	if ( CDbEntity::HasRefTo( refdEntity ) )
		return TRUE;

	int indx = m_list.Find( (CDbEntity*) refdEntity );

	return (indx >= 0);
}

void
CDbPattern::Delete()
{
	if ( CDbEntity::IsDeleted() )
		return;

	CDbEntity::DeleteFlag( true );
	CDbEntity::Record();

	// A pattern owns its entities.  Therefore, when the
	// patter is deleted, its entities must also be deleted.

	DestructiveFlush();

#if REQUIRED  // V16-beta, a pattern does not have an owner.
	CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( CDbEntity::Owner() );
	if (dbContainer != NULL)
		dbContainer->Disown( this );
#endif

	CDbEntity::Workplane( NULL );
}

void
CDbPattern::Accept( CDbEntityVisitor* visitor )
{
	int		count, indx;

	visitor->Visit( this );

	indx = 0;
	count = m_list.Count();

	while (indx < count)
	{
		m_list[indx]->Accept( visitor );
		if (count == m_list.Count())
			++indx;
		else
			count = m_list.Count();  // the list changed
	}
}

void
CDbPattern::RemoveRef( CDbEntity* dbEntity )
{
	int indx = m_list.Find( dbEntity );

	if (indx < 0)
		return;

	m_list.Remove( indx );

	dbEntity->RefDec();
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const CDbPattern&
CDbPattern::operator = ( const CDbPattern& dbPattern )
{
	CDbEntity::CommonCopy( dbPattern );

	m_list.BenignFlush();

	int count = dbPattern.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = dbPattern[ indx ];
		m_list.Append( dbEntity );
	}

	return (*this);
}

CReturn
CDbPattern::Clone( CDbEntity** dbEntity ) const
{
	CReturn status;

	CDbPattern* dbPattern = new CDbPattern( (*this) );

	(*dbEntity) = dbPattern;

	return status;
}

CReturn
CDbPattern::ContentsSwap( CDbEntity* dbEntity )
{
	CReturn status;

	CDbPattern* dbPattern = dynamic_cast<CDbPattern*>( dbEntity );

	ASSERT( (dbPattern != NULL) );

	CDbPattern tmp( (*this) );
	(*this) = (*dbPattern);
	(*dbPattern) = tmp;

	// Suppress reference count modifications.
	tmp.ReferenceFlagClear();

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
CReturn 
CDbPattern::CopyTo( 
	CEntityDb*		io_dest,
	tDbEntityMap*	io_refmap, 
	CDbEntity**		dbEntity )
{
	CReturn status;

	//
	// Generic Copy... sets up the database and maps...
	//
	status += CDbEntity::CopyTo( io_dest, io_refmap, dbEntity );
	CDbPattern* dbPattern = (CDbPattern*)(*dbEntity);

	//
	// References and specific data ...
	//
	CDbEntity*	refto;
	int count = m_list.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		if (!io_refmap->Lookup( m_list[indx], refto ))
			m_list[indx]->CopyTo( io_dest, io_refmap, &refto );

		status += dbPattern->Append( refto );
	}

	return status;
}


C3dBox
CDbPattern::Box( ID workplaneId ) const
{
	C3dBox	world;

	for (int idx=0; idx<m_list.Count(); idx++)
	{
		CDbEntity*	db_ent = m_list[idx];
		const C3dBox& box = db_ent->Box(workplaneId);
		if (box.IsDefined())
			world += box;
	}

	return world;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//		Transform
//
void
CDbPattern::Transform( 
	const C3x4Matrix&	in_xform )
{
	if (DidAction())
		return;

	CDbEntity::Transform( in_xform );

	int count = Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = m_list[ indx ];
		dbEntity->Transform( in_xform );
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
bool
CDbPattern::IsMain() const
{
	CString	type = StringGet( STR_TYPE, "" );
	return (type.CompareNoCase("_main") == 0);
}

bool
CDbPattern::IsSheet() const
{
	CString	type = StringGet( STR_TYPE, "" );
	return (type.CompareNoCase("_sheet") == 0);
}


bool CDbPattern::IsA(CString test) const
{
	CString	type = StringGet( STR_TYPE, "" );
	return (type.Left(test.GetLength()).CompareNoCase(test) == 0);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//		Describe2d
//
//	Describe yourself, using lines.
//	See CDisplayEntity for details...
//
void
CDbPattern::Describe2d( 
	int				regen,
	C3dCoord*		io_tooltip,
	double			in_tolerance,
	CDisplayEntity*	dispent ) const
{
	bool allocated = false;

	if (dispent == NULL)
		dispent = DisplayEntityGet( &allocated );

	int		indx;

	// Draw the pattern label relative to the given point (io_tooltip)
	// In the case of an instance, the point will be the instance
	// origin.  In the case of a pattern, this will be (0,0).

	CString		name = StringGet( "_label", "<pattern>" );
	C3dCoord	tip = (*io_tooltip);
	double		dx = DoubleGet( "_label_dx", 0. );
	double		dy = DoubleGet( "_label_dy", 0. );
	double		angle = DoubleGet( "_label_angle", 0. );
	int			size = IntGet( "_label_size", 10 );
	eDisplayTextPos pos = (eDisplayTextPos)IntGet( "pos", 0 );

	tip.X( tip.X() + dx );
	tip.Y( tip.Y() + dy );

	dispent->CommandAppend( DCMD_COLOR, (DWORD) DCOLOR_WHITE );
	dispent->CommandAppend( DCMD_TEXTPOS, (DWORD) pos );
	dispent->CommandAppend( DCMD_TEXTANG, (float) angle );
	dispent->CommandAppend( DCMD_TEXTSIZE, (DWORD) size );

	indx = dispent->CoordAppend( tip );
	dispent->CommandAppend( DCMD_TEXT, (DWORD) indx );

	int num = CDisplayEntity::CommandTextNum( name );
	for (int idx=0; idx<num; idx++)
	{
		dispent->CommandAppend( name, idx );
	}

#define REQUIRED 1
#if REQUIRED
	CDbCurve* ce = NULL;
	int count = m_list.Count();
	while (count)
	{
		ce = dynamic_cast<CDbCurve*>(m_list[ --count ]);
		if (ce)
			break;
	}

	if ( ce )
	{
		C3dCoord pe = ce->EndPt( 0 );
		CDbEntity::Describe2d( regen, &pe, in_tolerance );
	}
#endif
}


