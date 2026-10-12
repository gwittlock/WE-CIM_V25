
package Weng.Modeler;

import Weng.Geometry.Point;


/**
 *
 */
public class IntsctRec
{
	public IntsctRec( Point pt, DbCurve dbCurveA, DbCurve dbCurveB )
	{
		m_pt = pt;
		m_curveA = dbCurveA;
		m_curveB = dbCurveB;
	}

	public Point Pt()
	{
		return m_pt;
	}

	public DbCurve CurveA()
	{
		return m_curveA;
	}

	public DbCurve CurveB()
	{
		return m_curveB;
	}

	private Point	m_pt;
	private DbCurve	m_curveA;
	private DbCurve	m_curveB;
}

