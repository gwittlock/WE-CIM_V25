
package Weng.Geometry;


/**
 *
 */
public class ClosestData
{
	public ClosestData( Point pt, double uparam, double dist, int side )
	{
		m_pt     = pt;
		m_uparam = uparam;
		m_dist   = dist;
		m_side   = side;
	}

	public Point Point()		{ return m_pt; }
	public double Uparam()		{ return m_uparam; }
	public double Dist()		{ return m_dist; }
	public int Side()			{ return m_side; }

	private Point  m_pt;
	private double m_uparam;
	private double m_dist;
	private int    m_side;
}

