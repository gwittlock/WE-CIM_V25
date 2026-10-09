
// mac.dx|dbl|Length|5
// mac.dy|dbl|Width|4
// mac.dim90|dbl|Square Corner Distance|0.5
// mac.dim45|dbl|Chamfer distance|0.5
// mac.orient|dbl|Orientation|0.0
// mac.type|combo|Reference Point Type|Lower Left Corner|Center
// mac.xp|dbl|Reference Point X|0.0
// mac.yp|dbl|Reference Point Y|0.0

import Weng.System.*;
import Weng.Math.*;
import Weng.Geometry.*;
import Weng.Modeler.*;


public class NotchedRectangle implements WengMacro
{
	// This method is called via the Regenerate button.
	public void main()
	{
		try
		{
			double xc, yc;
			
			double dx = Model.DoubleGet( "mac.dx" ) / 2;
			double dy  = Model.DoubleGet( "mac.dy" ) / 2;
			double dim90 = Model.DoubleGet( "mac.dim90" );
			double dim45 = Model.DoubleGet( "mac.dim45" );
			double orient = Model.DoubleGet( "mac.orient" );
			String type = Model.StringGet( "mac.type" );
			double xref = Model.DoubleGet( "mac.xp" );
			double yref = Model.DoubleGet( "mac.yp" );
			
			if ( type.equalsIgnoreCase( "Lower Left Corner" ) )
			{
				xc = xref + dx;
				yc = yref + dy;
			}
			else
			{
				xc = xref;
				yc = yref;
			}
			
			Point ptA = new Point( (xc - dx), (yc + dy), 0. );
			Point ptB = new Point( (xc + dx), (yc + dy), 0. );
			Point ptC = new Point( (xc + dx), (yc - dy), 0. );
			Point ptD = new Point( (xc - dx), (yc - dy), 0. );
			
			DbProfile dbProfile = new DbProfile();
			dbProfile.Append( new DbLine( ptA, ptB ) );
			dbProfile.Append( new DbLine( ptB, ptC ) );
			dbProfile.Append( new DbLine( ptC, ptD ) );
			dbProfile.Append( new DbLine( ptD, ptA ) );
			
			NotchIt( dbProfile, dim90, dim45 );
			
			Selector.StateSave();
			Selector.All( true );
			Selector.Add( dbProfile );
			Editor.Rotate( xref, yref, orient, 0 );
			Selector.StateRestore();
		}
		catch (Exception e)
		{
			ExceptionPrinter.StackTracePrint( e );
		}
	}
	
	private void NotchIt( DbProfile dbProfile, double dim90, double dim45 )
	{
		DbLine dbLineA, dbLineB;
		DbLine dbNotchA, dbNotchB;
		Point pt;
		int indxA, indxB, jndx;
		
		Point [] notch = NotchPointsCreate( dim90, dim45 );
		
		if (notch == null)
		{
			Msg.Display( "Failed to create notch points." );
		}
		else
		{
			for (indxA = 0; indxA <= 3; ++indxA)
			{
				indxB = ((indxA < 3) ? (indxA + 1) : 0);
				
				dbLineA = DbLine.DbLine( dbProfile.Get(indxA) );
				dbLineB = DbLine.DbLine( dbProfile.Get(indxB) );
				
				pt = dbLineA.EndPt();
				
				Selector.StateSave();
				Selector.All( false );
				Selector.Line( true );
			
				for (jndx = 0; jndx < (notch.length - 1); ++jndx)
				{
					Selector.Add( new DbLine( notch[jndx], notch[jndx+1] ) );
				}
				
				Editor.Rotate( 0., 0., -(indxB * 90.), 0 );
				Editor.Move( pt.X(), pt.Y(), 0., 0 );
				
				dbNotchA = DbLine.DbLine( Selector.Get(0) );
				dbNotchB = DbLine.DbLine( Selector.Get(notch.length-2) );
				
				dbLineA.EndPt( dbNotchA.StartPt() );
				dbLineB.StartPt( dbNotchB.EndPt() );
				
				Selector.StateRestore();
			}
			
			Editor.Chain( DbLine.DbLine( dbProfile.Get(0) ), false, Const.SMALL );
		}
	}
	
	private Point [] NotchPointsCreate( double dim90, double dim45 )
	{
		Point [] pts = null;
		int count = ((dim90 > Const.SMALL) ? 3 : 0) + ((dim45 > Const.SMALL) ? 2 : 0);
		
		if (count == 2)
		{
			pts = new Point[2];
			pts[0] = new Point( 0., -dim45, 0. );
			pts[1] = new Point( dim45, 0., 0. );
		}
		else if (count == 3)
		{
			pts = new Point[3];
			pts[0] = new Point( 0., -dim90, 0. );
			pts[1] = new Point( dim90, - dim90, 0. );
			pts[2] = new Point( dim90, 0., 0. );
		}
		else if (count == 5)
		{
			pts = new Point[5];
			pts[0] = new Point( 0., -(dim90 + (2 * dim45)), 0. );
			pts[1] = new Point( dim45, -(dim90 + dim45), 0. );
			pts[2] = new Point( (dim90 + dim45), -(dim90 + dim45), 0. );
			pts[3] = new Point( (dim90 + dim45), -dim45, 0. );
			pts[4] = new Point( (dim90 + (2 * dim45)), 0., 0. );
		}
		
		return pts;
	}
}

