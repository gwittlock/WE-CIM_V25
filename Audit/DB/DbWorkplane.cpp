
#include "stdafx.h"
#include "EntityDb.h"
#include "DbWorkplane.h"
#include "DbEntityVisitor.h"
#include "DisplayEntity.h"



////////////////////////////////////////////////////////////////////////

CDbWorkplane::CDbWorkplane( CEntityDb* db )
	: CDbEntity( db )
{
	// Copy the default attributes from the database.
	// See also comment in CDbEntity::CDbEntity()
	// int color = CDbEntity::Db()->Default().getColor( RGB(255,0,0) );
	// pAttrib()->setColor( color );

	CDbEntity::CreateFlag( false );
}

CDbWorkplane::CDbWorkplane( const CDbWorkplane& dbWorkplane )
	: CDbEntity( dbWorkplane ),
	  m_xform( dbWorkplane.m_xform ),
	  m_inverse( dbWorkplane.m_inverse )
{
	CDbEntity::CreateFlag( false );
}

CDbWorkplane::~CDbWorkplane()
{
	if ( CDbEntity::IsReferencing() )
		CDbEntity::Remove( (*this) );
}

void
CDbWorkplane::Init( 
	const C3dVec&	ivec, 
	const C3dVec&	jvec, 
	const C3dVec&	kvec, 
	const C3dCoord& origin,
	bool			up)
{
	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	m_xform.setI( ivec );
	m_xform.setJ( jvec );
	m_xform.setK( kvec );
	m_xform.setT( origin );

	m_tool_up = up;

	m_xform.InvertTo( &m_inverse );
}

const C3x4Matrix&
CDbWorkplane::Transform() const
{
	return m_xform;
}

const C3x4Matrix&
CDbWorkplane::Inverse() const
{
	return m_inverse;
}

void
CDbWorkplane::RefsTo( CDbEntityList* list ) const
{
	// Do nothing.  A workplane does not reference any entities.
	return;
}

void
CDbWorkplane::Subordinates( CDbEntityList* list ) const
{
	// Do nothing.  A workplane does not have any subordinate entities.
	return;
}

bool
CDbWorkplane::HasRefTo( const CDbEntity* refdEntity ) const
{
	// A workplane never references another entity.
	return FALSE;
}

void
CDbWorkplane::Delete()
{
#if DISABLED_2017_06_24
	ASSERT( (CDbEntity::IsRefd() == FALSE) );  // All references must be gone!
#endif

	if ( CDbEntity::IsDeleted() )
		return;

	CDbEntity::DeleteFlag( true );
	CDbEntity::Record();

	CDbEntity::RemoveRefs();
}

void
CDbWorkplane::Accept( CDbEntityVisitor* visitor )
{
	visitor->Visit( this );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void
CDbWorkplane::RemoveRef( CDbEntity* dbEntity )
{
	// Nothing to do.  A workplane never references another entity.
	return;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const CDbWorkplane&
CDbWorkplane::operator = ( const CDbWorkplane& dbWorkplane )
{
	CDbEntity::CommonCopy( dbWorkplane );

	m_xform = dbWorkplane.m_xform;
	m_inverse = dbWorkplane.m_inverse;

	return (*this);
}

CReturn
CDbWorkplane::Clone( CDbEntity** dbEntity ) const
{
	CReturn status;

	CDbWorkplane* dbWorkplane = new CDbWorkplane( (*this) );

	(*dbEntity) = dbWorkplane;

	return status;
}

CReturn
CDbWorkplane::ContentsSwap( CDbEntity* dbEntity )
{
	CReturn status;

	CDbWorkplane* dbWorkplane = dynamic_cast<CDbWorkplane*>( dbEntity );

	ASSERT( (dbWorkplane != NULL) );

	CDbWorkplane tmp( (*this) );
	(*this) = (*dbWorkplane);
	(*dbWorkplane) = tmp;

	// Suppress reference count modifications.
	tmp.ReferenceFlagClear();

	return status;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//		Describe2d
//
//	Describe yourself, using lines.
//	See CDisplayEntity for details...
//
void
CDbWorkplane::Describe2d( 
	int				regen,
	C3dCoord*		io_tooltip,
	double			in_tolerance ) const
{
	// 2005.10.14 (PE) -- CDbEntity::CanDrawTarget() was added to
	// provide suppression of drawing instance handles during
	// printing.  Perhaps not the best approach, but we use that
	// same flag here to suppress the drawing of workplanes
	// during printing as well.  Alternately, we could introduce
	// another flag or draw the workplane much smaller.
	if ( !CDbEntity::CanDrawTarget() )
		return;

	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );

	// We always want to regenerate the display list
	// because a workplane is not a "scalable" entity.
	dispent->Flush();

	double	length = in_tolerance * 25;
	double	descend = length / 8;
	double	arm = length / 4;
	int		indx;

	dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_SOLID );
	dispent->CommandAppend( DCMD_META, (DWORD) META_SYS );

	// --------------------------------------
	dispent->CommandAppend( DCMD_COLOR, (DWORD) DCOLOR_CYAN );

	C3dCoord st = m_xform.getT();
	C3dCoord en = st + m_xform.getI() * length;

	indx = dispent->CoordAppend( st );
	dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

	indx = dispent->CoordAppend( en );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

	st = m_xform.getT() + m_xform.getI() * arm*3;
	st += m_xform.getJ() * -descend;
	en = st;
	en += m_xform.getI() * arm;
	en += m_xform.getJ() * -arm;

	indx = dispent->CoordAppend( st );
	dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

	indx = dispent->CoordAppend( en );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

	st = m_xform.getT() + m_xform.getI() * length;
	st += m_xform.getJ() * -descend;
	en = st;
	en += m_xform.getI() * -arm;
	en += m_xform.getJ() * -arm;

	indx = dispent->CoordAppend( st );
	dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

	indx = dispent->CoordAppend( en );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );
	

	// --------------------------------------
	dispent->CommandAppend( DCMD_COLOR, (DWORD) DCOLOR_GREEN );

	st = m_xform.getT();
	en = st + m_xform.getJ() * length;

	indx = dispent->CoordAppend( st );
	dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

	indx = dispent->CoordAppend( en );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

	if (m_tool_up < 0)
		st = m_xform.getT() + m_xform.getJ() * (length - arm);
	else
		st = m_xform.getT() + m_xform.getJ() * length;

	st += m_xform.getI() * -descend;
	en = st;
	en += m_xform.getI() * -(arm/2);
	en += m_xform.getJ() * -(m_tool_up*arm/2);

	indx = dispent->CoordAppend( st );
	dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

	indx = dispent->CoordAppend( en );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

	st += m_xform.getI() * -arm;

	indx = dispent->CoordAppend( st );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

	st = en;
	st += m_xform.getJ() * -(m_tool_up*arm/2);

	indx = dispent->CoordAppend( st );
	dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

	indx = dispent->CoordAppend( en );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

	// --------------------------------------
	dispent->CommandAppend( DCMD_COLOR, (DWORD) DCOLOR_BLUE );

	st = m_xform.getT();
	en = st + m_xform.getK() * length;

	indx = dispent->CoordAppend( st );
	dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

	indx = dispent->CoordAppend( en );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

	dispent->CommandAppend( DCMD_META, (DWORD) META_NONE );
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
CReturn 
CDbWorkplane::CopyTo( 
	CEntityDb*		io_dest,
	tDbEntityMap*	io_refmap, 
	CDbEntity**		dbEntity )
{
	CReturn status;

	//
	// Generic Copy... sets up the database and maps...
	//
	status += CDbEntity::CopyTo( io_dest, io_refmap, dbEntity );
	CDbWorkplane* dbWorkplane = (CDbWorkplane*)(*dbEntity);

	//
	// References and specific data ...
	//
	dbWorkplane->Init( m_xform.getI(), m_xform.getJ(), m_xform.getK(), m_xform.getT(), m_tool_up );
	dbWorkplane->Name( Name() );

	return status;

}


