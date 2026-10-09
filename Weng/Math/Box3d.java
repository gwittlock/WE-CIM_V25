
package Weng.Math;

import Weng.System.*;

/**
 * A 3-dimensional bounding box representation.
 */
public class Box3d
{
    public Box3d()
	{
	}

    public Box3d( double xmin, double ymin, double zmin, double xmax, double ymax, double zmax )
	{
		m_xmin = xmin;
		m_ymin = ymin;
		m_zmin = zmin;
		m_xmax = xmax;
		m_ymax = ymax;
		m_zmax = zmax;
	}

	public double Xmin()  { return m_xmin; }
	public double Ymin()  { return m_ymin; }
	public double Zmin()  { return m_zmin; }
	public double Xmax()  { return m_xmax; }
	public double Ymax()  { return m_ymax; }
	public double Zmax()  { return m_zmax; }

	public double Dx()    { return (m_xmax - m_xmin); }
	public double Dy()    { return (m_ymax - m_ymin); }
	public double Dz()    { return (m_zmax - m_zmin); }

	public void Xmin( double xmin )  { m_xmin = xmin; }
	public void Ymin( double ymin )  { m_ymin = ymin; }
	public void Zmin( double zmin )  { m_zmin = zmin; }
	public void Xmax( double xmax )  { m_xmax = xmax; }
	public void Ymax( double ymax )  { m_ymax = ymax; }
	public void Zmax( double zmax )  { m_zmax = zmax; }

	public void Union( Box3d box )
	{
		if (box.Xmin() < m_xmin)
			m_xmin = box.Xmin();

		if (box.Ymin() < m_ymin)
			m_ymin = box.Ymin();

		if (box.Zmin() < m_zmin)
			m_zmin = box.Zmin();

		if (box.Xmax() > m_xmax)
			m_xmax = box.Xmax();

		if (box.Ymax() > m_ymax)
			m_ymax = box.Ymax();

		if (box.Zmax() > m_zmax)
			m_zmax = box.Zmax();
	}

	private double m_xmin =  Const.UNDEFINED;
	private double m_ymin =  Const.UNDEFINED;
	private double m_zmin =  Const.UNDEFINED;
	private double m_xmax = -Const.UNDEFINED;
	private double m_ymax = -Const.UNDEFINED;
	private double m_zmax = -Const.UNDEFINED;
}


 
 
 
 
 
 
 
 
 

