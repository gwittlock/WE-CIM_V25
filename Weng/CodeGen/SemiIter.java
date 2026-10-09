
package Weng.CodeGen;

import Weng.System.Const;
import Weng.Math.UnitVec2d;
import Weng.Geometry.*;

/**
 * An object of the class SemiIter (semi-circle iterator) is typically 
 * used when generating arc output for machines tools that are
 * restricted to processing arc semi-circle (180 degree max.) data.
 */
public class SemiIter
{
	/**
	 * Creates and intializes a semi-circle iterator for the given arc
	 * @param arc the arc that is to be iterated
	 */
	public SemiIter( Arc arc )
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
		double xc = arc.CenterPt().X();
		double yc = arc.CenterPt().Y();
		double xs = arc.StartPt().X();
		double ys = arc.StartPt().Y();
		double xe = arc.EndPt().X();
		double ye = arc.EndPt().Y();

		m_count = 0;

		if (arc.IncludedAngle() > Const.PI)
		{
			// The arc angle exceeds 180 degrees.  Therefore, we
			// must break the arc at the 180 degree boundary.

			double dx = xs - xc;
			double dy = ys - yc;

			double x180 = xc - dx;
			double y180 = yc - dy;

			m_x[m_count] = x180;
			m_y[m_count] = y180;
			++m_count;
		}

		m_x[m_count] = xe;
		m_y[m_count] = ye;
		++m_count;
	}

	protected void finalize() throws Throwable
	{
		m_x = null;
		m_y = null;

		super.finalize();
	}

	double [] m_x = new double[2];
	double [] m_y = new double[2];

	int m_count;
	int m_indx;
}

