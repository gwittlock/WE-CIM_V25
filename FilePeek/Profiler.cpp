
#include "stdafx.h"
#include "Profiler.h"

#include "Return.h"


CProfiler::CProfiler()
{
}

CProfiler::~CProfiler()
{
	ResetAll();
}

void CProfiler::ResetAll()
{
	int count = m_slot_array.GetSize();
	for (int indx = 0; indx < count; ++indx)
	{
		CSlot* slot = m_slot_array[idx];
		if (slot)
			delete slot;
	}
}

int CProfiler::Register( const CString& name )
{
	return m_slot_array.Add( new CSlot( name ) );
}

void CProfiler::Reset( int in_idx )
{
	m_slot_array[in_idx]->Reset();
}

void CProfiler::In( int in_idx )
{
	CSlot* slot = m_slot_array[in_idx];
	if (slot->In())
		return;

	DWORD time = GetTickCount();
	slot->In( time );
}

void CProfiler::Out( int in_idx )
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

double CProfiler::Delta( int in_idx )
{
	DWORD time = GetTickCount();
	CSlot* slot = m_slot_array[in_idx];
	if (!slot->In())
		return 0.0;

	double delta = (double)((time - slot->In()) / 1000.0);
	return delta;
}

void CProfiler::Dump() const
{
	CReturn		status;
	CString		note;
	CSlot*		slot;

	if (CReturn::Debug()>=3)
	{
		status.Diagnostic( "\n\n=================== PROFILE RESULTS ================" );

		int num = m_slot_array.GetSize();
		for (int idx=0; idx<num; idx++)
		{
			slot = m_slot_array[idx];

			double total = (double)(slot->Time() / 1000.0);
			double max = (double)(slot->Max() / 1000.0);
			double per = total / slot->Count();

			CString total_str;
			if (total > 60)
			{ total_str.Format( "%6.4f minutes", total/60.0 ); }
			else
			{ total_str.Format( "%6.4f seconds", total ); }

			CString per_str;
			if (per > 60)
			{ per_str.Format( "%6.4f minutes", per/60.0 ); }
			else
			{ per_str.Format( "%6.4f seconds", per ); }

			CString max_str;
			if (max > 60)
			{ max_str.Format( "%6.4f minutes", max/60.0 ); }
			else
			{ max_str.Format( "%6.4f seconds", max ); }


			note.Format( "    %s : %d calls : %s total, %s per call, %s max",
						slot->Name(),
						slot->Count(),
						total_str,
						per_str,
						max_str );

			status.Diagnostic( note );
		}
	}

//	MessageBox( NULL, "", NULL, MB_OK );
}

