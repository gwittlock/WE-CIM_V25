
#include "stdafx.h"
#include "cmn_resource.h"

#include "MathConst.h"
#include "StringConst.h"

#include "token.h"
#include "VarList.h"
#include "3dCoord.h"
#include "3dBox.h"
#include "DbWorkplane.h"
#include "DbTool.h"
#include "DbLine.h"
#include "DbArc.h"
#include "DbHole.h"
#include "Model.h"
#include "AutoModDb.h"
#include "AutoMod.h"
#include "ExtAscii.h"


static const CString TOP = "A_TOP";

static CString ENTITY_TYPES = "PHLAC";
const int MAXTOKENS = 17;

// The types of tokens found in an extended ascii file.
enum ETokenTypes
{
	eTYPE,
	ePLANE,
	eXS,
	eYS,
	eZS,
	eXE,
	eYE,
	eZE,
	eXC,
	eYC,
	eZC,
	eRADIUS,
	eDIR,
	eDIAM,
	eOFFSET,
	eTOOL,
	N
};

// The token order of each entity type.
ETokenTypes ENTITY_DEFN[5][MAXTOKENS] =
{
	// Stock - P
	{ eTYPE, eXS, eYS, eZS, N, N, N, N, N, N, N, N, N, N, N, N },

	// Hole - H
	{ eTYPE, ePLANE, eXC, eYC, eDIAM, eZC, N, N, N, N, N, N, N, N, N, N },

	// Line - L
	{ eTYPE, ePLANE, eXS, eYS, eXE, eYE, eDIAM, eZS, eZE, N, N, N, N, N, N, N },

	// Arc - A
	{ eTYPE, ePLANE, eXS, eYS, eXE, eYE, eXC, eYC, eDIR, eDIAM, eZS, eZE, N, N, N, N },

	// Circle - C
	{ eTYPE, ePLANE, eXC, eYC, eRADIUS, eDIR, eDIAM, eZC, N, N, N, N, N, N, N, N }
};


////////////////////////////////////////////////////////////////////////

// TODO: A yacc parser would work well for the extended ascii grammar.

CExtAscii::CExtAscii()
	: m_importUtil(),
	  m_model( NULL ),
	  m_autoModDb( NULL ),
	  m_lineNo( 0 )
{
}

CExtAscii::~CExtAscii()
{
}

CReturn
CExtAscii::Read(
				const CString&	ascFileName,
				CAutoModDb*		autoModDb,
				CModel*			model,
				bool			buildToolSetup )
{
	CFileException err;
	CReturn status;

	if (model == NULL)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CExtAscii::Read() no model" );
		return status;
	}

	status = m_file.Open( ascFileName, FILEMODE_READ );
	if ( !status.isOkay())
	{
		status.User( IDS_FILE_READ_ERR, ascFileName );
		return status;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	m_model = model;
	m_autoModDb = autoModDb;
	m_importUtil.Init( m_model, (CAutoModDb*) m_autoModDb );
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	m_model = model;
	m_autoModDb = m_autoModDb;

	m_importUtil.Init( model, (CAutoModDb*) m_autoModDb );

	m_lineNo = 0;
	while ( !m_file.isEOF() )
	{
		CString buf = m_file.ReadLine();
		++m_lineNo;

		status = EntityCreate( buf );
		if ( !status.IsOk() )
			break;
	}
	m_file.Close();

	m_importUtil.AsciiPlanesTransform( 0, 0, 0 );
	m_importUtil.AsciiPlanesDestroy();

	if ( status.IsOk() )
		status = m_importUtil.ProfilesCreate();

	if ( status.IsOk() )
		status = m_importUtil.SystemFlagsSet();

	if ( status.IsOk() && !m_autoModDb->IsPreview() )
	{
		CAutoMod autoMod;
		autoMod.Init( m_autoModDb, m_model );
		status += autoMod.ModelPostProcess( buildToolSetup, FALSE );
	}

	CDbWorkplane* dbWork;
	status += m_model->EntityFind( STR_TOP, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

	if (dbWork != NULL)
		m_model->ActiveWorkplane( dbWork );

	m_model->PostReadInit();

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Construct layer names from the entity data and
// add them to the import utility's layer list.
CReturn
CExtAscii::Analyze( const CString& ascFileName, const CAutoModDb& autoModDb )
{
	CReturn status = m_file.Open( ascFileName, FILEMODE_READ );
	if ( !status.isOkay() )
		return status;


	m_importUtil.Init( NULL, NULL );

	CString buf;
	CString layerName;

	m_lineNo = 0;
	while ( !m_file.isEOF() )
	{
		buf = m_file.ReadLine();
		++m_lineNo;

		LayerAdd( buf );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Write the layer list built by CExtAscii::Analyze() to a file.
CReturn
CExtAscii::AnalysisWrite( const CString& anlFilePath ) 
{
	CReturn status;

	FILE* f = fopen( anlFilePath, "w" );

	if (f == NULL)
	{
		status.User( IDS_FILE_WRITE_ERR, anlFilePath );
	}
	else
	{
		int count = m_importUtil.LayerCount();
		for (int indx = 0; indx < count; ++indx)
		{
			CString layerName = m_importUtil.LayerName( indx );

			fprintf( f, "%s\n", layerName );
		}
		fclose( f );
	}
	
	return status;
}

int
CExtAscii::Parse(  const CString& buf, CStringArray* fields )
{
	int type = ENTITY_TYPES.Find( buf[0] );
	if (type < 0)
		return -1;

	// ------ Initialize the parser ------
	CToken parser;
	parser.setLine( buf );

	parser.addDelimit( ',' );
	parser.addDelimit( ' ' );		// Also ignore spaces
	parser.setBreak( TOKEN_BREAK_ALNUM );
	// -----------------------------------

	CString token;
	int indx = 0;

	while (1)
	{
		ETokenTypes tokenType = ENTITY_DEFN[ type ][ indx ];
		if (tokenType == N)
			break;  // we're at the end

		token = parser.getToken();

		(*fields)[ tokenType ] = token;

		++indx;
	}

	// Get the offset direction.
	indx = buf.FindOneOf( "<>G" );
	if (indx >= 0)
		(*fields)[ eOFFSET ] = buf[indx];

	// Get any embedded tool number.
	// This is required of later Cabinet Vision files.
	// See also ToolFind()
	(*fields)[ eTOOL ] = "-1";

	indx = 0;
	while (1)
	{
		indx = buf.Find( 'T', indx );
		if (indx < 0)
			break;

		++indx;
		if (indx >= buf.GetLength())
			break;

		if ( isdigit( buf[indx] ) )
		{
			int jndx = buf.Find( ',', indx );
			if (jndx < 0)
				(*fields)[ eTOOL ] = buf.Mid( indx, (buf.GetLength() - indx) );
			else
				(*fields)[ eTOOL ] = buf.Mid( indx, (jndx - indx + 1) );

			break;
		}
	}

	return type;
}

void
CExtAscii::LayerAdd( const CString& buf )
{
	if (buf.GetLength() < 1)
		return;
	
	if (buf[0] == '*')
		return;  // Simply skip the comment.

	CStringArray fields;
	fields.SetSize( MAXTOKENS );

	Parse( buf, &fields );
	
	CString layerName = LayerNameCreate( fields );

	if (layerName.GetLength() > 0)
		m_importUtil.LayerAdd( layerName );
}

CString
CExtAscii::LayerNameCreate( const CStringArray& fields )
{
	CString layerName;

	CString type = fields[ eTYPE ];
	CString work = fields[ ePLANE ];
	CString diam = fields[ eDIAM ];

	if ( !type.CompareNoCase( "P" ) )
	{
		layerName = STR_STOCK;
	}
	else if ( !type.CompareNoCase( "H" ) )
	{
		// Holes will be auto-tooled.
		double zLevel = atof( fields[eZC] );

		layerName.Format( "H_W_%s_Z%10.4f_Diam%s", work, zLevel, diam );
	}
	else if ( !type.CompareNoCase( "L" ) ||
			  !type.CompareNoCase( "A" ) ||
			  !type.CompareNoCase( "C" ) )
	{
		// Routing and Dadoing need a specific tool.
		CString offsetDir = OffsetDir( fields[ eOFFSET ] );
		double zLevel = atof( fields[eZE] );
		int toolNo = ToolFind( fields );

		CString sToolNo = "?";
		if (toolNo > 0)
			sToolNo.Format( "%d", toolNo );
		
		layerName.Format( "W_%s_T%s_Z%10.4f_O_%s", work, sToolNo, zLevel, offsetDir );

		if (toolNo <= 0)
		{
			CString suffix;
			suffix.Format( "_Diam%s", diam );
			layerName += suffix;
		}
	}

	layerName.Remove( ' ' );

	return layerName;
}

CString
CExtAscii::OffsetDir( const CString& buf )
{
	CString offsetDir = "None";

	if (buf.Find( "<" ) >= 0)
		offsetDir = "Left";
	else if (buf.Find( ">" ) >= 0)
		offsetDir = "Right";

	return offsetDir;

}

// See also Parse(), which gets any embedded tool number.
int
CExtAscii::ToolFind( const CStringArray& fields )
{
	CVarList* tool;
	int indx;

	if (m_autoModDb->IsPreview() )
		return IUNDEFINED;

	const TArrayOfAttribLists& tools = m_autoModDb->TooledStations();

	int count = tools.Count();

	int toolNo = atoi( fields[ eTOOL ] );

	if (toolNo < 0)
	{
		// There is no embeded tool number.
		// Find a tool by parameter matching.

		CString tmpType;
		CString tmpFace;
		double tmpDiam;

		CString type = fields[ eTYPE ];
		CString face = fields[ ePLANE ];
		CString offset = fields[ eOFFSET ];
		double  diam = atof( fields[ eDIAM ] );

		bool isDado = (type.Find( "L" ) >= 0 && offset.Find( "G" ) >= 0);
		CString toolType = (( isDado ) ? "Disc_Saw" : "Router_Bit");

		double holeMinusTol = m_autoModDb->HoleMinusTol();
		double holePlusTol = m_autoModDb->HolePlusTol();

		for (indx = 0; indx < count; ++indx)
		{
			tool = tools[indx];

			tool->getString( STR_TYPE, &tmpType );
			tool->getString( STR_WORKPLANE, &tmpFace  );
			tool->getReal( STR_DIAMETER, &tmpDiam );

			if ( !tmpType.CompareNoCase( toolType ) &&
				 !tmpFace.CompareNoCase( face )  &&
				  (diam - tmpDiam) <= holeMinusTol &&
				  (tmpDiam - diam) <= holePlusTol )
			{
				toolNo = tool->getInt( STR_NC_CODE_NUMBER, -1 );
				break;
			}
		}
	}
	else
	{
		bool match = FALSE;
		for (indx = 0; indx < count; ++indx)
		{
			tool = tools[indx];
			match = (toolNo == tool->getInt( STR_NC_CODE_NUMBER, -1 ));
			if ( match )
				break;
		}

		if ( !match )
		{
			CReturn status;
			status.User( IDS_TOOL_NOT_FOUND, toolNo );
		}
	}

	return toolNo;
}

CReturn
CExtAscii::EntityCreate( const CString& buf )
{
	CReturn status;

	if (buf.GetLength() < 1)
		return status;
	
	if (buf[0] == '*')
		return status;  // Simply skip the comment.

	CStringArray fields;
	fields.SetSize( MAXTOKENS );

	Parse( buf, &fields );
		
	CString layerName = LayerNameCreate( fields );

	if (layerName.GetLength() < 1)
		return status;

	if ( (layerName.CompareNoCase( STR_STOCK ) != 0) &&
		 (m_autoModDb->SourceLayerFind( layerName ) < 0))
			return status;  // Ignore this layer.


	CString type = fields[ eTYPE ];
	
	if ( !type.CompareNoCase( "P" ) )
	{
		status = StockAndPlanesCreate( fields );
	}
	else if ( !type.CompareNoCase( "L" ) )
	{
		status = LineCreate( fields, layerName );
	}
	else if ( !type.CompareNoCase( "A" ) )
	{
		status = ArcCreate( fields, layerName );
	}
	else if ( !type.CompareNoCase( "C" ) )
	{
		status = CircleCreate( fields, layerName );
	}
	else if ( !type.CompareNoCase( "H" ) )
	{
		status = HoleCreate( fields, layerName );
	}

	return status;
}

CReturn
CExtAscii::StockAndPlanesCreate( const CStringArray& fields )
{
	CReturn status;
	
	C3dBox box;
	double zpan;

	double length = atof(fields[eXS]);
	double width  = atof(fields[eYS]);
	double thick  = atof(fields[eZS]);

	int workType = m_autoModDb->WorkplaneType();
	int machType = m_autoModDb->MachineType();

	status = m_importUtil.StandardPlanesCreate( length, width, thick, workType, machType );
	status = m_importUtil.AsciiPlanesCreate( length, width, thick, workType );

	if ( status.IsOk() )
	{
		int dir = m_importUtil.StockDirection();
		status = m_importUtil.StockCreate( TOP, dir, 0., length, 0., width, 0.0 );
	}

	if ( status.IsOk() )
	{
		int dir = m_importUtil.StockDirection();
		status = m_importUtil.HeaderUpdate( TOP, dir, thick, &box, &zpan );
	}

	if ( status.IsOk() )
		status = m_importUtil.ViewPlanesCreate( length, width, thick );

	return status;
}

CReturn
CExtAscii::LineCreate( const CStringArray& fields, const CString& layerName )
{
	CReturn status;

	int layerRecIndx = m_importUtil.LayerRecFind( layerName );
	if (layerRecIndx >= 0)
	{
		C3dCoord ps( atof(fields[eXS]), atof(fields[eYS]), atof(fields[eZS]) );
		C3dCoord pe( atof(fields[eXE]), atof(fields[eYE]), atof(fields[eZE]) );

		// Crap ASCII Files... filter them
		// TODO:  Issue a clever and insightful warning to the idiot user
		if ( !ps.WithinTol( pe, SMALL ) )
		{
			double depth = ps.Z();

			m_importUtil.CurrentRecordSet( layerRecIndx );
			m_importUtil.ZLevel( &depth );
			ps.Z( depth );
			pe.Z( depth );

			CDbLine* dbLine;
			status = m_model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );
			if ( status.IsOk() )
			{
				CDbTool* dbTool = DbLayer( layerName );
				CDbWorkplane* dbWork = DbWork( fields[ ePLANE ] );

				dbLine->Init( dbTool, dbWork, ps, pe );
				m_importUtil.Color( dbLine );
			}
		}
	}

	return status;
}

CReturn
CExtAscii::ArcCreate( const CStringArray& fields, const CString& layerName )
{
	CReturn status;

	int layerRecIndx = m_importUtil.LayerRecFind( layerName );
	if (layerRecIndx >= 0)
	{
		// NOTE: The extended ascii format appears to allow helix entities
		// because it allows different start and end Z ordinates.  For now,
		// will treat such entities a planar arcs.
		C3dCoord ps( atof(fields[eXS]), atof(fields[eYS]), atof(fields[eZS]) );
		C3dCoord pe( atof(fields[eXE]), atof(fields[eYE]), atof(fields[eZS]) );
		C3dCoord pc( atof(fields[eXC]), atof(fields[eYC]), atof(fields[eZS]) );

		// Crap ASCII Files... filter them
		// TODO:  Issue a clever and insightful warning to the idiot user
		if ( !ps.WithinTol( pc, SMALL ) && !pe.WithinTol( pc, SMALL ) )
		{
			double depth = ps.Z();

			m_importUtil.CurrentRecordSet( layerRecIndx );
			m_importUtil.ZLevel( &depth );

			CDbArc* dbArc;
			status = m_model->EntityCreate( DBARC, (CDbEntity**) &dbArc );
			if ( status.IsOk() )
			{
				CDbTool* dbTool = DbLayer( layerName );
				CDbWorkplane* dbWork = DbWork( fields[ ePLANE ] );

				int dir = atoi(fields[eDIR]);

				C3dVec ct_st = pc - ps;
				C3dVec ct_en = pc - pe;
				double avg_rad = (ct_st.Length() + ct_en.Length()) / 2.0;

				ct_st = ct_st * (1/ct_st.Length());
				ct_en = ct_en * (1/ct_en.Length());

				ps = pc - ct_st * avg_rad;
				pe = pc - ct_en * avg_rad;

				ps.Z( depth );
				pe.Z( depth );
				pc.Z( depth );

				dbArc->Init( dbTool, dbWork, ps, pe, pc, (dir*2)-1 );
				m_importUtil.Color( dbArc );
			}
		}
	}

	return status;
}

CReturn
CExtAscii::CircleCreate( const CStringArray& fields, const CString& layerName )
{
	CReturn status;

	int layerRecIndx = m_importUtil.LayerRecFind( layerName );
	if (layerRecIndx >= 0)
	{
		m_importUtil.CurrentRecordSet( layerRecIndx );

		CDbArc* dbArc;
		status = m_model->EntityCreate( DBARC, (CDbEntity**) &dbArc );
		if ( status.IsOk() )
		{
			CDbTool* dbTool = DbLayer( layerName );
			CDbWorkplane* dbWork = DbWork( fields[ ePLANE ] );

			double radius = atof(fields[eRADIUS]);
			int dir = atoi(fields[eDIR]);

			C3dCoord pc( atof(fields[eXC]), atof(fields[eYC]), atof(fields[eZC]) );
			C3dCoord ps( (pc.X() + radius), pc.Y(), pc.Z() );

			double depth = ps.Z();
			m_importUtil.ZLevel( &depth );
			ps.Z( depth );
			pc.Z( depth );

			dbArc->Init( dbTool, dbWork, ps, ps, pc, dir );
			m_importUtil.Color( dbArc );
		}
	}

	return status;
}

CReturn
CExtAscii::HoleCreate( const CStringArray& fields, const CString& layerName )
{
	CReturn status;

	int layerRecIndx = m_importUtil.LayerRecFind( layerName );
	if (layerRecIndx >= 0)
	{
		m_importUtil.CurrentRecordSet( layerRecIndx );

		CDbArc* dbArc;
		status = m_model->EntityCreate( DBARC, (CDbEntity**) &dbArc );
		if ( status.IsOk() )
		{
			CDbTool* dbTool = DbLayer( layerName );
			CDbWorkplane* dbWork = DbWork( fields[ ePLANE ] );

			double diam = atof(fields[eDIAM]);
			int dir = 1;

			C3dCoord pc( atof(fields[eXC]), atof(fields[eYC]), atof(fields[eZC]) );
			C3dCoord ps( (pc.X() + 0.5 * diam), pc.Y(), pc.Z() );

			double depth = pc.Z();
			m_importUtil.ZLevel( &depth );
			pc.Z( depth );

			dbArc->Init( dbTool, dbWork, ps, ps, pc, dir );
			m_importUtil.Color( dbArc );
		}
	}

	return status;
}

// Given the CAD layer name, fetches/creates the associated CAM layer.
CDbTool*
CExtAscii::DbLayer( const CString& layerName )
{
	CString targetLayerName = m_autoModDb->TargetName();

	CDbTool* dbTool;

	m_model->EntityFind( targetLayerName, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );

	if (dbTool == NULL)
	{
		m_model->EntityCreate( DBTOOL, (CDbEntity**) &dbTool );
		dbTool->Name( targetLayerName );
	}

	return dbTool;
}

CDbWorkplane*
CExtAscii::DbWork( const CString& planeName )
{
	CDbWorkplane* dbWork;
	CString			realName;

	// Hard-coded for the names we expect from an ASCII file
	// TODO:  Make this better
	if (!planeName.CompareNoCase( "LEF" ))
		realName = "A_Left";
	else
	if (!planeName.CompareNoCase( "RIG" ))
		realName = "A_Right";
	else
	if (!planeName.CompareNoCase( "FRO" ))
		realName = "A_Front";
	else
	if (!planeName.CompareNoCase( "REA" ))
		realName = "A_Back";
	else
	if (!planeName.CompareNoCase( STR_TOP ))
		realName = "A_Top";
	else
		realName = planeName;		// Just in case

	m_model->EntityFind( realName, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	return dbWork;
}

