#if !defined(_PROFILER_H)
#define _PROFILER_H

// ==================================================================
//		Profiler
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include <math.h>

// ==================================================================
//		Slot -- Dummy object.
//
class CSlot
{
public:

	CSlot( const CString& name )
	{
		m_name	= name;
		Reset();
	}

	const CString&	Name( void ) const			{ return m_name; }
	void			Name( const CString& name )	{ m_name = name; }

	DWORD		Max( void ) const			{ return m_max; }
	void		Max( DWORD max )			{ m_max = max; }

	DWORD		Time( void ) const			{ return m_time; }
	void		Time( DWORD time )			{ m_time = time; }

	DWORD		In( void ) const			{ return m_in; }
	void		In( DWORD in )				{ m_in = in; }

	int			Count( void ) const			{ return m_count; }
	void		Count( int in_count )		{ m_count = in_count; }
	void		Hit( void )					{ m_count++; }

	void Reset()
	{
		m_count	= 0;
		m_time	= 0;
		m_max	= 0;
		m_in	= 0;
	}

	virtual ~CSlot()	{};

private:
	CString			m_name;
	int				m_count;
	DWORD			m_time;
	DWORD			m_max;
	DWORD			m_in;
};

// ==================================================================

class dllExport CProfiler
{
public:

	CProfiler();

	int		Register( const CString& name );

	void	Reset( int in_indx );
	void ResetAll();

	void	In( int in_idx );
	void	Out( int in_idx );

	double	Delta( int in_idx );

	void	Dump( void ) const;

	virtual ~CProfiler();

protected:

private:
	// Disabled.
	CProfiler( const CProfiler& );
	const CProfiler& operator = ( const CProfiler& );
	int operator == ( const CProfiler& ) const;
	int operator != ( const CProfiler& ) const;

private:
	CArray<CSlot*, CSlot*>	m_slot_array;
};

#endif

