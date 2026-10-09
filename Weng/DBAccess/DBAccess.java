
import Weng.System.*;
import Weng.Modeler.*;
import Weng.Math.*;
import Weng.Geometry.*;
import Weng.CodeGen.*;
import Weng.Access.*;
import java.io.*;
import java.util.*;
import java.sql.*;
import java.sql.SQLException;
import java.sql.Statement;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// MSaccess_archive.java
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
public class DBAccess implements WengMacro
{
	private final boolean DEBUG = false; // Set to "true" to test!

	public void main()
	{
		try
		{
			if (DEBUG) Msg.Display( "MSaccess_archive.java is running" );


				SetDBFile( "C:\\1wittlockeng\\Customers\\Greg King\\BS\\TruMatic6000.mdb");
				if (DEBUG) Msg.Display("WE set the cmdb");

				ConnectDB(sMDBPath);
//				//************************************************************************
//				 	        if (DEBUG) Msg.Display("Begining conn");
//	        Class.forName("sun.jdbc.odbc.JdbcOdbcDriver");
//	        String accessFileName = "C:\\1wittlockeng\\Customers\\Greg King\\BS\\TruMatic6000.mdb";
//	        String connURL = "jdbc:odbc:Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=" + accessFileName + ";PWD=";
//
//	        Msg.Display("Connecting");
//	        conn = DriverManager.getConnection(connURL, "", "");
//	        Msg.Display("Finished Connecting");
//
////				//***************************************************************************

				statement = conn.createStatement();
           		if (DEBUG) Msg.Display("Did we create the statement");

				myQuery = "Select ToolType.* From ToolType Where (ToolType.ToolType = 11 )";

           		if (DEBUG) Msg.Display("trying select");
		        boolean foundResults = statement.execute(myQuery);

		        //statement.execute(myQuery);

		        if (DEBUG) Msg.Display("finished select");

		        if(foundResults)
		        {
		            ResultSet set = statement.getResultSet();

		            if (DEBUG) Msg.Display("10");

		            if(set!=null) displayResults(set);
		        }
		        else
		        {
		            //conn.close();
		            if (DEBUG) Msg.Display("No records found for query");
		        }
           		if (DEBUG) Msg.Display("Statement Succesful!");

           		CreateTable( "Gary", "( Name string, ID INT PRIMARY KEY )");
           		if (DEBUG) Msg.Display("CreateTable Succesful!");

           		CreateField("Gary", "Greg DOUBLE");
           		if (DEBUG) Msg.Display("CreateField Succesful!");


			if (DEBUG) Msg.Display("Finished");

		}

		catch( Exception e )
		{
			ExceptionPrinter.StackTracePrint( e );
		}

		DisconnectDB();

	}

	public void ConnectDB(String Gary)
	{

        try {

			if (DEBUG) Msg.Display("Begining conn");
	        Class.forName("sun.jdbc.odbc.JdbcOdbcDriver");
	        String accessFileName = "C:\\1wittlockeng\\Customers\\Greg King\\BS\\TruMatic6000.mdb";
	        String connURL = "jdbc:odbc:Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=" + accessFileName + ";PWD=";

	        if (DEBUG) Msg.Display("Connecting");
	        conn = DriverManager.getConnection(connURL, "", "");
	        if (DEBUG) Msg.Display("Finished Connecting");

//  	        if (DEBUG) Msg.Display("Begining conn");
//	        Class.forName("sun.jdbc.odbc.JdbcOdbcDriver");
//	        String accessFileName = "C:\\1wittlockeng\\Customers\\Greg King\\BS\\TruMatic6000.mdb";
//	        String connURL = "jdbc:odbc:Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=" + accessFileName + ";PWD=";
//
//	        if (DEBUG) Msg.Display("Connecting");
//	        conn = DriverManager.getConnection(connURL, "", "");
//	        if (DEBUG) Msg.Display("Finished Connecting");


//
            //stmt.execute("select * from student"); // execute query in table student

            //ResultSet rs = stmt.getResultSet(); // get any Result that came from our query

            //if (rs != null)
             //while ( rs.next() ){

             //   System.out.println("Name: " + rs.getString("Name") + " ID: "+rs.getString("ID"));
             //   }

             //   stmt.close();
              //  con.close();
            }
            catch (Exception err) {
                System.out.println("ERROR: " + err);
            }
    }

	private void DisconnectDB()
	{
		try
		{

			db.Close();
			AccessDb.Destroy( db );

			conn.close();
			dbIsOpen=false;
		}

		catch( Exception e )
		{
			ExceptionPrinter.StackTracePrint( e );
		}


	}

	public void SetDBFile(String sDatabase)
	{
		sMDBPath= sDatabase;
	}

	public  void buildStatement() throws SQLException
	{
        statement = conn.createStatement();
    }

    public  void SelectQuery(String myQuery) throws SQLException
    {
    		int nRowCOunt;

	        boolean foundResults = statement.execute(myQuery);
	        if(foundResults)
	        {
	            ResultSet set = statement.getResultSet();

	            if (DEBUG) Msg.Display("10");

	            if(set!=null) displayResults(set);
	        }
	        else
	        {
	            conn.close();
	            if (DEBUG) Msg.Display("No records found for query");
	        }

	         conn.close();
    }


//IE: ("create table student ( Name string, ID integer )"); // create a student
//IE: "( Name string, ID integer )"
    public void CreateTable(String sTableName, String sNameValuePair )
    {
    	try
    	{
    		statement.executeUpdate("create table " + sTableName +sNameValuePair); // create a student
    	}
    	catch ( Exception e )
		{
			ExceptionPrinter.StackTracePrint( e );
		}

    }


    public int ColumnCount()
    {
       	try
    	{
     //if (DEBUG) Msg.Display("9");
        ResultSetMetaData metaData = rs.getMetaData();
       //if (DEBUG) Msg.Display("10");
        int columns=metaData.getColumnCount();
        String text="";
        int ni = 0;

        while(rs.next())
        	{
        		ni++;
    		}

    		if (DEBUG) Msg.Display("The Column count of ni is < " + ni + " >");
    	}

    	catch( Exception e )
		{
			ExceptionPrinter.StackTracePrint( e );
		}
		return nRowCount;
    }

    public int RowCount()
    {
    	int RowCount=0;

       	try
    	{

        ResultSetMetaData metaData = rs.getMetaData();

        int ni = 0;

        while(rs.next())
        	{
        		ni++;
    		}

    		RowCount=ni;

    		if (DEBUG) Msg.Display("The Row count of ni is < " + ni + " >");

    	}

    	catch( Exception e )
		{
			ExceptionPrinter.StackTracePrint( e );
		}

			return RowCount;
    }



	public void UpdateFieldValue(String sTableName, String sFieldName,String sNewValue)
	{
		try
		{
			String sql = "UPDATE " + sTableName +  " SET " + sFieldName + " = " + sNewValue ;
         	statement.executeUpdate(sql);
		}

  		catch( Exception e )
		{
			ExceptionPrinter.StackTracePrint( e );
		}
	}

    public void DeleteTable(String sTableName)
    {
       	try
    	{
			String sql = "DROP TABLE REGISTRATION";
         	statement.executeUpdate(sql);
    	}

    	catch( Exception e )
		{
			ExceptionPrinter.StackTracePrint( e );
		}

    }

    public void CreateField(String sTableName,String sNameValuePair)
    {
       	try
    	{
	  		//Query to alter the table
	      	String query = "ALTER TABLE " + sTableName + " ADD " + sNameValuePair;
	      	//Executing the query
	      	statement.executeUpdate(query);
    	}

    	catch( Exception e )
		{
			ExceptionPrinter.StackTracePrint( e );
		}

    }

    public void DeleteField(String sTableName, String sFieldName)
    {
       	try
    	{
 			String query = "ALTER TABLE " + sTableName + " Drop " + sFieldName;

            // Step 5: Execute the query

            // executeUpdate() returns number of rows
            // affected by the execution of the statement
            int result = statement.executeUpdate(query);

    	}

    	catch( Exception e )
		{
			ExceptionPrinter.StackTracePrint( e );
		}

    }


    public  void displayResults(ResultSet rs) throws SQLException
	{
       //if (DEBUG) Msg.Display("9");
        ResultSetMetaData metaData = rs.getMetaData();
       //if (DEBUG) Msg.Display("10");
        int columns=metaData.getColumnCount();
        String text="";
        int ni = 0;

        while(rs.next())
        	{
        		ni++;
//        		 text+=""+metaData.getColumnName(1)+":\t";
//		                text+=rs.getString(1);
//		                text+="</"+metaData.getColumnName(i)+">";
//		                text+="\n";
//		                //if (DEBUG) Msg.Display("The text is < "+ text + " >");
//	        	if (DEBUG) Msg.Display("11");
//	            for(int i=1;i<=columns;++i)
//	            	{
//		            	if (DEBUG) Msg.Display("12");
//		                text+=""+metaData.getColumnName(i)+":\t";
//		                text+=rs.getString(i);
//		                text+="</"+metaData.getColumnName(i)+">";
//		                text+="\n";
//		                if (DEBUG) Msg.Display("The text is < "+ text + " >");
//            		}
//
//        text+="\n";
    		}

    		if (DEBUG) Msg.Display("The Column count of ni is < " + ni + " >");


	}

	String stmt = null;
	boolean dbIsOpen = false;
	AccessDb	db = AccessDb.Create();
	String sMDBPath = null;
    Connection conn = null;
    ResultSet rs;
    Statement statement = null;
    int nRowCount = 0 ;
    int nColCOunt = 0;
    String myQuery = null;

}
