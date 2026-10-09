
#include "stdafx.h"

#include "MathConst.h"
#include "StringConst.h"
#include "cmn_resource.h"
#include "Register.h"

#include "2dCoord.h"

#include "DbWorkplane.h"
#include "DbTool.h"
#include "DbCurve.h"
#include "DbLine.h"
#include "DbArc.h"
#include "DbHole.h"
#include "DbProfile.h"
#include "DbIterator.h"
#include "AutoModDb.h"
#include "AutoMod.h"
#include "ImportUtil.h"
#include "DbToolGen.h"
#include "mm2.h"

#include "ViewMgr.h"
#include "ModelUtil.h"

#include "ModelProcess.h"



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CurveSave( FILE* f, const CDbCurve& dbCurve, C2dCoord* pe );
void LineSave( FILE* f, const CDbLine& dbLine, C2dCoord* pe );
void ArcSave( FILE* f, const CDbArc& dbArc, C2dCoord* pe );

double CModelProcessApp::m_dropdoor[2];

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Model:Init: db=%s, mach=%s, tool=%s, mat=%s [, nest=%s] [, stations=%d], dx=%f, dy=%f [,dz=%f]
// Model:Init: db=%s, tsid=%d, matid=%d [, stations=%d], dx=%f, dy=%f [,dz=%f]
// Returns bool = %d
//
// ASSUMPTION: Admin:New: has been executed prior !
//
// NOTE: This method was overloaded to help automate the mrp/nesting/coding process.
// This does reduce redundant code, but it produces potential problems as well.
//
CReturn 
CModelProcessApp::ModelInit( CCommand* io_cmd ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	CString	ctg_path;

	const CVarList& params = io_cmd->VarList();

	// One use
	CString dbPath			= params.getString( "db", "" );
	CString machineName		= params.getString( "mach", "" );
	CString toolSetupName	= params.getString( "tool", "" );
	CString materialName	= params.getString( "mat",  "" );
//	CString nestSetupName	= params.getString( "nest", "" );

	// ... or the other
	int toolSetupId			= params.getInt( "tsid", 0 );
	int materialId			= params.getInt( "matid", 0 );

	double dx	= params.getReal( "dx", UNDEFINED );
	double dy	= params.getReal( "dy", UNDEFINED );
	double dz	= params.getReal( "dz", UNDEFINED );

	bool loadEmptyStations = (io_cmd->VarList().getInt( "stations", 0 ) != 0);

	if ( (dbPath.GetLength() < 1) ||
		 (dx < SMALL || dx >= UNDEFINED) ||
		 (dy < SMALL || dy >= UNDEFINED) )
	{
		status = STATUS_ERROR;
	}
	else
	{
		CAutoModDb autoModDb;

		if (machineName.GetLength() > 0)
		{
			status = autoModDb.Init(
				dbPath, machineName, toolSetupName, materialName, loadEmptyStations );
		}
		else
		{
			status = autoModDb.Init(
				dbPath, toolSetupId, 0, materialId, loadEmptyStations );
		}

		if ( status.IsOk() )
		{
			CImportUtil importUtil;  // Creates work-planes, view-lanes, stock
			CAutoMod autoMod;        // Creates tools.

			C3dBox box;
			double zStock;

			CModel&	model = io_cmd->getModel();

			importUtil.Init( &model, &autoModDb );
			autoMod.Init( &autoModDb, &model );

			int workType = autoModDb.WorkplaneType();
			int machType = autoModDb.MachineType();

			if (dz >= UNDEFINED)
				dz = autoModDb.MaterialThick();

			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

			model.UndoBufferSuppress();

			status = importUtil.StandardPlanesCreate( dx, dy, dz, workType, machType );

			if ( status.IsOk() )
			{
				status = importUtil.StockCreate( STR_WORLD, CW, 0.0, dx, 0.0, dy, 0.0 );
			}

			if ( status.IsOk() )
			{
				status = importUtil.HeaderUpdate( STR_WORLD, CW, dz, &box, &zStock );

				// Put this here for now, because of bullshit set in previous method
				// TODO: TRACK DOWN PROPER FUNCTIONING OF MACHINE NAME HEADER SET

				// MachCfg (aka. ToolSetup)
				model.pHeader()->setString( "MachCfg", toolSetupName );
			}


			if ( status.IsOk() )
				status = importUtil.ViewPlanesCreate( box.Dx(), box.Dy(), dz );

			if ( status.IsOk() )
			{
				CDbToolGen	gen;
				gen.DbToolsCreate( autoModDb.TooledStations(), &model );
			}

			CDbTool* dbTool;
			status += model.EntityFind( STR_STOCK, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );

			if (dbTool != NULL)
				model.ActiveTool( dbTool );

			CDbWorkplane* dbWork;
			status += model.EntityFind( STR_TOP, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

			if (dbWork != NULL)
				model.ActiveWorkplane( dbWork );


			// Write the max dropdoor dimensions to the model header.
			DropDoorUpdate( autoModDb, &model );

			// Instantiate CTGs for clamps and holddowns.
			ctg_path = autoModDb.MachineAttributes().getString( "Clamp_CTG", "" );
			model.pHeader()->setString( "Clamp_CTG", ctg_path );
			CDbFeature::ClampInitFromCTG( ctg_path );

			ctg_path = autoModDb.MachineAttributes().getString( "HoldDown_CTG", "" );
			model.pHeader()->setString( "Hold_CTG", ctg_path );
			CDbFeature::HoldInitFromCTG( ctg_path );

			model.UndoBufferActivate();

			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		}
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CModelProcessApp::ModelInit()" );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Model:ActiveTool:
// Returns id = %d
//
// Model:ActiveTool: id = %d
// Returns id = %d of previously active tool
//
CReturn 
CModelProcessApp::ModelActiveTool( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();

	ID id = io_cmd->VarList().getInt( "id", 0 );

	CDbTool* dbTool = NULL;
	if (id > 0)
	{
		// Setting the active tool.
		model.EntityFind( id, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
		if (dbTool != NULL)
			model.ActiveTool( dbTool );
	}
	else
	{
		// Getting the active tool.
		dbTool = model.ActiveTool();
	}

	io_cmd->setInt( "id", ((dbTool == NULL) ? 0 : dbTool->Id()) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Model:ActiveWorkplane:
// Returns id = %d
//
// Model:ActiveWorkplane: id = %d
// Returns id = %d of previously active workplane
//
CReturn 
CModelProcessApp::ModelActiveWorkplane( CCommand* io_cmd ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();

	ID id = io_cmd->VarList().getInt( "id", 0 );

	CDbWorkplane* dbWork = NULL;
	if (id > 0)
	{
		// Setting the active layer.
		model.EntityFind( id, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
		if (dbWork != NULL)
			model.ActiveWorkplane( dbWork );
	}
	else
	{
		// Getting the active layer.
		dbWork = model.ActiveWorkplane();
	}

	io_cmd->setInt( "id", ((dbWork == NULL) ? 0 : dbWork->Id()) );

	return status;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Model:EntityCount: type = %d
// Returns count = %d
//
// NOTE: There is tight coupling between the C++ enums
// [DBLAYER..DBPATTERN] and the corresponding Java enums
// declared in Bbi/System/Const.java
//
CReturn 
CModelProcessApp::ModelEntityCount( CCommand* io_cmd ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	int count = 0;

	EDbEntityType type;
	status = io_cmd->getInt( "type", (int*) &type );

	if ( status.IsOk() )
	{
		CModel&	model = io_cmd->getModel();
		count = model.Db().Count( type );
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CModelProcessApp::ModelEntityCount()" );
	}

	io_cmd->setInt( "count", count );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Model:Get: name = %s, lb = %d, ub = %d
// Returns id = %d, name = %s
//
// For VB, limits search over range of entity types.
// Model:Get: id = %s, lb = %d, ub = %d
//
// NOTE: There is tight coupling between the C++ enums
// [DBLAYER..DBPATTERN] and the corresponding Java enums
// declared in Bbi/System/Const.java
//
CReturn 
CModelProcessApp::ModelGet( CCommand* io_cmd ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();

	CString	name;
	ID		id;
	ID		entid;
	int		lowerBound;
	int		upperBound;

	lowerBound = io_cmd->VarList().getInt( "lb", -1 );
	upperBound = io_cmd->VarList().getInt( "ub", -1 );
	name = io_cmd->VarList().getString( "name", "" );
	entid = io_cmd->VarList().getInt( "id", 0 );

	id = 0;

	if (name.IsEmpty() && entid == 0)
		status.setStatus( STATUS_ERROR );

	if (lowerBound < DBWORKPLANE || lowerBound >= DBTERMINAL)
		status.setStatus( STATUS_ERROR );

	if (upperBound < DBWORKPLANE || upperBound >= DBTERMINAL)
		status.setStatus( STATUS_ERROR );

	if ( status.IsOk() )
	{
		EDbEntityType lb = (EDbEntityType) lowerBound;
		EDbEntityType ub = (EDbEntityType) upperBound;

		CDbEntity* dbEntity;
		if (id > 0)
			model.EntityFind( entid, &dbEntity, lb, ub );
		else
			model.EntityFind( name, &dbEntity, lb, ub );

		if (dbEntity == NULL)
		{
			status.setStatus( STATUS_ERROR );
			name = "";
		}
		else
		{
			id = dbEntity->Id();
		}
	}

	io_cmd->setInt( "id", id );
	io_cmd->setString( "name", name );

#if REQUIRED
	if ( !status.IsOk() )
	{
		CString msg;
		msg.Format( "CModelProcessApp::ModelGet( \"%s\")", name );
		status.Diagnostic( msg );
	}
#endif

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Model:Get2: indx = %d, type = %d
// Returns id = %d, name = %s
//
// NOTE: There is tight coupling between the C++ enums
// [DBLAYER..DBPATTERN] and the corresponding Java enums
// declared in Bbi/System/Const.java
//
CReturn 
CModelProcessApp::ModelGet2( CCommand* io_cmd ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();

	EDbEntityType type;
	int indx;

	status += io_cmd->getInt( "indx", &indx );
	status += io_cmd->getInt( "type", (int*) &type );

	if ( status.IsOk() )
	{
		if (type < DBWORKPLANE || type >= DBTERMINAL)
			status.setStatus( STATUS_ERROR );
	}

	if ( status.IsOk() )
	{
		CDbEntity* dbEntity = model.Db().Get( type, indx );

		if (dbEntity == NULL)
		{
			status.setStatus( STATUS_ERROR );
		}
		else
		{
			io_cmd->setInt( "id", dbEntity->Id() );
			io_cmd->setString("name", dbEntity->Name( ) );
		}
	}

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CModelProcessApp::ModelGet2()" );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Model:ToolGet: (station = %d | CribID = %d)
// Returns id = %d
//
// NOTE: There is tight coupling between the C++ enums
// [DBLAYER..DBPATTERN] and the corresponding Java enums
// declared in Bbi/System/Const.java
//
CReturn 
CModelProcessApp::ModelToolGet( CCommand* io_cmd ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CDbIterator iter;
	CModel&	model = io_cmd->getModel();
	CDbTool* dbTool = NULL;

	bool match;
	int ival;

	int station = io_cmd->VarList().getInt( "station", 0 );
	int cribID = io_cmd->VarList().getInt( "CribID", 0 );

	if (station <= 0 && cribID <= 0)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CModelProcessApp::ModelToolGet()" );
		return status;
	}

	iter.Init( model.Db(), DBTOOL );
	while (1)
	{
		dbTool = dynamic_cast<CDbTool*>( iter() );
		if (dbTool == NULL)
			break;

		if (station > 0)
		{
			ival = dbTool->IntGet( STR_NC_CODE_NUMBER, IUNDEFINED );
			match = (ival == station);
		}
		else if (cribID > 0)
		{
			ival = dbTool->IntGet( STR_TOOL_ID, IUNDEFINED );
			match = (ival == cribID);
		}

		if ( match )
			break;

		iter.Next();
	}

	if ( match )
	{
		io_cmd->setInt( "id", dbTool->Id() );
	}
	else
	{
		io_cmd->setInt( "id", 0 );
		if (station > 0)
			status.Internal( IDS_TOOL_NOT_FOUND, station );
		else
			status.Internal( IDS_TOOL_NOT_FOUND2, cribID );
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Model:NameCheck: name = %s
// Returns bool = %d
//
CReturn 
CModelProcessApp::ModelNameCheck( CCommand* io_cmd ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	CModel&	model = io_cmd->getModel();

	CString name;
	status = io_cmd->getString( "name", &name );

	if ( !status.IsOk() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CModelProcessApp::ModelNameCheck()" );
		return status;
	}

	// Check for lexical correctness.

	bool okay = CDbEntity::NameCheck( name );

	// Check for uniqueness.

	if ( okay )
	{
		okay = (model.Db().NameFind( name ) < 0);
	}

	io_cmd->setInt( "bool", (okay ? 1 : 0) );

	return status;
}




//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Model:PartExtent:
// Returns xmin=%g, ymin=%g, xmax=%g, ymax=%g 
//
//
//	Only checks USER geometry; returns the values in WORLD
//
CReturn 
CModelProcessApp::ModelPartExtent( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn ret;
	CModel&	model = io_cmd->getModel();

	C3dBox box = model.BoxUser(0);
	if (!box.IsDefined())
		return CReturn( STATUS_ERROR );

	io_cmd->setReal( STR_XMIN, box.Xmin() );
	io_cmd->setReal( STR_YMIN, box.Ymin() );
	io_cmd->setReal( STR_XMAX, box.Xmax() );
	io_cmd->setReal( STR_YMAX, box.Ymax() );

	return ret;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
// Model:MachineUpdate: db=%s, machineid=%d
//
CReturn 
CModelProcessApp::ModelMachineUpdate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;
	CAutoModDb	autoModDb;
	CString		dbPath;
	CVar*		attrib;
	CString		name;
	int			machineID;
	int			count, indx;

	CModel&	model = io_cmd->getModel();

	dbPath = io_cmd->VarList().getString( "db", "" );
	machineID = io_cmd->VarList().getInt( "machineid", 0 );

	if (dbPath.IsEmpty() || machineID <= 0)
	{
		status.Internal( IDS_INTERNAL_ERROR, "CModelProcessApp::ModelMachineUpdate()" );
	}
	else
	{
		status = autoModDb.DbOpen( dbPath );
		if ( status.IsOk() )
		{
			status = autoModDb.MachineValuesLoad( machineID );

			if ( status.IsOk() )
			{
				count = autoModDb.MachineAttributes().countVar();
				for (indx = 0; indx < count; ++indx)
				{
					attrib = autoModDb.MachineAttributes().getVar(indx);
					name = attrib->getName();

					if (name.CompareNoCase("Description") == 0)
					{
						model.pHeader()->setString( "MachineName", attrib->getString() );
					}
					else if (name.CompareNoCase("Code_Part_Profile") == 0)
					{
						model.pHeader()->setInt( "PartProf", attrib->getInt() );
					}
					else if (name.CompareNoCase("Type_ID") == 0)
					{
						model.pHeader()->setInt( "MachineType", attrib->getInt() );
					}
					else if (name.CompareNoCase("Workplane_Type_ID") == 0)
					{
						model.pHeader()->setInt( "WorkplaneType", attrib->getInt() );
					}
					else if (name.CompareNoCase("Hole_Plus_Tolerance") == 0)
					{
						model.pHeader()->setReal( "HolePlusTol", attrib->getReal() );
					}
					else if (name.CompareNoCase("Hole_Minus_Tolerance") == 0)
					{
						model.pHeader()->setReal( "HoleMinusTol", attrib->getReal() );
					}
					else if (name.CompareNoCase("Space_Tolerance") == 0)
					{
						model.pHeader()->setReal( "SpaceTol", attrib->getReal() );
					}
					else if (name.CompareNoCase("Min_Travel_Limit_X") == 0)
					{
						model.pHeader()->setReal( "xmin", attrib->getReal() );
					}
					else if (name.CompareNoCase("Min_Travel_Limit_Y") == 0)
					{
						model.pHeader()->setReal( "ymin", attrib->getReal() );
					}
					else if (name.CompareNoCase("Max_Travel_Limit_X") == 0)
					{
						model.pHeader()->setReal( "xmax", attrib->getReal() );
					}
					else if (name.CompareNoCase("Max_Travel_Limit_Y") == 0)
					{
						model.pHeader()->setReal( "ymax", attrib->getReal() );
					}
					else if (name.CompareNoCase("HoldDown_CTG") == 0)
					{
						name = attrib->getString();
						model.pHeader()->setString( "Hold_CTG", name );
						CDbFeature::HoldInitFromCTG( name );
					}
					else if (name.CompareNoCase("Clamp_CTG") == 0)
					{
						name = attrib->getString();
						model.pHeader()->setString( "Clamp_CTG", name );
						CDbFeature::ClampInitFromCTG( name );
					}
					else if (
						(name.CompareNoCase("Length") == 0) ||
						(name.CompareNoCase("Width") == 0) ||
						(name.CompareNoCase("Thickness") == 0) ||
						(name.CompareNoCase("Clamp_Center") == 0) ||
						(name.CompareNoCase("Clamp_Length") == 0) ||
						(name.CompareNoCase("Clamp_Width") == 0) ||
						(name.CompareNoCase("MaterialWeight") == 0) ||
						(name.CompareNoCase("xmin") == 0) ||
						(name.CompareNoCase("xmax") == 0) ||
						(name.CompareNoCase("ymin") == 0) ||
						(name.CompareNoCase("ymax") == 0) )
					{
						model.pHeader()->setReal( name, attrib->getReal() );
					}
					else if (
						(name.CompareNoCase("MachineType") == 0) ||
						(name.CompareNoCase("Ind_Hits") == 0) ||
						(name.CompareNoCase("Number_Of_Clamps") == 0) ||
						(name.CompareNoCase("ScribeWithTorch") == 0) )
					{
						model.pHeader()->setReal( name, attrib->getInt() );
					}
					else if (
						(name.CompareNoCase("MachCfg") == 0) ||
						(name.CompareNoCase("MatCfg") == 0) )
					{
						model.pHeader()->setString( name, attrib->getString() );
					}
					else if (name.Find("Holddown") == 0)
					{
						// 2007.03.04 (PE) -- Blah, get holddowns to update.

						CDbFeature::HoldDownInvalidate();

						if (name.CompareNoCase("Holddown_Type") == 0)
							model.pHeader()->setInt( "hold_type", attrib->getInt() );
						else if (name.CompareNoCase("Holddown_Diameter") == 0)
							model.pHeader()->setReal( "hold_dia", attrib->getReal() );
						else if (name.CompareNoCase("Holddown_Location_X1") == 0)
							model.pHeader()->setReal( "hold_x1", attrib->getReal() );
						else if (name.CompareNoCase("Holddown_Location_X2") == 0)
							model.pHeader()->setReal( "hold_x2", attrib->getReal() );
						else if (name.CompareNoCase("Holddown_Location_Y") == 0)
							model.pHeader()->setReal( "hold_y", attrib->getReal() );

					}
				}

				HoldDownsUpdate( model );
				DropDoorUpdate( autoModDb, &model );
			}
		}
		autoModDb.DbClose();
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Model:ToolsUpdate: db=%s, toolsetupid=%d
//
// Updates the attributes of existing tools in the model
// by first matching a tool by is NC_Code_Number and then
// substituting the model tool attibutes with those from
// the machining database.  Additionally, new tools in
// the database setup are added to the model.
//
CReturn 
CModelProcessApp::ModelToolsUpdate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;
	CDbToolGen	gen;
	CAutoModDb	autoModDb;
	CAutoMod	autoMod;
	CString		dbPath;
	int			toolSetupId = 0;

	CModel&	model = io_cmd->getModel();

	status += io_cmd->getString( "db", &dbPath );
	status += io_cmd->getInt( "toolsetupid", &toolSetupId );

	if (status.IsOk() && (dbPath.GetLength() < 1 || toolSetupId < 1))
	{
		status = STATUS_ERROR;
	}

	if ( status.IsOk() )
	{
		status = autoModDb.Init( dbPath, toolSetupId, 0, 0, FALSE );

		if ( status.IsOk() )
		{
			autoMod.Init( &autoModDb, &model );
			status = gen.DbToolsUpdate( autoModDb.TooledStations(), &model );
			model.pHeader()->setInt( STR_PARTPROF, autoModDb.CodePartProf() );
			model.pHeader()->setString( "MachCfg", autoModDb.ToolSetup().Name() );
		}
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CModelProcessApp::ModelToolsUpdate()" );

	return status;
}

// Model:StockUpdate: dx=%f, dy=%f, dz=%f [, wp=%d] [, machtype=%d]
CReturn
CModelProcessApp::ModelStockUpdate( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	double dx, dy, dz;

	status += io_cmd->getReal( "dx", &dx );
	status += io_cmd->getReal( "dy", &dy );
	status += io_cmd->getReal( "dz", &dz );

	if ( status.IsOk() )
	{
		CModel&	model = io_cmd->getModel();

		int workType = IUNDEFINED;
		io_cmd->getInt( "wp", &workType );

		int machType = IUNDEFINED;
		io_cmd->getInt( "machtype", &machType );

		if (workType == IUNDEFINED)
			model.Header().getInt( "WorkplaneType", &workType );

		if (machType == IUNDEFINED)
			model.Header().getInt( "MachineType", &machType );

		CImportUtil importUtil;
		importUtil.Init( &model, NULL);

		status = importUtil.StockUpdate( dx, dy, dz, workType, machType );
	}

	if ( !status.IsOk() )
		status.Internal( IDS_INTERNAL_ERROR, "CModelProcessApp::ModelStockUpdate()" );

	return status;
}

// Model:IsRightHanded:
//   returns bool
CReturn
CModelProcessApp::ModelIsRightHanded( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	int rh = io_cmd->getModel().IsRightHanded();
	io_cmd->setInt( "bool", rh );
	return CReturn( STATUS_OKAY );
}

// ==================================================================
// Model:VarsLoad: key=%s, prefix=%s
CReturn 
CModelProcessApp::ModelVarsLoad( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn status;

	const int MAXBUF = 256;
	char entryName[MAXBUF];
	char value[MAXBUF];

	HKEY hKey;
	DWORD indx;
	DWORD entrySize;
	DWORD valueSize;
	DWORD typeCode;
	LONG lResult = -1;

	CModel& model = io_cmd->getModel();

	CString key = io_cmd->VarList().getString( "key", "" );
	CString prefix = io_cmd->VarList().getString( "prefix", "" );

	if ( !key.IsEmpty() && !prefix.IsEmpty() )
		lResult = RegOpenKeyEx( CRegister::HKEYGet(), key, 0, KEY_ENUMERATE_SUB_KEYS, &hKey );

	if (lResult == ERROR_SUCCESS)
	{
		prefix += '.';

		indx = 0;
		while (1)
		{
			entrySize = MAXBUF;
			valueSize = MAXBUF;
			lResult = RegEnumValue(
				hKey, indx, entryName, &entrySize, NULL, &typeCode, (BYTE*) value, &valueSize );

			if (lResult == ERROR_NO_MORE_ITEMS)
				break;

			if (strncmp( entryName, prefix, prefix.GetLength() ) == 0)
				model.pHeader()->setString( entryName, value );

			++indx;
		}
	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CModelProcessApp::VarsLoad()" );
	}

	RegCloseKey(hKey);

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Model:CTGRead: id=%d, path=%s
// Returns bool = %d   (0) failed / (1) success
//
CReturn 
CModelProcessApp::ModelCTGRead( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;
	CDbTool*	dbTool;
	CString		path;
	ID			id;

	CModel&	model = io_cmd->getModel();

	id = io_cmd->VarList().getInt( "id", 0 );
	path = io_cmd->VarList().getString( "path", "" );

	if (id > 0 && !path.IsEmpty())
	{
		status = model.EntityFind( id, (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
		if (dbTool != NULL)
		{
			status = dbTool->CustomToolInit( path );
		}
	}

	io_cmd->setInt( "bool", (( status.IsOk() ) ? 1 : 0) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Model:CTGSave: path=%s
// Returns bool = %d   (0) failed / (1) success
//
CReturn 
CModelProcessApp::ModelCTGSave( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	CString	path;
	bool	okay;
	
	CModel&	model = io_cmd->getModel();

	okay = FALSE;

	path = io_cmd->VarList().getString( "path", "" );
	if ( path.IsEmpty() )
	{
		status.Internal( IDS_INTERNAL_ERROR, "CModelProcessApp::ModelCTGSave()" );
	}
	else
	{
		const double	TOL = 1.e-5;

		CDbEntityArray	dbEntities;
		CDbIterator		iter;
		CDbEntity*		dbEntity;
		CDbTool*		dbTool;
		CDbProfile*		dbProfile;
		CDbEntity*		dbOwner;
		int				count, indx;

		iter.Init( model.Db(), DBLINE );
		while (1)
		{
			dbEntity = iter();
			if (dbEntity == NULL)
				break;

			dbTool = dbEntity->Tool();
			if (dbTool != NULL && dbTool->Name().CompareNoCase(STR_STOCK) != 0)
			{
				switch (dbEntity->Type())
				{
				case DBLINE:
				case DBARC:
					dbOwner = dbEntity->Owner();
					if ((dbOwner == NULL) || (dbOwner->Type() != DBPROFILE))
						dbEntities.Append( dbEntity );
					break;
				case DBPROFILE:
					dbProfile = dynamic_cast<CDbProfile*>( dbEntity );
					count = dbProfile->Count();
					for (indx = 0; indx < count; ++indx)
						dbEntities.Append( (*dbProfile)[indx] );
					break;
				default:
					break;
				}
			}

			iter.Next();
		}

		count = dbEntities.Count();
		if (count > 0)
		{
			FILE* f = fopen( path, "w" );
			if (f != NULL)
			{
				C2dCoord pt;
				for (indx = 0; indx < count; ++indx)
				{
					CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( dbEntities[indx] );
					if (dbCurve != NULL)
						CurveSave( f, (*dbCurve), &pt );
				}

				fclose(f);

				okay = TRUE;
			}
		}
	}

	io_cmd->setInt( "bool", (okay ? 1 : 0) );

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//	Model:Purge:
//
CReturn 
CModelProcessApp::ModelPurge( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn		status;
	CDbIterator	iter;
	CDbTool*	dbTool;

	CModel&	model = io_cmd->getModel();

	iter.Init( model.Db(), DBTOOL );
	while (1)
	{
		dbTool = dynamic_cast<CDbTool*>( iter() );
		if (dbTool == NULL)
			break;

		if (dbTool->RefCnt() < 1)
			dbTool->Delete();

		iter.Next();
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Apply the indicated layer map to the selected entities.
// Model:LayerMapApply: db=%s, toolsetupid=%d, layersetupid=%d
//
// Much code was pilfered from CFileProcessApp::ImportDXF()
//
CReturn
CModelProcessApp::LayerMapApply( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	CString	dbPath;
	int		toolSetupID;
	int		layerSetupID;

	CModel&	model = io_cmd->getModel();
	CSelector& selector = model.SelectorStack()();

	const CVarList& params = io_cmd->VarList();

	dbPath			= params.getString( "db", "" );

	toolSetupID		= params.getInt( "toolsetupid", 0 );
	layerSetupID	= params.getInt( "layersetupid", 0 );

	if ((selector.Count() > 0) && (toolSetupID > 0) && (layerSetupID > 0))
	{
		CImportUtil	importUtil;
		CAutoMod	autoMod;
		CAutoModDb	autoModDb;

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Extract the necessary data from the cmdb.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		status = autoModDb.DbOpen( dbPath );
		if (status.IsOk() )
		{
			status = autoModDb.Init(
								dbPath,
								toolSetupID,
								layerSetupID,
								0,
								FALSE );
		}

		if ( status.IsOk() )
		{
			importUtil.Init( &model, &autoModDb );

			status = importUtil.ProfilesCreate();
		}

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Tool-up the geometry as necessary.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		if ( status.IsOk() )
		{
			autoMod.Init( &autoModDb, &model );

			status = autoMod.ModelPostProcess( FALSE, FALSE );
		}

		autoModDb.DbClose();

	}
	else
	{
		status.Internal( IDS_INTERNAL_ERROR, "CModelProcessApp::LayerMapApply(#1)" );
	}

	return status;
}

// Model:StatisticsGet:file=%s
CReturn CModelProcessApp::StatisticsGet( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;
	
	CString path = io_cmd->VarList().getString( "file", "" );
	if ( !path.IsEmpty() )
	{
		CMM2 reader;
		int flags = (MM2_SKIP_SEQ_OBJS | MM2_KEEP_SEQ_ORDER | MM2_SKIP_WORK_ZONES);
	
		CModel model;
		status = reader.Read( path, &model, flags );

		if ( status.IsOk() )
		{
			double cutDistance;
			int numPierces;
			CModelUtil::StatisticsGet( model, &cutDistance, &numPierces );

			io_cmd->setReal( "travel", cutDistance );
			io_cmd->setReal( "pierces", numPierces );
		}
	}

	return status;
}

CReturn CModelProcessApp::WorkZonesWrite( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CString path = io_cmd->VarList().getString( "file", "" );
	if ( !path.IsEmpty() )
	{
		FILE* f = fopen( path, "a+" );
		if (f != NULL)
		{
			CDbEntityArray zones;

			CModel&	model = io_cmd->getModel();

			CDbIterator	iter;
			iter.Init( model.Db(), DBFEATURE );
			while (1)
			{
				CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( iter() );
				if (dbFeature == NULL)
					break;

				if ( dbFeature->IsWorkZone() )
					zones.Append( dbFeature );

				iter.Next();
			}

			int count = zones.Count();
			fprintf( f, "%d\n", count );

			for (int indx = 0; indx < count; ++indx)
			{
				CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( zones.GetAt(indx) );

				double left = dbFeature->DoubleGet( "_zone_left", 0. );
				double top = dbFeature->DoubleGet( "_zone_top", 0. );
				double right = dbFeature->DoubleGet( "_zone_right", 0. );
				double bottom = dbFeature->DoubleGet( "_zone_bottom", 0. );

				fprintf( f, "%f %f %f %f\n", left, top, right, bottom );
			}

			fclose( f );
		}
	}

	return status;
}

CReturn CModelProcessApp::ClampsWrite( CCommand* io_cmd )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CReturn	status;

	CString path = io_cmd->VarList().getString( "file", "" );
	if ( !path.IsEmpty() )
	{
		CModel&	model = io_cmd->getModel();

		CDbIterator	iter;
		iter.Init( model.Db(), DBFEATURE );
		while (1)
		{
			CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( iter() );
			if (dbFeature == NULL)
				break;

			if ( dbFeature->IsWorkZone() )
				dbFeature->WriteClampsToFile( path );

			iter.Next();
		}
	}

	return status;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void
CModelProcessApp::HoldDownsUpdate( const CModel& model )
{
	CDbIterator iter;
	CDbFeature*	dbFeature;

	iter.Init( model.Db(), DBFEATURE );
	while (1)
	{
		dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
			break;

		if ( dbFeature->IsWorkZone() )
			dbFeature->HoldDownUpdate( model.Header() );

		iter.Next();
	}
}

// Write the max dropdoor dimensions to the model header.
void
CModelProcessApp::DropDoorUpdate( const CAutoModDb& autoModDb, CModel* model )
{
	CString	name;
	double	dval;

	m_dropdoor[0] = 0.;
	m_dropdoor[1] = 0.;

	name = "Punch_Dropdoor_Max_Length";
	dval = autoModDb.MachineAttributes().getReal( name, 0. );
	DropDoorValueRecord( name, dval );

	name = "Punch_Dropdoor_Max_Width";
	dval = autoModDb.MachineAttributes().getReal( name, 0. );
	DropDoorValueRecord( name, dval );

	name = "Torch_Dropdoor_Max_Length";
	dval = autoModDb.MachineAttributes().getReal( name, 0. );
	DropDoorValueRecord( name, dval );

	name = "Torch_Dropdoor_Max_Width";
	dval = autoModDb.MachineAttributes().getReal( name, 0. );
	DropDoorValueRecord( name, dval );

	DropDoorRecord( model );
}

void
CModelProcessApp::DropDoorValueRecord( const CString& name, double dval )
{
	int indx = ((name.Find("Length") > 0) ? 0 : 1);
	if (dval > m_dropdoor[indx])
		m_dropdoor[indx] = dval;
}

void
CModelProcessApp::DropDoorRecord( CModel* model )
{
	model->pHeader()->setReal( "DoorDx", m_dropdoor[0] );
	model->pHeader()->setReal( "DoorDy", m_dropdoor[1] );
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void CurveSave( FILE* f, const CDbCurve& dbCurve, C2dCoord* pe )
{
	if (dbCurve.Type() == DBLINE)
	{
		const CDbLine& dbLine = dynamic_cast<const CDbLine&>( dbCurve );
		LineSave( f, dbLine, pe );
	}
	else
	{
		const CDbArc& dbArc = dynamic_cast<const CDbArc&>( dbCurve );
		ArcSave( f, dbArc, pe );
	}
}

void LineSave( FILE* f, const CDbLine& dbLine, C2dCoord* pe )
{
	const double TOL = 1.e-5;
	C2dCoord	ps;

	ps = dbLine.StartPt();
	if ( !pe->WithinTol( ps, TOL ) )
	{
		fprintf( f, "0 %10.5f %10.5f\n", ps.X(), ps.Y() );
		(*pe) = ps;
	}

	ps = dbLine.EndPt();
	if ( !pe->WithinTol( ps, TOL ) )
	{
		fprintf( f, "1 %10.5f %10.5f\n", ps.X(), ps.Y() );
		(*pe) = ps;
	}
}

void ArcSave( FILE* f, const CDbArc& dbArc, C2dCoord* pe )
{
	const double TOL = 1.e-5;
	C2dCoord	ps;

	ps = dbArc.StartPt();
	if ( !pe->WithinTol( ps, TOL ) )
	{
		fprintf( f, "0 %10.5f %10.5f\n", ps.X(), ps.Y() );
		(*pe) = ps;
	}

	ps = dbArc.CenterPt();
	if (dbArc.Dir() < 0)
		fprintf( f, "2 %10.5f %10.5f\n", ps.X(), ps.Y() );
	else
		fprintf( f, "3 %10.5f %10.5f\n", ps.X(), ps.Y() );

	ps = dbArc.EndPt();
	// if ( !pe->WithinTol( ps, TOL ) )  // might have full circle
	{
		fprintf( f, "1 %10.5f %10.5f\n", ps.X(), ps.Y() );
		(*pe) = ps;
	}
}
