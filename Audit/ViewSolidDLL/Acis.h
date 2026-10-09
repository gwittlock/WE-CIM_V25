#if !defined(_ACIS_H)
#define _ACIS_H
// ==================================================================================
// Acis.h: interface for the CAcis class.
//
// ==================================================================================

#pragma once

#include "3dCoord.h"

#if !defined(_NO_ACIS)
// ==================================================================================

#include "kernel/acis.hxx"

// ==================================================================================

#include "blend/kernapi/api/blendapi.hxx"

#include "boolean/kernapi/api/boolapi.hxx"
#include "boolean/kernbool/boolean/boolean.hxx"

#include "constrct/kernapi/api/cstrapi.hxx"

#include "faceter/api/af_api.hxx"

#include "ga_husk/attrib/at_str.hxx"
#include "ga_husk/attrib/at_int.hxx"
#include "ga_husk/attrib/at_pos.hxx"
#include "ga_husk/attrib/at_real.hxx"
#include "ga_husk/attrib/at_ent.hxx"
#include "ga_husk/attrib/at_ptr.hxx"

#include "gihusk/api/gi_api.hxx"
#include "gihusk/dl.hxx"
#include "gihusk/grid_def.hxx"
#include "gihusk/pick_evt.hxx "
#include "gihusk/view_utl.hxx"

#include "gihusk/rb_utl.hxx"
#include "gihusk/rb_view.hxx"
#include "gihusk/rb_wndo.hxx"
#include "gihusk/rb_line.hxx"

#include "gihusk/sketch.hxx"

#include "gihusk/view3d.hxx"
#include "gihusk/view3dex.hxx"

#include "gihusk/windows/view3dms.hxx"

#include "gl_husk/api/gl_api.hxx"
#include "gl_husk/gl_ctx.hxx"

#include "intersct/kernapi/api/intrapi.hxx"

#include "kernel/geomhusk/ckoutcom.hxx" 
#include "kernel/geomhusk/efilter.hxx"
#include "kernel/geomhusk/entwray.hxx"
#include "kernel/geomhusk/pick_ray.hxx"
#include "kernel/geomhusk/wcs.hxx"
#include "kernel/geomhusk/wcs_utl.hxx" 

#include "kernel/kernapi/api/api.hxx" 
#include "kernel/kernapi/api/kernapi.hxx"

#include "kernel/kerndata/bulletin/bulletin.hxx" 
#include "kernel/kerndata/data/entity.hxx"
#include "kernel/kerndata/geom/curve.hxx"
#include "kernel/kerndata/geom/point.hxx"
#include "kernel/kerndata/lists/lists.hxx"
#include "kernel/kerndata/top/alltop.hxx"
#include "kernel/kerndata/top/edge.hxx" 

//#include "kernel/kernel_thread_ctx.hxx"
#include "kernel/kernutil/errorsys/errorsys.hxx"

#include "kernel/kerngeom/curve/curdef.hxx"
#include "kernel/kernint/intcucu/intcucu.hxx" 

#include "kernel/kernutil/vector/transf.hxx"
#include "kernel/kernutil/fileio/satfile.hxx"

#include "kernel/logical.h" 

#include "kernel/sg_husk/sweep/swp_spl.hxx"
#include "kernel/sg_husk/sweep/swp_spl.hxx" 

#include "lop_husk/api/lop_api.hxx"

#include "offset/kernapi/api/ofstapi.hxx"

#include "operator/kernapi/api/operapi.hxx"

#include "part/pmhusk/api/part_api.hxx"

#include "pmhusk/api/pm_api.hxx"

#include "sweep/kernapi/api/sweepapi.hxx"

#endif


// ==================================================================================

#define NAME_PREFIX		"bubba_"		// Prefix of all attributes from the default
#define HEADER_PREFIX	"head_"		// Prefix of all attributes that relate to the header
#define HEADER_MARK		"h_"			// Marker that indicates a default attribute paramater is for the header

#define NAME_BUBBA_CENTER	"bubba_center"
#define NAME_CENTER			"center"
#define NAME_COLOR			"toolcolor"
#define NAME_DIAM				"diameter"
#define NAME_DIR				"dir"
#define NAME_FACE				"face"
#define NAME_HOLE_DEPTH		"hole_depth"
#define NAME_MIDPOINT		"middle"
#define NAME_OFFSET			"offset"
#define NAME_PANEL_X			"panel_x"
#define NAME_PANEL_Y			"panel_y"
#define NAME_PANEL_Z			"panel_z"
#define NAME_PROFTOP			"prof_top"
#define NAME_SEQUENCE		"seq_num"
#define NAME_SNAP				"snap"
#define NAME_TOOLING			"tooling"
#define NAME_UID				"uid"

#define NAME_PANEL_COLOR	"h_panel_color"
#define NAME_LAYERMAP		"h_layermap"
#define NAME_OUTLINE			"h_outline"

enum eAttribType
{
	ATTRIB_UNKNOWN,
	ATTRIB_STRING,
	ATTRIB_INT,
	ATTRIB_DOUBLE,
};

// ==================================================================================

#define MATH_APERATURE	0.1

#define COLOR_STOCK		5
#define COLOR_SELECT	1
#define COLOR_TOOL		3
#define COLOR_PLUG		2
#define COLOR_TICK		2


// ==================================================================================

#if !defined(_NO_ACIS)

class dllExport CAcis
{
public:
	CAcis();
	virtual ~CAcis();

	BOOL	Init( void );
	BOOL	Terminate( void );

	BOOL	Import( char* in_name );

	static char* attrib_name_to_label( char* in_name );
	static char* attrib_label_to_name( char* in_label );

	static BOOL set_attrib_string( ENTITY* in_ent, char* in_name, char* in_att );
	static BOOL set_attrib_int( ENTITY* in_ent, char* in_name, int in_att );
	static BOOL set_attrib_double( ENTITY* in_ent, char* in_name, double in_att );
	static BOOL set_attrib_point( ENTITY* in_ent, char* in_name, position* in_att );
	static BOOL set_attrib_pointer( ENTITY* in_ent, char* in_name, ENTITY* in_att );
	static BOOL set_owner( ENTITY* in_ent, char* in_name, ENTITY* in_slave );

	static BOOL		get_attrib_string( ENTITY* in_ent, char* in_name, char** io_att );
	static BOOL		get_attrib_int( ENTITY* in_ent, char* in_name, int* io_att );
	static BOOL		get_attrib_double( ENTITY* in_ent, char* in_name, double* io_att );
	static BOOL		get_attrib_point( ENTITY* in_ent, char* in_name, position* io_att );
	static BOOL		get_attrib_pointer( ENTITY* in_ent, char* in_name, ENTITY** io_att );

	static int			count_attrib( ENTITY* in_ent, BOOL in_header );
	static BOOL			reset_attrib( ENTITY* in_ent, BOOL in_header );
	static BOOL			del_attrib( ENTITY* in_ent, char* in_name );

	static char*		get_attrib_name( ENTITY* in_ent, int in_idx );
	static ATTRIB*		get_attrib_first( ENTITY* in_ent );
	static ATTRIB*		get_attrib_next( ENTITY* in_ent );
	static eAttribType get_attrib_type( ENTITY* in_ent, char* in_name );

	static BOOL CheckOutcome( const outcome& result );

	static void DumpBody( ENTITY* body, char* name );

protected:
	static void process(outcome result);
	static void	process_file(FILE* fp);


	BOOL			m_alive;
	
	static int			m_attrib_num;
	static ATTRIB*		m_attrib_at;
	static ENTITY*		m_attrib_ent;
};

#endif

#endif
