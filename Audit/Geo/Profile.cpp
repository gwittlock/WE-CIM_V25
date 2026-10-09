
#include "stdafx.h"
#include <math.h>
#include "cmn_resource.h"

#include "Path.h"
#include "MathConst.h"
#include "3dCoord.h"
#include "3dVec.h"
#include "3dBox.h"
#include "3x4Matrix.h"
#include "GeoCurve.h"
#include "GeoLine.h"
#include "GeoArc.h"
#include "GeoReducer.h"
#include "Solution.h"
#include "Profile.h"


// =======================================================================

CProfile::CProfile()
{
	m_attribs = NULL;
	m_propogate = false;
}

CProfile::~CProfile()
{
	delete m_attribs;

	if (CProfile::Count() > 0)
		CProfile::DestructiveFlush();
}

int CProfile::Count() const
{
	return m_list.Count();
}

void CProfile::BenignFlush()
{
	m_list.BenignFlush();
}

void CProfile::DestructiveFlush( int startIndx )
{
	int count = m_list.Count();

	for (int indx = startIndx; indx < count; ++indx)
	{
		delete m_list.Remove( startIndx );
	}
}

CGeoCurve* CProfile::GetAt( int indx ) const
{
	return m_list[ indx ];
}

int CProfile::AttribCount() const
{
	return ((m_attribs == NULL) ? 0 : m_attribs->countVar());
}

int CProfile::IntGet( const CString& name, int defval ) const
{
	return ((m_attribs == NULL) ? defval : m_attribs->getInt( name, defval ));
}

double CProfile::DoubleGet( const CString& name, double defval ) const
{
	return ((m_attribs == NULL) ? defval : m_attribs->getReal( name, defval ));
}

CString CProfile::StringGet( const CString& name, const CString& defval ) const
{
	return ((m_attribs == NULL) ? defval : m_attribs->getString( name, defval ));
}

void CProfile::IntSet( const CString& name, int ival )
{
	pAttrib()->setInt( name, ival );
}

void CProfile::DoubleSet( const CString& name, double dval )
{
	pAttrib()->setReal( name, dval );
}

void CProfile::StringSet( const CString& name, const CString& sval )
{
	pAttrib()->setString( name, sval );
}

void CProfile::AttribDelete( const CString& name )
{
	if (m_attribs != NULL)
		m_attribs->deleteVar( name );
}

const CVarList& CProfile::Attrib() const
{
	return ((m_attribs == NULL) ? CVarList::Bogus() : (*m_attribs));
}

CVarList* CProfile::pAttrib()
{
	if (m_attribs == NULL)
		m_attribs = new CVarList();

	return m_attribs;
}


CReturn CProfile::Append( CGeoCurve* curve )
{
	CReturn status;
	m_list.Append( curve );
	return status;
}

CReturn CProfile::Prepend( CGeoCurve* curve )
{
	CReturn status;
	m_list.Prepend( curve );
	return status;
}

CReturn CProfile::CopyAppend( const CGeoCurve& curve )
{
	CReturn status;

	CGeoCurve* copy = (CGeoCurve*) curve.Clone( m_propogate );

	if (copy == NULL)
		status.Fatal( IDS_MEM_ALLOC_FAILURE, "CProfile::CopyAppend(#1)" );

	if ( status.IsOk() )
		status = Append( copy );

	return status;
}

CReturn CProfile::Append( CProfile* prof )
{
	CReturn status;

	int count = prof->Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CGeoCurve* elem = prof->GetAt( indx );

		status = Append( elem );
		if ( !status.IsOk() )
			break;
	}

	prof->BenignFlush();

	return status;
}

CReturn CProfile::CopyAppend( const CProfile& prof )
{
	CReturn status;

	int startCount = CProfile::Count();
	int count = prof.Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CGeoCurve* elem = prof.GetAt( indx );

		status = CopyAppend( *elem );
		if ( !status.IsOk() )
		{
			// Restore this profile to its original state.
			CProfile::DestructiveFlush( startCount );
			break;
		}
	}

	return status;
}

void CProfile::Reverse()
{
	CProfile tmp;

	int count = Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CGeoCurve* curve = m_list.Remove( 0 );

		curve->Reverse();

		tmp.Prepend( curve );
	}

	Append( &tmp );
}

void CProfile::Split( int index )
{
	CGeoCurve* lead = m_list.GetAt( index );
	
	C3dCoord pt = lead->MidPt();
	CGeoCurve* trail = dynamic_cast<CGeoCurve*>( lead->Clone( true ) );

	lead->EndPt( pt );
	trail->StartPt( pt );

	m_list.InsertAfter( index, trail );
}

void CProfile::Reorder( int startIndex )
{
	m_list.Shift( -startIndex );
}

void CProfile::ZSet( double elevation )
{
	int count = Count();

	for (int indx = 0; indx < count; ++indx)
	{
		CGeoCurve* curve = m_list[indx];

		// This is okay because the profile owns the curves.
		C3dCoord& ps = (C3dCoord&) curve->StartPt();
		C3dCoord& pe = (C3dCoord&) curve->EndPt();

		ps.Z( elevation );
		pe.Z( elevation );

		if (curve->Type() == GEOARC)
		{
			C3dCoord& pc = (C3dCoord&) ((CGeoArc*) curve)->CenterPt();
			pc.Z( elevation );
		}
	}

}

bool CProfile::IsClosed() const
{
	return ( IsClosed( SMALL ) );
}

bool CProfile::IsClosed( double tol ) const
{
	int end = m_list.Count() - 1;

	const C3dCoord& ps = m_list[ 0 ]->StartPt();
	const C3dCoord& pe = m_list[ end ]->EndPt();

	C3dVec vec = pe - ps;

	return (vec.Length() < tol);
}

int CProfile::ConditionalClose()
{
	int end = m_list.Count() - 1;

	if (end < 0)
		return -1;	// empty profile.

	if ( IsClosed() )
		return 0;	// nothing to do.

	CGeoCurve* curveA = m_list[ 0 ];
	CGeoCurve* curveB = m_list[ end ];

	if ((curveA == curveB) && (curveA->Type() == GEOLINE))
		return -2;	// can not close


	CGeoLine* line = new CGeoLine( curveA->StartPt(), curveB->EndPt() );

	if (line == NULL)
		return -3;

	CProfile::Append( line );

	return 0;	// okay
}

// Apply colinear point elimination and merge adjacent 'like' arcs.
void CProfile::Reduce( double tol )
{
	int count = m_list.Count();
	if (count > 1)
	{
		ArcsReduce();
		LinesReduce( tol );
	}
}

C3dBox CProfile::Box3d() const
{
	C3dBox box;

	int count = m_list.Count();
	for (int indx = 0; indx < count; ++indx)
	{
		if (indx == 0)
			box = m_list.GetAt(0)->Box();
		else
			box += m_list.GetAt(indx)->Box();
	}

	return box;
}

// ============================================================================
//		Area
//
//	Calculate profile area.  Only works correctly for closed profiles.
//
//		Math reference:  (CRC) Standard Mathematic Tables & Formulae 
//
//		The area of a closed linear profile can be found by:
//			1/2 sum of (1<=i<=k) X[i]*Y[i+1] - X[i+1]*Y[i]
//
//		With arc sections, we need to adjust +- by the arc-cap area:
//			1/2 R^2(ang - sin(ang)) gives the cap area.
//
//		If the area is positive, the profile winds CCW, else it winds CW
//
#if ORIGINAL_CODE
	double CProfile::Area() const
	{
		double area = CSolution::Area( m_list );
		return area;
	}
#else
	// 2004.10.04 (PE) -- Prior to this change, the magnitude and sign of
	// the calculated area could be severely wrong.  It appears the geometry
	// must be centered about the origin for the calculate to be correct.
	double CProfile::Area() const
	{
		CReturn		status;
		C3x4Matrix	xform;

		CProfile tmp;
		tmp.CopyAppend( *this );

		C3dBox box = tmp.Box3d();
		xform.Shift( C3dVec( -box.Xc(), -box.Yc(), 0. ) );

		int count = tmp.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			tmp.GetAt(indx)->Xform( xform );
		}

		double area = CSolution::Area( tmp.m_list );

		return area;
	}
#endif

void CProfile::GapsClose()
{
	int count = m_list.Count();
	if (count > 1)
	{
		int indx = 0;

		while (1)
		{
			CGeoCurve* curveA = m_list[indx];
			++indx;

			if (indx >= m_list.Count())
				indx = 0;

			CGeoCurve* curveB = m_list[indx];

			const C3dCoord& ptA = curveA->EndPt();
			const C3dCoord& ptB = curveB->StartPt();

			if ( !ptA.WithinTol( ptB, SMALL ) )
			{
				m_list.InsertBefore( indx, new CGeoLine( ptA, ptB ) );

				if (indx != 0)
					++indx;
			}

			if (indx == 0)
				break;
		}
	}
}

const C3dCoord* CProfile::StartPt() const
{
	if (Count() < 1)
		return NULL;

	CGeoCurve* curve = m_list[0];

	const C3dCoord& pt = curve->StartPt();

	return &pt;
}

const C3dCoord* CProfile::EndPt() const
{
	int count = Count();

	if (count < 1)
		return NULL;

	CGeoCurve* curve = m_list[ count-1 ];

	const C3dCoord& pt = curve->EndPt();

	return &pt;
}

// Pilfered from CGeoPoly::Dump()
void CProfile::Dump() const
{
	static const char* lineTemplate =
		"Create:Line: action=0, sncs=101, id = 0, xs=%f, ys=%f, zs=0, xe=%f, ye=%f, ze=0\n";

	static const char* arcTemplate =
		"Create:Arc: action=CICREATE, sncs=205, xs=%f, ys=%f, zs=0, xe=%f, ye=%f, ze=0, xc=%f, yc=%f, zc=0, dir=%d\n";

	CString path = CPath::DebugDir() + "\\profile.log";

	FILE* f = fopen( (LPCSTR) path, "w" );
	if (f != NULL)
	{
		CString buf;

		fprintf( f, "admin:prepare:\n" );

		int count = m_list.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			CGeoCurve* geoCurve = m_list.GetAt( indx );
			const C3dCoord& ps = geoCurve->StartPt();
			const C3dCoord& pe = geoCurve->EndPt();
			if (geoCurve->Type() == GEOARC)
			{
				CGeoArc* geoArc = dynamic_cast<CGeoArc*>( geoCurve );
				const C3dCoord& pc = geoArc->CenterPt();

				buf.Format( arcTemplate, ps.X(), ps.Y(), pe.X(), pe.Y(), pc.X(), pc.Y(), geoArc->Dir() );
			}
			else
			{
				buf.Format( lineTemplate, ps.X(), ps.Y(), pe.X(), pe.Y() );
			}

			fprintf( f, buf );
		}

		fprintf( f, "admin:commmit:\n" );

		fclose( f );
	}
}

// Merge 'like' arcs.
// ASSUMPTION: Profile entities have proper order and direction.
void CProfile::ArcsReduce()
{
	CGeoReducer::ArcsReduce( m_list, true, SMALL );
}

// Colinear point reduction.
// ASSUMPTION: Profile entities have proper order and direction
// and are well behaved (as in not laying on one another).
void CProfile::LinesReduce( double tol )
{
	CGeoReducer::LinesReduce( m_list, false, tol );
}

// v20.0.90.1 -- Introduced for ITI because the DXF explode function that Gary uses
// created really goofy geometry at the start/end boundary of the profile. Dunno
// why it does that because the original profile is simply lines and arcs. The
// exploding is intended only for ellipses.
//
// By "goofy" I mean a bunch of tiny zig-zagging lines are created. Said geometry
// wreaks havoc on the offsetter.
//
// NOTE: Much of the duplicate code seen in this function could be repackaged
// into a single (limited) reusable function.
void CProfile::Clean()
{
	C2dUnitVec tanA;
	C2dUnitVec tanB;
	CGeoLine* lineA;
	CGeoLine* lineB;
	double dot;
	int indxA, indxB;

	double TOL = 1.e-5;  // arbitrary tolerance
	bool initially_closed = IsClosed( TOL );
	int initial_count = m_list.Count();

	indxA = 0;
	while (1)
	{
		indxB = indxA + 1;
		if (indxB >= m_list.Count())
			break;  // we've exhausted the input (shouldn't happen)

		lineA = dynamic_cast<CGeoLine*>( m_list[indxA] );
		lineB = dynamic_cast<CGeoLine*>( m_list[indxB] );
		if ((lineA == NULL) || (lineB == NULL))
			break;  // this function is applicable to a pair of lines only

		tanA = lineA->StartTan();
		tanB = lineB->StartTan();

		dot = tanA * tanB;

		if (dot > -0.99999)  // arbitrary tolerance
			break;  // 'cause we're looking for backtracks

		// Do more detailed testing, eg. is the start point of lineA on lineB?

		delete m_list.Replace( indxA, NULL );

		++indxA;
	}

	if (indxA > 0)
		m_list.Compact();

	//-------------

	indxB = m_list.Count() - 1;
	while (1)
	{
		indxA = indxB - 1;
		if (indxA < 0)
			break;  // we've exhausted the input (shouldn't happen)

		lineB = dynamic_cast<CGeoLine*>( m_list[indxB] );
		lineA = dynamic_cast<CGeoLine*>( m_list[indxA] );
		if ((lineA == NULL) || (lineB == NULL))
			break;  // this function is applicable to a pair of lines only

		tanB = lineB->StartTan();
		tanA = lineA->StartTan();

		dot = tanA * tanB;

		if (dot > -0.99999)  // arbitrary tolerance
			break;  // 'cause we're looking for backtracks

		// Do more detailed testing, eg. is the start point of lineA on lineB?

		delete m_list.Replace( indxB, NULL );

		--indxB;
	}

	if (indxB < (m_list.Count() - 1))
		m_list.Compact();

	int final_count = m_list.Count();
	if (initially_closed && (final_count < initial_count))
	{
		// Deletions occurred.
		if ( IsClosed( TOL ) )
		{
			CGeoCurve* curveA = m_list[0];
			CGeoCurve* curveB = m_list[final_count-1];

			const C3dCoord ps = m_list[0]->StartPt();
			const C3dCoord pe = m_list[m_list.Count() - 1]->EndPt();

			if ((curveA->Type() == GEOLINE) && (curveB->Type() == GEOLINE))
			{
				curveB->EndPt( ps );
			}
			else if ((curveA->Type() == GEOLINE) && (curveB->Type() != GEOLINE))
			{
				curveA->StartPt( pe );
			}
			else if ((curveA->Type() != GEOLINE) && (curveB->Type() == GEOLINE))
			{
				curveB->EndPt( ps );
			}
		}
	}
}
