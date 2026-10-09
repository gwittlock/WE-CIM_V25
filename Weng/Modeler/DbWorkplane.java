
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;

/**
 * Use this class to create, query and manipulate a workplane
 * entity that resides in the modeler's database.
 */
public class DbWorkplane extends DbEntity
{
	/**
	 * Creates a workplane entity having the given properties.
	 * @param name a unique name for this workplane.  System
	 *             reserved names are ( top, left, right, front, back ).
	 * @param ix the X component of the X axis vector
	 * @param iy the Y component of the X axis vector
	 * @param iz the Z component of the X axis vector
	 * @param jx the X component of the Y axis vector
	 * @param jy the Y component of the Y axis vector
	 * @param jz the Z component of the Y axis vector
	 * @param kx the X component of the Z axis vector
	 * @param ky the Y component of the Z axis vector
	 * @param kz the Z component of the Z axis vector
	 * @param tx the X origin relative to  world
	 * @param ty the Y origin relative to  world
	 * @param tz the Z origin relative to  world
	 * @param up (true) Z axis out out workplane / (false) Z axis into workplane
	 */
	public DbWorkplane(
				String name,
				double ix, double iy, double iz,
				double jx, double jy, double jz,
				double kx, double ky, double kz,
				double tx, double ty, double tz,
				boolean up )
	{
		String cmd = "Create:Plane:"
					 + " name=\"" + name + "\""
					 + ",ix=" + ix
					 + ",iy=" + iy
					 + ",iz=" + iz
					 + ",jx=" + jx
					 + ",jy=" + jy
					 + ",jz=" + jz
					 + ",kx=" + kx
					 + ",ky=" + ky
					 + ",kz=" + kz
					 + ",tx=" + tx
					 + ",ty=" + ty
					 + ",tz=" + tz
					 + ",up=" + (up ? 1 : 0);

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			Id( id );
		}
	}

	/**
	 * Conversion operator.
	 */
	static public DbWorkplane DbWorkplane( DbEntity dbEntity )
	{
		int id = dbEntity.Id();
		if (id <= 0)
			return null;

		if (dbEntity.Type() != Const.WORKPLANE)
			return null;
			
		return (new DbWorkplane( id ));
	}


	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Protected methods
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Create a proxy for an existing database workplane.
	// Generally used when obtaining the workplane associated
	// with an entity.  See also DbEntity::Workplane().
	protected DbWorkplane( int id )
	{
		Id( id );
	}
}

