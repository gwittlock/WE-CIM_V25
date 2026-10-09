// ==================================================================
//		PanelNestMgr
//
// ==================================================================

#include "stdafx.h"

#include "StringConst.h"

#include "MM2.h"
#include "TrueRemnant.h"
#include "PanelList.h"

#include "Profiler.h"
#include "Lead.h"

#include "PanelNestMgr.h"

// ==================================================================

CPanelNestMgr::CPanelNestMgr()
{
}

CPanelNestMgr::~CPanelNestMgr()
{
}


// ==================================================================
//		Execute
//
//	Note that the front-end form of Panel Nesting was taken from
//	True-Shape nesting.  It isn't necessarily a great fit, but was
//	easier than re-inventing from scratch.
//
CReturn		
CPanelNestMgr::Execute(  // DEAD CODE
	CCommand*			io_cmd, 
	CNestConfig&		config, 
	CSheet*				sheet, 
	CPartBin*			partbin,
	CViewMgr&			view )
{
	CReturn		ret;
	int			idx;

	int sheet_count = 0;

	config.ClearResults();
//	ret += clear_results( partbin->Database() );
	if (!ret.isOkay())
		return ret;

	// Do we REALLY want to do this???
	int retid = IDOK;
#if 0
	if (partbin->Count() >= 17)
		retid = ret.Question( IDS_PANELNEST_BIG20, MB_OKCANCEL );
	else
	if (partbin->Count() >= 10)
		retid = ret.Question( IDS_PANELNEST_BIG10, MB_OKCANCEL );
#endif

	if (retid == IDCANCEL)
		return ret;

	// How many parts to nest?
	int parts_to_cut = 0;
	for (idx=0; idx<partbin->Count(); idx++)
		parts_to_cut += partbin->GetAt(idx)->Quantity();

	while ((parts_to_cut > 0) && ret.isOkay())
	{
		view.Clear();

		m_sheet.Reset();
		ret += sheet->Prepare( config, view, io_cmd->getModel() );
		sheet_count++;

		if (ret.isOkay())
		{
			// Reset the part's "use" value
			for (idx=0; idx<partbin->Count(); idx++)
				partbin->GetAt(idx)->Used(0);

			// From "real" sheet, to simplified node form...
			// growing slightly so panels rest at edge
			C2dBox	base_ext = sheet->Extent();
			double half_space = config.Spacing() / 2.0;
			m_sheet.Extent( C2dBox(
									base_ext.Xmin() - half_space,
									base_ext.Ymin() - half_space,
									base_ext.Xmax() + half_space,
									base_ext.Ymax() + half_space ) );

			// From "real" panel list, to simplified working form
			CPanelList	panel_list;
			fill_panel_list( config, *partbin, &panel_list );

			// Everything to this point has been using the databases, loading the parts,
			// and generic prep work.  Config is only barely used at this time.
			//
			// This one Cut call will take this sheet and subdivide it in different ways.
			// Each subdivision will generate 0 to 2 sub-sheets, each of which will be
			// subdivided, etc.  The end result is a n-ary tree of alternating sheets
			// (of diminishing size) and the panels cut on those sheets.
			//
			// The magic is -- this is a depth-first search, and the tree is pruned
			// automatically on the way up.  I'm pretty sure that the end result is
			// optimum for minimum number of cuts and maximum use of material -- though
			// the score balance between depth of cuts and material usage could be adjusted.
			// 
			// All of the work at this call alternates between SheetNode and PanelNode
			//
			m_sheet.Place( config, &panel_list, 1 );

			// Now that we have the winning tree, cut it in order to
			// (a) place the cut panels onto a sheet
			// (b) decrement the panel's quantities in the partbin
			// (c) create a panel-saw cut list
			// (d) update the quantity left to cut
			C2dBox	scrap;
			int did_cut = m_sheet.Cut( config, partbin, sheet, &scrap, view );
			parts_to_cut -= did_cut;

			// See if this sheet can be cut as-is more than once
			parts_to_cut -= duplicate_sheet( partbin, sheet );

			// Update the panel with the new partbin values...
			update_panel_list( config, *partbin, &panel_list );

			//
			// Quadrant crap
			// IT #26, IT#476
			if ( config.YNegative() )
			{ config.ChangeModelY( sheet->Width(), sheet->pModel(), FALSE, TRUE ); }


			// Remnants, if that is indicated
			if ( config.Remnant()
				&& !sheet->IsRemnant() )
				remnant_sheet( config, sheet, scrap, view );

			//
			// Auto-lead the sheet
			//
			CSelector selector(*(sheet->pModel()));

			selector.All( 0 );
			selector.Filter( DBPROFILE, 1 );
			selector.Filter( DBFEATURE, 1 );
			selector.Restrictions( TRUE );
			selector.SelectAll( TRUE );

			CLead	lead(*(sheet->pModel()));

			ret += lead.Auto( config.CmdbPath(), config.LeadSetup(), selector,
				TRUE, TRUE, config.LeadRestrictions() );

			selector.Clear();

			// Save the sheet model
			sheet->Label( *partbin );
			sheet->Save( config.ResultName(), config, sheet_count );

			// Save the DB Results
#if (_CI || _NST)
			config.SavePassResults( sheet_count, *sheet, *partbin );
#else
			config.SavePassResults( *sheet, *partbin );
#endif

			if (!did_cut)
				break;
		}
	} // end while parts_to_cut

	// Tell our client how many sheets we built
	io_cmd->setInt( "Sheet Count", sheet_count );

	config.SaveFinalResults( *sheet, *partbin );

	return ret;
}


// ==================================================================
//		fill_panel_list
//
//	Convert "real" parts from the bin, into the representative
//	dimensions used for rectangular nesting.
//
//	Incorporates the spacing value as well.
//
void	
CPanelNestMgr::fill_panel_list( 
	CNestConfig&		config, 
	const CPartBin&		partbin, 
	CPanelList*			panel_list )
{
	config.PassInit( 1 );

	for (int idx=0; idx<partbin.Count(); idx++)
	{
		CNestingPart* part = partbin.GetAt(idx);

//		C2dBox part_extent = part->Extent();				// WORLD
		C2dBox part_extent = part->pModel()->BoxUser(~0);	// LOCAL

		double half_space = config.Spacing() / 2.0;
		C2dBox	offset_extent(	part_extent.Xmin() - half_space,
								part_extent.Ymin() - half_space,
								part_extent.Xmax() + half_space,
								part_extent.Ymax() + half_space );

		config.Narrow( 1, min( config.Narrow( 1 ), min( offset_extent.Dx(), offset_extent.Dy() ) ) );

		panel_list->Add( offset_extent, part->Quantity(), part->Pass(), part->Rotation()>1 );
	}
}


// ==================================================================
//		update_panel_list
//
//	Keep the existing panel list in-synch with the partbin.
//
void	
CPanelNestMgr::update_panel_list( 
	const CNestConfig&		config, 
	const CPartBin&	partbin, 
	CPanelList*				panel_list )
{
	for (int idx=0; idx<partbin.Count(); idx++)
	{
		CNestingPart* part = partbin.GetAt(idx);
		CPanelData* panel = panel_list->pPanelData(idx);

		panel->Quantity( part->Quantity() );
	}
}


// ==================================================================
//		duplicate_sheet
//
//	See how many times we can repeat this sheet?  Return the number of
//	parts we eliminated this way...
//
int
CPanelNestMgr::duplicate_sheet( 
	CPartBin*	partbin,
	CSheet*	sheet )
{
	int			cut_num;
	int			quant;
	int			used;
	int			idx;
	CNestingPart*	npart;
	int			min_repeat;
	int			repeat;

	// Find our repeat count...
	cut_num = 0;
	min_repeat = INT_MAX;
	for (idx=0; idx<partbin->Count(); idx++)
	{
		npart = partbin->GetAt(idx);

		quant = npart->Quantity();
		used = npart->Used();
		if (quant == 0)
		{
			if (used > 0)
				min_repeat = 0;
		}
		else
		if (quant > 0)
		{
			cut_num += used;
			if (used > 0)
			{
				repeat = quant / used;
				min_repeat = min( repeat, min_repeat );
			}
		}
	}

	if (sheet->Quantity() >= 0)
	{
		if (min_repeat > sheet->Quantity())
			min_repeat = sheet->Quantity();
	}

	// Decrement the part quantities...
	for (idx=0; idx<partbin->Count(); idx++)
	{
		npart = partbin->GetAt(idx);

		quant = npart->Quantity();
		if (quant > 0)
		{
			used = npart->Used();
			npart->Quantity( quant - used*min_repeat );
		}
	}

	// Record the min_repeat in the sheet....
	sheet->Repeat( min_repeat );
	if (sheet->Quantity() > 0)
		sheet->Quantity( sheet->Quantity() - min_repeat );

	// Return the total cut out here...
	if (min_repeat == INT_MAX)
		return 0;

	// Return the total cut out here...
	return cut_num * min_repeat;
}


// ==================================================================
//		remnant_sheet
//
//	Create a square remnant out of whats left of this sheet
//
//	Cloned from the True Shape nesting's square remnant creation; it
//	is far more complex than it needs to be, but re-uses lots of code.
//
// TODO:  Put the body of this, the Save and DB update, into the Remnant class.
//
void
CPanelNestMgr::remnant_sheet(
	CNestConfig&		config, 
	CSheet*				sheet,
	const C2dBox&		scrap,
	CViewMgr&			view )
{
	CTrueRemnant remnant( config, view );

	if (!scrap.IsDefined())
		return;

	// SQUARE REMNANT
	remnant.Extent( scrap );

	C3dVec	shift( -scrap.Xmin(), -scrap.Ymin(), 0.0 );

	// 1. Get the MER from the remnant and check against minimums...
	if ( (scrap.Dx() >= config.RemnantLength())
		&& (scrap.Dy() >= config.RemnantWidth())
		&& ((scrap.Dx()*scrap.Dy()) >= config.RemnantArea()) )
	{
		// 2. Clone the sheet's model -- at the MER size
		CModel	remn_model;
		CModel*	sheet_model = sheet->pModel();

		C3dBox	volume( scrap.Xmin(), scrap.Ymin(), 0.0,
						scrap.Xmax(), scrap.Ymax(), sheet->Extent().Dz() );
		sheet->CreateStock( *sheet_model, volume, &remn_model );

		// 3. Extract into the cloned model (with delta offset to origin)
		remnant.Extract( &remn_model, shift );

		// 4. Save the remnant model to disk
		CMM2	mm2;
		CString	vector;
		CString	remn_file;

		vector.Format( "%%s\\%%s%%%02dd.mm2", config.RemnantDigits() );
		remn_file.Format( vector,
						config.RemnantPath(),
						"Remnant",
						config.RemnantCounter() );
		mm2.Write( remn_file, remn_model );

		// 5. Flush and wipe
		remn_model.Flush();
		config.RemnantCounter( config.RemnantCounter() + 1 );

		// 6. Update remnanate tables 
		CDaoDB		config_db;

		int shop_tab = config_db.addTable( "Remnant Control" );
		int remn_tab = config_db.addTable( "Remnant Inventory" );

		CReturn ret = config_db.Open( config.CmdbPath() );
		if (ret.isOkay())
		{
			// Update counter...
			config_db.moveRecord( shop_tab, 0 );
			config_db.editRecord( shop_tab );
			config_db.setInt( shop_tab, "Remnant_Counter", config.RemnantCounter() );
			config_db.commitRecord( shop_tab );

			// Add this remnant to the inventory...
			config_db.createRecord( remn_tab );

			config_db.setString(	remn_tab, "Filename", remn_file );
			config_db.setString(	remn_tab, "Description", sheet->Description() );
			config_db.setInt(		remn_tab, STR_TYPE_ID, sheet->TypeID() );
			config_db.setDouble(	remn_tab, "Thickness", sheet->Thick() );
			config_db.setDouble(	remn_tab, "Weight", sheet->Weight() );
			config_db.setInt(		remn_tab, "Quantity", 1 );
			config_db.setDouble(	remn_tab, "Cost_Per_Unit", sheet->CostPerUnit() );

			C3dBox	extent = sheet->pModel()->Box();
			config_db.setDouble(	remn_tab, STR_LENGTH, extent.Dx() );
			config_db.setDouble(	remn_tab, STR_WIDTH, extent.Dy() );

			config_db.commitRecord( remn_tab );
		}

	}
}

// ==================================================================
void		
CPanelNestMgr::Dump( 
	const CString& filename )
{
	HANDLE	file;

	file = ::CreateFile( filename,
							GENERIC_WRITE,
							0,
							NULL,
							CREATE_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							(HANDLE)NULL );
	if (file == INVALID_HANDLE_VALUE)
		return;

	m_sheet.Dump( file, 0 );

	::CloseHandle( file );
}

