
#include "stdafx.h"
#include "StringConst.h"
#include "3dBox.h"
#include "3x4Matrix.h"
#include "3dCoord.h"
#include "GeoPoint.h"
#include "GeoArc.h"
#include "Command.h"
#include "Register.h"

#include "EntityDb.h"
#include "DbWorkplane.h"
#include "DbTool.h"
#include "DbCommand.h"
#include "DbPattern.h"
#include "DisplayEntity.h"
#include "DbContainer.h"
#include "EntityDb.h"
#include "DbEntityVisitor.h"


////////////////////////////////////////////////////////////////////////

CDbCommand::CDbCommand( CEntityDb* db )
	: CDbEntity( db ),
	  m_pt(),
	  m_text(),
	  m_cmdstr()
{
	// Copy the default attributes from the database.
	// See also comment in CDbEntity::CDbEntity()
	(*pAttrib()) = CDbEntity::Db()->Default();
	m_target_color = DCOLOR_WHITE;
}

CDbCommand::CDbCommand( const CDbCommand& dbCommand )
	: CDbEntity( dbCommand ),
	  m_pt( dbCommand.m_pt ),
	  m_text( dbCommand.m_text ),
	  m_cmdstr( dbCommand.m_cmdstr )
{
	m_target_color = DCOLOR_WHITE;
}

CDbCommand::~CDbCommand()
{
	if ( CDbEntity::IsReferencing() )
		CDbEntity::Remove( (*this) );
}

void
CDbCommand::RefsTo( CDbEntityList* list ) const
{
	// Do nothing.  A command does not reference any entities.
}

void
CDbCommand::Subordinates( CDbEntityList* list ) const
{
	// Do nothing.  A command does not have any subordinate entities.
}

bool
CDbCommand::HasRefTo( const CDbEntity* refdEntity ) const
{
	if ( CDbEntity::HasRefTo( refdEntity ) )
		return TRUE;

	return FALSE;
}

void
CDbCommand::Delete()
{
	if ( CDbEntity::IsDeleted() )
		return;

	CDbEntity::DeleteFlag( true );
	CDbEntity::Record();

	// Divorce this entity from its owner.
	CDbContainer* dbContainer = dynamic_cast<CDbContainer*>( CDbEntity::Owner() );
	if (dbContainer != NULL)
		dbContainer->Disown( this );

	CDbSequence* dbSequence = CDbEntity::Sequence();
	if (dbSequence != NULL)
		this->Sequence( NULL );

	// Sever the ties between all other entities and this entity.
	CDbEntity::RemoveRefs();

	CDbEntity::Workplane( NULL );
	CDbEntity::Tool( NULL );
}

void
CDbCommand::Accept( CDbEntityVisitor* visitor )
{
	visitor->Visit( this );
}

bool
CDbCommand::IsInsert() const
{
	CString	type = StringGet( STR_TYPE, "" );
	return (type.CompareNoCase("_insert") == 0);
}

bool
CDbCommand::IsClamp() const
{
	return IsA("CLAMP");
//	return (m_text.Left(6).CompareNoCase("@CLAMP") == 0);
}

bool
CDbCommand::IsInstance() const
{
	return IsA("INSTANCE");
//	return (m_text.Left(9).CompareNoCase("@INSTANCE") == 0);
}

bool
CDbCommand::IsTooledText() const
{
	CDbTool* dbTool = Tool();

	return ((dbTool == NULL || dbTool->IsLayer()) ? FALSE : TRUE);
}

bool
CDbCommand::IsA( CString cmd ) const
{
	return (m_cmdstr.Left(cmd.GetLength()).CompareNoCase(cmd) == 0);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void
CDbCommand::RemoveRef( CDbEntity* dbEntity )
{
	// Nothing to do.  Commands don't reference anything.
	return;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const CDbCommand&
CDbCommand::operator = ( const CDbCommand& dbCommand )
{
	CDbEntity::CommonCopy( dbCommand );

	m_pt = dbCommand.m_pt;
	m_text = dbCommand.m_text;

	return (*this);
}

CReturn
CDbCommand::Clone( CDbEntity** dbEntity ) const
{
	CReturn status;

	CDbCommand* dbCommand = new CDbCommand( (*this) );

	(*dbEntity) = dbCommand;

	return status;
}

CReturn
CDbCommand::ContentsSwap( CDbEntity* dbEntity )
{
	CReturn status;

	CDbCommand* dbCommand = dynamic_cast<CDbCommand*>( dbEntity );

	ASSERT( (dbCommand != NULL) );

	CDbCommand tmp( (*this) );
	(*this) = (*dbCommand);
	(*dbCommand) = tmp;

	// Suppress reference count modifications.
	tmp.ReferenceFlagClear();

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
CReturn 
CDbCommand::CopyTo( 
	CEntityDb*		io_dest,
	tDbEntityMap*	io_refmap, 
	CDbEntity**		dbEntity )
{
	CReturn		status;
	//
	// Generic Copy... sets up the database and maps...
	//
	status += CDbEntity::CopyTo( io_dest, io_refmap, dbEntity );
	CDbCommand* dbCommand = (CDbCommand*)(*dbEntity);

	dbCommand->Init( dbCommand->Tool(), dbCommand->Workplane(), m_pt, m_text );

	return status;
}



C3dBox
CDbCommand::Box( ID workplaneId ) const
{
	if ( CDbEntity::IsDeleted() )
		return C3dBox();

// TODO:  box the actual text displayed?

	C3dBox box;

/*
	if (IsInstance())
	{
		// Box the pattern and shift it
		//
		ID patID = IntGet( "patid", 0 );

		CDbEntity*	dbPattern;
		((CDbCommand*) this)->Db()->Find( patID, &dbPattern, DBPATTERN, DBPATTERN );
		if (dbPattern != NULL)
		{
			box = dbPattern->Box(workplaneId);
			C3dCoord pt = Coord( workplaneId );

			box += pt;

		}
	}
	else
*/
	{
		C3dCoord pt = Coord( workplaneId );
		CGeoPoint geoPt( pt );
		box = geoPt.Box();
	}
	return box;
}

C3dBox
CDbCommand::RealBox( ID workplaneID ) const
{
	C3dBox	box;

	// Of course, we could probably extend this to
	// other things like clamps and drop doors ....
	if ( !CDbEntity::IsDeleted() && IsInstance() )
	{
		CDbPattern*	dbPattern;
		C3dCoord	pt;

		// Box the pattern and shift it
		ID pat_id = IntGet( "patid", 0 );

		((CDbCommand*) this)->Db()->Find(
			pat_id, (CDbEntity**) &dbPattern, DBPATTERN, DBPATTERN );

		if (dbPattern != NULL)
		{
			box = dbPattern->Box( workplaneID );

			pt = Coord( workplaneID );

			box.Shift( C3dVec( pt.X(), pt.Y(), 0. ) );
		}
	}

	return box;
}

void
CDbCommand::Init( 
	CDbTool*			tool, 
	CDbWorkplane*		workplane, 
	const C3dCoord&		pt, 
	const CString&		text )
{
	CDbEntity::Record();
	CDbEntity::CreateFlag( false );

	CDbEntity::Tool( tool );
	CDbEntity::Workplane( workplane );

	Text( text );

	m_pt = pt;
}

void
CDbCommand::Init( 
	CDbTool* tool, 
	CDbWorkplane* workplane, 
	double x, double y, double z,
	const CString& text )
{
	C3dCoord pt( x, y, z );

	Init( tool, workplane, pt, text );
}

CGeoPoint
CDbCommand::Point() const
{
	CGeoPoint pt( m_pt );
	return pt;
}

void
CDbCommand::Text( const CString& text)
{
	m_text = text;
	m_cmdstr.Empty();

	// Conditionally move specially formatted information
	// to the attribute list of this entity. For example,
	// "@INSTANCE: instang=0.0, patid=36, repo=1" results
	// in the addition/update of the three attributes
	// instang, patid and repo.
	if ((text.GetLength() > 1) && (text[0] == '@'))
	{
		CCommand cmd;

		cmd.setCommand( text.Mid(1), NULL, NULL );
		cmd.nextRoute( &m_cmdstr );

		(*pAttrib()) += cmd.VarList();
	}

	int mysize = IntGet("label_size", IUNDEFINED);
	if (mysize == IUNDEFINED)
	{ 
		// Only force size if not already defined...
		int size = CRegister::IntGetV( "Preferences\\ViewOptions", "LabelSize", 10 );

		IntSet("label_size", size);
	}
}


C3dCoord
CDbCommand::Coord( ID workplaneId ) const
{
	C3dCoord result;

	if (workplaneId == ~0 || workplaneId == CDbEntity::WorkplaneId())
	{
		// No transformation required.
		result = m_pt;
	}
	else
	{
		// Transform to global coordinates.
		const CDbWorkplane* dbWorkplane = CDbEntity::Workplane();
		const C3x4Matrix& xform = dbWorkplane->Transform();
		xform.TransformTo( m_pt, &result );

		if (workplaneId > 0)
		{
			// Transform back to the specified workplane.
			CDbEntity::Find( workplaneId, (CDbEntity**) &dbWorkplane, DBWORKPLANE, DBWORKPLANE );
			ASSERT( (dbWorkplane != NULL) );
			const C3x4Matrix& inverse = dbWorkplane->Inverse();
			inverse.TransformTo( result, &result );
		}
	}

	return result;
}

// See also notes in header per this method.
C3dCoord
CDbCommand::Coord( const C3dCoord& pt )
{
	C3dCoord curr = m_pt;
	m_pt = pt;
	return curr;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//		Describe2d
//
//	Describe yourself, using lines.
//	See CDisplayEntity for details...
//
void
CDbCommand::Describe2d( 
	int				regen,
	C3dCoord*		io_tooltip,
	double			tolerance ) const
{
	if ( is_hidden() )
		return;  // Nothing to do.

	const CDbWorkplane* dbWorkplane = CDbEntity::Workplane();
	if (!dbWorkplane)
		return;

	bool allocated;
	CDisplayEntity* dispent = DisplayEntityGet( &allocated );

	// We always want to regenerate the display list
	// because a command is not a "scalable" entity.
	dispent->Flush();


	CString		display;
	C3dCoord	pt;
	C3dCoord	tip;
	DWORD		color;
	int			indx;

	double	space = tolerance * 2;
	double	mark = tolerance * 4;
	double	angle = 0.;

	const C3x4Matrix& xform = dbWorkplane->Transform();

	pt = Coord();
	xform.TransformTo( pt, &tip );

	if ((tip.X() >= LARGE) || (tip.Y() >= LARGE))
		return;

	if ( IsSelected() )
		color = DCOLOR_SELECT;
	else
		color = (IsTooledText() ? ColorGet( m_target_color ) : m_target_color);

	dispent->CommandAppend( DCMD_COLOR, color );

	CDbEntity::DescribeTarget( regen, "", &tip, tolerance );

	if (IsSelected())
	{
		dispent->CommandAppend( DCMD_COLOR, (DWORD) DCOLOR_SELECT );

		if (Flags() & DBSHOWPATH)
			dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_SOLID );
	}
	else
	{
		color = (DWORD) ColorGet( DCOLOR_BLUE );
		dispent->CommandAppend( DCMD_STYLE, (DWORD) DSTYLE_SOLID );
	}

	if ( IsInstance() )
	{
		ID patID = IntGet( "patid", 0 );

		// cast away const.
		CDbPattern*	dbPattern;
		((CDbCommand*) this)->Db()->Find(
			patID, (CDbEntity**) &dbPattern, DBPATTERN, DBPATTERN );

		if (dbPattern != NULL)
		{
			dbPattern->Describe2d( regen, &tip, tolerance, dispent );
		}
	}
	else
	{
		bool okay = CDbEntity::CanDrawLegend();
		if ( !okay )
		{
			// Regular command entities have a "pos" attribute
			// but a "nesting legend" does not.
			okay = (IntGet( "pos", IUNDEFINED ) != IUNDEFINED);
		}

		if ( okay )
		{
			display = m_text;
			CDbEntity::Describe2d( regen, &tip, tolerance );
			if (display.GetLength() > 0)
				angle = DoubleGet( "angle", 0. );
		}
	}

	// The text
	if (display.GetLength() > 0)
	{
		eDisplayTextPos pos = (eDisplayTextPos)IntGet( "pos", 0 );
		int size = IntGet("label_size", 10);

		dispent->CommandAppend( DCMD_TEXTPOS, (DWORD) pos );
		dispent->CommandAppend( DCMD_TEXTANG, (float) angle );
		dispent->CommandAppend( DCMD_TEXTSIZE, (DWORD) size );

		indx = dispent->CoordAppend( tip );
		dispent->CommandAppend( DCMD_TEXT, (DWORD) indx );
		int num = CDisplayEntity::CommandTextNum( display );
		for (int idx=0; idx<num; idx++)
		{
			dispent->CommandAppend( display, idx );
		}
	}
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//		Transform
//
void
CDbCommand::Transform( 
	const C3x4Matrix&	in_xform )
{
	if (DidAction())
		return;

	CDbEntity::Transform( in_xform );


	double angle = DoubleGet( "angle", 0. );
	C2dUnitVec angvec( DEG2RAD*angle );
	C3dVec xformvec( angvec.X(), angvec.Y(), 0. );

	in_xform.Transform( &xformvec );
	angvec = xformvec;
	angle = angvec.Radians();
	DoubleSet( "angle", RAD2DEG*angle );


	in_xform.Transform( &m_pt );
}

CString
CDbCommand::InstanceLabel() const
{
	CString label;

	if ( IsInstance() )
	{
		ID patID = IntGet( "patid", 0 );

		// cast away const.
		CDbEntity*	dbPattern;
		((CDbCommand*) this)->Db()->Find( patID, &dbPattern, DBPATTERN, DBPATTERN );
		if (dbPattern != NULL)
			label = dbPattern->StringGet( "_label", "" );
	}

	return label;
}


DWORD
CDbCommand::TargetColor( DWORD color )
{
	DWORD prev_color = m_target_color;
	m_target_color = color;
	return prev_color;
}
