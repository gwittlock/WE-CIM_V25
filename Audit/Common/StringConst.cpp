
#include "stdafx.h"

// This #define supresses linkage error
#define INCLUDED_BY_STRINGCONST_CPP 1
#include "StringConst.h"


// NOTE: The '~' character is used to identify 'temporary attribute' names
// Such attributes are often removed/filtered during various processes.

extern dllExport const CString STR_AUTO_INDEX			= CString( "Auto_Index" );
// used during clfile generation -- containment depth
extern dllExport const CString STR_CD					= CString( "~cd" );
// used for shape matching while building tool setups on-the-fly
extern dllExport const CString STR_CODE					= CString( "~code" );
extern dllExport const CString STR_COLOR				= CString( "Color" );
extern dllExport const CString STR_CRADIUS				= CString( "Cradius" );
extern dllExport const CString STR_CUTSIDE				= CString( "CutSide" );
extern dllExport const CString STR_DESCRIPTION			= CString( "Description" );
extern dllExport const CString STR_DIAMETER				= CString( "Diameter" );
extern dllExport const CString STR_DU					= CString( "~du" );
extern dllExport const CString STR_DV					= CString( "~dv" );
extern dllExport const CString STR_EMPTY				= CString( "" );
extern dllExport const CString STR_FEED					= CString( "Feed" );
extern dllExport const CString STR_FEEDMODE				= CString( "Feed_Mode" );
extern dllExport const CString STR_FIXED_STATION		= CString( "Fixed_Station" );
extern dllExport const CString STR_HIER					= CString( "~hier" );
extern dllExport const CString STR_INDEX_ANGLE			= CString( "Index_Angle" );
extern dllExport const CString STR_LENGTH				= CString( "Length" );
// used for shape matching while building tool setups on-the-fly
extern dllExport const CString STR_MATCH				= CString( "~match" );
extern dllExport const CString STR_NC_CODE_NUMBER		= CString( "NC_Code_Number" );
extern dllExport const CString STR_ORIENT				= CString( "Orient" );
extern dllExport const CString STR_OX					= CString( "~ox" );
extern dllExport const CString STR_OY					= CString( "~oy" );
extern dllExport const CString STR_PARTPROF				= CString( "PartProf" );
extern dllExport const CString STR_REFCNT				= CString( "~refcnt" );
// a tool attribute indicating that a tool requires an 'auto indexable' station
extern dllExport const CString STR_REQD_AUTO_INDEX		= CString( "Reqd_Auto_Index" );
// a tool attribute indicating a tools required station size
extern dllExport const CString STR_REQD_STATION_SIZE	= CString( "Reqd_Station_Size" );
// used for shape matching while building tool setups on-the-fly
extern dllExport const CString STR_SHAPE				= CString( "~shape" );
// used for shape matching while building tool setups on-the-fly
extern dllExport const CString STR_SHAPEPTR				= CString( "~shapeptr" );
extern dllExport const CString STR_SPEED				= CString( "Speed" );
extern dllExport const CString STR_STATION_ID			= CString( "Station_ID" );
extern dllExport const CString STR_STATION_LOCATION_X	= CString( "Station_Location_X" );
extern dllExport const CString STR_STATION_LOCATION_Y	= CString( "Station_Location_Y" );
extern dllExport const CString STR_STATION_SIZE			= CString( "Station_Size" );
extern dllExport const CString STR_STOCK				= CString( "Stock" );
extern dllExport const CString STR_SUBDEF				= CString( "~subdef" );
extern dllExport const CString STR_SUBDEF_BEGIN			= CString( "~subdef_begin" );
extern dllExport const CString STR_SUBDEF_END			= CString( "~subdef_end" );
extern dllExport const CString STR_SUBU					= CString( "~subu" );
extern dllExport const CString STR_SUBV					= CString( "~subv" );
extern dllExport const CString STR_SUBX					= CString( "~subx" );
extern dllExport const CString STR_SUBY					= CString( "~suby" );
extern dllExport const CString STR_SUBLAYER				= CString( "~sublayer" );
extern dllExport const CString STR_SUBTOOL				= CString( "~subtool" );
extern dllExport const CString STR_SUBWORK				= CString( "~subwork" );
extern dllExport const CString STR_SUB_XMIN				= CString( "~sub_xmin" );
extern dllExport const CString STR_SUB_YMIN				= CString( "~sub_ymin" );
extern dllExport const CString STR_THICKNESS			= CString( "Thickness" );
extern dllExport const CString STR_TOOL_ID				= CString( "Tool_ID" );
extern dllExport const CString STR_TOOL_NEW_ID			= CString( "New_Tool_ID" );
extern dllExport const CString STR_TOP					= CString( "Top" );
extern dllExport const CString STR_TYPE					= CString( "Type" );
extern dllExport const CString STR_TYPE_ID				= CString( "Type_ID" );
extern dllExport const CString STR_UNKNOWN				= CString( "Unknown" );
extern dllExport const CString STR_WIDTH				= CString( "Width" );
extern dllExport const CString STR_WORKPLANE			= CString( "Workplane" );
extern dllExport const CString STR_WORLD				= CString( "World" );
extern dllExport const CString STR_XMAX					= CString( "xmax" );
extern dllExport const CString STR_XMIN					= CString( "xmin" );
extern dllExport const CString STR_YMAX					= CString( "ymax" );
extern dllExport const CString STR_YMIN					= CString( "ymin" );
extern dllExport const CString STR_ZONE					= CString( "Zone" );
extern dllExport const CString STR_NESTPARTNUM			= CString( "NestPartNum" );
extern dllExport const CString STR_NESTPARTROT			= CString( "NestPartRot" );
extern dllExport const CString STR_PART_OUTLINE			= CString( "Part_Outline" );
extern dllExport const CString STR_PROFILE_DEPTH		= CString( "Profile_Depth" );
extern dllExport const CString STR_LEAD_HULL			= CString( "~LeadHull" );
