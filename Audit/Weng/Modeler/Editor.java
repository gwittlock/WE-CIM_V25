
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;
import Weng.Geometry.*;


/**
 * Use this singleton class for editing and manipulating database entities.
 */
public class Editor
{
	/**
	 * Creates a profile containing an ordered sequence of curves
	 * that are adjacent to the given seed curve within the given
	 * gap tolerance. When the end points of adjacent curves are
	 * within system tolerance, and associate is true, the adjacent
	 * curves will be forced to share a common end point.
	 * @return the profile that was created.
	 */
	static public DbProfile Chain( DbCurve seedCurve, boolean associate, double gapTol )
	{
		DbProfile dbProfile = null;

		String cmd = "Profile:Grow:"
					 + " seed=" + seedCurve.Id()
					 + ",assoc=" + (associate ? 1 : 0)
					 + ",tol=" + gapTol;

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				dbProfile = new DbProfile( id );
		}

		return dbProfile;
	}

	/**
	 * Split the given curve at the given point.
	 * @return the newly created (trailing) curve
	 */
	static public DbCurve Split( DbCurve dbCurve, Point pt )
	{
		DbCurve result = null;

		String cmd = "Profile:Split:"
					 + " id=" + dbCurve.Id()
					 + ",px=" + pt.X()
					 + ",py=" + pt.Y()
					 + ",pz=" + pt.Z();

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				result = new DbCurve( id );
		}

		return result;
	}

	/**
	 * Split the given curve at the given point.
	 * @return the newly created (trailing) curve
	 */
	static public DbCurve Split( DbCurve dbCurve, Point pt, double gap )
	{
		DbCurve result = null;

		String cmd = "Profile:Split:"
					 + " id=" + dbCurve.Id()
					 + ",px=" + pt.X()
					 + ",py=" + pt.Y()
					 + ",pz=" + pt.Z()
					 + ",gap=" + gap;

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				result = new DbCurve( id );
		}

		return result;
	}

	/**
	 * Trims two bounded curves to their common intersection point.
	 * @param curveA the first curve to consider
	 * @param endA (true) trims the end of the curve back to the
	 *             intersection point / (false) trims the start of
	 *             the curve back to the intersection point.
	 * @param curveB the second curve to consider
	 * @param endB (true) trims the end of the curve back to the
	 *             intersection point / (false) trims the start of
	 *             the curve back to the intersection point.
	 * @return (null) indicates failure / (else) the intersection
	 *         as a geometric point.
	 */
	static public Point Trim( DbCurve curveA, boolean endA, DbCurve curveB, boolean endB )
	{
		Point pt = Solver.Intersect( curveA, curveB );

		if (pt != null)
		{
			if ( endA )
				curveA.EndPt( pt );
			else
				curveA.StartPt( pt );

			if ( endB )
				curveB.EndPt( pt );
			else
				curveB.StartPt( pt );
		}

		return pt;
	}

	/**
	 * Inserts a blend radius between two 'adjacent' curves whose
	 * end points are within a given tolerance of each other.  This
	 * is particularly useful for curve entities that are known to
	 * to touch each other, as in profiles.
	 * @param curveA the first curve to consider
	 * @param curveB the second curve to consider
	 * @param radius the required radius of the blending arc
	 * @param dir the required direction of the blending arc
	 * @param tol the tolerance with which to determine the adjacency
	 *            of the ends of the given curves
	 * @return (null) indicates failure / (else) the resulting blending arc entity
	 */
	static public DbArc Blend(
							DbCurve curveA,
							DbCurve curveB,
							double radius,
							int dir,
							double tol )
	{
		DbArc dbBlend = null;

		Arc blend = Solver.Blend( curveA, curveB, Math.abs(radius), dir, tol );

		if (blend != null)
		{
			Point psA = curveA.StartPt();
			Point peA = curveA.EndPt();
			Point psB = curveB.StartPt();
			Point peB = curveB.EndPt();

			boolean endA = true;
			boolean endB = false;
			boolean okay = true;

			if ( peA.WithinTol( psB, tol ) )
			{
				// Desired case.  Nothing to do.
			}
			else if ( peA.WithinTol( peB, tol ) )
			{
				endA = true;
				endB = true;
			}
			else
			{
				if ( psA.WithinTol( psB, tol ) )
				{
					endA = false;
					endB = false;
				}
				else if ( psA.WithinTol( peB, tol ) )
				{
					endA = false;
					endB = true;
				}
				else
				{
					okay = false;
				}
			}

			if ( okay )
			{
				Point ps = blend.StartPt();
				Point pe = blend.EndPt();
				Point pc = blend.CenterPt();

				dbBlend = Editor.Blend(
					curveA, endA, ps.X(), ps.Y(),
					curveB, endB, pe.X(), pe.Y(),
					pc.X(), pc.Y(), dir );

				if ((dbBlend != null) && (radius < 0.))
					dbBlend.Invert();
			}
		}

		return dbBlend;
	}

	/**
	 * A combination of the Trim() and Blend() methods that can be
	 * used with construction geometry that may, or may not intersect.
	 */
	static public DbArc Blend(
							DbCurve curveA,
							boolean endA,
							DbCurve curveB,
							boolean endB,
							double radius,
							int dir )
	{
		DbArc dbBlend = null;

		Point hintA = (endA ? curveA.EndPt() : curveA.StartPt());
		Point hintB = (endB ? curveB.EndPt() : curveB.StartPt());

		Arc arc = Solver.ArcTT( curveA, curveB, Math.abs(radius), dir, false, hintA, hintB );

		if (arc != null)
		{
			Point ps = arc.StartPt();
			Point pe = arc.EndPt();
			Point pc = arc.CenterPt();
			dbBlend = Editor.Blend(
				curveA, endA, ps.X(), ps.Y(),
				curveB, endB, pe.X(), pe.Y(),
				pc.X(), pc.Y(), dir );

			if ((dbBlend != null) && (radius < 0.))
				dbBlend.Invert();
		}

		return dbBlend;
	}

	/**
	 * Insert a blending arc between two adjacent curves.
	 * @return (null) indicates failure / (else) the blending arc
	 */
	static public DbArc Blend(
							DbCurve dbCurveA, boolean endA, double xA, double yA,
							DbCurve dbCurveB, boolean endB, double xB, double yB,
							double xc, double yc, int dir )
	{
		DbArc result = null;

		String cmd = "Profile:Blend:"
					 + " id1=" + dbCurveA.Id()
					 + ",end1=" + (endA ? 1 : 0)
					 + ",x1=" + xA
					 + ",y1=" + yA
					 + ",id2=" + dbCurveB.Id()
					 + ",end2=" + (endB ? 1 : 0)
					 + ",x2=" + xB
					 + ",y2=" + yB
					 + ",cx=" + xc
					 + ",cy=" + yc
					 + ",dir=" + dir;

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				result = new DbArc( id );
		}

		return result;
	}

	/**
	 * Inserts a chamfer between two adjacent curves.
	 * @return (null) indicates failure / (else) the chamfer
	 */
	static public DbLine Chamfer(
							DbCurve dbCurveA, boolean endA, double xA, double yA,
							DbCurve dbCurveB, boolean endB, double xB, double yB )
	{
		DbLine result = null;

		String cmd = "Profile:Chamfer:"
					 + " id1=" + dbCurveA.Id()
					 + ",end1=" + (endA ? 1 : 0)
					 + ",x1=" + xA
					 + ",y1=" + yA
					 + ",id2=" + dbCurveB.Id()
					 + ",end2=" + (endB ? 1 : 0)
					 + ",x2=" + xB
					 + ",y2=" + yB;

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				result = new DbLine( id );
		}

		return result;
	}

	/**
	 * Constants for use with LeadsAdd() and RampsAdd()
	 */
	static public final int LINE     = 0;
	static public final int ARC      = 1;
	static public final int LINE_ARC = 2;

	static public final int IN       = 0;
	static public final int OUT      = 1;
	static public final int IN_OUT   = 2;


	/**
	 * Adds a lead-in and/or lead-out to a curve (or its parent profile).
	 * @param style one of Editor.LINE, Editor.ARC, Editor.LINE_ARC
	 * @param combin one of Editor.IN, Editor.OUT, Editor.IN_OUT
	 * @param side Const.LEFT or Const.RIGHT
	 * @param angle angle of line, or included angle of arc (degrees)
	 * @param length line length (ignored when not applicable to style)
	 * @param radius arc radius (ignored when not applicable to style)
	 */
	static public boolean LeadsAdd(
							DbCurve dbCurve,
							int style,
							int combin,
							int side,
							double angle,
							double length,
							double radius )
	{
		String cmd = "Toolpath:Lead:"
					 + " pid=" + dbCurve.Id()
					 + ",type=" + style
					 + ",dir=" + combin
					 + ",side=" + side
					 + ",ang=" + angle
					 + ",len=" + length
					 + ",rad=" + radius;

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Adds a ramp-in and/or ramp-out to a curve (or its parent profile).
	 * @param combin one of IN, OUT, IN_OUT
	 * @param overlap ? 
	 * @param length line length
	 */
	static public boolean RampsAdd(
							DbCurve dbCurve,
							int combin,
							double overlap,
							double length )
	{
		String cmd = "Toolpath:Ramp:"
					 + " pid=" + dbCurve.Id()
					 + ",dir=" + combin
					 + ",lap=" + overlap
					 + ",len=" + length;

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Adds a lead-in and/or lead-out to all selected curves/profiles.
	 * @param db fully qualified path to the database containing the lead information
	 * @param setup the id of the database record containing the lead information
	 */
	static public boolean AutoLead( String db, int setup )
	{
		String cmd = "Lead:Selection:"
					 + " configdb=\"" + db + "\""
					 + ",setup=" + setup;

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Moves and/or copies all entities in the selection set.  Note, in order
	 * to move or copy toolpath,it must be disassociated from its reference geometry.
	 * See also Weng.Modeler.Selector
	 * @param dx the X delta
	 * @param dy the Y delta
	 * @param dz the Z delta
	 * @param numberOfCopies (0) to move / (+) the number of copies
	 */
	static public boolean Move( double dx, double dy, double dz, int numberOfCopies )
	{
		String cmd = "Transform:Move: sx=0.0,sy=0.0,sz=0.0"
					 + ",ex=" + dx
					 + ",ey=" + dy
					 + ",ez=" + dz
					 + ",copies=" + numberOfCopies;

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Rotates and/or copies all entities in the selection set.  Note, in order
	 * to move or copy toolpath, it must be disassociated from its reference geometry.
	 * See also Weng.Modeler.Selector
	 * @param xOrigin the X origin of rotation
	 * @param yOrigin the Y origin of rotation
	 * @param angle the rotation amount in degrees
	 * @param numberOfCopies (0) to move / (+) the number of copies
	 */
	static public boolean Rotate( double xOrigin, double yOrigin, double angle, int numberOfCopies )
	{
		String cmd = "Transform:Rotate:"
					 + " ox=" + xOrigin
					 + ",oy=" + yOrigin
					 + ",ang=" + (angle * Const.DEG2RAD)
					 + ",copies=" + numberOfCopies;

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Scales all entities in the selection set.  See
	 * also Weng.Modeler.Selector
	 * @param xOrigin the X origin of scaling
	 * @param yOrigin the Y origin of scaling
	 * @param xScaleFactor self explainatory
	 * @param yScaleFactor self explainatory
	 */
	static public boolean Scale( double xOrigin, double yOrigin, double xScaleFactor, double yScaleFactor )
	{
		String cmd = "Transform:Scale:"
					 + " ox=" + xOrigin
					 + ",oy=" + yOrigin
					 + ",fx=" + xScaleFactor
					 + ",fy=" + yScaleFactor;

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Mirrors all entities in the selection set about either a
	 * vertical or horizontal line.  Note, in order to move or copy toolpath,
	 * it must be disassociated from its reference geometry.
	 * See also Weng.Modeler.Selector
	 * @param xs the X origin of the mirror axis
	 * @param ys the Y origin of the mirror axis
	 * @param aboutY (true) mirror about a vertical line / (false) mirror
	 *               about a horizontal line
	 * @param copy (true) creates a copy / (false) moves the selected entities
	 */
	static public boolean Mirror( double xs, double ys, boolean aboutY, boolean copy )
	{
		String cmd = "Transform:Mirror:"
					 + " ox=" + xs
					 + ",oy=" + ys
					 + ",mx=" + (aboutY ? 0 : 1)
					 + ",my=" + (aboutY ? 1 : 0)
					 + ",copies=" + (copy ? 1 : 0);

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Unchains all profiles in the selection set.
	 */
	static public boolean UnchainSelected()
	{
		String cmd = "Profile:Explode: group=1";

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Reverses all profiles/curves in the selection set.
	 */
	static public boolean ReverseSelected()
	{
		String cmd = "Profile:Reverse: group=1";

		return ( Portal.Execute( cmd ) );
	}
	
	private Editor()
	{
	}
}

