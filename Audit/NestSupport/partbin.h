#if !defined(_PARTBIN_H)
#define _PARTMIN_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "common.h"
#include "DaoDB.h"

#if (_NST)
#include "CiModel.h"
#endif

#include "NestingPart.h"

// ==================================================================

class dllExport CPartBin
{
public:

	CPartBin();

	CReturn Load( const CNestConfig& config );

	CReturn Clone( const CNestConfig& config, CModel& model );
	int AddSelection( const CNestConfig& config, CModel& model );
	int AddFile( const CNestConfig& config );

	const CString& Database( void ) const		{ return m_partdb; }
	void Database( const CString& in_name )		{ m_partdb = in_name; }

	int Count( void ) const						{ return m_parts.Count(); }
	CNestingPart* GetAt( int in_idx ) const		{ return m_parts[in_idx]; }

	// Do not call until all of the parts have been loaded.
	void PartsSort( eNestOrder part_order );

	virtual ~CPartBin();

private:

	static int DecreasingSize( const void* ptrA, const void* ptrB );
	static int IncreasingSize( const void* ptrA, const void* ptrB );
	static int DecreasingPriority( const void* ptrA, const void* ptrB );

private:
	// Disabled.
	CPartBin( const CPartBin& );
	const CPartBin& operator = ( const CPartBin& );
	int operator == ( const CPartBin& ) const;
	int operator != ( const CPartBin& ) const;

private:

	CReturn load_names( const CString& in_filename, int factor );
	CReturn load_parts( const CNestConfig& config );

	CReturn load_ci_names( const CString& in_filename, int factor );

#if (_NST)
	void LayerMapParamsGet(
					const CNestConfig&	config,
					CVarList*			lm_params );

	void LayerMapGet(
					const CNestConfig&	config,
					CVarList*			layers );

	CReturn	ProfilesCreate(
					const CVarList&	allowed_layers,
					const CVarList&	lm_params,
					CCiModel*		ciModel );
#endif

private:

	CString				m_partdb;

	CNestingPartArray	m_parts;
};

#endif
