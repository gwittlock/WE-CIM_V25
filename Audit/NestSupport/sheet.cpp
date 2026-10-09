
#include "stdafx.h"
#include "assert.h"
#include <float.h>

#include "Sheet.h"
#include "Register.h"
#include "MathConst.h"
#include "CommonFlags.h"
#include "StringConst.h"
#include "MM2.h"

#include "DbLine.h"
#include "DbArc.h"
#include "DbHole.h"
#include "DbTool.h"
#include "DbPoint.h"
#include "DbIterator.h"
#include "DbCommand.h"

#include "3dCoord.h"
#include "FileSnoop.h"
#include "Conversion.h"
#include "Worm.h"
#include "PartBin.h"
#include "ModelUtil.h"
#include "Solution.h"
#include "Int2d.h"

#include "DaoAttrib.h"

#include "Profile.h"
#include "Conversion.h"
#include "SeedMgr.h"

#include "ChPart.h"
#include "ChTool.h"
#include "ConvexHull.h"

// Thank the Lord for people who share their hard work!
#include "ProgressWnd.h"

const double BUMP_FACTOR = 3.0;

const double SEED_FACTOR = 0.50;
const double INFLUENCE_FACTOR = (SEED_FACTOR*0.50);

double g_mid_ord = 0.0;
int g_mid_axis = 0;

static int DBG1 = 0;
static int DBG2 = 0;
static int DBG3 = 0;
static int DBG4 = 0;
static int DBG5 = 0;
static int DBG6 = 0;  // PierceHoleAvoid()

// Bit flags for do_overlap().
#define OVERLAP_NONE         0x00
#define OVERLAP_LEAD_HULL    0x01
#define OVERLAP_SHEET_CURVE  0x02

// ==================================================================

static CSelectorStack sheet_selector_stack;
static CTreeViewSupport	sheet_tree_view;

CSheet::CSheet()
{
	m_matkey = -1;
	m_remnkey = -1;

	m_quantity = 0;
	m_repeat = 0;
	m_cutback = 0;
	m_toolhit = NULL;
	//
	// TODO:  Move allocation into later code
	//
	m_model = new CModel();
	m_weownthemodel = true;
	m_model->Init( sheet_selector_stack, sheet_tree_view );
	m_model->UndoBufferSuppress();

	m_seed_mgr = NULL;

	m_nested_area = NULL;
	m_config = NULL;
	m_view_mgr = NULL;

	m_ignore_lead = true;
	m_pierce_bump = false;

	// Default to the behavior exhibited prior to
	// introducing the changes related to 'm_retain_seeds'.
	m_retain_seeds = false;

	m_fine_tuning = false;
	m_nesting_phase = PHASE_MAIN;
}

CSheet::~CSheet()
{
	ToolHitDelete();

	if (m_model && m_weownthemodel)
		delete m_model; 

	m_model=NULL;
	m_config = NULL;

	m_part_places.DestructiveFlush();

	delete m_nested_area;
	m_nested_areas.DestructiveFlush();

	m_lead_boxes.DestructiveFlush();
}

void CSheet::ViewMgr( CViewMgr* view_mgr )
{
	m_view_mgr = view_mgr;
}

void CSheet::ConfigSet( const CNestConfig* config )
{
	m_config = config;
}

// grid space
const C2dBox& CSheet::WorkZone() const
{
	return m_workzone;
}

// grid space
void CSheet::WorkZone( const C2dBox& zone )
{
	m_workzone = zone;
}

// ==================================================================
//	Set up a sheet w/o loading anything; based on the model in memory
CReturn CSheet::Init( CModel* model )
{
	CReturn		ret;

	if (m_model && m_weownthemodel) 
		delete m_model;

	m_model = model;
	m_weownthemodel = false;

	m_matkey = -1;
	m_remnkey = -1;

	double length = m_model->Header().getReal( STR_LENGTH, 0.0 );
	double width = m_model->Header().getReal( STR_WIDTH, 0.0 );
	double thickness = m_model->Header().getReal( STR_THICKNESS, 0.0 );

	if ( ZERO(length) || ZERO(width) )
	{
		ret.Fatal( IDS_ZERO_SHEET, length, width, thickness );
		return ret;
	}
	m_extent = C3dBox( 0,0,0,length,width,thickness );

	m_cutback = 0.0;
	ToolHitDelete();
	m_toolhit = build_toolhit( NULL );
	// 
	// Panel layer, default layer
	// TODO: REDUNDANT with much of CreateStock()... PUT IN COMMON METHOD
	//
	CDbWorkplane* workplane = NULL;
	m_model->EntityFind( STR_TOP, (CDbEntity**)&workplane, DBWORKPLANE, DBWORKPLANE );
	m_model->ActiveWorkplane( workplane );

	return ret;
}

void CSheet::SeedMgr( CSeedMgr* seed_mgr )
{
	m_seed_mgr = seed_mgr;
}

CSeedMgr* CSheet::SeedMgr()
{
	return m_seed_mgr;
}

// ==================================================================

static char* inventory_name[2] = { "Remnant Inventory", "Material Inventory" };

CReturn CSheet::Load( const CNestConfig& config )
{
	CReturn		ret;

#if (_CI || _NST)

	ret = LoadCI(config);

#else
	CDaoDB		part_db;
	int			inventory_tab[2];
	int			first_tab;

	if (config.RemnantFirst())
		first_tab = 0;
	else
		first_tab = 1;

	inventory_tab[0] = part_db.addTable( "Remnant Inventory" );
	inventory_tab[1] = part_db.addTable( "Material Inventory" );

	ret += part_db.Open( config.PdbPath() );
	if (ret.isOkay())
	{
		for (int idx=0; idx<=1; idx++)
		{
			int sub_idx = idx ^ first_tab;

			if (part_db.countRecord( inventory_tab[sub_idx] ) < 1 )
				continue;
			
			if (!part_db.moveRecord( inventory_tab[sub_idx], 0 ).isOkay())
			{
				ret.User( IDS_DB_RECORD, config.PdbPath(), inventory_name[sub_idx], 0 );
				return ret;
			}

			while (!part_db.isEOF( inventory_tab[sub_idx] ))
			{
				if (sub_idx)
				{
					m_stock_array.Add( part_db.getInt( inventory_tab[sub_idx], "ID" ) );
					m_remnant_array.Add( -1 );
				}
				else
				{
					m_stock_array.Add( -1 );
					m_remnant_array.Add( part_db.getInt( inventory_tab[sub_idx], "ID" ) );
				}

				part_db.moveRecord( inventory_tab[sub_idx], 1 );
			}
		}
		// Done!
		part_db.Close();
	}

	m_list_index = -1;
	m_quantity = 0;
#endif

	return ret;
}

// ============================================================================

CReturn CSheet::LoadCI( const CNestConfig& config )
{
	CReturn		ret;
	CDaoDB		part_db;

	m_list_index = -1;
	m_quantity = 0;
	//
	//
	int inventory_tab = part_db.addTable( "MaterialInventory" );

	ret += part_db.Open( config.PdbPath() );
	if (!ret.isOkay())
	{ return ret; }

	if (!part_db.moveRecord( inventory_tab, 0 ).isOkay())
	{
		ret.User( IDS_DB_RECORD, config.PdbPath(), inventory_name[1], 0 );
		return ret;
	}

	while (!part_db.isEOF(inventory_tab))
	{
		// Only one type of record, InternalName = MaterialID
		m_stock_array.Add( part_db.getInt( inventory_tab, "Value" ) );
		m_remnant_array.Add( -1 );

		part_db.moveRecord( inventory_tab, 1 );
	}

	// Done!
	part_db.Close();

	return ret;
}

// ==================================================================

CReturn CSheet::LoadStock( const CNestConfig& config, int in_matkey )
{
	CReturn		ret;

#if (_CI || _NST)
	
	ret = LoadStockCI(config, in_matkey);

#else

	CDaoDB	part_db;
	int		mat_tab;

	m_matkey = in_matkey;
	m_remnkey = -1;

	mat_tab = part_db.addTable( "Material Inventory" );

	ret += part_db.Open( config.PdbPath() );
	if (ret.isOkay())
	{
		// Load nesting details
		if (part_db.findRecord( mat_tab, "ID", in_matkey ) < 0)
		{
			ret.User( IDS_DB_RECORD, config.PdbPath(), "Material Inventory", in_matkey );
			return ret;
		}

		double length	= part_db.getDouble( mat_tab, "Length" );
		double width	= part_db.getDouble( mat_tab, "Width" );
		double thickness= part_db.getDouble( mat_tab, "Thickness" );

		if ( ZERO(length) || ZERO(width) )
		{
			ret.Fatal( IDS_ZERO_SHEET, length, width, thickness );
			return ret;
		}

		m_extent = C3dBox( 0,0,0,length,width,thickness );

		m_weight = part_db.getDouble( mat_tab, "Weight" );

		m_quantity = part_db.getInt( mat_tab, "Quantity" );

		// Carried along
		m_costper = part_db.getDouble( mat_tab, "Cost_Per_Unit" );
		m_descrip = part_db.getString( mat_tab, "Description" );
		m_typeid = part_db.getInt( mat_tab, "Type_ID" );

		// Done!
		part_db.Close();
	}
#endif

	return ret;
}

// ============================================================================

CReturn CSheet::LoadStockCI( const CNestConfig& config, int in_matkey )
{
	CReturn		ret;
	CDaoDB		cidb;
	int			mat_tab;

	m_matkey = in_matkey;
	m_remnkey = -1;

	mat_tab = cidb.addTable( "MaterialVar" );

	ret += cidb.Open( config.CmdbPath() );
	if (!ret.isOkay())
		return ret;

	CDaoAttrib attrib(&cidb, "MaterialVar");

	double length	= attrib.ciDouble("Material_Length", "MaterialID", m_matkey, 1.0);
	double width	= attrib.ciDouble("Material_Width", "MaterialID", m_matkey, 1.0);
	double thickness= attrib.ciDouble("Material_Thickness", "MaterialID", m_matkey, 1.0);

	if ( ZERO(length) || ZERO(width) )
	{
		ret.Fatal( IDS_ZERO_SHEET, length, width, thickness );
		return ret;
	}

	m_extent = C3dBox( 0,0,0,length,width,thickness );

//	m_weight = attrib.ciDouble( mat_tab, "Weight", "MaterialID", m_matkey );
	m_weight = 0.0;

#if (_NST)
	m_quantity = -1;  // ie. unlimited.
#else
	m_quantity = attrib.ciInt("Material_Qty", "MaterialID", m_matkey, 1);
#endif

	// Carried along
	m_costper = 0.0;
	m_descrip = attrib.ciString("Material_Name", "MaterialID", m_matkey, "<error>");
	m_typeid = 0;

	// Done!
	cidb.Close();

	return ret;
}

// ==================================================================

CReturn CSheet::LoadRemnant( const CNestConfig& config, int in_remnkey )
{
	CReturn	status;

#if (_CI || _NST)
	
	status = CReturn(STATUS_ERROR);

#else

	CDaoDB	part_db;
	int		remn_tab;

	m_matkey = -1;
	m_remnkey = in_remnkey;

	remn_tab = part_db.addTable( "Remnant Inventory" );

	status += part_db.Open( config.PdbPath() );
	if (status.isOkay())
	{
		// Load nesting details
		if (part_db.findRecord( remn_tab, "ID", in_remnkey ) < 0)
		{
			status.User( IDS_DB_RECORD, config.PdbPath(), "Remnant Inventory", in_remnkey );
			return status;
		}

		m_remname = part_db.getString( remn_tab, "Filename" );

		double length = part_db.getDouble( remn_tab, "Length" );
		double width = part_db.getDouble( remn_tab, "Width" );
		double thickness= part_db.getDouble( remn_tab, "Thickness" );

		if ( ZERO(length) || ZERO(width) )
		{
			status.Fatal( IDS_ZERO_SHEET, length, width, thickness );
			return status;
		}

		m_extent = C3dBox( 0,0,0,length,width,thickness );

		m_weight = part_db.getDouble( remn_tab, "Weight" );

		m_quantity = part_db.getInt( remn_tab, "Quantity" );

		// Carried along
		m_costper = part_db.getDouble( remn_tab, "Cost_Per_Unit" );
		m_typeid = part_db.getInt( remn_tab, "Type_ID" );

		// Done!
		part_db.Close();
	}
#endif

	return status;
}

// ==================================================================
//	Prep a fresh sheet for nesting... error if unable to create a
//	new sheet.
CReturn CSheet::Prepare(
	const CNestConfig&	config,
	CViewMgr&			view,
	CModel&				proto )		// Prototype for tools, header info
{
	CReturn	status;
	CString	filepath;

	EWMFile( FILE_INFO );

	while ( (m_quantity == 0) && status.IsOk() )
	{
		status += Next( config );
	}

	if (m_quantity == 0)
	{
		status.User( IDS_NEST_NO_SHEETS );
		return status;
	}

	if (m_quantity > 0)
		m_quantity--;

	m_cutback = 0.0;

	if ( status.IsOk() )
	{
		// Not sure if this is the best place to update the header.
		// Could be deferred until the point where we save the model.
		filepath = config.MachineString( "Clamp_CTG", "" );
		m_model->pHeader()->setString( "Clamp_CTG", filepath );

		filepath = config.MachineString( "HoldDown_CTG", "" );
		m_model->pHeader()->setString( "Hold_CTG", filepath );

		m_model->pHeader()->setString( "MatCfg", m_descrip );

		if (IsRemnant())
			status = PrepareRemnant( config, proto );
		else
			status = PrepareStock( config, proto );

		if ( status.IsOk() )
		{
			// Embed the PDB file in the header
			m_model->pHeader()->setString( "pdb_file", config.PdbPath() );

			if ( config.DebugBmp() )
			{
				// Draw scanned and filtered sheet
				pGrid()->Draw( view, 0, 0, false, true );
			}

			// Refresh

			CCommand cmd;
			cmd.setCommand( "null:", m_model, &view );
			view.Clear();
			view.ModelSet( *m_model );
			view.Full( &cmd );
		}
	}

	return status;
}

CReturn CSheet::Next( const CNestConfig& config )
{
	CReturn	ret;
	int		key;

	m_list_index++;

	if (m_list_index >= m_stock_array.GetSize())
	{
		ret.User( IDS_NEST_NO_SHEETS );
		return ret;
	}

	if ((key = m_stock_array[m_list_index]) >= 0)
		LoadStock( config, key );
	else if ((key = m_remnant_array[m_list_index]) >= 0)
		LoadRemnant( config, key );

	return ret;
}

// ==================================================================

CReturn CSheet::PrepareRemnant( 
	const CNestConfig&	config,
	CModel&				proto )		// Prototype for tools, header info
{
	CReturn	ret;
	CMM2 mm2;

	m_model->Flush();

	ret += mm2.Read( m_remname, m_model, (MM2_STD_MODE | MM2_SKIP_SEQ_OBJS) );
	if (!ret.isOkay())
	{
		ret.User( IDS_NEST_BAD_REMN, m_remname );
		return ret;
	}
	m_model->UndoBufferSuppress();

	// And just why do we do this?
	CDbTool* layer = NULL;
	m_model->EntityFind( "Default", (CDbEntity**)&layer, DBTOOL, DBTOOL );
	m_model->ActiveTool( layer );

	RemnantWorkplaneSet();

	// Carry across the proto tooling
	ToolsCopy( proto, m_model );

	// Remnant -- ask the file for our extents
	m_extent = remnant_shift( m_model );
	
	ret = MaterialProcess( config, m_extent.XY(), false );

	return ret;
}

// ==================================================================

CReturn CSheet::PrepareStock( 
	const CNestConfig&	config,
	CModel&				proto )		// Prototype for tools, header info
{
	CReturn	status;
	C2dBox	tmp;

	CreateStock( proto, m_extent, m_model );

	// Nesting setup
	if ( config.GridNest() )
	{
		// We expand the bounding box of the material by the spacing
		// because when placing a square part in the lower-left corner
		// of the sheet, we do not want TestCore() (via exact_score())
		// to tell us the part is offsheet.
		tmp = m_extent;
		tmp += config.Spacing();
	}

	// QUESTION: Just what should tmp be when GridNest() returns false?
	status = MaterialProcess( config, tmp, true );

	return status;
}

// ==================================================================

// NOTE: PrepareBlank() appears to be used only for pre-nesting.
CReturn CSheet::PrepareBlank( 
	const CNestConfig&	config,
	C2dBox&				extent,
	CModel&				proto )		// Prototype for tools, header info, and SIZE
{
	CReturn	ret;

	m_extent = C3dBox( 0, 0, 0, extent.Dx(), extent.Dy(), 0. );

	ret = PrepareStock( config, proto );

	FitZone( config, m_extent, 0. );

	// Panel layer, default layer
	// TODO: REDUNDANT with much of CreateStock()... PUT IN COMMON METHOD
	//
	CDbWorkplane* workplane = NULL;
	m_model->EntityFind( STR_TOP, (CDbEntity**)&workplane, DBWORKPLANE, DBWORKPLANE );
	m_model->ActiveWorkplane( workplane );

	return ret;
}


// ==================================================================
//	Build up a blank sheet of material... Stock panel is in WORLD
CReturn CSheet::CreateStock( 
	CModel&		proto, 
	C3dBox&		extent, 
	CModel*		model )
{
	CReturn		ret;
	CDbIterator iter;

	model->Flush();

	//
	// Copy across header from prototype model
	//
	*(model->pHeader()) = proto.Header();
	*(model->pDefault()) = proto.Default();

	//
	// Override header information
	//
	model->pHeader()->setReal( STR_LENGTH, extent.Dx() );
	model->pHeader()->setReal( STR_WIDTH, extent.Dy() );
	model->pHeader()->setReal( STR_THICKNESS, extent.Dz() );
	model->pHeader()->setColor( 0 );

	if ( ZERO(extent.Dx()) || ZERO(extent.Dy()) )
	{
		ret.Fatal( IDS_ZERO_SHEET, extent.Dx(), extent.Dy(), extent.Dz() );
		return ret;
	}

	//
	// Bring across all workplanes, T-adjusted by current panel size
	//
	CDbWorkplane*	new_plane;
	iter.Init( proto.Db(), DBWORKPLANE );
	while (true)
	{
		const CDbWorkplane* plane = dynamic_cast<const CDbWorkplane*>( iter() );
		if (!plane)
			break;
		iter.Next();

		C3x4Matrix xform = plane->Transform();
		C3dVec tvec = (C3dCoord)xform.getT();

		// 2012.09.08 (PE) -- I'm not really sure why Edwin scaled the
		// translation vector; perhaps it had something to with modeling
		// or rendering when the software supported wood applications.
		// At this time, however, I know the translation causes problems
		// for the Z-values in fab applications, where all the source-code
		// (pretty much) ensures everything is at Z0. So .....
		tvec.Z( 0. );

		if (!EQUAL( tvec.Length(), 0.0 ))
		{
			tvec = tvec * (1 / tvec.Length());
			tvec.X( tvec.X() * extent.Dx() );
			tvec.Y( tvec.Y() * extent.Dy() );
			tvec.Z( tvec.Z() * extent.Dz() );
		}

		ret += model->EntityCreate( DBWORKPLANE, (CDbEntity**)&new_plane );

		new_plane->Init(	xform.getI(),
							xform.getJ(),
							xform.getK(),
							tvec,
							plane->ToolUp() );
		new_plane->Name( plane->Name() );
		new_plane->Hide();
	}

	// Panel tool, panel lines, default tool
	CDbTool* tool = NULL;
	model->EntityCreate( DBTOOL, (CDbEntity**)&tool );
	tool->Name( STR_STOCK );
	model->ActiveTool( tool );

	CDbWorkplane* workplane = NULL;
	model->EntityFind( STR_WORLD, (CDbEntity**)&workplane, DBWORKPLANE, DBWORKPLANE );
	model->ActiveWorkplane( workplane );

//
// TODO:  Break out Poly creation to common method for Remnants
//

#if (_CI || _NST)

	//------ top ------
	ret = MaterialCreate( extent.Dx(), extent.Dy(), 0., model );

	//------ bottom ------
	ret = MaterialCreate( extent.Dx(), extent.Dy(), -extent.Dz(), model );

#else

	ret = MaterialCreate( extent.Dx(), extent.Dy(), extent.Dz(), model );

	model->EntityCreate( DBTOOL, (CDbEntity**)&tool );
	tool->Name( "Default" );
	tool->ColorSet( DCOLOR_YELLOW );
	model->ActiveTool( tool );

#endif

	// Carry across the proto tooling
	ToolsCopy( proto, model );

	return ret;
}

// ==================================================================
//	Add part and kerf to the sheet to prevent parts from shifting 
//	off the current fill barrier
//
//	Removes any previous fill barrier lines
CReturn CSheet::FillBarrier(
	CNestConfig& config,
	const C2dBox& extent )
{
	CReturn ret;

	CGeoPoly* part = ReservePart(RESERVE_BARRIER);
	CGeoPoly* kerf = ReserveKerf(RESERVE_BARRIER);

	part->Flush();
	kerf->Flush();

	CGeoLine geo_line;

#if (_CI)
	// Since we don't have to worry about repo ...
	double xmin = config.Border(BORDER_LEFT);
	double xmax = Length() - config.Border(BORDER_RIGHT);

	double ymin = config.Border(BORDER_BOTTOM);
	double ymax = Width() - config.Border(BORDER_TOP);
#else
	double xmin = max( config.Border(BORDER_LEFT), extent.Xmin() );
	double xmax = min( Length() - config.Border(BORDER_RIGHT), extent.Xmax() );

	double ymin = max( config.Border(BORDER_BOTTOM), extent.Ymin() );
	double ymax = min( Width() - config.Border(BORDER_TOP), extent.Ymax() );

	if ( !CNestConfig::IsBitmapNest() )  // DYNATORCH
	{
		// 2006.11.05 (PE) -- Connweld noticed the left-most parts in every
		// workzone (beyond the initial workzone) were nest immediately
		// against the left edge of the workzone. Now we ensure the spacing
		// value is considered under this condition,
		xmin -= config.Spacing();
		if (config.CurrentZoneNumber() > 0)
			xmin += config.Border(BORDER_LEFT);

		ymin -= config.Spacing();
		xmax += config.Spacing();
		ymax += config.Spacing();
	}
#endif
	//
	// Bottom, Right to Left
	//
	if (config.NestPosY())
	{
		geo_line.StartPt( C3dCoord( xmax, ymin, 0.0 ) );
		geo_line.EndPt( C3dCoord( xmin, ymin, 0.0 ) );

		part->CopyAppend( geo_line );
		kerf->CopyAppend( geo_line );
	}
	//
	// Left, Bottom to Top
	//
	geo_line.StartPt( C3dCoord( xmin, ymin, 0.0 ) );
	geo_line.EndPt( C3dCoord( xmin, ymax, 0.0 ) );

	part->CopyAppend( geo_line );
	kerf->CopyAppend( geo_line );

	//
	// Top, Left to Right
	//
	if (!config.NestPosY())
	{
		geo_line.StartPt( C3dCoord( xmin, ymax, 0.0 ) );
		geo_line.EndPt( C3dCoord( xmax, ymax, 0.0 ) );
		
		part->CopyAppend( geo_line );
		kerf->CopyAppend( geo_line );
	}

	// This affects both SeedBarrier() and collect_geo_xy()
	kerf->Reverse();

	return ret;
}

// ==================================================================
//	Add part and kerf to the sheet around the clamp rectangle(s)
//
//	Removes any previous clamp barrier lines
CReturn CSheet::ClampBarrier(
	const C2dBox*	extent,
	bool			reset )
{
	CReturn ret;

	CGeoPoly* part = ReservePart(RESERVE_CLAMP);
	CGeoPoly* kerf = ReserveKerf(RESERVE_CLAMP);

	if (reset)
	{
		part->Flush();
		kerf->Flush();
	}
	if (!extent)
	{ return ret; }

	CGeoLine geo_line;
	//
	// Right, Top to Bottom
	//
	geo_line.StartPt( C3dCoord( extent->Xmax(), extent->Ymax(), 0.0 ) );
	geo_line.EndPt( C3dCoord( extent->Xmax(), extent->Ymin(), 0.0 ) );
	kerf->CopyAppend( geo_line );
	geo_line.Reverse();
	part->CopyAppend( geo_line );

	//
	// Bottom, Right to Left
	//
	geo_line.StartPt( C3dCoord( extent->Xmax(), extent->Ymin(), 0.0 ) );
	geo_line.EndPt( C3dCoord( extent->Xmin(), extent->Ymin(), 0.0 ) );
	kerf->CopyAppend( geo_line );
	geo_line.Reverse();
	part->CopyAppend( geo_line );
	//
	// Left, Bottom to Top
	//
	geo_line.StartPt( C3dCoord( extent->Xmin(), extent->Ymin(), 0.0 ) );
	geo_line.EndPt( C3dCoord( extent->Xmin(), extent->Ymax(), 0.0 ) );
	kerf->CopyAppend( geo_line );
	geo_line.Reverse();
	part->CopyAppend( geo_line );
	//
	// Top, Left to Right
	//
	geo_line.StartPt( C3dCoord( extent->Xmin(), extent->Ymax(), 0.0 ) );
	geo_line.EndPt( C3dCoord( extent->Xmax(), extent->Ymax(), 0.0 ) );
	kerf->CopyAppend( geo_line );
	geo_line.Reverse();
	part->CopyAppend( geo_line );

	return ret;
}


// ==================================================================
//	Proof-of concept test for possible client
CReturn CSheet::TestBarrier()
{
	CReturn ret;

	CGeoPoly* part = ReservePart(RESERVE_CLAMP);
	CGeoPoly* kerf = ReserveKerf(RESERVE_CLAMP);

	CEntityDb& db = m_model->Db();

	CGeoLine geo_line;

	//
	// 40,48 to 39,39
	//
	geo_line.StartPt(C3dCoord(40, 48, 0));
	geo_line.EndPt(C3dCoord(39,39,0));

	geo_line.Reverse();
	kerf->CopyAppend( geo_line );
	db.GeoConvert(geo_line, m_model->ActiveTool(), m_model->ActiveWorkplane());
	geo_line.Reverse();
	part->CopyAppend( geo_line );

	//
	// 39,39 to 40,30
	//
	geo_line.StartPt(C3dCoord(39, 39, 0));
	geo_line.EndPt(C3dCoord(40, 30, 0));

	geo_line.Reverse();
	kerf->CopyAppend( geo_line );
	geo_line.Reverse();
	part->CopyAppend( geo_line );
	db.GeoConvert(geo_line, m_model->ActiveTool(), m_model->ActiveWorkplane());
	//
	// 40, 30 to 42, 29
	//
	geo_line.StartPt(C3dCoord(40, 30, 0));
	geo_line.EndPt(C3dCoord(42, 29, 0));

	geo_line.Reverse();
	kerf->CopyAppend( geo_line );
	geo_line.Reverse();
	part->CopyAppend( geo_line );
	db.GeoConvert(geo_line, m_model->ActiveTool(), m_model->ActiveWorkplane());
	//
	// 42, 29 to 41, 40
	//
	geo_line.StartPt(C3dCoord(42, 29, 0));
	geo_line.EndPt(C3dCoord(41, 40, 0));

	geo_line.Reverse();
	kerf->CopyAppend( geo_line );
	geo_line.Reverse();
	part->CopyAppend( geo_line );
	db.GeoConvert(geo_line, m_model->ActiveTool(), m_model->ActiveWorkplane());
	//
	// 41, 40 to 42, 48
	//
	geo_line.StartPt(C3dCoord(41, 40, 0));
	geo_line.EndPt(C3dCoord(42, 48, 0));

	geo_line.Reverse();
	kerf->CopyAppend( geo_line );
	geo_line.Reverse();
	part->CopyAppend( geo_line );
	db.GeoConvert(geo_line, m_model->ActiveTool(), m_model->ActiveWorkplane());
	//
	// 42, 48 to 40, 48
	//
	geo_line.StartPt(C3dCoord(42, 48, 0));
	geo_line.EndPt(C3dCoord(40, 48, 0));

	geo_line.Reverse();
	kerf->CopyAppend( geo_line );
	geo_line.Reverse();
	part->CopyAppend( geo_line );
	db.GeoConvert(geo_line, m_model->ActiveTool(), m_model->ActiveWorkplane());

	return ret;
}

void CSheet::NestingPhaseSet( eNestingPhase phase )
{
	m_nesting_phase = phase;
}

eNestingPhase CSheet::NestingPhaseGet( ) const
{
	return m_nesting_phase;
}

void CSheet::SeedsShift( eSeedBank which_bank, C2dVec delta )
{
	m_seed_mgr->SeedsShift( which_bank, delta );
}

// ==================================================================
//	Find the optimum location and score for this gridded part
//	Returns false if we've run out of spots...
//		-1: No score
//		-2: Really no score; skip ahead farther
//		-3: 
//
static bool SHOW_PART_NAME = false;

CPartPlace CSheet::Score(
	CNestConfig&	config,
	const C3dCoord&	place,			// in WORLD...
	CToolHit*		toolhit,
	CNestedArea*	area_to_nest,
	bool			ignore_clamp,
	bool			hole )			// true if scoring into a hole
{
	CReturn status;
	CPartPlace best;
	CPartPlace trial;
	C3dCoord candidate;

	hole = true;

	EWMFile( FILE_INFO );

	// 2008.07.06 (PE) -- How did we squeek-by for so
	// long without having this condition in place?
	if ((area_to_nest != NULL) && (toolhit != NULL))
	{
		if ( SHOW_PART_NAME )
		{
			CString note;
			note.Format( "::Score(), %s", toolhit->Part()->Name() );
			EWMNesting( note );
		}

		best.ScoresInit( -3., -3.);

		candidate = place;

		if ( CNestConfig::IsBitmapNest() )  // DYNATORCH
		{
			trial = bitmap_score( config, toolhit, &candidate, hole );
		}
		else
		{
			trial = exact_score( config, toolhit, area_to_nest, ignore_clamp,
				&candidate, hole, 0 );
		}

		EWMFile( FILE_INFO );

		if ( trial.HasBetterScore( best ) )
			best = trial;

		if (trial.BumpScoreGet() < -UNDEFINED)
			best = trial;  // End-user must have cancelled.  Force termination.
	}

	return best;
}

// ----------------------------------------------------------------------------

CPartPlace CSheet::exact_score(
	CNestConfig&	config,
	CToolHit*		toolhit,
	CNestedArea*	area_to_nest,
	bool			ignore_clamp,
	C3dCoord*		place,		// in WORLD...
	bool			hole,
	int				gravity )
{
	const double MAX_SHIFT_COUNT = 8;
	const double BAD_SHIFT_COUNT = 2;

	CPartPlace	best;
	CPartPlace	trial;
	CString		note;
	CReturn		status;
	C2dBox		fitzone;
	C2dBox		part_shift_mer;
	C2dBox		pierce_shift_mer;
	C2dCoord	seed;
	double		bump_score;
	double		overlap_score;
	double		left_shift;
	double		vert_shift;
	double		max_bump;
	ePartFit	fit;

	CNestingPart* part = toolhit->Part();  // for convenience

	EWMFile( FILE_INFO );

	config.PlacementFailed( true );
	config.ShiftedOntoSheet( false );

	// Trivial oversize rejection
	CGeoPolyArray* outside_poly = toolhit->PolysGet(HIT_NEST_OUTSIDE);
	if (outside_poly->Count() < 1)
		return best;  // Something is very wrong.

	C2dBox part_mer = toolhit->Mer();

	// TODO: Put into function.
	C2dBox pierce_mer;

	FitZoneGet( config,  toolhit->Part()->Large(), &fitzone );

	// Add SMALL because I found a case of exact fit.
	if (((fitzone.Dx() + SMALL) < part_mer.Dx()) ||
		((fitzone.Dy() + SMALL) < part_mer.Dy()))
	{ 
		config.Oversize(true);
		return best;
	}
	else
	{
		config.Oversize(false);
	}

	// 2009.06.13 (PE) -- Rittal reported that "Nesting Around Clamps"
	// did not seem to be working. See use of clamp_interference_box below.
	C2dBox clamp_interference_box;
	if ( !ignore_clamp )
	{
		// Width composite
		double dval1 = config.MachineReal( "Punch_Clamp_Deadzone_Width", 0.0 );
		double dval2 = config.MachineReal( "Torch_Clamp_Deadzone_Width", 0.0 );

		double width = max( dval1, dval2 );

		double origin = width;
		if (config.YNegative())
			origin = Width();

		clamp_interference_box.Update( 0., (origin - width), (UNDEFINED - SMALL), origin );
	}

	BOOL xfill = ((config.Progression() == PROGRESS_PPX_SPY) ||
				  (config.Progression() == PROGRESS_PPX_SNY));

	bool vshift = true;
	if ( config.PreNest() )
		vshift = ((gravity ^ xfill) ? false : true);

	// (-1) nesting up the Y-axis / (+1) nesting down the Y-axis
	// These values may seem reversed but they are used in
	// determining which way to scan/shift the part. It's
	// probably best to leave this alone.
	int sign_y = (config.NestPosY() ? -1 : +1);

	CDexGrid* in_dexgrid = toolhit->Grid();

	bool offsheet = false;

	double high_score = -1.0;
	C3dCoord high_place;	// in WORLD

	bool test_fit = true;
	int shift_counter = 0;

	int did_salvage = 0;
	bool states_update;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// start of the business loop
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	EWMFile( FILE_INFO );

	m_fine_tuning = false;

	while (1)
	{
		left_shift = 0.0;
		vert_shift = 0.0;

		// The minimum enclosing rectangle of the part at this location.
		part_shift_mer = part_mer;
		part_shift_mer.Shift( place->X(), place->Y() );

		seed = m_toolhit->WorldToGrid( part_shift_mer.Xmin(), part_shift_mer.Ymin() );

		if ( pierce_mer.IsDefined() )
		{
			pierce_shift_mer = pierce_mer;
			pierce_shift_mer.Shift( place->X(), place->Y() );

			part_shift_mer += pierce_shift_mer;
		}

		// This was the original place where we displayed the candidate
		// position. At this place, we see all candidates, even those
		// that are trivially rejected.  Though it may be valuable to
		// see these candidate positions, the screen gets very cluttered.
		// As such, you can control rendering via the DBG variables.
		if (DBG1)
			Render( (*outside_poly), (*place), DBG2 );

		// Allow changes to test_fit and vshift.
		states_update = true;

		// 2009.06.13 (PE) -- Rittal reported that "Nesting Around Clamps"
		// did not seem to be working. This change forces TestCore() to be
		// called only when clamps must be considered. Furthermore, since
		// TestCore() is expensive, this bounding box test further reduces
		// the likelyhood that TestCore() will be called.
		if (!test_fit && !ignore_clamp)
		{
			test_fit = part_shift_mer.Intersects( clamp_interference_box, 1.e-3 );
		}

		// Test and tune the part's placement...
		fit = PART_FIT_ERROR;
		if (test_fit)
		{
			EWMFile( FILE_INFO );

			fit = TrivialFitStatus( fitzone, part_shift_mer );
			if (fit != PART_FIT_ERROR)
			{
				if (fit == PART_FIT_OFF_SHEET)
				{
					// Attempt to salvage this seed point under conditions
					// where the part is slightly off of the sheet.
					BorderShift( config, fitzone, part_shift_mer, &left_shift, &vert_shift );

					config.ShiftedOntoSheet( true );

					++did_salvage;
					if (did_salvage > 1)
						break;  // Simply give up.

					// Prevent changes to test_fit and vshift.
					states_update = false;
				
					// Prevent subsequent actions on this iteration.
					fit = PART_FIT_ERROR;
				}
				else
				{
					// TestCore() is simply the first line of defense. In essence, it
					// overlaps a bitmap of the part with that of the sheet and checks
					// for interference (like body/body overlap). It is certainly much
					// faster than trying to intersect geometry (as does TestFit()) but
					// it is also very sensitive to the grid tolerance of the bitmap
					// (ie. small parts or parts having small features are sometimes
					// not adequately represented, leading to overlap-detection failures).
					fit = pGrid()->TestCore(
						config, *in_dexgrid, seed, m_workzone, hole, ignore_clamp, 0 );

					if (fit == PART_FIT_ERROR)
						break;  // Simply give up.

					if ((did_salvage > 0) && (fit > PART_FIT_WARNING))
						break;  // Simply give up.
				}
			}

			if (fit != PART_FIT_ERROR)
			{
				EWMFile( FILE_INFO );

				if ( states_update )
				{
					// We must have passed grid TestCore().
					// Now test for actual geometric interference.
					fit = TestFit( config, *place, *toolhit, *area_to_nest );

					if (fit == PART_FIT_ERROR)
					{
						// Attempt to salvage this seed point under conditions
						// where the part is slightly off of the sheet.
						BorderShift( config, fitzone, part_shift_mer, &left_shift, &vert_shift );

						// Prevent changes to test_fit and vshift.
						states_update = false;
					}
				}
			}

			if (fit != PART_FIT_ERROR)
			{
				if (fit == PART_FIT_OFF_SHEET)
				{
					if (fitzone.Contains( part_shift_mer, SMALL ) )
						fit = PART_FIT;
				}

				if ((fit != PART_FIT) && (shift_counter > BAD_SHIFT_COUNT))
					fit = PART_FIT_ERROR;

				if (fit == PART_FIT_OFF_SHEET)
					offsheet |= true;
			}
		}
		else
		{
			fit = PART_FIT;

			if (!fitzone.Contains( part_shift_mer, SMALL ) )
				fit = PART_FIT_OFF_SHEET;

			// 2006.09.?? (PE) -- Connweld reported overlapping parts.
			// 1st quadrant machine, nesting away from clamps, initial
			// part placement placed part so that it overlapped the
			// edge of the sheet. Initial placement was corrected by
			// bumping the part onto the sheet. Subsequent bump should
			// *not* happen, but we catch it post-facto by looking for
			// interference of part bodies.
			if (fit == PART_FIT)
			{
				fit = pGrid()->TestCore(
					config, *in_dexgrid, seed, m_workzone, hole, ignore_clamp, MARK_BODY );

				if (fit != PART_FIT_ERROR)
					fit = PART_FIT;
			}
		}

		if (config.DebugWire() && (fit != PART_FIT_ERROR))
		{
			// At times, the view gets so cluttered with images of candidate
			// part positions that you can not see the position of interest.
			// As such, you can refresh the sheet by setting DBG3.
			Render( (*outside_poly), (*place), DBG3 );
		}

		// At this point, we know for sure the part fits.  It may not
		// be the best place, but we need to record it anyways in case
		// bumping the part fails to find a better place.  Failing to
		// do this caused a single instance of dxfdemo6 (the raygun)
		// not to be nested as the only part on a sheet).
		if (fit == PART_FIT)
		{
			EWMFile( FILE_INFO );

			// A quick and painless scoring technique.
			bump_score = BumpScore( config, *place, *toolhit );

			// A much more expensive scoring technique.
			// TODO: In the future, we may want to accumulate candidate
			// part placements based upon their bump_scores and defer
			// (calculating and) comparing their overlap scores.  This
			// approach would greatly improve nesting speeds for those
			// parts that have a fixed orientation.
			overlap_score = OverlapScore( config, *place, *toolhit, -1, sign_y );

			// Only so that we can compare scores ....
			trial.ScoresInit( bump_score, overlap_score );

			if ( trial.HasBetterScore( best ) )
			{
				high_score = overlap_score;  // high_score & high_place are redundant?
				high_place = *place;

				best.Init( (*place), seed, toolhit );
				best.ScoresInit( bump_score, overlap_score );
				
				if ( config.DebugWire() )
					Render( (*outside_poly), (*place), DBG3 );
			}
		}

		if (fit != PART_FIT_ERROR)
		{
			if (shift_counter < MAX_SHIFT_COUNT)
			{
				if (!vshift)
				{ 
					max_bump = -(BUMP_FACTOR * part_mer.Dx());
					left_shift = Bump( *place, *toolhit, *area_to_nest, BUMP_X, max_bump );
					ShiftValidate( &left_shift );
				}

				if (vshift || ZERO(left_shift))
				{ 
					max_bump = sign_y * BUMP_FACTOR * part_mer.Dy();
					vert_shift = Bump( *place, *toolhit, *area_to_nest, BUMP_Y, max_bump );
					ShiftValidate( &vert_shift );
				}

				if (vshift && ZERO(vert_shift))
				{ 
					max_bump = -(BUMP_FACTOR * part_mer.Dx());
					left_shift = Bump( *place, *toolhit, *area_to_nest, BUMP_X, max_bump );
					ShiftValidate( &left_shift );
				}
			}
		}


		if ( ZERO(left_shift) && ZERO(vert_shift) )
		{
			config.PlacementFailed( false );

			// NOTE: Not all sure about the following conditional statement.
			//   if (!config.PreNest() && (best.BumpScoreGet() > SMALL))
			if (config.LeadSetup() && (best.BumpScoreGet() > SMALL))
			{
				// Can do this conditionally (ie. only when there is a lead setup?)
				if ( !m_fine_tuning )
				{
					m_fine_tuning = true;
					m_pretuning_best = best;
				}
				else
				{
					bump_score = best.BumpScoreGet();
					overlap_score = best.OverlapScoreGet();

					best = m_pretuning_best;  // to set world and seed
					best.ScoresInit( bump_score, overlap_score );  // reset with fine-tuned scores.

					if (DBG1)
						Render( (*outside_poly), best.WorldPointGet(), DBG2 );
					break;
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			// Adjust the placement of the part.
			place->X( place->X() - left_shift );
			place->Y( place->Y() + (vert_shift * sign_y) );

			if ( states_update )
			{
				vshift = (ZERO(left_shift) ? 0 : 1);

				++shift_counter;

				test_fit = ((fit == PART_FIT) ? false : true);
			}
		}
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// end of the business loop
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	config.OffSheet( (offsheet && (high_score < 0)) );

	return best;
}

CPartPlace CSheet::bitmap_score(
	CNestConfig&	config,
	CToolHit*		toolhit,
	C3dCoord*		place,		// in WORLD...
	bool			hole )
{
	CPartPlace		best;
	CPartPlace		trial;
	CString			note;
	CReturn			status;
	C2dBox			part_shift_mer;
	C2dCoord		seed;
	double			bump_score;
	ePartFit		fit;
	C2dBox			fitzone;
	double			left_shift, vert_shift;
	bool			did_salvage = false;

	eSheetRejection	reject_score;
	double			shift_dist;
	bool			vshift, hshift;
	int				max_shift_count;

	EWMFile( FILE_INFO );

	// Trivial oversize rejection
	CGeoPolyArray* outside_poly = toolhit->PolysGet(HIT_NEST_OUTSIDE);
	if (outside_poly->Count() < 1)
		return best;  // Something is very wrong.

	C2dBox part_mer = toolhit->Mer();

	// Unlike conventional nesting, the outside kerf of the part
	// is not offset by the spacing distance. As such, we correct
	// the part_mer here so that it maps to the correct grid position.
	part_mer += config.Spacing();

	FitZoneGet( config,  toolhit->Part()->Large(), &fitzone );

	if ((m_fitzone.Dx() < part_mer.Dx()) || (m_fitzone.Dy() < part_mer.Dy()))
	{ 
		config.Oversize(true);
		return best;
	}
	else
	{
		config.Oversize(false);
	}

	eNestProgression progression = config.Progression();

	switch (progression)
	{
	case PROGRESS_PPY_SPX:
	case PROGRESS_PNY_SPX:
		vshift = TRUE;
		hshift = FALSE;
		max_shift_count = (int) (part_mer.Dy() / config.Resolution());
		break;

	case PROGRESS_PPX_SPY:
	case PROGRESS_PPX_SNY:
		vshift = FALSE;
		hshift = TRUE;
		max_shift_count = (int) (part_mer.Dx() / config.Resolution());
		break;
	}

	shift_dist = config.Resolution();

	int sign_y = config.NestPosY()?-1:+1;

	CDexGrid* in_dexgrid = toolhit->Grid();

	if (DBG1)
	{
		// So that we can see the part at the initial seed location,
		Render( (*outside_poly), (*place), DBG1 );
	}

	bool offsheet = false;
	int shift_counter = 0;

	while (1)
	{
		// For those nasty, difficult parts ....
		if ((shift_counter %10) == 0)
		{
			status = CProgressWnd::ProgressWndUpdate();
			if ( !status.IsOk() )
			{
				// Use magic number to forcer termination of client.
				best.BumpScoreSet( -(2 * UNDEFINED) );
				break;
			}
		}

		// The minimum enclosing rectangle of the part at this location.
		part_shift_mer = part_mer;
		part_shift_mer.Shift( place->X(), place->Y() );

		if ( !did_salvage )
		{
			fit = TrivialFitStatus( fitzone, part_shift_mer );
			if (fit != PART_FIT_ERROR)
			{
				if (fit == PART_FIT_OFF_SHEET)
				{
					// Attempt to salvage this seed point under conditions
					// where the part is slightly off of the sheet.
					BorderShift( config, fitzone, part_shift_mer, &left_shift, &vert_shift );

					place->X( place->X() - left_shift );
					place->Y( place->Y() - vert_shift );

					part_shift_mer = part_mer;
					part_shift_mer.Shift( place->X(), place->Y() );
				}
			}

			did_salvage = true;
		}

		if (DBG2)
		{
			// So that we can see the part at this adjusted location,
			Render( (*outside_poly), (*place), DBG2 );
		}

		reject_score = RejectionScore( config, part_shift_mer, shift_counter );
		if (reject_score == STARTED_OFFSHEET)
			break;  // Trivially reject non-recoverable offsheet conditions.

		fit = ((reject_score == IS_ONSHEET) ? PART_FIT : PART_FIT_ERROR);
		if (fit == PART_FIT)
		{
			// Map the candidate position to grid space.
			seed = m_toolhit->WorldToGrid(
				part_shift_mer.Xmin(), part_shift_mer.Ymin() );

			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// The big test!
			fit = pGrid()->TestCore(
				config, *in_dexgrid, seed, m_workzone, hole, true, 0 );
		}

		if (fit == PART_FIT_ERROR)
		{
			shift_dist = ShiftGet( config, &vshift, &hshift );
			if (shift_dist < 0)
				break;  // We've exhausted our placement options.

			if (vshift)
				max_shift_count = (int) (part_mer.Dy() / config.Resolution());
			else
				max_shift_count = (int) (part_mer.Dx() / config.Resolution());

			// Reset things so that we can bump the
			// part in the orthogonal direction.
			shift_counter = 0;
			if (best.BumpScoreGet() > 0)
			{
				// We have at least placed the part in a fair
				// location, so continue iterating from there.
				(*place) = best.WorldPointGet();
			}
		}

		if ((fit != PART_FIT) && (shift_counter > max_shift_count))
			fit = PART_FIT_ERROR;

		if ((fit != PART_FIT_ERROR) && config.DebugWire())
			Render( (*outside_poly), (*place), DBG2 );

		// At this point, we know for sure the part fits.  It may not
		// be the best place, but we need to record it anyways in case
		// bumping the part fails to find a better place.  Failing to
		// do this caused a single instance of dxfdemo6 (the raygun)
		// not to be nested as the only part on a sheet).
		//   if (fit == PART_FIT)
		if ((fit == PART_FIT) || (fit == PART_FIT_WARNING))
		{
			// A quick and painless scoring technique.
			bump_score = BumpScore( config, *place, *toolhit );

			// Only so that we can compare scores ....
			trial.ScoresInit( bump_score, 0. );

			if ( trial.HasBetterScore( best ) )
			{
				best.Init( (*place), seed, toolhit );
				best.ScoresInit( bump_score, 0. );
				
				if ( config.DebugWire() )
					Render( (*outside_poly), (*place), DBG1 );
			}
		}

		// Stop if out of hints...
		if (shift_dist < 0)
		{
			break;
		}
		else
		{
			if (hshift)
				place->X( place->X() - shift_dist );

			if (vshift)
				place->Y( place->Y() + (shift_dist * sign_y) );

			++shift_counter;
		}
	} // END while ( true )

	if (best.BumpScoreGet() > 0.)
	{
		// We at least appeared to find a good position for
		// the part.  However, it may be a false solution.
		fit = FinalRejectionScore( best.WorldPointGet(), part_mer );
		if (fit != PART_FIT)
			best.ScoresInit( -3., -3.);
	}

	config.OffSheet( (reject_score == STARTED_OFFSHEET) );

	if (DBG1)
	{
		// So that we can see the part at the initial seed location,
		(*place) = best.WorldPointGet();
		Render( (*outside_poly), (*place), DBG1 );
	}

	return best;
}

double CSheet::ShiftGet(
	const CNestConfig&	config,
	bool*				vshift,
	bool*				hshift )
{
	double	shift_dist;

	shift_dist = -UNDEFINED;

	switch (config.Progression())
	{
	case PROGRESS_PPY_SPX:
	case PROGRESS_PNY_SPX:
		if (*vshift)
		{
			shift_dist = config.Resolution();
			(*vshift) = FALSE;
			(*hshift) = TRUE;
		}
		else if (*hshift)
		{
			(*vshift) = FALSE;
			(*hshift) = FALSE;
		}
		break;

	case PROGRESS_PPX_SPY:
	case PROGRESS_PPX_SNY:
		if (*hshift)
		{
			shift_dist = config.Resolution();
			(*hshift) = FALSE;
			(*vshift) = TRUE;
		}
		else if (*vshift)
		{
			(*vshift) = FALSE;
			(*hshift) = FALSE;
		}
		break;
	}

	return shift_dist;
}

bool CSheet::HaveClampPolys()
{
	CGeoPoly* clamps = ReservePart(RESERVE_CLAMP);
	return ((clamps != NULL) && (clamps->Count() > 0));
}

// Does this part overlap geometry on the sheet?
ePartFit CSheet::TestFit(
	const CNestConfig& config,
	const C3dCoord& shift,
	const CToolHit& toolhit,
	const CNestedArea&	area_to_nest )
{
	CReturn status;
	int overlapA, overlapB, overlapC;

	EWMFile( FILE_INFO );

	C2dBox outline_mer = toolhit.Mer();

	C2dBox bump_mer = outline_mer;
	bump_mer.Shift( shift.X(), shift.Y() );

	// 2011.02.26 (PE) -- Oooops. Need to check against clamps.
	if (config.ClampAround() && HaveClampPolys())
	{
		CGeoPolyArray tmp;
		tmp.Append( ReservePart(RESERVE_CLAMP) );

		EWMFile( FILE_INFO );

		overlapA = do_overlap( shift, &tmp,
			toolhit.PolysGet(HIT_PART_OUTSIDE), bump_mer );
		if (overlapA)
			return PART_FIT_ERROR;
	}

	overlapA = do_overlap( shift, area_to_nest.PolysGet(HIT_PART_INSIDE),
		toolhit.PolysGet(HIT_NEST_OUTSIDE), bump_mer );
	if (overlapA)
		return PART_FIT_ERROR;

	overlapB = do_overlap( shift, area_to_nest.PolysGet(HIT_NEST_INSIDE),
		toolhit.PolysGet(HIT_PART_OUTSIDE), bump_mer );
	if (overlapB)
		return PART_FIT_ERROR;

	//-------
	
	bool overlap_leads = false;

	CGeoPolyArray* part_polys = toolhit.PolysGet(HIT_NEST_OUTSIDE);

	// Check against outside-kerf polys of all parts nested within this area.
	int count = area_to_nest.InteriorAreasCount();
	for (int indx = 0; indx < count; ++indx)
	{
		CNestedArea* nested_area = area_to_nest.InteriorAreaGet( indx );
		CGeoPolyArray* area_polys = nested_area->PolysGet(HIT_PART_OUTSIDE);

		if (0)
			area_polys->Draw();

		overlapC = do_overlap( shift, area_polys, part_polys, bump_mer );
		if (overlapC & OVERLAP_SHEET_CURVE)
			return PART_FIT_ERROR;

		if (overlapC & OVERLAP_LEAD_HULL)
			overlap_leads = true;
	}

	if (!m_fitzone.Contains( bump_mer, SMALL ) )
		return PART_FIT_OFF_SHEET;

	return (overlap_leads ? PART_FIT_WARNING : PART_FIT);
}

// ==================================================================
//	Calculate the Bump Distance of the part on the sheet, along the
//	specified axis.
//
//	The part has already been pre-placed on the sheet, and is assumed
//	to be valid;  in fact, Punch could be called now with impunity.
//
//double	// 0.0 if we failed to bump, otherwise the score (for the given direction)
double CSheet::Bump(
	const C3dCoord&		shift,
	const CToolHit&		candidate,
	const CNestedArea&	area_to_nest,
	eBumpDir			dir,
	double				max_bump )
{
	C2dBox		bump_mer;
	C2dBox		inverse_bump_mer;
	C3dCoord	inverse_shift;
	double		xmin, ymin;
	double		xmax, ymax;
	bool		compliment;
	int			sign;

	C2dBox outline_mer = candidate.Mer();

	sign = SGN( max_bump );
	max_bump = fabs( max_bump);

	if (dir == BUMP_X)
	{
		if (sign < 0)
		{
			xmin = outline_mer.Xmin() + shift.X() - max_bump;
			xmax = outline_mer.Xmax() + shift.X();
		}
		else
		{
			xmin = outline_mer.Xmin() + shift.X();
			xmax = outline_mer.Xmax() + shift.X() + max_bump;
		}

		ymin = outline_mer.Ymin() + shift.Y();
		ymax = outline_mer.Ymax() + shift.Y();
	}
	else
	{
		xmin = outline_mer.Xmin() + shift.X();
		xmax = outline_mer.Xmax() + shift.X();

		if (sign < 0)
		{
			ymin = outline_mer.Ymin() + shift.Y() - max_bump;
			ymax = outline_mer.Ymax() + shift.Y();
		}
		else
		{
			ymin = outline_mer.Ymin() + shift.Y();
			ymax = outline_mer.Ymax() + shift.Y() + max_bump;
		}
	}

	if (xmin < m_fitzone.Xmin())
		xmin = m_fitzone.Xmin();

	if (ymin < m_fitzone.Ymin())
		ymin = m_fitzone.Ymin();

	// 2008.02.24 (PE) -- Prenest was (wrongly) shifting when
	// there was no need to shift at all.
	if (xmax > m_fitzone.Xmax())
		xmax = m_fitzone.Xmax();

	if (ymax > m_fitzone.Ymax())
		ymax = m_fitzone.Ymax();

	// 2004.05.10 (PE) -- Why do we allow xmin < 0?
	bump_mer.Update( xmin, ymin, xmax, ymax );

	// Find the minimum distance across depth 0+3 against 1+2...

	CNestedArea* nested_area;
	CGeoPolyArray* candidate_polys;
	CGeoPolyArray* sheet_polys;
	double distA, distB;
	int count, indx;

	// QUESTION: Are both distA & distB necessary (?)
	sheet_polys = area_to_nest.PolysGet(HIT_NEST_INSIDE);
	candidate_polys = candidate.PolysGet(HIT_PART_OUTSIDE);
	distA = do_process( dir, SPROC_BUMP, shift,
		sheet_polys, candidate_polys, true, sign, bump_mer );

	sheet_polys = area_to_nest.PolysGet(HIT_PART_INSIDE);
	candidate_polys = candidate.PolysGet(HIT_NEST_OUTSIDE);
	distB = do_process( dir, SPROC_BUMP, shift,
		sheet_polys, candidate_polys, false, sign, bump_mer );

	distA = min( distA, distB );

	compliment = false;
	if ( m_pierce_bump )
	{
		inverse_shift.XYZ( -shift.X(), -shift.Y(), -shift.Z() );

		inverse_bump_mer = bump_mer;
		inverse_bump_mer.Shift( inverse_shift.X(), inverse_shift.Y() );
	}

	// TODO:
	// 3) Test candidate HIT_NEST_OUTSIDE against HIT_PART_OUTSIDE of nested_areas
	//    within area_to_nest (?)
	DBG5 = DBG4;
	
	count = area_to_nest.InteriorAreasCount();
	for (indx = 0; indx < count; ++indx)
	{
		nested_area = area_to_nest.InteriorAreaGet( indx );

		if (DBG5 && (m_view_mgr != NULL))
			m_view_mgr->Clear();

		sheet_polys = nested_area->PolysGet(HIT_NEST_OUTSIDE);
		candidate_polys = candidate.PolysGet(HIT_PART_OUTSIDE);
		distB = do_process( dir, SPROC_BUMP, shift,
			sheet_polys, candidate_polys, true, sign, bump_mer );

		distA = min( distA, distB );

		sheet_polys = nested_area->PolysGet(HIT_PART_OUTSIDE);
		candidate_polys = candidate.PolysGet(HIT_NEST_OUTSIDE);
		distB = do_process( dir, SPROC_BUMP, shift,
			sheet_polys, candidate_polys, false, sign, bump_mer );

		compliment = ( m_pierce_bump && ((distA - distB) > SMALL));

		distA = min( distA, distB );

		if ( compliment )
		{
			sheet_polys = nested_area->PolysGet(HIT_PART_OUTSIDE);
			candidate_polys = candidate.PolysGet(HIT_NEST_OUTSIDE);
			distB = do_process( dir, SPROC_BUMP, inverse_shift,
				candidate_polys, sheet_polys, false, -sign, inverse_bump_mer );

			distA = min( distA, distB );
		}
	}

	DBG5 = FALSE;

	if (fabs(distA) > (max_bump+SMALL))
		return DBL_MAX;

	return distA;
}

// Do a geometric overlap test from this part to the sheet
int CSheet::do_overlap(
	const C3dCoord&	shift, 
	CGeoPolyArray*	sheet_array,
	CGeoPolyArray*	part_array,
	const C2dBox&	bump_zone )
{
	int overlap = OVERLAP_NONE;

	int sheet_count = (sheet_array ? sheet_array->Count() : 0);
	int part_count = (part_array ? part_array->Count() : 0);

	if ((sheet_count > 0) && (part_count > 0))
	{
		CGeoCurveArray sheet_curves;
		CGeoCurveArray part_curves;

		// Assemble all geometry that overlaps the bump zone into the two lists, 
		// to reduce the amount of work done in the face-off.
		do_collect_geo_box( &part_curves, *part_array, shift, bump_zone );
	
		C3dCoord zero( 0, 0, 0 );
		do_collect_geo_box( &sheet_curves, *sheet_array, zero, bump_zone );

		EWMFile( FILE_INFO );

		if (0)
		{
			sheet_curves.Draw();
			part_curves.Draw();
		}

		// Now scan through the entire overlap zone...
		int sheet_num = sheet_curves.Count();
		int part_num = part_curves.Count();
		for (int pidx=0; pidx<part_num; pidx++)
		{
			CGeoCurve* part_curve = part_curves.GetAt(pidx);

			C2dBox part_mer = part_curve->Box();

			C2dBox shift_mer( part_mer.Xmin() + shift.X(),
							  part_mer.Ymin() + shift.Y(),
							  part_mer.Xmax() + shift.X(),
							  part_mer.Ymax() + shift.Y() );


			for (int sidx=0; sidx<sheet_num; sidx++)
			{
				CGeoCurve* sheet_curve = sheet_curves.GetAt(sidx);

				if (ent_intersect( *sheet_curve, *part_curve, shift ))
				{
					if (sheet_curve->IntGet( STR_LEAD_HULL, 0 ) != 0)
					{
						overlap |= OVERLAP_LEAD_HULL;
					}
					else
					{
						overlap |= OVERLAP_SHEET_CURVE;
						break;
					}
				}
			}

			if (overlap & OVERLAP_SHEET_CURVE)
				break;
		}
	}

	return overlap;
}

double CSheet::BumpScore(
	CNestConfig&	config,
	const C3dCoord&	place,
	const CToolHit&	toolhit ) const
{
	double		px, py;
	double		dx, dy;
	double		score;

	dx = m_extent.Dx();
	dy = m_extent.Dy();

	px = place.X() + toolhit.Mer().Xmin();
	
	switch (config.Progression())
	{
	case PROGRESS_PPY_SPX:
		// Higher score for building columns in +Y (min X)
		// (px,py) -- lower left corner of bounding box.
		py = place.Y() + toolhit.Mer().Ymin();
		score = (dy * (dx - px)) + (dy - py);
		break;

	case PROGRESS_PNY_SPX:
		// Higher score for building columns in -Y (min X)
		// (px,py) -- upper left corner of bounding box.
		py = place.Y() + toolhit.Mer().Ymax();
		score = (dy * (dx - px)) + py;
		break;
	
	case PROGRESS_PPX_SPY:
		// Higher score for building rows in +X (min Y)
		// (px,py) -- lower left corner of bounding box.
		py = place.Y() + toolhit.Mer().Ymin();
		score = (dx * (dy - py)) + (dx - px);
		break;
	
	case PROGRESS_PPX_SNY:
		// Higher score for building rows in +X (max Y)
		// (px,py) -- upper left corner of bounding box.
		py = place.Y() + toolhit.Mer().Ymax();
		score = (dx * py) + (dx - px);
		break;
	}

	dx = 0;  // so i can set a break point and change score

	return score;
}

// ==================================================================
//	Exact Scoring... takes the area between the toolhit part and the
//	relevant sheet geometry
//
//	Scores should be scaled, 0.0 for worst fit, 1.0 for best.
//
double CSheet::OverlapScore( CNestConfig& config,
					const C3dCoord&		place,
					const CToolHit&		candidate,
					int					sign_x,
					int					sign_y ) const
{
	CString note;
	CReturn ret;

	CGeoPolyArray* outside_poly = candidate.PolysGet(HIT_PART_OUTSIDE);
	if (outside_poly->Count() < 1)
		outside_poly = candidate.PolysGet(HIT_NEST_OUTSIDE);

	if (outside_poly->Count() < 1)
		return 0.;

	C2dBox outline_mer = CGeoPoly::Extent(outside_poly);

	// ---------------------------------------------
	// Specialized form for pre-nesting.  Hell, could probably
	// work for normal nesting, too; hella faster.
	//
	if (config.PreNest())
	{
		C2dBox bump_mer( 
		outline_mer.Xmin() + place.X(),
		outline_mer.Ymin() + place.Y(),
		outline_mer.Xmax() + place.X(),
		outline_mer.Ymax() + place.Y() );

		C2dBox total_mer = m_recentpart;
		total_mer += bump_mer;

		if ( !ZERO(m_recentpart.Area())
			&& m_recentpart.IsDefined() )
		{ 
			double area1;// = m_recentpart.Area();
			double area2;//= total_mer.Area();
			bool shift_fail;
			bool size_fail;

#define AXIS_STRETCH 1.5
			if ((config.Progression() == PROGRESS_PPX_SPY) ||
				(config.Progression() == PROGRESS_PPX_SNY))
			{
				area1 = pow(m_recentpart.Dx(), AXIS_STRETCH) + m_recentpart.Dy();
				area2 = pow(total_mer.Dx(), AXIS_STRETCH) + total_mer.Dy();

				size_fail = EQUAL(m_recentpart.Dx(), total_mer.Dx());
				shift_fail = bump_mer.Ymin() >= m_recentpart.Ymax();
			}
			else
			{
				area1 = m_recentpart.Dx() + pow(m_recentpart.Dy(), AXIS_STRETCH);
				area2 = total_mer.Dx() + pow(total_mer.Dy(), AXIS_STRETCH);

				size_fail = EQUAL(m_recentpart.Dy(), total_mer.Dy());
				shift_fail = bump_mer.Xmin() >= m_recentpart.Xmax();
			}

			if (size_fail || shift_fail)
				return 0.;

			double score = area1 / area2;
#if 0
			if ( EWMNestingAllow() )
			{
				note.Format( "* area score at %f, %f gets %f (recent %f / adjusted %f)", 
						place.X(), place.Y(), score, area1, area2 );
				EWMNesting( (LPCSTR) note );
			}
#endif

			return score;
		}
	}
	// ---------------------------------------------

	// NOTE: Using the intermediates 'candidate_polys' and 'sheet_polys'
	// makes for minor code-bloat but also makes debugging easier.
	CGeoPolyArray* candidate_polys;
	CGeoPolyArray* sheet_polys;

	double trap_x = 0.0;
	double trap_y = 0.0;

#if BEFORE_2008_03_22
	double factor = (config.PreNest() ? 0. : BUMP_FACTOR);
#else
	double factor = (config.PreNest() ? 0. : 0.5);
#endif

	C2dBox bump_mer( 
					outline_mer.Xmin() + place.X() - (factor *outline_mer.Dx()),
					outline_mer.Ymin() + place.Y(),
					outline_mer.Xmax() + place.X(),
					outline_mer.Ymax() + place.Y() );

	sheet_polys = m_toolhit->PolysGet(HIT_NEST_INSIDE);
	candidate_polys = candidate.PolysGet(HIT_PART_OUTSIDE);
	double trapx1 = do_process( BUMP_X, SPROC_SCORE, place,
		sheet_polys, candidate_polys, true, sign_x, bump_mer );

	sheet_polys = m_toolhit->PolysGet(HIT_PART_INSIDE);
	candidate_polys = candidate.PolysGet(HIT_PART_OUTSIDE);
	double trapx2 = do_process( BUMP_X, SPROC_SCORE, place,
		sheet_polys, candidate_polys, false, sign_x, bump_mer );

	if ( config.NestPosY() )
	{
		// Look above the part.
		bump_mer.Update( outline_mer.Xmin() + place.X(),
						outline_mer.Ymin() + place.Y(),
						outline_mer.Xmax() + place.X(),
						outline_mer.Ymax() + place.Y() + (factor * outline_mer.Dy()) );
	}
	else
	{
		// Look beneath the part.
		bump_mer.Update( outline_mer.Xmin() + place.X(),
						outline_mer.Ymin() + place.Y() - (factor * outline_mer.Dy()),
						outline_mer.Xmax() + place.X(),
						outline_mer.Ymax() + place.Y() );
	}

	sheet_polys = m_toolhit->PolysGet(HIT_NEST_INSIDE);
	candidate_polys = candidate.PolysGet(HIT_PART_OUTSIDE);
	double trapy1 = do_process( BUMP_Y, SPROC_SCORE, place,
		sheet_polys, candidate_polys, true, sign_y, bump_mer );

	sheet_polys = m_toolhit->PolysGet(HIT_PART_INSIDE);
	candidate_polys = candidate.PolysGet(HIT_PART_OUTSIDE);
	double trapy2 = do_process( BUMP_Y, SPROC_SCORE, place,
		sheet_polys, candidate_polys, false, sign_y, bump_mer );

	trap_x = min(trapx1, trapx2);
	trap_y = min(trapy1, trapy2);

	double trap = (trap_x*config.HFactor() + trap_y*config.VFactor()) / (config.HFactor() + config.VFactor());

	double area = CGeoPoly::Area(outside_poly);
	if (area < CLOSED_ENOUGH)
	{
		outside_poly = candidate.PolysGet(HIT_NEST_OUTSIDE);
		area = fabs((*outside_poly)[0]->Area());
	}

	double score;
	if (area > trap)
		score = (1.0 - (trap / area)) * 0.5 + 0.5;
	else
		score = (area / trap ) * 0.5;

	if ( ((trap_x / trap_y) > 4.0) || ((trap_y / trap_x) > 4.0) )
		config.BadRatio( true );
	else
		config.BadRatio( false );

	return score;
}

// Sort low to high
int CSheet::compare_double( const void* u, const void* v )
{
	double du = (*(double*)u);
	double dv = (*(double*)v);

	return ((du <= dv) ? -1 : 1);
}

// ==================================================================
//		do_collect_geo_xy
//		do_collect_geo_box
//
//	slave to do_process, collects all of the geometry in the given
//	poly array into a special benign curve list, for later processing.
//	ONLY takes curves that fit in the zone and have the correct winding.
//	Massive filtering.
//
bool CSheet::do_collect_geo_xy( 
	int						prim_axis,
	CGeoCurveArray*			dst_list,
	const CGeoPolyArray&	src_array,
	const C3dCoord&			offset,
	bool					reverse,
	const C2dBox			bump_zone ) const
{
	int sec_axis = FLIP_XY(prim_axis);

	if (!dst_list)
		return false;

	C2dBox poly_shift;
	C2dBox crv_shift;

	int poly_num = src_array.Count();
	for (int poly_idx=0; poly_idx<poly_num; poly_idx++)
	{
		CGeoPoly* poly = src_array[poly_idx];

		if ( m_ignore_lead )
		{
			if (poly->HasAttrib() && (poly->IntGet( "lead", 0 ) != 0))
				continue;
		}

		const C2dBox& poly_extent = poly->Extent();
		poly_shift.Update( poly_extent.Xmin() + offset.X(),
						   poly_extent.Ymin() + offset.Y(),
						   poly_extent.Xmax() + offset.X(),
						   poly_extent.Ymax() + offset.Y() );
		if (!poly_shift.Intersects( bump_zone, SMALL ))
			continue;

		int crv_num = poly->Count();
		for (int crv_idx=0; crv_idx<crv_num; crv_idx++)
		{
			const CGeoCurve& curve = (*poly)[crv_idx];

			const C3dCoord& ps = curve.StartPt();
			const C3dCoord& pe = curve.EndPt();

			double start = ps[sec_axis];
			double end = pe[sec_axis];

			bool skip = false;

			if (m_fine_tuning && curve.IntGet( STR_LEAD_HULL, 0 ) != 0)
				skip = true;

			// 2008.01.19 (PE) -- We want to avoid filtering out
			// arcs because pierce holes (in particular) get
			// overlooked, causing the HIT_NEST_OUTSIDE of the
			// part to overlap the pierce hole. One would think
			// this is not a big deal, but according to Sunflower
			// it destroys the integrity of the web.
			if (!skip && curve.Type() != GEOARC)
			{
				// NOTE: If we ever change this, be sure to also
				// change the reversal in FillBarrier()
				if (prim_axis == ORD_X)
				{
					// ie. We're nesting along the X-axis.
					skip = (reverse ^ (end < start));
				}
				else
				{
					// ie. We're nesting along the Y-axis.
					skip = (reverse ^ (end > start));
				}
			}

			if ( !skip )
			{
				// We want to avoid vertical lines when nesting along
				// the X-axis. Likewise, we want to avoid horizontal
				// lines when nesting along the Y-axis.
				skip = EQUAL( end, start );
			}

			if ( !skip )
			{
				const C2dBox& crv_extent = curve.Box();
				crv_shift.Update( crv_extent.Xmin() + offset.X(),
								  crv_extent.Ymin() + offset.Y(),
								  crv_extent.Xmax() + offset.X(),
								  crv_extent.Ymax() + offset.Y() );

				if (!crv_shift.Intersects( bump_zone, SMALL ))
					continue;

				dst_list->Append( (CGeoCurve*)&curve );
			}
		}
	}

	if ( DBG5 )
		dst_list->Draw();

	return (dst_list->Count() > 0);
}


// ==================================================================
bool CSheet::do_collect_geo_box( 
	CGeoCurveArray*			dst_list, 
	const CGeoPolyArray&	src_array, 
	const C3dCoord&			offset, 
	const C2dBox			bump_zone ) const
{
	if (!dst_list)
		return false;

	C2dBox poly_shift;
	C2dBox crv_shift;

	int poly_num = src_array.Count();
	for (int poly_idx=0; poly_idx<poly_num; poly_idx++)
	{
		CGeoPoly* poly = src_array[poly_idx];

		const C2dBox& poly_extent = poly->Extent();
		poly_shift.Update( poly_extent.Xmin() + offset.X(),
						   poly_extent.Ymin() + offset.Y(),
						   poly_extent.Xmax() + offset.X(),
						   poly_extent.Ymax() + offset.Y() );
		if (!poly_shift.Intersects( bump_zone, SMALL ))
			continue;

		int crv_num = poly->Count();
		for (int crv_idx=0; crv_idx<crv_num; crv_idx++)
		{
			const CGeoCurve& curve = (*poly)[crv_idx];

			const C2dBox& crv_extent = curve.Box();
			crv_shift.Update( crv_extent.Xmin() + offset.X(),
							  crv_extent.Ymin() + offset.Y(),
							  crv_extent.Xmax() + offset.X(),
							  crv_extent.Ymax() + offset.Y() );

			if (!crv_shift.Intersects( bump_zone, SMALL ))
				continue;

			dst_list->Append( (CGeoCurve*)&curve );
		}
	}

	return (dst_list->Count() > 0);
}

// ==================================================================
bool CSheet::ent_intersect( 
	const CGeoCurve& sheet_geo,
	const CGeoCurve& part_geo,
	const C3dCoord&	part_shift ) const
{
	CInt2d int2d;
	CGeoCurve* shift_geo;
	
	shift_geo = (CGeoCurve*)part_geo.Clone( false );
	shift_geo->Shift( C3dVec( part_shift.X(), part_shift.Y(), 0.0 ) );

	// 2005.12.30 (PE) -- Whereas a debug version of the product yeilded
	// a nest, the release version did not because of some difference in
	// floating point calculations.  Loosening the measure of "parallel"
	// resolved the issue.  Prior to this change, the tolerance was
	// defaulted (internal to CInt2d) as VECTOR_SMALL (ie. 1.e-12).
	int2d.ParallelTol( 1.e-9 );

	int2d.CrvCrv( sheet_geo, *shift_geo );
	delete shift_geo;

	if (int2d.Tangent())
		return false;

	int num = int2d.Count();
	for (int idx=0; idx<num; idx++)
	{
		double vparam = int2d.Vparam(idx);
		double uparam = int2d.Uparam(idx);

#if 1
		// Edwin's original test.
		if ( !ZERO(vparam)
			&& !EQUAL(vparam, 1.0)
			&& !ZERO(uparam)
			&& !EQUAL(uparam, 1.0) )
				return true;
#else
		// BugID: 779 -- overlapping parts.
		// 2004.04.18 (PE) -- Introduced whilst floundering around
		// trying to prevent nesting from overlapping parts.  There
		// remains doubts as to the correctness of the original test,
		// but it appears to do the trick.  I only tried this change
		// because it temporarily fixed the error ... but caused parts
		// to drift in some cases.
		if ( ((uparam >= 0.001) && (uparam <= (1.-0.001))) ||
			 ((vparam >= 0.001) && (vparam <= (1.-0.001))) )
				return true;
#endif
	}

	return false;
}


// ==================================================================

double CSheet::ent_dist_xy( 
	int prim_axis,
	const CGeoCurve& sheet_geo,
	const CGeoCurve& part_geo,
	double	min_ord,
	double	max_ord,
	double	sign,
	bool*	parallel ) const
{
	int sec_axis = FLIP_XY(prim_axis);

	*parallel = false;

	if (0)
	{
		m_view_mgr->ActiveView()->Clear( PTEMP_LIST );
		m_view_mgr->ActiveView()->DrawCurve( (CGeoCurve*) &sheet_geo , TRUE );
		m_view_mgr->ActiveView()->DrawCurve( (CGeoCurve*) &part_geo , TRUE );
	}

	// Test only the span where the entities overlap
	C2dBox from_ext = sheet_geo.Box();
	C2dBox to_ext = part_geo.Box();

	double tmp = max( from_ext.BL()[sec_axis], to_ext.BL()[sec_axis] );
	min_ord = max( min_ord, tmp  );

	tmp = min( from_ext.TR()[sec_axis], to_ext.TR()[sec_axis] );
	max_ord = min( max_ord, tmp );

	double dist = distance_xy( prim_axis, part_geo, sheet_geo, min_ord, max_ord, sign );

	return dist;
}

// ============================================================================

double CSheet::ent_area_xy( 
	int prim_axis,
	const CGeoCurve& sheet_geo,
	const CGeoCurve& part_geo,
	double min_ord,
	double max_ord ) const
{
	// Sadly, the area methods are not easy converts to index ordinates
	//
	if (prim_axis == ORD_X)
	{ return ent_area_x( sheet_geo, part_geo, min_ord, max_ord ); }
	else // ORD_Y
	{ return ent_area_y( sheet_geo, part_geo, min_ord, max_ord ); }
}

// -----------------------------------------------

double CSheet::ent_area_x( 
	const CGeoCurve& sheet_geo,
	const CGeoCurve& part_geo,
	double Yn,
	double Yx ) const
{
	// Move to math notation, easier to read.
	//
	//	P is Part, S is Sheet, Y is y boundary
	//	n is min, x is max
	//
	//
	// Test only the span where the entities overlap
	//
	C2dBox from_ext = sheet_geo.Box();
	C2dBox to_ext = part_geo.Box();

	double tmp = max( from_ext.Ymin(), to_ext.Ymin() );
	Yn = max( Yn, tmp );

	tmp = min( from_ext.Ymax(), to_ext.Ymax() );
	Yx = min( Yx, tmp );

	if (EQUAL( Yn, Yx ))
	{
		return 0.0;	// Return error if we are a point-hit
	}
	//
	// Now take the various areas at the overlap
	//	This is a bit of a fake... but whatever
	//
	double Pn = part_geo.InterceptX( Yn );
	double Px = part_geo.InterceptX( Yx );

	double Sn = sheet_geo.InterceptX( Yn );
	double Sx = sheet_geo.InterceptX( Yx );

	//
	// Area calculations perverted from CGeoPoly::sub_area()
	//
	// PnYn to PxYx
	double area = Pn*Yx - Px*Yn;
	if (part_geo.Type() == GEOARC)
	{
		CGeoArc* arc = (CGeoArc*)(&part_geo);
		double angle;
		int dir;
		if (arc->StartPt().Y() > arc->EndPt().Y())
		{ 
			angle = arc->IncludedAngle( Px, Yx, Pn, Yn );
			dir = -arc->Dir();
		}
		else
		{ 
			angle = arc->IncludedAngle( Pn, Yn, Px, Yx );
			dir = arc->Dir(); 
		}
		double arcarea = arc->Radius() * arc->Radius() * (angle - sin(angle));
		area += dir * arcarea;
	}
	double tot_area = area;

	// PxYx to SxYx
	area = Px*Yx - Sx*Yx;
	tot_area += area;

	// SxYx to SnYn
	area = Sx*Yn - Sn*Yx;
	if (sheet_geo.Type() == GEOARC)
	{
		const CGeoArc* arc = (CGeoArc*)(&sheet_geo);
		double angle;
		int dir;
		if (arc->StartPt().Y() > arc->EndPt().Y())
		{ 
			angle = arc->IncludedAngle( Sx, Yx, Sn, Yn );
			dir = arc->Dir();
		}
		else
		{ 
			angle = arc->IncludedAngle( Sn, Yn, Sx, Yx );
			dir = -arc->Dir();
		}
		double arcarea = arc->Radius() * arc->Radius() * (angle - sin(angle));
		area += dir * arcarea;
	}
	tot_area += area;

	// SnYn to PnYn
	area = Sn*Yn - Pn*Yn;
	tot_area += area;

	tot_area = fabs(tot_area) * 0.5;

	if (tot_area < 0.0)
		tot_area = 0.0;

	return tot_area;
}

// -----------------------------------------------

double CSheet::ent_area_y( 
	const CGeoCurve& sheet_geo,
	const CGeoCurve& part_geo,
	double Xn,
	double Xx ) const
{
	// Move to math notation, easier to read.
	//
	//	P is Part, S is Sheet, X is x-axis boundary
	//	n is min, x is max
	//
	//
	// Test only the span where the entities overlap
	//
	C2dBox from_ext = sheet_geo.Box();
	C2dBox to_ext = part_geo.Box();

	double tmp = max( from_ext.Xmin(), to_ext.Xmin() );
	Xn = max( Xn, tmp );

	tmp = min( from_ext.Xmax(), to_ext.Xmax() );
	Xx = min( Xx, tmp );

	if (EQUAL( Xn, Xx ))
	{
		return 0.0;	// Return error if we are a point-hit
	}
	//
	// Now take the various areas at the overlap
	//	This is a bit of a fake... but whatever
	//
	double Pn = part_geo.InterceptY( Xn );
	double Px = part_geo.InterceptY( Xx );

	double Sn = sheet_geo.InterceptY( Xn );
	double Sx = sheet_geo.InterceptY( Xx );

	//
	// Area calculations perverted from CGeoPoly::sub_area()
	//
	// XnPn to XxPx
	double area = Xn*Px - Xx*Pn;
	if (part_geo.Type() == GEOARC)
	{
		CGeoArc* arc = (CGeoArc*)(&part_geo);
		double angle;
		int dir;
		if (arc->StartPt().X() > arc->EndPt().X())
		{ 
			angle = arc->IncludedAngle( Xx, Px, Xn, Pn );
			dir = -arc->Dir();
		}
		else
		{ 
			angle = arc->IncludedAngle( Xn, Pn, Xx, Px );
			dir = arc->Dir();
		}
		if (EQUAL(angle, TWOPI))
		{ angle = 0.0; }

		double arcarea = arc->Radius() * arc->Radius() * (angle - sin(angle));
		area += dir * arcarea;
	}
	double tot_area = area;

	// XxPx to XxSx
	area = Xx*Sx - Xx*Px;
	tot_area += area;

	// XxSx to XnSn
	area = Xx*Sn - Xn*Sx;
	if (sheet_geo.Type() == GEOARC)
	{
		const CGeoArc* arc = (CGeoArc*)(&sheet_geo);
		double angle;
		int dir;
		if (arc->StartPt().X() > arc->EndPt().X())
		{ 
			angle = arc->IncludedAngle( Xx, Sx, Xn, Sn );
			dir = arc->Dir();
		}
		else
		{ 
			angle = arc->IncludedAngle( Xn, Sn, Xx, Sx );
			dir = -arc->Dir();
		}
		double arcarea = arc->Radius() * arc->Radius() * (angle - sin(angle));
		area += dir * arcarea;
	}
	tot_area += area;

	// XnSn to XnPn
	area = Xn*Pn - Xn*Sn;
	tot_area += area;

	tot_area = fabs(tot_area) * 0.5;

	if (tot_area < 0.0)
		tot_area = 0.0;

	return tot_area;
}

// ============================================================================

CReturn CSheet::Save(
	const CString&		in_result,
	const CNestConfig&	config,
	int					in_num )
{
	CReturn		ret;
	CMM2		file;
	CString		vector;
	CDaoDB		part_db;
	int			mat_tab;

	// Update the material quantity as specified...
	if (IsRemnant())
	{
		mat_tab = part_db.addTable( "Remnant Inventory" );
		ret += part_db.Open( config.PdbPath() );
		if (ret.isOkay())
		{
			// Save adjusted quantity
			if (part_db.findRecord( mat_tab, "ID", m_remnkey ) >= 0)
			{
				part_db.editRecord( mat_tab );
				part_db.setInt( mat_tab, "Quantity", m_quantity );
				part_db.commitRecord( mat_tab );
			}

			part_db.Close();
		}
	}
	else	// IsStock
	{
		mat_tab = part_db.addTable( "Material Inventory" );
		ret += part_db.Open( config.PdbPath() );
		if (ret.isOkay())
		{
			// Save adjusted quantity
			if (part_db.findRecord( mat_tab, "ID", m_matkey ) >= 0)
			{
				part_db.editRecord( mat_tab );
				part_db.setInt( mat_tab, "Quantity", m_quantity );
				part_db.commitRecord( mat_tab );
			}
			part_db.Close();
		}
	}

	// Save the results of this soggy sheet...
	vector.Format( "%%s\\%%s%%%02dd.nst", config.ResultDigits() );
	m_pathname.Format( vector,
					config.ResultPath(),
					in_result,
					in_num );

	m_model->pHeader()->setString( "ProjNum", m_pathname );
	m_model->pHeader()->setString( "MatCfg", m_descrip );

	// 2008.03.05 (PE) -- The gap tolerance is used by code generation
	// to identify profiles that are 'logically closed'. In turn, this
	// helps avoid containment parity issues.
	m_model->pHeader()->setReal( "code_gap", config.GapTolerance() );

	LeadHullsDelete();

	return file.Write( m_pathname, *m_model );
}

CReturn CSheet::MaterialCreate( double dx, double dy, double z, CModel* model )
{
	CReturn			status;
	CGeoLine		geo_line;
	CDbTool*		dbTool;
	CDbWorkplane*	dbWork;
	CDbLine*		dbLine;

	dbTool = model->ActiveTool();
	dbWork = model->ActiveWorkplane();

	model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );
	geo_line.StartPt( C3dCoord( 0., dy, z ) );
	geo_line.EndPt( C3dCoord( dx, dy, z ) );
	dbLine->Init( dbTool, dbWork, geo_line );
	dbLine->SystemFlag( true );

	model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );
	geo_line.StartPt( C3dCoord( dx, dy, z ) );
	geo_line.EndPt( C3dCoord( dx, 0., z ) );
	dbLine->Init( dbTool, dbWork, geo_line );
	dbLine->SystemFlag( true );

	model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );
	geo_line.StartPt( C3dCoord( dx, 0., z ) );
	geo_line.EndPt( C3dCoord( 0., 0., z ) );
	dbLine->Init( dbTool, dbWork, geo_line );
	dbLine->SystemFlag( true );

	model->EntityCreate( DBLINE, (CDbEntity**) &dbLine );
	geo_line.StartPt( C3dCoord( 0., 0., z ) );
	geo_line.EndPt( C3dCoord( 0., dy, z ) );
	dbLine->Init( dbTool, dbWork, geo_line );
	dbLine->SystemFlag( true );

	return status;
}

// =======================================================================
CString CSheet::Name( void ) const
{
	CFileSnoop	snoop;

	return snoop.getFilename( m_pathname );
}

double CSheet::Area( void ) const
{
	if (IsRemnant())
	{
		CDbIterator iter;
		CProfile	profile;
		double		area;

		iter.Init( m_model->Db(), DBPROFILE );
		while ( true )
		{
			CDbProfile* db_prof = dynamic_cast<CDbProfile*>( iter() );
			if (!db_prof)
				break;
			iter.Next();

			for (int idx=0; idx<db_prof->Count(); idx++)
			{
				CDbEntity* db_ent = (*db_prof)[idx];

				CGeoCurve* curve = ((CDbCurve*)db_ent)->Curve();
				profile.Append( curve );
			}

			area = fabs(profile.Area());

			profile.DestructiveFlush();

			break;
		}

		return area;
	}

	return Length() * Width();
}

// =======================================================================
//	Put bunches of labels on the sheet, to reflect what we just did
CReturn CSheet::Label( const CPartBin& partbin )
{
	CReturn	ret;

	CDbWorkplane* work = NULL;
	m_model->EntityFind( STR_WORLD, (CDbEntity**)&work, DBWORKPLANE, DBWORKPLANE );

	CDbTool* tool = NULL;
	m_model->EntityFind( "Default", (CDbEntity**)&tool, DBTOOL, DBTOOL );
	if (tool == NULL)
	{
		m_model->EntityCreate( DBTOOL, (CDbEntity**)&tool );
		tool->ColorSet( DCOLOR_YELLOW );
		tool->Name( "Default" );
	}

	//
	// See how many diverse parts were punched..
	//
	int idx, partnum = partbin.Count();
	int hits = 0;
	for (idx=0; idx<partnum; idx++)
	{
		CNestingPart* npart = partbin.GetAt(idx);
		if (npart->Used() > 0)
			hits++;
	}

	//
	// Where to start our BOM
	//
	C3dCoord corner( m_extent.Dx(), m_extent.Dy(), 0.0 );

	//
	// The labels beginneth...
	//
	CString block;
	CString	text;

	if (m_repeat > 0)
	{
		text.Format( " Cut %d Sheets\n", m_repeat+1 );
		block += text;
	}

	if (m_cutback > 0)
	{
		text.Format( " Cutback to:  %4.2f x %4.2f\n", m_cutback, m_extent.Dy() );
		block += text;
	}

	text.Format( " PARTS AND QTY\n" );
	block += text;

	for (idx=0; idx<partnum; idx++)
	{
		CNestingPart* npart = partbin.GetAt(idx);

		if (npart->Used() > 0)
		{
			if (npart->Quantity() < 0)
			{
				text.Format( " %d. %s - %d (fill)\n",
					npart->SortedID(), npart->Name(), npart->Used() );
			}
			else
			{
				text.Format( " %d. %s - %d of %d\n",
					npart->SortedID(), npart->Name(), npart->Used(), npart->TotalQuantity() );
			}
			block += text;
		}
	}

	print( work, tool, block, &corner );
	return ret;
}




// =======================================================================
// Utility function to take care of the details of creating a "note"
//	on the sheet and update the line position
void CSheet::print( 
	CDbWorkplane*	work,
	CDbTool*		tool,
	const CString&	text,
	C3dCoord*		at )
{
	CDbCommand*	cmd;
	m_model->EntityCreate( DBCOMMAND, (CDbEntity**)&cmd );

	cmd->Init( tool, work, *at, text );

//	at->Y( at->Y() - m_textheight );
}


// =======================================================================
//	Find all closed, tooled profiles and place a Drop or Stop command after them in the
//	containing feature.
//
//	Drop is issued when the profile's MER has one dimensions greater than min_trap
//	and all dimensions less than max_trap.  Otherwise, it will place a Stop.
//
//	1) determine if the profile is a punch or burn
//	2a) if it is larger than the max trap in either dimension, do a stop
//	2b) else if it is larger than the min trap in either dimension, do a drop
//	2c) otherwise, it's a speck and can be ignored.
//	
//	3) The drop extend depends on whether we are in punch or burn
//	4) the part is centered in the drop door, incl. the trap origin
//
CReturn CSheet::DropStop( const CNestConfig& config )
{
	CReturn ret;
	CDbIterator iter;

	iter.Init(  m_model->Db(), DBPROFILE );
	while ( true )
	{
		CDbProfile* db_prof = dynamic_cast<CDbProfile*>( iter() );
		if (db_prof == NULL)
			break;  // exhausted the profiles
		iter.Next();

		if (!db_prof->IsToolpath() || db_prof->IsLeadHull() || (db_prof->Count() == 0))
			continue;  // nothing to do

		CDbTool* tool = db_prof->Tool();

		if ( !db_prof->IsClosed() )
		{
			CDbCurve* dbCurve = dynamic_cast<CDbCurve*>( db_prof->GetAt(0) );
			if ((dbCurve != NULL) && (dbCurve->IntGet( "_split", 0 ) != 0))
				continue;  // avoid putting drop/stop at split point

			if ((db_prof->IntGet("_is_overlapped",0) == 0) &&
				(db_prof->IntGet("winding",0) == 0))
					continue;
		}


		int dropskip = db_prof->IntGet( "dropskip", 0 );
		if (dropskip)
			continue;

		C3dBox mer = db_prof->Box(0);
		CDbCurve*	db_end = (CDbCurve*)(*db_prof)[db_prof->Count()-1];

		C2dBox	min_door(0,0,0,0);
		C2dBox	max_door(0,0,0,0);
		double x_origin = 0.0;
		double y_origin = 0.0;
		if ( tool 
			&& tool->IsPunchTool())
		{
			max_door.Xmax( config.MachineReal( "Punch_Dropdoor_Max_Length", 0.0 ) );
			max_door.Ymax( config.MachineReal( "Punch_Dropdoor_Max_Width", 0.0 ) );
			min_door.Xmax( config.MachineReal( "Punch_Dropdoor_Min_Length", 0.0 ) );
			min_door.Ymax( config.MachineReal( "Punch_Dropdoor_Min_Width", 0.0 ) );

			x_origin = config.MachineReal( "Punch_Dropdoor_Location_X", 0.0 );
			y_origin = config.MachineReal( "Punch_Dropdoor_Location_Y", 0.0 );
		}
		else // If not punch, then profile; drills don't get here
		{
			max_door.Xmax( config.MachineReal( "Torch_Dropdoor_Max_Length", 0.0 ) );
			max_door.Ymax( config.MachineReal( "Torch_Dropdoor_Max_Width", 0.0 ) );
			min_door.Xmax( config.MachineReal( "Torch_Dropdoor_Min_Length", 0.0 ) );
			min_door.Ymax( config.MachineReal( "Torch_Dropdoor_Min_Width", 0.0 ) );

			x_origin = config.MachineReal( "Torch_Dropdoor_Location_X", 0.0 );
			y_origin = config.MachineReal( "Torch_Dropdoor_Location_Y", 0.0 );
		}

		CString	cmd_str;

		if ( (mer.Dx() > max_door.Dx())
			|| (mer.Dy() > max_door.Dy()) )
		{
			// Too big; do a stop
			cmd_str.Format( "@STOP: dx=%f, dy=%f", mer.Dx(), mer.Dy() );

		}
		else
		if ( (mer.Dx() > min_door.Dx())
			|| (mer.Dy() > min_door.Dy()) )
		{
			// Not too small; Trap
			// Position the Trap so the part is in the center of the door,
			// including the door's offset from the cutting tool
			double x_gap = (mer.Dx() - max_door.Dx()) / 2.0;
			double y_gap = (mer.Dy() - max_door.Dy()) / 2.0;

			C3dCoord pt;//( db_end->EndPt(0) );
			pt.X( mer.Xmin() - x_gap - x_origin );
			pt.Y( mer.Ymin() - y_gap - y_origin );

			cmd_str.Format( "@DROP: x=%f, y=%f, dx=%f, dy=%f",
							pt.X(), pt.Y(), mer.Dx(), mer.Dy() );
		}

		if (cmd_str.GetLength() > 2)
			db_prof->StringSet( "@DROPSTOP", cmd_str );
		else
			db_prof->AttribDelete( "@DROPSTOP" );
	}

	return ret;
}

// ==================================================================
//	Old-form scan; copy across the grid from the part
//	INCLUDES PIERCE HOLE -- 
CReturn CSheet::Scan( 
	const CNestConfig&		config,
	const CDexGrid&			target,
	const C3dCoord&			place,
	CViewMgr&				view )
{
	CReturn	ret;

	pGrid()->Punch( config, target, place );
	//
	// Filter our results
	//
	if (config.Filter())
	{
		// Okay, let's do the filtering of small junk as a post
		// loop.
		C2dCoord extent( place.X() + target.Width(),
						place.Y() + target.Height() );

		pGrid()->Filter( config, place, extent, config.Narrow( config.Pass() ), view );
	}


	if ( config.DebugBmp() )
	{
		// Draw scanned and filtered sheet
		pGrid()->Draw( view, 0, 0, false, true );
	}

	return ret;
}

// ==================================================================
//		SeedPoly
//
//	There are 8 possible environments where we would seed:
//
//	X-axis or Y-axis primary fill directions
//	times four gravities; x+y+, x+y-, x-y+, x-y-
//
//	There are in fact four actually used:
//		X axis x-y+
//		X axis x-y-
//		Y axis x-y+
//		Y axis x-y-
//
//	There are five curve conditions where we seed in these environments:
//		Line - Horizontal
//		Line - Vertical
//		Line - at Angle
//		Arc - Convex
//		Arc - Concave
//
//	Material is always to the left of the curve, kerf is always to the right of the curve.
//
//	Cornering information is stored in Z.  Z+ Top-Left, Z- means Bottom Left
//
// TODO:  Seed the poly into a temp array, sort this, then transfer to the pre/post arrays
//
const int SEED_SIDE = -1;

int CSheet::SeedPoly( 
	const CNestConfig&	config,
	CGeoPoly*			poly,
	double				flip,  // If in hole (e.g. seed side flips), set to -1, otherwise +1
	bool				box_seed )
{
	bool fill_seed = config.FillSeed();
	if ( config.PreNest() )  // || !restrict ) ... which was *always* false.
	{
		// restrict = false;
		((CNestConfig&) config).FillSeed( true );
	}

	ConfigSet( &config );

	int seed_count = do_seed( PRIMARY_BANK, SECONDARY_BANK, poly, NULL, box_seed, flip );

	ConfigSet( NULL );

	((CNestConfig&) config).FillSeed( fill_seed );

	return seed_count;
}

int CSheet::SeedBarrier(
	CNestConfig& config, 
	CGeoPoly* poly, 
	double flip )	// If in hole (e.g. seed side flips), set to -1, otherwise +1
{
	bool	fillseed;
	int		seed_count;
	
	fillseed = config.FillSeed();
	config.FillSeed(true);

	ConfigSet( &config );

	seed_count = do_seed( SECONDARY_BANK, PRIMARY_BANK, poly, NULL, false, flip );

	ConfigSet( NULL );

	config.FillSeed( fillseed );

	return seed_count;
}


int CSheet::SeedPartEdge( CNestConfig& config )
{
	C2dBox box = BoxUser();
	//
	// Somewhat duplicated from so_seed
	//
	double step = config.Narrow( config.Pass() ) * INFLUENCE_FACTOR;
	int corner=0;
	if ((config.Progression() == PROGRESS_PPX_SPY) ||
		(config.Progression() == PROGRESS_PPX_SNY))
	{ corner = config.NestPosY()?-1:1; }
	else // FILL_Y
	{ corner = 0; }

	return do_seed_vstrip( SECONDARY_BANK,
		step, corner, box.Ymin(), box.Ymax(), box.Xmax());
}

int CSheet::do_seed(
	eSeedBank	primary_bank,
	eSeedBank	secondary_bank,
	CGeoPoly*	poly,
	C2dCoord*	center,
	bool		box_seed,
	double		flip )
{
	int	seed_count = 0;

	if ( poly )  // Should always have a poly but ....
	{
		eNestProgression progression = m_config->Progression();

		if (box_seed)
		{
			// High-priority seeds on box corners. NOTE: This should
			// only be done when seeding the outside of a part.
			const C2dBox& box = poly->Extent();
			SeedsDerive( box, primary_bank, secondary_bank );
		}

		if ( !m_config->GridSquare() ||
			  m_config->FillSeed() ||
			  (IsRemnant() && !m_config->PreNest()) )
		{
			// Add seeds based on the configuration and the curves in this poly
			seed_count = do_seed_poly(
				poly, center, flip, primary_bank, secondary_bank );
		}

		m_seed_mgr->SeedsSort( progression, this );

		if (CReturn::Debug() >= 3)
			SeedsDisplay();
	}

	return seed_count;
}

int CSheet::do_seed_poly(
	CGeoPoly*	poly,
	C2dCoord*	center,
	double		flip,
	eSeedBank	primary_bank,
	eSeedBank	secondary_bank )
{
	C2dUnitVec	primary;
	C2dUnitVec	secondary;
	int			seed_count;
	int			count, indx;
	
	seed_count = 0;

	// 2007.08.30 (PE) -- The initialization of these vectors used to
	// be more complicated, having taken the progression direction into
	// consideration. Said implementation was re-examined after receiving
	// a report from Timpte Trailers that nesting would fail when the
	// progression direction was set to Along-X. Where normally this
	// solution *appeared* to work, Timpte's nesting configuration (a
	// narrow sheet that allowed parts to be nested only between clamps)
	// revealed the flaw. In the final analysis, it appears the vectors
	// are not related to the progression direction. Instead, the are
	// used to filter out entities that should *not* be seeded. This
	// *filter* is based upon the unstated assumption that a given curve
	// is a member of a profile that has a corrected winding direction.
	primary.Init( 0., m_config->NestPosY() ? -1. : 1. );
	secondary.Init( -1., 0. );

	// Add seeds based on the configuration and the curves in this poly
	count = poly->Count();
	for (indx = 0; indx < count; ++indx)
	{
		const CGeoCurve& curve = (*poly)[indx];

		if (curve.HasAttrib() && (curve.IntGet( "lead", 0 ) != 0))
			continue;

		if (curve.Type() == GEOLINE)
		{
			const CGeoLine& line = *(CGeoLine*)(&curve);
			if (EQUAL( line.StartPt().Y(), line.EndPt().Y() ))
			{ 
				seed_count += do_seed_hline( primary_bank, secondary_bank,
					line, primary, secondary, center, flip );
			}
			else
			if (EQUAL( line.StartPt().X(), line.EndPt().X() ))
			{ 
				seed_count += do_seed_vline( primary_bank, secondary_bank,
					line, primary, secondary, center, flip );
			}
			else
			{ 
				seed_count += do_seed_decompose( primary_bank, secondary_bank,
					line, primary, secondary, center, flip );
			}
		}
		else // GEOARC
		{
			seed_count += do_seed_decompose( primary_bank, secondary_bank,
				curve, primary, secondary, center, flip );
		}
	}

	return seed_count;
}

// ----------------------------------------------------------------------------

void CSheet::seed_it( eSeedBank which_bank, double x, double y, int corner )
{
	C3dCoord world( x, y, corner );

	if ( CNestConfig::IsBitmapNest() )  // DYNATORCH
	{
		CSeed seed( 1, world, NULL, 0 );
		m_seed_mgr->Deposit( which_bank, seed );
	}
	else
	{

		// Just let 'er rip.
		CSeed seed( 1, world, NULL, 0 );
		m_seed_mgr->Deposit( which_bank, seed );
	}
}

// ----------------------------------------------------------------------------

// TODO: The room for a great deal of improvement here. Particularly
// if we take advantage of contextual information like curve orientation
// and which end (if any) we are at.
C2dCoord CSheet::seed_it_salvage( const C2dCoord& grid )
{
	C2dCoord	salvaged;
	int		interference;

	if (m_config == NULL)
	{
		// This branch exists because seed_it() can be called without
		// a config object being present. In fact, this is (essentially)
		// the original behavior; the other branch was introduced because
		// (what should have been) good seeds were being discarded.

		// Step to the right one, see if we can get out of the dead zone
		salvaged.X( grid.X() + 1 );
		salvaged.Y( grid.Y() );
	}
	else
	{
		if ((m_config->Progression() == PROGRESS_PPY_SPX) ||
			(m_config->Progression() == PROGRESS_PNY_SPX))
		{
			salvaged.X( grid.X() );
			salvaged.Y( grid.Y() - 1 );

			interference = (pGrid()->Cell(salvaged) & (MARK_BODY | MARK_DEAD));
			if ( interference )
			{
				salvaged.Y( grid.Y() + 1 );
				interference = (pGrid()->Cell(salvaged) & (MARK_BODY | MARK_DEAD));
			}
		}
		else
		{
			salvaged.Y( grid.Y() );
			salvaged.X( grid.X() + 1 );

			interference = (pGrid()->Cell(salvaged) & (MARK_BODY | MARK_DEAD));
			if ( interference )
			{
				salvaged.X( grid.Y() - 1 );
				interference = (pGrid()->Cell(salvaged) & (MARK_BODY | MARK_DEAD));
			}
		}
	}

	return salvaged;  // candidate only
}

// ----------------------------------------------------------------------------

int CSheet::do_seed_hline( 
	eSeedBank			primary_bank,
	eSeedBank			secondary_bank,
	const CGeoLine&		raw_line, 
	const C2dUnitVec&	pvec, 
	const C2dUnitVec&	svec,
	const C2dCoord*		center,
	double				flip )
{
	CGeoLine	line;
	C3dCoord	ps;
	C3dCoord	pe;
	double		at_x, at_y;
	double		end_x;

	if (flip > SMALL)
	{
		ps = raw_line.StartPt();
		pe = raw_line.EndPt();
	}
	else
	{
		ps = raw_line.EndPt();
		pe = raw_line.StartPt();
	}

	line = CGeoLine( ps, pe );

	C2dUnitVec& tan = line.StartTan();
	double p_side = tan.PerpDot(pvec);
	if (EQUAL(p_side, 1))
		return 0;

	double s_side = tan.PerpDot(svec);
	if (EQUAL(s_side, 1))
		return 0;

	// Combinations:
	//	one of p or s will be zero in ortho tests (p+s+, p+s-, p-s+, p-s-)
	//	p0s0 is also not possible 
	//	p+ or s+ rejected (p0s+, p+s0)
	//
	// So, only have to test for p0s- and p-s0
	//
	// Intepretation of PerpDot
	//	0 means the vectors are parallel or anti-parallel
	//	+ means the vector points to the left of the line (cc turn)
	//	- means the vector points to the right of the line (cw turn)
	if (ZERO(p_side))
	{
		// p0s-
		// Single secondary seed at end pointed to by P
		//
		if (EQUAL( tan.X(), pvec.X() ))
		{
			at_x = pe.X();
			at_y = pe.Y();
		}
		else // opposite
		{
			at_x = ps.X();
			at_y = ps.Y();
		}

		seed_it( primary_bank, at_x, at_y, (int)svec.Y() );

		return 1;
	}
	else
	{
		// p-s0
		// Row of % spaced points along line
		//
		double	vx, vy, step;

		at_y = ps.Y();
		vy = pvec.Y();
		step = m_config->Narrow( m_config->Pass() ) * SEED_FACTOR;

		vx = svec.X() + pvec.X();

		if (tan.X() < -SMALL)
		{
			at_x = pe.X();
			end_x = ps.X();
		}
		else // opposite
		{
			at_x = ps.X();
			end_x = pe.X();
		}

		if ( center && (end_x > center->X()) )
			end_x = center->X();

		if ( CNestConfig::IsBitmapNest() )  // DYNATORCH
		{
			double tmp = at_y + m_config->Spacing() + m_config->Resolution();
			int count = (int) (tmp /  m_config->Resolution());
			if ((tmp - (count * tmp)) < 0.001)  // arbitrary
				tmp += m_config->Resolution();
			at_y = tmp;

			if ( flip )
				at_x += m_config->Spacing();  // ASSUMPTION: We're in a hole.
		}

		return do_seed_hstrip( primary_bank, step, (int)vy, at_x, end_x, at_y );
	}

	return 0;
}

// ----------------------------------------------------------------------------

int CSheet::do_seed_hstrip( 
	eSeedBank	which_bank,
	double		step, 
	int			corner, 
	double		from_x, 
	double		to_x, 
	double		at_y )
{
	double at_x;
	int dir;
	if (from_x < to_x)
	{ 
		at_x = from_x;
		if (at_x > to_x)
			return 0;

		to_x += SMALL;
		dir = 1;
	}
	else
	{ 
		at_x = from_x;
		if (at_x < to_x)
			return 0;

		to_x -= SMALL;
		dir = -1;
	}
	//
	int num = abs((int)floor( fabs(at_x-to_x)/step )) + 1;
	int count = num;
	step *= dir;
	while (num--)
	{
		seed_it( which_bank, at_x, at_y, corner );
		at_x += step;
	}

	return count;
}

// ----------------------------------------------------------------------------

int CSheet::do_seed_vline(
	eSeedBank			primary_bank,
	eSeedBank			secondary_bank,
	const CGeoLine&		raw_line, 
	const C2dUnitVec&	pvec, 
	const C2dUnitVec&	svec,
	const C2dCoord*		center,
	double				flip )
{
	CGeoLine	line;
	C3dCoord	ps;
	C3dCoord	pe;
	double		at_x, at_y;
	double		end_y;

	if (flip > SMALL)
	{
		ps = raw_line.StartPt();
		pe = raw_line.EndPt();
	}
	else
	{
		ps = raw_line.EndPt();
		pe = raw_line.StartPt();
	}

	line = CGeoLine( ps, pe );

	//
	// Rotated clone of do_seed_hline
	// TODO:  Merge?
	//
	C2dUnitVec& tan = line.StartTan();
	double p_side = tan.PerpDot(pvec);
	if (EQUAL(p_side, 1))
		return 0;

	double s_side = tan.PerpDot(svec);
	if (EQUAL(s_side, 1))
		return 0;

	// Combinations:
	//	one of p or s will be zero in ortho tests (p+s+, p+s-, p-s+, p-s-)
	//	p0s0 is also not possible 
	//	p+ or s+ rejected (p0s+, p+s0)
	//
	// So, only have to test for p0s- and p-s0
	//
	// Intepretation of PerpDot
	//	0 means the vectors are parallel or anti-parallel
	//	+ means the vector points to the left of the line (cc turn)
	//	- means the vector points to the right of the line (cw turn)
#if BEFORE_V18
	// 2006.11.12 (PE) -- During V18-beta, it became apparent that seeding was
	// not working properly for 1st quadrant machines because p_side was zero
	// in cases when it should not have been zero. After spending more than an
	// hour trying to understand the relationship between tan, pvec & svec, I
	// could not find the 'correct' way to fix said problem. As such, I have
	// disabled the offensive code. Time studies indicate there is no
	// performance degradation and the nesting results appear to be identical
	// except that now the 1st quadrant case works.
	if (ZERO(p_side))
	{
		// p0s-
		// Single secondary seed at end pointed to by P
		//
		if (EQUAL( tan.Y(), pvec.Y() ))
		{
			at_x = pe.X();
			at_y = pe.Y();
		}
		else // opposite
		{
			at_x = ps.X();
			at_y = ps.Y();
		}

		seed_it( secondary_bank, at_x, at_y, (int)pvec.Y() );

		return 1;
	}
	else
#endif
	{
		// p-s0
		// Row of % spaced points along line
		//
		double	vy, step;

		at_x = ps.X();
		vy = svec.Y();
		step = m_config->Narrow( m_config->Pass() ) * SEED_FACTOR;

		if (vy < -SMALL)
		{
			at_y = pe.Y();
			end_y = ps.Y();

			if ( center	&& (end_y > center->Y()) )
				end_y = center->Y();
		}
		else // vy+
		{
			at_y = ps.Y();
			end_y = pe.Y();

			if ( center && (end_y < center->Y()) )
				end_y = center->Y();
		}

		if ( m_config->IsBitmapNest() )
		{
			at_x += m_config->Spacing();
			if ( flip )
				end_y += m_config->Spacing();  // ASSUMPTION: We are in a hole.
		}

		return do_seed_vstrip( primary_bank, step, (int)vy, at_y, end_y, at_x );
	}

	return 0;
}

// ----------------------------------------------------------------------------

int CSheet::do_seed_vstrip( 
	eSeedBank	which_bank,
	double		step, 
	int			corner, 
	double		from_y, 
	double		to_y, 
	double		at_x )
{
	double at_y;
	int dir;
	if (from_y < to_y)
	{ 
		at_y = from_y;
		if (at_y > to_y)
			return 0;

		to_y += SMALL;
		dir = 1;
	}
	else
	{ 
		at_y = from_y;
		if (at_y < to_y)
			return 0;

		to_y -= SMALL;
		dir = -1;
	}
	//
	int num = abs((int)floor( fabs(at_y-to_y)/step )) + 1;
	int count = num;
	step *= dir;
	while (num--)
	{
		seed_it( which_bank, at_x, at_y, corner );
		at_y += step;
	}

	return count;
}

// ----------------------------------------------------------------------------

int CSheet::do_seed_arc( 
	eSeedBank		which_bank,
	int				corner,
	const CGeoArc&	geoArc )
{
	static double	chordal_tol = 1.e-2;  // arbitrary
	C3dCoordArray	tmp;
	
	geoArc.Tabulate( chordal_tol, C3dVec( 0., 0., 0. ), &tmp );

	int count = tmp.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		C3dCoord* pt = tmp.GetAt( indx );
		seed_it( which_bank, pt->X(), pt->Y(), corner );
	}

	tmp.DestructiveFlush();

	return count;
}

// ----------------------------------------------------------------------------

int CSheet::do_seed_decompose( 
	eSeedBank			primary_bank,
	eSeedBank			secondary_bank,
	const CGeoCurve&	raw_curve, 
	const C2dUnitVec&	pvec, 
	const C2dUnitVec&	svec,
	const C2dCoord*		center,
	double				flip )
{
	// Set mid-curve ordinates
	C3dCoord mid = raw_curve.MidPt();
	double mx = mid.X();
	double my = mid.Y();
	//
	// Now set the working curve...
	//
	CGeoLine curve;
	if (flip > SMALL)
	{ curve = CGeoLine( raw_curve.StartPt(), raw_curve.EndPt() ); }
	else
	{ curve = CGeoLine( raw_curve.EndPt(), raw_curve.StartPt() ); }

	C2dUnitVec chord = curve.EndPt() - curve.StartPt();
	int code = 0x00;
	if ((chord.X()) < -SMALL)
	{ code += 0x01; }

	if ((chord.Y()) < -SMALL)
	{ code += 0x02; }
	// Code: 0 up-right, 1 up left, 2 down right, 3 down left

	double p_side = chord.PerpDot(pvec);
	double s_side = chord.PerpDot(svec);

	if ((p_side > SMALL) && (s_side > SMALL))
		return 0;


	// TODO:  Fix all corner values.  Currently random.

	//
	// Combinations:
	//	neither p nor s will be zero in angled tests (p0s0, p0s+, p0s-, p+s0, p-s0)
	//	The ++ case is rejected (p+s+)
	//
	// Three basic nesting conditions:  Primary in X-, Primary in Y-, Primary in Y+
	//
	// Intepretation of PerpDot
	//	0 means the vectors are parallel or anti-parallel
	//	+ means the vector points to the left of the line (cc turn)
	//	- means the vector points to the right of the line (cw turn)
	//
	double vy = pvec.Y() + svec.Y();
	//
	// Decompose
	//
	int count = 0;
	double step = m_config->Narrow( m_config->Pass() ) * SEED_FACTOR;

	if (EQUAL(pvec.X(), -1))
	{
		// Primary X-
		//
		if ( vy > SMALL )
		{
			// Secondary Y+
			double min_y = (center ? (center->Y() - SMALL) : DBL_MIN);

			switch (code)
			{
			case 0:
				break;

			case 1:
				seed_it( secondary_bank, curve.EndPt().X(), curve.StartPt().Y(), (int)vy );
				count = 1;
				break;

			case 2:
				if (raw_curve.Type() == GEOARC)
				{
					count += do_seed_arc( primary_bank, (int) vy, (CGeoArc&) raw_curve );
				}
				else
				{
					if (my > min_y)
					{ count += do_seed_vstrip( primary_bank, step, (int)vy, curve.StartPt().Y(), my, mx ); }

					if (curve.EndPt().Y() > min_y)
					{ count += do_seed_vstrip( primary_bank, step, (int)vy, curve.StartPt().Y(), curve.EndPt().Y(), curve.EndPt().X() ); }
				}
				break;

			case 3:
				if (raw_curve.Type() == GEOARC)
				{
					count += do_seed_arc( primary_bank, (int) vy, (CGeoArc&) raw_curve );
				}
				else
				{
					if (my > min_y)
					{ count += do_seed_vstrip( primary_bank, step, (int)vy, curve.EndPt().Y(), curve.StartPt().Y(), curve.StartPt().X() ); }

					if (curve.EndPt().Y() > min_y)
					{ count += do_seed_vstrip( primary_bank, step, (int)vy, curve.EndPt().Y(), my, mx ); }
				}
				break;
			}
		}
		else // vy-
		{
			// Secondary Y-
			double max_y = (center ? (center->Y() + SMALL) : DBL_MAX);

			switch (code)
			{
			case 0:
				seed_it( secondary_bank, curve.StartPt().X(), curve.EndPt().Y(), (int)vy );
				count = 1;
				break;

			case 1:
				break;

			case 2:
				if (raw_curve.Type() == GEOARC)
				{
					count += do_seed_arc( primary_bank, (int) vy, (CGeoArc&) raw_curve );
				}
				else
				{
					if (my < max_y)
					{ count += do_seed_vstrip( primary_bank, step, (int)vy, curve.StartPt().Y(), curve.EndPt().Y(), curve.EndPt().X() ); }

					if (curve.StartPt().Y() < max_y)
					{ count += do_seed_vstrip( primary_bank, step, (int)vy, curve.StartPt().Y(), my, mx ); }
				}
				break;

			case 3:
				if (raw_curve.Type() == GEOARC)
				{
					count += do_seed_arc( primary_bank, (int) vy, (CGeoArc&) raw_curve );
				}
				else
				{
					if (my < max_y)
					{ count += do_seed_vstrip( primary_bank, step, (int)vy, curve.EndPt().Y(), my, mx ); }

					if (curve.StartPt().Y() < max_y)
					{ count += do_seed_vstrip( primary_bank, step, (int)vy, curve.EndPt().Y(), curve.StartPt().Y(), curve.StartPt().X() ); }
				}
				break;
			}
		}

	}
	else
	if ( vy > SMALL )
	{
		// Primary Y+
		double max_x = (center ? (center->X() + SMALL) : DBL_MAX);

		switch (code)
		{
		case 0:
			break;

		case 1:
			if (raw_curve.Type() == GEOARC)
			{
				count += do_seed_arc( primary_bank, (int) vy, (CGeoArc&) raw_curve );
			}
			else
			{
				if (mx < max_x)
				{ count += do_seed_hstrip( primary_bank, step, (int)vy, curve.EndPt().X(), mx, my ); }

				if (curve.StartPt().X() < max_x)
				{ count += do_seed_hstrip( primary_bank, step, (int)vy, curve.EndPt().X(), curve.StartPt().X(), curve.StartPt().Y() ); }
			}
			break;

		case 2:
			if (raw_curve.Type() == GEOARC)
			{
				count += do_seed_arc( primary_bank, (int) vy, (CGeoArc&) raw_curve );
			}
			else
			{
				seed_it( secondary_bank, curve.EndPt().X(), curve.StartPt().Y(), (int)vy );
				count = 1;
			}
			break;

		case 3:
			if (raw_curve.Type() == GEOARC)
			{
				count += do_seed_arc( primary_bank, (int) vy, (CGeoArc&) raw_curve );
			}
			else
			{
				if (mx < max_x)
				{ count = do_seed_hstrip( primary_bank, step, (int)vy, curve.StartPt().X(), curve.EndPt().X(), curve.EndPt().Y() ); }

				if (curve.StartPt().X() < max_x)
				{ count = do_seed_hstrip( primary_bank, step, (int)vy, curve.StartPt().X(), mx, my ); }
			}
			break;
		}
	}
	else // vy-
	{
		// Primary Y-
		double max_x = (center ? (center->X() + SMALL) : DBL_MAX);

		switch (code)
		{
		case 0:
			if (mx < max_x)
			{ count += do_seed_hstrip( primary_bank, step, (int)vy, curve.StartPt().X(), mx, my ); }

			if (curve.EndPt().X() < max_x)
			{ count += do_seed_hstrip( primary_bank, step, (int)vy, curve.StartPt().X(), curve.EndPt().X(), curve.EndPt().Y() ); }
			break;

		case 1:
			break;

		case 2:
			if (raw_curve.Type() == GEOARC)
			{
				count += do_seed_arc( primary_bank, (int) vy, (CGeoArc&) raw_curve );
			}
			else
			{
				if (mx < max_x)
				{ count += do_seed_hstrip( primary_bank, step, (int)vy, curve.EndPt().X(), curve.StartPt().X(), curve.StartPt().Y() ); }

				if (curve.EndPt().X() < max_x)
				{ count += do_seed_hstrip( primary_bank, step, (int)vy, curve.EndPt().X(), mx, my ); }
			}
			break;

		case 3:
			seed_it( secondary_bank, curve.StartPt().X(), curve.EndPt().Y(), (int)vy );
			count = 1;
			break;
		}
	}

	return count;
}

// ==================================================================
bool CSheet::SeedNext( CNestConfig& config, bool kill )
{
	CReturn		status;
	CString		note;
	C2dCoord	pt;
	CSeed		candidate;

	bool do_kill = kill;
	int test = TEST_OCCUPIED_NOCLAMP | MARK_DEAD;

	if (config.ClampAround())
		test |= MARK_CLAMP;

	candidate.Gravity( IUNDEFINED );

	while (1)
	{
		// TODO: This logic seems a bit odd because "kill"
		// always seems to be true on the first iteration.
		if ( !kill )
		{
			m_this_seed = candidate;
			break;
		}

		kill = false;

		candidate = m_seed_mgr->NextCandidate();
		if (candidate.Gravity() == IUNDEFINED)
			return false;

		if ( EWMNestingAllow() )
		{
			pt = candidate.Placement();
			note.Format( "   SeedNext() %f, %f", pt.X(), pt.Y() );
			EWMNesting( note );
		}
	} 

	return (candidate.Gravity() < IUNDEFINED);
}

// Punch (or burn, whatever) the part geometry into the sheet
CDbCommand* CSheet::PunchInstance(
	const CNestConfig&	config,
	const CPartPlace&	part_place,
	int					zone,
	bool				torch_shift,
	CViewMgr&			view )
{
	CReturn ret;
	double torch_offset = config.MachineReal( "Torch Offset in X", 0.0 );
	
	CNestingPart* npart = part_place.PartGet();
	CToolHit* toolhit = part_place.ToolHitGet();
	int rot = toolhit->Index();

	// Pattern this hit!  If need be...
	CDbTool* dbTool;
	m_model->EntityFind( "Instances", (CDbEntity**) &dbTool, DBTOOL, DBTOOL );
	if (dbTool == NULL)
	{
		m_model->EntityCreate( DBTOOL, (CDbEntity**) &dbTool );
		dbTool->Name("Instances");
		dbTool->ColorSet( DCOLOR_YELLOW );
	}

	CDbWorkplane* dbWork=NULL;// = m_model->ActiveWorkplane();
	m_model->EntityFind( STR_TOP, (CDbEntity**)&dbWork, DBWORKPLANE, DBWORKPLANE );

	if (dbWork == NULL)
	{
		CString note;
		note.Format( "Can not find required workplane '%s'.  Program will now crash.", STR_TOP );
		MessageBox( NULL, note, NULL, MB_OK );
		m_model->EntityFind( STR_TOP, (CDbEntity**)&dbWork, DBWORKPLANE, DBWORKPLANE );
	}

	int torch_zone = zone;
	if (config.PatternSplit() && torch_shift)
		++torch_zone;

	if (!toolhit->hasPatterns())
	{
		CDbPattern* dbPattern;

		CModelUtil::PatternCreate(&(m_model->Db()), &dbPattern);

		if (dbPattern != NULL)
			dbPattern->IntSet( "label_size", config.LabelSize() );

		if (config.PatternSplit())
		{ 
			// Burn, Punch, and Other
			npart->CopyTo(rot, m_model, dbPattern, DBLINE, DBARC);
			set_pattern(config, npart, toolhit, dbPattern, PATTERN_MARK_BURN);
			if (dbPattern->Count())
			{ toolhit->PatternBurn(dbPattern); }
			else
			{ dbPattern->Delete(); }

			CModelUtil::PatternCreate(&(m_model->Db()), &dbPattern);
			npart->CopyTo(rot, m_model, dbPattern, DBHOLE, DBHOLE);
			set_pattern(config, npart, toolhit, dbPattern, PATTERN_MARK_PUNCH);
			if (dbPattern->Count())
			{ toolhit->PatternPunch(dbPattern); }
			else
			{ dbPattern->Delete(); }

			CModelUtil::PatternCreate(&(m_model->Db()), &dbPattern);
			npart->CopyTo(rot, m_model, dbPattern, DBCOMMAND, DBCOMMAND);
			set_pattern(config, npart, toolhit, dbPattern, PATTERN_MARK_OTHER);
			if (dbPattern->Count())
			{ toolhit->PatternOther(dbPattern); }
			else
			{ dbPattern->Delete(); }
		}
		else
		{ 
			// All entities in the Burn pattern
			npart->CopyTo(rot, m_model, dbPattern, DBLINE, DBCOMMAND);
			set_pattern(config, npart, toolhit, dbPattern, PATTERN_MARK_ALL);

			toolhit->PatternBurn(dbPattern);
		}
	
		toolhit->hasPatterns(TRUE);
	}

	// Now Instance this hit's pattern
	CDbCommand* cmd = NULL;
	if (toolhit->PatternBurn())
	{
		C2dCoord handle = part_place.WorldPointGet();

		cmd = put_pattern( config, handle,
			torch_zone, dbWork, dbTool, toolhit->PatternBurn(), view );

		if ((cmd != NULL) && npart->Large())
		{
			// This information is critical to CNestMgr::nest_sheet_repo()
			double xmax = toolhit->Part()->Extent().Xmax();

			cmd->IntSet( "_large", 1 );
			cmd->DoubleSet( "_xmax", (handle.X() + xmax) );
		}
	}

	if (toolhit->PatternPunch())
	{
		put_pattern( config, part_place.WorldPointGet(), zone,
			dbWork, dbTool, toolhit->PatternPunch(), view );
	}

	if (toolhit->PatternOther())
	{
		put_pattern(config, part_place.WorldPointGet(), zone,
			dbWork, dbTool, toolhit->PatternOther(), view );
	}

	return cmd;
}

// ============================================================================

void CSheet::set_pattern(
	const CNestConfig& config,
	CNestingPart* npart,
	CToolHit* toolhit,
	CDbPattern* dbPattern,
	const CString& prefix)
{
	C3dBox box = dbPattern->Box();
	double dx = (box.Xmin() + box.Xmax()) / 2;
	double dy = (box.Ymin() + box.Ymax()) / 2;

	dbPattern->IntSet( "part_id", npart->Id() );
	dbPattern->IntSet( "part_mirror", toolhit->Mirror() );
	dbPattern->DoubleSet( "part_angle", toolhit->Rotation() );

	int ang = (int) ceil(toolhit->Rotation() * RAD2DEG);
	bool mirror = toolhit->Mirror();

	CString name;
#if (_CI || _NST)
	// We don't really care until conversion back to VDraw.
	// if (config.LabelName())
	{
		name.Format( "%s%s", prefix, npart->Label() );
	}
#else
	if (config.LabelName())
	{
		if (config.LabelTag())
		{ name.Format( "%s%s_%d%s", prefix, npart->Label(), ang, (mirror ? "M" : "") ); }
		else
		{ name.Format( "%s%s", prefix, npart->Label() ); }
	}
	else
	{
		if (config.LabelTag())
		{ name.Format( "%s%d_%d%s", prefix, (npart->Index()+1), ang, (mirror ? "M" : "") ); }
		else
		{ name.Format( "%s%d", prefix, (npart->Index()+1) ); }
	}
#endif

	dbPattern->Name( dbPattern->NameConvert(name) );

	// The _label value is what people see.  It is different
	// from the pattern name because it allows white space.
	CVarList* attrib = dbPattern->pAttrib();
	attrib->setString( "_label", name );

	attrib->setReal( "_label_dx", dx );
	attrib->setReal( "_label_dy", dy );
	attrib->setReal( "_label_angle", config.LabelAngle() );
#if (_CI || _NST)
	attrib->setReal( "_label_size", config.LabelSize() );
#else
	attrib->setInt( "_label_size", config.LabelSize() );
#endif
	attrib->setString(STR_TYPE, prefix);
	
	if (config.LabelTool() > 0)
	{
		CDbTool* tool = CModelUtil::FindToolByID(dbPattern->Db(), config.LabelTool(), TRUE);
		dbPattern->Tool(tool);
	}
}

// ----------------------------------------------------------

CDbCommand* CSheet::put_pattern(
	const CNestConfig& config,
	const C3dCoord& shift,
	int zone,
	CDbWorkplane* dbWork,
	CDbTool* dbTool,
	CDbPattern* pattern,
	CViewMgr& view)
{
	CReturn ret;

	CDbCommand* cmd = NULL;
	ret += m_model->EntityCreate( DBCOMMAND, (CDbEntity**)&cmd );

	CString inst;
	inst.Format( "@INSTANCE: instang=0.0, patid=%d, repo=%d", pattern->Id(), zone+1 );

	C3dCoord place( shift.X(), shift.Y( ), 0.0 );
	dbWork->Inverse().Transform( &place );
	cmd->Init( dbTool, dbWork, place, inst );
	cmd->SystemFlag( false );

	cmd->IntSet( "label_size", config.LabelSize() );
	cmd->DoubleSet( "angle", 0.0 );
	cmd->IntSet( "pos", TEXTPOS_BTMCTR );

	// Redundant, but makes life easier in VB
	cmd->StringSet( STR_TYPE, "_instance" );

	if (config.DisplayParts())
	{
		view.ModelSet( *m_model );

		view.ClearEnable( false );

		if ( config.DebugWire() )
		{
			// Start with a clean slate and redraw all model geometry.
			view.Refresh( false );
		}
		else
		{
			// Using the following statement offers better performance
			// while debugging the nest, but it does not clean-up the
			// view as nicely as the alternate call.
			view.Refresh( cmd->Id(), false );
		}

		view.ClearEnable( true );
	}

	return cmd;
}

// ============================================================================
// 2007.12.09 (PE) -- The original implementation of DidPlace() employed
// the single class boolean 'm_did_place' to track whether a part was
// just placed on the sheet. Said implementation became inadequate with
// introduction of recursive part-in-part algorithm. This of particular
// importance wrt. pre-nested parts because we must track the 'did place'
// status at each containment level.
bool CSheet::DidPlace() const
{
	bool did_place = false;

	CPartPlace* part_place = PartPlaceGet();
	if (part_place != NULL)
	{
		CToolHit* tool_hit = part_place->ToolHitGet();
		did_place = ((tool_hit != NULL) && tool_hit->JustPlaced());
	}

	return did_place;
}

void CSheet::DidPlace( bool did_place )
{
	CPartPlace* part_place = PartPlaceGet();
	if (part_place != NULL)
	{
		CToolHit* tool_hit = part_place->ToolHitGet();
		if (tool_hit != NULL)
			tool_hit->JustPlaced( did_place );
	}
}

CNestedArea* CSheet::PunchGeo(
	const CNestConfig&	config,
	const CPartPlace&	part_place,
	CNestedArea*		area_to_nest,
	CViewMgr&			view )
{
	CReturn status;

	// For debugging (?)
	CNestingPart* npart = part_place.PartGet();

	// NOTE: Even the sheet is treated as a nested area.
	CNestedArea* nested_area = new CNestedArea( part_place );
	area_to_nest->InteriorAreaAdd( nested_area );

	if ( config.DebugWire() )
	{
		// Dump the toolhit geometry into the target model.
		C3dCoord loc = part_place.WorldPointGet();
		CToolHit* toolhit = part_place.ToolHitGet();

		toolhit->DebugToModel( loc, &view, m_model );
	}

	PartPlaceSet( part_place );

	return nested_area;
}

// ==================================================================
//	Remove the sheet boundary geometry related to this instance
CReturn CSheet::ClearInstance( CDbCommand* inst )
{
	CReturn ret;

	for (int depth = HIT_NEST_OUTSIDE; depth <= HIT_NEST_INSIDE; ++depth)
	{
		CGeoPolyArray* geo_array = m_toolhit->PolysGet(depth);
		if (geo_array != NULL)
		{
			int num = geo_array->Count() -1;
			for (int idx=num; idx>=0; idx--)
			{
				CGeoPoly* poly = geo_array->GetAt(idx);
				if (poly->UserData() == (void*)inst)
				{
					delete geo_array->Remove( idx ); 
				}
			}
		}
	}

	return ret;
}


// ==================================================================
void CSheet::FitZone( const CNestConfig& config, const C2dBox& zone, double delta )
{
	// NOTE: This was initially impemented using "if-then-else".
// #if (_CI || _NST)
#if (_CI)
		// A bit cheezy ... but allows more parts on the sheet.
		// And since we don't have to worry about repo ...

		m_fitzone = C2dBox(
			(config.Border(BORDER_LEFT) - config.Spacing()),
			(config.Border(BORDER_BOTTOM) - config.Spacing()),
			(Length() - config.Border(BORDER_RIGHT) + config.Spacing()),
			(Width() - config.Border(BORDER_TOP) + config.Spacing()) );
#else
		m_fitzone = zone + delta;
#endif
}

// ==================================================================
//	Create the toolhit structures for this sheet
CToolHit* CSheet::build_toolhit( CGrid* grid )
{
	CToolHit* toolhit = new CToolHit();
	if (grid)
		toolhit->Grid( new CDexGrid( *grid ) );

	// Create the reserved polys
	for (int idx=0; idx<RESERVE_NUM; idx++)
	{
		toolhit->PolyAdd( HIT_NEST_OUTSIDE, new CGeoPoly() );
		toolhit->PolyAdd( HIT_PART_OUTSIDE, new CGeoPoly() );
	}

	toolhit->PolyAdd( HIT_PART_INSIDE,  new CGeoPoly() );
	toolhit->PolyAdd( HIT_NEST_INSIDE,  new CGeoPoly() );

	return toolhit;
}



// ==================================================================
//	Debug generate... instantiates the geometry in the model
//	while drawing it... may crash your program, so use wisely.
CReturn CSheet::DebugToModel( CModel* model, CViewMgr* view )
{
	return ( m_toolhit->DebugToModel( C3dCoord(0,0,0), view, model ) );
}



// ==================================================================
//		distance_x
//		distance_y
//
//	Distance between two curves along an axis, for shifting porpoises.
//	Note that we SPECIFY the range of the curve we are interested in.
//	Note that we ASSUME the curves both exist in the range of the span.
//
//	THIS IS NOT A GENERAL PURPOSE DISTANCE SYSTEM -- it is fairly specific
//	to nesting.
//
//	This is one case where code could be cut in HALF if we indexed X and Y
//	instead of name them.
//
enum eArcArcTest
{
	AA_INSIDE,
	AA_OUTSIDE,
	AA_TIPS
};


double CSheet::distance_xy(
	int prim_axis,
	const CGeoCurve& curve1,
	const CGeoCurve& curve2,
	double min_ord,
	double max_ord,
	double sign ) const
{
	switch (curve1.Type())
	{
	case GEOLINE:
		switch (curve2.Type())
		{
		case GEOLINE:
			return dist_ln_ln_xy( prim_axis, (CGeoLine&)curve1, (CGeoLine&)curve2, min_ord, max_ord, sign );
		case GEOARC:
			return dist_arc_ln_xy( prim_axis, (CGeoArc&)curve2, (CGeoLine&)curve1, min_ord, max_ord, -sign );
		}
		break;

	case GEOARC:
		switch (curve2.Type())
		{
		case GEOLINE:
			return dist_arc_ln_xy( prim_axis, (CGeoArc&)curve1, (CGeoLine&)curve2, min_ord, max_ord, sign );
		case GEOARC:
			return dist_arc_arc_xy( prim_axis, (CGeoArc&)curve1, (CGeoArc&)curve2, min_ord, max_ord, sign );
		}
		break;
	}
	return DBL_MAX;
}

double CSheet::dist_ln_ln_xy( 
	int prim_axis,
	const CGeoLine& line1, 
	const CGeoLine& line2, 
	double min_ord,
	double max_ord,
	double sign ) const
{
	//
	// Note that min and max distances are the distances AT the min and max secondary ordinate, and not
	// a statement of relative magnitude
	//
	double min_dist = sign * (line1.InterceptXY(prim_axis, min_ord) - line2.InterceptXY(prim_axis, min_ord));
	if (EQUAL( min_ord, max_ord ))
		return min_dist;

	double max_dist = sign * (line1.InterceptXY(prim_axis, max_ord) - line2.InterceptXY(prim_axis, max_ord));

	// We can test for parallel lines if we cared...

	if (min_dist > max_dist)
		min_dist = max_dist;

	return min_dist;
}

double CSheet::dist_arc_ln_xy( 
	int prim_axis,
	const CGeoArc& arc1, 
	const CGeoLine& line2, 
	double min_ord,
	double max_ord,
	double sign ) const
{
	int sec_axis = FLIP_XY(prim_axis);

	C3dCoord pt; 
	double uparam;
	const C3dCoord& ctr = arc1.CenterPt();

	line2.PointClosest( ctr, &pt, &uparam );
	C2dVec radial = pt - ctr;
	double factor = arc1.Radius() / radial.Length();
	double crossover = ctr[sec_axis] + radial[sec_axis] * factor;

	double dist = DBL_MAX;
	if ( (crossover < (min_ord+SMALL)) || (crossover > (max_ord-SMALL)) )
	{
		double min_dist = sign * (arc1.InterceptXY(prim_axis, min_ord) - line2.InterceptXY(prim_axis, min_ord));
		if (EQUAL( min_ord, max_ord ))
		{
			dist = min_dist;
		}
		else
		{
			double max_dist = sign * (arc1.InterceptXY(prim_axis, max_ord) - line2.InterceptXY(prim_axis, max_ord));

			dist = ((min_dist > max_dist) ? max_dist : min_dist);
		}
	}
	else 
	{
		dist = sign * (arc1.InterceptXY(prim_axis, crossover) - line2.InterceptXY(prim_axis, crossover));
	}

	return dist;
}

// ----------------------------------------------------------------------------
// This could be reduced to logic, but I want the pictures and better control
// and understanding over the behavior.
static eArcArcTest g_test_quad_quad[16] =
{
	// Quadrants
	//
	// Y^	01 /~~\ 00
	//	|	  |  . |
	//	|	  |    |
	//	|	10 \__/ 11
	//	|
	//	------>
	//        X
	//
	AA_INSIDE,	// 0  00 <- 00 ~\ ~\ 
	AA_OUTSIDE,	// 1  00 <- 01 ~\ /~ 
	AA_OUTSIDE,	// 2  00 <- 10 ~\ \_ 
	AA_TIPS,	// 3  00 <- 11 ~\ _/ 

	AA_TIPS,	// 4  01 <- 00 /~ ~\ 
	AA_INSIDE,	// 5  01 <- 01 /~ /~ 
	AA_INSIDE,	// 6  01 <- 10 /~ \_ 
	AA_TIPS,	// 7  01 <- 11 /~ _/ 

	AA_TIPS,	// 8  10 <- 00 \_ ~\ 
	AA_INSIDE,	// 9  10 <- 01 \_ /~ 
	AA_INSIDE,	// A  10 <- 10 \_ \_ 
	AA_TIPS,	// B  10 <- 11 \_ _/ 

	AA_INSIDE,	// C  11 <- 00 _/ ~\ 
	AA_OUTSIDE,	// D  11 <- 01 _/ /~ 
	AA_OUTSIDE,	// E  11 <- 10 _/ \_ 
	AA_INSIDE	// F  11 <- 11 _/ _/ 
};

double CSheet::dist_arc_arc_xy( 
	int prim_axis, 
	const CGeoArc& arc1, // Part
	const CGeoArc& arc2, // moves to Sheet
	double min_ord,
	double max_ord,
	double sign ) const	// This direction along X
{
	int sec_axis = FLIP_XY(prim_axis);
	double dist = 0.0;
	//
	// Discriminate the arc cases by quadrant.
	//
	double m1 = (arc1.StartPt()[prim_axis] + arc1.EndPt()[prim_axis]) / 2.0;
	double m2 = (arc1.StartPt()[sec_axis] + arc1.EndPt()[sec_axis]) / 2.0;
	//
	int q1 = (m1 > arc1.CenterPt()[prim_axis])?0:1;
	if (m2 < arc1.CenterPt()[sec_axis])
	{ q1 ^= 0x03; }
	if (prim_axis == ORD_Y)
	{ q1 ^= 0x03; }
	//
	//
	m1 = (arc2.StartPt()[prim_axis] + arc2.EndPt()[prim_axis]) / 2.0;
	m2 = (arc2.StartPt()[sec_axis] + arc2.EndPt()[sec_axis]) / 2.0;
	//
	int q2 = (m1 > arc2.CenterPt()[prim_axis])?0:1;
	if (m2 < arc2.CenterPt()[sec_axis])
	{ q2 ^= 0x03; }
	if (prim_axis == ORD_Y)
	{ q2 ^= 0x03; }
	//
	//
	if (sign < 0)
	{ q1 <<= 2; }
	else 
	{ q2 <<= 2; }
	int code = q1 + q2;
	eArcArcTest test = g_test_quad_quad[code];
	//
	//
	double hyp;
	if (test != AA_TIPS)
	{
		if (test == AA_INSIDE) // Interior case
		{ hyp = arc2.Radius() - arc1.Radius(); }
		else // AA_OUTSIDE
		{  hyp = arc1.Radius() + arc2.Radius(); }
		//
		// Where do the arcs touch?
		//
		double d2 = arc1.CenterPt()[sec_axis] - arc2.CenterPt()[sec_axis];
		double touch = arc1.CenterPt()[sec_axis] - d2*(arc1.Radius() / hyp);

		if ( (touch > (min_ord-SMALL))
			&& (touch < (max_ord+SMALL)) )
		{
			// Distance across bulge...
			//
			double d1 = arc1.CenterPt()[prim_axis] - arc2.CenterPt()[prim_axis];
			double dest = SGN(d1) * sqrt( hyp*hyp - d2*d2 );
			dist = sign * (d1 - dest);
		}
		else
		{ test = AA_TIPS; }
	}

	if (test == AA_TIPS)
	{
		// Base distance, tip to tip
		//
		double min_dist = sign * (arc1.InterceptXY(prim_axis, min_ord) - arc2.InterceptXY(prim_axis, min_ord));
		if (EQUAL( min_ord, max_ord ))
		{ dist = min_dist; }
		else
		{
			double max_dist = sign * (arc1.InterceptXY(prim_axis, max_ord) - arc2.InterceptXY(prim_axis, max_ord));

			dist = ((min_dist > max_dist) ? max_dist : min_dist);
		}
	}

	return dist;
}


// ==================================================================
//	Return the extent of the sheet that has been cut.  Note that
//	we know this best, not by using the pModel()->BoxUser(), but by
//	iterating the outside kerf geometry.
//
C2dBox CSheet::BoxUser() const
{
	C2dBox extent;

	int icnt = m_nested_area->InteriorAreasCount();
	for (int indx = 0; indx < icnt; ++indx)
	{
		CNestedArea* interior_area = m_nested_area->InteriorAreaGet( indx );
		CGeoPolyArray* polys = interior_area->PolysGet( HIT_NEST_OUTSIDE );

		int jcnt = polys->Count();  // Should be only one.
		for (int jndx = 0; jndx < jcnt; ++jndx)
		{
			CGeoPoly* poly = polys->GetAt( jndx );
			extent += poly->Extent();
		}
	}

	return extent;
}

void CSheet::CollectionsDraw(
	const CGeoCurveArray& curvesA, const C3dCoord& shiftA,
	const CGeoCurveArray& curvesB, const C3dCoord& shiftB ) const
{
	C2dBox boxA = curvesA.Box();
	boxA.Shift( shiftA.X(), shiftA.Y() );

	C2dBox boxB = curvesB.Box();
	boxB.Shift( shiftB.X(), shiftB.Y() );

	boxA += boxB;

	CGeoRenderer& renderer = (CGeoRenderer&) GeoRendererGet();

	renderer.WindowSet( boxA );

	renderer.DisplayListOpen();

	renderer.DrawColorSet( DCOLOR_GREEN );
	renderer.OriginSet( shiftA );
	curvesA.Draw();

	renderer.DrawColorSet( DCOLOR_RED );
	renderer.OriginSet( shiftB );
	curvesB.Draw();

	renderer.OriginSet( C3dCoord( 0,0,0 ) );

	renderer.DisplayListClose();
	renderer.Draw();
}

// ==================================================================
//	All nest geometry is processed here.  The axis indicates if we are
//	thinking in X or Y, and the process function performs the action.
//
double CSheet::do_process(	int				prim_axis,
				    eSheetProc		process,
					const C3dCoord& shift,
					CGeoPolyArray*	sheet_polys,
					CGeoPolyArray*	part_polys,
					bool			prof_reverse,
					double			sign,
					const C2dBox&	bump_zone ) const
{
	CGeoCurveArray	sheet_curves;
	CGeoCurveArray	part_curves;

	int sec_axis = FLIP_XY(prim_axis);
	bool reverse = prof_reverse;
	if (sign > 0)
	{ reverse = !reverse; }

	if ( !sheet_polys
		|| !sheet_polys->Count()
		|| !part_polys
		|| !part_polys->Count() )
	{ return 0.0; }

	double result = ((process == SPROC_SCORE) ? 0. : DBL_MAX);

	// In case we get an empty sheet, here is a boundary line to work against
	CGeoLine block_line;
	if (sign > 0)
	{
		if (prim_axis == ORD_X)
		{ block_line.StartPt( C3dCoord(bump_zone.Xmax(), bump_zone.Ymin(), 0.0) ); }
		else // ORD_Y
		{ block_line.StartPt( C3dCoord(bump_zone.Xmin(), bump_zone.Ymax(), 0.0) ); }

		block_line.EndPt( C3dCoord(bump_zone.Xmax(), bump_zone.Ymax(), 0.0) );
	}
	else
	{
		block_line.StartPt( C3dCoord(bump_zone.Xmin(), bump_zone.Ymin(), 0.0) );

		if (prim_axis == ORD_X)
		{ block_line.EndPt( C3dCoord(bump_zone.Xmin(), bump_zone.Ymax(), 0.0) ); }
		else // ORD_Y
		{ block_line.EndPt( C3dCoord(bump_zone.Xmax(), bump_zone.Ymin(), 0.0) ); }
	}

	// Assemble all geometry that overlaps the bump zone into the two lists, 
	// to reduce the amount of work done in the face-off.
	do_collect_geo_xy( prim_axis, &part_curves, *part_polys, shift, reverse, bump_zone );

	C3dCoord zero( 0, 0, 0 );
	do_collect_geo_xy( prim_axis, &sheet_curves, *sheet_polys, zero, reverse, bump_zone );
	
	// For debugging.
	if (0)
	{
		CollectionsDraw( sheet_curves, zero, part_curves, shift );
	}

	//
	// Build a list of ordinates from both lists... this is our chop list
	//	Ignore duplicates, they are skipped during the run
	//
	CDoubleArray chop;
	double ord;

	double min_ord = bump_zone.BL()[sec_axis];
	double max_ord = bump_zone.TR()[sec_axis];

	chop.Append( min_ord );
	chop.Append( max_ord );

	int snum = sheet_curves.Count();
	for (int sidx=0; sidx<snum; sidx++)
	{
		ord = sheet_curves[sidx]->StartPt()[sec_axis];
		if ((ord > min_ord) && (ord < max_ord))
			chop.Append( ord );

		ord = sheet_curves[sidx]->EndPt()[sec_axis];
		if ((ord > min_ord) && (ord < max_ord))
			chop.Append( ord );
	}

	int pnum = part_curves.Count();
	double shift_ord = shift[sec_axis];
	for (int pidx=0; pidx<pnum; pidx++)
	{
		ord = part_curves[pidx]->StartPt()[sec_axis] + shift_ord;
		if ((ord > min_ord) && (ord < max_ord))
			chop.Append( ord );

		ord = part_curves[pidx]->EndPt()[sec_axis] + shift_ord;
		if ((ord > min_ord) && (ord < max_ord))
			chop.Append( ord );
	}

	chop.Qsort( compare_double );	// Low to High

	ChopListReduce( &chop );
	//
	// Now scan through the entire bump zone in tiny steps
	//

	C2dBox test_box;
	C2dBox curve_shift;
	double curr_ord, next_ord;
	int chop_cnt, chop_idx;

	chop_cnt = chop.Count();
	for (chop_idx = 1; chop_idx < chop_cnt; ++chop_idx)
	{
		curr_ord = chop.GetAt( chop_idx-1 );
		next_ord = chop.GetAt(  chop_idx  );

		if (prim_axis == ORD_X)
		{
			// 2007.11.07 (PE)
			//    test_box.Update( 0.0, curr_ord+SMALL, bump_zone.Xmax(), next_ord-SMALL );
			test_box.Update( bump_zone.Xmin(), curr_ord+SMALL, bump_zone.Xmax(), next_ord-SMALL );
		}
		else // ORD_Y
		{
			// 2007.11.07 (PE)
			//    test_box.Update( curr_ord+SMALL, 0.0, next_ord-SMALL, bump_zone.Ymax() );
			test_box.Update( curr_ord+SMALL, bump_zone.Ymin(), next_ord-SMALL, bump_zone.Ymax() );
		}

		// Now collect the geometry this box crosses
		CGeoCurveArray sub_part;
		for (int pidx=0; pidx<pnum; pidx++)
		{
			CGeoCurve* curve = part_curves[pidx];
			const C2dBox& curve_extent = curve->Box();
			curve_shift.Update( curve_extent.Xmin() + shift.X(),
								curve_extent.Ymin() + shift.Y(),
								curve_extent.Xmax() + shift.X(),
								curve_extent.Ymax() + shift.Y() );
			if (curve_shift.Intersects(test_box, 0.0))
			{
				sub_part.Append( curve );
				if (0)
				{
					m_view_mgr->ActiveView()->DrawGeoAt( (*curve), shift );
					m_view_mgr->ActiveView()->DrawGeoAt( (*curve), shift );
				}
			}
		}

		if (!sub_part.Count())
			continue;

		CGeoCurveArray sub_sheet;
		for (int sidx=0; sidx<snum; sidx++)
		{
			CGeoCurve* curve = sheet_curves[sidx];
			const C2dBox& curve_extent = curve->Box();
			if (curve_extent.Intersects(test_box, 0.0))
				sub_sheet.Append( curve );
		}

		if (!sub_sheet.Count())
			sub_sheet.Append( &block_line );


		// 2004.05.26 (PE) -- CRITICAL: g_mid_ord is used (indirectly)
		// by Qsort() (via compare_geo_left() and compare_geo_right())
		// to shift geometry to a local coordinate system. It was
		// determined (after much help from Edwin) that sheet geometry
		// must be compared in sheet-space and part geometry must be
		// compared in part-space.
		//
		// Prior to this change, everything
		// was being compared in sheet-space, which caused sporatic
		// failures (of parts to nest).  In retrospect, we were left
		// wondering how nesting ever worked so well (given the
		// previously flawed optimization).
		//
		g_mid_ord = (curr_ord + next_ord) * 0.5;
		g_mid_axis = prim_axis;

		// Now sort and test from the sub-lists
#if BEFORE_2008_03_25
		if (prof_reverse)
#else
		if (true)
#endif
		{
			sub_sheet.Qsort( &CSeedMgr::compare_geo_left );

			g_mid_ord -= ((prim_axis == ORD_X) ? shift.Y() : shift.X());

			sub_part.Qsort( &CSeedMgr::compare_geo_right );
		}
		else
		{
			sub_sheet.Qsort( &CSeedMgr::compare_geo_right );

			g_mid_ord -= ((prim_axis == ORD_X) ? shift.Y() : shift.X());

			sub_part.Qsort( &CSeedMgr::compare_geo_left );
		}

		// For debugging.
		if (0)
		{
			CollectionsDraw( sub_sheet, zero, sub_part, shift );
		}

//******************
		CGeoCurve* pcrv;
		CGeoCurve* scrv;
		double pcrv_intercept;
		double scrv_intercept;
		double dist, nearest, val;
		int pcrv_cnt, pcrv_indx;
		int scrv_cnt, scrv_indx;
		int pcrv_near, scrv_near;

		nearest = -DBL_MAX;
		pcrv_near = -1;
		scrv_near = -1;

		pcrv_cnt = sub_part.Count();
		scrv_cnt = sub_sheet.Count();

		if (process == SPROC_SCORE)
			pcrv_cnt = 1;

		for (pcrv_indx = 0; pcrv_indx < pcrv_cnt; ++pcrv_indx)
		{
			pcrv = (CGeoCurve*) sub_part[pcrv_indx]->Clone( false );
			pcrv->Shift( C3dVec( shift.X(), shift.Y(), 0.0 ) );

			pcrv_intercept = pcrv->InterceptXY( prim_axis, curr_ord );

			for (scrv_indx = 0; scrv_indx < scrv_cnt; ++scrv_indx)
			{
				scrv = sub_sheet[scrv_indx];

				scrv_intercept = scrv->InterceptXY( prim_axis, curr_ord );

				dist = -sign * (scrv_intercept - pcrv_intercept);
				if ((dist <= SMALL) && (dist > nearest))
				{
					nearest = dist;
					pcrv_near = pcrv_indx;
					scrv_near = scrv_indx;
				}
			}

			delete pcrv;
		}

		if ((pcrv_near >= 0) && (scrv_near >= 0))
		{
			scrv = sub_sheet[scrv_near];
			pcrv = (CGeoCurve*) sub_part[pcrv_near]->Clone( false );
			pcrv->Shift( C3dVec( shift.X(), shift.Y(), 0.0 ) );

			val = evaluate( process, prim_axis, (*scrv), (*pcrv),
				curr_ord, next_ord, -sign );

			switch (process)
			{
			case SPROC_SCORE:
				// result is an area
				result += ((val < DBL_MAX) ? val : 0.);
				break;

			case SPROC_BUMP:
				// result is a distance
				if ((val > -SMALL) && (val < result))
					result = val;
				break;
			}

			delete pcrv;
		}
//******************

		sub_part.BenignFlush();
		sub_sheet.BenignFlush();
	}

	sheet_curves.BenignFlush();
	part_curves.BenignFlush();

	return result;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// (2004.05.08) Whilst getting remnant tracker working, I discovered
// that parts would fail to nest because their kerf would intersect
// with the remnant kerf.  Shifting the lower left corner of the
// remnant to (0,0) resolves that problem.
//
C3dBox CSheet::remnant_shift( CModel* remnant_model )
{
	C3x4Matrix	to_origin;
	C3x4Matrix	mirror;
	C3x4Matrix	from_origin;
	C3dBox		box;
	CDbProfile*	dbProfile;
	int			count, indx;

	CSelectorStack& selectorStack = remnant_model->SelectorStack();
	selectorStack.Push();

	CSelector& selector = selectorStack();

	selector.All( 0 );
	selector.Filter( DBPROFILE, 1 );

	selector.SystemFlag( TRUE );
	selector.SelectAll( TRUE );

	// 2006.09.21 (PE) -- Hmmmmm, and just why do we reverse the
	// profiles?  Will this cause problems if someone has manually
	// edited the remnant?
	count = selector.Count();
	for (indx = 0; indx < count; ++indx)
	{
		dbProfile = dynamic_cast<CDbProfile*>( selector[indx] );
		if (dbProfile != NULL)
			dbProfile->Reverse();
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Move the left edge back to the Y axis.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	to_origin.setUnit();
	mirror.setUnit();


	// 2004.05.09 (PE) --  Must shift the lower left corner of the
	// remnant to (0,0).  Otherwise parts will fail to be nested
	// on the remnant because they will intersect the sheet kerf.
	//
	// NOTE: We may need to compare against a smaller number, but
	// that is unlikely since kerf is typically larger than 1.e-3.
	box = remnant_model->Box();
	if ((fabs(box.Xmin()) > 0.001) || fabs(box.Ymin()) > 0.001)
	{
		C3dVec	vec;

		vec.Init( -box.Xmin(), -box.Ymin(), 0. );

		to_origin.Shift( vec );

		CModelUtil::Transform( remnant_model, to_origin, 0, XFORM_NO_SYSTEM );

		// 2006.09.21 (PE) -- Returning this information is critical
		// to the client.  Otherwise, nesting will fail (for the same
		// reason mentioned above) when nesting 'away from clamps' on
		// a 1st quadrant machine (Connweld).
		box.Shift( vec );
	}

	selector.Clear();
	selectorStack.Pop();

	return box;
}

void CSheet::SeedsDerive(
	const C2dBox&	box,
	eSeedBank		primary_bank,
	eSeedBank		secondary_bank )
{
	if ( CNestConfig::IsBitmapNest() )
		BitmapSeedsDerive( box, primary_bank, secondary_bank );
	else
		ExactSeedsDerive( box, primary_bank, secondary_bank );
}

void CSheet::ExactSeedsDerive(
	const C2dBox&	box,
	eSeedBank		primary_bank,
	eSeedBank		secondary_bank )
{
	C3dCoord	primary_corner;
	C3dCoord	secondary_corner;
	CSeed		seed;
	C2dCoord	grid_pt;
	double		xp, yp;
	double		dx, dy;
	eNestProgression	sort_dir;

	CPartPlace* pp = PartPlaceGet();
	CToolHit* toolhit = ((pp == NULL) ? NULL : pp->ToolHitGet());

	if ((m_config->Progression() == PROGRESS_PPX_SPY) ||
		(m_config->Progression() == PROGRESS_PPX_SNY))
		sort_dir = (m_config->NestPosY() ? PROGRESS_PPX_SPY : PROGRESS_PPX_SNY);
	else // FILL_Y
		sort_dir = (m_config->NestPosY() ? PROGRESS_PPY_SPX : PROGRESS_PNY_SPX);

	// Get the parameters describing the positions of the lower-left
	// corner of the mer, of the candidate part positions, along the
	// primary and secondary propogation directions.
	xp = box.Xmin();
	if ((sort_dir == PROGRESS_PPY_SPX) || (sort_dir == PROGRESS_PPX_SPY))
		yp = box.Ymin();
	else
		yp = box.Ymax();

#if BEFORE_V19
#else

	// The grid-link toolhit delta was determined by CNestMgr::PreNest() by
	// actually nesting parts to find the best packing. Using this delta
	// for seeding should yield an *exact* placement solution.
	//
	// TODO: Remove this comment because it is not accurate.
	// From CNestingPart::GridLink() we see that this dx & dy represent
	// the (mer - (mer_of_outside_kerf - mer_of_outside_part). Using this
	// dx & dy for seeding yields an *exact* placement.solution. This was
	// discovered while nesting dxfdemo5 (the Y-shaped part) along the
	// X-axis, where the part has pre-rotation of zero and no degrees of freedom, 
	if (toolhit != NULL)
	{
		dx = toolhit->Delta().X();
		dy = toolhit->Delta().Y();

		// Calcuate the part positions.
		switch (sort_dir)
		{
		case PROGRESS_PPY_SPX:
			//
			// P
			// |
			// |
			// |_____ S
			//
			primary_corner.XYZ( xp, (yp + dy), -1. );
			secondary_corner.XYZ( (xp + dx), yp, -1. );
			break;
		case PROGRESS_PNY_SPX:
			//  _____ S
			// |
			// |
			// |
			// P
			//
			primary_corner.XYZ( xp, (yp - dy), -1. );
			secondary_corner.XYZ( (xp + dx), yp, -1. );
			break;
		case PROGRESS_PPX_SPY:
			//
			// S
			// |
			// |
			// |_____ P
			//
			primary_corner.XYZ( (xp + dx), yp, -1. );
			secondary_corner.XYZ( xp, (yp + dy), -1. );
			break;
		case PROGRESS_PPX_SNY:
			//  _____ P
			// |
			// |
			// |
			// S
			//
			primary_corner.XYZ( (xp + dx), yp, -1. );
			secondary_corner.XYZ( xp, (yp - dy), -1. );
			break;
		}

		seed = CSeed( 2, primary_corner, toolhit, 0 );
		m_seed_mgr->Deposit( primary_bank, seed );

		seed = CSeed( 2, secondary_corner, toolhit, 0 );
		m_seed_mgr->Deposit( secondary_bank, seed );
	}
#endif

	dx = box.Dx();
	dy = box.Dy();

	// Calcuate the part positions.
	switch (sort_dir)
	{
	case PROGRESS_PPY_SPX:
		//
		// P
		// |
		// |
		// |_____ S
		//
		primary_corner.XYZ( xp, (yp + dy), -1. );
		secondary_corner.XYZ( (xp + dx), yp, -1. );
		break;
	case PROGRESS_PNY_SPX:
		//  _____ S
		// |
		// |
		// |
		// P
		//
		primary_corner.XYZ( xp, (yp - dy), -1. );
		secondary_corner.XYZ( (xp + dx), yp, -1. );
		break;
	case PROGRESS_PPX_SPY:
		//
		// S
		// |
		// |
		// |_____ P
		//
		primary_corner.XYZ( (xp + dx), yp, -1. );
		secondary_corner.XYZ( xp, (yp + dy), -1. );
		break;
	case PROGRESS_PPX_SNY:
		//  _____ P
		// |
		// |
		// |
		// S
		//
		primary_corner.XYZ( (xp + dx), yp, -1. );
		secondary_corner.XYZ( xp, (yp - dy), -1. );
		break;
	}

	// 2007.08.24 (PE) -- The condition where toohit is NULL indicates
	// that we are processing the inner perimeter of the sheet. As such,
	// the primary seeds are not relevant and would be discarded anyways
	// because the initial place of any part using these seeds would be
	// off the sheet. Note, given this we could altogether forego calling
	// this function (or the preceding seed calculations) but keeping those
	// in place is useful for debugging purposes.
	//
	// 2009.12.19 (PE) -- Introduced use of 'm_retain_seeds'.
	if ((toolhit != NULL) || m_retain_seeds)
	{
		seed = CSeed( 2, primary_corner, toolhit, 0 );
		m_seed_mgr->Deposit( primary_bank, seed );

		seed = CSeed( 2, secondary_corner, toolhit, 0 );
		m_seed_mgr->Deposit( secondary_bank, seed );
	}
}

void CSheet::BitmapSeedsDerive(  // DYNATORCH
	const C2dBox&	box,
	eSeedBank		primary_bank,
	eSeedBank		secondary_bank )
{
	C3dCoord	primary_corner;
	C3dCoord	secondary_corner;
	C2dCoord	tmp;
	CSeed		seed;
	C2dCoord	grid_pt;
	CToolHit*	toolhit;
	double		xp, yp;
	double		dx, dy;
	double		bump;
	eNestProgression	sort_dir;

	CPartPlace* pp = PartPlaceGet();
	toolhit = ((pp == NULL) ? NULL : pp->ToolHitGet());

	if ((m_config->Progression() == PROGRESS_PPX_SPY) ||
		(m_config->Progression() == PROGRESS_PPX_SNY))
		sort_dir = (m_config->NestPosY() ? PROGRESS_PPX_SPY : PROGRESS_PPX_SNY);
	else // FILL_Y
		sort_dir = (m_config->NestPosY() ? PROGRESS_PPY_SPX : PROGRESS_PNY_SPX);

	// Get the parameters describing the positions of the lower-left
	// corner of the mer, of the candidate part positions, along the
	// primary and secondary propogation directions.
	xp = box.Xmin();
	if ((sort_dir == PROGRESS_PPY_SPX) || (sort_dir == PROGRESS_PPX_SPY))
		yp = box.Ymin();
	else
		yp = box.Ymax();

	double res = m_config->Resolution();
	dx = ((int)(box.Dx() / res) + 1) * res;
	dy = ((int)(box.Dy() / res) + 1) * res;

	// TODO: DYNATORCH -- the factor of 2 is a kludge to get around
	// mapping-issues between world and grid coordinates.
	// TODO: We might consider subtract (SOME_FACTOR * config.Spacing())
	bump = m_config->Spacing() + (2. * m_config->Resolution());

	// Calculate the part positions.
	switch (sort_dir)
	{
	case PROGRESS_PPY_SPX:
		//
		// P
		// |
		// |
		// |_____ S
		//
		primary_corner.XYZ( xp, (yp + dy + bump), -1. );
		secondary_corner.XYZ( (xp + dx + bump), yp, -1. );
		break;
	case PROGRESS_PNY_SPX:
		//  _____ S
		// |
		// |
		// |
		// P
		//
		primary_corner.XYZ( xp, (yp - dy - bump), -1. );
		secondary_corner.XYZ( (xp + dx + bump), yp, -1. );
		break;
	case PROGRESS_PPX_SPY:
		//
		// S
		// |
		// |
		// |_____ P
		//
		primary_corner.XYZ( (xp + dx + bump), yp, -1. );
		secondary_corner.XYZ( xp, (yp + dy + bump), -1. );
		break;
	case PROGRESS_PPX_SNY:
		//  _____ P
		// |
		// |
		// |
		// S
		//
		primary_corner.XYZ( (xp + dx + bump), yp, -1. );
		secondary_corner.XYZ( xp, (yp - dy - bump), -1. );
		break;
	}

	// 2007.08.24 (PE) -- The condition where toohit is NULL indicates
	// that we are processing the inner perimeter of the sheet. As such,
	// the primary seeds are not relevant and would be discarded anyways
	// because the initial place of any part using these seeds would be
	// off the sheet. Note, given this we could altogether forego calling
	// this function (or the preceding seed calculations) but keeping those
	// in place is useful for debugging purposes.
	if (toolhit != NULL)
	{
		seed = CSeed( 2, primary_corner, toolhit, 0 );
		m_seed_mgr->Deposit( primary_bank, seed );

		seed = CSeed( 2, secondary_corner, toolhit, 0 );
		m_seed_mgr->Deposit( secondary_bank, seed );
	}
}

CSeed CSheet::NextSeedGet()
{
	CSeed candidate;

	if ( DidPlace() )
	{
		candidate = CSeed( 2, PartPlaceGet()->WorldPointGet(),
			PartPlaceGet()->ToolHitGet(), 0 );
	}
	else
	{
		candidate = m_seed_mgr->NextCandidate();
	}

	return candidate;
}

int CSheet::GridScore( eNestProgression progression, const C2dCoord& pt ) const
{
	C2dCoord bb;
	int score;

	CDexGrid* grid = m_toolhit->Grid();

	int dx = grid->Width();
	int dy = grid->Height();

	double px = pt.X();
	double py = pt.Y();
	
	switch (progression)
	{
	case PROGRESS_PPY_SPX:
		// Higher score for building columns in +Y (min X)
		// (px,py) -- lower left corner of bounding box.
		bb = m_toolhit->WorldToGrid( px, py );
		score = (dy * (dx - (int) bb.X())) + (dy - (int) bb.Y());
		break;

	case PROGRESS_PNY_SPX:
		// Higher score for building columns in -Y (min X)
		// (px,py) -- upper left corner of bounding box.
		bb = m_toolhit->WorldToGrid( px, py );
		score = (dy * (dx - (int) bb.X())) + (int) bb.Y();
		break;
	
	case PROGRESS_PPX_SPY:
		// Higher score for building rows in +X (min Y)
		// (px,py) -- lower left corner of bounding box.
		bb = m_toolhit->WorldToGrid( px, py );
		score = (dx * (dy - (int) bb.Y())) + (dx - (int) bb.X());
		break;
	
	case PROGRESS_PPX_SNY:
		// Higher score for building rows in +X (max Y)
		// (px,py) -- upper left corner of bounding box.
		bb = m_toolhit->WorldToGrid( px, py );
		score = (dx * (int) bb.Y()) + (dx - (int) bb.X());
		break;
	}

	dx = 0;  // so i can set a break point and change score

	return score;
}

void CSheet::SeedsDisplay()
{
#if REQUIRED
	CDbPoint* db_pt;
	CString note;
	CReturn ret;

	for (int idx=0; idx<m_seed_pre->debug_tail(); idx++)
	{
		CSeed& seed = m_seed_pre->debug_seed(idx);

		if (!seed.DebugTouch())
		{
			seed.DebugTouch(true);

			m_model->EntityCreate( DBPOINT, (CDbEntity**)&db_pt );
			db_pt->Init( m_model->ActiveTool(), m_model->ActiveWorkplane(), seed.X(), seed.Y(), 0.0 );
			db_pt->SystemFlag( false );
			db_pt->ColorSet( 0xff0000 + 0x00ff00*(seed.Priority()-1) );

			note.Format( "      (pre %f, %f)", seed.X(), seed.Y() );
			ret.Diagnostic( note );
			if (m_view_mgr != NULL)
				m_view_mgr->Refresh( db_pt->Id(), false );
		}
	}

	for (idx=0; idx<m_seed_post->debug_tail(); idx++)
	{
		CSeed& seed = m_seed_post->debug_seed(idx);

		if (!seed.DebugTouch())
		{
			seed.DebugTouch(true);
			m_model->EntityCreate( DBPOINT, (CDbEntity**)&db_pt );
			db_pt->Init( m_model->ActiveTool(), m_model->ActiveWorkplane(), seed.X(), seed.Y(), 0.0 );
			db_pt->SystemFlag( false );
			db_pt->ColorSet( 0x0000ff + 0x00ff00*(seed.Priority()-1) );

			note.Format( "      (post %f, %f)", seed.X(), seed.Y() );
			ret.Diagnostic( note );
			if (m_view_Mgr != NULL)
				m_view_mgr->Refresh( db_pt->Id(), false );
		}
	}
#endif
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTE: This method may well cause problems.  If so, it should
// be disabled.
//
// HISTORY: This method was introduced as a minor optimization. Its
// intent is to minimize processing for candidate positions where
// the initial part placement is classified as PART_FIT_OFF_SHEET.
// It does this by comparing the calculated shift against the
// amount of part that overhangs the work area.  The candidate
// position is rejected when the calculated shift is not sufficient
// to move the entire part into the work area.
//
// ASSUMPTIONS:The seeds are sorted and processed in a manner
// consistent with the progression. For the sake of discussion,
// assume a progression of PROGRESS_PPY_SPX.  In this case, the
// seeds will be sorted and processed left-to-right & bottom-to-top.
// As such, (and this is the BIG assumption) if the part is "off sheet"
// along the Y-axis & it can not be moved into the work area, then
// there is no need to test further perturbations.
//
// TODO: We could introduce further optimization by tracking failed
// positions against a given part orientation.  A simple look-up
// could then eliminate even the first assessment of "off sheetness".
bool CSheet::OffSheetRejection(
	const CNestConfig&	config,
	C2dBox				part_shift_mer,
	bool				test_vert,
	double*				vert_shift,
	double*				left_shift )
{
	double	delta;
	bool	reject;

	if ( test_vert )
	{
		eNestProgression progression = config.Progression();

		if ((progression == PROGRESS_PPY_SPX) ||
			(progression == PROGRESS_PPX_SPY) )
		{
			delta = part_shift_mer.Ymax() - m_fitzone.Ymax();
		}
		else
		{
			delta = m_fitzone.Ymin() - part_shift_mer.Ymin();
		}

		reject = (delta > (*vert_shift));
	}
	else
	{
		delta = part_shift_mer.Xmax() - m_fitzone.Xmax();

		reject = (delta > (*left_shift));
	}

	if ( reject )
	{
		// NOTE: This code is here only because it
		// simplifies reading higher level code.
		(*vert_shift) = 0.;
		(*left_shift) = 0.;
	}

	return reject;
}

void CSheet::SheetRedraw() const
{
	if (m_view_mgr != NULL)
	{
		m_view_mgr->Clear();
		m_view_mgr->ModelSet( *m_model );
		m_view_mgr->Refresh( false );
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// TODO: DYNATORCH -- Can we make this more clever so that it
// rejects candidate positions when the seed is on-sheet but
// there is insufficient space [eg. ((seed.y + part.dy) > sheet.dy)]?
eSheetRejection CSheet::RejectionScore(
	const CNestConfig&	config,
	const C2dBox&		part_shift_mer,
	int					shift_counter )
{
	eNestProgression	progression;
	eSheetRejection		score;
	
	score = IS_ONSHEET;

	progression = config.Progression();

	if ((progression == PROGRESS_PPY_SPX) || (progression == PROGRESS_PPX_SPY))
	{
		if ( !m_fitzone.Contains( part_shift_mer.BL(), 1.e-3 ) )
			score = ((shift_counter <= 0) ? STARTED_OFFSHEET: SLIPPED_OFFSHEET);
	}
	else
	{
		if ( !m_fitzone.Contains( part_shift_mer.TL(), 1.e-3 ) )
			score = ((shift_counter <= 0) ? STARTED_OFFSHEET: SLIPPED_OFFSHEET);
	}


	return score;
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// TODO: DYNATORCH -- Introduced because dxfdemo6 would be placed
// in bad position.  In particular, RejectionSore() indicated the
// part was on the sheet and TestCore() failed to find an intersection
// between the sheet and part boundaries because the part wrapped
// around the outside of the top-right corner of the sheet.  As such,
// FinalRejectionScore() simply determines whether the entire
// bounding box of the part is on the sheet.
//
// The alternative is to mess with TestCore(). That is not very
// appealing because of its complexity.  It is probably also a
// less efficient solution.
ePartFit CSheet::FinalRejectionScore(
	const C2dCoord&	place,
	const C2dBox&	part_mer )
{
	C2dBox	part_shift_mer;

	// The minimum enclosing rectangle of the part at this location.
	part_shift_mer = part_mer;
	part_shift_mer.Shift( place.X(), place.Y() );

	if ( !m_fitzone.Contains( part_shift_mer.BL(), 1.e-3 ) )
		return PART_FIT_ERROR;

	if ( !m_fitzone.Contains( part_shift_mer.TR(), 1.e-3 ) )
		return PART_FIT_ERROR;

	return PART_FIT;
}

CReturn CSheet::OverlapScore( const CNestConfig& config, CPartPlace* best, CPartPlace* trial ) const
{
	CReturn	status;

	status = OverlapScore( config, best );
	if ( status.IsOk() )
		status = OverlapScore( config, trial );

	if ( status.IsOk() )
	{
		// Force success of next call to HasBetterScore(), as necessary.
		if (trial->OverlapScoreGet() < best->OverlapScoreGet())
			trial->BumpScoreSet( best->BumpScoreGet() );
	}

	return status;
}

CReturn CSheet::OverlapScore( const CNestConfig& config, CPartPlace* pp ) const
{
	CReturn status;

	if (pp->OverlapScoreGet() < SMALL)
	{
		CDexGrid* sheet_grid = ((CSheet*) this)->pGrid();
		CDexGrid* part_grid = pp->ToolHitGet()->Grid();

		part_grid->Debug( CString("_overlap_score.txt") );

		C2dCoord pt = pp->GridPointGet();

		int y_sign = (((config.Progression() == PROGRESS_PPY_SPX) ||
			(config.Progression() == PROGRESS_PPX_SPY)) ? 1 : -1);

		double overlap_score = sheet_grid->OverlapCalc( pt, (*part_grid), y_sign );

		pp->OverlapScoreSet( overlap_score );
	}

	return status;
}

void CSheet::ScoresScrutinize(
	const CNestConfig& config, CPartPlace* best, CPartPlace* candidate )
{
	if ( candidate->HasSameScore( *best ) )
	{
		OverlapScore( config, best, candidate );
	}
	else
	{
		int foo = 0;
	}
}

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// 2006.10.23 (PE) -- Introduced for V18 because Sunflower reported
// cases when the pierce hole would overlap an adjacent part.
//
// This method returns either a simple unadulterated poly or a poly
// representing the outside kerf with pierce hole geometry included
// (when present in the data). It should never return a poly that
// represents a pierce hole by itself.
//
// ASSUMPTIONS:
// 1) The poly at index of zero is always the outside kerf.
// 2) Only poly's representing a pierce hole have attributes.
//
// NOTE: We can revert to previous behavior by simply always returning
// the unadulterated poly.
CGeoPoly* CSheet::PolyGet(
	const CGeoPolyArray&	src_array,
	int						depth,
	int						poly_indx )
{
	CGeoPoly* result = NULL;

	CGeoPoly* src_poly = src_array[poly_indx];

	if (depth == HIT_NEST_OUTSIDE)
	{
		const CGeoCurve& geoCurve = (*src_poly)[0];
		if ( !geoCurve.HasAttrib() || (geoCurve.IntGet( "lead", 0 ) == 0))
		{
			// Simply copy the poly.
			result = new CGeoPoly();
			(*result) = (*src_poly);
		}

		if (poly_indx == 0)
		{
			// And append any pierce hole geometry that we may encounter.
			int poly_cnt = src_array.Count();
			for (int poly_poly_indx = 1; poly_poly_indx < poly_cnt; ++poly_poly_indx)
			{
				CGeoPoly* pierce_poly = src_array[poly_poly_indx];
				
				int pierce_cnt = pierce_poly->Count();
				for (int pierce_poly_indx = 0; pierce_poly_indx < pierce_cnt; ++pierce_poly_indx)
				{
					const CGeoCurve& geoCurve = (*pierce_poly)[pierce_poly_indx];
					if ( !geoCurve.HasAttrib() || (geoCurve.IntGet( "lead", 0 ) == 0))
						break;

					result->CopyAppend( geoCurve );
				}
			}
		}
	}
	else
	{
		// Simply copy the poly.
		result = (CGeoPoly*) src_poly->Clone( true );
	}

	return result;
}

int CSheet::NestedAreasCount() const
{
	return ( m_nested_areas.Count() );
}

CNestedArea* CSheet::NestedAreaGet( int indx ) const
{
	return ( m_nested_areas.GetAt( indx ) );
}

// Creates m_toolhit and its grid representation.
CReturn CSheet::MaterialProcess( 
	const CNestConfig&	config,
	const C2dBox&		extents,
	bool				raw_material )
{
	CReturn		status;
	CPartPlace	part_place;
	CGrid		grid;
	CDbIterator	iter;
	CDbProfile*	dbProfile;
	int			indx;

	ToolHitDelete();

	if ( config.GridNest() )
	{
		status = grid.Create( extents, C2dCoord( 0, 0 ), config.Resolution(), 0. );

		if ( raw_material )
			grid.Border( 1 );

		grid.Debug("sheet.txt");

		// Slice the sheet/remnant for nesting.
		if (status.isOkay())
		{
			indx = 0;
			iter.Init( m_model->Db(), DBPROFILE );
			while ( true )
			{
				dbProfile = dynamic_cast<CDbProfile*>( iter() );
				if (dbProfile == NULL)
					break;

				// 2007.12.30 (PE) -- It appears that we get here (now)
				// only when we are nesting on a remnant.
				// Of course, we could reinforce this using "if ( IsRemnant() )"
				//    status += grid.Slice( *dbProfile );

				// ASSUMPTION: The zeroth profile represents the sheet boundary.
				status += grid.Slice( *dbProfile, MARK_BORDER );
				status += grid.Fill( config, MARK_FILL_OKERF );
				break;
			}
		}

		if ( !raw_material )
			status += grid.Invert();


		if ( status.IsOk() )
			m_toolhit = build_toolhit( &grid );
		else
			m_toolhit = build_toolhit( NULL );


		// Enter the sheet/remnant outline into the part/kerf geo
		if ( status.IsOk() )
		{
			CGeoCurve*	geoCurve;

			// Get the pointer to the empty storage areas that were
			// just created by the preceding call to build_toolhit().
			CGeoPoly* nest_outside = m_toolhit->PolysGet( HIT_NEST_OUTSIDE )->GetAt(0);
			CGeoPoly* part_outside = m_toolhit->PolysGet( HIT_PART_OUTSIDE )->GetAt(0);

			CGeoPoly* part_inside = m_toolhit->PolysGet( HIT_PART_INSIDE )->GetAt(0);
			CGeoPoly* nest_inside = m_toolhit->PolysGet( HIT_NEST_INSIDE )->GetAt(0);

			// Dunno why but these too are necessary.
			CGeoPoly* part_poly = ReservePart( RESERVE_REMNANT );
			CGeoPoly* kerf_poly = ReserveKerf( RESERVE_REMNANT );

			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// Since we treat the material as if it were the nestable
			// area within a part, we must adjust the boundaries to
			// reflect this behavior.

			if (IsRemnant() && !config.PreNest())
			{
				CGeoPoly		poly;
				CDbWorkplane*	dbWork;
				int				dir;

				m_model->EntityFind( STR_WORLD, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

				// ASSUMPTION: The zeroth profile represents the sheet boundary.
				indx = 0;

				// This block is simply an experiment!!!!!
				iter.Init( m_model->Db(), DBPROFILE );
				while ( true )
				{
					dbProfile = dynamic_cast<CDbProfile*>( iter() );
					if (dbProfile == NULL)
						break;

					if (indx == 0)
					{
						CConversion::Convert( dbWork, dbProfile, &m_material_poly );

						nest_inside->CopyAppend( m_material_poly );
						part_inside->CopyAppend( m_material_poly );
						part_outside->CopyAppend( m_material_poly );
						nest_outside->CopyAppend( m_material_poly );

						dir = m_material_poly.Winding();
						if (dir > 0)
						{
							// Otherwise, CTrueRemnant::PartsRemove() yields incorrect results.
							m_material_poly.Reverse();

							// 2007.12.31 (PE) -- At minimum, must reverse part_inside. Otherwise,
							// the call to sheet->SeedPoly() (via CNestMgr::nest_area()) will fail.
							part_inside->Reverse();
							nest_inside->Reverse();
						}

						// We have deferred creating the 'nested area' object till
						// this point because the ctor copied poly data from m_toolhit.
						part_place.Init( C2dCoord(0,0), C2dCoord(0,0), m_toolhit );
						m_nested_area = new CNestedArea( part_place );
					}
					else
					{
						CGeoPoly cutout_poly;
						C2dBox cutout_extents;
						CToolHit* toolhit;
						CNestedArea* cutout_area;

						CConversion::Convert( dbWork, dbProfile, &cutout_poly );

						cutout_extents = cutout_poly.Extent();

						status = grid.Create( cutout_extents, C2dCoord( 0, 0 ), config.Resolution(), 0. );

						status += grid.Slice( cutout_poly, MARK_FILL_BODY );
						status += grid.Fill( config, MARK_FILL_BODY );
						
						toolhit = build_toolhit( &grid );

						// Get the pointer to the empty storage areas that were
						// just created by the preceding call to build_toolhit().
						// NOTE: We can ignore HIT_PART_INSIDE & HIT_NEST_INSIDE
						// because we can not nest within a hole in the remnant.
						part_outside = toolhit->PolysGet( HIT_PART_OUTSIDE )->GetAt(0);
						nest_outside = toolhit->PolysGet( HIT_NEST_OUTSIDE )->GetAt(0);

						dir = cutout_poly.Winding();

						// The polys must have a CCW winding. Otherwise, CSheet::Bump()
						// fails to use the appropriate pieces of the poly.
						if (dir < 0)
							cutout_poly.Reverse();

						part_outside->CopyAppend( cutout_poly );
						{
							CGeoPolyArray offset_polys;
							CGeoPoly* nest_poly;

							// We must generate the HIT_NEST_OUTSIDE boundary
							// so as to have curves that yield appropriate seeds.
							// Alternately, we could just saved these curves in
							// the remnant file but then 1) the end-user would see
							// them and 2) he could mess things up by editing them.
							CWorm::PolyOffset( cutout_poly, RIGHT,
								config.Spacing(), HALFPI, &offset_polys );

							// In case CWorm::PolyOffset() fails in some way ....
							nest_poly = &cutout_poly;
							if (offset_polys.Count() == 1)
								nest_poly = offset_polys.GetAt(0);

							nest_outside->CopyAppend( (*nest_poly) );

							// CRITICAL: To CNestMgr::nest_area() wrt generating seeds!
							nest_outside->IntSet( "remnant", 1 );

							offset_polys.DestructiveFlush();
						}

						// CRITICAL: to CTrueRemnant::PartsRemove()
						CGeoPoly* kerf_outside = (CGeoPoly*) cutout_poly.Clone( true );

						toolhit->PolysGet( HIT_KERF_OUTSIDE )->Append( kerf_outside );  

						// We have deferred creating the 'nested area' object till
						// this point because the ctor copied poly data from m_toolhit.
						C2dCoord seed = m_toolhit->WorldToGrid( cutout_extents.Xmin(), cutout_extents.Ymin() );
						part_place.Init( C2dCoord(0,0), seed, toolhit );
						cutout_area = new CNestedArea( part_place );

						m_nested_area->InteriorAreaAdd( cutout_area );

						// Transfer the bitmap of the part to the sheet.
						Scan( config, grid, part_place.GridPointGet(), *m_view_mgr );

						poly.Flush();
					}

					++indx;
					iter.Next();
				}
			}
			else
			{
				CProfile	material_boundary;
				CProfile	material_inside;
				CProfile	material_outside;

				StockBoundariesCreate( config,
					&material_boundary, &material_inside, &material_outside );

				m_material_poly.Flush();
				for (indx = 0; indx < 4; ++indx)
				{
					geoCurve = material_boundary.GetAt(indx);
					m_material_poly.CopyAppend( *geoCurve );

					geoCurve = material_inside.GetAt(indx);
					nest_inside->CopyAppend( *geoCurve );

					geoCurve = material_outside.GetAt(indx);
					part_inside->CopyAppend( *geoCurve );
					part_outside->CopyAppend( *geoCurve );
					nest_outside->CopyAppend( *geoCurve );
				}

				// Is this really necessary?
				part_poly->Reverse();

				// We have deferred creating the 'nested area' object till
				// this point because the ctor copied poly data from m_toolhit.
				part_place.Init( C2dCoord(0,0), C2dCoord(0,0), m_toolhit );

				delete m_nested_area;
				m_nested_area = new CNestedArea( part_place );
			}
		}
	}

	return status;
}

void CSheet::ToolHitDelete()
{
	if (m_toolhit)
	{
		delete m_toolhit;
		m_toolhit = NULL;
	}
}

// Prior to V17, remnant geometry was created on the world workplane.
// And though this worked wonderfully, it prevented you from selecting
// remnant geomtry (by box) and likewise you could not edit (add entities
// to) the remnant because the application works with TOP and not WORLD.
//
// In a feeble attempt to to get the nesting engine to nest on remnants,
// we no simply move (lie about) the remnant geometry back to WORLD.
// It appears that we do not have to transform the geometry because its
// (xmin,ymin) is shifted to the origin regarless..
//
// See also CTrueRemnant:Extract().
void CSheet::RemnantWorkplaneSet()
{
	CDbIterator		iter;
	CDbTool*		remnant_layer;
	CDbWorkplane*	dbWork;
	CDbEntity*		dbEntity;

	m_model->EntityFind( "Remnant", (CDbEntity**) &remnant_layer, DBTOOL, DBTOOL );

	// Default workplane.
	m_model->EntityFind( STR_WORLD, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );
	m_model->ActiveWorkplane( dbWork );

	iter.Init( m_model->Db(), DBPOINT );
	while (1)
	{
		dbEntity = iter();
		if (dbEntity == NULL)
			break;

		if (dbEntity->Tool() == remnant_layer)
			dbEntity->Workplane( dbWork );

		iter.Next();
	}
}

CGeoPoly* CSheet::ReservePart( eReservePoly num )
{
	CGeoPolyArray* poly_array = m_toolhit->PolysGet(HIT_PART_OUTSIDE);
	return poly_array->GetAt(num);
}

CGeoPoly* CSheet::ReserveKerf( eReservePoly num )
{
	CGeoPolyArray* poly_array = m_toolhit->PolysGet(HIT_NEST_OUTSIDE);
	return poly_array->GetAt(num);
}

// REQUIRES: chop must be sorted prior to calling.
void CSheet::ChopListReduce( CDoubleArray* chop ) const
{
	int count = chop->Count();
	if (count > 1)
	{
		double curr, next;
		int indxA, indxB;

		// Compact the data.
		indxA = 0;
		curr = chop->GetAt( indxA );
		for (indxB = 1; indxB < count; ++indxB)
		{
			next = chop->GetAt( indxB );
			if ( !CLOSE( curr, next, SMALL ) )
			{
				++indxA;
				if (indxA < indxB)
					chop->Replace( indxA, next );

				curr = next;
			}
		}

		// Truncate the list.
		--indxB;
		while (indxB > indxA)
		{
			chop->Remove( indxB );
			--indxB;
		}
	}
}

void CSheet::StockBoundariesCreate(
	const CNestConfig&	config,
	CProfile*			material_boundary,
	CProfile*			material_inside,
	CProfile*			material_outside )
{
	CGeoLine* geoLine = NULL;

	double xmin = m_extent.Xmin();
	double ymin = m_extent.Ymin();
	double xmax = m_extent.Xmax();
	double ymax = m_extent.Ymax();

	double lbor = 0.;
	double rbor = 0.;
	double tbor = 0.;
	double bbor = 0.;

	double delta = 0.;

	if (1)  // config.PreNest() )
	{
		lbor = config.Border( BORDER_LEFT );
		rbor = config.Border( BORDER_RIGHT );
		tbor = config.Border( BORDER_TOP );
		bbor = config.Border( BORDER_BOTTOM );

		delta = config.Spacing();
	}

	//=-=-=-=-=-=-=

	geoLine = new CGeoLine(
		(xmin), (ymin), 0.,
		(xmin), (ymax), 0. );

	material_boundary->Append( geoLine );

	geoLine = new CGeoLine(
		(xmin), (ymax), 0.,
		(xmax), (ymax), 0. );

	material_boundary->Append( geoLine );

	geoLine = new CGeoLine(
		(xmax), (ymax), 0.,
		(xmax), (ymin), 0. );

	material_boundary->Append( geoLine );

	geoLine = new CGeoLine(
		(xmax), (ymin), 0.,
		(xmin), (ymin), 0. );

	material_boundary->Append( geoLine );

	//=-=-=-=-=-=-=

	geoLine = new CGeoLine(
		(xmin + lbor), (ymin + bbor), 0.,
		(xmin + lbor), (ymax - tbor), 0. );

	material_inside->Append( geoLine );

	geoLine = new CGeoLine(
		(xmin + lbor), (ymax - tbor), 0.,
		(xmax - rbor), (ymax - tbor), 0. );

	material_inside->Append( geoLine );

	geoLine = new CGeoLine(
		(xmax - rbor), (ymax - tbor), 0.,
		(xmax - rbor), (ymin + bbor), 0. );

	material_inside->Append( geoLine );

	geoLine = new CGeoLine(
		(xmax - rbor), (ymin + bbor), 0.,
		(xmin + lbor), (ymin + bbor), 0. );

	material_inside->Append( geoLine );

	//=-=-=-=-=-=-=

	geoLine = new CGeoLine(
		(xmin + lbor - delta), (ymin + bbor - delta), 0.,
		(xmin + lbor - delta), (ymax - tbor + delta), 0. );

	material_outside->Append( geoLine );

	geoLine = new CGeoLine(
		(xmin + lbor - delta), (ymax - tbor + delta), 0.,
		(xmax - rbor + delta), (ymax - tbor + delta), 0. );

	material_outside->Append( geoLine );

	geoLine = new CGeoLine(
		(xmax - rbor + delta), (ymax - tbor + delta), 0.,
		(xmax - rbor + delta), (ymin + bbor - delta), 0. );

	material_outside->Append( geoLine );

	geoLine = new CGeoLine(
		(xmax - rbor + delta), (ymin + bbor - delta), 0.,
		(xmin + lbor - delta), (ymin + bbor - delta), 0. );

	material_outside->Append( geoLine );
}

// 2007.09.16 (PE) -- Introduced with V19.
// There are cases where the initial placement of a part (in the
// original case, a triangular part) is slightly off the sheet.
// Under some conditions, it is possible to salvage the seed
// point by nudging the part onto the sheet.
void CSheet::BorderShift(
	const CNestConfig&	config,
	const C2dBox&		fitzone,
	const C2dBox&		part_shift_mer,
	double*				hshift,
	double*				vshift )
{
	double	dx, dy;
	int		sign_y;

	(*vshift) = 0.;
	(*hshift) = 0.;

	dx = part_shift_mer.Xmax() - fitzone.Xmax();
	if (dx > SMALL)
	{
		(*hshift) = dx;
	}
	else
	{
		dx = part_shift_mer.Xmin() - fitzone.Xmin();
		if (dx < SMALL)
		{
			// 2007.12.17 (PE)
			// TODO: This should never happen ... but it is for bitmap nesting.
			(*hshift) = dx;
		}
	}

	// (-1) nesting up the Y-axis / (+1) nesting down the Y-axis
	// These values may seem reversed but they are used in
	// determining which way to scan/shift the part. It's
	// probably best to leave this alone.
	sign_y = (config.NestPosY() ? -1 : +1);

	// 2007.12.15 (PE) -- Modified in an attempt to help bitmap nesting.
	// I don't think the precedence rules here are necessary.
	if (sign_y < 0)
	{
		dy = part_shift_mer.Ymax() - fitzone.Ymax();
		if (dy > SMALL)
		{
			(*vshift) = dy;
		}
		else
		{
			dy = part_shift_mer.Ymin() - fitzone.Ymin();
			if (dy < SMALL)
				(*vshift) = dy;
		}
	}
	else
	{
		dy = part_shift_mer.Ymin() - fitzone.Ymin();
		if (dy < SMALL)
		{
			(*vshift) = -dy;
		}
		else
		{
			dy = part_shift_mer.Ymax() - fitzone.Ymax();
			if (dy > SMALL)
				(*vshift) = -dy;
		}
	}
}

ePartFit CSheet::TrivialFitStatus( const C2dBox& fitzone, const C2dBox& part_shift_mer )
{
	double TOL = 1.e-3;  // arbitrary
	double delta;

	delta = part_shift_mer.Xmin() - fitzone.Xmax();
	if (delta > -TOL)
		return PART_FIT_ERROR;

	delta = part_shift_mer.Ymin() - fitzone.Ymax();
	if (delta > -TOL)
		return PART_FIT_ERROR;

	if ( !fitzone.Contains( part_shift_mer, TOL ) )
		return PART_FIT_OFF_SHEET;

	return PART_FIT;
}

void CSheet::PartPlaceSet( const CPartPlace& part_place )
{
	CPartPlace* pp = PartPlaceGet();
	(*pp) = part_place;
}

// NOTE: Something is very wrong if the stack is empty!!!
CPartPlace* CSheet::PartPlaceGet() const
{
	int top = m_part_places.Count() - 1;
	return ((top >= 0) ? m_part_places.GetAt( top ) : NULL);
}

// 
void CSheet::PartPlacePop()
{
	int top = m_part_places.Count() - 1;
	if (top >= 0)
		delete m_part_places.Remove( top );
}

void CSheet::PartPlacePush()
{
	CPartPlace* pp = new CPartPlace();
	m_part_places.Append( pp );
}

// TODO: Create a Draw() method for CGeoPolyArray.
void CSheet::Render(
	const CGeoPolyArray&	outside_poly,
	const C3dCoord&			place,
	int						redraw_sheet )
{
	if (m_view_mgr != NULL)
	{
		int	count, indx;

		CViewBase* vbase = m_view_mgr->ActiveView();

		if ( redraw_sheet )
			vbase->Refresh( false );

		vbase->DisplayListBegin( PTEMP_LIST );
		vbase->DrawAtColor( DCOLOR_RED );

		// And why would we have more than one?
		// Because the 2nd one *should* be the pierce hole.
		count = outside_poly.Count();
		for (indx = 0; indx < count; ++indx)
		{
			CGeoPoly* poly = outside_poly.GetAt( indx );
			vbase->DrawGeoAt( (*poly), place );
		}

		vbase->DisplayListEnd( PTEMP_LIST );
		vbase->BufferShow( FRONT_BUFFER );  // BACK_BUFFER );
	}
}

void CSheet::FitZoneGet(
		const CNestConfig&	config,
		bool				is_large_part,
		C2dBox*				fitzone )
{
	if ( is_large_part )
	{
		double xmin = max( config.Border(BORDER_LEFT), this->Extent().Xmin() );
		double xmax = min( this->Length() - config.Border(BORDER_RIGHT), this->Extent().Xmax() );

		double ymin = max( config.Border(BORDER_BOTTOM), this->Extent().Ymin() );
		double ymax = min( this->Width() - config.Border(BORDER_TOP), this->Extent().Ymax() );

		if ( CNestConfig::IsBitmapNest() )
		{
			xmin -= config.Resolution();
			ymin -= config.Resolution();
			xmax += config.Resolution();
			ymax += config.Resolution();
		}
		else
		{
			xmin -= config.Spacing();
			ymin -= config.Spacing();
			xmax += config.Spacing();
			ymax += config.Spacing();
		}

		fitzone->Update( xmin, ymin, xmax, ymax );
	}
	else
	{
		(*fitzone) = m_fitzone;
	}
}

// 2008.02.03 (PE) -- The seed for initial part placement is frequently
// derived from the primary corners of and instance of the same part
// (sometimes having a different orientation). Additionally, the part
// from which the seeds were derived may have been nested quite nicely,
// in a manner that causes the initial position of the next part to
// overlap some pierce hole. Note, we only consider this condition when
// HardPierce() returns true. And under this condition, we simply try
// to nudge the initial placement off of the pierce hole.
// 
void CSheet::TestPierceOverlap(
	const C3dCoord&		shift,
	const CToolHit&		candidate_toolhit,
	const CNestedArea&	area_to_nest,
	double*				left_shift,
	double*				vert_shift )
{
	double lshift, vshift;
	
	(*left_shift) = 0.;
	(*vert_shift) = 0.;

	// Check against outside-kerf polys of all parts nested within this area.
	int count = area_to_nest.InteriorAreasCount();
	for (int indx = 0; indx < count; ++indx)
	{
		CNestedArea* nested_area = area_to_nest.InteriorAreaGet( indx );
		CToolHit* nested_toolhit = nested_area->ToolHitGet();

		check_pierce_overlap( shift, (*nested_toolhit), candidate_toolhit,
			&lshift, &vshift );

		if (!ZERO(lshift) && (fabs(lshift) > fabs(*left_shift)))
			(*left_shift) = lshift;

		if (!ZERO(vshift) && (fabs(vshift) > fabs(*vert_shift)))
			(*vert_shift) = vshift;
	}
}

// NOTE: If left_shift is returned as a non-zero value, then it is
// required to be a negative value (because of how CSheet::exact_score()
// uses it to shift the part).
void CSheet::check_pierce_overlap(
	const C3dCoord&		shift,
	const CToolHit&		nested_toolhit,
	const CToolHit&		candidate_toolhit,
	double*				left_shift,
	double*				vert_shift )
{
	double	lshiftA, vshiftA;
	double	lshiftB, vshiftB;
	double	xshift = shift.X();
	double	yshift = shift.Y();
	
	(*left_shift) = 0.;
	(*vert_shift) = 0.;

	// Test the candidate against the nested part.
	check_pierce_overlap_core( xshift, yshift, nested_toolhit,
		candidate_toolhit, &lshiftA, &vshiftA );

	// And visa versa.
	check_pierce_overlap_core( -xshift, -yshift, candidate_toolhit,
		nested_toolhit, &lshiftB, &vshiftB );

	lshiftB = -lshiftB;
	vshiftB = -vshiftB;

	if (!ZERO(lshiftA) || !ZERO(lshiftB))
		(*left_shift) = ((fabs(lshiftA) > fabs(lshiftB)) ? lshiftA : lshiftB);

	if (!ZERO(vshiftA) || !ZERO(vshiftB))
		(*vert_shift) = ((fabs(vshiftA) > fabs(vshiftB)) ? vshiftA : vshiftB);
}

void CSheet::check_pierce_overlap_core(
	double				xshift,
	double				yshift,
	const CToolHit&		toolhitA,
	const CToolHit&		toolhitB,
	double*				left_shift,
	double*				vert_shift )
{
	CGeoPoly*	part_poly;
	
	(*left_shift) = 0.;
	(*vert_shift) = 0.;

	double radius = toolhitA.DoubleGet( "radius", UNDEFINED );
	if (radius < UNDEFINED)
	{
		C2dBox	extent;
		double	xc, yc;

		// The center of the pierce hole in the coordinate system
		// of the untranslated part poly. Shifting the point is
		// *much* cheaper than copying and shifting the poly.
		xc = toolhitA.DoubleGet( "xc", UNDEFINED );
		yc = toolhitA.DoubleGet( "yc", UNDEFINED );

		part_poly = toolhitB.PolysGet(HIT_NEST_OUTSIDE)->GetAt(0);
		
		extent = part_poly->Extent();
		extent.Shift( xshift, yshift );

		// Trivial elimination.
		if ( extent.Contains( xc, yc, radius ) )
		{
			C3dCoord	pc;
			C3dCoord	closest;
			C2dUnitVec	vec;
			double		dist, dx, dy;
			bool		pt_in_poly;

			pc.XYZ( (xc - xshift), (yc - yshift), 0. );

			dist = part_poly->PointClosest( pc, &closest );
			pt_in_poly = part_poly->PtInPoly( pc );
			if ((dist < radius) || pt_in_poly)
			{
				// Whereas PtInPoly() is conclusive, (dist < radius) simply
				// indicates proximity (pt could be either inside or outside).

				dx = pc.X() - closest.X();
				dy = pc.Y() - closest.Y();
				if ( pt_in_poly )
				{
					dist += radius;
				}
				else
				{
					dx = -dx;
					dy = -dy;
					dist = radius - dist;
				}

				vec.Init( dx, dy );
				(*left_shift) = dist* vec.X();
				(*vert_shift) = dist * vec.Y();
			}
		}
	}
}

double CSheet::evaluate(
	eSheetProc			process,
	int					prim_axis,
	const CGeoCurve&	sheet_curve,
	const CGeoCurve&	part_curve,
	double				curr_ord,
	double				next_ord,
	double				sign ) const
{
	double	result = DBL_MAX;
	double	area, dist;
	bool	parallel;

	switch (process)
	{
	case SPROC_SCORE:
		
		area = ent_area_xy( prim_axis, sheet_curve, part_curve, curr_ord, next_ord );
		result = area;
		break;

	case SPROC_BUMP:

		dist = ent_dist_xy( prim_axis, sheet_curve, part_curve,
			curr_ord, next_ord, sign, &parallel );

		result = ((fabs(dist) < CLOSED_ENOUGH) ? 0. : dist);
		break;
	}

	return result;
}

void CSheet::ShiftValidate( double* shift )
{
	if (EQUAL((*shift), DBL_MAX) || (fabs((*shift)) < 1.e-3))
		(*shift) = 0.;
}

// 2008.03.25 (PE) -- PierceHoleAvoid() is a recent addition to the
// nesting algorithm and results from a great deal of experimentation.
// Neglecting pierce holes, PierceHoleAvoid() assumes the initial part
// place is just peachy. If the candidate part is found to overlap a
// pierce hole then an attempt is made to move the part off of the
// pierce hole, while keeping the part off of other parts but still
// on the sheet.
//
// NOTE: This is potentially very problematic but it is, at least,
// restricted to the HardPiece() case.
ePartFit CSheet::PierceHoleAvoid(
	const CNestConfig&	config,
	const CNestedArea&	area_to_nest,
	const CToolHit&		toolhit,
	C3dCoord*			place )  // in WORLD...
{
	static double SMALL_SHIFT = 1.e-3;  // arbitrarily tolerance.

	C3dCoordArray	pts;
	C2dBox			fitzone;
	C2dBox			part_shift_mer;
	double			left_shift, vert_shift;
	int				count, indx;
	bool			special_bump;

	m_ignore_lead = false;

	ePartFit fit = PART_FIT;

	// In case we can iterate to a suitable location.
	C3dCoord original_place = (*place);

	CGeoPolyArray* outside_poly = toolhit.PolysGet(HIT_NEST_OUTSIDE);

	FitZoneGet( config, toolhit.Part()->Large(), &fitzone );

	C2dBox part_mer = toolhit.Mer();
	double max_bump = 0.5 * part_mer.Dy();

	C3dCoord test = (*place);

	bool done = false;
	while ( !done )
	{
		TestPierceOverlap( test, toolhit, area_to_nest,
			&left_shift, &vert_shift );

		special_bump = ((config.NestPosY() && (vert_shift > 0.)) ||
			(!config.NestPosY() && (vert_shift < 0.)));

		// Eliminate obvious (?) cases where vertically shifting the part
		// causes it to overlap an adjacent part. This solution may not be
		// 100% effective and it may even cause problems. There is no simple
		// way to verbally describe the geometric configuration of this
		// scenario, so the following hieroglyphics are an attempt to
		// illustrate the scenario.
		//
		//
		//  Nesting in +Y   Nesting in -Y
		//
		//    BBBB CC         AAAAAAAAA    1) A & B are already nested
		//    BBBpCCC         AAAAAAAAA    2) 'p' is the pierce hole of B
		//    BB CCCC         AAAAAAAAA    3) B is used to seed C
		//    AAAAAAAAA       BB CCCC      4) shift C away from B causes
		//    AAAAAAAAA       BBBpCCC         C to overlap A.
		//    AAAAAAAAA       BBBB CC
		//
		// In essence, forcing vert_shift to be zero restricts the iterations
		// to be along the X-axis.
		if ((config.NestPosY() && (vert_shift < 0.)) ||
			(!config.NestPosY() && (vert_shift > 0.)))
				vert_shift = 0.;

		// Another problematic situation placing C (because of (c))
		//
		//    AAAAAAAAA BBBBBBBB
		//    AAAAAAAAAa BBBBBBB
		//    AAAAAAAAACC BBBBBB
		//    AAAAAAAAACCC BBBBBb
		//    AAAAAAAAACCCCcBBBB
		//    AAAAAAAAACCCCC BBB
		//    AAAAAAAAACCCCCC BB
		//    
		if (left_shift < 0.)
			left_shift = 0.;

		if ((fabs(left_shift) > SMALL_SHIFT) || (fabs(vert_shift) > SMALL_SHIFT))
		{
			// Move the part off of the pierce hole.
			test.X( test.X() + left_shift );
			test.Y( test.Y() + vert_shift );
			
			if ( config.DebugWire() )
				Render( (*outside_poly), test, DBG6 );

			// The minimum enclosing rectangle of the part at this location.
			part_shift_mer = part_mer;
			part_shift_mer.Shift( test.X(), test.Y() );

			// Make sure the part is still on the sheet.
			fit = TrivialFitStatus( fitzone, part_shift_mer );
			if (fit == PART_FIT_OFF_SHEET)
			{
				// Attempt to salvage this seed point under conditions
				// where the part is slightly off of the sheet.
				BorderShift( config, fitzone, part_shift_mer,
					&left_shift, &vert_shift );

				// Adjust the placement of the part.
				test.X( test.X() - left_shift );
				test.Y( test.Y() + (config.NestPosY() ? -vert_shift : vert_shift) );
			}

			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// Assume the candidate part overlaps another part. Move the
			// candidate part off of the part and then back in the direction
			// from whence it came.
			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

			// Move off of the part.
			vert_shift = Bump( test, toolhit, area_to_nest,
				BUMP_Y, (config.NestPosY() ? max_bump : -max_bump) );
			ShiftValidate( &vert_shift );

			if (fabs(vert_shift) > SMALL)
			{
				test.Y( test.Y() + (config.NestPosY() ? vert_shift : -vert_shift) );
				
				if ( config.DebugWire() )
					Render( (*outside_poly), test, DBG6 );

				// Move back.
				m_pierce_bump = special_bump;

				vert_shift = Bump( test, toolhit, area_to_nest,
					BUMP_Y, (config.NestPosY() ? -max_bump : max_bump) );
				
				m_pierce_bump = false;

				ShiftValidate( &vert_shift );

				test.Y( test.Y() + (config.NestPosY() ? -vert_shift : vert_shift) );
				
				if ( config.DebugWire() )
					Render( (*outside_poly), test, DBG6 );
			}

			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			// Exit the loop if we find oscillations in the part position.
			//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
			
			count = pts.Count();
			for (indx = 0; indx < count; ++indx)
			{
				if ( pts.GetAt(indx)->WithinTolXY( test, SMALL_SHIFT ) )
				{
					if (fit == PART_FIT_OFF_SHEET)
						test = original_place;

					break;
				}
			}

			done = (indx < count);
			if ( !done )
				pts.Append( new C3dCoord( (*place) ) );
		}
		else
		{
			done = true;
		}
		
		if ( config.DebugWire() )
			Render( (*outside_poly), (*place), DBG6 );
	}

	pts.DestructiveFlush();

	m_ignore_lead = true;

	(*place) = test;

	return fit;
}

void CSheet::ToolsCopy( CModel& from, CModel* to )
{
	CDbIterator iter;
	CDbTool*	new_tool;
	CDbWorkplane* dbWork;

	to->EntityFind( STR_TOP, (CDbEntity**) &dbWork, DBWORKPLANE, DBWORKPLANE );

	from.EntityPrepareCopy( to );
	iter.Init( from.Db(), DBTOOL );
	while (true)
	{
		const CDbTool* tool = dynamic_cast<const CDbTool*>( iter() );
		if (!tool)
			break;

		from.EntityCopy( (CDbEntity&)(*tool), (CDbEntity**) &new_tool );

		if (!new_tool->Workplane())
			new_tool->Workplane( dbWork );

		iter.Next();
	}
}


#define REPO_ZONE_NAME	"_repo_zone_%d"

// Pilfered from CRepo::PackageInstances().
CDbFeature* CSheet::CutbackWorkzoneCreate( CDbLine* cutback_line )
{
	CDbFeature* dbZone = NULL;

	CDbFeature* max_workzone = MaxWorkzoneGet();
	if (max_workzone != NULL)
	{
		int zidx = max_workzone->IntGet( "_zone_num", 0 ) + 1;

		m_model->EntityCreate( DBFEATURE, (CDbEntity**) &dbZone );

		CString name;
		name.Format( REPO_ZONE_NAME, zidx );
		name = dbZone->NameConvert( name );
		dbZone->SystemName( name );

		// Zone numbers are [1..N] to make VB-side easier.
		dbZone->Owner( NULL );
		dbZone->IntSet( "_zone_num", zidx );

		//
		double left = cutback_line->StartPt().X();
		double top = max_workzone->DoubleGet( "_zone_top", 0. );
		double right = Length();  // of this sheet
		double bottom = max_workzone->DoubleGet( "_zone_bottom", 0. );

		//	Some attributes...
		dbZone->StringSet( STR_TYPE, "_zone" );
		dbZone->DoubleSet( "_zone_left", left );
		dbZone->DoubleSet( "_zone_top", top );
		dbZone->DoubleSet( "_zone_right", right );
		dbZone->DoubleSet( "_zone_bottom", bottom );

		dbZone->Append( cutback_line );
	}

	return dbZone;
}

// In support of CutbackWorkzoneCreate()
// Ugh. Wish there was a better way ....
CDbFeature* CSheet::MaxWorkzoneGet()
{
	CDbIterator	iter;
	CDbFeature*	max_workzone = NULL;
	int			max_index = -1;

	iter.Init( m_model->Db(), DBFEATURE );
	while (1)
	{
		CDbFeature* dbFeature = dynamic_cast<CDbFeature*>( iter() );
		if (dbFeature == NULL)
			break;

		if ( dbFeature->IsWorkZone() )
		{
			int index = dbFeature->IntGet( "_zone_num", -1 );
			if (index > max_index)
			{
				max_workzone = dbFeature;
				max_index = index;
			}
		}

		iter.Next();
	}

	return max_workzone;
}

// Delete the lead-hull profiles that were created by LeadHullCreate()
// for the purposes of simplifying part placement during nesting.
void CSheet::LeadHullsDelete()
{
	CDbIterator iter;

	iter.Init( m_model->Db(), DBPROFILE );
	while (1)
	{
		CDbProfile* dbProfile = dynamic_cast<CDbProfile*>( iter() );
		if (dbProfile == NULL)
			break;

		if ( dbProfile->IsLeadHull() )
			dbProfile->Delete();

		iter.Next();
	}
}
