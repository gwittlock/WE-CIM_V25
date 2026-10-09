
#include "stdafx.h"
#include "Profiler.h"

#include "Return.h"

// ==================================================================

CProfiler::CProfiler()
{
	m_enabled = false;
}

CProfiler::~CProfiler()
{
	ResetAll();
}

void CProfiler::ResetAll()
{
	m_map.clear();
	m_time_records.clear();
}

void CProfiler::In( const char* name )
{
	if ( m_enabled )
	{
		std::string key = ((name == nullptr) ? "<???>" : name);

		size_t indx = 0;
		if (m_map.count( key ) == 0)
		{
			m_time_records.push_back( CTimeRec( key ) );
			indx = m_time_records.size() - 1;
			m_map[key] = indx;
		}
		else
		{
			indx = m_map[key];
		}

		DWORD time = GetTickCount();
		m_time_records[indx].In( time );
	}
}

void CProfiler::Out( const char* name )
{
	if ( m_enabled )
	{
		DWORD time = GetTickCount();

		std::string key = ((name == nullptr) ? "<???>" : name);

		if (m_map.count( key ) > 0)
		{
			size_t indx = m_map[key];
			CTimeRec& rec = m_time_records[indx];

			DWORD delta = time - rec.In();
			rec.Time( rec.Time() + delta );
			rec.Max( max(rec.Max(), delta) );

			rec.In( 0 );
			rec.Hit();
		}
		else
		{
			ASSERT( 0 );
		}
	}
}

double CProfiler::Delta( const char* name )
{
	double delta = 0.;

	if ( m_enabled )
	{
		DWORD time = GetTickCount();
		std::string key = ((name == nullptr) ? "<???>" : name);

		if (m_map.count( key ) > 0)
		{
			size_t indx = m_map[key];
			CTimeRec& rec = m_time_records[indx];

			delta = (double)((time - rec.In()) / 1000.0);
		}
		else
		{
			ASSERT( 0 );
		}
	}

	return delta;
}

void CProfiler::Dump( const char* label ) const
{
	if ( m_enabled)
	{
		CReturn status;

		CString header;
		header.Format( "\n=-=-=-=-=-=-=-=-=-= BEGIN %s =-=-=-=-=-=-=-=-=-=", ((label == nullptr) ? "???" : label) );
		status.Diagnostic( header );

		size_t count = m_time_records.size();
		for (size_t indx = 0; indx < count; ++indx)
		{
			const CTimeRec& rec = m_time_records[indx];

			double total = (double)(rec.Time() / 1000.0);
			double max = (double)(rec.Max() / 1000.0);
			double per = total / rec.Count();

			CString total_str;
			if (total > 60)
				total_str.Format( "%6.4f minutes", total/60.0 );
			else
				total_str.Format( "%6.4f seconds", total );

			CString per_str;
			if (per > 60)
				per_str.Format( "%6.4f minutes", per/60.0 );
			else
				per_str.Format( "%6.4f seconds", per );

			CString max_str;
			if (max > 60)
				max_str.Format( "%6.4f minutes", max/60.0 );
			else
				max_str.Format( "%6.4f seconds", max );

			CString note;
			note.Format( "    %s : %d calls : %s total, %s per call, %s max",
				rec.Name(), rec.Count(), total_str, per_str, max_str );

			status.Diagnostic( note );
		}

		header.Format( "\n=-=-=-=-=-=-=-=-=-= END %s =-=-=-=-=-=-=-=-=-=", ((label == nullptr) ? "???" : label) );
		status.Diagnostic( header );
	}
}

