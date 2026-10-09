
#include "stdafx.h"
#include "MathConst.h"
#include "Nurb.h"


//zero tolerance. This has been tweaked to this value, because
//of the roundoff MS inflicts.

static double NURB_SMALL = 1.0e-8;


//////////////////////////////////////////////////////
//
//
CNurb::CNurb()
	: m_rational(false),
	  m_dataChecked(false),
	  m_basisFunc(),
	  m_cpts(),
	  m_wts()
{

}

//////////////////////////////////////////////////////
//
//
CNurb::~CNurb()
{

}

//////////////////////////////////////////////////////
//
//
CNurb::CNurb(int deg, int numPts)
	:m_rational(false),
	 m_dataChecked(false),
	 m_basisFunc(deg,numPts + deg + 1),
	 m_cpts(numPts),
	 m_wts(numPts,1)
{
	ASSERT(deg > 0);
	ASSERT(numPts>=deg);
}


//////////////////////////////////////////////////////
//
//
CNurb::CNurb(const CNurb & nurb)
{
	*this = nurb;
}

//////////////////////////////////////////////////////
//
//
CNurb& CNurb::operator =(const CNurb & nurb)
{
	//check for self assignment
	if(this != &nurb)
	{
		m_rational = nurb.m_rational;
		m_dataChecked = nurb.m_dataChecked;
		m_basisFunc = nurb.m_basisFunc;
		m_cpts = nurb.m_cpts;
		m_wts = nurb.m_wts;
	}
	return *this;
}

//////////////////////////////////////////////////////
//
//
bool CNurb::isValid() const
{
	return m_basisFunc.isValid();
}

//////////////////////////////////////////////////////
//
//
bool CNurb::validNurb() const
{
	//proceed if we need to check
	if(!m_dataChecked)
	{
		//check to see if we are default constructed
		if(!isValid()) return false;

		//validate the knot vector
		if(!validKnots()) return false;

		// check control point are intialized
		for(int indx=0; indx<numControlPoints(); ++indx) 
		{
			if(getPoint(indx).X() == UNDEFINED)
				return false;
		}

		//set check status
		m_dataChecked = true;
	}
	return true;
}

//////////////////////////////////////////////////////
//
//
bool CNurb::validKnots() const
{
	return m_basisFunc.validKnots();
}

//////////////////////////////////////////////////////
//
//
int CNurb::degree() const
{
	ASSERT(isValid());
	return m_basisFunc.degree();
}

//////////////////////////////////////////////////////
//
//
int CNurb::order() const
{
	ASSERT(isValid());
	return m_basisFunc.order();
}


//////////////////////////////////////////////////////
//
//
bool CNurb::rational() const
{
	ASSERT(isValid());
	return m_rational;
}

//////////////////////////////////////////////////////
//
//
double CNurb::tMin() const
{
	ASSERT(validKnots());
	return m_basisFunc.tMin();
}


//////////////////////////////////////////////////////
//
//
double CNurb::tMax() const
{
	ASSERT(validKnots());
	return m_basisFunc.tMax();
}


//////////////////////////////////////////////////////
//
//
int CNurb::numKnots() const
{
	ASSERT(isValid());
	return m_basisFunc.numKnots();
}


//////////////////////////////////////////////////////
//
//
int CNurb::numControlPoints() const
{
	ASSERT(isValid());
	return m_cpts.size();
}

//////////////////////////////////////////////////////
//
//
C3dCoord CNurb::start() const
{
	ASSERT(validNurb());
	return evaluate(tMin());
}

//////////////////////////////////////////////////////
//
//
C3dCoord CNurb::end() const
{
	ASSERT(validNurb());
	return evaluate(tMax());
}

//////////////////////////////////////////////////////
//
//
void CNurb::setPoint( int indx, const C3dCoord& pt )
{
	ASSERT(isValid());
	ASSERT(indx>=0 && indx<numControlPoints());

	//only multiply by weight if rational
	m_dataChecked = false;

	//if the curve is rational we need to multiply the 
	//point by the weight
	if ( rational() )
	{
		m_cpts[indx].X( pt.X() * m_wts[indx] );
		m_cpts[indx].Y( pt.Y() * m_wts[indx] );
		m_cpts[indx].Z( pt.Z() * m_wts[indx] );
	}
	else
		m_cpts[indx] = pt;
}

//////////////////////////////////////////////////////
//
//
C3dCoord CNurb::getPoint(int indx) const
{
	ASSERT(isValid());
	ASSERT(indx>=0 && indx<numControlPoints());

	C3dCoord pt;

	// control point are product of weights and poitns
	// so need to divide by the weight if rational
	if ( rational() )
	{
		double weight = getWeight(indx);
		pt.X( m_cpts[indx].X() / weight );
		pt.Y( m_cpts[indx].Y() / weight );
		pt.Z( m_cpts[indx].Z() / weight );
	}
	else
		pt = m_cpts[indx];

	return pt;
}

//////////////////////////////////////////////////////
//
//
void CNurb::setWeight( int indx, double weight )
{
	ASSERT(isValid());
	ASSERT(indx>=0 && indx<numControlPoints());
	ASSERT(weight > 0.0);

	// check if the control point has been set
	//if it has then we also need to update the control points
	if(m_cpts[indx].X() != UNDEFINED)
	{
		C3dCoord pt = getPoint(indx);
		m_cpts[indx].X( pt.X() * weight );
		m_cpts[indx].Y( pt.Y() * weight );
		m_cpts[indx].Z( pt.Z() * weight );
	}
	m_rational = true;
	m_wts[indx] = weight;
}

//////////////////////////////////////////////////////
//
//
double CNurb::getWeight(int indx) const
{
	ASSERT(isValid());
	ASSERT(indx>=0 && indx<numControlPoints());
	return m_wts[indx];
}

//////////////////////////////////////////////////////
//
//
void CNurb::setKnot(int indx, double knot)
{
	ASSERT(isValid());
	ASSERT(indx>=0 && indx<numKnots());
	m_basisFunc.setKnot(indx,knot);
}

//////////////////////////////////////////////////////
//
//
double CNurb::getKnot(int indx) const
{
	ASSERT(isValid());
	ASSERT(indx>=0 && indx<numKnots());
	return m_basisFunc.getKnot(indx);
}

//////////////////////////////////////////////////////
//
//
C3dCoord CNurb::evaluate(double t) const
{
	ASSERT(validNurb());
	ASSERT((t-tMin()) >= -NURB_SMALL && (t-tMax()) <= NURB_SMALL);

	//choose evaluation type if rational
	return (rational() ? rat_eval(t) : non_rat_eval(t));
}

//////////////////////////////////////////////////////
//
//A point on a non rational curve is the product of the
//control points and the basis(indx) evaluated at value t
C3dCoord CNurb::non_rat_eval( double t ) const
{
	ASSERT(validNurb());
	ASSERT((t-tMin()) >= -NURB_SMALL && (t-tMax())<=NURB_SMALL);
	C3dCoord sum( 0., 0., 0. );

	for(int indx=0; indx<numControlPoints(); ++indx) 
	{
		//get basis val at currrent paremeter
		double basisVal = m_basisFunc.basis(indx,t);

		//only form product if the basis value is non zero
		if(basisVal != 0.0)
		{
			C3dCoord pt = getPoint(indx);
			sum.X( sum.X() + (pt.X() * basisVal) );
			sum.Y( sum.Y() + (pt.Y() * basisVal) );
			sum.Z( sum.Z() + (pt.Z() * basisVal) );
		}
	}   
	return sum; 
}

//////////////////////////////////////////////////////
//
//First form product of homogeneous points 
//(weights*points) and basis functions. 
//Form product of weights and basis functions.
//Divide point product by weight product
C3dCoord CNurb::rat_eval(double t) const
{
	ASSERT(validNurb());
	ASSERT((t-tMin()) >= -NURB_SMALL && (t-tMax())<=NURB_SMALL);
	C3dCoord sum( 0., 0., 0. );
	double weight = 0.0;

	for(int indx=0; indx<numControlPoints(); ++indx) 
	{
		//get basis functions
		double basisVal = m_basisFunc.basis(indx,t);

		//only form products for non zero basis values
		if(basisVal != 0.0)
		{
			C3dCoord pt = m_cpts[indx];
			sum.X( sum.X() + (pt.X() * basisVal) );
			sum.Y( sum.Y() + (pt.Y() * basisVal) );
			sum.Z( sum.Z() + (pt.Z() * basisVal) );

			weight += getWeight(indx) * basisVal;
		}
	}

	sum.X( sum.X() / weight );
	sum.Y( sum.Y() / weight );
	sum.Z( sum.Z() / weight );

	return sum; 
}


