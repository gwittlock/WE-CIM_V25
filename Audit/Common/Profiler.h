#pragma once

#include <string>
#include <vector>
#include <map>

// ==================================================================
//		Slot -- Dummy object.
//
class CTimeRec
{
public:

	CTimeRec( const std::string& name )
	{
		m_name = name;
		Reset();
	}

	virtual ~CTimeRec() {};

	const char* Name() const   { return m_name.c_str(); }
	void Name( const std::string& name ) { m_name = name; }

	DWORD Max() const   { return m_max; }
	void Max( DWORD max )   { m_max = max; }

	DWORD Time() const   { return m_time; }
	void Time( DWORD time )   { m_time = time; }

	DWORD In() const   { return m_in; }
	void In( DWORD in )    { m_in = in; }

	int Count() const   { return m_count; }
	void Count( int in_count )  { m_count = in_count; }
	void Hit()     { m_count++; }

	void Reset()
	{
		m_count = 0;
		m_time = 0;
		m_max = 0;
		m_in = 0;
	}

private:

	std::string m_name;
	int m_count;
	DWORD m_time;
	DWORD m_max;
	DWORD m_in;
};

// ==================================================================

typedef std::map<std::string, int> MapOfStringInt;
typedef std::vector<CTimeRec> VectorOfTimeRec;

class dllExport CProfiler
{
public:

	CProfiler();

	virtual ~CProfiler();

	void Enable( bool enable )  { m_enabled = enable; }

	void ResetAll();

	void In( const char* name );
	void Out( const char* name );

	double Delta( const char* name );

	void Dump( const char* label ) const;

private:  // Disabled.

	CProfiler( const CProfiler& );
	CProfiler& operator = ( const CProfiler& );
	int operator == ( const CProfiler& ) const;
	int operator != ( const CProfiler& ) const;

private:

	bool m_enabled;

	MapOfStringInt m_map;
	VectorOfTimeRec m_time_records;
};

