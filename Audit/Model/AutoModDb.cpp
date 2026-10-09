
#include <stdafx.h>
#include "cmn_resource.h"
#include "MathConst.h"
#include "StringConst.h"

#include "Path.h"
#include "VarList.h"
#include "AutoModDb.h"
#include "DaoQuery.h"
#include "DbEntity.h"

const int DIRECTIVES_TABLE = 13;
const int TABLE_COUNT = 14;
const int MY_DAO_FLAGS = (DAO_REPLACE_WHITE_SPACE | DAO_IGNORE_COMPOUND_NAMES);

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

static CString CAD_LAYER( "CAD_Layer" );
static CString CAM_LAYER( "CAM_Layer" );

int CAutoModDb::g_cadLayerIndx = -1;
int CAutoModDb::g_camLayerIndx = -1;


// ============================================================================

CAutoModDb::CAutoModDb()
	: m_db()
{
	m_dbIsOpen = FALSE;
	m_toolSetupId = 0;
	m_materialId = 0;
	m_currRecIndx = -1;
	m_allow_torch_piercing = false;
}

CAutoModDb::~CAutoModDb()
{
	m_table.DestructiveFlush();
	m_layerNames.DestructiveFlush();
	m_tool_kerf_map.RemoveAll();
}

CReturn
CAutoModDb::DbOpen( const CString& dbFilePath  )
{
	CReturn	status;

	m_table.DestructiveFlush();
	m_table.ReallocationIncrement( 20 );
	m_layerNames.DestructiveFlush();

	m_machineAttribs.Reset();
	m_materialAttribs.Reset();
	m_layerSetupAttribs.Reset();

	m_cachedSourceName = "";
	m_cachedTargetName = "";
	m_cachedMoveToName = "";

	m_currRecIndx = -1;
	g_cadLayerIndx = -1;
	g_camLayerIndx = -1;

	// Prepare to open the database with the given tables.
	for (int indx = 0; indx < TABLE_COUNT; ++indx)
	{
		m_db.addTable( TABLE[indx] );
	}

	m_toolSetupId = 0;
	m_toolSetupName.Empty();
	m_materialId = 0;

	status = m_db.Open( dbFilePath );

	m_dbIsOpen = status.IsOk();

	return status;
}

void
CAutoModDb::DbClose()
{
	m_db.Close();
	m_dbIsOpen = FALSE;
}

// NOTE: Post V12, the automod stuff is used for previewing files.
// As such, it should be independent of any machining database settings,
// ie. the toolSetupId, layerSetupId and materialId will be zero.
CReturn
CAutoModDb::Init(
				const CString&	dbPath,
				int				toolSetupId,
				int				layerSetupId,
				int				materialId,
				bool			buildToolSetup )
{
	CReturn status;
	
	m_allow_torch_piercing = false;
	m_tool_kerf_map.RemoveAll();

	bool localOpen = ( !m_db.IsOpen() );
	if ( localOpen )
		status = DbOpen( dbPath );

	if ( status.IsOk() )
	{
		m_toolSetupId = ((toolSetupId > 0) ? toolSetupId : 0);

		if ( !IsPreview() )
			status = MachineValuesGet( toolSetupId );

		if ( status.IsOk() && !IsPreview() )
		{
			int flags;
			
			if ( buildToolSetup )
				// flags = (TS_GET_TOOLED_STATIONS | TS_GET_STATIONS | TS_COLLATE | TS_SHAPES_INIT);
				flags = TS_ALL_FLAGS;
			else
				flags = (TS_GET_TOOLED_STATIONS | TS_SHAPES_INIT);

			m_toolSetup.Init( dbPath, toolSetupId, flags );
		}

		if ( status.IsOk() )
			status = LayerSetupValuesGet( layerSetupId );

		if ( status.IsOk() && !IsPreview() )
			status = MaterialValuesGet( materialId );

		if ( status.IsOk() )
			AllowTorchPiercingInit();
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CAutoModDb::Init() database not open" );
	}

	if ( localOpen )
		DbClose();

	return status;
}

bool
CAutoModDb::IsPreview() const
{
	return (m_toolSetupId <= 0);
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Converts the given parameters, converts them to toolSetupId & materialId
// and then calls the overloaded method CAutoModDb::Init().
//
// As this method was developed to support the applications' File/New (and Java
// equivalent Model.Init()) a layer setup is not required.  Therefore, the
// layerSetupId is set to zero.
//
CReturn
CAutoModDb::Init(
				const CString&	dbPath,
				CString&		machineName,
				CString&		toolSetupName,
				CString&		materialName,
				bool			buildToolSetup )
{
	CReturn status;
	
	bool localOpen = ( !m_db.IsOpen() );
	if ( localOpen )
		status = DbOpen( dbPath );

	if ( status.IsOk() )
	{
		int toolSetupId = ToolSetupId( machineName, toolSetupName );
		int materialId = MaterialId( materialName );

		// 2004.05.13 (PE) -- Whilst dealing with nesting on remnants, we
		// discovered failures when materialId=0.  Ultimately, however,
		// the materialId is used simply to obtain the material thickness
		// and/or to update the model header with the material name.
		//
		// Now, we have decided to allow a materialId=0 and (in known cases)
		// VB will update the material name post-facto.
		//
		//     if (toolSetupId > 0 && materialId > 0)
		if (toolSetupId > 0)
		{
			status = Init( dbPath, toolSetupId, 0, materialId, buildToolSetup );
		}
		else
		{
			status = STATUS_ERROR;
		}
	}
	else
	{
		status = STATUS_ERROR;
	}

	if ( localOpen )
		DbClose();

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CAutoModDb::Init() database not open" );

	return status;
}

int
CAutoModDb::Units() const
{
	int units = m_machineAttribs.getInt( "Units", IUNDEFINED );
	return units;
}

int
CAutoModDb::WorkplaneType() const
{
	int wpType = 1;
	
	if ( !IsPreview() )
		wpType = m_machineAttribs.getInt( "Workplane_Type_ID", IUNDEFINED );

	return wpType;
}

int
CAutoModDb::MachineType() const
{
	int type = m_machineAttribs.getInt( STR_TYPE_ID, IUNDEFINED );
	return type;
}

double
CAutoModDb::HoleMinusTol() const
{
	double tol = m_machineAttribs.getReal( "Hole_Minus_Tolerance", UNDEFINED );
	if (tol < SMALL)
		tol = SMALL;
	return fabs( tol );
}

double
CAutoModDb::HolePlusTol() const
{
	double tol = m_machineAttribs.getReal( "Hole_Plus_Tolerance", UNDEFINED );
	if (tol < SMALL)
		tol = SMALL;
	return fabs( tol );
}

double
CAutoModDb::SpaceTol() const
{
	double tol = m_machineAttribs.getReal( "Space_Tolerance", UNDEFINED );
	return tol;
}

int
CAutoModDb::NumberOfClamps() const
{
	int num = m_machineAttribs.getInt( "Number_Of_Clamps", 0 );
	return num;
}

bool
CAutoModDb::CodePartProf() const
{
	bool flag = (m_machineAttribs.getInt( "Code_Part_Profile", FALSE ) != FALSE);
	return flag;
}

CString
CAutoModDb::MaterialDesc() const
{
	CString desc = m_materialAttribs.getString( "Description", "" );
	return desc;
}

double
CAutoModDb::MaterialLength() const
{
	double length = m_materialAttribs.getReal( STR_LENGTH, UNDEFINED );
	return length;
}

double
CAutoModDb::MaterialWidth() const
{
	double width = m_materialAttribs.getReal( STR_WIDTH, UNDEFINED );
	return width;
}

double
CAutoModDb::MaterialThick() const
{
	double thick = m_materialAttribs.getReal( STR_THICKNESS, UNDEFINED );
	return thick;
}

EAutoModMode
CAutoModDb::ZLevelMode() const
{
	EAutoModMode mode = (EAutoModMode) m_layerSetupAttribs.getInt( "Z_Level_Mode", AUTOMOD_USE_CAD_Z );
	return mode;
}

double
CAutoModDb::GapTol() const
{
	return ( m_layerSetupAttribs.getReal( "Gap_Tolerance", SMALL ) );
}

double
CAutoModDb::CleanTol() const
{
	return ( m_layerSetupAttribs.getReal( "Clean_Tolerance", SMALL ) );
}

double
CAutoModDb::SharpAngle() const
{
	return ( m_layerSetupAttribs.getReal( "Sharp_Angle", 90. ) );
}

double
CAutoModDb::FilterTol() const
{
	double tol = m_layerSetupAttribs.getReal( "Filter_Tolerance", SMALL );
	return ((tol < SMALL) ? SMALL : tol);
}

bool
CAutoModDb::RestrictOffset() const
{
	int ival = m_layerSetupAttribs.getInt( "Restrict_Offset", FALSE );
	return (ival != 0);
}

int
CAutoModDb::Count() const
{
	return m_table.Count();
}

CReturn
CAutoModDb::CurrentRecordSet( int recIndx ) const
{
	CReturn status;

	if (recIndx < 0 || recIndx >= m_table.Count())
	{
		status = STATUS_ERROR;
	}
	else if (recIndx != m_currRecIndx)
	{
		((CAutoModDb*) this)->m_currRecIndx = recIndx;
	}

	return status;
}

int
CAutoModDb::IntGet( const CString& fieldName ) const
{
	CVarList* attribs = m_table[m_currRecIndx];
	int value = attribs->getInt( fieldName, IUNDEFINED );
	return value;
}

bool
CAutoModDb::BoolGet( const CString& fieldName ) const
{
	CVarList* attribs = m_table[m_currRecIndx];
	int value = attribs->getInt( fieldName, 0 );
	return (value != 0);
}

double
CAutoModDb::DoubleGet( const CString& fieldName ) const
{
	CVarList* attribs = m_table[m_currRecIndx];
	double value = attribs->getReal( fieldName, UNDEFINED );
	return value;
}

CString
CAutoModDb::StringGet( const CString& fieldName ) const
{
	CVarList* attribs = m_table[m_currRecIndx];
	CString value = attribs->getString( fieldName, "" );
	return value;
}

int
CAutoModDb::SourceLayerFind( const CString& layerName ) const
{
	int count = m_table.Count();
	if (count == 1)
	{
		CString thisLayerName = CadLayerName( m_table[0] );
		if (thisLayerName.CompareNoCase( "*default*" ) == 0)
			return 0;
	}

	int indx = IUNDEFINED;
	bool found = m_table.BinarySearch( (void*) &layerName, &BinarySearchCompare, &indx );

	return (found ? indx : -1);
}

const CString
CAutoModDb::TargetName() const
{
	CString targetName = "";

	if (m_currRecIndx >= 0 && m_currRecIndx <= m_table.Count())
	{
		targetName = CamLayerName( m_table[m_currRecIndx] );
	}

	return targetName;
}

C2dBox
CAutoModDb::MachineLimits() const
{
	double xMin = UNDEFINED;
	double yMin = UNDEFINED;
	double xMax = UNDEFINED;
	double yMax = UNDEFINED;

	m_machineAttribs.getReal( "Min_Travel_Limit_X", &xMin );
	m_machineAttribs.getReal( "Min_Travel_Limit_Y", &yMin );
	m_machineAttribs.getReal( "Max_Travel_Limit_X", &xMax );
	m_machineAttribs.getReal( "Max_Travel_Limit_Y", &yMax );

	C2dBox box( xMin, yMin, xMax, yMax );
	return box;
}

CString
CAutoModDb::MachineName() const
{
	return ( m_machineAttribs.getString( "Description", "" ) );
}

CReturn
CAutoModDb::SpeedFeed( int toolTypeId, double diam, CVarList* attribs ) const
{
	CString sqlQuery;
	CReturn status;

	sqlQuery.Format(
		"SELECT [Material Attributes].* FROM [Tool Types] " \
		"INNER JOIN ([Material Inventory] " \
		"INNER JOIN [Material Attributes] " \
		"ON [Material Inventory].ID = [Material Attributes].[Material ID]) " \
		"ON [Tool Types].ID = [Material Attributes].[Tool Type ID] " \
		"WHERE ((([Material Attributes].[Diameter Limit])>=%-6.4f) " \
		"AND (([Material Inventory].ID)=%d) " \
		"AND (([Tool Types].ID)=%d)) " \
		"ORDER BY [Material Attributes].[Diameter Limit]",
				diam, m_materialId, toolTypeId );

	CDaoQuery* query = m_db.QueryExecute( sqlQuery );

	if (query != NULL)
	{
		query->AttributesCopy( attribs, MY_DAO_FLAGS );
		delete query;
	}
	else
		status = STATUS_ERROR;

	return status;
}

CReturn
CAutoModDb::MachineValuesGet( int toolSetupId )
{
	CReturn status;

	CString sqlQuery;
	CString attribName;
	CString attribValue;
	CDaoQuery* query = NULL;

	if ( m_dbIsOpen )
	{
		// First, get the machine id using the tool setup id.
		int machineId = 0;

		sqlQuery.Format(
			"SELECT * FROM [Tool Setups] WHERE (ID=%d)", toolSetupId );

		query = m_db.QueryExecute( sqlQuery );
		if (query != NULL)
		{
			machineId = query->IntGet( "Machine ID" );
			// TODO: ?? m_toolSetupAttribs.setString( "Description", query->StringGet( "Description" ) );
		}
		delete query;

		// Now, get the machine information.
		if (machineId < 1)
		{
			status = STATUS_ERROR;
		}
		else
		{
			status = MachineValuesLoad( machineId );
		}
	}

	return status;
}

int
CAutoModDb::MachineID( const CString& machineName )
{
	CString sqlQuery;
	CDaoQuery* query;

	int machineID = 0;

	if ( m_dbIsOpen )
	{
		sqlQuery.Format( "SELECT * FROM Machines WHERE ([Description]=\"%s\")", machineName );

		query = m_db.QueryExecute( sqlQuery );

		if (query != NULL)
			machineID = query->IntGet( "ID" );

		delete query;
	}

	return machineID;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Get all of the attributes from the 'Machines' table
// and add them to the list of machine attributes.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
CReturn
CAutoModDb::MachineValuesLoad( int machineID )
{
	CReturn status;

	CString sqlQuery;
	CString attribName;
	CString attribValue;
	CDaoQuery* query = NULL;

	if ( m_dbIsOpen )
	{
		if (machineID < 1)
		{
			status = STATUS_ERROR;
		}
		else
		{
			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// Get all of the attributes from the 'Machines' table
			// and add them to the list of machine attributes.
			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

			sqlQuery.Format( "SELECT * FROM Machines WHERE ([ID]=%d)", machineID );

			query = m_db.QueryExecute( sqlQuery );

			if (query == NULL)
				status = STATUS_ERROR;
			else
				query->AttributesCopy( &m_machineAttribs, MY_DAO_FLAGS );

			delete query;
		}

		if ( status.IsOk() )
		{
			sqlQuery.Format(
				"SELECT [Machine Attribute Types].Description, [Machine Attributes].Value " \
				"FROM [Machine Attribute Types] INNER JOIN [Machine Attributes] ON "
				"[Machine Attribute Types].ID = [Machine Attributes].[Attribute Type ID] " \
				"WHERE ((([Machine Attributes].[Machine ID])=%d))",
						machineID );

			query = m_db.QueryExecute( sqlQuery );

			if (query == NULL)
			{
				status = STATUS_ERROR;
			}
			else
			{
				int recordCount = query->RecordCount();
				for (int indx = 0; indx < recordCount; ++indx)
				{
					query->Move( ((indx == 0) ? 0 : 1) );

					attribName = query->StringGet( "Description" );
					attribValue = query->StringGet( "Value" );
					m_machineAttribs.setString( attribName, attribValue );
				}
			}

			delete query;
		}
	}

	return status;
}

// Get the material dimensions.
CReturn
CAutoModDb::MaterialValuesGet( int materialId )
{
	CReturn status;

	if (materialId > 0)
	{
		CString sqlQuery;
		sqlQuery.Format( "SELECT * FROM [Material Inventory] WHERE ([ID]=%d)", materialId );

		CDaoQuery* query = m_db.QueryExecute( sqlQuery );

		if (query == NULL)
		{
			status = STATUS_ERROR;
		}
		else
		{
			m_materialId = materialId;
			query->AttributesCopy( &m_materialAttribs, MY_DAO_FLAGS );
		}

		delete query;
	}

	return status;
}

CReturn
CAutoModDb::LayerSetupValuesGet( int layerSetupId )
{
	CReturn status;
	CVarList* attribs;

	if (layerSetupId <= 0)
	{
		// assume being used for previewing a file.

		attribs = new CVarList();
		attribs->setString( "CAD_Layer", "*default*" );
		attribs->setString( "CAM_Layer", "all" );
		m_table.Append( attribs );
	}
	else
	{
		CString sqlQuery;
		CDaoQuery* query = NULL;

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=
		// Get the 'Z Level Mode', 'Gap Tol', 'Clean Tol' & 'Sharp Angle'
		sqlQuery.Format( "SELECT * FROM [Layer Setups] WHERE ([ID]=%d)", layerSetupId );

		query = m_db.QueryExecute( sqlQuery );

		if (query == NULL)
			status = STATUS_ERROR;
		else
			query->AttributesCopy( &m_layerSetupAttribs, MY_DAO_FLAGS );

		delete query;

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-==-=-=-=-=-=-=
		// Populate the map between CAD and CAM layers.
		sqlQuery.Format( "SELECT * FROM [Directives] WHERE ([Layer Setup ID]=%d)", layerSetupId );

		query = m_db.QueryExecute( sqlQuery );

		if (query == NULL)
		{
			status = STATUS_ERROR;
		}
		else
		{
			CString cadLayerName;
			CString camLayerName;
			int count, indx;

			count = query->RecordCount();
			for (indx = 0; indx < count; ++indx)
			{
				status = query->Move( ((indx == 0) ? 0 : 1) );
				attribs = new CVarList();
				query->AttributesCopy( attribs, MY_DAO_FLAGS );

				cadLayerName = attribs->getString( CAD_LAYER, "" );
				camLayerName = attribs->getString( CAM_LAYER, "" );

				m_layerNames.Append( new CString( cadLayerName ) );

				if ( !camLayerName.CompareNoCase( "Panel" ) )
					camLayerName = STR_STOCK;
				else
					camLayerName = CDbEntity::NameConvert( camLayerName );

				attribs->setString( CAM_LAYER, camLayerName );

				m_table.Append( attribs );
			}
		}

		delete query;
	}

	if ( status.IsOk() )
	{
		g_cadLayerIndx = attribs->find( CAD_LAYER );
		g_camLayerIndx = attribs->find( CAM_LAYER );

		m_table.Qsort( QSortCompare );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Converts a 'tool setup id' to a 'machine id'.
//
int
CAutoModDb::MachineId( int toolSetupId )
{
	CString sqlQuery;

	sqlQuery.Format(
		"SELECT * FROM [Tool Setups] WHERE (ID=%d)", toolSetupId );

	CDaoQuery* query = m_db.QueryExecute( sqlQuery );

	int machineId = ((query == NULL) ? 0 : query->IntGet( "Machine_ID" ));

	delete query;

	return machineId;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Convert a 'machine name' & 'tool setup name' to a 'tool setup id'
// This method was introduced with the advent of Java macros.  In the
// case Java macros, you must identify a tool setup by its name and
// its association with a machine name because you do not know the
// 'tool setup id'.
//
int
CAutoModDb::ToolSetupId( const CString& machineName, const CString& toolSetupName )
{
	CString sqlQuery;

	if ( machineName.IsEmpty() )
	{
		// This option was introduced to support 'speed ramping' during
		// code generation.  Per convesation with Rick (1/3/2000), the
		// toolSetupName is supposed to be unique.  This is a good thing
		// because we do not save the machine name in the model header.

		sqlQuery.Format(
			"SELECT * FROM [Tool Setups] WHERE (([Tool Setups].Description)='%s')",
				toolSetupName );
	}
	else
	{
		sqlQuery.Format(
			"SELECT [Tool Setups].ID FROM Machines INNER JOIN " \
			"[Tool Setups] ON Machines.ID = [Tool Setups].[Machine ID] " \
			"WHERE (((Machines.Description)='%s') AND " \
			"       (([Tool Setups].Description)='%s'))",
							machineName, toolSetupName );
	}

	CDaoQuery* query = m_db.QueryExecute( sqlQuery );

	int toolSetupId = ((query == NULL) ? 0 : query->IntGet( "ID" ));

	delete query;

	return toolSetupId;
}

int
CAutoModDb::MaterialId( const CString& materialName )
{
	CString sqlQuery;

	sqlQuery.Format(
		"SELECT * FROM [Material Inventory] WHERE " \
		"([Description] = '%s')",	materialName );

	CDaoQuery* query = m_db.QueryExecute( sqlQuery );

	int materialId = ((query == NULL) ? 0 : query->IntGet( "ID" ));

	delete query;

	return materialId;
}

CString
CAutoModDb::ToolTypeDescription( int toolTypeId )
{
	CString desc;

	CString sqlQuery;
	sqlQuery.Format( "SELECT Description FROM [Tool Types] WHERE (ID=%d)", toolTypeId );

	CDaoQuery* query = m_db.QueryExecute( sqlQuery );

	if (query != NULL)
		desc = query->StringGet( "Description" );

	delete query;

	return desc;
}


CString
CAutoModDb::CadLayerName( const CVarList* attribs )
{
	CVar* attrib = attribs->getVar( CAutoModDb::g_cadLayerIndx );
	return ((attrib == NULL) ? "" : attrib->getString());
}

CString
CAutoModDb::CamLayerName( const CVarList* attribs )
{
	CVar* attrib = attribs->getVar( CAutoModDb::g_camLayerIndx );
	return ((attrib == NULL) ? "" : attrib->getString());
}

// For sorting the layer records on increasing order of 'source layer' name.
int
CAutoModDb::QSortCompare( const void* ptrA, const void* ptrB )
{
	CVarList* attribsA = ( *(CVarList**) ptrA);
	CVarList* attribsB = ( *(CVarList**) ptrB);

	CString nameA = CadLayerName( attribsA );
	CString nameB = CamLayerName( attribsB );

	return ( nameA.CompareNoCase( nameB ) );
}

// For searching for my 'source layer' name.
int
CAutoModDb::BinarySearchCompare( const void* myItem, const void* arrayItem )
{
	CString* myLayerName = ((CString*) myItem);
	CVarList* attribs = ((CVarList*) arrayItem);

	CString thisLayerName = CadLayerName( attribs );

	return ( thisLayerName.CompareNoCase( (*myLayerName) ) );
}

void CAutoModDb::AllowTorchPiercingInit()
{
	/* 2013.01.20 (PE) -- Per email from Gary on the 17th ...
	  After looking at the issue I believe what we should be keying off of is the MACHINE TYPE ID.
	  If the MACHINE TYPE ID does NOT = 1 or 2 or 4. Then we want the points to come in as holes.
	  Along wit that if there is a CIRCLE with the same diameter of the tool specified in the
	  layer map it should also be read in as a hole as well.
	*/
	m_allow_torch_piercing = true;

	// eMachineType, actually.
	int machine_type = MachineType();
	switch (machine_type)
	{
	case PUNCH:
	case PUNCH_PLASMA:
	case PUNCH_LASER:
		m_allow_torch_piercing = false;
		break;
	}
}

// Yeargh, duplicated from automod.cpp
static CString STATION( "Station" );
static CString AUTO_TOOL( "**Auto**" );
static CString NO_TOOL( "None" );

// Ugly as sin.
double CAutoModDb::KerfGet() const
{
	double kerf = UNDEFINED;

	ToolKerfMapInit();

	CString station = StringGet( STATION );

	if ((station.CompareNoCase( AUTO_TOOL ) != 0) &&
		(station.CompareNoCase( NO_TOOL ) != 0))
	{
		int station_num = IntGet( STATION );

		double dval;
		if ( m_tool_kerf_map.Lookup( station_num, dval ) )
			kerf = dval;
	}

	return kerf;
}

// At first implementation, KerfGet() repeatedly traversed the station
// attributes. And since we don't know often ToolKerfMapInit() might be
// called, here we attempt to improve performance by building a map.
//    mutable actually ....
void CAutoModDb::ToolKerfMapInit() const
{
	if ( m_tool_kerf_map.IsEmpty() )
	{
		const TArrayOfAttribLists& stations = TooledStations();

		int count = stations.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			CVarList* station_attribs = stations.GetAt( indx );

			int tnum = station_attribs->getInt( STR_NC_CODE_NUMBER, IUNDEFINED );
			double kerf = station_attribs->getReal( "Kerf", UNDEFINED );

			((CAutoModDb*) this)->m_tool_kerf_map[tnum] = kerf;
		}
	}
}

