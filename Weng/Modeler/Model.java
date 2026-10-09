
package Weng.Modeler;

import Weng.System.Portal;
import Weng.System.Const;
import Weng.System.Registry;
import Weng.Math.Box2d;


/**
 * This singleton class serves as a proxy to the modeler database.
 * Use this class to get entities from the modeler database.
 */
public class Model
{
	/**
	 * Initializes the model database.  Creates the standard workplane
	 * and tool entities, creates the stock outline, initializes the
	 * model header attributes and creates the tool entities.
	 */
	static public boolean Init(
							String machineName,
							String toolSetupName,
							String materialName,
							String nestSetupName,
							double dx,
							double dy )
	{
		String cmd = "Model:Init:"
					 + " db=\""   + DatabasePath() + "\""
					 + ",mach=\"" + machineName + "\""
					 + ",tool=\"" + toolSetupName + "\""
					 + ",mat=\""  + materialName + "\""
					 + ",nest=\"" + ((nestSetupName == null) ? "" : nestSetupName) + "\""
					 + ",dx="     + dx
					 + ",dy="     + dy;

		Portal.Execute( "Admin:New:" );

		return ( Portal.Execute( cmd ) );
	}

	static public boolean Init(
							int toolSetupID,
							int materialID,
							double dx,
							double dy )
	{
		String cmd = "Model:Init:"
					 + " db=\""   + DatabasePath() + "\""
					 + ",tsid=\"" + toolSetupID + "\""
					 + ",matid=\""  + materialID + "\""
					 + ",dx="     + dx
					 + ",dy="     + dy;

		Portal.Execute( "Admin:New:" );

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Reads the contents of an MM2 file into the modeler.
	 */
	static public boolean Import( String fullyQualifiedPath )
	{
		return ImportMM2( fullyQualifiedPath );
	}

	/**
	 * Writes the contents of the modeler to an MM2 file.
	 */
	static public boolean Export( String fullyQualifiedPath )
	{
		return ExportMM2( fullyQualifiedPath );
	}

	/**
	 * Reads the contents of an MM2 file into the modeler.
	 * @param path the fully qualified mm2 file path
	 */
	static public boolean ImportMM2( String path )
	{
		String cmd = "File:ImportMM2: file=\"" + path + "\"";

		// Flush the model and selector.
		Portal.Execute( "Admin:New:" );

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Merge the contents of an MM2 file into the modeler.
	 * @param path the fully qualified mm2 file path
	 * @param ox the X ordinate at which to place the X origin of the model being merged.
	 * @param oy the Y ordinate at which to place the Y origin of the model being merged.
	 * @param oz the Z ordinate at which to place the Z origin of the model being merged.
	 */
	static public boolean MergeMM2( String path, double ox, double oy, double oz )
	{
		String cmd = "File:MergeMM2:"
				   + " file=\"" + path + "\""
				   + ",ox=" + ox
				   + ",oy=" + oy
				   + ",oz=" + oz;

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Reads the contents of a DXF file into the modeler.
	 * @param dxfPath the fully qualified dxf file path
	 * @param dbPath the fully qualified machining database file path
	 * @param layerSetupID the machining database Layer Setup ID to be used
	 * @param toolSetupID the machining database Tool Setup ID to be used
	 * @param materialID the machining database Material ID to be used
	 */
	static public boolean ImportDXF(
								String	dxfPath,
								String	dbPath,
								int		layerSetupID,
								int		toolSetupID,
								int		materialID )
	{
		String cmd = "File:ImportDXF: dxf=\"" + dxfPath + "\""
					 + ",db=\"" + dbPath + "\""
					 + ",layersetupid=" + layerSetupID
					 + ",toolsetupid="  + toolSetupID
					 + ",materialindex=" + materialID;

		// Flush the model and selector.
		Portal.Execute( "Admin:New:" );

		// Import the specified file.
		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Writes the contents of the modeler to an MM2 file.
	 * @param path the fully qualified mm2 file path
	 */
	static public boolean ExportMM2( String path )
	{
		String cmd = "File:ExportMM2: file=\"" + path + "\"";

		return ( Portal.Execute( cmd ) );
	}

	static public boolean StockUpdate( double dx, double dy, double dz )
	{
		String cmd = "Model:StockUpdate:"
					 + " dx=" + dx
					 + ",dy=" + dy
					 + ",dz=" + dz;

		return ( Portal.Execute( cmd ) );
	}
			
	/**
	 * Gets the modelers active tool entity.
	 */
	static public DbTool ActiveToolGet()
	{
		DbTool dbTool = null;

		if ( Portal.Execute("Model:ActiveTool:") )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				dbTool = new DbTool( id );
		}

		return dbTool;
	}

	/**
	 * Sets the modelers active tool entity.
	 * @return the active tool
	 */
	static public DbTool ActiveToolSet( DbTool dbTool )
	{
		DbTool	activeTool;
		String	cmd;
		
		activeTool = ActiveToolGet();
		
		cmd = "Model:ActiveTool: id=" + dbTool.Id();
		if ( Portal.Execute( cmd ) )
			DefIntSet( "color", dbTool.Color() );

		return activeTool;
	}

	/**
	 * Gets the modelers active workplane entity.
	 */
	static public DbWorkplane ActiveWorkplaneGet()
	{
		DbWorkplane dbWorkplane = null;

		if ( Portal.Execute("Model:ActiveWorkplane:") )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				dbWorkplane = new DbWorkplane( id );
		}

		return dbWorkplane;
	}

	/**
	 * Sets the modelers active workplane entity.
	 */
	static public DbWorkplane ActiveWorkplaneSet( DbWorkplane dbWork )
	{
		DbWorkplane	activeWork;
		String		cmd;
		
		activeWork = ActiveWorkplaneGet();
		
		cmd = "Model:ActiveWorkplane: id=" + dbWork.Id();
		Portal.Execute( cmd );
		
		return activeWork;
	}

	/**
	 * Gets the count of the given entity type.
	 * @param entityType one of {System.Const.WORKPLANE .. System.Const.FEATURE}
	 */
	 static public int EntityCount( int entityType )
	 {
		String cmd = "Model:EntityCount: type=" + entityType;

		boolean okay = Portal.Execute( cmd );

		return (okay ? Portal.IntGet( "count" ) : 0);
	 }

	/**
	 *
	 */
	static public DbEntity EntityGet( String name )
	{
		int id = ModelGet( name, Const.WORKPLANE, Const.FEATURE );

		return ((id == 0) ? null : new DbEntity( id ));
	}

	/**
	 * Gets the named tool entity from the modeler.
	 * @return (null) indicates the named tool was not found
	 */
	static public DbTool ToolGet( String name )
	{
		DbTool dbTool = null;

		String cmd = "Model:Get:"
					 + " name=\"" + name + "\""
					 + ",lb=" + Const.TOOL
					 + ",ub=" + Const.TOOL;

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				dbTool = new DbTool( id );
		}

		return dbTool;
	}

	/**
	 * Gets the Ith tool entity from the modeler.
	 * @return (null) indicates the tool was not found
	 */
	static public DbTool ToolGet( int indx )
	{
		int id = ModelGet( indx, Const.TOOL );

		return ((id == 0) ? null : new DbTool( id ));
	}

	/**
	 * Gets the tool entity having the given station number from the modeler.
	 * @return (null) indicates the tool was not found
	 */
	static public DbTool ToolByStation( int stationNumber )
	{
		DbTool dbTool = null;

		String cmd = "Model:ToolGet:" + "station=" + stationNumber;

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				dbTool = new DbTool( id );
		}

		return dbTool;
	}

	/**
	 * Gets the tool entity having the given station number from the modeler.
	 * @return (null) indicates the tool was not found
	 */
	static public DbTool ToolByCribID( int cribID )
	{
		DbTool dbTool = null;

		String cmd = "Model:ToolGet:" + "CribID=" + cribID;

		if ( Portal.Execute( cmd ) )
		{
			int id = Portal.IntGet( "id" );
			if (id > 0)
				dbTool = new DbTool( id );
		}

		return dbTool;
	}

	/**
	 * Gets the named workplane entity from the modeler.
	 * @return (null) indicates the named workplane was not found
	 */
	static public DbWorkplane WorkplaneGet( String name )
	{
		int id = ModelGet( name, Const.WORKPLANE, Const.WORKPLANE );

		return ((id == 0) ? null : new DbWorkplane( id ));
	}

	/**
	 * Gets the Ith workplane entity from the modeler.
	 * @return (null) indicates the workplane was not found
	 */
	static public DbWorkplane WorkplaneGet( int indx )
	{
		int id = ModelGet( indx, Const.WORKPLANE );

		return ((id == 0) ? null : new DbWorkplane( id ));
	}

	/**
	 * Gets the named point entity from the modeler.
	 * @return (null) indicates the named point was not found
	 */
	static public DbPoint PointGet( String name )
	{
		int id = ModelGet( name, Const.POINT, Const.POINT );

		return ((id == 0) ? null : new DbPoint( id ));
	}

	/**
	 * Gets the Ith point entity from the modeler.
	 * @return (null) indicates the point was not found
	 */
	static public DbPoint PointGet( int indx )
	{
		int id = ModelGet( indx, Const.POINT );

		return ((id == 0) ? null : new DbPoint( id ));
	}

	/**
	 * Gets the named line entity from the modeler.
	 * @return (null) indicates the named line was not found
	 */
	static public DbLine LineGet( String name )
	{
		int id = ModelGet( name, Const.LINE, Const.LINE );

		return ((id == 0) ? null : new DbLine( id ));
	}

	/**
	 * Gets the Ith line entity from the modeler.
	 * @return (null) indicates the line was not found
	 */
	static public DbLine LineGet( int indx )
	{
		int id = ModelGet( indx, Const.LINE );

		return ((id == 0) ? null : new DbLine( id ));
	}

	/**
	 * Gets the named arc entity from the modeler.
	 * @return (null) indicates the named arc was not found
	 */
	static public DbArc ArcGet( String name )
	{
		int id = ModelGet( name, Const.ARC, Const.ARC );

		return ((id == 0) ? null : new DbArc( id ));
	}

	/**
	 * Gets the Ith arc entity from the modeler.
	 * @return (null) indicates the arc was not found
	 */
	static public DbArc ArcGet( int indx )
	{
		int id = ModelGet( indx, Const.ARC );

		return ((id == 0) ? null : new DbArc( id ));
	}

	/**
	 * Gets the named hole entity from the modeler.
	 * @return (null) indicates the named hole was not found
	 */
	static public DbHole HoleGet( String name )
	{
		int id = ModelGet( name, Const.HOLE, Const.HOLE );

		return ((id == 0) ? null : new DbHole( id ));
	}

	/**
	 * Gets the Ith hole entity from the modeler.
	 * @return (null) indicates the hole was not found
	 */
	static public DbHole HoleGet( int indx )
	{
		int id = ModelGet( indx, Const.HOLE );

		return ((id == 0) ? null : new DbHole( id ));
	}

	/**
	 * Gets the named profile entity from the modeler.
	 * @return (null) indicates the named profile was not found
	 */
	static public DbProfile ProfileGet( String name )
	{
		int id = ModelGet( name, Const.PROFILE, Const.PROFILE );

		return ((id == 0) ? null : new DbProfile( id ));
	}

	/**
	 * Gets the Ith profile entity from the modeler.
	 * @return (null) indicates the profile was not found
	 */
	static public DbProfile ProfileGet( int indx )
	{
		int id = ModelGet( indx, Const.PROFILE );

		return ((id == 0) ? null : new DbProfile( id ));
	}

	/**
	 * Gets the named feature entity from the modeler.
	 * @return (null) indicates the named feature was not found
	 */
	static public DbFeature FeatureGet( String name )
	{
		int id = ModelGet( name, Const.FEATURE, Const.FEATURE );

		return ((id == 0) ? null : new DbFeature( id ));
	}

	/**
	 * Gets the Ith feature entity from the modeler.
	 * @return (null) indicates the feature was not found
	 */
	static public DbFeature FeatureGet( int indx )
	{
		int id = ModelGet( indx, Const.FEATURE );

		return ((id == 0) ? null : new DbFeature( id ));
	}

/*
	**
	 * Gets the named pattern entity from the modeler.
	 * @return (null) indicates the named pattern was not found
	 *
	static public DbPattern PatternGet( String name )
	{
		int id = ModelGet( name, Const.PATTERN, Const.PATTERN );

		return ((id == 0) ? null : new DbPattern( id ));
	}
*/

	/**
	 * Gets the Ith pattern entity from the modeler.
	 * @return (null) indicates the pattern was not found
	 */
	static public DbPattern PatternGet( int indx )
	{
		int id = ModelGet( indx, Const.PATTERN );

		return ((id == 0) ? null : new DbPattern( id ));
	}

	/**
	 * Gets the named command entity from the modeler.
	 * @return (null) indicates the named line was not found
	 */
	static public DbCommand CommandGet( String name )
	{
		int id = ModelGet( name, Const.COMMAND, Const.COMMAND );

		return ((id == 0) ? null : new DbCommand( id ));
	}

	/**
	 * Gets the Ith command entity from the modeler.
	 * @return (null) indicates the line was not found
	 */
	static public DbCommand CommandGet( int indx )
	{
		int id = ModelGet( indx, Const.COMMAND );

		return ((id == 0) ? null : new DbCommand( id ));
	}

	/**
	 * Gets the named sequence entity from the modeler.
	 * @return (null) indicates the named line was not found
	 */
	static public DbSequence SequenceGet( String name )
	{
		int id = ModelGet( name, Const.SEQUENCE, Const.SEQUENCE );

		return ((id == 0) ? null : new DbSequence( id ));
	}

	/**
	 * Gets the Ith sequence entity from the modeler.
	 * @return (null) indicates the line was not found
	 */
	static public DbSequence SequenceGet( int indx )
	{
		int id = ModelGet( indx, Const.SEQUENCE );

		return ((id == 0) ? null : new DbSequence( id ));
	}

	/**
	 * Determines whether the current model has a right-handed coordinate system.
	 */
	static public boolean IsRightHanded()
	{
		boolean status = true;

		if ( Portal.Execute( "Model:IsRightHanded:" ) )
			status = (Portal.IntGet( "bool" ) != 0);

		return status;
	}

	/**
	 * Gets the 2d bounding box of the model.
	 * @return (null) indicates failure.
	 */
	static public Box2d Box2d()
	{
		Box2d box = null;

		if ( Portal.Execute( "Model:PartExtent:" ) )
		{
			box = new Box2d(
						Portal.DoubleGet( "xmin" ),
						Portal.DoubleGet( "ymin" ),
						Portal.DoubleGet( "xmax" ),
						Portal.DoubleGet( "ymax" ) );
		}

		return box;
	}

	/**
	 * Gets the named-attribute from the modelers header as an integer value.
	 * @return (Const.UNDEFINED) indicates the named-atribute was not found
	 */
	static public int IntGet( String attribName )
	{
		int result = (int) Const.UNDEFINED;

		String cmd = "*Attrib:HeadGet: name=\"#" + attribName + "\"";
		if ( Portal.Execute( cmd ) )
		{
			result = Portal.IntGet( "val" );
		}

		return result;
	}

	/**
	 * Gets the named-attribute from the modelers header as a double value.
	 * @return (Const.UNDEFINED) indicates the named-atribute was not found
	 */
	static public double DoubleGet( String attribName )
	{
		double result = Const.UNDEFINED;

		String cmd = "*Attrib:HeadGet: name=\"" + attribName + "\"";
		if ( Portal.Execute( cmd ) )
		{
			result = Portal.DoubleGet( "val" );
		}

		return result;
	}

	/**
	 * Gets the named-attribute from the modelers header as a string value.
	 * @return (null) indicates the named-atribute was not found
	 */
	static public String StringGet( String attribName )
	{
		String result = null;

		String cmd = "*Attrib:HeadGet: name=\"$" + attribName + "\"";
		if ( Portal.Execute( cmd ) )
		{
			result = Portal.StringGet( "val" );
		}

		return result;
	}

	/**
	 * Sets the integer value named-attribute in the modelers header.
	 */
	static public boolean IntSet( String attribName, int value )
	{
		String cmd = "Attrib:HeadSet:"
					 + " name=\"#" + attribName + "\""
					 + ",val=" + value;

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Sets the double value named-attribute in the modelers header.
	 */
	static public boolean DoubleSet( String attribName, double value )
	{
		String cmd = "Attrib:HeadSet:"
					 + " name=\"" + attribName + "\""
					 + ",val=" + value;

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Sets the string value named-attribute in the modelers header.
	 */
	static public boolean StringSet( String attribName, String value )
	{
		String cmd = "Attrib:HeadSet:"
					 + " name=\"$" + attribName + "\""
					 + ",val=\"" + value + "\"";

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Deletes the named-attribute from the modelers header.
	 */
	static public boolean VarDel( String attribName )
	{
		String cmd = "Attrib:HeadDel:name=\"$" + attribName + "\"";

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Gets the named-attribute from the modelers Defer as an integer value.
	 * @return (Const.UNDEFINED) indicates the named-atribute was not found
	 */
	static public int DefIntGet( String attribName )
	{
		int result = (int) Const.UNDEFINED;

		String cmd = "Attrib:DefGet: name=\"#" + attribName + "\"";
		if ( Portal.Execute( cmd ) )
		{
			result = Portal.IntGet( "val" );
		}

		return result;
	}

	/**
	 * Gets the named-attribute from the modelers Defer as a double value.
	 * @return (Const.UNDEFINED) indicates the named-atribute was not found
	 */
	static public double DefDoubleGet( String attribName )
	{
		double result = Const.UNDEFINED;

		String cmd = "Attrib:DefGet: name=\"" + attribName + "\"";
		if ( Portal.Execute( cmd ) )
		{
			result = Portal.DoubleGet( "val" );
		}

		return result;
	}

	/**
	 * Gets the named-attribute from the modelers Defer as a string value.
	 * @return (null) indicates the named-atribute was not found
	 */
	static public String DefStringGet( String attribName )
	{
		String result = null;

		String cmd = "Attrib:DefGet: name=\"$" + attribName + "\"";
		if ( Portal.Execute( cmd ) )
		{
			result = Portal.StringGet( "val" );
		}

		return result;
	}

	/**
	 * Sets the integer value named-attribute in the modelers Defer.
	 */
	static public boolean DefIntSet( String attribName, int value )
	{
		String cmd = "Attrib:DefSet:"
					 + " name=\"#" + attribName + "\""
					 + ",val=" + value;

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Sets the double value named-attribute in the modelers Defer.
	 */
	static public boolean DefDoubleSet( String attribName, double value )
	{
		String cmd = "Attrib:DefSet:"
					 + " name=\"" + attribName + "\""
					 + ",val=" + value;

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Sets the string value named-attribute in the modelers Defer.
	 */
	static public boolean DefStringSet( String attribName, String value )
	{
		String cmd = "Attrib:DefSet:"
					 + " name=\"$" + attribName + "\""
					 + ",val=\"" + value + "\"";

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Deletes the named default attribute.
	 */
	static public boolean DefDelete( String attribName )
	{
		String cmd = "Attrib:DefDel:name=\"$" + attribName + "\"";

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Loads variables from the named registry key into the model header.
	 * For example, to load code generator variables:<br>
	 * <dd>Model.VarsLoad( "Software\\WE-CIM\\V20.0\\CodeGeneration", "cg" );
	 */
	static public boolean VarsLoad( String regkey, String prefix )
	{
		String cmd = "Model:VarsLoad:"
					 + " key=\"" + regkey + "\""
					 + ",prefix=\"" + prefix + "\"";

		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Regenerates 'dirty' toolpath entities.
	 */
	static public boolean Regen()
	{
		return ( Portal.Execute( "Admin:Regen:" ) );
	}

	/**
	 * Creates an HTML document representing the currrent state of the model.
	 */
	static public boolean Dump( String filePath )
	{
		String cmd = "File:ModelDump: file=\"" + filePath + "\", type=1";
		return ( Portal.Execute( cmd ) );
	}

	/**
	 * Restricted to RTL application.
	 */
	static public void DatabasePathOveride( String path )
	{
		m_dbPath = path;
	}

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// Private methods.
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	static private int ModelGet( String name, int lowerBound, int upperBound )
	{
		String cmd = "Model:Get:"
					 + " name=\"" + name + "\""
					 + ",lb=" + lowerBound
					 + ",ub=" + upperBound;

		boolean okay = Portal.Execute( cmd );

		return (( okay ) ? Portal.IntGet( "id" ) : 0);
	}

	static private int ModelGet( int indx, int entityType )
	{
		String cmd = "Model:Get2:"
					 + " indx=" + indx
					 + ",type=" + entityType;

		boolean okay = Portal.Execute( cmd );

		return (( okay ) ? Portal.IntGet( "id" ) : 0);
	}

	static private boolean ModelInit( int toolSetupId )
	{
		if (toolSetupId < 1)
			return false;

		String cmd = "Model:Init: toolsetupid=" + toolSetupId;

		return ( Portal.Execute( cmd ) );
	}

	static private String DatabasePath()
	{
		if (m_dbPath.length() <= 0)
		{
			m_dbPath = Registry.StringGet(
				Registry.REGROOT + "\\ConfigurationManager", "Database" );
		}

		return m_dbPath;
	}

	private Model()
	{
	}

	static private String m_dbPath = "";
}

