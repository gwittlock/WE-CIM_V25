// ==================================================================
//		RepoZone
//
// ==================================================================

#include "stdafx.h"
#include <float.h>

#include "StringConst.h"

#include "VarList.h"

#include "RepoZone.h"
#include "ProgressWnd.h"


// 2005.03.31 (PE) -- See also CRepoZone::HoldGrid() and CRepoZone::HoldMark()
double g_hold_x1;
double g_hold_x2;
double g_hold_y;
double g_hold_dia;


// ==================================================================

CRepoZone::CRepoZone()
{
	m_clamp_array = NULL;
	m_hot_repo = 0;
	m_did_repo = 0;
}

CRepoZone::~CRepoZone()
{
	if (m_clamp_array) delete[] m_clamp_array;
}


// ==================================================================

CReturn
CRepoZone::InitRepo( 
	const CNestConfig&	config, 
	const CSheet&		sheet )
{

	double mach_xmax;
	if (config.Progressive())
	{ 
		mach_xmax = sheet.Length();
	}
	else
	{
		mach_xmax = config.MachineTravelX();
//		if (ZERO(mach_xmax))
//		{ mach_xmax = config.MachineReal( "Max_Travel_Limit_X", sheet.Length() ); }
	}

	double narrow = DBL_MAX;
	// Start at pass 1; pass zero is special.
	for (int idx=1; idx<=config.PassNum(); idx++)
		narrow = min( narrow, config.Narrow( idx ) );

	C2dBox	clamp_box = ClampBox( config, 0, 0., sheet );

	return InitRepo( 
		sheet.Length(), sheet.Width(),
		mach_xmax,
		config.RepoOverlap(),
		config.RepoShort()?narrow:0.0,
		clamp_box.Dx(),
		config.MachineReal( "Clamp_Buffer", 0.0 ) );
}

CReturn
CRepoZone::InitRepo( 
	double		sheet_length,
	double		sheet_width,
	double		mach_xmax,
	double		repo_overlap,
	double		repo_short,
	double		clamp_width,
	double		clamp_buffer )
{
	CReturn	ret;

	int repo_factor = (int)ceil(sheet_length / mach_xmax);
	if (repo_factor < 2)
	{
		C2dBox zone( 0, 0, sheet_length, sheet_width );
		m_zone_array.Add( zone );
	}
	else
	{
		double mach_move = mach_xmax - repo_overlap;
		if (mach_move < 1)
			mach_move = 1;

		repo_factor = (int)(sheet_length / mach_move);

		double left = 0.0;

		C2dBox zone;
		if (!EQUAL(repo_short, 0.0))
		{
#define SHORT_PADDING 1.25
			double short_move = max( repo_short*SHORT_PADDING, 
									sheet_length - (mach_move * repo_factor) );

			if (short_move > SMALL)
			{
				zone = C2dBox( 0, 0, short_move, sheet_width );
				m_zone_array.Add( zone );

				left = short_move - repo_overlap;
			}

			zone = C2dBox( left, 0, left+mach_xmax, sheet_width );
		}
		else
		{
			zone = C2dBox( 0, 0, mach_xmax, sheet_width );
			repo_factor++;
		}
		while (repo_factor--)
		{
			if (zone.Xmax() > sheet_length)
			{
				double backup = zone.Xmax() - sheet_length;
				zone.Xmin( zone.Xmin() - backup );
				zone.Xmax( zone.Xmax() - backup );
			}


			if (!EQUAL(repo_short, 0.0))
			{ zone.Xmin( left ); }

			left = zone.Xmax()-repo_overlap;

			m_zone_array.Add( zone );

			if (zone.Xmax()+SMALL >= sheet_length)
			{ repo_factor = 0; }

			if (repo_factor)
			{
				zone.Xmax( zone.Xmax() + mach_move );
				zone.Xmin( zone.Xmin() + mach_move );
			}
		}
	}

	//
	// Now, make sure each zone moves AT LEAST the clamp width
	// As a post-process, this is compact and selectable, but possibly
	// more complex and/or error prone
	//
	if (clamp_width > SMALL)
	{
		C2dBox prev_zone;
		C2dBox zone;

		prev_zone = m_zone_array[0];
		for (int idx=1; idx<m_zone_array.GetSize(); idx++)
		{
			zone = m_zone_array[idx];

			double repo = zone.Xmin() - prev_zone.Xmin();
			if (repo <= (clamp_width+(clamp_buffer*2)-SMALL) )
			{
#define REPO_FUDGE 0.25

				double adjust = (clamp_width+(clamp_buffer*2)) - repo + REPO_FUDGE;

				zone.Xmax( zone.Xmax() + adjust );
				zone.Xmin( zone.Xmin() + adjust );

				if (zone.Xmax() > sheet_length)
				{
					double limit = zone.Xmax() - sheet_length;
					zone.Xmax( zone.Xmax() - limit );
				}

				m_zone_array.SetAt( idx, zone );
			}
			prev_zone = zone;
		}
	}

	return ret;
}




// ==================================================================
//		ClampGrid
//
//	I don't know if clamps are strictly a function of repositions, but
//	it fits here fairly nicely.
//
//	Given the current setup, sheet, and repo work zone, this function
//	will add or remove clamps to the sheet's grid.
//
CReturn	
CRepoZone::ClampGrid( 
	const CNestConfig&	config, 
	CSheet&				sheet, 
	int					zone,
	bool				on )
{
	CReturn	ret;

	if ( !CNestConfig::IsBitmapNest() )  // DYNATORCH
	{
		double buffer = config.MachineReal( "Clamp_Buffer", 0. );

		// Length composite
		double dval1 = config.MachineReal( "Punch_Clamp_Deadzone_Length", 0.0 );
		double dval2 = config.MachineReal( "Torch_Clamp_Deadzone_Length", 0.0 );
		
		double length = max( dval1, dval2 );

		// Width composite
		dval1 = config.MachineReal( "Punch_Clamp_Deadzone_Width", 0.0 );
		dval2 = config.MachineReal( "Torch_Clamp_Deadzone_Width", 0.0 );
		
		double width = max( dval1, dval2 );

		// Center offset...
		dval1 = config.MachineReal( "Punch_Clamp_Deadzone_Center", 0.0 );
		dval2 = config.MachineReal( "Torch_Clamp_Deadzone_Center", 0.0 );

		double offset = (dval1 + dval2 ) / 2.0;
		offset += Zone(zone).Xmin();

		length += fabs(dval1 - dval2);
		double halflen = length / 2.0;

		// 2011.02.26 (PE) -- Build buffer into clamps.
		halflen += buffer;
		width += buffer;

		// Okay, build the damn clamps and draw/erase them in
		for (int indx = 0; indx < config.ClampCountGet(); indx++)
		{
			if (!config.UseClamp( indx ))
				continue;

			double ctr = config.ClampPos( indx ) + offset;

			double origin = width;
			if (config.YNegative())
				origin = sheet.Width();

			if (on)
			{
				C2dBox clamp( ctr - halflen,	// xmin
								origin-width,	// ymin
								ctr + halflen,	// xmax
								origin );		// ymax
				sheet.ClampBarrier( &clamp, !indx );
			}
			else
			{ sheet.ClampBarrier( NULL, !indx ); }

			origin = ceil(width / config.Resolution());
			// if (config.NestPosY())
			if (config.YNegative())
				origin = sheet.Grid().Height()-1;

			C2dBox clamp_grid( floor( (ctr-halflen)/config.Resolution() ),
								origin - floor(width/config.Resolution() ),
								ceil( (ctr+halflen)/config.Resolution() ),
								origin );
			sheet.pGrid()->MarkBox( clamp_grid, MARK_CLAMP, on );
			if (0)
				sheet.pGrid()->Debug( CString("_clamped_sheet.txt") );
		}
	}

	return ret;
}

// ==================================================================
//		ClampMark
//
//	Given the current setup, sheet, and repo work zone, this function
//	will drop in clamp markers (hints) for later Repo use.
//
CReturn	
CRepoZone::ClampMark( 
	const CNestConfig&	config, 
	CSheet*				sheet, 
	int					zone )
{
	CReturn	ret;

	CModel*	model = sheet->pModel();

	CDbWorkplane*	workplane = NULL;
	model->EntityFind( STR_WORLD, (CDbEntity**)&workplane, DBWORKPLANE, DBWORKPLANE );
	if (!workplane)
		workplane = model->ActiveWorkplane();

	double clamp_y = 0;
	if ( config.YNegative() )
		clamp_y = sheet->Width();

	// Okay, mark the clamps in
	for (int indx = 0; indx < config.ClampCountGet(); indx++)
	{
		if (!config.UseClamp( indx ))
			continue;

		// Create the actual Command for the clamp here...
		CString	str;

		C2dBox box = ClampBox( config, indx, Zone(zone).Xmin(), *sheet );

		str.Format( "@CLAMP: repo=%d, num=%d,\n  xmin=%f, ymin=%f,\n  xmax=%f, ymax=%f", 
			zone+1, indx+1, box.Xmin(), box.Ymin(), box.Xmax(), box.Ymax() );

		C3dCoord pt( (box.Xmin() + box.Xmax())/2.0, clamp_y, 0.0 );

		CDbCommand*	cmd = NULL;
		ret += model->EntityCreate( DBCOMMAND, (CDbEntity**)&cmd );
		cmd->Init( model->ActiveTool(), workplane, pt, str );
		cmd->SystemFlag( true );
	}

	return ret;
}

// Calculate the exclusion box for a given clamp
C2dBox CRepoZone::ClampBox( 
	const CNestConfig&	config, 
	int					clamp,
	double				zone_offset,
	const CSheet&		sheet )
{
	// Length composite
	double dval1 = config.MachineReal( "Punch_Clamp_Deadzone_Length", 0.0 );
	double dval2 = config.MachineReal( "Torch_Clamp_Deadzone_Length", 0.0 );
	
	double length = max( dval1, dval2 );

	// Width composite
	dval1 = config.MachineReal( "Punch_Clamp_Deadzone_Width", 0.0 );
	dval2 = config.MachineReal( "Torch_Clamp_Deadzone_Width", 0.0 );
	
	double width = max( dval1, dval2 );

	// Center offset...
	dval1 = config.MachineReal( "Punch_Clamp_Deadzone_Center", 0.0 );
	dval2 = config.MachineReal( "Torch_Clamp_Deadzone_Center", 0.0 );

	double offset = (dval1 + dval2 ) / 2.0;
	offset += zone_offset;

	length += fabs(dval1 - dval2);
	double halflen = length / 2.0;

	double ctr = config.ClampPos( clamp ) + offset;

	double origin = width;
	if ( config.YNegative() )
		origin = sheet.Width();

	C2dBox box = C2dBox( ctr - halflen,			// xmin
						origin-width,			// ymin
						ctr + halflen,			// xmax
						origin );				// ymax

	return box;
}


// ==================================================================
//		ClampReMark
//
//	Given a complete m_clamp_array, drop in the clamp commands
//
CReturn	
CRepoZone::ClampReMark( 
	CModel*	model,
	int		zone )
{
	CReturn	ret;
	CString	str;

	CDbWorkplane*	workplane = NULL;
	model->EntityFind( STR_WORLD, (CDbEntity**)&workplane, DBWORKPLANE, DBWORKPLANE );
	if (!workplane)
		workplane = model->ActiveWorkplane();

	int cnum= m_clamp_array[zone].GetSize();
	for (int cidx=0; cidx<cnum; cidx++)
	{
		C2dBox	clamp = (m_clamp_array[zone])[cidx];

		str.Format( "@CLAMP: repo=%d, num=%d,\n  xmin=%f, ymin=%f,\n  xmax=%f, ymax=%f", 
							zone+1,
							cidx+1,
							clamp.Xmin(), clamp.Ymin(),
							clamp.Xmax(), clamp.Ymax() );

		C3dCoord pt( (clamp.Xmin() + clamp.Xmax())/2.0,
					(clamp.Ymin() + clamp.Ymax())/2.0,
					0.0 );

		CDbCommand*	cmd = NULL;
		ret += model->EntityCreate( DBCOMMAND, (CDbEntity**)&cmd );
		cmd->Init( model->ActiveTool(), workplane, pt, str );
		cmd->SystemFlag( true );
	}

	return ret;
}


// ==================================================================
//	Drop in a command to identify the geometric extents of this repo zone
CReturn CRepoZone::Mark( CSheet* sheet, int zone )
{
	return Mark( sheet->pModel(), zone );
}

CReturn CRepoZone::Mark( CModel* model, int zone )
{
	CReturn	ret;
	CString	str;

	CDbWorkplane*	workplane = NULL;
	model->EntityFind( STR_WORLD, (CDbEntity**)&workplane, DBWORKPLANE, DBWORKPLANE );
	if (!workplane)
		workplane = model->ActiveWorkplane();

	C2dBox	box = m_zone_array[zone];
	str.Format( "@ZONE: num=%d,\n  xmin=%f, ymin=%f,\n  xmax=%f, ymax=%f", 
							zone+1,
							box.Xmin(), box.Ymin(),
							box.Xmax(), box.Ymax() );

	C3dCoord pt( box.Xmin(), box.Ymin(), 0.0 );

	CDbCommand*	cmd = NULL;
	ret += model->EntityCreate( DBCOMMAND, (CDbEntity**)&cmd );
	cmd->Init( model->ActiveTool(), workplane, pt, str );
	cmd->SystemFlag( true );

	m_did_repo = max( m_did_repo, zone+1 );

	return ret;
}


// ==================================================================
//		AnalyzeClamps
//
//	Extract all clamp markers from the model, and then duplicate
//	them across all repo zones.
//
CReturn		
CRepoZone::AnalyzeClamps( 
	CModel*	model )
{
	CReturn		ret;

	if (m_clamp_array) delete[] m_clamp_array;
	int znum = m_zone_array.GetSize();
	m_clamp_array = new CArray<C2dBox, C2dBox>[znum];

	//
	// Extract all zone markers from the model
	//
	CSelector	select( *model );
	select.All( 0 );
	select.Filter( DBCOMMAND, 1 );
	select.Restrictions( TRUE );
	select.SelectAll( TRUE );

	int num = select.Count();
	for (int idx=0; idx<num; idx++)
	{
		// pre-filtered; only commands will be found
		CDbCommand* db_cmd = dynamic_cast<CDbCommand*>(select[idx]);
		if (db_cmd->IsA("CLAMP"))
		{
			const CVarList& var = db_cmd->Attrib();

			int repo = var.getInt( "repo", 1 );
			int idx = var.getInt( "num", -1 );
			if (idx>=0)
			{
				C2dBox	box;

				if (repo == 1)
				{
					box.Xmax( var.getReal( STR_XMAX, 0.0 ) );
					box.Ymax( var.getReal( STR_YMAX, 0.0 ) );
					box.Xmin( var.getReal( STR_XMIN, 0.0 ) );
					box.Ymin( var.getReal( STR_YMIN, 0.0 ) );

					idx--;
					m_clamp_array[0].SetAtGrow( idx, box );
				}

				db_cmd->Delete();
			}
		}
	}

	select.Clear();

	//
	// Now, propogate the clamps across the zones
	//
	int cnum= m_clamp_array[0].GetSize();
	for (int zidx=1; zidx<znum; zidx++)
	{
		for (int cidx=0; cidx<cnum; cidx++)
		{
			C2dBox	clamp = (m_clamp_array[0])[cidx];
			double	shift = m_zone_array[zidx].Xmin() - m_zone_array[0].Xmin();

			clamp = C2dBox( clamp.Xmin()+shift,
							clamp.Ymin(),
							clamp.Xmax()+shift,
							clamp.Ymax() );

			m_clamp_array[zidx].SetAtGrow( cidx, clamp );
		}
	}

	return ret;
}



// ==================================================================
CReturn	
CRepoZone::HoldGrid( 
	CViewMgr&			view,
	const CNestConfig&	config, 
	CSheet&				sheet, 
	int					zone,	// 0..N
	bool				on )
{
	CReturn	ret;

	if (on)
	{
		// As put in the model header by CImportUtil::HeaderUpdate().
		m_hold_type = (eHoldType) sheet.Model().Header().getInt( "hold_type", IUNDEFINED );
		g_hold_x1 = sheet.Model().Header().getReal( "hold_x1", 0. );
		g_hold_x2 = sheet.Model().Header().getReal( "hold_x2", 0. );
		g_hold_y  = sheet.Model().Header().getReal( "hold_y", 0. );
		g_hold_dia = sheet.Model().Header().getReal( "hold_dia", 0. );

		// Get hold-down parameters; operate in grid space
		m_hold_x1 = (int)(g_hold_x1 / config.Resolution() + 0.5);
		m_hold_x2 = (int)(g_hold_x2 / config.Resolution() + 0.5);
		m_hold_y = (int)(g_hold_y / config.Resolution() + 0.5);
		m_hold_dia = (int)(g_hold_dia / config.Resolution() + 0.5);

		if (m_hold_x1 > m_hold_x2)
		{
			int temp = m_hold_x1;
			m_hold_x1 = m_hold_x2;
			m_hold_x2 = temp;
		}

		// Length composite
		m_hold_dead = (int)ceil( max( config.MachineReal( "Punch_Clamp_Deadzone_Width", 0.0 ),
							 config.MachineReal( "Torch_Clamp_Deadzone_Width", 0.0 ) )
							/ config.Resolution() );

		// Locate likely hold-down locations
		ret += hold_locate( view, config, sheet, zone );
	}

	if (ret.isOkay())
	{
		switch (m_hold_type)
		{
		case HOLD_2CIRCLE:
			{
			C2dCoord ctr = m_hold_array[zone] + C2dVec( g_hold_x1, g_hold_y );
			sheet.pGrid()->MarkDisk( ctr, m_hold_dia, MARK_CLAMP, on );

			ctr = m_hold_array[zone] + C2dVec( g_hold_x2, g_hold_y );
			sheet.pGrid()->MarkDisk( ctr, m_hold_dia, MARK_CLAMP, on );
			}
			break;

		case HOLD_RECTANGLE:
			{
			C2dBox box( (m_hold_array[zone]).X() + m_hold_x1,
						(m_hold_array[zone]).Y() + m_hold_y - m_hold_dia/2.0,
						(m_hold_array[zone]).X() + m_hold_x2,
						(m_hold_array[zone]).Y() + m_hold_y + m_hold_dia/2.0 );
			sheet.pGrid()->MarkBox( box, MARK_CLAMP, on );
			}
			break;
		}
	}

	return ret;
}

// ==================================================================
//		hold_locate
//
//	Using a comprehensive search pattern, try to find a location for the
//	hold-down clamps.
//
//	If can't find a "good" spot -- just put them in a default position.
//
CReturn
CRepoZone::hold_locate( 
	CViewMgr&			view,
	const CNestConfig&	config, 
	CSheet&				sheet,
	int					zone )
{
	CReturn ret;

	double default_x = config.HoldDefaultX();
	double default_y = config.HoldDefaultY();
	if (config.YNegative())
	{
		// End-user must supply a negative value for 4th quadrant machines!
		default_y = sheet.Width() + default_y;
	}

	int high_x = 0;
	int high_y = 0;

	if ( !config.ForceHoldX() || !config.ForceHoldY() )
	{
		if ( config.DebugBmp() )
			sheet.Grid().Draw( view, 0, 0, FALSE, TRUE );

		// A bunch of operating parameters
		int edge_dist = m_hold_dia * 2;
		int dead_dist = max( edge_dist, m_hold_dead );

		C2dBox& travel = m_zone_array[zone];

		double mach_xmax = config.MachineTravelX();
//		if (ZERO(mach_xmax))
//		{ mach_xmax = config.MachineReal( "Max_Travel_Limit_X", sheet.Length() ); }

		// Weight mid-x to the zone overlap
		// Note that bar clamps MUST go in the zone overlap
		int from_x = (int)ceil( travel.Xmin() / config.Resolution() );
		int to_x = (int)floor( travel.Xmax() / config.Resolution() );
		if (config.RepoShort())
		{ to_x = from_x + (int)floor(mach_xmax / config.Resolution() ); }

		int next_from_x = from_x;
		if ((zone+1)<m_zone_array.GetSize())
		{ 
			C2dBox& next_travel = m_zone_array[zone+1];
			next_from_x = (int)ceil( next_travel.Xmin() / config.Resolution() );
		}

		int mid_x = (next_from_x + to_x) >> 1;

		if (m_hold_type == HOLD_RECTANGLE)
		{ from_x = next_from_x; }
		//
		// Now, inset by the edge distance IF WE CAN
		//
		if ( (to_x - from_x) > (2.0*edge_dist + SMALL) )
		{
			from_x += edge_dist;
			to_x -= edge_dist;
		}
		to_x += 1;
		//
		//
		int from_y = (int)ceil( (travel.Ymin() / config.Resolution()) + dead_dist );
		int to_y = (int)floor( (travel.Ymax() / config.Resolution()) - dead_dist );

		int mid_y = (from_y + to_y) >> 1;

		int	hold_midx = (m_hold_x1 + m_hold_x2) >> 1;

		// Force holds in the face of any possible previous calculations
		if (config.ForceHoldX())
		{
			from_x = (int)ceil( default_x / config.Resolution() );
			to_x = from_x;
		}
		if (config.ForceHoldY())
		{
			from_y = (int)ceil( default_y / config.Resolution() );
			to_y = from_y;
		}

		//  Perform the scan, scoring as we go
		int high_score = INT_MIN;
		int step = max( 2, m_hold_dia >> 2 );

		bool visible = FALSE;

		for (int at_x=from_x; at_x<=to_x; at_x+=step)
		{
			ret += CProgressWnd::ProgressWndUpdate();
			if ( !ret.IsOk() )
				return ret;  // user abort

			for (int at_y=from_y; at_y<=to_y; at_y+=step)
			{
	#define X_WEIGHT 1
	#define Y_WEIGHT 5
				int penalty = abs(mid_x-(at_x+hold_midx))*X_WEIGHT + abs(mid_y-(at_y+m_hold_y))*Y_WEIGHT;
		
				int score = hold_score( sheet, at_x, at_y );
				if (score)
				{
					score = (score * 10) - penalty;
					if (score > (high_score+5))
					{
						if (visible && config.DebugWire())
						{
							hold_draw( view, sheet, high_x, high_y );
							visible = FALSE;
						}

						high_score = score;
						high_x = at_x;
						high_y = at_y;

						if ( config.DebugWire() )
							hold_draw( view, sheet, high_x, high_y );
						visible = TRUE;
					}
				}
			}
		}
		if (visible && config.DebugWire())
		{
			hold_draw( view, sheet, high_x, high_y );
			visible = FALSE;
		}

		// Default location on failure...
		if (high_score == INT_MIN)
		{
			if ( ZERO(default_x)
				&& ZERO(default_y) )
			{
				default_x = (from_x + to_x) / 2;
				default_y = to_y - m_hold_y;
			}
			else
			{
				// Offset default_x to edge of zone
				default_x += travel.Xmin();
				// Scale to grid
				default_x /= config.Resolution();
				default_y /= config.Resolution();
			}

			high_x = (int)default_x;
			high_y = (int)default_y;

		}

		if ( config.DebugWire() )
			hold_draw( view, sheet, high_x, high_y );
	}
	else // Both defaults forced
	{
		high_x = (int)(default_x / config.Resolution());
		high_y = (int)(default_y / config.Resolution());
	}

	m_hold_array.Add( C2dCoord( high_x, high_y ) );

	return ret;
}

// ==================================================================
int
CRepoZone::hold_score(
	CSheet&	sheet,
	int		at_x,
	int		at_y )
{
	switch (m_hold_type)
	{
	case HOLD_2CIRCLE:
		{

		C2dCoord ctr( at_x + m_hold_x1,
						at_y + m_hold_y );
		int score = hold_score_disk( sheet, ctr, m_hold_dia );

		if (score)
		{
			C2dCoord ctr = C2dCoord( at_x + m_hold_x2,
									at_y + m_hold_y );
			int score2 = hold_score_disk( sheet, ctr, m_hold_dia );
			if (score2)
				score += score2;
			else
				score = 0;
		}
		return score;
		}

	case HOLD_RECTANGLE:
		{
		C2dBox box( at_x + m_hold_x1,
					at_y + m_hold_y - m_hold_dia/2.0,
					at_x + m_hold_x2,
					at_y + m_hold_y + m_hold_dia/2.0 );
		return hold_score_box( sheet, box );
		}
	}

	return 0;
}

int
CRepoZone::hold_score_disk(
	CSheet&			sheet,
	const C2dCoord&	ctr,
	int				dia )
{
	int		score = 0;
	int radius = dia / 2;
	double radius2 = radius*radius;

	int from_y = (int)floor(ctr.Y() - radius);
	int to_y = (int)ceil(ctr.Y() + radius);

	C2dCoord at_pos;
	for (int at_y=from_y; at_y<=to_y; at_y++)
	{
		if ( (at_y < 0)
			|| (at_y >= sheet.Grid().Height() ))
			return 0;

		at_pos.Y(at_y);

		double rise = at_y - ctr.Y();
		double run = sqrt( radius2 - rise*rise );

		int from_x = (int)floor(ctr.X() - run);
		int to_x = (int)ceil(ctr.X() + run);

		for (int at_x=from_x; at_x<=to_x; at_x++)
		{
			if ( (at_x < 0)
				|| (at_x >= sheet.Grid().Width() ) )
				return 0;

			at_pos.X(at_x);
			int cell = sheet.pGrid()->Cell(at_pos);

			if (cell & MARK_CLAMP)
				return 0;

			if (!(cell & (MARK_BODY | MARK_HOLE)))
			{
				score++;
				
				if (at_x == ctr.X())
					score+=10;

				if (at_y == ctr.Y())
					score+=10;

				if ( (at_x == ctr.X())
					&& (at_y == ctr.Y()) )
					score += 20;
			}
		}
	}

	return score;
}

int
CRepoZone::hold_score_box(
	CSheet&		sheet,
	C2dBox&		box )
{
	int		score = 0;
	int from_y = (int)floor(box.Ymin());
	int to_y = (int)ceil(box.Ymax());

	int from_x = (int)floor(box.Xmin());
	int to_x = (int)ceil(box.Xmax());

	C2dCoord at_pos;
	for (int at_y=from_y; at_y<=to_y; at_y++)
	{
		if ( (at_y < 0)
			|| (at_y >= sheet.Grid().Height() ))
			continue;

		at_pos.Y(at_y);

		for (int at_x=from_x; at_x<=to_x; at_x++)
		{
			if ( (at_x < 0)
				|| (at_x >= sheet.Grid().Width() ) )
				return 0;

			at_pos.X(at_x);
			int cell = sheet.pGrid()->Cell(at_pos);

			if (!(cell & (MARK_BODY | MARK_HOLE)))
			{
				score++;
				
				if (at_x == box.Xc())
					score++;

				if (at_y == box.Yc())
					score++;

				if ( (at_x == box.Xc())
					&& (at_y == box.Yc()) )
					score += 2;
			}
		}
	}

	return score;
}

// ==================================================================
void
CRepoZone::hold_draw(
	CViewMgr&	view,
	CSheet&		sheet,
	int			at_x,
	int			at_y )
{
	switch (m_hold_type)
	{
	case HOLD_2CIRCLE:
		{
		C2dCoord ctr( at_x + m_hold_x1,
						at_y + m_hold_y );
		hold_draw_disk( view, sheet, ctr, m_hold_dia );

		ctr = C2dCoord( at_x + m_hold_x2,
								at_y + m_hold_y );
		hold_draw_disk( view, sheet, ctr, m_hold_dia );
		}
		break;

	case HOLD_RECTANGLE:
		{
		C2dBox box( at_x + m_hold_x1,
					at_y + m_hold_y - m_hold_dia/2.0,
					at_x + m_hold_x2,
					at_y + m_hold_y + m_hold_dia/2.0 );
		hold_draw_box( view, sheet, box );
		}
		break;
	}
}

void
CRepoZone::hold_draw_disk(
	CViewMgr&		view,
	CSheet&			sheet,
	const C2dCoord&	ctr,
	int				dia )
{
	CDC* dest = view.getCWnd()->GetDC();

	int radius = dia / 2;
	double radius2 = radius*radius;

	int from_y = ((int)ctr.Y() - radius);
	int to_y = ((int)ctr.Y() + radius);

	for (int at_y=from_y; at_y<=to_y; at_y++)
	{
		if ( (at_y < 0)
			|| (at_y >= sheet.Grid().Height() ))
			return;

		double rise = at_y - ctr.Y();
		double run = sqrt( radius2 - rise*rise );

		int from_x = (int)(ctr.X() - run + 0.5);
		int to_x = (int)(ctr.X() + run + 0.5);

		for (int at_x=from_x; at_x<=to_x; at_x++)
		{
			if ( (at_x < 0)
				|| (at_x >= sheet.Grid().Width() ) )
				return;

			COLORREF color = dest->GetPixel( at_x, at_y );
			if (color == 0xffffff)
				color = 0x000000;
			else
				color = 0xffffff;

			dest->SetPixel( at_x, at_y, color );
		}
	}
}

void
CRepoZone::hold_draw_box(
	CViewMgr&	view,
	CSheet&		sheet,
	C2dBox&		box )
{
	CDC* dest = view.getCWnd()->GetDC();

	for (int at_y=(int)box.Ymin(); at_y<=(int)box.Ymax(); at_y++)
	{
		if ( (at_y < 0)
			|| (at_y >= sheet.Grid().Height() ))
			return;

		for (int at_x=(int)box.Xmin(); (int)at_x<=box.Xmax(); at_x++)
		{
			if ( (at_x < 0)
				|| (at_x >= sheet.Grid().Width() ) )
				return;

			COLORREF color = 0xffffff ^ dest->GetPixel( at_x, at_y );
			dest->SetPixel( at_x, at_y, color );
		}
	}
	return;
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// 2005.03.31 (PE) -- LBT discovered that holddowns were not accurately displayed.
// And though things appear to display better after this change, I am not convinced
// that holddown placement is correct because of the mapping between grid and
// machine spaces.
//
#if BEFORE_2005_03_31

CReturn	
CRepoZone::HoldMark( 
	const CNestConfig&	config, 
	CSheet&				sheet, 
	int					dest_zone,
	int					hold_zone,	
	CDbCommand**		hold )
{
	CReturn	ret;

	CModel*	model = sheet.pModel();

	// Bulletproof... but why?  Large part with no fill, limited number of zones
	int hold_num = m_hold_array.GetSize();
	if (hold_zone >= hold_num)
		return ret;

	CDbWorkplane*	workplane = NULL;
	model->EntityFind( STR_WORLD, (CDbEntity**)&workplane, DBWORKPLANE, DBWORKPLANE );
	if (!workplane)
		workplane = model->ActiveWorkplane();

	double ctr_x = m_hold_array[hold_zone].X();
	double ctr_y = m_hold_array[hold_zone].Y();

	// Create the Command for the hold-down
	CString	str;
	switch (m_hold_type)
	{
	case HOLD_2CIRCLE:
		str.Format( "@HOLD: repo=%d, type=%d,\n  \
			c1x=%f, c1y=%f,\n  \
			c2x=%f, c2y=%f,\n  \
			dia=%f, \
			x=%f, y=%f",
						dest_zone+1,
						HOLD_2CIRCLE,
						((m_hold_array[hold_zone]).X() + m_hold_x1) * config.Resolution(),
						(ctr_y + m_hold_y) * config.Resolution(),
						((m_hold_array[hold_zone]).X() + m_hold_x2) * config.Resolution(),
						(ctr_y + m_hold_y) * config.Resolution(),
						m_hold_dia * config.Resolution(),
						(m_hold_array[hold_zone]).X() * config.Resolution(),
						ctr_y * config.Resolution() );
		break;

	case HOLD_RECTANGLE:
		str.Format( "@HOLD: repo=%d, type=%d,\n  \
			xmin=%f, ymin=%f,\n  \
			xmax=%f, ymax=%f,\n  \
			x=%f, y=%f",
						dest_zone+1,
						HOLD_RECTANGLE,
						((m_hold_array[hold_zone]).X() + m_hold_x1) * config.Resolution(),
						(ctr_y - m_hold_dia/2.0) * config.Resolution(),
						((m_hold_array[hold_zone]).X() + m_hold_x2) * config.Resolution(),
						(ctr_y + m_hold_dia/2.0) * config.Resolution(),
						(m_hold_array[hold_zone]).X() * config.Resolution(),
						ctr_y * config.Resolution() );
		break;
	}

	C3dCoord pt( (m_hold_array[hold_zone]).X()*config.Resolution(), ctr_y*config.Resolution(), 0.0 );

	*hold = NULL;
	ret += model->EntityCreate( DBCOMMAND, (CDbEntity**)hold );
	(*hold)->Init( model->ActiveTool(), workplane, pt, str );
	(*hold)->SystemFlag( true );

	return ret;
}

#else

CReturn	
CRepoZone::HoldMark( 
	const CNestConfig&	config, 
	CSheet&				sheet, 
	int					dest_zone,
	int					hold_zone,	
	CDbCommand**		hold )
{
	CReturn	ret;
	CString	str;

	CModel*	model = sheet.pModel();

	(*hold) = NULL;

	// Bulletproof... but why?  Large part with no fill, limited number of zones
	int hold_num = m_hold_array.GetSize();
	if (hold_zone >= hold_num)
		return ret;

	CDbWorkplane*	workplane = NULL;
	model->EntityFind( STR_WORLD, (CDbEntity**)&workplane, DBWORKPLANE, DBWORKPLANE );
	if (!workplane)
		workplane = model->ActiveWorkplane();

	double ctr_x = m_hold_array[hold_zone].X() * config.Resolution();
	double ctr_y = m_hold_array[hold_zone].Y() * config.Resolution();

	// Create the Command for the hold-down
	switch (m_hold_type)
	{
	case HOLD_2CIRCLE:
		str.Format( "@HOLD: repo=%d, type=%d,\n  \
			c1x=%f, c1y=%f,\n  \
			c2x=%f, c2y=%f,\n  \
			dia=%f, x=%f, y=%f",
				(dest_zone + 1), HOLD_2CIRCLE,
				(ctr_x + g_hold_x1), (ctr_y + g_hold_y),
				(ctr_x + g_hold_x2), (ctr_y + g_hold_y),
				g_hold_dia, ctr_x, ctr_y );
		break;

	case HOLD_RECTANGLE:
		str.Format( "@HOLD: repo=%d, type=%d,\n  \
			xmin=%f, ymin=%f,\n  \
			xmax=%f, ymax=%f,\n  \
			x=%f, y=%f",
				(dest_zone + 1), HOLD_RECTANGLE,
				(ctr_x + g_hold_x1), (ctr_y + g_hold_y - (0.5 * g_hold_dia)),
				(ctr_x + g_hold_x2), (ctr_y + g_hold_y + (0.5 * g_hold_dia)),
				ctr_x, ctr_y );
		break;
	}

	C3dCoord pt( ctr_x, ctr_y, 0. );

	ret += model->EntityCreate( DBCOMMAND, (CDbEntity**)hold );
	(*hold)->Init( model->ActiveTool(), workplane, pt, str );
	(*hold)->SystemFlag( true );

	return ret;
}

#endif

