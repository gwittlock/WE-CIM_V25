
public class ArcTest
{
	static public final double QUARTERPI = (0.25 * Math.PI);
	static public final double RAD2DEG = (180.0 / Math.PI);

	static public void main ( String argv[] )
	{
		double [] angles = new double[2];

		Arc cwArc  = new Arc( 1.0, 0.0, 0.0,
							  0.0, 1.0, 0.0,
							  0.0, 0.0, 0.0,
							  Arc.CW );

		Arc ccwArc = new Arc( 1.0, 0.0, 0.0,
							  0.0, 1.0, 0.0,
							  0.0, 0.0, 0.0,
							  Arc.CCW );

		System.out.println( "------------ Test CW arc ------------------" );

		for (int indx = 8; indx >= 0; --indx)
		{
			double radians = (indx * QUARTERPI);

			double xs = Math.cos( radians );
			double ys = Math.sin( radians );

			cwArc.Xs( xs );
			cwArc.Ys( ys );

			cwArc.Angles( angles );

			angles[0] *= RAD2DEG;
			angles[1] *= RAD2DEG;

			System.out.println( "as <" + angles[0] + ">  ae <" + angles[1] + ">" );

			// System.out.println( "---------------------------------" );
			// System.out.println( "xs <" + xs + ">   ys <" + ys + ">" );
		}


		System.out.println( "------------ Test CCW arc ------------------" );

		for (int jndx = 0; jndx < 9; ++jndx)
		{
			double radians = (jndx * QUARTERPI);

			double xs = Math.cos( radians );
			double ys = Math.sin( radians );

			ccwArc.Xs( xs );
			ccwArc.Ys( ys );

			ccwArc.Angles( angles );

			angles[0] *= RAD2DEG;
			angles[1] *= RAD2DEG;

			System.out.println( "as <" + angles[0] + ">  ae <" + angles[1] + ">" );

			// System.out.println( "---------------------------------" );
			// System.out.println( "xs <" + xs + ">   ys <" + ys + ">" );
		}

	}

}
