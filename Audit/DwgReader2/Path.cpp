
#include "stdafx.h"
#include "cmn_resource.h"
#include "Return.h"
#include "Register.h"
#include "Path.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPath::CPath()
{
}

CPath::CPath( const char* fullyQualifiedFilePath )
{
	Set( fullyQualifiedFilePath );
}

CPath::~CPath()
{
}

void
CPath::Set( const char* fullyQualifiedFilePath )
{
	int len = strlen( fullyQualifiedFilePath );
	if (len < 1)
		return;

	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	// ASSUMPTION: All filenames will have an extension.
	// If we find a filename but no extension, then we will
	// assume we were given a directory path of the form
	//    c:\Bbi\AdvMach\Debug
	// as opposed to
	//    c:\Bbi\AdvMach\Debug\
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	char* tmp = (char*) calloc( sizeof(char), len+2 );
	strcpy( tmp, fullyQualifiedFilePath );

	_splitpath( tmp, m_drive, m_dir, m_fname, m_ext );
	if (strlen(m_fname) > 0 && strlen(m_ext) <= 0)
	{
		tmp[len] = '\\';
		_splitpath( tmp, m_drive, m_dir, m_fname, m_ext );
	}

	m_path = tmp;
	free( tmp );
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	// Strip the trailing '\'
	len = strlen( m_dir );
	if (len> 0 && m_dir[--len] == '\\')
		m_dir[len] = '\0';

	// Strip the leading '.'
	len = strlen( m_ext );
	if (len > 0)
		strcpy( m_ext, &m_ext[1] );
}

CString
CPath::operator = ( const CPath& path ) const
{
	return path.m_path;
}

int
CPath::operator == ( const CPath& path ) const
{
// For path operations, case is irrelevant?  eww
//	return ( !m_path.Compare( path.m_path ) );
	return ( !m_path.CompareNoCase( path.m_path ) );
}

int
CPath::operator != ( const CPath& path ) const
{
// For path operations, case is irrelevant?  eww
//	return ( m_path.Compare( path.m_path ) );
	return ( m_path.CompareNoCase( path.m_path ) );
}

CString
CPath::Drive() const
{
	return m_drive;
}

CString
CPath::Dir() const
{
	return m_dir;
}

CString
CPath::DriveDir() const
{
	CString tmp;

	tmp.Format( "%s%s", m_drive, m_dir );

	return tmp;
}

CString
CPath::FileName() const
{
	return m_fname;
}

CString
CPath::Ext() const
{
	return m_ext;
}

CString
CPath::FileNameExt() const
{
	CString tmp;

	tmp.Format( "%s.%s", m_fname, m_ext );

	return tmp;
}

CString
CPath::DriveDirFileName() const
{
	CString tmp;

	tmp.Format( "%s%s\\%s", m_drive, m_dir, m_fname );

	return tmp;
}

void
CPath::ConditionalExt( CString* fpath, const char* ext )
{
	CPath path( *fpath );
	CString theExt = path.Ext();

	if ( theExt.CompareNoCase( ext ) )
	{
		(*fpath) += ".";
		(*fpath) += ext;
	}
}

// CRITICAL: IsNewer() was introduced to minimized java
// file recompilations.  By default, we want to recompile
// if IsNewer() fails.  No harm done, just adds overhead.
//
// NOTE: Used only by CCodeGenProcessApp::Generate()
BOOL
CPath::IsNewer( const CString& javaPath, const CString& classPath )
{
	CFileStatus javaStatus;
	CFileStatus classStatus;

	if (CFile::GetStatus( javaPath, javaStatus ) == 0)
	{
		// We got real problems!
		CReturn status;
		status.Internal( IDS_INTERNAL_ERROR, "CPath::IsNewer()" );
		return TRUE;
	}

	if (CFile::GetStatus( classPath, classStatus ) == 0)
	{
		// Hasn't ever been compiled, or has been deleted.
		return TRUE;
	}
	
	// Force recompile when java file is newer than class file
	// (ie. the java file was modified).
	return (javaStatus.m_mtime > classStatus.m_mtime);
}

int
CPath::Exists() const
{
	FILE* f = fopen( m_path, "r" );

	if (f != NULL)
	{
		fclose( f );
		return TRUE;
	}

	return FALSE;
}

CString
CPath::FileNameExpand( const CString& fname )
{
	CString expanded = fname;

	if (expanded.Left(10).CompareNoCase( "$RTLSOURCE" ) == 0)
	{
		CString	srcRootDir = CRegister::StringGet( "Software\\CIMBlock\\RTL", "$RTLSOURCE" );
		expanded = srcRootDir + expanded.Mid( 10 );
	}
	else if (expanded.Left(11).CompareNoCase( "$RTLRESULTS" ) == 0)
	{
		CString	rsltRootDir = CRegister::StringGet( "Software\\CIMBlock\\RTL", "$RTLRESULTS" );
		expanded = rsltRootDir + expanded.Mid( 11 );
	}

	return expanded;
}
