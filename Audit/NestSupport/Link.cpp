
#include "stdafx.h"
#include "Link.h"

// ==================================================================

CLink::CLink()
{
	m_hit = -1;
	m_priority = 0.;
	m_delta.Init( 0, 0 );
	m_prelink = false;
	m_bump_score = 0.;
}

CLink::CLink( const CLink& rhs )
{
	(*this) = rhs;
}

CLink::CLink( int hit, double priority, C2dVec& delta )
{
	m_hit = hit;
	m_priority = priority;
	m_delta = delta;
	m_prelink = false;
	m_bump_score = 0.;
	m_overlap_score = 0.;
}

CLink::~CLink()
{
}

CLink& CLink::operator = ( const CLink& rhs )
{
	m_hit = rhs.m_hit;
	m_delta = rhs.m_delta;
	m_priority = rhs.m_priority;
	m_prelink = rhs.m_prelink;
	m_bump_score = rhs.m_bump_score;
	m_overlap_score = rhs.m_overlap_score;

	return (*this);
}

bool CLink::IsEqual( const CLink& rhs, double tol )
{
	C2dVec vec = m_delta - rhs.m_delta;
	return ((fabs( vec.X() ) <= tol) && (fabs( vec.Y() ) <= tol));
}


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void CLinkArray::Sort()
{
	this->Qsort( CLinkArray::DecreasingScore );
}

int CLinkArray::DecreasingScore( const void* ptrA, const void* ptrB )
{
	static double TOL = 1.e-5;  // arbitrary

	CLink* linkA = (*(CLink**) ptrA);
	CLink* linkB = (*(CLink**) ptrB);

	double scoreA = linkA->BumpScore();
	double scoreB = linkB->BumpScore();

	double diff = scoreA - scoreB;
	if (fabs(diff) < TOL)
	{
		scoreA = linkA->OverlapScore();
		scoreB = linkB->OverlapScore();

		diff = scoreA - scoreB;
	}

	return ((diff > 0) ? -1 : 1);
}
