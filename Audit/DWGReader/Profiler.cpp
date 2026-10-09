// ==================================================================
//		Profiler
//
// ==================================================================

#include "stdafx.h"
#include "Profiler.h"

#include "Return.h"

// ==================================================================

CProfiler::CProfiler()
{
}

CProfiler::~CProfiler()
{
	int num = m_slot_array.GetSize();
	for (int idx=0; idx<num; idx++)
	{
		CSlot* slot = m_slot_array[idx];
		if (slot)
			delete slot;
	}
}


// ==================================================================
int		
CProfiler::Register( 
	const CString& name )
{
	return m_slot_array.Add( new CSlot( name ) );
}

// ==================================================================
void		
CProfiler::Reset( int in_idx )
{
	m_slot_array[in_idx]->Reset();
}

// ==================================================================
void	
CProfiler::In( 
	int		in_idx )
{
	CSlot* slot = m_slot_array[in_idx];
	if (slot->In())
		return;

	DWORD time = GetTickCount();
	slot->In( time );
}

// ==================================================================
void	
CProfiler::Out( 
	int		in_idx )
{
	DWORD time = GetTickCount();
	CSlot* slot = m_slot_array[in_idx];
	if (!slot->In())
		return;

	DWORD delta = time - slot->In();
	slot->Time( slot->Time() + delta );
	slot->Max( max(slot->Max(), delta) );

	slot->In( 0 );
	slot->Hit();
}

// ==================================================================
double	
CProfiler::Delta( int in_idx )
{
	DWORD time = GetTickCount();
	CSlot* slot = m_slot_array[in_idx];
	if (!slot->In())
		return 0.0;

	double delta = (double)((time - slot->In()) / 1000.0);
	return delta;
}


// ==================================================================
void	
CProfiler::Dump( void ) const
{
	CReturn		status;
	CString		note;
	CSlot*		slot;

	status.Diagnostic( "=================== PROFILE RESULTS ================" );

	int num = m_slot_array.GetSize();
	for (int idx=0; idx<num; idx++)
	{
		slot = m_slot_array[idx];

		double total = (double)(slot->Time() / 1000.0);
		double max = (double)(slot->Max() / 1000.0);
		double per = total / slot->Count();

		note.Format( "    %s : %d hits : %6.4f seconds total, %6.4f per hit, %6.4f max",
					slot->Name(),
					slot->Count(),
					total,
					per,
					max );

		status.Diagnostic( note );
	}

	MessageBox( NULL, "", NULL, MB_OK );
}

