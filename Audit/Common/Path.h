
#ifndef _PATH_H
#define _PATH_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"


class dllExport CPath  
{
public:

	CPath();

	// Give a path like "c:\root\subdir\foo.bar"
	CPath( const char* fullyQualifiedFilePath );

	virtual ~CPath();

	// Give a path like "c:\root\subdir\foo.bar"
	void Set( const char* fullyQualifiedFilePath );

	// Returns the fully qualified file path.
	CString operator = ( const CPath& path ) const;

	int operator == ( const CPath& path ) const;

	int operator != ( const CPath& path ) const;

	// Returns "c:" for example.
	CString Drive() const;

	// Returns "\root\subdir" for example.
	CString Dir() const;

	// Returns "c:\root\subdir" for example.
	CString DriveDir() const;

	// Returns "foo" for example.
	CString FileName() const;

	// Returns "bar" for example.
	CString Ext() const;

	// Returns "foo.bar" for example.
	CString FileNameExt() const;

	// Returns "c:\root\subdir\foo" for example.
	CString DriveDirFileName() const;

	CString FullPath() const;

	// eg. ForceExt( "log" )
	void ForceExt( const char* ext );

	bool Exists() const;

public:

	// Add an extension if one does not exist.
	// Call as ConditionalExt( &myPath, "foo" ) and
	// the extension ".foo" will be appended to myString.
	static void ConditionalExt( CString* fpath, const char* ext );

	// Returns TRUE if thisPath is newer than thatPath.
	// NOTE: Used only by CCodeGenProcessApp::Generate()
	static bool IsNewer( const CString& javaPath, const CString& classPath );

	// In support of RTLs.
	static CString PathExpand( const CString& entry, const CString& orig_path );

	static CString DebugDir();

private:

	// Disable methods.
	CPath( const CPath& );

	char m_drive[_MAX_DRIVE];
	char m_dir[_MAX_DIR];
	char m_fname[_MAX_FNAME];
	char m_ext[_MAX_EXT];

	CString m_path;
};

#endif
