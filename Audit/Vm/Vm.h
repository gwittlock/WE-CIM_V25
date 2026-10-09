#pragma once

#include "jni.h"
#include "java.h"

#include "Return.h"


// The Virtual Machine is a singleton.

class dllExport CVm  
{
public:

	/////////////////////////////////////////////////////////////////////////
	// Wrt the virtual machine initialization, the classPath is critcal to
	// java file compilation and execution.  A known working configuration
	// is:
	//
	//     classPath = "c:\\Bbi\\AdvMach\\Debug\\Bbi.zip; \
	//                  c:\\jdk1.2\\lib\\tools.jar; \
	//                  c:\\jdk1.2\\jre\\lib\\rt.jar";
	//
	// The storagePath is the target directory where the compiled (.class)
	// files reside.  A known working configuration is:
	//
	//     storagePath = "c:\\Bbi\\AdvMach\\Store\\.";
	//
	static CReturn Init( const CString& jvmDllFolder, const CString& classPath, const CString& storagePath );

	static bool IsOk();

	// javaClassPath -- the name of the .java file to be compiled
	static CReturn Compile( const CString& javaClassPath );

	// javaClassPath -- the name of the .class file to execute
	static CReturn Execute( const CString& javaClassPath, bool localStoragePath );

	static void Term();

private:

	// Disabled methods.
	CVm();
	virtual ~CVm();
	CVm( const CVm& );
	CVm& operator = ( const CVm& );

private:

	static CReturn JVMInit( const CString& jvmDllFolder );
	static void JVMTerm();
};
