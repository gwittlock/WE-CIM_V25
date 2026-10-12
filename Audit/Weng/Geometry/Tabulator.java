
package Weng.Geometry;

import Weng.System.Const;
import java.util.Vector;

/**
 * Use this class to create the tabulated point representation of a geometry entity.
 * Note, the method Count() will return a value of zero when the method Init() failed.
 * Otherwise, you are guaranteed the 0th and Nth points correspond to the start and
 * end points of the entity respectively.
 */
public class Tabulator
{
	/**
	 * Default constructor.
	 */
	public Tabulator()
	{
	}

	/**
	 * Creates the tabulated point representation of the given geometry entity.
	 * @param elem the geometric entity to be tabulated
	 * @param step the step distance along the entity
	 * @param normalize (false) use step as given / (true) adjust the step distance
	 *        to get evenly distributed tabulation points.
	 */
	public void Init( Elem elem, double step, boolean normalize )
	{
		int nsteps, indx;
		double arclen, residual, u;

		m_count = 0;
		m_pts = null;

		if (step < Const.SMALL)
			return;  // Early exit

		arclen = elem.ArcLen();

		nsteps = (int) (arclen / step);

		residual = arclen - (nsteps * step);
		if (residual > Const.SMALL)
		{
			if ( normalize )
				step = arclen / (nsteps + 1);
		}
		else if (nsteps > 0)
		{
			// There is an integral number of steps, so eliminate the
			// final step because the associated point is the end point.

			--nsteps;
		}

		// Allocate the point array, accounting for the start and end point.
		m_count = nsteps + 2;
		m_pts = new Point[m_count];

		// Fill the array.
		m_pts[0] = new Point( elem.StartPt() );

		for (indx = 1; indx < (m_count - 1); ++indx)
		{
			u = (indx * step) / arclen;
			m_pts[indx] = new Point( elem.Point( u ) );
		}

		m_pts[m_count-1] = new Point( elem.EndPt() );

	}

	/**
	 * Gets the number of tabulated points.  A value of zero indicates failure.
	 */
	public int Count()
	{
		return m_count;
	}

	/**
	 * Gets the Ith tabulated point.
	 */
	public Point Point( int indx )
	{
		return m_pts[ indx ];
	}

	private Point [] m_pts = null;
	private int m_count = 0;
}

