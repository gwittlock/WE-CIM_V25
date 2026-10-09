
package Weng.CodeGen;

import Weng.System.Const;
import Weng.Geometry.*;

/**
 * An object of the class QuadIter (quadrant iterator) is typically 
 * used when generating arc output for machines tools that are
 * restricted to processing arc quadrant data.
 */
public class QuadIter
{
	/**
	 * Creates and intializes a quadrant iterator for the given arc
	 * @param arc the arc that is to be iterated
	 */
	public QuadIter( Arc arc )
	{
		m_count = 0;
		m_indx = 0;

		PtsCalc( arc );
	}

	/**
	 * Indicates whether this object has iterated through all quadrants of its arc
	 * @return (true) when all arc quadrants have been processed
	 */
	public boolean Done()
	{
		return (m_indx >= m_count);
	}

	/**
	 * Reinitializes this quadrant iterator, restarting the iteration process
	 */
	public void Reset()
	{
		m_indx = 0;
	}

	/**
	 * Advances this quadrant iterator to the next arc quadrant
	 * @return (null) if done / (else) the end point of the arc quadrant
	 */
	public Point NextEndPt()
	{
		Point endPt = null;

		if ( !Done() )
		{
			endPt = new Point( m_x[m_indx], m_y[m_indx], 0.0 );
			++m_indx;
		}

		return endPt;
	}

	private void PtsCalc( Arc arc )
	{
		double [] angles = new double[2];

		arc.Angles( angles );

		// as,ae & xc,yc & radius for convenience.
		double as = angles[0];
		double ae = angles[1];
		double xc = arc.CenterPt().X();
		double yc = arc.CenterPt().Y();
		double radius = arc.Radius();

		// Shouldn't encounter zero-length arcs but ....
		double adiff = ae - as;
		if (Math.abs(adiff) < Const.SMALL)
			return;

		double delta = ((adiff < 0) ? -Const.HALFPI : Const.HALFPI);

		// The intermediate angle.
		double ai = ((int) (as / Const.HALFPI)) * Const.HALFPI;
		if (delta < 0 && Math.abs(ai-as) > Const.SMALL)
			ai += Const.HALFPI;

		ai += delta;
		while ( !Done((int) delta, ai, ae) )
		{
			m_x[m_count] = xc + radius * Math.cos( ai );
			m_y[m_count] = yc + radius * Math.sin( ai );
			++m_count;

			ai += delta;
		}

		m_x[m_count] = xc + radius * Math.cos( ae );
		m_y[m_count] = yc + radius * Math.sin( ae );
		++m_count;
	}

	public boolean Done( int dir, double ai, double ae )
	{
		if (dir < 0)
			return ((ae - ai) > -Const.SMALL);
		else
			return ((ai - ae) > -Const.SMALL);
	}

	protected void finalize() throws Throwable
	{
		m_x = null;
		m_y = null;

		super.finalize();
	}

	double [] m_x = new double[5];
	double [] m_y = new double[5];

	int m_count;
	int m_indx;
}

