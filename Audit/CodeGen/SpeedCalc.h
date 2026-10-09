
#ifndef _SPEEDCALC_H
#define _SPEEDCALC_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef _VARLIST_H
#include "VarList.h"
#endif


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Create to support 'speed ramping', extracts all of the relevant
// parameters from an attribute list and generates a feedrate table.
//
class dllExport CSpeedCalc
{
public:

	CSpeedCalc();

	// sfparams must have the following attributes:
	//    "Slowdown Setback"
	//    "Slowdown Divisions"
	//    "Slowdown Formula"
	//    "Feed"
	// Returns false when given bad/missing data.
	bool Init( const CVarList& sfparams );

	// Determines whether this object has a valid feedrate table
	// Returns false when Init() is given bad/missing data.
	bool IsOk() const					{ return m_okay; }

	// Gets the setback distance (this distance away from a
	// sharp corner at which speed ramping should start).
	double Setback() const				{ return m_setback; }

	// Gets the number of intervals in the setback distance.
	int Divisions() const				{ return m_ndivs; }

	// Gets the value by which to reduce the feedrate per interval.
	double Factor() const				{ return m_factor; }

	// Gets the maximum feedrate.
	double MaxFeed() const				{ return m_maxfeed; }

	// Gets the length of an interval.
	double IntervalLength() const		{ return (m_setback / m_ndivs); }

	// Gets the feedrate for the ith interval. Note, the feedrate
	// table is zero-based and is sorted in descending order.  That
	// is, table[0] = max_feedrate
	double Feed( int interval_indx ) const;

	virtual ~CSpeedCalc();

protected:

private:  // Methods

private:  // Disabled

	CSpeedCalc( const CSpeedCalc& );
	const CSpeedCalc& operator = ( const CSpeedCalc& );
	int operator == ( const CSpeedCalc& ) const;
	int operator != ( const CSpeedCalc& ) const;

private:  // Data

	CArray<double,double> m_feedrates;

	int m_ndivs;

	double m_setback;
	double m_factor;
	double m_maxfeed;

	bool m_okay;
};

#endif

