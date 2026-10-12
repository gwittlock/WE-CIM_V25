
package Weng.Modeler;

import Weng.System.Msg;
import Weng.System.Portal;
import Weng.System.Const;
import Weng.System.Registry;
import Weng.Math.UnitVec2d;
import Weng.Math.UnitVec3d;
import Weng.Geometry.*;


/**
 * Use this singleton class to create 'toolpath' entities, that is,
 * features whose contained entities reference a tool and whose
 * geometry is used to drive the tool during cutting processes.
 * <p>
 * With regards to <b>offset direction</b>, it suggested that you
 * use the constants Const.LEFT and Const.RIGHT that are
 * define in Weng.System.Const.  Otherwise, offset direction
 * follows protractor convention (+) LEFT / (-) RIGHT.  For
 * the uniformed, offset direction is relative to arc direction.
 * <p>
 * With regards to <b>cut direction</b>, it suggested that you
 * use the constants Const.CCW and Const.CW that are
 * defined in Weng.System.Const.  Otherwise, arc direction
 * follows protractor convention (+) CCW / (-) CW.
 */
public class Toolpath
{
	/**
	 * This method is analogous to the machining menu item known
	 * as Create/Associate.  Given appropriate input parameters,
	 * this method will create profiling toolpath that is associated
	 * with a given profile entity.
	 * @param dbProfile the reference profile entity
	 * @param dbTool the tool entity that will be used
	 * @param offsetDirection the side of the reference profile towards
	 *                        which the resulting offset toolpath should
	 *                        be created
	 * @param wallAllowance a signed delta offset distance (also known as
	 *                      stock allowance).  By default, the offset distance
	 *                      is taken as the radius of the tool.  The wall
	 *                      allowance value is added to the default offset
	 *                      distance.
	 * @param sharpAngle controls the insertion of blending radius between
	 *                   adjacent curves in the resulting offset profile.
	 *                   A blend radius be inserted into the offset profile
	 *                   when the interior angle between the corresponding
	 *                   adjacent reference curves is less than the specified
	 *                   angle (<b>degrees</b>).  For instance, given a square
	 *                   reference profile and a sharpAngle of 90 degrees, the
	 *                   resulting offset profile will not contain any blending
	 *                   arcs.  If the sharpAngle is 90.01 degrees the resulting
	 *                   offset profile will contain four blending arcs.
	 * @param zLevel the elevation of the resulting toolpath geometry
	 * @return (null) indicates failure / (else) the resulting toolpath feature
	 */
	static public DbFeature Profile(
								DbProfile	dbProfile,
								DbTool		dbTool,
								int			offsetDirection,
								double		wallAllowance,
								double		sharpAngle,
								double		zLevel )
	{
		DbFeature dbFeature = DbProfile.Offset(
									dbProfile.Id(),
									dbTool.Id(),
									offsetDirection,
									wallAllowance,
									sharpAngle,
									zLevel );

		return dbFeature;
	}

	static public DbFeature Profile(
								DbProfile	dbProfile,
								DbTool		dbTool,
								boolean		climbCut,
								double		wallAllowance,
								double		sharpAngle,
								double		zLevel )
	{
		DbFeature dbFeature = DbProfile.Offset(
									dbProfile.Id(),
									dbTool.Id(),
									climbCut,
									wallAllowance,
									sharpAngle,
									zLevel );

		return dbFeature;
	}

	/**
	 * This is the sister method to <b>Profile( DbProfile .... )</b> but is used
	 * for creating profiling toolpath that is associated to a single curve entity.
	 */
	static public DbFeature Profile(
								DbCurve	dbCurve,
								DbTool	dbTool,
								int		offsetDirection,
								double	wallAllowance,
								double	sharpAngle,
								double	zLevel )
	{
		DbFeature dbFeature = DbProfile.Offset(
									dbCurve.Id(),
									dbTool.Id(),
									offsetDirection,
									wallAllowance,
									sharpAngle,
									zLevel );

		return dbFeature;
	}

	/**
	 * This method is analogous to the machining menu item known
	 * as Create/Pocket.  Given appropriate input parameters, this
	 * method will create 'spiral' pocketing toolpath that is associated
	 * with a given profile entity.
	 * @param dbProfile the reference profile entity
	 * @param dbTool the tool entity that will be used
	 * @param stepOverDistance the lateral separation between adjacent offset
	 *                         passes in the resulting toolpath.
	 * @param wallAllowance a signed delta offset distance (also known as
	 *                      stock allowance).  By default, the offset distance
	 *                      is taken as the radius of the tool.  The wall
	 *                      allowance value is added to the default offset
	 *                      distance.
	 * @param cutDirection the sense of forward cutting progression (Const.CCW,
	 *                     Const.CW)
	 * @param insideOut the sense of lateral cutting progression. (true) cut
	 *                  from the inside of the pocket towards the outside / (false)
	 *                  cut from the outside of the pocket towards the inside.
	 * @param sharpAngle controls the insertion of blending radius between
	 *                   adjacent curves in the resulting offset profile.
	 *                   A blend radius be inserted into the offset profile
	 *                   when the interior angle between the corresponding
	 *                   adjacent reference curves is less than the specified
	 *                   angle (<b>degrees</b>).  For instance, given a square
	 *                   reference profile and a sharpAngle of 90 degrees, the
	 *                   resulting offset profile will not contain any blending
	 *                   arcs.  If the sharpAngle is 90.01 degrees the resulting
	 *                   offset profile will contain four blending arcs.
	 * @param passDepth the vertical separation between planar cutting regions
	 * @param floorAllowance the stock allowance amount to apply to the floor of
	 *                       the pocket.  Note that the floor of the pocket is
	 *                       taken to be the elevation of the reference profile.
	 * @return (null) indicates failure / (else) the resulting toolpath feature
	 */
	static public DbFeature Spiral(
								DbProfile	dbProfile,
								DbTool		dbTool,
								double		stepOverDistance,
								double		wallAllowance,
								int			cutDirection,
								boolean		insideOut,
								double		sharpAngle,
								double		passDepth,
								double		floorAllowance )
	{
		DbFeature dbFeature = Spiral(
									dbProfile.Id(), dbTool, stepOverDistance,
									wallAllowance, cutDirection, insideOut,
									sharpAngle, passDepth, floorAllowance );

		return dbFeature;
	}

	/**
	 * This is the sister method to <b>Spiral( DbProfile .... )</b> but is used
	 * for creating spiral pocketing toolpath that is associated to a single
	 * curve entity.
	 */
	static public DbFeature Spiral(
								DbCurve	dbCurve,
								DbTool	dbTool,
								double	stepOverDistance,
								double	wallAllowance,
								int		cutDirection,
								boolean	insideOut,
								double	sharpAngle,
								double	passDepth,
								double	floorAllowance )
	{
		DbFeature dbFeature = Spiral(
									dbCurve.Id(), dbTool, stepOverDistance,
									wallAllowance, cutDirection, insideOut,
									sharpAngle, passDepth, floorAllowance );

		return dbFeature;
	}

	/**
	 * Creates 'Pencil Milling' toolpath that cleans up the pocket-floor/wall and
	 * wall/wall intersections.  Optional nib removal is provided.  It is assumed
	 * that the top of the material is at Z0.  It follows that clearance is a
	 * distance above Z0, and depth is a distance below Z0.
	 * @param profile the profile representing the outline of the pocket floor.
	 * @param dbTool the tool that will be used to cut the material.
	 * @param cutSide one of <b>Const.LEFT</b> or <b>Const.RIGHT</b>.
	 * @param offsetDist the distance to offset the center of the tool from the profile
	 * @param sharpAngle the limiting angle (degrees) of an outside corner.  Any outside
	 *     corner whose interior angle is smaller than this limit will be navigated using
	 *     a blending arc.
	 * @param depth the distance below Z0 at which cutting will take place.
	 * @param clear the distance above Z0 to which the tool must move to clean up the
	 *     wall/wall intersections.
	 * @param wallAngle the inclination angle (degrees) of the wall from the vertical.
	 * @param nibRemoval determines whether nib-removal movements will be generated.
	 * @param distA used during nib-removal, is the distance to move away from the corner.
	 * @param distB used during nib-removal, is the distance to move parallel to a wall.
	 */
	static public DbFeature PencilMill(
								DbProfile	profile,
								DbTool		dbTool,
								int			cutSide,
								double		offsetDist,
								double		sharpAngle,
								double		depth,
								double		clear,
								double		wallAngle,
								boolean		nibRemoval,
								double		distA,
								double		distB )
	{
		DbTool activeTool;
		DbWorkplane activeWorkplane;
		DbFeature tp;
		DbProfile offset;
		DbCurve curveA, curveB;
		DbLine lineA, lineB;
		UnitVec2d tanA, tanB;
		UnitVec3d anorm, bnorm, axb;
		Point ps, pe;
		double diam, rad, cross;
		double cosB, sinB, lift, t;
		int profileCount;
		int offsetCount;
		int indxA, indxB;
		int color;
		boolean isClosed;
		
		int sign = (Model.IsRightHanded() ? 1 : -1);

		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Make some trivial checks as to whether we can even attempt to
		// create pencil milling toolpath.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		profileCount = profile.Count();
		if (profileCount < 2)
			return null;  // Must have at least one corner.


		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Create the pencil mill toolpath at the bottom and get the
		// profile representing the toolpath centerline.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		tp = Toolpath.Profile( profile, dbTool, cutSide, offsetDist, sharpAngle, depth );
		if (tp == null || tp.Count() != 1)
		{
			// We have no result, or the result has a bifurcation.
			tp.Delete();
			return null;
		}

		offset = DbProfile.DbProfile( tp.Get(0) );
		offsetCount = offset.Count();


		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// General preparation.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		anorm	= new UnitVec3d();
		bnorm	= new UnitVec3d();
		axb		= new UnitVec3d();
		pe		= new Point();

		activeTool = Model.ActiveToolGet();
		activeWorkplane = Model.ActiveWorkplaneGet();

		Model.ActiveToolSet( offset.Tool() );
		Model.ActiveWorkplaneSet( offset.Workplane() );

		isClosed = offset.IsClosed();
		color = offset.Color();


		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
		// Generate the ramp and nib removal moves.
		//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

		diam = dbTool.DoubleGet( "diameter" );
		rad = ((diam >= Const.UNDEFINED) ? 0.05 : (0.5 * diam));

		cosB = Math.cos( wallAngle * Const.DEG2RAD );
		sinB = Math.sin( wallAngle * Const.DEG2RAD );
		lift = ((Math.abs(clear) + Math.abs(depth)) * sign);

		indxA = 0;
		indxB = 0;

		curveA = null;
		curveB = DbCurve.DbCurve( offset.Get( 0 ) );

		// As with offsetting, we must adjust for
		// the 'handedness' of the coordinate system.
		// cutSide *= sign;
	
		while (true)
		{
			offsetCount = offset.Count();
			if (indxA >= offsetCount)
				break;  // Completed the traversal.

			indxB = indxA + 1;
			if (isClosed && indxB >= offsetCount)
				indxB = 0;

			curveA = curveB;
			curveB = DbCurve.DbCurve( offset.Get( indxB ) );

			tanA = curveA.EndTan();
			tanB = curveB.StartTan();

			cross = UnitVec2d.Cross( tanA, tanB );

			// Msg.Display( "cross: " + cross + "  cutside: " + cutSide +
			//			"  product:" + (cutSide*cross) );
			
			if (((cutSide * sign) * cross) >= 0.5)    // Const.SMALL)
			{
				// We've encountered an inside corner.  Create a ramp move
				// that walks up the intersection between the adjacent walls.
				// NOTE: This action is limited to corners whose interior
				// angle is greater-than-or-equal to 30 degrees.  Sharper
				// angles will yield ramp moves whose lateral component is
				// so large that the move will rip through the part.

				//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
				// Calculate the face/face intersection.
				//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
				//
				// The normal to each face is calculated by
				//
				//		|x|   | cosA -sinA 0 | |  1   0    0   | |x'|
				//		|y| = | sinA  cosA 0 | |  0  cosB sinB | |y'|
				//		|z|   |  0     0   1 | |  0 -sinB cosB | |z'|
				//
				// which reduces to
				//
				//		|x|   |-(sinA*cosB)|
				//		|y| = | (cosA*cosB)|
				//		|z|   |  -(sinB)   |
				//
				// where
				//
				//		A is the angle of rotation about the Z axis
				//		B is the angle of rotation about the X' axis
				//		(x',y',z') = (0,1,0) is the nominal plane normal
				//
				if (cutSide < 0)
				{
					anorm.Assign( -(tanA.Y() * cosB), (tanA.X() * cosB), (sinB) );
					bnorm.Assign( -(tanB.Y() * cosB), (tanB.X() * cosB), (sinB) );
				}
				else
				{
					anorm.Assign( (tanA.Y() * cosB), -(tanA.X() * cosB), (sinB) );
					bnorm.Assign( (tanB.Y() * cosB), -(tanB.X() * cosB), (sinB) );
				}

				UnitVec3d.Cross( anorm, bnorm, axb );
				if (axb.Z() < 0)
					axb.Reverse();

				//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
				// Calculate where the vector intersects the clearance plane.
				//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

				t = lift / axb.Z();

				// Calculate the end points of the ramp moves.
				ps = curveA.EndPt();
				pe.Assign(
						(ps.X() - (t * axb.X())),
						(ps.Y() - (t * axb.Y())),
						(ps.Z() + (t * axb.Z())) );

				// Create the ramp moves.
				lineA = new DbLine( ps, pe );
				lineB = new DbLine( pe, ps );

				lineA.Color( color );
				lineB.Color( color );

				offset.InsertAfter( curveA, lineA );
				offset.InsertAfter( lineA, lineB );

				indxA += 2;

				if (nibRemoval && distA > Const.SMALL && distB > Const.SMALL)
				{
					DbLine away, back, legA, legB, legC;

					UnitVec2d bisector = new UnitVec2d(
												((tanB.X() - tanA.X()) / 2),
												((tanB.Y() - tanA.Y()) / 2) );

					Point apex = new Point(
										(ps.X() + (distA * bisector.X())),
										(ps.Y() + (distA * bisector.Y())),
										 ps.Z() );

					// Create the move away from the corner.
					away = new DbLine( ps, apex );
					away.Color( color );

					// Create the move back to the corner.
					back = new DbLine( apex, ps );
					back.Color( color );

					// Create the first leg of the triangle.
					pe.Assign(
							(apex.X() + (distB * tanB.X())),
							(apex.Y() + (distB * tanB.Y())),
							 apex.Z() );

					legA = new DbLine( apex, pe );
					legA.Color( color );

					// Create the final leg of the triangle.
					ps.Assign(
							(apex.X() - (distB * tanA.X())),
							(apex.Y() - (distB * tanA.Y())),
							 apex.Z() );

					legC = new DbLine( ps, apex );
					legC.Color( color );

					// Create the hypotenuse of the triangle.
					legB = new DbLine( legA.EndPt(), legC.StartPt() );
					legB.Color( color );

					// Add the moves into the offset profile.
					offset.InsertAfter( lineB, away );
					offset.InsertAfter( away,  legA );
					offset.InsertAfter( legA,  legB );
					offset.InsertAfter( legB,  legC );
					offset.InsertAfter( legC,  back );

					indxA += 5;
				}
			}

			++indxA;
		}

		Model.ActiveToolSet( activeTool );
		Model.ActiveWorkplaneSet( activeWorkplane );

		return tp;
	}

	/**
	 * Applies lead-in/lead-out moves to the given toolpath feature as
	 * directed by the lead parameters associated with the given setup id.
	 * @param tp the toolpath feature to which the leads will be applied
	 * @param leadSetupID the id of the lead setup parameters in the cmdb
	 * @param exterior (true) the given toolpath represents exterior cuts
	 *     (false) the given toolpath represents interior cuts.
	 */
	static public boolean LeadsApply( DbFeature tp, int leadSetupID, boolean exterior )
	{
		if (tp == null || tp.Tool() == null)
			return false;

		int count = tp.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			DbEntity dbEntity = tp.Get(indx);
			int type = dbEntity.Type();

			if (type == Const.PROFILE)
			{
				String cmd = "Lead:Semi:"
							 + " configdb=\"" + CMDB.Path() + "\""
							 + ",id=" + dbEntity.Id()
							 + ",setup=" + leadSetupID
							 + ",exterior=" + (exterior ? 1 : 0);

				boolean okay = Portal.Execute( cmd );
			}
		}

		return true;
	}

	/**
	 * Applies lead-in/lead-out moves to the given toolpath feature as
	 * directed by the lead parameters associated with the given setup id.
	 * @param tp the toolpath feature to which the leads will be applied
	 * @param leadSetupID the id of the lead setup parameters in the cmdb
	 * @param exterior (true) the given toolpath represents exterior cuts
	 *     (false) the given toolpath represents interior cuts.
	 * @param minOverlap the minimum overlap distance.  This value is used
	 *     when the product of the distance factor and the tool tip diameter
	 *     is less that the specified minimum overlap distance.
	 */
	static public boolean LeadsApply(
								DbFeature	tp,
								int			leadSetupID,
								boolean		exterior,
								double		minOverlap )
	{
		// This latter method was introduced to resolve a PanelFront issue
		// where a pencil milling tool would leave material behind.  This
		// behavior was related to the fact that the tool is defined using
		// the tip diameter, and that the overlap distance is specified
		// as a factor of the tool diameter.

		if (tp == null || tp.Tool() == null)
			return false;

		int count = tp.Count();
		for (int indx = 0; indx < count; ++indx)
		{
			DbEntity dbEntity = tp.Get(indx);
			int type = dbEntity.Type();

			if (type == Const.PROFILE)
			{
				String cmd = "Lead:Semi:"
							 + " configdb=\"" + CMDB.Path() + "\""
							 + ",id=" + dbEntity.Id()
							 + ",setup=" + leadSetupID
							 + ",exterior=" + (exterior ? 1 : 0)
							 + ",min_overlap=" + minOverlap;

				boolean okay = Portal.Execute( cmd );
			}
		}

		return true;
	}

	/**
	 * Intended for laser/burners, segments the material skeleton so
	 * that it can be removed in sections.
	 * @param dbTool the tool that will perform the cutting operation
	 * @param angle the slicing angle (degrees) from horizontal
	 * @param stepover the distance between adjacent slices
	 * @return (null) indicates failure / (?) the feature containing
	 *         all of the slicing geometry.
	 */
	static public DbFeature SkeletonSlit(
								DbTool	dbTool,
								double	angle,
								double	stepover )
	{
		String cmd = "Toolpath:Slit:"
					+ " toolid=" + dbTool.Id()
					+ ",ang=" + angle
					+ ",step=" + stepover;
					
		DbFeature dbFeature = FeatureCreate( cmd );

		return dbFeature;
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	static private DbFeature Spiral(
								int entityId,
								DbTool dbTool,
								double stepOverDistance,
								double wallAllowance,
								int cutDirection,
								boolean insideOut,
								double sharpAngle,
								double passDepth,
								double floorAllowance )
	{
		String cmd = "Toolpath:Spiral:"
					 + " id=0"
					 + ",profid=" + entityId
					 + ",toolid=" + dbTool.Id()
					 + ",stepover=" + stepOverDistance
					 + ",wallallow=" + wallAllowance
					 + ",cw=" + ((cutDirection > 0) ? 0 : 1)
					 + ",insideout=" + (insideOut ? 1 : 0)
					 + ",sharp=" + sharpAngle
					 + ",passdepth=" + passDepth
					 + ",floorallow=" + floorAllowance;

		DbFeature dbFeature = FeatureCreate( cmd );

		return dbFeature;
	}

	static private DbFeature FeatureCreate( String cmd )
	{
		DbFeature dbFeature = null;

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				dbFeature = new DbFeature( id );
		}

		return dbFeature;
	}

	private Toolpath()
	{
	}
}

