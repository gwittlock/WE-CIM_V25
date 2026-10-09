
package Weng.Modeler;

import Weng.System.Const;
import Weng.System.Portal;
import Weng.Geometry.Point;


/**
 * Use this singleton class to create 'standard shape' profile
 * entities that reside in the modeler's database.
 */
public class Shape
{
	/**
	 * Creates a profile entity representing a rectangle whose adjacent
	 * curves share common end points (associated).  Corner blending
	 * arcs can be conditionally created.
	 * @param origin a point specifying one corner of the rectangle
	 * @param xdim the signed X dimension of the rectangle
	 * @param ydim the signed Y dimension of the rectangle
	 * @param radius the blending arc radius (<b>iff</b> radius > Const.SMALL).
	 * If the radius is less-than Const.SMALL, the arcs are inverted.
	 * @param windingDirection one of (Const.CW, Const.CCW)
	 * @return a profile entity whose 0th curve is a line entity.
	 */
	static public DbProfile Rectangle(
								Point	origin,
								double	xdim,
								double	ydim,
								double	radius,
								int		windingDirection )
	{
		if (Math.abs( xdim ) < Const.SMALL ||	Math.abs( ydim ) < Const.SMALL)
			return null;  // No solution, coincident points.

		double xo = origin.X();
		double yo = origin.Y();
		double zo = origin.Z();
		
		double dx = Math.abs( xdim );
		double dy = Math.abs( ydim );

		double rad = Math.abs( radius );
		if (rad < Const.SMALL)
			radius = 0.;
		
		int dir = 0;
		if (rad >= Const.SMALL)
			dir = ((radius > 0) ? Const.CW : Const.CCW);

		// Construct the radiused rectangle in a cw fashion.
					
		DbLine lineA = new DbLine( xo, (yo+rad), zo, xo, (yo+dy-rad), zo );
		DbLine lineC = new DbLine( (xo+rad), (yo+dy), zo, (xo+dx-rad), (yo+dy), zo );
		DbLine lineE = new DbLine( (xo+dx), (yo+dy-rad), zo, (xo+dx), (yo+rad), zo );
		DbLine lineG = new DbLine( (xo+dx-rad), yo, zo, (xo+rad), yo, zo );

		DbArc arcB = null;
		DbArc arcD = null;
		DbArc arcF = null;
		DbArc arcH = null;
		Point pc;
		
		if (dir != 0)
		{
			if (dir == Const.CCW)
				pc = new Point( xo, (yo+dy), zo );
			else
				pc = new Point( lineC.StartPt().X(), lineA.EndPt().Y(), zo );
			arcB = new DbArc( lineA.EndPt(), lineC.StartPt(), pc, dir );

			if (dir == Const.CCW)
				pc = new Point( (xo+dx), (yo+dy), zo );
			else
			pc = new Point( lineC.EndPt().X(), lineE.StartPt().Y(), zo );
				arcD = new DbArc( lineC.EndPt(), lineE.StartPt(), pc, dir );

			if (dir == Const.CCW)
				pc = new Point( (xo+dx), yo, zo );
			else
				pc = new Point( lineG.StartPt().X(), lineE.EndPt().Y(), zo );
			arcF = new DbArc( lineE.EndPt(), lineG.StartPt(), pc, dir );

			if (dir == Const.CCW)
				pc = new Point( xo, yo, zo );
			else
				pc = new Point( lineG.EndPt().X(), lineA.StartPt().Y(), zo );
			arcH = new DbArc( lineG.EndPt(), lineA.StartPt(), pc, dir );
		}
		
		DbProfile dbProfile = new DbProfile();
		if (dbProfile != null)
		{
			dbProfile.Append( lineA );
			if (arcB != null)
				dbProfile.Append( arcB );
			
			dbProfile.Append( lineC );
			if (arcD != null)
				dbProfile.Append( arcD );
			
			dbProfile.Append( lineE );
			if (arcF != null)
				dbProfile.Append( arcF );
			
			dbProfile.Append( lineG );
			if (arcH != null)
				dbProfile.Append( arcH );

			Selector.StateSave();
			Selector.All( false );
			Selector.Line( true );
			Selector.Arc( true );
			Selector.Profile( true );
			
			Selector.Add( dbProfile );
			
			if (xdim < 0)
				Editor.Mirror( xo, yo, true, false );  // mirror about Y
			
			if (ydim < 0)
				Editor.Mirror( xo, yo, false, false );  // mirror about X
			
			if (dbProfile.Dir() != windingDirection)
			{
				dbProfile.Reverse();
				// We still want lineA to be the first entity.
				Editor.Chain( lineA, true, Const.SMALL );
			}
			
			Selector.Flush();
			Selector.StateRestore();
		}

		return dbProfile;
	}
	
	/**
	 * Creates a feature entity representing a bolt hole circle (or arc).
	 * @param bhcOrigin the center point of the BHC.
	 * @param bhcRadius the radius of the BHC.
	 * @param bhcStartDegs the start angle of the BHC (in degrees).
	 * @param bhcDeltaDegs the signed increment between holes (in degrees).
	 * @param bhcHoleCount the number of holes.
	 * @param oneHoleRadius the radius of a hole in the pattern.
	 * @return a feature containing a BHC of circles.
	 */
	static public DbFeature BHC(
		Point	bhcOrigin,
		double	bhcRadius,
		double	bhcStartDegs,
		double	bhcDeltaDegs,
		int		bhcHoleCount,
		double	oneHoleRadius )
	{
		DbFeature bhc = null;
		
		if (bhcHoleCount > 0)
		{
			double zc = bhcOrigin.Z();
			
			for (int indx = 0; indx < bhcHoleCount; ++indx)
			{
				double radians = (bhcStartDegs + (indx * bhcDeltaDegs)) * Const.DEG2RAD;
				
				double xc = bhcOrigin.X() + (bhcRadius * Math.cos( radians ));
				double yc = bhcOrigin.Y() + (bhcRadius * Math.sin( radians ));
				
				Point pc = new Point( xc, yc, zc );
				DbArc hole = new DbArc( pc, oneHoleRadius, Const.CCW );
				
				if (indx == 0)
					bhc = new DbFeature();
					
				bhc.Append( hole );
			}
		}
		
		return bhc;
	}

	private Shape()
	{
	}
}

