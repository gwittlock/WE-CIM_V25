
#include "stdafx.h"
#include "StringConst.h"
#include "EntityDb.h"
#include "DbEntityVisitor.h"
#include "DbWorkplane.h"
#include "DbTool.h"
#include "DbLine.h"
#include "DbArc.h"
#include "DbHole.h"
#include "DbProfile.h"
#include "DbFeature.h"
#include "DisplayEntity.h"

CToolShape	g_clamp_shape;
CToolShape	g_hold_shape;

int g_zone_color[5] =
{ DCOLOR_BLUE, DCOLOR_GREEN, DCOLOR_YELLOW, DCOLOR_MAGENTA, DCOLOR_CYAN };

////////////////////////////////////////////////////////////////////////

CDbFeature::CDbFeature( CEntityDb* db )
	: CDbContainer( db ),
	  m_refd(),
	  m_list()
{
	// Copy the default attributes from the database.
	// See also comment in CDbEntity::CDbEntity()
	(*pAttrib()) = CDbEntity::Db()->Default();
	m_subtype = FEATURE_GENERIC;
}

// CAREFUL!  This ctor creates a shallow copy!  This ctor was
// introduced to support the undo buffer.  As features can be
// nested, operations on features are applied via recursive
// descent.  In turn, each feature will record itself in the
// undo buffer.  Hence, we only need a shallow copy.
CDbFeature::CDbFeature( const CDbFeature& dbFeature )
	: CDbContainer( dbFeature ),
	  m_refd(),
	  m_list()
{
	dbFeature.RefsTo( &m_refd );

	int count = dbFeature.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = dbFeature[ indx ];
		m_list.Append( dbEntity );
	}
	m_subtype = dbFeature.m_subtype;
}

CDbFeature::~CDbFeature()
{
	if ( CDbEntity::IsReferencing() )
		CDbEntity::Remove( (*this) );
}

CDbWorkplane* CDbFeature::Workplane() const
	{ return ((m_list.Count() > 0) ? m_list[0]->Workplane() : NULL); }

CDbTool* CDbFeature::Tool() const
	{ return ((m_list.Count() > 0) ? m_list[0]->Tool() : NULL); }


int CDbFeature::Count() const
	{ return m_list.Count(); }

CDbEntity* CDbFeature::GetAt( int indx ) const
	{ return m_list[indx]; }

CDbEntity* CDbFeature::operator[]( int indx ) const
	{ return m_list[indx]; }

CDbEntity* CDbFeature::ReplaceAt( int indx, CDbEntity* dbEntity )
{
	CDbEntity* prev = NULL;

	if (dbEntity == NULL)
	{
		CReturn status;
		status.Diagnostic( "CDbFeature::ReplaceAt()" );
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

CReturn CDbFeature::Prepend( CDbEntity* dbEntity, bool copy )
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

CReturn CDbFeature::Append( CDbEntity* dbEntity, bool copy )
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
CDbFeature::InsertBefore( CDbEntity* refEntity, CDbEntity* newEntity )
{
	int indx = m_list.Find( refEntity );

	if (indx >= 0)
		InsertBefore( indx, newEntity );

	return indx;
}

int
CDbFeature::InsertBefore( int indx, CDbEntity* newEntity )
{
	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	newEntity->Owner( this );

	m_list.InsertBefore( indx, newEntity );

	return indx;
}

int
CDbFeature::InsertAfter( CDbEntity* refEntity, CDbEntity* newEntity )
{
	int indx = m_list.Find( refEntity );

	if (indx >= 0)
		InsertAfter( indx, newEntity );

	return indx;
}

int
CDbFeature::InsertAfter( int indx, CDbEntity* newEntity )
{
	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	newEntity->Owner( this );

	m_list.InsertAfter( indx, newEntity );

	return indx;
}

int
CDbFeature::Position( const CDbEntity* refEntity ) const
{
	int indx = m_list.Find( (CDbCurve*) refEntity );
	return indx;
}


CReturn	
CDbFeature::MoveBefore( 
	CDbEntity*	dstEntity, 
	CDbEntity*	srcEntity )
{
	int src = m_list.Find( srcEntity );
	int dst = m_list.Find( dstEntity );

	if ( (src < 0)
		|| (dst < 0)
		|| (src == dst) )
		return CReturn( STATUS_ERROR );

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	if (src < dst)
		dst--;

	m_list.Remove( src );
	m_list.InsertBefore( dst, srcEntity );

	return CReturn( STATUS_OKAY );
}

CReturn	
CDbFeature::MoveAfter( 
	CDbEntity*	dstEntity, 
	CDbEntity*	srcEntity )
{
	int src = m_list.Find( srcEntity );
	int dst = m_list.Find( dstEntity );

	if ( (src < 0)
		|| (dst < 0)
		|| (src == dst) )
		return CReturn( STATUS_ERROR );

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	if (src < dst)
		dst--;

	m_list.Remove( src );
	m_list.InsertAfter( dst, srcEntity );

	return CReturn( STATUS_OKAY );
}


// NOTE: Mutually recursive with CDbEntity::Owner( CDbEntity* )
bool
CDbFeature::Disown( CDbEntity* dbEntity )
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
CDbFeature::DestructiveFlush()
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
CDbFeature::BenignFlush()
{
//	CReturn status;

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	int count = m_list.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = m_list[0];
		Disown( dbEntity );
	}

//	return status;
}



CReturn
CDbFeature::RefsFlush()
{
	CReturn status;

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	int count = m_refd.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = m_refd.Remove(0);
		dbEntity->RefDec();
	}

	return status;
}

void
CDbFeature::AddRef( CDbEntity* dbEntity )
{
	if (m_refd.Find( dbEntity ) >= 0)
		return;  // This feature already references the Entity.

	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	m_refd.Append( dbEntity );

	dbEntity->RefInc();
}

void
CDbFeature::RefsTo( CDbEntityList* list ) const
{
	int count = m_refd.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		list->Append( m_refd[indx] );
	}
}

void
CDbFeature::Subordinates( CDbEntityList* list ) const
{
	int count = m_list.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		list->Append( m_list[indx] );
	}
}

void
CDbFeature::Flatten( CDbEntityList* entities ) const
{
	int count = m_list.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = m_list[indx];

		CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( dbEntity );
		if (dbContainer != NULL)
			dbContainer->Flatten( entities );
		else
			entities->Append( dbEntity );
	}
}

bool
CDbFeature::HasRefTo( const CDbEntity* refdEntity ) const
{
	if ( CDbEntity::IsDeleted() )
		return FALSE;

	if ( CDbEntity::HasRefTo( refdEntity ) )
		return TRUE;

	int indx = m_refd.Find( (CDbEntity*) refdEntity );

	return (indx >= 0);
}

void
CDbFeature::Delete()
{
	if ( CDbEntity::IsDeleted() )
		return;

	CDbEntity::DeleteFlag( true );
	CDbEntity::Record();

	// A feature owns its entities.  Therefore, when the
	// feature is deleted, its entities must also be deleted.

	DestructiveFlush();

	// Remove the entity from any container.
	CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( CDbEntity::Owner() );
	if (dbContainer != NULL)
		dbContainer->Disown( this );

	CDbSequence* dbSequence = CDbEntity::Sequence();
	if (dbSequence != NULL)
		this->Sequence( NULL );
}

void
CDbFeature::Accept( CDbEntityVisitor* visitor )
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

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void
CDbFeature::RemoveRef( CDbEntity* dbEntity )
{
	int indx = m_refd.Find( dbEntity );

	if (indx < 0)
		return;

	m_refd.Remove( indx );

	dbEntity->RefDec();
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const CDbFeature&
CDbFeature::operator = ( const CDbFeature& dbFeature )
{
	CDbEntity::CommonCopy( dbFeature );

	m_refd.BenignFlush();
	dbFeature.RefsTo( &m_refd );

	m_list.BenignFlush();

	int count = dbFeature.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CDbEntity* dbEntity = dbFeature[ indx ];
		m_list.Append( dbEntity );
	}

	m_subtype = dbFeature.m_subtype;

	return (*this);
}

CReturn
CDbFeature::Clone( CDbEntity** dbEntity ) const
{
	CReturn status;

	CDbFeature* dbFeature = new CDbFeature( (*this) );

	(*dbEntity) = dbFeature;

	return status;
}

CReturn
CDbFeature::ContentsSwap( CDbEntity* dbEntity )
{
	CReturn status;

	CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( dbEntity );

	ASSERT( (dbFeature != NULL) );

	CDbFeature tmp( (*this) );
	(*this) = (*dbFeature);
	(*dbFeature) = tmp;

	// Suppress reference count modifications.
	tmp.ReferenceFlagClear();

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
CReturn 
CDbFeature::CopyTo( 
	CEntityDb*		io_dest,
	tDbEntityMap*	io_refmap, 
	CDbEntity**		dbEntity )
{
	CReturn status;

	//
	// Generic Copy... sets up the database and maps...
	//
	status += CDbEntity::CopyTo( io_dest, io_refmap, dbEntity );
	CDbFeature* dbFeature = (CDbFeature*)(*dbEntity);

	CDbEntity*	refto;
	int count = m_list.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		if (!io_refmap->Lookup( m_list[indx], refto ))
			m_list[indx]->CopyTo( io_dest, io_refmap, &refto );

		status += dbFeature->Append( refto );
	}

	dbFeature->m_subtype = m_subtype;

	return status;
}


C3dBox
CDbFeature::Box( ID workplaneId ) const
{
	C3dBox	world;

	CString type = m_attribs.getString( STR_TYPE, "none" );
	if ( !type.CompareNoCase( "_zone" )
		|| !type.CompareNoCase( "_clamp" ) )
	{
		// Zone features have a special definition
		world += C3dCoord( m_attribs.getReal( "_zone_left", 0.0 ),
							m_attribs.getReal( "_zone_top", 0.0 ), 
							0.0 );
				
		world += C3dCoord( m_attribs.getReal( "_zone_right", 0.0 ),
							m_attribs.getReal( "_zone_bottom", 0.0 ), 
							0.0 );
	}
	else
	{
		for (int idx=0; idx<m_list.Count(); idx++)
		{
			CDbEntity*	db_ent = m_list[idx];
			const C3dBox& box = db_ent->Box(workplaneId);
			if (box.IsDefined())
				world += box;
		}
	}

	return world;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//		Transform
//
void
CDbFeature::Transform( 
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
CDbFeature::IsWorkZone() const
{
	CString	type = StringGet( STR_TYPE, "" );
	return (type.CompareNoCase("_zone") == 0);
}

bool
CDbFeature::IsPart() const
{
	CString	type = StringGet( STR_TYPE, "" );
	return (type.CompareNoCase("_part") == 0);
}

bool
CDbFeature::IsLead() const
{
	CString	type = StringGet( STR_TYPE, "" );
	return (type.Left(5).CompareNoCase( "_lead" ) == 0);
}

bool
CDbFeature::IsLeadIn() const
{
	CString	type = StringGet( STR_TYPE, "" );
	return (type.CompareNoCase( "_lead_in" ) == 0);
}

bool
CDbFeature::IsLeadOut() const
{
	CString	type = StringGet( STR_TYPE, "" );
	return (type.CompareNoCase( "_lead_out" ) == 0);
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//		Describe2d
//
//	Describe yourself, using lines.
//	See CDisplayEntity for details...
//
void
CDbFeature::Describe2d( 
	int				regen,
	C3dCoord*		io_tooltip,
	double			in_tolerance ) const
{
	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );
	if ( regen && IsWorkZone() )
		dispent->Flush();

	CDbEntity::Describe2d( regen, io_tooltip, in_tolerance );
	DescribeAccessories( regen, io_tooltip, in_tolerance );
}

void
CDbFeature::DescribeAccessories( 
	int			regen,
	C3dCoord*	io_tooltip,		// In world... internal special
	double		tolerance ) const
{
	if ( ZERO(tolerance) )
		return;  // Nothing to do.

	CString	type;
	int		hold, clamp;
	int		color, style, erase;
	int		clampzone, zone;

	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );

	//
	// Entities may carry attributes that describe machine events or objects.
	// Here we must find and draw those attributes.
	// This replaces our earlier job or reading commands.  Commands now take care
	// of themselves.
	//
	type = m_attribs.getString( STR_TYPE, "none" );
	hold = m_attribs.getInt( "_hold", IUNDEFINED );
	clamp = m_attribs.getInt( "_clamp_num", 0 );

	clampzone = m_attribs.getInt( "_clamp_zone", 0 ) != 0;
	zone = (type.CompareNoCase("_zone") == 0);

	erase = IntGet( "Erase", 0 );

	// -----------------------------------------

	if ( zone || clamp || (hold < IUNDEFINED) )
	{
		if (IsSelected() && !erase)
		{
			color = DCOLOR_SELECT;
			style = DSTYLE_SELECT;
		}
		else
		{
			// If zone_num is less-than zero then something is *very* wrong.
			int zone_num = IntGet( "_zone_num", 1 ) - 1;
			color = g_zone_color[zone_num%5];

			// I'm not real keen on this, but it does force the workzones
			// to be drawn with correct color in the entity list.
			((CDbFeature*) this)->ColorSet( color );
			style = DSTYLE_SOLID;
		}

		dispent->CommandAppend( DCMD_COLOR, (DWORD) color );
	}

	// -----------------------------------------

	if (zone)
	{
		// 2007.03.05 (PE) -- Prior to adding this style command here,
		// a zone would appear to be selected even though it was not.
		dispent->CommandAppend( DCMD_STYLE, (DWORD) style );
		DescribeZone( regen, io_tooltip, tolerance );
		// io_cmd->Add( CDisplayEntity::Command( DCMD_STYLE, (DWORD)style ) );
	}

	// -----------------------------------------

	if (hold < IUNDEFINED)
	{
		// color is fetch in earlier conditional block.
		DescribeHold( regen, hold, io_tooltip, tolerance );

		if (!clamp && !erase)
			dispent->CommandAppend( DCMD_COLOR, (DWORD) color );
	}

	// -----------------------------------------

	if (clamp > 0)
	{
		// color is fetch in earlier conditional block.
		if (clampzone)
			dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_DASH );

		DescribeClamp( regen, clamp, io_tooltip, tolerance );

		// color is fetch in earlier conditional block.
		if (clampzone)
			dispent->CommandAppend( DCMD_STYLE, (DWORD) style );
	}
}

void
CDbFeature::DescribeZone(
	int				regen,
	C3dCoord*		io_tooltip,
	double			tolerance ) const
{
	int	indx;

	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );

//	io_cmd->Add( CDisplayEntity::Command( DCMD_STYLE, (DWORD)DSTYLE_CENTER ) );
	dispent->CommandAppend( DCMD_META, (DWORD) META_ZONE );

	C3dCoord tl( m_attribs.getReal( "_zone_left", 0.0 ),
				 m_attribs.getReal( "_zone_top", 0.0 ), 
				 0.0 );
	C3dCoord br( m_attribs.getReal( "_zone_right", 0.0 ),
				 m_attribs.getReal( "_zone_bottom", 0.0 ), 
				 0.0 );

	int is_clamp_zone = m_attribs.getInt( "_clamp_zone", 0 );
	if ( is_clamp_zone )
		dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_DASH );

	C3dCoord pt( tl.X(), tl.Y(), 0. );
	indx = dispent->CoordAppend( pt );
	dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

	pt.X(br.X());
	indx = dispent->CoordAppend( pt );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

	pt.Y(br.Y());
	indx = dispent->CoordAppend( pt );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

	pt.X(tl.X());
	indx = dispent->CoordAppend( pt );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

	pt.Y(tl.Y());
	indx = dispent->CoordAppend( pt );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

	if ( is_clamp_zone )
		dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_SOLID );

	DescribeTarget( regen, "(zone)", &pt, tolerance );

	dispent->CommandAppend( DCMD_META, (DWORD) META_NONE );
}

void CDbFeature::DescribeClamp(
	int				regen,
	int				clamp,
	C3dCoord*		io_tooltip,
	double			tolerance ) const
{
	CString		xname;
	CString		yname;
	CString		cstr;
	C3dCoord	pt;
	int			indx;

	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );

	const C3x4Matrix& xform = m_workplane->Transform();

	if ( g_clamp_shape.Count() < 2)
		ClampInitDefault();

	for (indx = 1; indx <= clamp; ++indx)
	{
		dispent->CommandAppend( DCMD_META, (DWORD) META_ZONE );

		xname.Format( "_clamp%d_x", indx );
		yname.Format( "_clamp%d_y", indx );

		pt.XYZ( m_attribs.getReal( xname, 0. ), 0., 0. );

		draw_2d( g_clamp_shape, dispent->CommandList(),
			dispent->WorldList(), xform, tolerance, pt, 0., false );

		cstr.Format( "(clamp%d)", indx);
		pt.Y( m_attribs.getReal( yname, 0. ) );
		DescribeTarget( regen, cstr, &pt, tolerance );
	}

	dispent->CommandAppend( DCMD_META, (DWORD) META_NONE );
}

// Pilfered (essentially) from CDbFeature::DescribeClamp().
void CDbFeature::WriteClampsToFile( const CString& path ) const
{
	FILE* f = fopen( path, "a+" );
	if (f != NULL)
	{
		CString xname, yname;
		C3dCoord origin, world;

		CDbWorkplane* workplane = m_workplane;
		if (workplane == NULL)
			workplane = GetAt(0)->Workplane();

		const C3x4Matrix& xform = workplane->Transform();
	
		if ( g_clamp_shape.Count() < 2)
			ClampInitDefault();

		int clamp_count = m_attribs.getInt( "_clamp_num", 0 );
		fprintf( f, "%d\n", clamp_count );

		for (int indx = 1; indx <= clamp_count; ++indx)
		{
			xname.Format( "_clamp%d_x", indx );
			yname.Format( "_clamp%d_y", indx );

			origin.XYZ( m_attribs.getReal( xname, 0. ), 0., 0. );

			{

				// local.setXYAngle( radians );

				int pt_count = g_clamp_shape.Count();
				fprintf( f, "%d\n", pt_count );

				// V16.5, increased max from 32 to 1024 (for custom tools, like cluster punches)
				// Also capped upper bound because overrun would cause really hard crash.
				if (pt_count > MAX_TOOL_PNT)
					pt_count = MAX_TOOL_PNT;

				for (int pndx = 0; pndx < pt_count; ++pndx)
				{
					// Really a 2D coordinate, but doesn't matter.
					C3dCoord pt = g_clamp_shape.Point( pndx );

					// Translate the tool so its origin coincides with the given point.
					pt += origin;

					// Transform to world coordinates.
					xform.TransformTo( pt, &world );

					fprintf( f, "%d %f %f\n", g_clamp_shape.Type( pndx ), world.X(), world.Y() );
				}
			}
		}

		fclose( f );
	}
}

// Introduced in support of CModelProcessApp::ModelMachineUpdate()
// TODO: What a mess, consolidate hold_down update/rendering/etc.
void
CDbFeature::HoldDownUpdate( const CVarList& model_header )
{
	CVarList*	attribs;
	double		hold_x1, hold_x2;
	double		hold_y, hold_dia;
	int			hold_type;

	attribs = pAttrib();

	hold_type = model_header.getInt( "hold_type", (int) UNDEFINED );
	if ((hold_type == HOLD_2CIRCLE) || (hold_type == HOLD_RECTANGLE))
	{
		hold_x1  = model_header.getReal( "hold_x1", UNDEFINED );
		hold_x2  = model_header.getReal( "hold_x2", UNDEFINED );
		hold_y   = model_header.getReal( "hold_y", UNDEFINED );
		hold_dia = model_header.getReal( "hold_dia", UNDEFINED );

		if ((hold_x1 < UNDEFINED) &&
			(hold_x2 < UNDEFINED) &&
			(hold_y < UNDEFINED) &&
			(hold_dia < UNDEFINED))
		{
			attribs->setReal( "_hold", hold_type );

			if (hold_type == HOLD_2CIRCLE)
			{
				attribs->setReal( "_hold_dia", hold_dia );
				attribs->setReal( "_hold_c1dx", hold_x1 );
				attribs->setReal( "_hold_c2dx", hold_x2 );
				attribs->setReal( "_hold_c1dy", hold_y );
				attribs->setReal( "_hold_c2dy", hold_y );
			}
			else if (hold_type == HOLD_RECTANGLE)
			{
				attribs->setReal( "_hold_tldx", hold_x1 );
				attribs->setReal( "_hold_brdx", hold_x2 );
				// NOTE: hold_y in the model header represents the Y offset.
				attribs->setReal( "_hold_tldy", (hold_y + (0.5 * -hold_dia)) );
				attribs->setReal( "_hold_brdy", (hold_y + (0.5 * hold_dia)) );
			}
		}
	}
}

void
CDbFeature::DescribeHold( 	
	int				regen,
	int				hold,
	C3dCoord*		io_tooltip,
	double			tolerance ) const
{
	CString		display;
	C3dCoord	pt;
	double		x_hold, y_hold;
	double		offset, repo;

	// const C3x4Matrix&	xform = m_workplane->Transform();
	C3x4Matrix	xform;


	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );

	offset = m_attribs.getReal( "_zone_left", 0. );
	x_hold = m_attribs.getReal( "_hold_x", 0. );
	y_hold = m_attribs.getReal( "_hold_y", 0. );
	repo = m_attribs.getReal( "_zone_repo", 0. );

	if ( g_hold_shape.Count() < 2)
		HoldInitDefault();

	dispent->CommandAppend( DCMD_META, (DWORD) META_ZONE );

#if BEFORE_V18_0
	pt.XYZ( (x_hold + offset), y_hold, 0. );
#else
	// 2006.09.27 (PE) -- According to conversation with Gary,
	// the coordinate of a hold position is treated just like
	// the coordinate of a hole. This kinda makes sense because
	// the hold down mechanism typically travels with the head.
	// So the previous implementation was wrong (for a very long time).
#endif
	pt.XYZ( x_hold, y_hold, 0. );
	draw_2d( g_hold_shape, dispent->CommandList(),
		dispent->WorldList(), xform, tolerance, pt, 0., false );

	display.Format( "REPO(%f)", (double)((int)(repo*100.0))/100.0 );
	DescribeTarget( regen, display, &pt, tolerance );

	dispent->CommandAppend( DCMD_META, (DWORD) META_NONE );
}

void
CDbFeature::Workplane( CDbWorkplane* workplane )
{
	CDbEntity::TempWorkplane( workplane );
}

// static
CReturn
CDbFeature::ClampInitFromCTG( const CString& ctg_path )
{
	CReturn	status;

	g_clamp_shape.Invalidate();

	if ( !ctg_path.IsEmpty() )
		status = g_clamp_shape.InitFromCTG( ctg_path );

	return status;
}

// static
CReturn
CDbFeature::HoldInitFromCTG( const CString& ctg_path )
{
	CReturn	status;

	HoldDownInvalidate();

	if ( !ctg_path.IsEmpty() )
		status = g_hold_shape.InitFromCTG( ctg_path );

	return status;
}

// static
void
CDbFeature::HoldDownInvalidate()
{
	g_hold_shape.Invalidate();
}

// static
const CToolShape&
CDbFeature::HoldDownShape()
{
	return g_hold_shape;
}

void
CDbFeature::ClampInitDefault() const
{
	CArray<C3dCoord, C3dCoord> pts;
	CArray<int,int> types;
	C3dCoord pt;

	double xll = m_attribs.getReal( "_clamp_lldx", 0. );
	double yll = m_attribs.getReal( "_clamp_lldy", 0. );
	double xur = m_attribs.getReal( "_clamp_trdx", 0. );
	double yur = m_attribs.getReal( "_clamp_trdy", 0. );

	g_clamp_shape.Invalidate();

	pt.XYZ( xll, yll, 0. );
	types.Add( 0 );
	pts.Add( pt );

	pt.X( xur );
	types.Add( 1 );
	pts.Add( pt );

	pt.Y( yur );
	types.Add( 1 );
	pts.Add( pt );

	pt.X( xll );
	types.Add( 1 );
	pts.Add( pt );

	pt.Y( yll );
	types.Add( 1 );
	pts.Add( pt );

	g_clamp_shape.Init( types, pts );
}

void
CDbFeature::HoldInitDefault() const
{
	CArray<C3dCoord, C3dCoord> pts;
	CArray<int,int> types;
	CString		xname;
	CString		yname;
	C3dCoord	pt;
	double		xul, yul;
	double		xbr, ybr;
	double		xc, yc;
	double		radius;
	int			hold_type, indx;
	
	hold_type = m_attribs.getInt( "_hold", IUNDEFINED );

	HoldDownInvalidate();

	switch (hold_type)
	{
	case HOLD_2CIRCLE:

		radius = 0.5 * m_attribs.getReal( "_hold_dia", 0. );

		for (indx = 1; indx <= 2; ++indx)
		{
			xname.Format( "_hold_c%ddx", indx );
			yname.Format( "_hold_c%ddy", indx );

			xc = m_attribs.getReal( xname, 0. );
			yc = m_attribs.getReal( yname, 0. );

			types.Add( 0 );
			pt.XYZ( (xc + radius), yc, 0. );
			pts.Add( pt );

			types.Add( 2 );
			pt.XYZ( xc, yc, 0. );
			pts.Add( pt );

			types.Add( 1 );
			pt.XYZ( (xc + radius), yc, 0. );
			pts.Add( pt );
		}

		g_hold_shape.Init( types, pts );

		break;

	case HOLD_RECTANGLE:

		xul = m_attribs.getReal( "_hold_tldx", 0. );
		yul = m_attribs.getReal( "_hold_tldy", 0. );
		xbr = m_attribs.getReal( "_hold_brdx", 0. );
		ybr = m_attribs.getReal( "_hold_brdy", 0. );

		types.Add( 0 );
		pt.XYZ( xul, yul, 0. );
		pts.Add( pt );

		types.Add( 1 );
		pt.Y( ybr );
		pts.Add( pt );

		types.Add( 1 );
		pt.X( xbr );
		pts.Add( pt );

		types.Add( 1 );
		pt.Y( yul );
		pts.Add( pt );

		types.Add( 1 );
		pt.X( xul );
		pts.Add( pt );

		g_hold_shape.Init( types, pts );

		break;
	}
}

