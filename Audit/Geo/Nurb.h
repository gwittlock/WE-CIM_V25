// CNurb.h: interface for the CNurb class.
//
// Description: This reperesents a general nurb class. The default 
//              for this class is non-rational (behaves like a nurb)
//              and all weights have value one. The curve becomes
//              rational if a weight is set to other than one. This 
//              improves efficienct, because all calculations are
//              done on coordinates only. For the rational case
//              calculations must be performed on weights also.
//
//////////////////////////////////////////////////////////////////////

#ifndef _NURB_H
#define _NURB_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Vector>
//#include <Valarray>
#include <Limits>
#include "BSplineBasis.h" 
#include "3dCoord.h"

class dllExport CNurb  
{
	friend class TestCNurb;

public:
	//----------
	//Construction
	//----------

	//Make nurb of degree deg with prescribed number of control points
	//The default nurb is non-rational (all weights are 1.)
	//Precodition: deg > 0
	//Precondition:numPts >= deg
	CNurb(int deg, int numPts);

	//Default constructor. The nurb will be invalid
	//unless control points and knots are input.
	CNurb();

	CNurb& operator=(const CNurb& nurb);
	CNurb(const CNurb& nurb);
	// copy/assign

	//----------
	//validation
	//----------
	// These functions should be called before operations
	// requiring the nurb in a particular state of validity

	bool validKnots() const;
	// are the knots valid
	// ie. all initialized, positive and increasing

	bool validNurb() const;
	// is  the nurb valid
	// ie.
	// isValid = true
	// validKnots == true
	// all control points intialized

	bool isValid() const;
	// is the nurb constructed, ie. not default constructed

	//----------
	//enquirires
	//---------

	int dimension() const;
	// dimension of the nurb, ie. 2D, 3D etc

	int numControlPoints() const;
	//number of control points in the nurb
	//Precondtion: isValid == true

	int numKnots() const;
	//number of knots in the nurb (number of control points + order)
	// Precondtion: isValid == true

	bool rational() const;
	//is curve rational, ie. not all the weights have value 1
	//Precondtion: isValid == true

	int order() const;
	int degree() const;
	//order/degree of nurb. Order = degree + 1
	//isValid==true


	//----------
	//evaluation
	//----------

	C3dCoord end() const;
	C3dCoord start() const;
	// start/end of curve
	// Precondtion: validNurb == true

	C3dCoord evaluate(double t)  const;
	//get a point at parameter t
	//Precondtion: validNurb == true
	//Precondition: t>= t_min && t<=t_tmax


	//----------
	//knots
	//----------
	double getKnot(int indx) const;
	void setKnot(int indx, double knot);
	// get / set the ith knot
	//Precondition: isValid = true
	//Pecondition: indx>=0 && indx<numKnots()

	double tMax() const;
	double tMin() const;
	// min/max parameter values. Note this is 
	// not the largest of the values of the knots, it is 
	// the value of the parameter where the curve starts and ends
	// Precondtion: validKnots = true

	//----------
	//points and weights
	//----------
	double getWeight(int indx) const;
	void setWeight(int indx, double weight);
	// get/set the weights
	//Precondition: weight > 0
	//Precondition: isValid == true
	//Precondition: indx>=0 && indx<numPoints

	C3dCoord getPoint(int indx) const;
	void setPoint(int indx, const C3dCoord& p);
	// get/set a point
	//Precondition: isValid == true
	//Precondition: indx>=0 && indx<numPoints

	virtual ~CNurb();

private:

	//evaluation functions in H-space
	C3dCoord rat_eval(double t)  const;
	C3dCoord non_rat_eval(double t) const;

	//flag tracking if curve is rational
	bool m_rational;

	//tells us if the data has been checked or 
	//needs rechecking
	mutable bool m_dataChecked;

	//B-slpine basis functions
	CBSplineBasis m_basisFunc;

	//control points. Note* for efficiency, this vector 
	//constains the product of the control point coordinates
	//and the weights 
	std::vector< C3dCoord > m_cpts;

	//weights
	std::vector<double> m_wts;
};

#endif
