// CBSplineBasis.h: interface for the CBSplineBasis class.
//
//Description: This class represents the b-spline basis functions
//             The class has caching, so that the basis functions
//             are calculated only if they have not already been
//             calculated or the pareamter value at which the
//             basis was calculated has changed
//////////////////////////////////////////////////////////////////////

#ifndef _BSPLINEBASIS_H
#define _BSPLINEBASIS_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "stdafx.h"

// STL includes
#include <Vector>




class dllExport CBSplineBasis  
{
	friend class TestCBSplineBasis;

public:

	//----
	//construction
	//----
	
	CBSplineBasis();

	// specify degree of basis and number of knots
	// Precondition:
	// deg > 0 
	// numKnts >= 2*(degree+1)
	CBSplineBasis(int deg, int numKnts);

	// copying/assignment
	CBSplineBasis(const CBSplineBasis& basis);
	CBSplineBasis& operator=(const CBSplineBasis& basis);

	//----
	//validation
	//----

	// is instance valid, i.e. not default constructed
	// true == valid
	bool isValid() const;

	// are the knots valid.
	// knots are valid if 
	// isValid == true
	// all knots are initialized
	// the knot values are increasing in order (multiple knots allowed).
	// Note* this function should be called for any operation requiring
	// a valid knot set
	bool validKnots() const;


	//----
	//enquiries
	//----

	// get min max knot values note these
	// are not the smallest and largest knot values
	// they are values for which the basis is defined over
	// Precondition:
	// validKnot == true
	double tMin() const;
	double tMax() const;

	// number of knots in vector
	// Precondition:
	// isValid == true
	int numKnots() const;

	// degree of the basis
	//Precondition:
	// isValid == true
	int degree() const;

	// get the order of the basis (oredr = degree+1)
	//Precondition:
	//isValid == true
	int order() const;

	//----
	//knots
	//----

	// get the interval containg the value t
	//Example: for knots 0,1,2,3,4 the interval for t=3.5 is 3 
	//(lies between the span [3,4)
	//Precondition:
	//t>=tMin && t<=tMax
	//validKnots == true
	int interval(double t) const;

	// get the ith knot
	// Precondition:
	// isValid == true
	// i >=0 && i<numKnots
	double getKnot(int i) const;

	// set the ith knot
	//Precondition:
	//isValid == true
	// i >=0 && i<numKnots
	// val >=0.0
	void setKnot(int i, double val);

	//----
	//evaluation
	//----

	// not implemented
	double basisDeriv(double t) const;

	// get the ith basis func at parameter t
	// Precondition:
	//validKnots == true
	//i>=0 && i<numKnots
	//t >=tMin && t<=tMax
	double basis(int i, double t) const;

	virtual ~CBSplineBasis();

private:

	//cache basis values. Calcualtes all basis 
	//function values for paramter value t
	void basisFuncs(double t) const;

	// degree of basis
	int m_degree;

	// flag for knot checking
	mutable bool m_knotChecked;

	// basis cache. Note the numknots - order position
	//in this array is used to  cache the paramer value
	//at which the basis were evaluated
	mutable std::vector<double> m_basisFuncCache;

	// knots
	std::vector<double> m_knots;
};

#endif
