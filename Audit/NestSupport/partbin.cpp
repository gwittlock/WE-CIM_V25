// ==================================================================
//		PartBin
//
// ==================================================================

#include "stdafx.h"
#include "Path.h"
#include "DbIterator.h"
#include "DbTool.h"
#include "DbLine.h"
#include "FileSnoop.h"

#include "DaoQuery.h"
#include "DaoAttrib.h"

#if (_CI || _NST)
#include "Register.h"
#include "Portal.h"
#include "CiCurve.h"
#include "CiProfile.h"
#include "CiModelConvertor.h"
#include "CiProfileMgr.h"
#include "CiModelIterator.h"
#include "ModelText.h"
#endif

#if (_PM4)
#include "ScModel.h"
#include "ScModelConvertor.h"
#endif

#include "PartBin.h"

#include "MM2.h"

// Thank the Lord for people who share their hard work!
#include "ProgressWnd.h"

// ==================================================================
// TODO:  Do I really want these here?  I don't really *need*
//	persistant stack and tree for the part...
//
static CSelectorStack		dummy_stack;
static CTreeViewSupport		dummy_tree;

// ==================================================================

CPartBin::CPartBin()
{
}

CPartBin::~CPartBin()
{
	m_parts.DestructiveFlush();
}


// ==================================================================


CReturn CPartBin::Load( const CNestConfig& config )
{
	CReturn status;

	Database( config.PdbPath() );

#if (_CI || _NST)
	status += load_ci_names( config.PdbPath(), 1 );
#else
	status += load_names( config.PdbPath(), 1 );
#endif

	if (status.isOkay())
		status += load_parts( config );

	return status;
}

// ==================================================================
//	Load the names (and other stats) of all our parts from the database.
//
CReturn CPartBin::load_names( const CString& in_filename, int factor )
{
	CReturn		ret;
	CDaoDB		part_db;
	CPath		path;
	CNestingPart*	npart;
	CString		name;
	CString		type;
	int			qty, is_filler;

	int part_tab = part_db.addTable( "Part" );

	ret += part_db.Open( in_filename ); 
	int idx = 0;
	if (ret.isOkay())
	{
		part_db.moveRecord( part_tab, 0 );
		while (1)
		{
			// Must do it this way because, simply, Microsoft sucks
			// mucas from a dead cat's ass (I don't understand, but
			// if you say so Edwin!)
			//    part_db.moveRecord( part_tab, 1 );
			part_db.moveToRecord( part_tab, idx );
			if ( part_db.isEOF( part_tab ) )
				break;

			++idx;

			name = part_db.getString( part_tab, "Filename" );
			qty = part_db.getInt( part_tab, "Part_Quantity" );
			is_filler = (part_db.getInt( part_tab, "Fill_Part" ) != 0);

			type = CFileSnoop::getSuffix( name );
			if (type.CompareNoCase("pdb") == 0)
			{
				if (qty > 0)
				{ ret += load_names( name, qty*factor ); }
			}
			else
			{
				// Icky. Under normal circumstances, prevent
				// adding parts having zero quantity.
				if (factor == 1)
				{
					if ((qty == 0) && !is_filler)
						continue;
				}

				npart = new CNestingPart;
				npart->Index( m_parts.Append( npart )-1 );
				npart->Id( part_db.getInt( part_tab, "ID" ) );

				npart->Filepath( name );

				path.Set( name );
				npart->Name( path.FileName() );

				npart->Quantity( qty*factor );
				npart->TotalQuantity( npart->Quantity() );

				if (factor > 1)
				{
					// Kits don't get fill, and don't get -1 quantity
					npart->Filler( false );
					if (npart->Quantity() < 0)
						npart->Quantity( 0 );
				}
				else
				{
					// The normal case ....
					npart->Filler( (is_filler != 0) );
					if (npart->Quantity() < 0)
						npart->Filler( TRUE );
				}

				npart->Rotation( part_db.getInt( part_tab, "Rotation_Num" ) );
				npart->StartAngle( part_db.getDouble( part_tab, "Rotation_Start" ) );
				npart->PreNest(part_db.getInt(part_tab, "PreNest") != 0);

				int mirror = part_db.getInt( part_tab, "MirrorPart" );
				npart->Mirror( mirror==1 );
				npart->StartMirror( mirror==2 );

				npart->Pass( part_db.getInt( part_tab, "Part_Priority" ) );
				npart->Cost( part_db.getDouble( part_tab, "Cost_Per_Part" ) );
				npart->Representation( part_db.getInt( part_tab, "Representation" ) );
				npart->Special( part_db.getInt( part_tab, "Special_Priority" ) != 0);

				npart->FirstPartInspection( 0 );

				// ----------------------------------------------------------
				// Special label management
				//
				CString question;
				question.Format("SELECT Value FROM [Part Variables] WHERE ((([Part ID])=%d) AND ((Description)=\"Part Label\"));", npart->Id() );

				CDaoQuery query;
				query.Init( part_db.Database(), question );

				CString label = npart->Name();
				if (query.RecordCount())
				{ 
					CString special = query.StringGet( "Value" );

					if (special .GetLength() > 1)
					{
						special.Replace("~", "\n");
						label = special;
					}
				}
				npart->Label(label);
			}
		}

		// Done!
		part_db.Close();
	}

	return ret;
}

// ============================================================================

CReturn CPartBin::load_ci_names( const CString& in_filename, int factor )
{
	CReturn	ret;
	CDaoDB	part_db;
	CPath	path;

	int part_tab = part_db.addTable( "Parts" );

	ret += part_db.Open( in_filename ); 
	if (!ret.isOkay())
	{ return ret; }
	//
	// First, we collect the ComponentIDs attached to all the Filenames
	//
	CString question;
	question.Format("SELECT * FROM Parts WHERE (InternalName=\"Filename\")");

	CDaoQuery query;
	ret += query.Init(part_db.Database(), question);
	if (!ret.isOkay())
	{ return ret; }

	int idx;
	for (idx=0; idx<query.RecordCount(); idx++)
	{
		CNestingPart*	npart;

		npart = new CNestingPart;

		npart->Id( query.IntGet("ComponentID" ) );
		npart->Filepath( query.StringGet("Value") );

#if (_CI)
		npart->Name( query.StringGet("Display") );
#else
		path.Set( npart->Filepath() );
		npart->Name( path.FileName() );
#endif

		npart->Index( m_parts.Append( npart )-1 );

		query.Move(1);
	}
	//
	// Now loop over our parts and extract the other fields
	//
	CDaoAttrib attrib(&part_db, "Parts");
	int num = m_parts.Count();
	for (idx=0; idx<num; idx++)
	{
		CNestingPart* npart = m_parts[idx];

		int qty = attrib.ciInt("Part_Quantity", "ComponentID", npart->Id(), -1);
		npart->Quantity( qty*factor );
		npart->TotalQuantity( npart->Quantity() );

		npart->Filler( (attrib.ciInt("Fill_Part", "ComponentID", npart->Id(), 0) != 0) );
		if (npart->Quantity() < 0)
			npart->Filler( true );

		// Kits don't get fill, and don't get -1 quantity
		if (factor > 1)
		{
			npart->Filler( false );
			if (npart->Quantity() < 0)
			{ npart->Quantity( 0 ); }
		}

		npart->Rotation( attrib.ciInt("Rotation_Num", "ComponentID", npart->Id(), 0) );
		npart->StartAngle( attrib.ciDouble("Rotation_Start", "ComponentID", npart->Id(), 0.0) );

		int mirror = attrib.ciInt("Mirror_Part", "ComponentID", npart->Id(), 0);
		npart->Mirror( mirror==1 );
		npart->StartMirror( mirror==2 );

		npart->PreNest( (attrib.ciInt("Pre_Nest", "ComponentID", npart->Id(), 0) != 0) );

		npart->Pass( attrib.ciInt("Part_Pass", "ComponentID", npart->Id(), 1) );

		npart->Cost( attrib.ciDouble("Cost_Per_Part", "ComponentID", npart->Id(), 0.0) );

		// 2005.03.06 (PE) -- added Actual_ID to the Parts tables to allow
		// the CI reporting system to trace data back to the ComponentVar table.
		npart->ActualID( attrib.ciInt("Actual_ID", "ComponentID", npart->Id(), 0) );

		npart->Special( attrib.ciInt("Special_Priority", "ComponentID", npart->Id(), 0) != 0);

		// For use by CiPolicy\CCiModelConvertor.
		// Determines which operations to exclude from conversion.
		npart->Exclusions( attrib.ciInt("Exclusions", "ComponentID", npart->Id(), 0) );

		npart->Label(npart->Name());
	}

	// Done!
	part_db.Close();

	return ret;
}


// ==================================================================
//	Load the Part MM2 files, as named in our list of names
//
CReturn CPartBin::load_parts( const CNestConfig& config )
{
	CReturn		status;
	CPath		path;
	CMM2		mm2;
	CVarList	lm_params;
	CVarList	allowed_layers;
	CString		msg;
	double		small_part_x;
	double		small_part_y;
	int			flags, got, num;

#if (_NST)
	double		filter_tol;
	int color = 0;

	LayerMapParamsGet( config, &lm_params );
	LayerMapGet( config, &allowed_layers );
#endif

	small_part_x = config.MachineReal( "Clamp_Min_Split_Length", 0.0 );
	small_part_y = config.MachineReal( "Clamp_Min_Split_Width", 0.0 );

	got = 0;
	num = m_parts.Count()-1;

	for (int idx=num; idx>=0; idx--)
	{
		CNestingPart* npart = m_parts[idx];

		msg.Format( "Loading Part <%s>", npart->Name() );
		CProgressWnd::SetText( msg );

		// Catch any user abort.
		status += CProgressWnd::ProgressWndUpdate();
		if ( !status.IsOk() )
			break;

		// Try to load the MM2
		CModel* model = new CModel;
		model->Init( dummy_stack, dummy_tree );

		flags = (MM2_STD_MODE | MM2_SKIP_SEQ_OBJS | MM2_SKIP_WORK_ZONES);
		if ( config.KeepSequence() )
			flags |= MM2_KEEP_SEQ_ORDER;

#if (_CI || _NST)
		{
			CCiModel* ciModel = CPortal::CiModel();
			ciModel->Flush();

#if (_NST)
			++color;
			if (color > 6) color = 1;

			filter_tol = lm_params.getReal( "Filter_Tol", 0.001 );

			path.Set( npart->Filepath() );
			if (path.Ext().CompareNoCase("PM4") == 0)
			{
#if (_PM4)
				CScModel scModel;

				status = scModel.FileRead( npart->Filepath() );
				if ( status.IsOk() )
				{
					CScModelConvertor convertor;
					status = convertor.ConvertToDWG( scModel, ciModel );
				}
#endif
			}
			else
			{
				status = ciModel->FileRead(
					npart->Filepath(), allowed_layers, filter_tol, color );
			}

			if ( status.IsOk() )
				status = ProfilesCreate( allowed_layers, lm_params, ciModel );
#else
			status = ciModel->FileRead(
				npart->Filepath(), allowed_layers, SMALL, -1 );
#endif

			if (status.IsOk())
			{
				CCiModelConvertor convertor;
				// ciModel->ModelDump("c:\\_tmp\\_ci_part_as_dwg.txt");
				convertor.Configure( false );
				convertor.ConvertToMM2(
					ciModel, model, config.ToolSetupKey(), npart->Exclusions() );
				{
					// CModelText text;
					// status = text.Dump( "c:\\_tmp\\_ci_part_as_mm2.txt", (*model) );
				}
			}
		}
#else
		model->UndoBufferSuppress();
		status = mm2.Read( npart->Filepath(), model, flags );
#endif
		EWMNesting( (LPCSTR) msg );

		if (!status.isOkay())
		{
			npart->Error("error");  // dunno what string should be set here
			npart->Quantity(0);

			msg.Format( "Part '%s' will not load. Removed from nest.", npart->Name() );
			EWMNesting( (LPCSTR) msg );
		}
		else
		{
			// Catch any user abort.
			status += CProgressWnd::ProgressWndUpdate();
			if ( !status.IsOk() )
				break;

			// Part Model, arbitrary workplanes
			npart->Model( model );
			got++;

			C2dBox	extent = model->BoxUser();
			npart->Extent( extent );

			// As Given / Bounding box / Convex Hull ?
			npart->RepresentationAdjust();

			if ( (max(extent.Dx(), extent.Dy()) < max(small_part_x, small_part_y))
				|| (min(extent.Dx(), extent.Dy()) < min(small_part_x, small_part_y)) )
				npart->Small( TRUE );
			else
				npart->Small( FALSE );
		}
	}

	if ( !status.IsOk() )
		got = 0;

	return ((got > 0) ? STATUS_OKAY : STATUS_ERROR);
}

//	Clone the parts from the nest in the given model
CReturn
CPartBin::Clone( const CNestConfig& config, CModel& model )
{
	CReturn		ret;
	CDbIterator iter;
	CDbTool*	db_tool;

	double small_part_x = config.MachineReal( "Clamp_Min_Split_Length", 0.0 );
	double small_part_y = config.MachineReal( "Clamp_Min_Split_Width", 0.0 );

	int got = 0;

	iter.Init( model.Db(), DBPATTERN );
	while ( TRUE )
	{
		CDbPattern* db_pattern = dynamic_cast<CDbPattern*>( iter() );
		if (!db_pattern)
			break;
		iter.Next();

		CNestingPart*	npart = new CNestingPart;
		npart->Id( db_pattern->Id() );
		npart->Index( m_parts.Append( npart )-1 );
		got++;
		//
		// Setup up the part's model
		//
		CModel* partmodel = new CModel;
		partmodel->Init( dummy_stack, dummy_tree );
		partmodel->UndoBufferSuppress();

		npart->Model( partmodel );
		got++;
		// -----------------------------------------------
		// Clone this pattern into the model.
		// Note that we must also carry across tools.
		//

		model.EntityPrepareCopy( partmodel );
		CDbEntity* new_ent;
		//
		// ... Tools
		//
		CDbIterator tooliter;
		tooliter.Init( model.Db(), DBTOOL );
		while ( TRUE )
		{
			db_tool = dynamic_cast<CDbTool*>( tooliter() );
			if (!db_tool)
				break;
			tooliter.Next();
			model.EntityCopy( (CDbEntity&)*db_tool, &new_ent );
		}
		//
		// ... Pattern geometry
		//
		CDbEntity* db_ent = NULL;
		int num = db_pattern->Count();
		for (int idx = 0; idx<num; idx++)
		{
			db_ent = (*db_pattern)[idx];
			if (db_ent->IsDeleted())
			{ continue; }

			model.EntityCopy( *db_ent, &new_ent );

			if ( (new_ent->Owner() == NULL) &&
				 (new_ent->Type() != DBFEATURE) )
			{
				db_tool = new_ent->Tool();
				if (db_tool != NULL)
				{
					CDbFeature* owner = NULL;
					partmodel->EntityCreate( DBFEATURE, (CDbEntity**)&owner );
					owner->Append( new_ent );
				}
			}
		}
		//
		// some post-op stuff
		//
		C2dBox	extent = partmodel->BoxUser();
		npart->Extent( extent );

		if ( (max(extent.Dx(), extent.Dy()) < max(small_part_x, small_part_y))
			|| (min(extent.Dx(), extent.Dy()) < min(small_part_x, small_part_y)) )
			npart->Small( TRUE );
		else
			npart->Small( FALSE );
	}

	if (got)
		return ret;

	return CReturn( STATUS_ERROR );
}


// ==================================================================
//		AddSelection
//
//	TODO: Merge common bits with Clone??
//
int
CPartBin::AddSelection( const CNestConfig& config, CModel& model )
{
	CSelectorStack& selectorStack = model.SelectorStack();
	CSelector& selector = selectorStack();

	int num = selector.Count();
	if (!num)
	{ return -1; }

	double small_part_x = config.MachineReal( "Clamp_Min_Split_Length", 0.0 );
	double small_part_y = config.MachineReal( "Clamp_Min_Split_Width", 0.0 );

	CNestingPart*	npart = new CNestingPart;
	npart->Id( -1 );
	npart->Index( m_parts.Append( npart )-1 );
	//
	// Setup up the part's model
	//
	CModel* partmodel = new CModel;
	partmodel->Init( dummy_stack, dummy_tree );
	partmodel->UndoBufferSuppress();

	npart->Model( partmodel );
	// -----------------------------------------------
	// Clone the selection into the model.
	// Note that we must also carry across tools.
	//

	model.EntityPrepareCopy( partmodel );
	CDbEntity* new_ent;
	//
	// ... Tools
	//
	CDbIterator tooliter;
	tooliter.Init( model.Db(), DBTOOL );
	while ( TRUE )
	{
		CDbTool* db_tool = dynamic_cast<CDbTool*>( tooliter() );
		if (!db_tool)
			break;
		tooliter.Next();
		model.EntityCopy( (CDbEntity&)*db_tool, &new_ent );
	}
	//
	// ... Geometry
	//
	CDbEntity* db_ent = NULL;
	for (int idx = 0; idx<num; idx++)
	{
		db_ent = selector[idx];
		if (db_ent->IsDeleted())
		{ continue; }

		model.EntityCopy( *db_ent, &new_ent );

		if ( (new_ent->Owner() == NULL) &&
			 (new_ent->Type() != DBFEATURE) )
		{
			CDbTool*	db_tool = new_ent->Tool();
			if (db_tool != NULL)
			{
				CDbFeature* owner = NULL;
				partmodel->EntityCreate( DBFEATURE, (CDbEntity**)&owner );
				owner->Append( new_ent );
			}
		}

		// ??????
		db_ent->Delete();
	}
	//
	// some post-op stuff
	//
	C2dBox	extent = partmodel->BoxUser();
	npart->Extent( extent );

	if ( (max(extent.Dx(), extent.Dy()) < max(small_part_x, small_part_y))
		|| (min(extent.Dx(), extent.Dy()) < min(small_part_x, small_part_y)) )
		npart->Small( TRUE );
	else
		npart->Small( FALSE );
 
	return npart->Index();
}


// ==================================================================
//		AddFile
//
//	TODO: Merge common bits with Clone??
//
int CPartBin::AddFile( const CNestConfig& config )
{
	CReturn	ret;
	CMM2	mm2;
	int		flags;

	double small_part_x = config.MachineReal( "Clamp_Min_Split_Length", 0.0 );
	double small_part_y = config.MachineReal( "Clamp_Min_Split_Width", 0.0 );

	CNestingPart* npart = new CNestingPart();
	npart->Id( -1 );
	npart->Index( m_parts.Append( npart )-1 );
	// -----------------------------------------------
	// Import the damn file
	//
	CModel* partmodel = new CModel();
	partmodel->Init( dummy_stack, dummy_tree );

	flags = (MM2_STD_MODE | MM2_SKIP_SEQ_OBJS | MM2_SKIP_WORK_ZONES);
	if ( config.KeepSequence() )
		flags |= MM2_KEEP_SEQ_ORDER;

	ret += mm2.Read( npart->Filepath(), partmodel, flags );
	partmodel->UndoBufferSuppress();

	if (!ret.isOkay())
	{
		delete partmodel;
		delete npart;
		m_parts.Remove( npart->Index() );

		return -1;
	}

	npart->Model( partmodel );
	//
	// some post-op stuff
	//
	C2dBox	extent = partmodel->BoxUser();
	npart->Extent( extent );

	if ( (max(extent.Dx(), extent.Dy()) < max(small_part_x, small_part_y))
		|| (min(extent.Dx(), extent.Dy()) < min(small_part_x, small_part_y)) )
		npart->Small( TRUE );
	else
		npart->Small( FALSE );
 
	return npart->Index();
}

// Do not call until all of the parts have been loaded.
void
CPartBin::PartsSort( eNestOrder part_order )
{
	CNestingPartArray	tmp;
	CNestingPart*		part;
	int					count, indx;

	count = m_parts.Count();
	for (indx = 0; indx < count; ++indx)
	{
		part = m_parts[indx];
		tmp.Append( part );
	}

	m_parts.BenignFlush();

	switch (part_order)
	{
	case NEST_INORDER:
		// Nothing to do, parts are already order.
		break;

	case NEST_LARGE:
		// ie. Large parts first.
		tmp.Qsort( CPartBin::DecreasingSize );
		break;

	case NEST_SMALL:
		// ie. Small parts first.
		tmp.Qsort( CPartBin::IncreasingSize );
		break;

	case NEST_PRESET:
		// ie. From priority 1 to n.
		tmp.Qsort( CPartBin::DecreasingPriority );
		break;
	}

	// Now move the parts back to their proper storage
	// place, ensuring that filler parts are last.

	// NOTE: We set the SortedID() so that the legend index
	// and printed part table part ids match. We could just
	// use the index of each part in the sorted list but we
	// may just as well carry the value with the part.

	for (indx = 0; indx < count; ++indx)
	{
		part = tmp[indx];
		if ( !part->Filler() )
		{
			m_parts.Append( part );
			part->SortedID( indx+1 );
		}
	}

	for (indx = 0; indx < count; ++indx)
	{
		part = tmp[indx];
		if ( part->Filler() )
		{
			m_parts.Append( part );
			part->SortedID( indx+1 );
		}
	}
}

#if (_NST)
void
CPartBin::LayerMapParamsGet(
				const CNestConfig&	config,
				CVarList*			lm_params )
{
	CReturn		status;
	CDaoDB		cidb;
	CDaoQuery	query;
	CString		sql;
	CString		layer_name;
	int			indx;

	CString cidbPath = CRegister::StringGetV( "Database", "CIDB", "<error>" );

	status = cidb.Open( cidbPath );
	if ( status.IsOk() )
	{
		sql.Format( "SELECT * FROM CadGlobalVar WHERE (CadGlobalID=%d)",
			config.LayerSetup() );

		query.Init( cidb.Database(), sql );
		for (indx = 0; indx < query.RecordCount(); ++indx)
		{
			query.Move( ((indx == 0) ? 0 : 1) );

			lm_params->setString(
				query.StringGet("InternalName"),
				query.StringGet("Value") );
		}
		query.Terminate();
	}
	cidb.Close();
}

void
CPartBin::LayerMapGet(
				const CNestConfig&	config,
				CVarList*			layers )
{
	CReturn		status;
	CDaoDB		cidb;
	CDaoQuery	queryA;
	CDaoQuery	queryB;
	CString		sql;
	CString		layer_name;
	int			cad_layer_map_id;
	int			indxA, indxB;

	CString cidbPath = CRegister::StringGet( "Database", "CIDB", "<error>" );

	status = cidb.Open( cidbPath );
	if ( status.IsOk() )
	{
		cad_layer_map_id = 0;

		sql.Format( "SELECT * FROM CadLayerMap WHERE (CadGlobalID=%d)",
			config.LayerSetup() );

		queryA.Init( cidb.Database(), sql );
		for (indxA = 0; indxA < queryA.RecordCount(); ++indxA)
		{
			queryA.Move( ((indxA == 0) ? 0 : 1) );

			cad_layer_map_id = queryA.IntGet("ID");
			if (cad_layer_map_id > 0)
			{
				sql.Format( "SELECT * FROM CadLayerMapVar WHERE (CadLayerMapID=%d)",
					cad_layer_map_id );

				queryB.Init( cidb.Database(), sql );
				for (indxB = 0; indxB < queryB.RecordCount(); ++indxB)
				{
					queryB.Move( ((indxB == 0) ? 0 : 1) );
					
					if (queryB.StringGet("InternalName").CompareNoCase("layer_name") == 0)
					{
						layer_name = queryB.StringGet("Value");
						layers->setInt( layer_name, 1 );
					}
				}
				queryB.Terminate();
			}
		}
		queryA.Terminate();
	}
	cidb.Close();
}
#endif

#if (_NST)
CReturn
CPartBin::ProfilesCreate(
				const CVarList&	allowed_layers,
				const CVarList&	lm_params,
				CCiModel*		ciModel )
{
	CReturn				status;
	CCiProfileMgr		prfmgr;
	CCiModelIterator	iter;
	CVarList			args;
	CString				layer_name;
	CCiProfile*			ciProfile;
	CCiCurve*			ciCurve;
	CCiLayer*			ciLayer;
	int					tool_id;
	int					count, indx;

	args.setInt( "dir", 0 );
	args.setReal( "filter", lm_params.getReal( "Filter_Tolerance", 0.001 ) );
	args.setReal( "gap", lm_params.getReal( "Clean_Tolerance", 0.001 ) );
	args.setReal( "clean", lm_params.getReal( "Clean_Tolerance", 0.001 ) );

	count = allowed_layers.countVar();
	for (indx = 0; indx < count; ++indx)
	{
		layer_name = allowed_layers.getVar(indx)->getName();

		ciLayer = (CCiLayer*) ciModel->EntityGetByName( layer_name, CILAYER );
		if (ciLayer == NULL)
		{
			// Simply ignore the layer.  Just because the layer is
			// specified in the layer map does not mean that it has
			// to exist in the model.
			// status += status.Internal( IDS_INTERNAL_ERROR, "CPartBin::ProfilesCreate(#1)" );
		}
		else
		{
			args.setInt( "layer", ciLayer->Id() );
			status += prfmgr.ProfileLayer( &args, ciModel );
		}
	}

	// Any profiles that we created must have a _tool_id attribute.
	// because nesting ignores profiles that lack the attribute.
	iter.Init( (*ciModel), CIPROFILE );
	while (1)
	{
		ciProfile = dynamic_cast<CCiProfile*>( iter() );
		if (ciProfile == NULL)
			break;

		ciCurve = ciProfile->GetAt(0);

		tool_id = ciCurve->IntGet( "_tool_id", 0 );
		if (tool_id > 0)
			ciProfile->IntSet( "_tool_id", tool_id );

		iter.Next();
	}

	return status;
}
#endif

// ie. we want large parts to precede small parts.
int
CPartBin::DecreasingSize( const void* ptrA, const void* ptrB )
{
	CNestingPart* partA = (*(CNestingPart**) ptrA);
	CNestingPart* partB = (*(CNestingPart**) ptrB);

	double areaA = partA->OuterKerfArea();
	double areaB = partB->OuterKerfArea();

	double diff = areaA - areaB;

	if (fabs(diff) < SMALL)  // arbitrary tolerance
		return 0;

	return ((diff > 0) ? -1 : 1);
}

// ie. we want small parts to precede large parts.
int
CPartBin::IncreasingSize( const void* ptrA, const void* ptrB )
{
	CNestingPart* partA = (*(CNestingPart**) ptrA);
	CNestingPart* partB = (*(CNestingPart**) ptrB);

	double areaA = partA->OuterKerfArea();
	double areaB = partB->OuterKerfArea();

	double diff = areaA - areaB;

	if (fabs(diff) < SMALL)  // arbitrary tolerance
		return 0;

	return ((diff < 0) ? -1 : 1);
}

// ie. we want parts ordered on priority 1..n
int
CPartBin::DecreasingPriority( const void* ptrA, const void* ptrB )
{
	CNestingPart* partA = (*(CNestingPart**) ptrA);
	CNestingPart* partB = (*(CNestingPart**) ptrB);

	int passA = partA->Pass();
	int passB = partB->Pass();

	return (passA - passB);
}
