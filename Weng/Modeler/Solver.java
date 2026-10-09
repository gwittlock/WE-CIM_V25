
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;
import Weng.Geometry.*;


/**
 * Use this singleton class to find solutions to geoemtric problems
 * such as entity intersections and tangencies.  NOTE: All arc solutions
 * are xy planar, requiring all input to be xy coplanar.
 * <p>
 * With regards to <b>arc direction</b>, it suggested that you
 * use the constants Const.CCW and Const.CW that are
 * defined in Weng.System.Const.  Otherwise, arc direction
 * follows protractor convention (+) CCW / (-) CW.
 */
public class Solver
{
	/**
	 * Finds the arc that runs through the given points.
	 * @param ps the start point of the arc
	 * @param pe the end point of the arc
	 * @param pm the intermediate point on the arc
	 * @return (null) no solution
	 */
	static public Arc ArcSEP( Point ps, Point pe, Point pm )
	{
		Arc result = null;

		String cmd = "Solve:ArcSEP:"
					 + " sx=" + ps.X()
					 + ",sy=" + ps.Y()
					 + ",ex=" + pe.X()
					 + ",ey=" + pe.Y()
					 + ",px=" + pm.X()
					 + ",py=" + pm.Y();

		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			double xc = Portal.DoubleGet( "cx" );
			double yc = Portal.DoubleGet( "cy" );
			int dir = Portal.IntGet( "dir" );

			result = new Arc( ps, pe, new Point( xc, yc, ps.Z() ), dir );
		}

		return result;
	}

	/**
	 * Finds the end point of an arc given its start point, center point
	 * and included angle.
	 * @param ps the start point of the arc
	 * @param pc the center point of the arc
	 * @param ang the include angle of the arc (<b>radians</b>)
	 * @param pe the <b>result</b>ing end point of the arc
	 * @return (false) no solution
	 */
	static public boolean ArcSCI( Point ps, Point pc, double ang, Point pe )
	{
		String cmd = "Solve:ArcSCI:"
					 + " sx=" + ps.X()
					 + ",sy=" + ps.Y()
					 + ",cx=" + pc.X()
					 + ",cy=" + pc.Y()
					 + ",ang=" + ang;

		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			double xe = Portal.DoubleGet( "ex" );
			double ye = Portal.DoubleGet( "ey" );

			pe.Assign( xe, ye, pc.Z() );
		}

		return okay;
	}

	/**
	 * Finds the center point of an arc given its start point, end point,
	 * radius, direction and an indication (big) of the desired solution.
	 * In most cases, big should be set to false.
	 * <p>
	 * @param ps the start point of the arc
	 * @param pe the end point of the arc
	 * @param radius the radius of the arc
	 * @param dir the arcs direction
	 * @param big (true) calculates the center based on an arc having an
	 *            included greater than 180 degrees / (false) calculates
	 *            the center based on an arc having an included angle
	 *            smaller than 180 degrees.
	 * @param pc the <b>result</b>ing center point of the arc
	 * @return (false) no solution
	 */
	static public boolean ArcSER(
							Point ps,
							Point pe,
							double radius,
							int dir,
							boolean big,
							Point pc )
	{
		String cmd = "Solve:ArcSER:"
					 + " sx=" + ps.X()
					 + ",sy=" + ps.Y()
					 + ",ex=" + pe.X()
					 + ",ey=" + pe.Y()
					 + ",rad=" + radius
					 + ",dir=" + dir
					 + ",big=" + (big ? 1 : 0);

		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			double xc = Portal.DoubleGet( "cx" );
			double yc = Portal.DoubleGet( "cy" );

			pc.Assign( xc, yc, ps.Z() );
		}

		return okay;
	}

	/**
	 * Finds the tangent point and center point of an arc that is tangent
	 * to the specified entity and runs through the given point, and
	 * has a known radius and direction.
	 *
	 * @param refPt the known point
	 * @param refCurve the id of the entity to which the arc will be tangent
	 * @param radius the arc radius of the resulting arc
	 * @param hint a point on the entity that helps define the correct solution
	 * @param pt the resulting tangent point
	 * @param pc the resulting center point
	 * @return (false) no solution
	 */
	static public boolean ArcPTR(
							Point	refPt,
							DbCurve	refCurve,
							double	radius,
							Point	hint,
							Point	pt,
							Point	pc )
	{
		String cmd = "Solve:ArcPTR:"
					 + " px="	+ refPt.X()
					 + ",py="	+ refPt.Y()
					 + ",stan="	+ refCurve.Id()
					 + ",rad="	+ radius
					 + ",hx="	+ hint.X()
					 + ",hy="	+ hint.Y();

		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			double xt = Portal.DoubleGet( "tx" );
			double yt = Portal.DoubleGet( "ty" );
			double xc = Portal.DoubleGet( "cx" );
			double yc = Portal.DoubleGet( "cy" );

			pt.Assign( xt, yt, refPt.Z() );
			pc.Assign( xc, yc, refPt.Z() );
		}

		return okay;
	}

	/**
	 * Finds the arc having the specified radius and direction, that is
	 * tangent to given curves.
	 * @param tanCurveA first known reference curve
	 * @param tanCurveB second known reference curve
	 * @param radius known radius if resulting arc
	 * @param dir known direction of resulting arc
	 * @param big <b>(true)</b> result >= 180 degrees, <b>(false)</b> result <= 180 degrees
	 * @param hintA a hint point on tanCurveA indicating the general location where the
	 *     the resulting arc should contact tanCurveA.
	 * @param hintB a hint point on tanCurveB indicating the general location where the
	 *     the resulting arc should contact tanCurveB.
	 * @return <b>(null)</b> no solution
	 */
	static public Arc ArcTT(
							DbCurve tanCurveA,
							DbCurve tanCurveB,
							double radius,
							int dir,
							boolean big,
							Point hintA,
							Point hintB )
	{
		Arc result = null;

		String cmd = "Solve:ArcTT:"
					 + " stan="	+ tanCurveA.Id()
					 + ",etan="	+ tanCurveB.Id()
					 + ",rad="	+ radius
					 + ",dir="	+ dir
					 + ",big="	+ (big ? 1 : 0)
					 + ",spx="	+ hintA.X()
					 + ",spy="	+ hintA.Y()
					 + ",epx="	+ hintB.X()
					 + ",epy="	+ hintB.Y();

		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			double xs = Portal.DoubleGet( "sx" );
			double ys = Portal.DoubleGet( "sy" );
			double xe = Portal.DoubleGet( "ex" );
			double ye = Portal.DoubleGet( "ey" );
			double xc = Portal.DoubleGet( "cx" );
			double yc = Portal.DoubleGet( "cy" );
			double z = tanCurveA.StartPt().Z();

			result = new Arc( xs, ys, z, xe, ye, z, xc, yc, z, dir ); 
		}

		return result;
	}

	/**
	 * Finds the circle that runs through a known point and that is tangent to
	 * a line and another circle.
	 * <br>
	 * <img SRC="ArcPAL.jpg" height=231 width=276>
	 * @param pt the known point through which the result must run
	 * @param circle the known circle to which the result must be tangent
	 * @param line the known line to which the result must be tangent
	 * @param hint a point on the known circle indicating the approximate
	 *     tangent location of the result circle.  As there can be up to
	 *     four solutions, the hint point is used as selection criteria.
	 * @return <b>(null)</b> no solution, <b>(otherwise)</b> an arc that
	 *     satisfies the criteria and whose start point is the tangent point
	 *     to the known circle.
	 */
	static public Arc ArcPAL( Point pt, Arc circle, Line line, Point hint )
	{
		Point pc = circle.CenterPt();
		Point ps = line.StartPt();
		Point pe = line.EndPt();

		String cmd = "Solve:ArcPAL:"
					 + " px="  + pt.X()
					 + ",py="  + pt.Y()
					 + ",cx="  + pc.X()
					 + ",cy="  + pc.Y()
					 + ",rad=" + circle.Radius()
					 + ",sx="  + ps.X()
					 + ",sy="  + ps.Y()
					 + ",ex="  + pe.X()
					 + ",ey="  + pe.Y()
					 + ",tx="  + hint.X()
					 + ",ty="  + hint.Y();

		Arc arc = null;

		if ( Portal.Execute( cmd ) )
		{
			double xc  = Portal.DoubleGet( "cx" );
			double yc  = Portal.DoubleGet( "cy" );
			double rad = Portal.DoubleGet( "rad" );
			double tx  = Portal.DoubleGet( "tx" );
			double ty  = Portal.DoubleGet( "ty" );

			pc = new Point( xc, yc, circle.CenterPt().Z() );
			ps = new Point( tx, ty, pc.Z() );

			arc = new Arc( ps, ps, pc, Const.CW );
		}

		return arc;
	}

	/**
	 * Finds the end point of a line given its start point, length
	 * and direction.
	 * @param ps the start point of the line
	 * @param ang the angle of the line (<b>radians</b>)
	 * @param len the length of the line
	 * @param pe the <b>result</b>ing end point of the line
	 * @return (false) indicates failure
	 */
	static public boolean LineSAL( Point ps, double ang, double len, Point pe )
	{
		String cmd = "Solve:LineSAL:"
					 + " sx=" + ps.X()
					 + ",sy=" + ps.Y()
					 + ",ang=" + ang
					 + ",len=" + len;

		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			double xe = Portal.DoubleGet( "ex" );
			double ye = Portal.DoubleGet( "ey" );

			pe.Assign( xe, ye, ps.Z() );
		}

		return okay;
	}

	/**
	 * Finds the end point of a line through a known start point and
	 * that is tangent to a specified entity.
	 * @param ps the known start point
	 * @param tanEntity the curve entity to which the line will be tangent
	 * @param end specifies the desired tangent point solution relative
	 *            to the end of the TanEntity ( [0] start / [1] end ).
	 * @param pe the <b>result</b>ing line end point
	 * @return (false) indicates failure
	 */
	static public boolean LineST( Point ps, DbCurve tanCurve, boolean end, Point pe )
	{
		String cmd = "Solve:LineST:"
					 + " sx=" + ps.X()
					 + ",sy=" + ps.Y()
					 + ",etan=" + tanCurve.Id()
					 + ",eend=" + (end ? 1 : 0);

		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			double xe = Portal.DoubleGet( "ex" );
			double ye = Portal.DoubleGet( "ey" );

			pe.Assign( xe, ye, ps.Z() );
		}

		return okay;
	}

	/**
	 * Finds the end points of a line that is tangent to two entities.
	 * @param tanCurveA the first reference curve entity
	 * @param endA (true) find a solution towards the end of the first
	 *             curve / (false) find a solution towards the start
	 *             of the first curve
	 * @param tanCurveB the second reference curve entity
	 * @param endB (true) find a solution towards the end of the second
	 *             curve / (false) find a solution towards the start
	 *             of the second curve
	 * @param ps the <b>result</b>ing start point
	 * @param pe the <b>result</b>ing end point
	 * @return (false) indicates failure
	 */
	static public boolean LineTT(
							DbCurve tanCurveA,
							boolean endA,
							DbCurve tanCurveB,
							boolean endB,
							Point ps,
							Point pe )
	{
		String cmd = "Solve:LineTT:"
					 + " stan=" + tanCurveA.Id()
					 + ",send=" + (endA ? 1 : 0)
					 + ",etan=" + tanCurveB.Id()
					 + ",eend=" + (endB ? 1 : 0);

		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			double xs = Portal.DoubleGet( "sx" );
			double ys = Portal.DoubleGet( "sy" );
			double zs = Portal.DoubleGet( "sz" );
			
			double xe = Portal.DoubleGet( "ex" );
			double ye = Portal.DoubleGet( "ey" );
			double ze = Portal.DoubleGet( "ez" );

			ps.Assign( xs, ys, zs );
			pe.Assign( xe, ye, ze );
		}

		return okay;
	}

	/**
	 * Finds the intersection point between two bounded curve entities
	 * that are known to have single intersection point.
	 * @param curveA the first curve to be considered
	 * @param curveA the seecond curve to be considered
	 * @return (null) indicates no solution / (else) the intersection point
	 */
	static public Point Intersect( DbCurve curveA, DbCurve curveB )
	{
		Point soln = null;

		String cmd = "Solve:Intersect:"
					 + " tan1=" + curveA.Id()
					 + ",end1=0"
					 + ",tan2=" + curveB.Id()
					 + ",end2=0"
					 + ",onseg=1";

		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			double xp = Portal.DoubleGet( "ix" );
			double yp = Portal.DoubleGet( "iy" );

			soln = new Point( xp, yp, curveA.EndPt().Z() );
		}

		return soln;
	}

	/**
	 * Finds the intersection point between two entities.
	 * @param curveA the first curve to be considered
	 * @param endA (true) finds the intersection near the end of the
	 *             first curve / (false) finds the intersection near
	 *             the start of the first curve
	 * @param curveA the seecond curve to be considered
	 * @param endB (true) finds the intersection near the end of the
	 *             second curve / (false) finds the intersection near
	 *             the start of the second curve
	 * @param onSegment (true) finds only solutions that are on the
	 *                  bounded portion of the curves / (false) allows
	 *                  solutions on the unbounded portion of the curves.
	 * @return (null) indicates no solution / (else) the intersection point
	 */
	static public Point Intersect(
							DbCurve curveA,
							boolean endA,
							DbCurve curveB,
							boolean endB,
							boolean onSegment )
	{
		Point soln = null;

		String cmd = "Solve:Intersect:"
					 + " tan1=" + curveA.Id()
					 + ",end1=" + (endA ? 1 : 0)
					 + ",tan2=" + curveB.Id()
					 + ",end2=" + (endB ? 1 : 0)
					 + ",onseg=" + (onSegment ? 1 : 0);

		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			double xp = Portal.DoubleGet( "ix" );
			double yp = Portal.DoubleGet( "iy" );

			soln = new Point( xp, yp, curveA.EndPt().Z() );
		}

		return soln;
	}

	/**
	 * Finds the intersection point between two bounded curve entities
	 * that are known to have single intersection point.
	 * @param curveA the first curve to be considered
	 * @param curveA the seecond curve to be considered
	 * @return (null) indicates no solution / (else) the intersection point
	 */
	static public IntsctRec[] Intersect( DbEntity entityA, DbEntity entityB )
	{
		int typeA = entityA.Type();
		int typeB = entityB.Type();

		if (typeA != Const.LINE && typeA != Const.ARC && typeA != Const.PROFILE)
			return null;

		if (typeB != Const.LINE && typeB != Const.ARC && typeB != Const.PROFILE)
			return null;

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		IntsctRec [] solns = null;

		String cmd = "Solve:MultiIntersect:"
					 + " idA=" + entityA.Id()
					 + ",idB=" + entityB.Id();

		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			int count = Portal.IntGet( "count" );
			if (count > 0)
			{
				solns = new IntsctRec[count];

				for (int indx = 0; indx < count; ++ indx)
				{
					Portal.Execute( "Solve:MultiIntersectGet: index=" + indx );

					double x = Portal.DoubleGet( "x" );
					double y = Portal.DoubleGet( "y" );
					double z = Portal.DoubleGet( "z" );
					int idA = Portal.IntGet( "idA" );
					int idB = Portal.IntGet( "idB" );

					solns[indx] = new IntsctRec(
										new Point( x, y, z ),
										new DbCurve( idA ),
										new DbCurve( idB ) );
				}
			}
		}

		Portal.Execute( "Solve:MultiIntersectFlush:" );

		return solns;
	}

	static public final int MIDPOINT  = 0;
	static public final int FROMSTART = 1;
	static public final int FROMEND   = 2;

	/**
	 * Finds a point on a curve.
	 * @param curve the curve entity to be considered
	 * @param method must be one of (MIDPOINT, FROMSTART, FROMEND)
	 * @param dist the arc distance along the curve (only relevant
	 *             when the method option is FROMSTART or FROMEND)
	 * @return (null) failure
	 */
	static public Point CurvePt( DbCurve curve, int method, double dist )
	{
		Point pt = null;
		double	len = curve.Length();
		
		String cmd = "Solve:Split:"
					 + " id=" + curve.Id()
					 + ",pos=" + method
					 + ",dist=" + dist;

		if ( Portal.Execute( cmd ) )
		{
			double x = Portal.DoubleGet( "cx" );
			double y = Portal.DoubleGet( "cy" );
			double z = Portal.DoubleGet( "cz" );

			pt = new Point( x, y, z );
		}

		return pt;
	}

	/**
	 * Finds the point on the reference curve that is closest to the reference point.
	 * NOTE: This method finds the 2D solution.  It assumes the reference point is
	 * in the same workplane as the reference curve.  Also, the Z ordinate of the
	 * the solution is returned as the Z ordinate of the reference point.
	 * @param curve the reference curve
	 * @param pt the reference point
	 * @param onseg (true) finds a solution in the bounded curve / (false) finds
	 * a solution on the unbounded curve.
	 * @return the closest point on the reference curve
	 */
	static public Point ClosestPt( DbCurve curve, Point pt, boolean onseg )
	{
		Point soln = null;

		String cmd = "Solve:PointClosest:"
					 + " id=" + curve.Id()
					 + ",x=" + pt.X()
					 + ",y=" + pt.Y();

		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			double uparam = Portal.DoubleGet( "uparam" );

			// The default answer.
			double xp = Portal.DoubleGet( "x" );
			double yp = Portal.DoubleGet( "y" );

			soln = new Point( xp, yp, pt.Z() );

			if ( onseg )
			{
				Point curvePt = null;

				if (uparam <= 0.0)
					curvePt = curve.StartPt();
				else if (uparam >= 1.0)
					curvePt = curve.EndPt();

				if (curvePt != null)
				{
					soln.X( curvePt.X() );
					soln.Y( curvePt.Y() );
				}
			}
		}

		return soln;
	}

	/**
	 * Calculates the parameters of a chamfer.
	 * @param curveA the first curve to consider
	 * @param curveB the second curve to consider
	 * @param angle the angle of the chamfer relative to the
	 *              first curve (<b>radians</b>)
	 * @param offset ???
	 * @param onseg (true) finds solutions on the bounded portions
	 *              of the curves / (false) allows solutions on the
	 *              unbounded portions of the curves
	 * @param ps the <b>result</b>ing start point of the chamfer
	 * @param pe the <b>result</b>ing end point of the chamfer
	 * @param solnEnds a two element array indicating which ends of
	 *                 the curves the solutions lay on, where (solnEnds[0])
	 *                 represents the solution for the first curve and
	 *                 (solnEnds[1]) represents the solution for the
	 *                 second curve.  A value of (true) indicates the
	 *                 solution is near the end of a curve and a value of
	 *                 (false) indicates the solution is near the start
	 *                 of a curve.
	 * @return (false) indicates failure
	 */
	static public boolean Chamfer(
							DbCurve curveA,
							DbCurve curveB,
							double angle,
							double offset,
							boolean onseg,
							Point ps,
							Point pe,
							boolean solnEnds[] )
	{
		String cmd = "Solve:Chamfer:"
					 + " id1=" + curveA.Id()
					 + ",id2=" + curveB.Id()
					 + ",ang=" + angle
					 + ",off=" + offset
					 + ",onseg=" + (onseg ? 1 : 0);
		
		boolean okay = Portal.Execute( cmd );

		if ( okay )
		{
			double xs = Portal.DoubleGet( "sx" );
			double ys = Portal.DoubleGet( "sy" );
			double zs = Portal.DoubleGet( "sz" );
			double xe = Portal.DoubleGet( "ex" );
			double ye = Portal.DoubleGet( "ey" );
			double ze = Portal.DoubleGet( "ez" );
			int endA = Portal.IntGet( "end1" );
			int endB = Portal.IntGet( "end2" );

			ps.Assign( xs, ys, Const.UNDEFINED );
			pe.Assign( xe, ye, Const.UNDEFINED );
			solnEnds[0] = ((endA > 0) ? true : false);
			solnEnds[1] = ((endB > 0) ? true : false);
		}

		return okay;
	}

	/**
	 * Finds the blending arc between two curve entities whose end points
	 * are within proximity of each other.  This method is particularly
	 * useful for curves that have been 'chained' into a profile.
	 * @param curveA the first curve to consider
	 * @param curveB the second curve to consider
	 * @param radius the radius of the blending arc
	 * @param dir the direction of the blending arc
	 * @param tol the tolerance by which 'proximity' of curve
	 *            end points is determined
	 * @return (null) indicates no solution / (else) the blending arc
	 */
	static public Arc Blend(
							DbCurve curveA,
							DbCurve curveB,
							double radius,
							int dir,
							double tol )
	{
		// NOTE: There should really be a core Blend( Arc, Arc, rad, dir, tol )
		// in the Geometry library that is used by this method.

		Arc blend = null;

		String cmd = "Solve:Blend:"
					 + " idA=" + curveA.Id()
					 + ",idB=" + curveB.Id()
					 + ",rad=" + radius
					 + ",dir=" + ((dir < 0) ? Const.CW : Const.CCW)
					 + ",tol=" + ((tol < Const.SMALL) ? Const.SMALL : tol);

		boolean okay = Portal.Execute( cmd );
		
		if ( okay )
		{
			double xs = Portal.DoubleGet( "sx" );
			double ys = Portal.DoubleGet( "sy" );
			double zs = Portal.DoubleGet( "sz" );
			double xe = Portal.DoubleGet( "ex" );
			double ye = Portal.DoubleGet( "ey" );
			double xc = Portal.DoubleGet( "cx" );
			double yc = Portal.DoubleGet( "cy" );

			blend = new Arc( xs ,ys, zs, xe, ye, zs, xc, yc, zs, dir );
		}
		
		return blend;		
	}

	private Solver()
	{
	}
}
