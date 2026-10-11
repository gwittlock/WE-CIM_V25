
#include "stdafx.h"
#include "ToolConst.h"
#include "JofDB.h"
#include "Plo.h"
#include "Jof.h"

const CString PLO_NAME("plo_name");
const CString EMPTY_STRING("");
const CString STEP("step");
const CString TOOL_ASSIGNMENT("tool_assignment");
const CString TOOL_ASSEMBLY("tool_assembly");
const CString TOOL("tool");
const CString OP( "op" );

const int MAXBUF = 256;
const int DEFAULT_COLOR = RGB(0,255,0);

// For tools that require a diameter/kerf but don't have one
// in the JOF, such as Burner, Laser and WaterJet tools.
const double MIN_DIA = 0.01;

static int SncsToToolType( int sncs );
static CString StepName( const CString& ploName );
static CString ToolTypeDescrip( int typeID );


////////////////////////////////////////////////////////////////////////

CJof::CJof()
	: m_nvus(),
	  m_steps(),
	  m_tool_assignments(),
	  m_tool_assemblies(),
	  m_tools(),
	  m_ops(),
	  m_tool_attribs()
{
}

CJof::~CJof()
{
	m_steps.DestructiveFlush();
	m_tool_assignments.DestructiveFlush();
	m_tool_assemblies.DestructiveFlush();
	m_tools.DestructiveFlush();
	m_ops.DestructiveFlush();
	m_tool_attribs.DestructiveFlush();
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Converts a pm4/jof combination to the equivalent mm2.
//
CReturn
CJof::Read( const CString& filepath )
{
	CReturn status;

	JOF_Parse( filepath );
	PlosConvert();

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Create the CMDB equivalent (actually subet of tables).
//
// Note: Requires the target file to be deleted first!
//
CReturn
CJof::DbCreate( const CString& filepath )
{
	CReturn status;
	CVarList* tool;
	CJofDB db;
	int count, indx;

	status = db.Create( filepath );
	// if ( status.IsOk() )
	// 	status = db.Open( filepath );

	if ( status.IsOk() )
	{
		count = m_tool_attribs.Count();
		for (indx = 0; indx < count; ++indx)
		{
			tool = m_tool_attribs[indx];

			db.Update( (*tool) );
		}

		db.Close();
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Returns the count of converted steps.
//
int
CJof::StepsCount() const
{
	return m_steps.Count();
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Returns the count of converted tools.
//
int
CJof::ToolsCount() const
{
	return m_tools.Count();
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Gets the attributes of a converted tool.
//
void
CJof::ToolAttribsGet( int indx, CVarList* attribs ) const
{
	// (*attribs) = (*(m_tools[indx]));
	(*attribs) = (*(m_tool_attribs[indx]));
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Used during jof parsing to add a nvu (name-value-unit) to plo.
//
void
CJof::NvuAdd( const char* name, const char* value )
{
	CString tmpName( name );
	CString tmpValue( value );
	m_nvus.newString( tmpName, tmpValue );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Used during jof parsing to add a plo (page-like-object).
//
// NOTE: As reported by Textron Marine, laser/burner/waterjet were
// not picking up the kerf_width.  For whatever reason, this value
// appears in an 'op' and not a 'tool'.  This next statement is not
// easy to explain but this 'missing' kerf_width issue may cause
// other problems (with mapping 'steps' to tools) if a jof contains
// more that one 'op' having a kerf_width.
//
void
CJof::PloCreate( const char* name, int id )
{
	CString	ploName( name );
	int		user_name;

	if (ploName.CompareNoCase( STEP ) == 0)
	{
		user_name = m_nvus.getInt( "user_name", 0 );
		m_nvus.setInt( "step.user_name", user_name );
		m_nvus.setInt( "step.id", id );
		m_steps.Append( new CPlo( STEP, id, m_nvus ) );
	}
	else if (ploName.CompareNoCase( TOOL_ASSIGNMENT ) == 0)
	{
		user_name = m_nvus.getInt( "user_name", 0 );
		m_nvus.setInt( "tassign.user_name", user_name );
		m_nvus.setInt( "tassign.id", id );
		m_tool_assignments.Append( new CPlo( TOOL_ASSIGNMENT, id, m_nvus ) );
	}
	else if (ploName.CompareNoCase( TOOL_ASSEMBLY ) == 0)
	{
		m_tool_assemblies.Append( new CPlo( TOOL_ASSEMBLY, id, m_nvus ) );
	}
	else if (ploName.CompareNoCase( TOOL ) == 0)
	{
		m_tools.Append( new CPlo( TOOL, id, m_nvus ) );
	}
	else if (ploName.CompareNoCase( OP ) == 0)
	{
		int sncs = m_nvus.getInt( "section", 0 );

		if ( (sncs == 1030501) ||	// laser
			 (sncs == 1030502) ||	// burner
			 (sncs == 1030504) )	// waterjet
		{
			m_ops.Append( new CPlo( OP, id, m_nvus ) );
		}
	}

	m_nvus.Reset();
}

int
CJof::ToolAssignmentGet( int step_num ) const
{
	int	count, indx;
	int	tassign_num;

	count = m_tool_attribs.Count();
	for (indx = 0; indx < count; ++indx)
	{
		if (m_tool_attribs[indx]->getInt( "step.id", 0 ) == step_num)
			break;
	}

	if (indx < count)
	{
		tassign_num = m_tool_attribs[indx]->getInt( "tassign.id", 0 );
	}
	else
	{
		tassign_num = 0;
	}

	return tassign_num;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// For debugging.
//
void
CJof::AttribsDump( const CString& filepath )
{
#if TODO
	FILE* f = fopen( filepath, "w" );
	if (f != NULL)
	{
		int pcount = m_plos.Count();
		for (int pndx = 0; pndx < pcount; ++pndx)
		{
			CString name;
			CString value;
			CVarList* plo;

			plo = m_plos[pndx];
			value = plo->getString( PLO_NAME, "" );

			fprintf( f, "%s\n", value );
			
			int ncount = plo->countVar();
			for (int nndx = 0; nndx < ncount; ++nndx)
			{
				CVar* var = plo->getVar( nndx );
				fprintf( f, "\t%s = %s\n", var->getName(), var->getString() );
			}

			fprintf( f, "\n" );
		}

		fclose( f );
	}
#endif
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// For debugging.
//
void
CJof::ToolsDump( const CString& filepath )
{
	FILE* f = fopen( filepath, "w" );
	if (f != NULL)
	{
		int tcount = m_tool_attribs.Count();
		for (int tndx = 0; tndx < tcount; ++tndx)
		{
			CVarList* tool = m_tool_attribs[tndx];
			
			int ncount = tool->countVar();
			for (int nndx = 0; nndx < ncount; ++nndx)
			{
				CVar* var = tool->getVar( nndx );
				fprintf( f, "\t%s = %s\n", var->getName(), var->getString() );
			}

			fprintf( f, "\n" );
		}

		fclose( f );
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Load the entire jof text into memory.
//
// Note: This solution is used because we already have working lex & yacc
// examples that do not use text files as input.  Hence, we can avoid
// diddling within the bowels of lex & yacc input processing.
//
CString*
CJof::Load( const CString& filepath )
{
	CString* text = NULL;

	FILE* f = fopen( filepath, "r" );
	if (f != NULL)
	{
		char buf[MAXBUF+1];

		text = new CString();
		while (fgets( buf, MAXBUF, f ) != NULL)
		{
			(*text) += buf;
		}

		fclose( f );
	}

	return text;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Finds all attributes in the plo tree, starting at the 'root' index.
//
// Note: Given an index to a tool_assignment, returns all of the
// attributes in the tree tool_assignment->tool_assembly->tool.
//
void
CJof::AttribsGet( int step_indx, CVarList* attribs )
{
	int		link;
	int		count, indx;

	// Copy the step attributes.
	(*attribs) += m_steps[step_indx]->Nvus();
	link = m_steps[step_indx]->IntGet( "tool_assignment", 0 );

	// Copy the tool assignment attributes.
	if (link > 0)
	{
		count = m_tool_assignments.Count();
		for (indx = 0; indx < count; ++indx)
		{
			if (m_tool_assignments[indx]->Id() == link)
			{
				(*attribs) += m_tool_assignments[indx]->Nvus();
				link = m_tool_assignments[indx]->IntGet( "tool_assembly", 0 );
				break;
			}
		}
	}

	// Copy the tool assembly attributes.
	if (link > 0)
	{
		count = m_tool_assemblies.Count();
		for (indx = 0; indx < count; ++indx)
		{
			if (m_tool_assemblies[indx]->Id() == link)
			{
				(*attribs) += m_tool_assemblies[indx]->Nvus();
				link = m_tool_assemblies[indx]->IntGet( "tool", 0 );
				break;
			}
		}
	}

	// Copy the tool attributes.
	if (link > 0)
	{
		count = m_tools.Count();
		for (indx = 0; indx < count; ++indx)
		{
			if (m_tools[indx]->Id() == link)
			{
				(*attribs) += m_tools[indx]->Nvus();
				// Conditionally add the kerf_width attribute.
				CondKerfAdd( attribs );
				break;
			}
		}
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Converts all jof plo data to the equivalent we-cim tool attributes.
//
void
CJof::PlosConvert()
{
	CVarList	superset;
	int			count, indx;

	count = m_steps.Count();

	// Loop over the step PLOs.
	for (indx = 0; indx < count; ++indx)
	{
		// Create the superset of all as an attribute list.
		// Extract the tree (step->tool_assignment -> tool_assembly -> tool).
		AttribsGet( indx, &superset );

		// Convert the attributes to our "CDbTool attributes".
		AttribsConvert( superset );

		superset.Reset();
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Convert the JOF type attributes to our tool attributes, whose
// attribute names are seen as the field names in the CMDB.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// NOTE: This painful mapping between SmartCAM and WE-CIM tool parameters
// allows for proper importing and nesting of SmartCAM parts.
//
// NOTE: This method also preprocesses information for CJofDB::ToolCribAdd()
// That method creates an Access database file, each record of which records
// the superset of all tool parameters.  In other words, CJofDB::ToolCribAdd()
// "fills in the blanks" where it does not find an attribute attached to a
// tool.
//
// NOTE: There is a painful dichotomy in attribute naming conventions.
// Whereas the application uses underscores as word-separators, the
// mdb uses spaces.  Therefore, all attributes must use the underscore
// convention but we must use the space convention when creating an mdb.
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// The typical jof PLOs appears as:
//
//    tool_assignment = 2
//    [
//        indexable = "0"
//        section = "2530101"
//        start_angle = "0"
//        station_size = "0"
//        tl_num = "2"
//        tool_assembly = "2"
//        user_name = "2"
//    ]
//
//    tool_assembly = 2
//    [
//        clr_type = "0"
//        die_cl = "0"
//        notes = ""
//        section = "2430402"
//        tool = "2"
//        user_name = "tool_assembly.2"
//    ]
//    
//    tool = 2
//    [
//        cradius = "0"
//        descrip = "0.500 Square 0.000 rad. 0.00 deg."
//        graphics = ""
//        section = "2010402"
//        user_name = "Square.2"
//        width = ".5"
//    ]
//
//
// Creates one mondo 'flat' VarList containing all necessary attributes
// (obtained from the step, tool assignent, tool assembly and tool plos).
//
void
CJof::AttribsConvert( const CVarList& superset )
{
	CString		stepName;
	CVarList*	this_set;
	int			tl_num;

	CString descrip = superset.getString( "descrip", "" );  // good for debugging
	int sncs = superset.getInt( "section", 0 );
	int typeID = SncsToToolType( sncs );
	
	int indexable = superset.getInt( "indexable", 0 );

	this_set = new CVarList();

	// The SmartCAM entity links to a step via the internal id seen
	// in the jof as eg. "step = 14 [ ... ]"
	// The user_name nvu is how the user identifies the step, however.
	this_set->setString( "step.id", superset.getString( "step.id", "" ) );
	this_set->setString( "step.user_name", superset.getString( "step.user_name", "" ) );

	// NOTE: The following assumption is not necessarily true, as discovered by Textron.
	// There is a one-to-one correspondence between steps and tool assignments.
	// The id and user_name appears to match with the corresponding step.
	this_set->setString( "tassign.id", superset.getString( "tassign.id", "" ) );
	this_set->setString( "tassign.user_name", superset.getString( "tassign.user_name", "" ) );

	// [Tool Crib] values (in order of appearance in CMDB table)
	this_set->setString( "Description", descrip );
	this_set->setInt( "Type_ID", typeID );
	this_set->setString( "Type", ToolTypeDescrip( typeID ) );
	this_set->setColor( DEFAULT_COLOR );
	this_set->setInt( "Loff", 0 );
	this_set->setInt( "Doff", 0 );

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// As copied from defn of g_matchlist[] in Db\DbTool.cpp
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//
	// Failure to properly set these parameters will cause
	// nesting to 'drop' parts.
	//
	// { "width", MATCH_CRIT },
	// { "length", MATCH_CRIT },
	// { "diameter", MATCH_CRIT },
	// { "corner_radius", MATCH_CRIT },
	// { "web", MATCH_CRIT },
	// { "radius", MATCH_CRIT },
	// { "angle", MATCH_CRIT },
	// { "major_diameter", MATCH_CRIT },
	// { "minor_diameter", MATCH_CRIT },

	// { "doff", MATCH_PREF },
	// { "loff", MATCH_PREF },
	// { "kerf", MATCH_PREF },
	//
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// NOTE: By NOT setting a parameter, it is treated by nesting
	// as being UNDEFINED, which is the expected behavior when
	// CDbTool::Matches( ) searches for matching tools.

	switch (typeID)
	{
	case TTYPE_TAP:
	case TTYPE_DRILL:
		this_set->setReal( "Angle", 0. );
		this_set->setReal( "Diameter", superset.getReal( "tl_dia", MIN_DIA ) );
		break;

	case TTYPE_BURNER:
	case TTYPE_LASER:
	case TTYPE_WATERJET:
		this_set->setReal( "Kerf", superset.getReal( "kerf_width", MIN_DIA ) );  // comes from op
		break;

	case TTYPE_SCRIBE:
	case TTYPE_POWDER_MARK:
		this_set->setReal( "Kerf", superset.getReal( "tl_dia", MIN_DIA ) );
		break;

	case TTYPE_CENTER_PUNCH:
	case TTYPE_ROUND:
	case TTYPE_HEXAGON:
		this_set->setReal( "Diameter", superset.getReal( "tl_dia", MIN_DIA ) );
		break;

	case TTYPE_SQUARE:
		this_set->setReal( "Corner_Radius", superset.getReal( "cradius", 0. ) );
		this_set->setReal( "Width", superset.getReal( "width", 0. )  );
		break;

	case TTYPE_RECTANGLE:
	case TTYPE_DIAMOND:
		this_set->setReal( "Corner_Radius", superset.getReal( "cradius", 0. ) );
		this_set->setReal( "Length", superset.getReal( "length", 0. ) );
		this_set->setReal( "Width", superset.getReal( "width", 0. )  );
		break;

	case TTYPE_OBROUND:
	case TTYPE_FORMING:
	case TTYPE_MARKING:
		this_set->setReal( "Length", superset.getReal( "length", 0. ) );
		this_set->setReal( "Width", superset.getReal( "width", 0. )  );
		break;

	case TTYPE_CORNER_RADIUS:
		this_set->setReal( "Radius", superset.getReal( "cradius", 0. ) );
		this_set->setReal( "Web", superset.getReal( "width", 0.0 ) );  // because graphics are wrong
		this_set->setReal( "Width", superset.getReal( "length", 0.0 ) );
		break;

	case TTYPE_SINGLE_D:
	case TTYPE_DOUBLE_D:
		this_set->setReal( "Diameter", superset.getReal( "tl_dia", MIN_DIA ) );
		this_set->setReal( "Width", superset.getReal( "width", 0. )  );
		break;

	case TTYPE_TRAPEZOID:
		{
		double slope = superset.getReal( "slope", 0. );

		this_set->setReal( "Angle", ((slope < 1.e-4) ? 90. : (90. - slope)) );
		this_set->setReal( "Corner_Radius", superset.getReal( "cradius", 0. ) );
		this_set->setReal( "Length", superset.getReal( "length", 0. ) );
		this_set->setReal( "Width", superset.getReal( "width", 0. )  );
		}
		break;

	case TTYPE_KEYHOLE:
		this_set->setReal( "Length", superset.getReal( "length", 0. ) );
		this_set->setReal( "Major_Diameter", 0. );
		this_set->setReal( "Minor_Diameter", 0. );
		break;
	}

	this_set->setReal( "Pitch", 0. );
	this_set->setReal( "Shear", 0 );
	this_set->setReal( "Cost", 0 );
	this_set->setReal( "Life",0 );
	this_set->setString( "Filename", "-" );
	this_set->setReal( "Reqd_Station_Size", 0. );
	this_set->setInt( "Reqd_Auto_Index", indexable );

	// [Station Configuration] values (in order of appearance in CMDB table)
	this_set->setInt( "Machine ID", 0 );		// ???
	this_set->setInt( "Workplane ID", 1 );	// top
	this_set->setReal( "Station_Location_X", 0. );
	this_set->setReal( "Station_Location_Y", 0. );
	this_set->setInt( "Auto_Index", indexable );

	// nobody should miss these
	// this_set->setString( "Primary_Code", "0" );
	// this_set->setString( "Secondary_Code", "0" );

	tl_num = superset.getInt( "tl_num", 0 );
	this_set->setInt( "NC_Code_Number", tl_num );
	this_set->setReal( "Station_Size", superset.getReal( "station_size", 0. ) );

	// [Tool Setup Members] values (in order of appearance in CMDB table)
	this_set->setInt( "Fixed_Station", 0 );

	if ( indexable )
	{
		// NOTE: The actual orientation will be determined by the associated entity.
		this_set->setReal( "Index_Angle", 0. );
	}
	else
	{
		this_set->setReal( "Index_Angle", superset.getReal( "start_angle", 0. ) );
	}

	m_tool_attribs.Append( this_set );
}

void
CJof::CondKerfAdd( CVarList* attribs )
{
	CPlo*	plo;
	double	kerf;
	int		count, indx;
	int		toolsncs;

	toolsncs = attribs->getInt( "section", 0 );

	if ( (toolsncs == 2010501) ||	// laser
		 (toolsncs == 2010502) ||	// burner
		 (toolsncs == 2010504) )	// waterjet
	{
		count = m_ops.Count();
		for (indx = 0; indx < count; ++indx)
		{
			plo = m_ops[indx];

			if ( IsReqdOP( toolsncs, plo ) )
			{
				kerf = plo->DoubleGet( "kerf_width", MIN_DIA );
				attribs->setReal( "kerf_width", kerf );
				break;
			}
		}
	}
}

bool
CJof::IsReqdOP( int toolsncs, CPlo* op )
{
	int	opsncs = op->IntGet( "section", 0 );

	if (toolsncs == 2010501)		// laser
	{
		return (opsncs == 1030501);
	}
	else if (toolsncs == 2010502)	// burner
	{
		return (opsncs == 1030502);
	}
	else if (toolsncs == 2010504)	// waterjet
	{
		return (opsncs == 1030504);
	}

	return FALSE;
}

void
CJof::JOF_Parse( const CString& filepath )
{
	CString	str;
	char	buf[MAXBUF];
	int		len;
	FILE*	f;

	f = fopen( filepath, "r" );
	if (f != NULL)
	{
		while (fgets( buf, (MAXBUF-1), f ) != NULL)
		{
			len = strlen( buf );
			buf[--len] = '\0';

			str = buf;

			if ( IsRecognizedPLO(str) )
			{
				PLO_Parse( f, str );
			}
		}

		fclose( f );
	}
}

void
CJof::PLO_Parse( FILE* f, const CString& text )
{
	CString	str;
	char	buf[MAXBUF];
	CString	plo_name;
	CString	plo_id;
	CString	nvu_name;
	CString	nvu_value;
	int		len;

	StringPairGet( text, &plo_name, &plo_id );

	while (fgets( buf, (MAXBUF-1), f ) != NULL)
	{
		len = strlen( buf );
		buf[--len] = '\0';

		str = buf;

		if (str.CompareNoCase("[") == 0)
		{
			// do nothing. start of PLO
		}
		else if (str.CompareNoCase("]") == 0)
		{
			PloCreate( plo_name, atoi(plo_id) );
			break;
		}
		else
		{
			StringPairGet( str, &nvu_name, &nvu_value );
			NvuAdd( nvu_name, nvu_value );
		}
	}
}

bool
CJof::IsRecognizedPLO( const CString& text )
{
	return ((text.Find("step =") == 0) ||
			(text.Find("tool_assignment =") == 0) ||
			(text.Find("tool_assembly =") == 0) ||
			(text.Find("tool =") == 0) ||
			(text.Find("op =") == 0));
}

// Possible input forms:
// 1. plo_name = 23
// 2.     nvu_name = "nvu value"
//
void
CJof::StringPairGet( const CString& text, CString* name, CString* value )
{
	int	indx;

	indx = text.Find('=');

	(*name) = text.Left( indx );
	name->TrimLeft();
	name->TrimRight();

	(*value) = text.Right( text.GetLength() - (indx + 1) );
	value->TrimLeft();
	if (value->GetAt(0) == '"')
		(*value) = value->Mid( 1, value->GetLength() - 2 );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Static methods
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Converts a jof sncs (shelf-notebook-chapter-section) number to
// the equivalent we-cim tool type.
//
int SncsToToolType( int sncs )
{
	switch ( sncs )
	{
	case 2010401:  return TTYPE_ROUND;
	case 2010402:  return TTYPE_SQUARE;
	case 2010403:  return TTYPE_RECTANGLE;
	case 2010404:  return TTYPE_OBROUND;
	case 2010405:  return TTYPE_DIAMOND;
	case 2010406:  return TTYPE_ROUND;			// SmartCAM 'other', which requires graphics file
	case 2010407:  return TTYPE_CORNER_RADIUS;
	case 2010408:  return TTYPE_SINGLE_D;
	case 2010409:  return TTYPE_DOUBLE_D;
	case 2010410:  return TTYPE_TRAPEZOID;
	// case :  return TTYPE_KEYHOLE;
	// case :  return TTYPE_FORMING;
	// case :  return TTYPE_MARKING;
	// case :  return TTYPE_HEXAGON;
	case 2010501:  return TTYPE_LASER;
	case 2010502:  return TTYPE_BURNER;
	case 2010504:  return TTYPE_WATERJET;
	case 2010602:  return TTYPE_SCRIBE;
	// case :  return TTYPE_POWDER_MARK;
	// case :  return TTYPE_CENTER_PUNCH;
	case 2010601:  return TTYPE_CENTER_PUNCH;  // because of Textron Marine 9/11/2002
	default: return TTYPE_NONE;
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Used to 'name' a tool in the we-cim entity database.
//

// ~!@#$%^&* Microsoft !!!  They do not have something like
// CString::Substring( a, b ) and CString::Delete() has a bug !
CString StepName( const CString& ploName )
{
	int len = ploName.GetLength();
	int indx = ploName.ReverseFind('.');

	return ("step_" + ploName.Right( len-indx-1 ));
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// This should probably come from the CMDB, but this is solution is cheap and dirty.
//
CString ToolTypeDescrip( int typeID )
{
	switch (typeID)
	{
	case 1:		return "Router_Bit";
	case 2:		return "Disc_Saw";
	case 3:		return "Brad_Point";
	case 4:		return "Lance_Bit";
	case 5:		return "Counter_Sink";
	case 6:		return "Aggregate";
	case 7:		return "Tap";
	case 8:		return "Drill";
	case 9:		return "Burner";
	case 10:	return "Scribe";
	case 11:	return "Powder_Mark";
	case 12:	return "Center_Punch";
	case 13:	return "Laser";
	case 14:	return "Waterjet";
	case 15:	return "Round";
	case 16:	return "Square";
	case 17:	return "Rectangle";
	case 18:	return "Obround";
	case 19:	return "Diamond";
	case 20:	return "Corner_Radius";
	case 21:	return "Single_D";
	case 22:	return "Double_D";
	case 23:	return "Trapezoid";
	case 24:	return "Keyhole";
	case 25:	return "Forming";
	case 26:	return "Marking";
	case 27:	return "Hexagon";
	case 28:	return "End_Mill";
	default:	return "UNKNOWN";
	}
}

