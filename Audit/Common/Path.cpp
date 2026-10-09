
// To suppress the complaint about _splitpath().
#pragma warning(disable : 4996)

#include "stdafx.h"
#include "cmn_resource.h"
#include "Return.h"
#include "Register.h"
#include "Path.h"

#include "portable.h"

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

// Because I've run into cases where _splitpath() returns garbage :-/
static void fixit( char* sval )
{
	if (*sval < 0)
		(*sval) = '\0';
}

void CPath::Set( const char* fullyQualifiedFilePath )
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
	strcpy_s( tmp, (len+2), fullyQualifiedFilePath );

	_splitpath( tmp, m_drive, m_dir, m_fname, m_ext );

	fixit( m_drive );
	fixit( m_dir );
	fixit( m_fname );
	fixit( m_ext );

	// 2017.02.01 (PE) -- Gary said nesting failed with result paths like:
	//    <C:\_gary\2017.02.01\.125 AL. Sht>
	// which is true because _splitpath() decided <. Sht> is an extension.
	//
	//    if (strlen(m_fname) > 0 && strlen(m_ext) <= 0)
	if ((strlen(m_fname) > 0) && ((strlen(m_ext) <= 0)) || (strchr(m_ext, (int) ' ') != nullptr))
	{
		tmp[len] = '\\';
		_splitpath( tmp, m_drive, m_dir, m_fname, m_ext );

		fixit( m_drive );
		fixit( m_dir );
		fixit( m_fname );
		fixit( m_ext );
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

CString CPath::operator = ( const CPath& path ) const
	{ return path.m_path; }

int CPath::operator == ( const CPath& path ) const
{
// For path operations, case is irrelevant?  eww
//	return ( !m_path.Compare( path.m_path ) );
	return ( !m_path.CompareNoCase( path.m_path ) );
}

int CPath::operator != ( const CPath& path ) const
{
// For path operations, case is irrelevant?  eww
//	return ( m_path.Compare( path.m_path ) );
	return ( m_path.CompareNoCase( path.m_path ) );
}

CString CPath::Drive() const
	{ return m_drive; }

CString CPath::Dir() const
	{ return m_dir; }

CString CPath::DriveDir() const
{
	CString tmp;

	tmp.Format( "%s%s", m_drive, m_dir );

	return tmp;
}

CString CPath::FileName() const
	{ return m_fname; }

CString CPath::Ext() const
	{ return m_ext; }

CString CPath::FileNameExt() const
{
	CString tmp;

	tmp.Format( "%s.%s", m_fname, m_ext );

	return tmp;
}

CString CPath::DriveDirFileName() const
{
	CString tmp;

	tmp.Format( "%s%s\\%s", m_drive, m_dir, m_fname );

	return tmp;
}

CString CPath::FullPath() const
{
	CString tmp;

	if (*m_ext != 0)
		tmp.Format( "%s%s\\%s.%s", m_drive, m_dir, m_fname, m_ext );
	else
		tmp = DriveDirFileName();

	return tmp;
}

void CPath::ConditionalExt( CString* fpath, const char* ext )
{
	CPath path( *fpath );
	CString theExt = path.Ext();

	if ( theExt.CompareNoCase( ext ) )
	{
		(*fpath) += ".";
		(*fpath) += ext;
	}
}

void CPath::ForceExt( const char* ext )
{
	strcpy( m_ext, ext );
}

// CRITICAL: IsNewer() was introduced to minimized java
// file recompilations.  By default, we want to recompile
// if IsNewer() fails.  No harm done, just adds overhead.
//
// NOTE: Used only by CCodeGenProcessApp::Generate()
bool CPath::IsNewer( const CString& javaPath, const CString& classPath )
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
	return ((javaStatus.m_mtime > classStatus.m_mtime) ? true : false);
}

bool CPath::Exists() const
{
	FILE* f = fopen( m_path, "r" );

	bool exists = (f != NULL);
	if ( exists )
		fclose( f );

	return exists;
}

// Pilfered from CVm::JVMInit().
#define MAXPATHLEN 256
CString CPath::DebugDir()
{
	CString	folder;
	char	buf[MAXPATHLEN];

    GetModuleFileName( 0, buf, (MAXPATHLEN-1) );

	// Remove the exe file name (creepy ... but straight from Sun).
    *strrchr(buf, '\\') = '\0';

	folder = CString( buf ) + "\\debug";

    return folder;
}

CString CPath::PathExpand( const CString& entry, const CString& orig_path )
{
	CString	fq_path;
	CPath	path;

	CString sval = CRegister::StringGetV( "Debug", entry, "" );

	if (sval.GetLength() > 0)
	{
		path.Set( orig_path );
		fq_path = sval + "\\" + path.FileNameExt();
	}
	else
	{
		fq_path = orig_path;
	}

	return fq_path;
}
