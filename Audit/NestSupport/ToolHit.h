#if !defined(_TOOLHIT_H)
#define _TOOLHIT_H

// ==================================================================
//	Support class for NestingPart -- holds the various levels of
//	GeoPoly outlines from the tool hits for each part orientation.
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

#include "Common.h"
#include "VarList.h"
#include "GeoPoly.h"
#include "DexGrid.h"
#include "Link.h"

class CNestingPart;

// ==================================================================

enum eToolhit
{
	HIT_NEST_OUTSIDE	= 0,  // includes outside kerf + spacing
	HIT_PART_OUTSIDE	= 1,
	HIT_PART_INSIDE		= 2,
	HIT_NEST_INSIDE		= 3,  // includes inside kerf + spacing
	HIT_NEST_LIMIT		= 4,  // ------ limit ------
	HIT_KERF_OUTSIDE	= 4,
	HIT_KERF_INSIDE		= 5,
	HIT_LIMIT			= 6   // ------ limit ------
};

// ==================================================================

class dllExport CToolHit
{
public:

	CToolHit( void );
	~CToolHit( void );

	CNestingPart* Part() const		{ return m_part; }
	void Part( CNestingPart* part )	{ m_part = part; }

	double Height( void ) const		{ return m_height; }
	void Height( double val )		{ m_height = val; }


	int	Index( void ) const			{ return m_hit; }
	void Index( int idx )				{ m_hit = idx; }

	double Rotation( void ) const	{ return m_rot; }
	void Rotation( double rot )		{ m_rot = rot; }

	bool Mirror( void ) const		{ return m_mirror; }
	void Mirror( bool mirror )		{ m_mirror = mirror; }

	const C2dCoord& Pos( void ) const	{ return m_pos; }
	void Pos( C2dCoord& pos )			{ m_pos = pos; }

	// PolysGet() replaces 'operator[]' because it use of the operator
	// makes it difficult to find instances of its use.
	CGeoPolyArray* PolysGet(int depth) const;

	void PolyAdd( int depth, CGeoPoly* poly );

	CDexGrid* Grid(void) const				{ return m_dexgrid; }
	void Grid( CDexGrid* grid );

	bool hasPatterns( void ) const				{ return m_has_patterns; }
	void hasPatterns( bool flag )				{ m_has_patterns = flag; }

	CDbPattern* PatternBurn( void ) const		{ return m_pat_burn; }
	void PatternBurn( CDbPattern* pattern )		{ m_pat_burn = pattern; }

	CDbPattern* PatternPunch( void ) const		{ return m_pat_punch; }
	void PatternPunch( CDbPattern* pattern )		{ m_pat_punch = pattern; }

	CDbPattern* PatternOther( void ) const		{ return m_pat_other; }
	void PatternOther( CDbPattern* pattern )	{ m_pat_other = pattern; }

	C2dCoord WorldToGrid( double x_world, double y_world ) const;

	C2dCoord GridToWorld( const C2dCoord& grid_pt ) const;

	int LinksCount(void) const				{ return m_link_array.Count(); }
	void LinkCopyAppend( const CLink& link );
	void LinksSort();
	CLink* LinkGet(int idx) const			{ return m_link_array.GetAt( idx ); }
	void LinksDestroy()						{ m_link_array.DestructiveFlush(); }

	void Delta( const C2dVec& delta )	{ m_delta = delta; }
	const C2dVec& Delta() const			{ return m_delta; }

	void Mer( const C2dBox& mer )		{ m_mer = mer; }
	const C2dBox& Mer() const			{ return m_mer; }


	CReturn DebugToModel( const C3dCoord& shift, CViewMgr* view, CModel* model );

	void PartialCopy( const CToolHit& source, const C2dCoord& );

	bool JustPlaced() const;
	void JustPlaced( bool just_placed );

	CReturn ExtremesExtract( eNestProgression progression );
	const CGeoPoly* ExtremesGet() const	{ return m_extremes; }

	// Attribute managment methods.
	int AttribCount() const;

	int IntGet( const CString& name, int defval ) const;
	double DoubleGet( const CString& name, double defval  ) const;
	CString StringGet( const CString& name, const CString& defval ) const;

	void IntSet( const CString& name, int ival );
	void DoubleSet( const CString& name, double dval );
	void StringSet( const CString& name, const CString& sval );

	void AttribsDelete();
	void AttribDelete( const CString& name );

	// Low-level methods (caution).
	const CVarList& Attrib() const	{ return m_attribs; }
	CVarList* pAttrib()				{ return &m_attribs; }

private:

	CGeoPoly* PolyGet(
		const CGeoPolyArray&	src_array,
		int						depth,
		int						poly_indx );

	CGeoPoly* TRExtremesExtract( const CGeoPoly& poly );
	CGeoPoly* BRExtremesExtract( const CGeoPoly& poly );

private:
	// Geometric information
	CGeoPolyArray*	m_profile[HIT_LIMIT];
	CGeoPoly*		m_extremes;
	CDexGrid*		m_dexgrid;

	bool			m_has_patterns;
	CDbPattern*		m_pat_punch;
	CDbPattern*		m_pat_burn;
	CDbPattern*		m_pat_other;

	double			m_height;

	// The part from which this object was derived.
	CNestingPart*	m_part;

	// The "hit index" of this object in the part.
	// Each "hit index" represents a CToolHit object
	// having a different orientation.
	int				m_hit;		// Hit index in the part

	// TRUE if this is a mirrored part relative to the original
	bool			m_mirror;

	// Rotation angle of this toolhit relative to the original
	double			m_rot;

	// Where the hit handle was (world)
	C2dCoord		m_pos;

	// Link to zero or more prioritized "next hit" links.
	// Replaces the "next_*()" system of calls.  Also
	// supports pre-nesting with critical-priority links.
	CLinkArray		m_link_array;

	C2dBox			m_mer;	// minimum enclosing rectangle of outer-kerf.
	C2dVec			m_delta;

	bool			m_just_placed;

	CVarList		m_attribs;
};

typedef CDynamicArray<CToolHit*> CToolHitArray;


#endif