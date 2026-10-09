// ==================================================================
//		File Snoop
//
//	File searching manager, wrapping up the standard MFC file search
//
// ==================================================================

#include "stdafx.h"
#include "FileSnoop.h"

// ==================================================================

CFileSnoop::CFileSnoop()
{
	m_handle = INVALID_HANDLE_VALUE;
}

CFileSnoop::~CFileSnoop()
{
	if (m_handle != INVALID_HANDLE_VALUE)
		FindClose( m_handle );
}


// ==================================================================
//		findAllDir
//
//		Locate all directory names following the given root path and
//		pattern.  Also fills the m_name_array, for later access.
//
const CStringArray*
CFileSnoop::findAllDir( 
	const CString&	in_pattern )
{
	CString	name;

	if (m_handle != INVALID_HANDLE_VALUE)
		FindClose( m_handle );

	m_name_array.RemoveAll();
	m_handle = FindFirstFile( in_pattern, &m_data );
	while(m_handle != INVALID_HANDLE_VALUE)
	{
		if (m_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			name = m_data.cFileName;
			if ( !(name == ".")
				&& !(name == "..") )
			{
				m_name_array.Add( m_data.cFileName );
			}
		}

		if (!FindNextFile( m_handle, &m_data ))
		{
			FindClose( m_handle );
			m_handle = INVALID_HANDLE_VALUE;
		}
	}

	return &m_name_array;
}

// ==================================================================
//		findAllFile
//
//		Locate all file names following the given root path and
//		pattern. Also fills the m_name_array, for later access.
//
const CStringArray*
CFileSnoop::findAllFile( 
	const CString&	in_pattern )
{
	if (m_handle != INVALID_HANDLE_VALUE)
		FindClose( m_handle );

	m_name_array.RemoveAll();
	m_handle = FindFirstFile( in_pattern, &m_data );
	while(m_handle != INVALID_HANDLE_VALUE)
	{
		if (!(m_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			m_name_array.Add( m_data.cFileName );

		if (!FindNextFile( m_handle, &m_data ))
		{
			FindClose( m_handle );
			m_handle = INVALID_HANDLE_VALUE;
		}
	}

	return &m_name_array;
}

// ==================================================================
//		getCurrentDir
//
//		Return the current working directory name.
//
CString
CFileSnoop::getCurrentDir( void )
{
	char	dir[MAX_PATH];

	dir[0] = 0;
	GetCurrentDirectory( sizeof(dir), dir );
	return CString(dir);
}

// ==================================================================
//		getFilename
//
//		Given a path and file name... extract and rturn the filename.
//
CString
CFileSnoop::getFilename( 
	const CString&	in_filepath )
{
	int	idx;

	idx = in_filepath.ReverseFind( '/' );
	if (idx < 0)
		idx = in_filepath.ReverseFind( '\\' );
	if (idx < 0)
		return CString("");

	return in_filepath.Mid( idx+1 );
}

// ==================================================================
//		getPath
//
//		Given a path and filename... extract and return the path
//
CString	
CFileSnoop::getPath(
	const CString&	in_filepath )
{
	int	idx;

	idx = in_filepath.ReverseFind( '/' );
	if (idx < 0)
		idx = in_filepath.ReverseFind( '\\' );
	if (idx < 0)
		return CString("");

	return in_filepath.Left( idx+1 );
}

// ==================================================================
//		getSuffix
//
//		Given a filename or path/file... return the file's suffix
//		(everything after the final '.', if anything)
//
CString
CFileSnoop::getSuffix( 
	const CString&	in_filepath )
{
	int	idx;

	idx = in_filepath.ReverseFind( '.' );
	if (idx < 0)
		return CString("");

	return in_filepath.Mid( idx+1 );
}

// ==================================================================
//		getSuffixIdx
//
//		Return the index of the final '.' in the filename
//		-1 means, nope!
//
int
CFileSnoop::getSuffixIdx( 
	const CString&	in_filepath )
{
	return in_filepath.ReverseFind( '.' );
}

// ==================================================================
//		stripSuffix
//
//		Strip the suffix from the filename.  Returns the filename
//		without the suffix.
//
CString
CFileSnoop::stripSuffix(
	const CString&	in_filepath )
{
	int	idx;

	idx = in_filepath.ReverseFind( '.' );
	if (idx < 0)
		return CString("");

	return in_filepath.Left( idx );
}

