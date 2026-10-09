
#include "stdafx.h"
#include "StringConst.h"
#include "cmn_resource.h"
#include "Path.h"
#include "DaoQuery.h"
#include "Shape.h"
#include "ToolShape.h"
#include "ShapeMatcher.h"
#include "DbTool.h"
#include "EntityDb.h"
#include "DbIterator.h"
#include "ToolSetup.h"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTES:
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// The class CToolSetup was created for the purpose of generating
// tool setups on-the-fly for punching applications, and for replacing
// the previous find-an-empty-station functionality.
//
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

static int DEBUGDUMP = FALSE;

static const int TS_MOVE	= 0x001;
static const int TS_COPY	= 0x002;

static const int MY_DAO_FLAGS = (DAO_REPLACE_WHITE_SPACE | DAO_IGNORE_COMPOUND_NAMES);

static const int TABLE_COUNT = 14;

static const char* TABLE[ TABLE_COUNT ] =
{
	"Machines",
	"Machine Attributes",
	"Machine Attribute Types",
	"Station Configuration",
	"Tool Setup Members",
	"Tool Setups",
	"Tool Crib",
	"Tool Types",
	"Workplanes",
	"Tool Type Attributes",
	"Tool Attribute Types",
	"Material Inventory",
	"Layer Setups",
	"Directives"
};


static const CString STR_ID( "ID" );
static const CString STR_FIELD( "Field" );
static const CString STR_DESCR( "Description" );

static const double SIZE_TOL = 1.e-2;
static const double ALIGNMENT_TOL = 1.e-3;

static int StationIDCompareFunc( const void* myItem, const void* arrayItem );
static int ToolIDCompareFunc( const void* myItem, const void* arrayItem );
static bool IsToolAttrib( int attribID );


////////////////////////////////////////////////////////////////////////

CToolSetup::CToolSetup()
	: m_dbIsOpen( FALSE ),
	  m_machineID( 0 ),
	  m_toolSetupID( 0 ),
	  m_materialID( 0 )
{
}

CToolSetup::~CToolSetup()
{
	m_desc.DestructiveFlush();

	ShapesDestroy( &m_tooledStationShapes );
	ShapesDestroy( &m_unpairedToolShapes );
	ShapesDestroy( &m_unpairedStationShapes );

	m_tools.DestructiveFlush();
	m_stations.DestructiveFlush();
	m_tooledStations.DestructiveFlush();
	m_garbage.DestructiveFlush();
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Initializes this tool setup by using the data in the given cmdb.
//
CReturn
CToolSetup::Init( const CString& dbPath, int toolSetupID, int flags )
{
	CReturn status;

	ShapesDestroy( &m_tooledStationShapes );
	ShapesDestroy( &m_unpairedToolShapes );
	ShapesDestroy( &m_unpairedStationShapes );

	m_tools.DestructiveFlush();
	m_stations.DestructiveFlush();
	m_tooledStations.DestructiveFlush();
	m_garbage.DestructiveFlush();

	status = Open( dbPath );
	if ( status.IsOk() )
	{
		m_toolSetupID = toolSetupID;

		m_toolSetupName = ToolSetupNameGet( toolSetupID );

		m_machineID = MachineGet( toolSetupID );

		if (m_machineID > 0)
		{
			ToolDescriptionsGet();

			if (flags & TS_GET_STATIONS)
				StationsGet();

			if (flags & TS_GET_TOOLS)
				ToolsGet();

			if (flags & TS_GET_TOOLED_STATIONS)
				TooledStationsGet();

			if (DEBUGDUMP) Dump();

			if (flags & TS_COLLATE)
				Reduce();

			if (DEBUGDUMP) Dump();

			if (flags & TS_SHAPES_INIT)
				SortedShapesInit();
		}

		Close();
	}

	if (DEBUGDUMP) DumpSortedShapes();

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Initializes this tool setup by using the tool data of the
// given model database.
//
CReturn
CToolSetup::InitShapesFromDb( const CEntityDb& db )
{
	CReturn status;
	CGeoCurveArray geoCurves ;
	CVarList* attribs;
	CDbTool* dbTool;
	CShape* shape;
	EShape type;
	CDbIterator iter;

	m_tools.DestructiveFlush();
	m_stations.DestructiveFlush();
	m_tooledStations.DestructiveFlush();

	iter.Init( db, DBTOOL );
	while (1)
	{
		dbTool = dynamic_cast<CDbTool*>( iter() );
		if (dbTool == NULL)
			break;

		if ( !dbTool->IsLayer() )
		{
			dbTool->Convert( &geoCurves );

			shape = new CShape();
			shape->Init( geoCurves, FALSE );

			type = shape->Type();
			if (type < SHAPE_TERMINAL)
			{
				attribs = dbTool->pAttrib();

				shape->IntSet(
					STR_STATION_ID, attribs->getInt( STR_STATION_ID, 0 ) );

				shape->IntSet(
					STR_TOOL_ID, attribs->getInt( STR_TOOL_ID, 0 ) );

				shape->IntSet(
					STR_AUTO_INDEX, attribs->getInt( STR_AUTO_INDEX, IUNDEFINED ) );

				// shape->pAttribs()->setReal(
				//  STR_ORIENT, attribs->getReal( STR_INDEX_ANGLE, UNDEFINED ) );

				// 2004.12.25 (PE) -- We record the Type_ID so that MatchingShapesFind()
				// will be able to ignore Forming tools during AutoPunch (Rittal).
				shape->IntSet(
					STR_TYPE_ID, attribs->getInt( STR_TYPE_ID, IUNDEFINED ) );

				m_tooledStationShapes[ type ].Append( shape );
			}

			geoCurves.DestructiveFlush();
		}

		iter.Next();
	}

	return status;
}

// Updates the 'tooled station' shapes to reflect the reference count of
// of the counterparts in a model database.  This is done to eliminate
// references to the model.
//
void
CToolSetup::ToolReferencesUpdate( const CEntityDb& db )
{
	for (int indx = 0; indx < SHAPE_TERMINAL; ++indx)
	{
		EShape type = (EShape) indx;

		TShapeArray& shapes = m_tooledStationShapes[ type ];

		int count = shapes.Count();
		for (int jndx = 0; jndx < count; ++jndx)
		{
			CShape* tooledStation = shapes[ jndx ];

			int stationID = tooledStation->IntGet( STR_STATION_ID, 0 );

			if (stationID > 0)
			{
				CDbTool* dbTool = ToolFind( db, STR_STATION_ID, stationID );
				if (dbTool != NULL)
				{
					tooledStation->IntSet( STR_REFCNT, dbTool->RefCnt() );
				}
			}
		}
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Finds an 'unpaired station' that can accept the given tool.
// NOTE: All 'unpaired stations' are created as round shapes.
//
CShape*
CToolSetup::EmptyFind( double reqdStationSize, double orientation, bool reqAutoIndex )
{
	const TShapeArray& roundStations = m_unpairedStationShapes[ SHAPE_ROUND ];
	CShape* matchingStation = NULL;

	int count = roundStations.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CShape* candidate = roundStations[indx];

		if ( IsMatchingStation( (*candidate), reqdStationSize, orientation, reqAutoIndex ) )
		{
			matchingStation = candidate;
			break;
		}
	}

	return matchingStation;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Finds an unreferenced 'tooled station' whose tool can be replaced with
// the given tool.  'Unreferenced' means the tool has not yet been used
// to generate toolpath.
//
CShape*
CToolSetup::UnrefdFind( double reqdStationSize, double orientation, bool reqAutoIndex )
{
	CShape* matchingStation = NULL;

	for (int indx = 0; indx < SHAPE_TERMINAL; ++indx)
	{
		EShape type = (EShape) indx;

		const TShapeArray& shapes = m_tooledStationShapes[type];

		int count = shapes.Count();
		for (int jndx = 0; jndx < count; ++jndx)
		{
			CShape* candidate = shapes[jndx];

			int refcnt = candidate->IntGet( STR_REFCNT, 0 );
			if (refcnt == 0)
			{
				if ( IsMatchingStation( (*candidate), reqdStationSize, orientation, reqAutoIndex ) )
				{
					matchingStation = candidate;
					break;
				}
			}
		}

		if (matchingStation != NULL)
			break;
	}

	return matchingStation;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Divorce the tool from the given 'tooled station'.  This action
// results in the addition of a new station to the 'unpaired stations'
// and a new tool to the 'unpaired tools'.
//
// ASSUMPTION: We have a legitimate 'tooled station' and the
// associated tool does not exist in the 'unpaired tools'.
//
void
CToolSetup::Divorce( CShape* tooledStation )
{
	int stationID = tooledStation->IntGet( STR_STATION_ID, 0 );
	int toolID = tooledStation->IntGet( STR_TOOL_ID, 0 );

	if (stationID > 0 && toolID > 0)
	{
		CVarList attribs;
		CShape* shape;
		EShape type;

		StationAttribsGet( tooledStation, TS_MOVE, &attribs );
		attribs.setInt( STR_TYPE_ID, TTYPE_OPEN );  // VERY IMPORTANT !
		shape = ShapeCreate( attribs );
		(*(shape->pAttribs())) += attribs;
		type = shape->Type();
		m_unpairedStationShapes[ type ].Append( shape );

		shape = ShapeCreate( tooledStation->Attribs() );
		(*(shape->pAttribs())) += attribs;
		type = shape->Type();
		m_unpairedToolShapes[ type ].Append( shape );
	}
	else
	{
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "CToolSetup::Divorce()" );
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// ASSUMPTIONS: We are given an 'unpaired station' and an 'unpaired tool'.
//
// NOTES:
//
//   As this class was designed for generating on-the-fly tool setups for
//   punching applications, it is unlikely that a given tool (ie. instance of)
//   will be combined with more that one station, even though such circumstances
//   are possible.
//
//   This method was designed for use by CAutoPuncher::AutoPunch().  As such, its
//   behavior may be unsuitable for other uses.  In particular, the given tool
//   is assumed to match some hole shape.  As the tool shape and descriptive
//   attributes have already been generated, the tool is simply amended with
//   the station attributes.  This design provides two things, 1) the tool
//   shape does not have to be regenerated and more importantly 2) the tool
//   will already be classified as a 'tooled station' for the next tool/hole
//   match.  If this explanation is unclear, then consider the case of and
//   obround hole that is decomposed into a rectangle and two circles.  In this
//   case, the two circles would both match with the same round punch.
//
CShape*
CToolSetup::Marry( CShape* station, CShape* tool )
{
	CShape* tooledStation = NULL;

	int stationID = tool->IntGet( STR_STATION_ID, 0 );
	int toolID = station->IntGet( STR_TOOL_ID, 0 );

	if (stationID == 0 && toolID == 0)
	{
		EShape type = tool->Type();

		ShapeRemove( tool );

		// We must preserve the tool type & diameter so ...
		station->AttribDelete( STR_TYPE_ID );
		station->AttribDelete( STR_DIAMETER );

		(*(tool->pAttribs())) += station->Attribs();

		ShapeDispose( station );

		tooledStation = tool;
		m_tooledStationShapes[ type ].Append( tooledStation );
	}
	else
	{
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "CToolSetup::Marry()" );
	}

	return tooledStation;
}

void
CToolSetup::DumpSortedShapes() const
{
	ToolsDump( "Unpaired Tools", m_unpairedToolShapes );
	ToolsDump( "Unpaired Stations", m_unpairedStationShapes );
	ToolsDump( "Tooled Stations", m_tooledStationShapes );
}

void
CToolSetup::Dump() const
{
	AttribsDump( "All Stations", m_stations );
	AttribsDump( "All Tools", m_tools );
	AttribsDump( "All Tooled Stations", m_tooledStations );
}

CReturn
CToolSetup::Open( const CString& dbPath )
{
	CReturn	status;

	if ( m_dbIsOpen )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CToolSetup::Open() missing matching Close()" );
	}
	else
	{
		// Prepare to open the database with the given tables.
		for (int indx = 0; indx < TABLE_COUNT; ++indx)
		{
			m_db.addTable( TABLE[indx] );
		}

		status = m_db.Open( dbPath );

		m_dbIsOpen = status.IsOk();
	}

	return status;
}

void
CToolSetup::Close()
{
	m_db.Close();
	m_dbIsOpen = FALSE;
}

CString
CToolSetup::ToolSetupNameGet( int toolSetupID )
{
	CString sqlQuery;
	CDaoQuery* query;
	CString desc;

	sqlQuery.Format( "SELECT * FROM [Tool Setups] WHERE (ID=%d)", toolSetupID );

	query = m_db.QueryExecute( sqlQuery );

	desc = ((query == NULL) ? "" : query->StringGet( "Description" ));

	delete query;

	return desc;
}

int
CToolSetup::MachineGet( int toolSetupID )
{
	CString sqlQuery;
	CDaoQuery* query;
	int machineID;

	sqlQuery.Format( "SELECT * FROM [Tool Setups] WHERE (ID=%d)", toolSetupID );

	query = m_db.QueryExecute( sqlQuery );

	machineID = ((query == NULL) ? 0 : query->IntGet( "Machine_ID" ));

	delete query;

	return machineID;
}

CReturn
CToolSetup::StationsGet()
{
	CReturn status;

	CDaoQuery* query;
	CString sqlQuery;

	// Get all of the necessary station information, including
	// the workplane that is associated with each station.

	// 2005.11.20 (PE) -- Added Primary Code and Secondary code
	// for Morton.  Prior to this, nobody used the values.
	sqlQuery.Format(
		"SELECT [Station Configuration].[ID],"					\
		"[Station Configuration].[NC Code Number],"				\
		"[Station Configuration].[Primary Code],"				\
		"[Station Configuration].[Secondary Code],"				\
		"[Station Configuration].[Station Size],"				\
		"[Station Configuration].[Station Location X],"			\
		"[Station Configuration].[Station Location Y],"			\
		"[Station Configuration].[Auto Index],"					\
		"Workplanes.Description AS Workplane "					\
		"FROM Workplanes INNER JOIN [Station Configuration] "	\
		"ON Workplanes.ID = [Station Configuration].[Workplane ID] "	\
		"WHERE ([Station Configuration].[Machine ID]=%d) "		\
		"ORDER BY [Station Configuration].ID", m_machineID );

	query = m_db.QueryExecute( sqlQuery );
	if (query != NULL)
	{
		CVarList* attribs;
		double stationSize;
		double BOGUS = 1.e-3;

		int count = query->RecordCount();
		for (int indx = 0; indx < count; ++indx)
		{
			query->Move( ((indx == 0) ? 0 : 1) );

			attribs = new CVarList();
			query->AttributesCopy( attribs, MY_DAO_FLAGS );

			attribs->VarRename( STR_ID, STR_STATION_ID );

			// By default, assume the station is not fixed and has a zero index angle.
			// Any actual values might be set later, as set in the tool setup.

			attribs->setInt( STR_FIXED_STATION, FALSE );
			attribs->setReal( STR_INDEX_ANGLE, 0. );

			// Setting the 'Type_ID' and 'Diameter' attributes ensures
			// the station is created as a round shape.  These attributes
			// are removed after the station shape has been created.
			//
			// Note: We must provide a non-zero diameter.
			// Otherwise, the station geometry will be invalid.
			attribs->setInt( STR_TYPE_ID, TTYPE_OPEN );

			stationSize = attribs->getReal( STR_STATION_SIZE, 0. );
			attribs->setReal( STR_DIAMETER, ((stationSize < BOGUS) ? BOGUS : stationSize) );

			m_stations.Append( attribs );
		}

		delete query;

		ToolSetupInfoAppend();
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CToolSetup::StationsGet()" );
	}

	return status;
}

CReturn
CToolSetup::ToolsGet()
{
	CReturn status;

	CDaoQuery* query;
	CString sqlQuery;

	// NOTE: We must explicitly request [Tool Crib].ID and
	// [Tool Crib].Description.  Doing so returns those values
	// as [Tool Crib].ID and [Tool Crib].Description instead
	// of ID and Description.  This is critical to the success
	// of SpecificToolAttributesAdd().
	//
	sqlQuery.Format(
		"SELECT DISTINCT [Tool Crib].*, "					\
		"[Tool Crib].ID, "									\
		"[Tool Crib].Description "							\
		"FROM [Tool Crib] INNER JOIN [Machine Tools] "		\
		"ON [Tool Crib].ID = [Machine Tools].[Tool ID] "	\
		"WHERE ([Machine Tools].[Machine ID]=%d) "			\
		"ORDER BY [Tool Crib].ID", m_machineID );

	query = m_db.QueryExecute( sqlQuery );

	if (query != NULL)
	{
		CVarList* attribs;
		int toolTypeID;

		int count = query->RecordCount();
		for (int indx = 0; indx < count; ++indx)
		{
			query->Move( ((indx == 0) ? 0 : 1) );

			toolTypeID = query->IntGet( STR_TYPE_ID );

			attribs = new CVarList();

			status = SpecificToolAttributesAdd( toolTypeID, (*query), TRUE, attribs );

			m_tools.Append( attribs );
		}

		delete query;
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CToolSetup::ToolsGet()" );
	}

	return status;
}

// Cloned from v14.5 CAutoModDb::ToolSetupValuesGet()
CReturn
CToolSetup::TooledStationsGet()
{
	CReturn status;

	CDaoQuery* query;
	CString sqlQuery;

	// NOTE: To prevent collisions, [Tool Crib].ID, [Tool Crib].Description
	//       and Workplanes.Description are explicitly requested.
	//
	// 2005.11.20 (PE) -- Added Primary Code and Secondary code
	// for Morton.  Prior to this, nobody used the values.
	sqlQuery.Format(
		"SELECT [Station Configuration].[NC Code Number],"	\
		"[Station Configuration].[Primary Code],"			\
		"[Station Configuration].[Secondary Code],"			\
		"[Station Configuration].[Station Size],"			\
		"[Station Configuration].[Station Location X],"		\
		"[Station Configuration].[Station Location Y],"		\
		"[Station Configuration].[Auto Index],"				\
		"[Tool Setup Members].[Station ID],"				\
		"[Tool Setup Members].[Tool ID],"					\
		"[Tool Setup Members].[Fixed Station],"				\
		"[Tool Setup Members].[Index Angle],"				\
		"[Tool Crib].*,"									\
		"[Tool Crib].ID, "									\
		"[Tool Crib].Description, "							\
		"Workplanes.Description "							\
		"FROM Workplanes INNER JOIN "						\
		"([Tool Types] INNER JOIN "							\
		"([Tool Crib] INNER JOIN "							\
		"([Station Configuration] INNER JOIN "				\
		"[Tool Setup Members] ON "							\
		"[Station Configuration].ID = [Tool Setup Members].[Station ID]) ON "	\
		"[Tool Crib].ID = [Tool Setup Members].[Tool ID]) ON "					\
		"[Tool Types].ID = [Tool Crib].[Type ID]) ON "							\
		"Workplanes.ID = [Station Configuration].[Workplane ID] "				\
		"WHERE (([Tool Setup Members].[Tool Setup ID]=%d)) "					\
		"ORDER BY [Tool Setup Members].[ID]", m_toolSetupID );

	query = m_db.QueryExecute( sqlQuery );
	if (query != NULL)
	{
		CVarList* attribs;
		int toolTypeID, stationID;

		int count = query->RecordCount();
		for (int indx = 0; indx < count; ++indx)
		{
			query->Move( ((indx == 0) ? 0 : 1) );

			toolTypeID = query->IntGet( STR_TYPE_ID );

			stationID = query->IntGet( STR_STATION_ID );

			attribs = new CVarList();
			attribs->setInt( STR_STATION_ID, stationID );

			// Whereas SpecificToolAttributesAdd() would normally do this
			// work, making it do so requires a nasty database change :-(
			attribs->setString( "Primary_Code", query->StringGet( "Primary Code" ) );
			attribs->setString( "Secondary_Code", query->StringGet( "Secondary Code" ) );

			status = SpecificToolAttributesAdd( toolTypeID, (*query), FALSE, attribs );

			m_tooledStations.Append( attribs );
		}

		delete query;
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CToolSetup::TooledStationsGet()" );
	}

	return status;
}

CReturn
CToolSetup::ToolSetupInfoAppend()
{
	CReturn status;

	CDaoQuery* query;
	CString sqlQuery;

	// Get all of the necessary station information, including
	// the workplane that is associated with each station.

	sqlQuery.Format(
		"SELECT [Tool Setup Members].[Station ID],"			\
		"[Tool Setup Members].[Fixed Station],"				\
		"[Tool Setup Members].[Index Angle] "				\
		"FROM [Tool Setup Members] "						\
		"WHERE ([Tool Setup Members].[Tool Setup ID]=%d) "	\
		"ORDER BY [Tool Setup Members].[Station ID]", m_toolSetupID );

	query = m_db.QueryExecute( sqlQuery );
	if (query != NULL)
	{
		CVarList* attribs;
		int stationID, sndx;
		bool found;

		int count = query->RecordCount();
		for (int indx = 0; indx < count; ++indx)
		{
			query->Move( ((indx == 0) ? 0 : 1) );

			stationID = query->IntGet( STR_STATION_ID );

			found = m_stations.BinarySearch( (void*) &stationID, &StationIDCompareFunc, &sndx );
			if ( found )
			{
				attribs = m_stations[sndx];
				attribs->setInt( STR_FIXED_STATION, query->IntGet( STR_FIXED_STATION ) );
				attribs->setReal( STR_INDEX_ANGLE, query->DoubleGet( STR_INDEX_ANGLE ) );
			}
		}

		delete query;
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CToolSetup::ToolSetupInfoAppend()" );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// For each populated station, remove the station and tool from their
// respective lists.  As the result, allStations will represent all
// empty stations and allTools will represent all unreferenced tools.
//
CReturn
CToolSetup::Reduce()
{
	CReturn status;

	const CVarList* tooledStationAttribs;
	int sndx, tndx, stationID, toolID;
	bool found;

	int count = m_tooledStations.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		tooledStationAttribs = m_tooledStations[indx];

		stationID = tooledStationAttribs->getInt( STR_STATION_ID, 0 );
		toolID = tooledStationAttribs->getInt( STR_TOOL_ID, 0 );

		if (stationID > 0 && toolID > 0)
		{
			found = m_stations.BinarySearch( (void*) &stationID, &StationIDCompareFunc, &sndx );
			if ( !found ) sndx = -1;

			found = m_tools.BinarySearch( (void*) &toolID, &ToolIDCompareFunc, &tndx );
			if ( !found ) tndx = -1;

			// Note: It is possible for a tool to be loaded into more than
			// one station.  As such, it is at times possible to find the
			// station but not the tool.

			if (sndx >= 0)
				delete m_stations.Remove( sndx );

			if (tndx >= 0)
				delete m_tools.Remove( tndx );
		}
		else
		{
			status.Internal( IDS_INTERNAL_ERROR, "CToolSetup::Reduce()" );
		}
	}

	return status;
}

// Generate the shapes
CReturn
CToolSetup::SortedShapesInit()
{
	CReturn status;

	CToolShape tshape;

	CShape* shape;
	EShape type;
	const CVarList* attribs;
	int count, indx;

	count = m_tooledStations.Count();
	for (indx = 0; indx < count; ++indx)
	{
		attribs = m_tooledStations[indx];

		shape = ShapeCreate( (*attribs) );
		(*(shape->pAttribs())) += (*attribs);

		type = shape->Type();
		m_tooledStationShapes[ type ].Append( shape );
	}

	count = m_tools.Count();
	for (indx = 0; indx < count; ++indx)
	{
		attribs = m_tools[indx];

		shape = ShapeCreate( (*attribs) );
		(*(shape->pAttribs())) += (*attribs);

		type = shape->Type();
		m_unpairedToolShapes[ type ].Append( shape );
	}

	count = m_stations.Count();
	for (indx = 0; indx < count; ++indx)
	{
		attribs = m_stations[indx];

		shape = ShapeCreate( (*attribs) );
		(*(shape->pAttribs())) += (*attribs);

		shape->AttribDelete( STR_CODE );
		shape->AttribDelete( STR_SHAPE );
		shape->AttribDelete( STR_TYPE_ID );
		shape->AttribDelete( STR_DIAMETER );
		shape->AttribDelete( STR_OX );
		shape->AttribDelete( STR_OY );

		type = shape->Type();
		m_unpairedStationShapes[ type ].Append( shape );
	}

	return status;
}

CReturn
CToolSetup::SpecificToolAttributesAdd(
							int			toolTypeID,
							CDaoQuery&	toolQuery,
							bool		filter,
							CVarList*	attribs )
{
	CReturn status;

	CDaoQuery* query;
	CString sqlQuery;

	// This query is used to get a list of all tool attributes
	// that should be assigned to this tool.
	//
	// NOTE: We may mant to change the reference from 'field' to '*'
	// Why?  Dunno, have to ask Rick.
	//
	sqlQuery.Format(
		"SELECT [Tool Attribute Types].* "		\
		"FROM [Tool Types] INNER JOIN "			\
		"([Tool Attribute Types] INNER JOIN "	\
		"[Tool Type Attributes] ON "			\
		"[Tool Attribute Types].ID = [Tool Type Attributes].[Attribute Type ID]) ON "\
		"[Tool Types].ID = [Tool Type Attributes].[Tool Type  ID] " \
		"WHERE ((([Tool Types].ID)=%d))",
				toolTypeID );

	query = m_db.QueryExecute( sqlQuery );

	if (query != NULL)
	{
		// Loop through record set fields 
		CString toolTypeDesc;
		CString fieldName;
		CString attribName;
		CString value;
		bool okay;

		attribs->setInt( STR_TYPE_ID, toolTypeID );

		toolTypeDesc = ToolTypeDescription( toolTypeID );
		attribs->setString( STR_TYPE, toolTypeDesc );

		int count = query->RecordCount();
		for (int indx = 0; indx < count; ++indx)
		{
			query->Move( ((indx == 0) ? 0 : 1) );

			okay = (( filter ) ? IsToolAttrib( query->IntGet( STR_ID ) ) : TRUE);

			if ( okay )
			{
				fieldName = query->StringGet( STR_FIELD );
				attribName = query->StringGet( STR_DESCR );
				if (attribName.GetLength() > 0)
				{
					value = toolQuery.StringGet( fieldName );
					if ( !value.IsEmpty() )
						attribs->setString( attribName, value );
				}
			}
		}

		delete query;
	}
	else
	{
		status = STATUS_ERROR;
	}

	return status;
}

CString
CToolSetup::ToolTypeDescription( int toolTypeID )
{
	return (*(m_desc[toolTypeID-1]));
}

CReturn
CToolSetup::ToolDescriptionsGet()
{
	CReturn status;

	CDaoQuery* query;
	CString sqlQuery;

	sqlQuery.Format( "SELECT * FROM [Tool Types]" );

	query = m_db.QueryExecute( sqlQuery );

	if (query != NULL)
	{
		int count = query->RecordCount();
		for (int indx = 0; indx < count; ++indx)
		{
			query->Move( ((indx == 0) ? 0 : 1) );

			// Note: The array index is the 'Tool Type ID'.
			m_desc.Append( new CString( query->StringGet( "Description" ) ) );
		}

		delete query;
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CToolSetup::ToolDescriptionsGet()" );
	}

	return status;
}

int StationIDCompareFunc( const void* myItem, const void* arrayItem )
{
	int toolNum = *((int*) myItem);
	CVarList* attribs = ((CVarList*) arrayItem);

	int candidateNum = attribs->getInt( STR_STATION_ID, 0 );
	int diff = candidateNum - toolNum;

	if (candidateNum <= 0)
	{
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "StationIDCompareFunc()" );
	}

	return diff;
}

int ToolIDCompareFunc( const void* myItem, const void* arrayItem )
{
	int toolID = *((int*) myItem);
	CVarList* attribs = ((CVarList*) arrayItem);

	int candidateID = attribs->getInt( STR_TOOL_ID, 0 );
	int diff = candidateID - toolID;

	if (candidateID <= 0)
	{
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "ToolIDCompareFunc()" );
	}

	return diff;
}

bool IsToolAttrib( int attribID )
{
	// Using the 'Tool Attribute Types' table of the cmdb.
	switch (attribID)
	{
	case 11:		// Index_Angle
	//case 16:		// Description
		return FALSE;
	case 29:		// Reqd_Station_Size
	case 30:		// Type_ID
	case 31:		// Reqd_Auto_Index
		return TRUE;
	default:
		return (attribID < 21);
	}
}

void
CToolSetup::ToolsDump( const CString& title, const TSortedShapes& tools ) const
{
	CReturn status;

	status.Diagnostic( title );

	for (int indx = 0; indx < SHAPE_TERMINAL; ++indx)
	{
		const TShapeArray& tshapes = tools[indx];

		int count = tshapes.Count();
		for (int jndx = 0; jndx < count; ++jndx)
		{
			CShape* tshape = tshapes[jndx];
			AttribsDump( tshape->Attribs() );
		}
	}
}

void
CToolSetup::AttribsDump( const CString& title, const TArrayOfAttribLists& taal ) const
{
	CReturn status;
	CString msg;

	int count = taal.Count();

	msg.Format( "%s (%d)", title, count );
	status.Diagnostic( msg );

	for (int indx = 0; indx < count; ++indx)
	{
		const CVarList* attribs = taal[indx];
		AttribsDump( (*attribs) );
	}
}

void
CToolSetup::AttribsDump( const CVarList& attribs ) const
{
	int num = attribs.countVar();
	if (num > 0)
	{
		CReturn status;
		CString tmp;

		CString text = "   ";
		for (int idx=0; idx<num; idx++)
		{
			CVar* var = attribs.getVar(idx);

			const CString& name = var->getName();

			tmp.Format( " %s=%s,", name, var->getString() );

			text += tmp;
			if (text.GetLength() >= 80)
			{
				status.Diagnostic( text );
				text = "       ";
			}
		}

		if (text.GetLength() > 0)
			status.Diagnostic( text );
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Moves 'station parameters' from the 'tooled station' to the given list.
//
void
CToolSetup::StationAttribsGet( CShape* tooledStation, int flags, CVarList* stationAttribs )
{
	CVarList* attribs = tooledStation->pAttribs();

	int indx = 0;

	while (indx < attribs->countVar())
	{
		CVar* attrib = attribs->getVar( indx );

		if ( !IsStationAttrib( (*attrib) ) )
		{
			if (flags & TS_MOVE)
			{
				attrib = attribs->Remove( indx );
				stationAttribs->Insert( attrib );
			}
			else if (flags & TS_COPY)
			{
				stationAttribs->setVar( (*attrib) );
				++indx;
			}
		}
		else
		{
			++indx;
		}
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// See also SELECT statement in CToolSetup::StationsGet()
//
bool
CToolSetup::IsStationAttrib( const CVar& attrib )
{

	const CString& name = attrib.getName();

	bool isStationAttrib =
					( (name.CompareNoCase( STR_STATION_ID ) == 0)			||
					  (name.CompareNoCase( STR_NC_CODE_NUMBER ) == 0)		||
					  (name.CompareNoCase( STR_STATION_SIZE ) == 0)		||
					  (name.CompareNoCase( STR_STATION_LOCATION_X ) == 0)	||
					  (name.CompareNoCase( STR_STATION_LOCATION_Y ) == 0)	||
					  (name.CompareNoCase( STR_AUTO_INDEX ) == 0)			||
					  (name.CompareNoCase( STR_WORKPLANE ) == 0) );

	return isStationAttrib;
}

CShape*
CToolSetup::ShapeCreate( const CVarList& attribs )
{
	CToolShape toolShape;
	CGeoCurveArray curves;
	CShape* theShape;

	toolShape.Init( attribs );
	toolShape.Convert( &curves );

	theShape = new CShape();
	theShape->Init( curves, FALSE );

	curves.DestructiveFlush();

	return theShape;
}

void
CToolSetup::ShapeDispose( CShape* shape )
{
	CShape* tmp = ShapeRemove( shape );
	if (tmp == shape)
	{
		m_garbage.Append( shape );
	}
	else
	{
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "CToolSetup::ShapeDispose()" );
	}
}

CShape*
CToolSetup::ShapeRemove( CShape* shape )
{
	CShape* theShape = NULL;

	int stationID = shape->IntGet( STR_STATION_ID, 0 );
	int toolID = shape->IntGet( STR_TOOL_ID, 0 );

	if (stationID > 0 && toolID > 0)
	{
		theShape = ShapeRemove( shape, &m_tooledStationShapes );
	}
	else if (stationID > 0)
	{
		theShape = ShapeRemove( shape, &m_unpairedStationShapes );
	}
	else if (toolID > 0)
	{
		theShape = ShapeRemove( shape, &m_unpairedToolShapes );
	}
	else
	{
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "CToolSetup::ShapeRemove()" );
	}

	return theShape;
}

CShape*
CToolSetup::ShapeRemove( CShape* shape, TSortedShapes* magazine )
{
	CShape* theShape = NULL;

	EShape type = shape->Type();
	TShapeArray& shapes = (*magazine)[ type ];

	int count = shapes.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		CShape* candidate = shapes[indx];

		if (candidate == shape)
		{
			theShape = shapes.Remove( indx );
			break;
		}
	}

	return theShape;
}

// Cloned from Policy/ModelUtil.cpp
// NOTE: Find a tool base upon some unique attribute
CDbTool*
CToolSetup::ToolFind( const CEntityDb& db, const CString& field, int value )
{
	CDbIterator iter;

	if (value < 0)
		return NULL;

	iter.Init( db, DBTOOL );
	while (1)
	{
		CDbTool* dbTool = dynamic_cast<CDbTool*>( iter() );
		if (dbTool == NULL)
			break;

		int ival = dbTool->Attrib().getInt( field, IUNDEFINED );
		if (ival == value)
			return dbTool;

		iter.Next();
	}

	return NULL;
}

bool
CToolSetup::IsMatchingStation(
	const CShape&	station,
	double			reqdStationSize,
	double			reqdOrientation,
	bool			reqdAutoIndex )
{
	bool isMatch = FALSE;
	
	double stationSize = station.DoubleGet( STR_STATION_SIZE, 0. );

	if ( (fabs( reqdStationSize - stationSize ) < SIZE_TOL)  ||  // standard case
		 (reqdStationSize < SMALL) )  // odd case -- size doesn't matter ;-)
	{
		if ( reqdAutoIndex )
		{
			isMatch = (station.IntGet( STR_AUTO_INDEX, FALSE ) != FALSE);
		}
		else
		{
			double stationOrientation = station.DoubleGet( STR_INDEX_ANGLE, 0. );

			isMatch = (fabs( reqdOrientation - stationOrientation ) <= ALIGNMENT_TOL);
		}
	}

	return isMatch;
}

void
CToolSetup::ShapesDestroy( TSortedShapes* catalog )
{
	for (int indx = 0; indx < SHAPE_TERMINAL; ++indx)
	{
		TShapeArray& shapes = (*catalog)[indx];
		shapes.DestructiveFlush();
	}
}
