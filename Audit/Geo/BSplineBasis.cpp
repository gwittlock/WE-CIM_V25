
#include "BSplineBasis.h"
#include <Algorithm>
#include <Limits>
#include "assert.h"

const double BSPLINE_SMALL = 1.e-8;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////
//
//
CBSplineBasis::CBSplineBasis()
	:m_degree(0),
	 m_knotChecked(false),
	 m_basisFuncCache(),
	 m_knots()
{

}

//////////////////////////////////////////////////////
//
//
CBSplineBasis::~CBSplineBasis()
{

}

//////////////////////////////////////////////////////
//
//
CBSplineBasis::CBSplineBasis(const CBSplineBasis& basis)
{
	*this = basis;
}


//////////////////////////////////////////////////////
//
//
CBSplineBasis& CBSplineBasis::operator=(const CBSplineBasis& basis)
{
	if(this != &basis)
	{
		m_degree = basis.m_degree;
		m_knotChecked = basis.m_knotChecked;
		m_basisFuncCache = basis.m_basisFuncCache;
		m_knots = basis.m_knots;
	}
	return *this;
}


//////////////////////////////////////////////////////
//
//
CBSplineBasis::CBSplineBasis(int deg, int numKnts)
	: m_degree(deg),
	  m_knotChecked(false),
	  m_basisFuncCache(numKnts - deg),
	  m_knots(numKnts,-911)
{
	assert(deg > 1);
	assert(numKnts >= 2*(deg + 1));

	// Note all the knot values are junked to -911 to indicate
	// and invalid value
	m_basisFuncCache[numKnots() - order()] = -911;
}

//////////////////////////////////////////////////////
//
//
int CBSplineBasis::degree() const
{
	assert(isValid());
	return m_degree;
}

//////////////////////////////////////////////////////
//
//
int CBSplineBasis::order() const
{
	assert(isValid());
	return degree()+1;
}

//////////////////////////////////////////////////////
//
//
int CBSplineBasis::numKnots() const
{
	assert(isValid());
	return m_knots.size();
}

//////////////////////////////////////////////////////
//
//
double CBSplineBasis::tMin() const
{
	assert(validKnots());
	return m_knots[degree()];
}

//////////////////////////////////////////////////////
//
//
double CBSplineBasis::tMax() const
{
	assert(validKnots());
	return m_knots[numKnots() - order()];
}

//////////////////////////////////////////////////////
//
//
int CBSplineBasis::interval(double t) const
{
	assert(validKnots());
	assert((t-tMin()) >= -BSPLINE_SMALL && (t-tMax())<=BSPLINE_SMALL);

	//get the last valid knot which we can evalute over
	//(its actually tMax)
	int lastVal = numKnots() - order();

	//if t is at last knot value then we know the 
	//interval for free
	if(fabs(m_knots[lastVal] - t) <= BSPLINE_SMALL)
		return lastVal - 1;


	//get iterator position of upper bound. This is the value in the
	//knot vector tha bounds the value t from above, searching
	//between first and last knots
	std::vector<double>::const_iterator upper = std::upper_bound(m_knots.begin(),
											  m_knots.end(),t);

	//now get the position in the array of knot values and
	//subtract one to find the span. e.g for knots 0,2,3,4,5 and t = 3.5
	//upper points to the value 4 in the knot vector. Distance between 
	//iterators upper and begin is 3. So the interval is 2.
	return std::distance(m_knots.begin(), upper) - 1;
}
  
//////////////////////////////////////////////////////
//
//This functions gets the ith basis function at 
//parameter t. It is more efficient to calculate all 
//the basis functions at value t since the information
//to get one basis function is just about the same
//as that required to calculate all of them. Also
//the most common use for this function is for nurb
//curve evaluation where all basis are required
double CBSplineBasis::basis(int indx, double t) const
{
	assert(indx>=0 && indx<numKnots());
	assert(validKnots());

	double tmin = tMin();
	double tmax = tMax();
	assert((t-tmin) >= -BSPLINE_SMALL && (t-tmax)<=BSPLINE_SMALL);

	//get the parameter value the basis was last evaluated 
	//at. If its different to that input, then we need
	//to recalculate the basis functions
	if(fabs(m_basisFuncCache[numKnots() - order()] - t) > BSPLINE_SMALL)
	{
		// may need to update the basis functions
		basisFuncs(t);
	}

	//return the basis value for basis indx
	return m_basisFuncCache[indx];
}

//////////////////////////////////////////////////////
//
//Basically this is an algorithm taken from the 
//nurb book. All that has been done is to modify 
//the indexing into the arrays
void CBSplineBasis::basisFuncs(double t) const
{ 

	// get the knot interval that t lies in
	int intval = interval(t);

	// initialize the basis Funcs 
	for(int indx = 0; indx< m_basisFuncCache.size(); ++indx) 
	{
		m_basisFuncCache[indx] = 0.0;
	}

	// initialize the first element of the basis cache 
	int offset = intval - degree();
	m_basisFuncCache[offset] = 1.0;

	std::vector<double> left(order());
	std::vector<double> right(order());


	//init temp arrays (see Piegel nurb book on page 70)
	for(int j = 1; j<=degree(); ++j)
	{
		left[j] = t - getKnot(intval+1-j);

		right[j] = getKnot(intval+j) - t;
		double saved = 0.0;

		for(int k=0; k<j; ++k)
		{
			double temp = m_basisFuncCache[offset + k] / (right[k+1] + left[j-k]);
			m_basisFuncCache[offset + k] = saved + right[k+1]*temp;
			saved = left[j-k] * temp;
		}
		m_basisFuncCache[offset + j] = saved;
	}
	//set the last basis value to hold the parameter value
	//that was used to calculate the basis functions
	//this is used for cahcing purposes
	m_basisFuncCache[numKnots() - order()] = t;
}


//////////////////////////////////////////////////////
//
//
double CBSplineBasis::basisDeriv(double t) const
{
	assert(0);
	return 0.0;
}

//////////////////////////////////////////////////////
//
//
void CBSplineBasis::setKnot(int indx, double val)
{
	assert(isValid());
	assert(indx>=0 && indx<numKnots());
	assert(val >= 0.0);

	m_knots[indx] = val;
	m_knotChecked = false;

	//invalidate the cache
	m_basisFuncCache[numKnots() - order()] = -911; 
}

//////////////////////////////////////////////////////
//
//
double CBSplineBasis::getKnot(int indx) const
{
	assert(isValid());
	assert(indx>=0 && indx<numKnots());

	return m_knots[indx];
}

//////////////////////////////////////////////////////
//
//
bool CBSplineBasis::isValid() const
{
	return !m_knots.empty();
}

//////////////////////////////////////////////////////
//
//
bool CBSplineBasis::validKnots() const
{ 
	// check that we need to verify the knots
	if(!m_knotChecked) 
	{
		// only carry checking if we have constructed anything
		if(!isValid()) return false;

		// check that the knots are initialized (not == -911) and
		// are in ascending order

		// get the first knot
		double lastKnot = getKnot(0);

		double currentKnot = -911;

		for(int indx=1; indx<numKnots(); ++indx) 
		{      
			currentKnot = m_knots[indx];
			//check the knots are initialized and that the knots are increasing
			//(they can be identical tho)
			if(lastKnot != -911 && (currentKnot - lastKnot) >= -BSPLINE_SMALL)
			{
				lastKnot = m_knots[indx];

			}
			else
			{   
				//invalid knot, return
				return false;
			}
		}
		// reset the check flag
		m_knotChecked = true;
	}
	return true;
}

 