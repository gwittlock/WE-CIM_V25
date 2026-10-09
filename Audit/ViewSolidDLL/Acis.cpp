// ==================================================================================
// Acis.cpp: implementation of the CAcis class.
//
// ==================================================================================

#include "stdafx.h"
#include "cmn_resource.h"

#include "return.h"

#include "Acis.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// ==================================================================================

int			CAcis::m_attrib_num = -1;
ATTRIB*		CAcis::m_attrib_at = NULL;
ENTITY*		CAcis::m_attrib_ent = NULL;

// ==================================================================================

CAcis::CAcis()
{
	m_alive = FALSE;

	m_attrib_num = -1;
	m_attrib_at = NULL;
	m_attrib_ent = NULL;
}

CAcis::~CAcis()
{
	Terminate();
}

// ==================================================================================

BOOL
CAcis::Terminate( void )
{

	if (!m_alive)
		return FALSE;

	// Iterate the operations, and remove all the graphical stuff...
	CheckOutcome(api_terminate_part_manager());
	CheckOutcome(api_terminate_operators());
	CheckOutcome(api_terminate_faceter());
	CheckOutcome(api_terminate_opengl_rendering());
	CheckOutcome(api_terminate_graphic_interaction());
	CheckOutcome(api_terminate_kernel());

	api_stop_modeller();

	m_alive = FALSE;
	return TRUE;
}


// ==================================================================================

BOOL
CAcis::Init( void )
{
	if (m_alive)
		return FALSE;

	CheckOutcome(api_start_modeller(0));

	CheckOutcome(api_initialize_kernel());
	CheckOutcome(api_initialize_graphic_interaction());
	CheckOutcome(api_initialize_opengl_rendering());
	CheckOutcome(api_initialize_faceter());
	CheckOutcome(api_initialize_operators());
	CheckOutcome(api_initialize_part_manager());

//	api_set_int_option( "logging", 1 );
//	api_set_int_option( "delete_forward_states", 0 ); // default 0
//	api_set_int_option( "distributed_history", 0 );	// default 1
//	api_set_int_option( "part_history", 0 );
//	api_set_int_option( "roll_limit", 10 );

	m_alive = TRUE;
	return TRUE;
}


// ==================================================================================

static char global_attrib_name_buf[256];

// Convert from name to bubba_name
char*
CAcis::attrib_name_to_label( 
	char* in_name )
{
	if (!strncmp( in_name, HEADER_MARK, strlen(HEADER_MARK)))
	{
		strcpy( global_attrib_name_buf, HEADER_PREFIX );
		strcat( global_attrib_name_buf, &in_name[strlen(HEADER_MARK)] );
	}
	else
	{
		strcpy( global_attrib_name_buf, NAME_PREFIX );
		strcat( global_attrib_name_buf, in_name );
	}

	return global_attrib_name_buf;
}

// Convert from bubba_name to name
char* 
CAcis::attrib_label_to_name( 
	char* in_label )
{
	if (!strncmp( in_label, HEADER_PREFIX, strlen(HEADER_PREFIX)))
	{
		strcpy( global_attrib_name_buf, HEADER_MARK );
		strcat( global_attrib_name_buf, &in_label[strlen(HEADER_PREFIX)] );
	}
	else
		strcpy( global_attrib_name_buf, &in_label[strlen(NAME_PREFIX)] );

	return global_attrib_name_buf;
}

// ==================================================================================

BOOL
CAcis::set_attrib_string( 
	ENTITY*		in_ent, 
	char*			in_name,
	char*			in_att )
{
	ATTRIB_GEN_STRING*	att;
	char*						label;

API_BEGIN

//TODO:  ONLY UNDEF/DEFINE NEW IF WE ARE IN DEBUG MODE!!!!!
#undef new
	result = 0;	
	label = attrib_name_to_label(in_name);
	att = new ATTRIB_GEN_STRING( in_ent, label, in_att, SplitCopy, MergeKeepKept, TransIgnore );
#define new DEBUG_NEW

API_END

	return result.ok();
}

// ==================================================================================

BOOL
CAcis::set_attrib_int( 
	ENTITY*		in_ent, 
	char*			in_name,
	int			in_att )
{
	ATTRIB_GEN_INTEGER*	att;
API_BEGIN

//TODO:  ONLY UNDEF/DEFINE NEW IF WE ARE IN DEBUG MODE!!!!!
#undef new
	result = 0;	
	att = new ATTRIB_GEN_INTEGER( in_ent, attrib_name_to_label(in_name), in_att, SplitCopy, MergeKeepKept, TransIgnore );
#define new DEBUG_NEW

API_END

	return result.ok();
}

// ==================================================================================

BOOL
CAcis::set_attrib_double( 
	ENTITY*		in_ent, 
	char*			in_name,
	double		in_att )
{
	ATTRIB_GEN_REAL*	att;
API_BEGIN

//TODO:  ONLY UNDEF/DEFINE NEW IF WE ARE IN DEBUG MODE!!!!!
#undef new
	result = 0;	
	att = new ATTRIB_GEN_REAL( in_ent, attrib_name_to_label(in_name), in_att, SplitCopy, MergeKeepKept, TransIgnore );
#define new DEBUG_NEW

API_END

	return result.ok();
}

// ==================================================================================

BOOL
CAcis::set_attrib_point( 
	ENTITY*		in_ent, 
	char*			in_name,
	position*	in_att )
{
	ATTRIB_GEN_POSITION*	att;
API_BEGIN

//TODO:  ONLY UNDEF/DEFINE NEW IF WE ARE IN DEBUG MODE!!!!!
#undef new
	result = 0;	
	att = new ATTRIB_GEN_POSITION( in_ent, attrib_name_to_label(in_name), *in_att, SplitCopy, MergeKeepKept, TransIgnore );
#define new DEBUG_NEW

API_END

	return result.ok();
}

// ==================================================================================

BOOL
CAcis::set_attrib_pointer( 
	ENTITY*		in_ent, 
	char*			in_name,
	ENTITY*		in_att )
{
	ATTRIB_GEN_POINTER*	att;
API_BEGIN

//TODO:  ONLY UNDEF/DEFINE NEW IF WE ARE IN DEBUG MODE!!!!!
#undef new
	result = 0;	
	att = new ATTRIB_GEN_POINTER( in_ent, attrib_name_to_label(in_name), in_att, SplitCopy, MergeKeepKept, TransIgnore );
#define new DEBUG_NEW

API_END

	return result.ok();
}

// ==================================================================================


BOOL
CAcis::set_owner( 
	ENTITY*		in_owner, 
	char*			in_name,
	ENTITY*		in_slave )
{
	ATTRIB_GEN_ENTITY*	att;
API_BEGIN

//TODO:  ONLY UNDEF/DEFINE NEW IF WE ARE IN DEBUG MODE!!!!!
#undef new
	result = 0;	
	att = new ATTRIB_GEN_ENTITY( in_owner, attrib_name_to_label(in_name), in_slave, SplitCopy, MergeKeepKept, TransApply );
#define new DEBUG_NEW

API_END

	return result.ok();
}

// ==================================================================================

int
CAcis::count_attrib( 
	ENTITY*	in_ent,
	BOOL		in_header )		// TRUE if counting header entries
{
	int			num;
	ATTRIB*		att;
	char*			name;

	if (!in_ent)
		return 0;

	if (in_header)
		name = HEADER_PREFIX;
	else
		name = NAME_PREFIX;

	num = 0;
	att = in_ent->attrib();
	while ( att )
	{
		if ( is_ATTRIB_GEN_NAME( att ) )
		{
			if (!strncmp( ((ATTRIB_GEN_NAME*)att)->name(), name, strlen(name) ) )
			{
				num++;
			}
		}
		att = att->next();
	}
	return num;
}

// ==================================================================================

BOOL
CAcis::reset_attrib( 
	ENTITY* in_ent,
	BOOL		in_header )		// TRUE if counting header entries
{
	ATTRIB*		att;
	ATTRIB*		next;
	char*			name;

	if (!in_ent)
		return FALSE;

	if (in_header)
		name = HEADER_PREFIX;
	else
		name = NAME_PREFIX;

	// Get the TYPE attribute
	att = in_ent->attrib();
	while ( att )
	{
		next = att->next();
		if ( is_ATTRIB_GEN_NAME( att ) )
		{
			if (!strncmp( ((ATTRIB_GEN_NAME*)att)->name(), name, strlen(name) ) )
			{
				att->lose();
			}
		}
		att = next;
	}

	return TRUE;
}

// ==================================================================================

BOOL
CAcis::del_attrib( 
	ENTITY*	in_ent, 
	char*		in_name )
{
	ATTRIB*		att;
	char*			label;

	if (!in_ent)
		return FALSE;

	label = attrib_name_to_label(in_name);

	// Get the TYPE attribute
	att = in_ent->attrib();
	while ( att )
	{
		if ( is_ATTRIB_GEN_NAME( att ) )
		{
			if (!stricmp( ((ATTRIB_GEN_NAME*)att)->name(), label ) )
			{
				att->lose();
				return TRUE;
			}
		}
		att = att->next();
	}

	return FALSE;
}

// ==================================================================================

char*
CAcis::get_attrib_name( 
	ENTITY*	in_ent, 
	int		in_idx )
{
	ATTRIB*	att;

	if (!in_ent)
		return NULL;

	if ( (in_ent != m_attrib_ent)
		|| (m_attrib_at == NULL)
		|| (in_idx < m_attrib_num) )
	{
		m_attrib_ent = in_ent;
		att = in_ent->attrib();
		m_attrib_num = -1;
	}
	else
		att = m_attrib_at->next();

	while ( att
			&& (m_attrib_num < in_idx) )
	{
		if ( is_ATTRIB_GEN_NAME( att ) )
		{
			if (!strncmp( ((ATTRIB_GEN_NAME*)att)->name(), NAME_PREFIX, strlen(NAME_PREFIX) ) )
			{
				m_attrib_num++;
				if (m_attrib_num == in_idx)
					break;
			}
		}
		att = att->next();
	}

	m_attrib_at = att;
	if (att)
		return attrib_label_to_name( (char*)((ATTRIB_GEN_NAME*)att)->name() );

	return NULL;
}

// ==================================================================================


ATTRIB*
CAcis::get_attrib_first( 
	ENTITY*	in_ent )
{
	ATTRIB*	att;

	if (!in_ent)
		return 0;

	att = in_ent->attrib();

	m_attrib_ent = in_ent;
	m_attrib_num = 0;

	while ( att )
	{
		if ( is_ATTRIB_GEN_NAME( att ) )
		{
			if (!strncmp( ((ATTRIB_GEN_NAME*)att)->name(), NAME_PREFIX, strlen(NAME_PREFIX) ) )
			{
				m_attrib_at = att;
				return att;
			}
		}
		att = att->next();
	}

	return NULL;
}

// ==================================================================================

ATTRIB*
CAcis::get_attrib_next( 
	ENTITY*	in_ent )
{
	ATTRIB*	att;

	if (!in_ent)
		return NULL;

	if (in_ent != m_attrib_ent)
		return NULL;

	att = m_attrib_at->next();
	m_attrib_num++;

	while ( att )
	{
		if ( is_ATTRIB_GEN_NAME( att ) )
		{
			if (!strncmp( ((ATTRIB_GEN_NAME*)att)->name(), NAME_PREFIX, strlen(NAME_PREFIX) ) )
			{
				m_attrib_at = att;
				return att;
			}
		}
		att = att->next();
	}

	return NULL;
}

// ==================================================================================

eAttribType 
CAcis::get_attrib_type( 
	ENTITY*	in_ent, 
	char*		in_name )
{
	ATTRIB*		att;
	char*			label;

	if ( !in_ent
		|| !in_name )
		return ATTRIB_UNKNOWN;

	label = attrib_name_to_label(in_name);

	// Get the TYPE attribute
	att = in_ent->attrib();
	while ( att )
	{
		if ( is_ATTRIB_GEN_NAME( att ) )
		{
			if (!stricmp( ((ATTRIB_GEN_NAME*)att)->name(), label) )
			{
				if (is_ATTRIB_GEN_INTEGER( att ))
					return ATTRIB_INT;
				else
				if (is_ATTRIB_GEN_REAL( att ))
					return ATTRIB_DOUBLE;
				else
				if (is_ATTRIB_GEN_STRING( att ))
					return ATTRIB_STRING;

				return ATTRIB_UNKNOWN;
			}
		}
		att = att->next();
	}

	return ATTRIB_UNKNOWN;
}

// ==================================================================================


BOOL
CAcis::get_attrib_string( 
	ENTITY*		in_ent, 
	char*			in_name,
	char**		io_att )
{
	ATTRIB*		att;
	char*			label;

	if (!in_ent)
		return FALSE;

	label = attrib_name_to_label(in_name);
	// Get the TYPE attribute
	att = in_ent->attrib();
	while ( att )
	{
		if ( is_ATTRIB_GEN_STRING( att ) )
		{
			if (!stricmp( ((ATTRIB_GEN_STRING*)att)->name(), label) )
			{
//				*io_att = (char*)(((ATTRIB_GEN_STRING*)att)->value());
				const char* tmp;
				tmp = ((ATTRIB_GEN_STRING*)att)->value();
				*io_att = (char*)tmp;
				return TRUE;
			}
		}
		att = att->next();
	}

	return FALSE;
}


// ==================================================================================

BOOL
CAcis::get_attrib_int( 
	ENTITY*		in_ent, 
	char*			in_name,
	int*			io_att )
{
	ATTRIB*		att;
	char*			label;

	if (!in_ent)
		return FALSE;

	label = attrib_name_to_label(in_name);

	// Get the TYPE attribute
	att = in_ent->attrib();
	while ( att )
	{
		if ( is_ATTRIB_GEN_INTEGER( att ) )
		{
			if (!stricmp( ((ATTRIB_GEN_INTEGER*)att)->name(), label) )
			{
				*io_att = (((ATTRIB_GEN_INTEGER*)att)->value());
				return TRUE;
			}
		}
		att = att->next();
	}

	return FALSE;
}

// ==================================================================================


BOOL
CAcis::get_attrib_double( 
	ENTITY*		in_ent, 
	char*			in_name,
	double*		io_att )
{
	ATTRIB*		att;
	char*			label;

	if (!in_ent)
		return FALSE;

	label = attrib_name_to_label(in_name);

	// Get the TYPE attribute
	att = in_ent->attrib();
	while ( att )
	{
		if ( is_ATTRIB_GEN_REAL( att ) )
		{
			if (!stricmp( ((ATTRIB_GEN_REAL*)att)->name(), label) )
			{
				*io_att = (((ATTRIB_GEN_REAL*)att)->value());
				return TRUE;
			}
		}
		att = att->next();
	}

	return FALSE;
}

// ==================================================================================

BOOL
CAcis::get_attrib_point( 
	ENTITY*		in_ent, 
	char*			in_name,
	position*	io_att )
{
	ATTRIB*		att;
	char*			label;

	if (!in_ent)
		return FALSE;

	label = attrib_name_to_label(in_name);

	// Get the TYPE attribute
	att = in_ent->attrib();
	while ( att )
	{
		if ( is_ATTRIB_GEN_POSITION( att ) )
		{
			if (!stricmp( ((ATTRIB_GEN_POSITION*)att)->name(), label) )
			{
				*io_att = (((ATTRIB_GEN_POSITION*)att)->value());
				return TRUE;
			}
		}
		att = att->next();
	}

	return FALSE;
}

// ==================================================================================

BOOL
CAcis::get_attrib_pointer( 
	ENTITY*		in_ent, 
	char*			in_name,
	ENTITY**		io_att )
{
	ATTRIB*		att;
	char*			label;

	if (!in_ent)
		return FALSE;

	label = attrib_name_to_label(in_name);

	// Get the TYPE attribute
	att = in_ent->attrib();
	while ( att )
	{
		if ( is_ATTRIB_GEN_POINTER( att ) )
		{
			if (!stricmp( ((ATTRIB_GEN_POINTER*)att)->name(), label) )
			{
				*io_att = (((ATTRIB_GEN_POINTER*)att)->value());
				return TRUE;
			}
		}
		att = att->next();
	}

	return FALSE;
}

// ==================================================================================
//
//	ALL ERROR REPORTING DISABLED FOR NOW -- it doesn't help anyway
//	24 July 00
//
BOOL
CAcis::CheckOutcome(
	const outcome& result )
{
	CReturn		ret;

	BOOL ok = TRUE;
	if( !result.ok() )
	{
		err_mess_type err_no = result.error_number();

		// Only return FALSE for catastrophic errors.. which we must learn by trial
		if (err_no == 4400)
			ok = FALSE;

#ifdef _DEBUG
		char default_msg[20];
		const char* error_string = find_err_mess( err_no );
		if( (error_string == NULL ) 
			|| (*error_string == '\0'))
		{
			sprintf(default_msg,"%d",result.error_number());
			error_string = default_msg;
		}

		ret.Internal( IDS_INTERNAL_ERROR, error_string );
#endif
	}

	return ok;
}


// ==================================================================================


BOOL
CAcis::Import(
	char*	in_name )
{
	ENTITY_LIST		temp_list;
	ENTITY*			ent;
//	HANDLE			file_handle;
//	FILE*			file_star;

//   file_handle = ::CreateFile( in_name,              // name
//								GENERIC_READ,        // access mode
//								FILE_SHARE_READ,     // share mode
//								NULL,                // security descriptor
//								OPEN_EXISTING,       // how to create
//								FILE_ATTRIBUTE_NORMAL,  // file attributes
//								NULL );              // handle of file to match

	CStdioFile	fred;
	fred.Open( in_name, CFile::modeRead );

	SatFile	sally( fred.m_pStream );
	CheckOutcome( api_restore_entity_list_file(  &sally, temp_list ) );

	temp_list.init();
	while (ent = temp_list.next())
	{
//		m_context->add( ent );
	}
//	Paint();

	return TRUE;
}




void 
CAcis::DumpBody(
	ENTITY*	body, 
	char*		name)
{
	ENTITY_LIST elist;
	elist.add(body);

	//	Open file to which model will be saved as (*.sat).
	FILE* fp = fopen(name,"w");
	process_file(fp);

	//	Save ENTITY_LIST to file.
	logical textmode = TRUE;			//	file format
	outcome result = api_save_entity_list(fp, textmode, elist);
	process(result);

	elist.clear();

	//	Close file *.sat file
	fclose(fp);
}


void 
CAcis::process(outcome result)
{
	if (!result.ok()) {										
		FILE* efp = fopen("apierror.txt","w");				
		print_warnerr_mess("API",result.error_number(), efp);	
		fclose(efp);											
		exit (1);												
	} // end if(!result.ok())
	else 
		return;
}

/*-----------------------------------------------------------*/
//	PROCESS_FILE -- prints out error message if a file pointer
//	returns FALSE or (0) value, which indicates the file was
//	not opened.
/*-----------------------------------------------------------*/
void 
CAcis::process_file(FILE* fp)
{
	if (!fp) {										
		printf("unable to open output file");		
		exit(1);									
	} // end if (!fp)
	else
		return;
}