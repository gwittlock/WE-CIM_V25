#if !defined(_FILESNOOP_H)
#define _FILESNOOP

// ==================================================================
//		File Snoop
//
//	File searching manager, wrapping up the standard MFC file search
//
// ==================================================================

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ==================================================================

class dllExport CFileSnoop
{
public:
	CFileSnoop();
	virtual ~CFileSnoop();

	// Major search functions...
	const CStringArray*	findAllDir( const CString& in_pattern );
	const CStringArray*	findAllFile( const CString& in_pattern );

	int		countDir( void )								{ return m_name_array.GetSize(); }
	CString	getDir( int in_idx )							{ return m_name_array.GetAt( in_idx ); }

	int		countFile( void )								{ return m_name_array.GetSize(); }
	CString	getFile( int in_idx )						{ return m_name_array.GetAt( in_idx ); }

	// Utility... 
	static CString		getCurrentDir( void );
	static CString		getFilename( const CString& in_filepath );
	static CString		getPath( const CString& in_filepath );
	static CString		getSuffix( const CString& in_file );
	static int			getSuffixIdx( const CString& in_file );
	static CString		stripSuffix( const CString& in_file );

protected:

private:
	// Disabled.
	CFileSnoop( const CFileSnoop& );
	const CFileSnoop& operator = ( const CFileSnoop& );
	int operator == ( const CFileSnoop& ) const;
	int operator != ( const CFileSnoop& ) const;

private:
	HANDLE			m_handle;
	WIN32_FIND_DATA	m_data;

	CStringArray		m_name_array;
};

#endif

