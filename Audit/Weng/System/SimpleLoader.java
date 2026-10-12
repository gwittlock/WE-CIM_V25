
package Weng.System;

import java.util.Hashtable;
import java.lang.ClassLoader;
import java.io.ByteArrayInputStream;
import java.io.FileInputStream;

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// DO NOT provide JavaDoc type comments because we
// do not want to publically document this class.
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/**
 * Do <b>not</b> attempt to use this class.
 */
public class SimpleLoader extends ClassLoader
{
    private Hashtable classes = new Hashtable();
	private String m_storagePath = new String();

    public SimpleLoader( String storagePath )
    {
		m_storagePath = storagePath;
    }

    // This is a simple version for external clients since they
    // will always want the class resolved before it is returned
    // to them.
    
    public Class loadClass( String className ) throws ClassNotFoundException
    {
        return ( loadClass( className, true ) );
    }

    // This is the required version of loadClass which is called
    // both from loadClass above and from the internal function
    // FindClassFromClass.
    
    public synchronized Class loadClass( String className, boolean resolveIt )
        throws ClassNotFoundException
    {
        Class result;
        byte classData[];

        // // TODO: Msg.Debug("        >>>>>> Load class : "+className);

        // Check our local cache of classes
        // result = (Class)classes.get(className);
        // if (result != null)
        // {
				// TODO: Msg.Debug("        >>>>>>>> returning cached result.");
        //     resolveClass(result);
        //     return result;
        // }

        // Check with the primordial class loader
        try
        {
            result = super.findSystemClass(className);
            // TODO: Msg.Debug("        >>>>>>>> returning system class (in CLASSPATH).");
			// System.out.println("        >>>>>>>> returning system class (in CLASSPATH).");
			return result;
        }
        catch (ClassNotFoundException e)
        {
            // TODO: Msg.Debug("        >>>>>>>> Not a system class.");
			// System.out.println("        >>>>>>>> Not a system class.");
        }

        // Try to load it from our repository
        classData = Fetch(className);
        if (classData == null)
        {
            // TODO: Msg.Debug("        >>>>>>>> Fetch() throws ClassNotFoundException.");
			// System.out.println("        >>>>>>>> Fetch() throws ClassNotFoundException.");
            throw new ClassNotFoundException();
        }

        // Define it (parse the class file)
        result = defineClass( className, classData, 0, classData.length );
        if (result == null)
        {
            // TODO: Msg.Debug("        >>>>>>>> defineClass() throws ClassFormatError.");
			// System.out.println("        >>>>>>>> defineClass() throws ClassFormatError.");
            throw new ClassFormatError();
        }

        if (resolveIt)
        {
            // TODO: Msg.Debug("        >>>>>>>>>> Resolving class "+className);
			// System.out.println("        >>>>>>>>>> Resolving class "+className);
            resolveClass( result );
        }

        // classes.put(className, result);

        return result;
    }

    // This sample function for reading class implementations reads
    // them from the local file system
    
    private byte Fetch( String className )[]
    {
        // TODO: Msg.Debug("        >>>>>>>>>> Fetching the implementation of "+className);
        byte result[];
        try
        {
            FileInputStream fi = new FileInputStream( m_storagePath+"\\"+className+".class");

            result = new byte[fi.available()];
            fi.read(result);

            return result;
        }
        catch (Exception e)
        {
            // If we caught an exception, either the class wasn't
            // found or it was unreadable by our process.

            // TODO: Msg.Debug( "        >>>>>>>>>> Exception in Fetch() -- can't find class " + className );
			// System.out.println( "        >>>>>>>>>> Exception in Fetch() -- can't find class " + className );
            return null;
        }
    }
}
