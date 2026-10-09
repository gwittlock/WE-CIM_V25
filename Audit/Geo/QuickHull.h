#ifndef _QUICKHULL_H
#define _QUICKHULL_H

#include <vector>
using namespace std;

#include "3dCoord.h"
#include "2dUnitVec.h"

typedef vector<const C2dCoord*> Tchvector;

void DestructiveClear( Tchvector* vec );

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class dllExport CQuickHull
{
public:

	CQuickHull();

	~CQuickHull();

	void QuickHull( const Tchvector& points, bool ccw, Tchvector* hull );

private:

	void QuickPartition( const Tchvector& P, const C2dCoord* l, const C2dCoord* r, int faceDir );
	int QuickExtreme( const Tchvector& P, const C2dCoord* l, const C2dCoord* r );
	bool HullPointAppend( const C2dCoord* pt, Tchvector* hull );

private:

	Tchvector	m_hull;
};

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

enum EChSide
{
	NEITHER = 0,
	SLEFT =    1,
	SRIGHT =   2
};

class chline
{
public:

	chline( const C2dCoord* ps, const C2dCoord* pe );

	~chline()  { }

	const C2dCoord& StartPt() const  { return (*m_ps); }

	const C2dCoord& EndPt() const    { return (*m_pe); }

	EChSide WhichSide( const C2dCoord& pi ) const;

	double DistanceTo( const C2dCoord& pi ) const;

private:

	double Delta( double a, double b ) const;

private:  // Disabled.

	chline();

private:

	const C2dCoord*	m_ps;
	const C2dCoord*	m_pe;

	C2dUnitVec	m_vec;
};

#endif
