
#ifndef _STDCURVEFILTER_H
#define _STDCURVEFILTER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "DbCurve.h"
#include "DbCurveList.h"
#include "Model.h"


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#define SCF_TSTHIDDEN	0x01
#define SCF_TSTLAYER	0x02
#define SCF_TSTOWNER	0x04
#define SCF_TSTLOOSE	0x08

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// NOTES:
//
// 1) Calls to Init() reinitialize the internal list of curves.
// 2) The seed curve is added to the internal list.
//
class dllExport CStdCurveFilter
{
public:

	CStdCurveFilter();

	// Flags to Control the Behavior... Danger, Will Robinson, Danger!
	// All init to TRUE.
	// Test the hidden flag?
	void	TestHidden( bool test )		{ m_testflag = (test?(m_testflag|SCF_TSTHIDDEN):(m_testflag&~SCF_TSTHIDDEN)); }
	bool	TestHidden( void )			{ return ((m_testflag&SCF_TSTHIDDEN) != 0); }

	// Test that the layers match?
	void	TestLayer( bool test )		{ m_testflag = (test?(m_testflag|SCF_TSTLAYER):(m_testflag&~SCF_TSTLAYER)); }
	bool	TestLayer( void )			{ return ((m_testflag&SCF_TSTLAYER) != 0); }

	// Test that the owners match?
	void	TestOwner( bool test )		{ m_testflag = (test?(m_testflag|SCF_TSTOWNER):(m_testflag&~SCF_TSTOWNER)); }
	bool	TestOwner( void )			{ return ((m_testflag&SCF_TSTOWNER) != 0); }

	// If TRUE, we test loose pieces as if they were owned.  If FALSE,
	// we *don't* test the ownership of loose pieces.  Sorry for the convoluted logic.
	void	TestLoose( bool test )		{ m_testflag = (test?(m_testflag|SCF_TSTLOOSE):(m_testflag&~SCF_TSTLOOSE)); }
	bool	TestLoose( void )			{ return ((m_testflag&SCF_TSTLOOSE) != 0); }

	// Find all curves in the model having the same
	// layer and owner as the given seed curve.
	void Init( const CModel& model, CDbCurve* dbSeedCurve );

	// Find all curves in the list of input curves having
	// the same layer and owner as the given seed curve.
	// Accepted curves are removed from the list of input curves.
	void Init( CDbCurveList* inputCurves, CDbCurve* dbSeedCurve );

	CDbCurveList& Curves();
	CGeoCurveList* GeoCurves();	// Allocates both the list and the curves

	virtual ~CStdCurveFilter();

protected:

private:  // Methods

	virtual bool Add( CDbCurve* dbCurve );

private:  // Disabled

	CStdCurveFilter( const CStdCurveFilter& );
	const CStdCurveFilter& operator = ( const CStdCurveFilter& );
	int operator == ( const CStdCurveFilter& ) const;
	int operator != ( const CStdCurveFilter& ) const;

private:  // Data

	CDbCurveList m_list;
	CDbCurve*	m_seed;
	BYTE		m_testflag;
};

#endif

