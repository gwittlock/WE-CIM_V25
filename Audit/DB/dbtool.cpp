
#include "stdafx.h"
#include "Register.h"
#include "EntityDb.h"
#include "MathConst.h"
#include "StringConst.h"

#include "GeoArc.h"
#include "GeoLine.h"
#include "GeoPoly.h"
#include "DbWorkplane.h"

#include "Solution.h"

#include "DbTool.h"
#include "DbEntityVisitor.h"
#include "DisplayEntity.h"


////////////////////////////////////////////////////////////////////////

CDbTool::CDbTool( CEntityDb* db )
	: CDbEntity( db )
{
	// Copy the default attributes from the database.
	// See also comment in CDbEntity::CDbEntity()
	(*pAttrib()) = CDbEntity::Db()->Default();

	CDbEntity::CreateFlag( false );

	// By default, all entities support snap & hotdot view behavior.
	CDbEntity::SnappableFlag( true );
	CDbEntity::HotDotFlag( true );
}

CDbTool::CDbTool( const CDbTool& dbTool )
	: CDbEntity( dbTool )
{
	CDbEntity::CreateFlag( false );
}

CDbTool::~CDbTool()
{
	if ( CDbEntity::IsReferencing() )
		CDbEntity::Remove( (*this) );
}

// 2004.08.11 (PE) -- Introduced for Sunflower so that a macro
// could be written that place a custom tool by matching the
// lower-left corner of the tool's  bounding box with the
// lower-left corner of a shape's bounding box.
C3dBox
CDbTool::Box( ID workplaneId ) const
{
	if ( !m_shape.IsInited() )
		((CDbTool*) this)->m_shape.Init( Attrib() );

	if ( !m_box_cache.IsDefinedXY() )
	{
		CGeoPoly geoPoly;
		C2dBox box2d;

		m_shape.Convert( &geoPoly );

		((CDbTool*) this)->m_box_cache = geoPoly.Extent();
	}

	return m_box_cache;
}

void
CDbTool::RefsTo( CDbEntityList* list ) const
{
	// Do nothing.  A tool does not reference any entities.
}

void
CDbTool::Subordinates( CDbEntityList* list ) const
{
	// Do nothing.  A tool does not have any subordinate entities.
}

bool
CDbTool::HasRefTo( const CDbEntity* refdEntity ) const
{
	// A tool never references another entity.
	return FALSE;
}

void
CDbTool::Delete()
{
	if ( CDbEntity::IsDeleted() )
		return;

	CDbEntity::DeleteFlag( true );
	CDbEntity::Record();

	CDbEntity::RemoveRefs();
}

void
CDbTool::Accept( CDbEntityVisitor* visitor )
{
	visitor->Visit( this );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void
CDbTool::RemoveRef( CDbEntity* dbEntity )
{
	// Nothing to do.  A tool never references another entity.
	return;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

const CDbTool&
CDbTool::operator = ( const CDbTool& dbTool )
{
	CDbEntity::CommonCopy( dbTool );

	return (*this);
}

CReturn
CDbTool::Clone( CDbEntity** dbEntity ) const
{
	CReturn status;

	CDbTool* dbTool = new CDbTool( (*this) );

	(*dbEntity) = dbTool;

	return status;
}

CReturn
CDbTool::ContentsSwap( CDbEntity* dbEntity )
{
	CReturn status;

	CDbTool* dbTool = dynamic_cast<CDbTool*>( dbEntity );

	ASSERT( (dbTool != NULL) );

	CDbTool tmp( (*this) );
	(*this) = (*dbTool);
	(*dbTool) = tmp;

	// Suppress reference count modifications.
	tmp.ReferenceFlagClear();

	return status;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
CReturn 
CDbTool::CopyTo( 
	CEntityDb*		io_dest,
	tDbEntityMap*	io_refmap, 
	CDbEntity**		dbEntity )
{
	CReturn status;

	//
	// Generic Copy... sets up the database and maps...
	//
	status += CDbEntity::CopyTo( io_dest, io_refmap, dbEntity );
	CDbTool* dbTool = (CDbTool*)(*dbEntity);

	//
	// References and specific data ...
	//
//	status += dbTool->Init( Layer(), Workplane(), ... );

	return status;

}




//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void 
CDbTool::Describe2d( 
	int					regen, 
	const C3x4Matrix&	in_ref2world,			// Local plane of reference
	const C3dCoord&		in_localtip,			// Tooltip, in LOCAL REF!!!
	const C2dUnitVec&	in_tangent,				// Tangent along entity...
	bool				in_codePartProf,
	int					in_cutside,
	double				in_tolerance,
	CDisplayEntity*		dispent ) const
{
	if ( !IsLayer() )
	{
		C3dCoord	pc;

		int type_id = IntGet( STR_TYPE_ID, TTYPE_NONE );

		// Radius, used in round tools to define the circle, and
		// in all other tools to determine offset distance (if needed)
		//
		double radius;
		if ( IsRoundTool() 
			|| (type_id == TTYPE_SINGLE_D)
			|| (type_id == TTYPE_DOUBLE_D)
			|| (type_id == TTYPE_KEYHOLE)
			|| (type_id == TTYPE_HEXAGON) )
		{
			radius = EffectiveDiameter() / 2.0;
		}
		else
		{
			double width = 5.0;
			Attrib().getReal( STR_WIDTH, &width);
			radius = width / 2.0;
		}

		// Find center to draw tool at... perp is also used for the cutside arrow
		//
		C2dUnitVec perp( -in_tangent.Y(), in_tangent.X() );
		C2dUnitVec normal = perp * in_cutside;

		pc = in_localtip;
		if ( in_codePartProf )
			pc += (C3dVec( normal.X(), normal.Y(), 0.0 ) * radius);

		// Draw the marker and tool!
		//
		// NOTE:  BY USING SYSTEM LINES to draw tools, we get two effects:
		//	1) The mouse does not snap to the tooling, and this is desired
		//	2) The tool markers disappear when view modes turn off profile markers,
		//			a possibly undesireable side-effect.  23 Mar 00 eww
		//
		//	io_cmd->Add( CDisplayEntity::Command( DCMD_START, (DWORD) 0 ) );

		if ( in_cutside )
		{
			CutsideIndicator( regen, in_ref2world, pc,
				normal, radius, dispent );
		}

		if ( !m_shape.IsInited() )
			((CDbTool*) this)->m_shape.Init( Attrib() );

		bool fill = (dispent->Entity()->IsFilled() != 0);
		CDbEntity::draw_2d( m_shape, dispent->CommandList(), dispent->WorldList(),
			in_ref2world,	in_tolerance, pc, IndexAngle( in_tangent ), fill );
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//           \   left leg
//      o-----> central leg
//           /   right leg
void 
CDbTool::CutsideIndicator(
	int					regen,
	const C3x4Matrix&	in_ref2world,			// Local plane of reference
	const C3dCoord&		in_pc,
	const C2dUnitVec&	in_normal,				// Normal away from entity 
	double				in_radius,
	CDisplayEntity*		dispent ) const
{
	C2dUnitVec vec;
	C3dCoord pe, p1, p2;
	C3dCoord pc = in_pc;
	int		indx;

	// End point of central leg.
	pe = pc + (C3dVec( in_normal.X(), in_normal.Y(), 0.0 ) * in_radius);
	in_radius /= 2;

	// End point of left leg.
	vec = in_normal + ( PI * 7 / 8 );
	p1 = pe + (vec * in_radius);

	// End point of right leg.
	vec = in_normal - ( PI * 7 / 8 );
	p2 = pe + (vec * in_radius);

	in_ref2world.Transform( &pc );
	in_ref2world.Transform( &pe );
	in_ref2world.Transform( &p1 );
	in_ref2world.Transform( &p2 );

	dispent->CommandAppend( DCMD_META, (DWORD) META_TOOL );

	indx = dispent->CoordAppend( pc );
	dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

	indx = dispent->CoordAppend( pe );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

	indx = dispent->CoordAppend( p1 );
	dispent->CommandAppend( DCMD_MOVETO, (DWORD) indx );

	indx = dispent->CoordAppend( pe );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

	indx = dispent->CoordAppend( p2 );
	dispent->CommandAppend( DCMD_LINETO, (DWORD) indx );

	dispent->CommandAppend( DCMD_META, (DWORD) META_NONE );
}

C3dVec
CDbTool::AggregateOffset( void )
{
	if (agg_off.X() == UNDEFINED)
	{
		double	offx = Attrib().getReal( STR_STATION_LOCATION_X, 0. );
		double	offy = Attrib().getReal( STR_STATION_LOCATION_Y, 0. );
		
		agg_off = C3dVec( offx, offy, 0.0 );

		CDbWorkplane* top = NULL;
		Db()->Find( STR_TOP, (CDbEntity**)&top, DBWORKPLANE, DBWORKPLANE );

		if (top)
			top->Transform().Transform( &agg_off );
	}

	return agg_off;
}

double
CDbTool::EffectiveDiameter() const
{
	return ( m_shape.EffectiveDiameter( Attrib() ) );
}

double
CDbTool::EffectiveLength() const
{
	// TODO:  Merge with EffectiveDiameter???
	return ( m_shape.EffectiveLength( Attrib() ) );
}

bool
CDbTool::IsLayer() const
{
	int type_id = IntGet( STR_TYPE_ID, IUNDEFINED );
	return (type_id == IUNDEFINED);
}

bool
CDbTool::IsStockLayer() const
{
	return (IsLayer() && (Name().CompareNoCase("STOCK") == 0));
}

bool
CDbTool::IsMachineLayer() const
{
	return (IsLayer() && (Name().CompareNoCase("MACHINE") == 0));
}

bool
CDbTool::IsOpenStation() const
{
	int type_id = IntGet( STR_TYPE_ID, IUNDEFINED );
	return (type_id == TTYPE_OPEN );
}

bool
CDbTool::IsHoleTool() const
{
	return ( m_shape.IsHoleTool( Attrib() ) );
}

bool
CDbTool::IsRoundTool() const
{
	return ( m_shape.IsRoundTool( Attrib() ) );
}

bool
CDbTool::IsFormTool() const
{
	return ( m_shape.IsFormTool( Attrib() ) );
}

bool
CDbTool::IsIndexable() const
{
	return (IntGet( STR_AUTO_INDEX, 0 ) != 0);
}

// Convert the 2d representation of this tool to planar geometry.
void
CDbTool::Convert( CGeoCurveArray* geoCurves ) const
{
	if ( !m_shape.IsInited() )
		((CDbTool*) this)->m_shape.Init( Attrib() );

	m_shape.Convert( geoCurves );
}

void
CDbTool::Convert( CGeoPoly* geoPoly ) const
{
	if ( !m_shape.IsInited() )
		((CDbTool*) this)->m_shape.Init( Attrib() );

	m_shape.Convert( geoPoly );
}


void
CDbTool::PointRepClear()
{
	m_shape.Invalidate();
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Determine the orientation at which the tool should be drawn.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// There are four cases to consider:
//    (where N/A means either "not available" or "not applicable")
//
// 1. A non-punching tool
//      (tool:auto_index=N/A, tool:index_angle=N/A,    entity:orient=N/A)
// 2. A fixed orientation punching tool
//      (tool:auto_index=0,   tool:index_angle=0..360, entity:orient=N/A)
// 3. An unconstrained indexable punching tool
//      (tool:auto_index=1,   tool:index_angle=N/A,    entity:orient=N/A)
// 4. A constrained indexable punching tool
//      (tool:auto_index=1,   tool:index_angle=N/A,    entity:orient=0..360)
//
// In case 2, the tool orientation is represented by the "orient"
// attribute of the tool.
//
// In case 3, the tool orientation is determined by the tangent
// vector at the start point of the current geometric entity.
//
// In case 4, the tool orientation is represented by the transient
// attribute "~orient" which is set by the current geometric entity.
// This case is representative of entities that are created via
// AutoPunch().  The "~orient" attribute is used because the '~'
// character indicates the attribute should be not be written to
// the MM2 file.
//
double
CDbTool::IndexAngle( const C2dUnitVec& vec ) const
{
	bool isIndexable = FALSE;
	double orient = 0.;
	double indexAngle = UNDEFINED;

	if ( IsPunchTool() )
	{
		if ( IsIndexable() )
		{
			indexAngle = DoubleGet( "~orient", UNDEFINED );

			if (indexAngle < UNDEFINED)
			{
				// Case 4
				indexAngle *= DEG2RAD;
				((CDbTool*) this)->AttribDelete( "~orient" );
			}
			else
			{
				// Case 3
				indexAngle = vec.Radians();
			}
		}
		else
		{
			// Case 2
			// NOTE: Tool should be constructed with correct orientation!
			indexAngle = 0.;
			// indexAngle = Attrib().getReal( STR_INDEX_ANGLE, 0. ) * DEG2RAD;
		}
	}
	else
	{
		// Case 1
		indexAngle = vec.Radians();  // or maybe Attrib().getReal( STR_ORIENT, 0. ) ?
	}

	return indexAngle;
}

bool
CDbTool::IsPunchTool() const
{
	int	type_id = IntGet( STR_TYPE_ID, TTYPE_NONE );

	switch (type_id)
	{
	case TTYPE_ROUND:
	case TTYPE_SQUARE:
	case TTYPE_RECTANGLE:
	case TTYPE_OBROUND:
	case TTYPE_DIAMOND:
	case TTYPE_CORNER_RADIUS:
	case TTYPE_SINGLE_D:
	case TTYPE_DOUBLE_D:
	case TTYPE_KEYHOLE:
	case TTYPE_TRAPEZOID:
	case TTYPE_HEXAGON:
	case TTYPE_FORMING:
	case TTYPE_CUSTOM:
		return TRUE;
	}

	return FALSE;
}

bool
CDbTool::IsLeadTool() const
{
	if (IsPunchTool())
		return FALSE;

	if (IsHoleTool())
		return FALSE;

	// Another odd bag of unleadable tools...
	// TODO:  Reverse the logic, list leadable tools, and ditch the previous tests???
	int	type_id = IntGet( STR_TYPE_ID, TTYPE_NONE );
	switch (type_id)
	{
	case TTYPE_NONE:
	case TTYPE_DISC_SAW:
	case TTYPE_POWDER_MARK:
	case TTYPE_SCRIBE:
		return FALSE;
	}

	return TRUE;
}

bool
CDbTool::IsThruTool() const
{
	// Does this tool cut THROUGH the material?  Important for nesting

	int	type_id = IntGet( STR_TYPE_ID, TTYPE_NONE );
	switch (type_id)
	{
	case TTYPE_NONE:
	case TTYPE_POWDER_MARK:
	case TTYPE_SCRIBE:
		return FALSE;
	}

	return TRUE;
}

// 2005.01.25 (PE) -- See also other comments using same timestamp.
// Yields same result as IsLeadTool() (?)
bool
CDbTool::IsCuttingTool() const
{
	int	type_id = IntGet( STR_TYPE_ID, TTYPE_NONE );
	switch (type_id)
	{
	case TTYPE_BURNER:
	case TTYPE_LASER:
	case TTYPE_WATERJET:
		return TRUE;
	default:
		return FALSE;
	}
}

bool
CDbTool::IsGapTool() const
{
	return (IntGet( "gap_tool", FALSE ) != FALSE);
}

eToolSymmetry	
CDbTool::Symmetry() const
{
	if ( IsIndexable() || !IsPunchTool() )
		return TSYM_1;	// Auto-index; happy tool

	int type_id = IntGet( STR_TYPE_ID, TTYPE_NONE );
	switch (type_id)
	{
		case TTYPE_ROUND:
			return TSYM_1;

		case TTYPE_SQUARE:
		case TTYPE_CORNER_RADIUS:
			return TSYM_90;

		case TTYPE_RECTANGLE:
		case TTYPE_OBROUND:
		case TTYPE_DIAMOND:
		case TTYPE_DOUBLE_D:
		case TTYPE_TRAPEZOID:
			return TSYM_180;

		case TTYPE_SINGLE_D:
		case TTYPE_KEYHOLE:
		case TTYPE_HEXAGON:
		default:
			return TSYM_NONE;
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//	Matches
//
//	Score the match between two tools
//
//	Most attributes are *required* to match, but it may be possible
//	for there to be attributes that are optional (but preferred) matches,
//	or even attributes that are completely optional.  These three tiers
//	of attribute are listed in the attribute table at the head of the method.
//	Unfortunately, this is all hard-coded right now -- it may be beneficial
//	to find a way to embed this info with the tool itself, set from the DB.
//
//
//	USE:
//		*this is the fixed tool
//		The "tool" parameter is from a list being scored against
//	This user precedence is IMPORTANT when it comes to rotation tests.
//	It checks to see if the destination (tool) IS CAPABLE of rotating to 
//	match the range of the *this tool... if the order is reversed, we may
//	match a tool with full rotation range against a tool without it.
//
sMatchType g_matchlist[] =
{
	// Auto-index and angles are handled specially
	// Type is also special, as an integer
	// The rest are all doubles...
	{ "width", MATCH_CRIT },
	{ "length", MATCH_CRIT },
	{ "diameter", MATCH_CRIT },
	{ "corner_radius", MATCH_CRIT },
	{ "web", MATCH_CRIT },
	{ "radius", MATCH_CRIT },
	{ "angle", MATCH_CRIT },
	{ "major_diameter", MATCH_CRIT },
	{ "minor_diameter", MATCH_CRIT },

	{ "doff", MATCH_PREF },
	{ "loff", MATCH_PREF },
	{ "kerf", MATCH_PREF },

	{ NULL, MATCH_OPT }
};
//
//	
int
CDbTool::Matches( const CDbTool& list_tool ) const
{
	const int FAILURE = 0;
	const int SCORE_PREF = 5;
	const int SCORE_OPT = 1;
	const int SCORE_CRIT = 1;
	//
	// Layers will come back IUNDEFINED or TTYPE_NONE?
	// Open statios will come back TTYPE_OPEN
	// Tools will have a different TTYPE
	//
	int type_id = IntGet( STR_TYPE_ID, IUNDEFINED );
	if (type_id != list_tool.IntGet( STR_TYPE_ID, IUNDEFINED ))
		return FAILURE;

	if (IsOpenStation())
		return FAILURE;

	int	score = 1;		// A matching type is worth 1

	if (Name().CompareNoCase( list_tool.Name() ) == 0)
		++score;	// For layers, check the damned name too
	else
		--score;

	if ( IsLayer() )
		return score;

	if ( IsPhenolicProject() )
	{
		// 'NC_Code_Number' was added to address requirements of the
		// phenolic-board project related to ITI. In said case, the tool
		// setup can contain multiple tools having identical geometric
		// properties. Unlike standard nesting scenarios, however, we
		// need to maintain the unique tool assignments because the tool
		// order is leveraged during code generation. Specifically (at
		// first implementation anyways) the part has no closed profiles.
		// yet we must make the various cuts in the proper order.
		int itest = list_tool.IntGet( STR_NC_CODE_NUMBER, (int) UNDEFINED );
		int ival = IntGet( STR_NC_CODE_NUMBER, -1 );

		// Alternately, we could bail if no match?
		if (ival == itest)
			++score;
	}

	CReturn ret;
	CString note;
	if (CReturn::Debug()>=5)
	{
		note.Format( "--- %d (%s) -----------", list_tool.Id(), list_tool.StringGet(STR_DESCRIPTION, "") );
		ret.Diagnostic(note);
	}

	// First match up the attributes
	int indx = 0;
	while (TRUE)
	{
		sMatchType* test = &g_matchlist[indx];
		if (!test->m_name)
			break;

		double dval = Attrib().getReal( test->m_name, UNDEFINED );
		if (dval != UNDEFINED)
		{
			double testval = list_tool.Attrib().getReal( test->m_name, UNDEFINED );
			if (testval < UNDEFINED)
			{
				if (CReturn::Debug()>=5)
				{
					note.Format( "%s %g vs %g", test->m_name, dval, testval );
					ret.Diagnostic(note);
				}

				bool match = EQUAL( dval, testval );

				switch (test->m_match)
				{
				case MATCH_CRIT:
					if (!match)
					{
						if (CReturn::Debug()>=5)
						{
							note.Format( "FAIL" );
							ret.Diagnostic(note);
						}
						return FAILURE;
					}
					score += SCORE_CRIT;
					break;
				case MATCH_PREF:
					score += (match?SCORE_PREF:0);
					break;
				case MATCH_OPT:
					score += (match?SCORE_OPT:0);
					break;
				}
			}
		}

		++indx;
	}

	if (CReturn::Debug()>=5)
	{
		note.Format( "GOOD" );
		ret.Diagnostic(note);
	}
	// Survived the criticals...
	// Now, if it's a punch tool, check against rotations and symmetry
	// ... but we can avoid orientation checks for round tools.
	//
	if (IsPunchTool() && !IsRoundTool())
	{
		// 1. If the list tool is autoindex, WE STILL NEED TO CHECK BASE ANGLES
		int list_aidx = list_tool.IntGet( STR_AUTO_INDEX, 0 );
		int aidx = IntGet( STR_AUTO_INDEX, 0 );

		if ( aidx && !list_aidx )
			return FAILURE;

		// 3. Otherwise, if both fixed, must match angles
		double list_angle = list_tool.Attrib().getReal( STR_INDEX_ANGLE, 0.0 );
		double angle = Attrib().getReal( STR_INDEX_ANGLE, 0.0 );
		if (CReturn::Debug()>=5)
		{
			note.Format( "angle %g vs %g (symmetry %d)", angle, list_angle, Symmetry() );
			ret.Diagnostic(note);
		}
		
		bool match = EQUAL(angle, list_angle);
		if (!match)
		{
			switch (Symmetry())	// Both tools have the same symmetry
			{
			case TSYM_180:
				match = EQUAL(angle, list_angle+180);
				break;

			case TSYM_90:
				match = EQUAL(angle, list_angle+90);
				match = match || EQUAL(angle, list_angle+180);
				match = match || EQUAL(angle, list_angle+270);
				break;

			case TSYM_1:
				match = TRUE;
				break;
			}
		}

		if (!match)
			return FAILURE;
	}

	if (CReturn::Debug()>=5)
	{
		note.Format( "GOOD" );
		ret.Diagnostic(note);
	}

	return score;
}

CReturn
CDbTool::CustomToolInit( const CString& path )
{
	CReturn status;

	IntSet( STR_TYPE_ID, TTYPE_CUSTOM );
	status = m_shape.InitFromCTG( path );

	return status;
}

// For debugging.
void CDbTool::Trace() const
{
	CString descrip = StringGet( STR_DESCRIPTION, "" );
	int tnum = IntGet( STR_NC_CODE_NUMBER, -1 );
	int toolID = IntGet( STR_TOOL_ID, -1 );

	CString buf;
	buf.Format( "0x%x ID:%d T%d <%s>\n", this, toolID, tnum, descrip );

	CReturn status;
	status.Diagnostic( (LPCSTR) buf );
}
