
#include "stdafx.h"
#include "Register.h"
#include "NestConfig.h"
#include "Sheet.h"
#include "SeedMgr.h"

// Ewwwwwwwww, icky!
// TODO: NOTE: g_mid_axis is set via CSheet::do_process()
extern int g_mid_axis;
extern double g_mid_ord;


bool CSeedMgr::m_print_seeds = FALSE;
bool CSeedMgr::m_use_bump_score = FALSE;
eNestProgression CSeedMgr::m_progression;

CSeedMgr::CSeedMgr()
{
	bool	def_use;

	m_print_seeds = CRegister::BoolGetV( "Database", "print_seeds", false );

	def_use = (CNestConfig::IsBitmapNest() ? TRUE : FALSE);
}

CSeedMgr::~CSeedMgr()
{
	m_primary.DestructiveFlush();
	m_secondary.DestructiveFlush();
}

CReturn CSeedMgr::Push()
{
	CReturn	status;

	// TODO: Perhaps this should be implemented using CDynamicArray?
	// There have been cases where nesting failed to place parts that
	// clearly fit simply because we did not have enough seeds!
#if (_NST)
	m_primary.Append( new CSeedList( 5096 ) );
	m_secondary.Append( new CSeedList( 5096 ) );
#else
	m_primary.Append( new CSeedList( 1024 ) );
	m_secondary.Append( new CSeedList( 1024 ) );
#endif

	return status;
}

CReturn CSeedMgr::Pop()
{
	CReturn	status;

	int top = m_primary.Count() - 1;
	if (top < 0)
	{
		status.Internal( IDS_INTERNAL_ERROR, FILE_INFO );
	}
	else
	{
		delete m_primary.Remove( top );
		delete m_secondary.Remove( top );
	}

	return status;
}

bool CSeedMgr::ConditionalPop()
{
	bool did_pop = false;
	int top = m_primary.Count() - 1;
	if (top >= 0)  // really should be (top == 0)
	{
		m_primary[top]->ReduceToPriority( 2 );
		m_secondary[top]->ReduceToPriority( 2 );

		if ((m_primary[top]->Count() < 1) &&
			(m_secondary[top]->Count() < 1))
		{
			Pop();
			did_pop = true;
		}
	}

	return did_pop;
}

CReturn CSeedMgr::Deposit( eSeedBank which_bank, const CSeed& seed )
{
	CReturn	status;

	int top = m_primary.Count() - 1;
	if (top < 0)
	{
		status.Internal( IDS_INTERNAL_ERROR, FILE_INFO );
	}
	else
	{
		if (which_bank == PRIMARY_BANK)
			m_primary[top]->Append( seed );
		else
			m_secondary[top]->Append( seed );
	}

	return status;
}

void CSeedMgr::SeedsPrint()
{
	int	top = m_primary.Count() - 1;
	if (m_print_seeds && (top >= 0))
	{
		enum eSeverity temp = CReturn::ErrorLevel();

		CReturn::ErrorLevel( WARN_DIAGNOSTIC );

		SeedsDump( "primary", m_primary[top] );
		SeedsDump( "secondary", m_secondary[top] );

		CReturn::ErrorLevel( temp );
	}
}

CReturn CSeedMgr::SeedsSort(
	eNestProgression	progression,
	CSheet*				sheet )
{
	CReturn	status;

	m_progression = progression;

	int top = m_primary.Count() - 1;
	if (top < 0)
	{
		status.Internal( IDS_INTERNAL_ERROR, FILE_INFO );
	}
	else
	{
		// This sorting bit is original code.  I do not understand
		// why but with this code disabled dxfdemo4 will not nest
		// properly into dxfdemo1 when part-in-part is enabled.
		m_primary[top]->Qsort( progression, (*sheet) );
		m_secondary[top]->Qsort( progression, (*sheet) );

		SeedsPrint();
	}

	return status;
}

CSeed CSeedMgr::BumpScoreCandidate()
{
	CSeed candidate;
	int top = m_primary.Count() - 1;

	if ((m_primary[top]->Count() > 0) && (m_secondary[top]->Count() > 0))
	{
		const CSeed& candidateA = m_primary[top]->PeekTail();
		const CSeed& candidateB = m_secondary[top]->PeekTail();
	
		switch (m_progression)
		{
		case PROGRESS_PPY_SPX:
			if (candidateB.X() < (candidateA.X() - SMALL))
				candidate = m_secondary[top]->PopTail();
			else
				candidate = m_primary[top]->PopTail();
			break;

		case PROGRESS_PNY_SPX:
			if (candidateB.X() < (candidateA.X() - SMALL))
				candidate = m_secondary[top]->PopTail();
			else
				candidate = m_primary[top]->PopTail();
			break;
		
		case PROGRESS_PPX_SPY:
			if (candidateB.Y() < (candidateA.Y() - SMALL))
				candidate = m_secondary[top]->PopTail();
			else
				candidate = m_primary[top]->PopTail();
			break;
		
		case PROGRESS_PPX_SNY:
			if (candidateB.Y() < (candidateA.Y() - SMALL))
				candidate = m_secondary[top]->PopTail();
			else
				candidate = m_primary[top]->PopTail();
			break;
		}
	}
	else
	{
		candidate = StandardCandidate();
	}

	return candidate;
}

CSeed CSeedMgr::StandardCandidate()
{
	CSeed candidate;
	int top = m_primary.Count() - 1;

	// Alternately, this problem could be solved by having four banks
	// of seeds 1) prim/high 2) sec/high 3) prim/low 4) sec low.
	if (m_primary[top]->Count() > 0)
	{
		if (m_secondary[top]->Count() > 0)
		{
			const CSeed& tmpA = m_primary[top]->PeekTail();
			const CSeed& tmpB = m_secondary[top]->PeekTail();

			if (tmpA.Priority() >= tmpB.Priority())
				candidate = m_primary[top]->PopTail();
			else if (m_secondary[top]->Count())
				candidate = m_secondary[top]->PopTail();
		}
		else
		{
			candidate = m_primary[top]->PopTail();
		}
	}
	else if (m_secondary[top]->Count() > 0)
	{
		candidate = m_secondary[top]->PopTail();
	}

	return candidate;
}

CSeed CSeedMgr::NextCandidate()
{
	CSeed	candidate;
	int		top = m_primary.Count() - 1;

	if (top >= 0)
	{
		if ( m_use_bump_score )
			candidate = BumpScoreCandidate();
		else
			candidate = StandardCandidate();
	}

	if (m_print_seeds && EWMSeedAllow())
	{
		CString msg;

		msg.Format( "candidate %10.5f %10.5f %3d",
			candidate.X(), candidate.Y(), candidate.Priority() );

		EWMSeed( msg );
	}

	return candidate;
}

bool CSeedMgr::HasSeeds() const
{
	int	top = m_primary.Count() - 1;
	int	total_seeds = 0;

	if (top >= 0)
	{
		total_seeds =
			m_primary[top]->Count() +
			m_secondary[top]->Count();
	}

	return (total_seeds > 0);
}

void CSeedMgr::SeedsDump(
	const CString&	which_seeds,
	CSeedList*		seeds )
{
	CString	msg;

	TRACE( which_seeds + "\n" );
	TRACE( "-----------------------------\n" );

	int indx = 0;
	seeds->MyReset();
	while ( !seeds->AtEnd() )
	{
		const CSeed& seed = seeds->Next();

		msg.Format( "%2d) %10.5f %10.5f %3d\n",
			indx, seed.X(), seed.Y(), seed.Priority() );
		TRACE( msg );

		++indx;
	}

	// Not the right thing to do but ....
	seeds->MyReset();
}

CReturn CSeedMgr::SeedsShift( eSeedBank which_bank, C2dVec delta )
{
	CReturn	status;

	int top = m_primary.Count() - 1;
	if (top < 0)
	{
		status.Internal( IDS_INTERNAL_ERROR, FILE_INFO );
	}
	else
	{
		if (which_bank == PRIMARY_BANK)
			m_primary[top]->Shift( delta );
		else
			m_secondary[top]->Shift( delta );
	}

	return status;
}


// Sort left to right
// TODO: NOTE: g_mid_axis is set via CSheet::do_process()
int CSeedMgr::compare_geo_left( const void* u, const void* v )
{
	CGeoCurve* curve_u = (*(CGeoCurve**)u);
	CGeoCurve* curve_v = (*(CGeoCurve**)v);

	C3dCoord pt;
	pt[FLIP_XY(g_mid_axis)] = g_mid_ord;
	pt[g_mid_axis] = curve_u->InterceptXY(g_mid_axis, g_mid_ord);
	pt.Z(0.0);

	// Return -1 if the point is to the left, +1 to the right, 0 on the arc
	return curve_v->PointSide( pt );
}

// Sort right to left
// TODO: NOTE: g_mid_axis is set via CSheet::do_process()
int CSeedMgr::compare_geo_right( const void* u, const void* v )
{
	CGeoCurve* curve_u = (*(CGeoCurve**)u);
	CGeoCurve* curve_v = (*(CGeoCurve**)v);

	C3dCoord pt;
	pt[FLIP_XY(g_mid_axis)] = g_mid_ord;
	pt[g_mid_axis] = curve_u->InterceptXY(g_mid_axis, g_mid_ord);
	pt.Z(0.0);

	// Return -1 if the point is to the left, +1 to the right, 0 on the arc
	return -curve_v->PointSide( pt );
}

