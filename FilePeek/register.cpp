// ==================================================================
//		Register
//
//		Registry manager, simple function wrappers
//
// ==================================================================

#include "stdafx.h"
#include "MathConst.h"
#include "register.h"

static const int MAXBUF = 1024;
static char g_buf[MAXBUF];

// ============================================================================

int
CRegister::IntGet( const CString& key, const CString& subkey, int defval )
{
	CString buf;
	
	buf.Format( "%d", defval );
	buf = StringGet( key, subkey, buf );

	return ( atoi( buf ) );
}

double
CRegister::DoubleGet( const CString& key, const CString& subkey, double defval )
{
	CString buf;
	
	buf.Format( "%f", defval );
	buf= StringGet( key, subkey, buf );

	return ( atof( buf ) );
}

CString
CRegister::StringGet( const CString& key, const CString& subkey, const CString& defval )
{
	HKEY reg_key;

	long status = RegOpenKey( HKEY_LOCAL_MACHINE, key, &reg_key );
	if (status == ERROR_SUCCESS)
	{
		static DWORD type;
		unsigned long buf_len = sizeof( g_buf );

		status = RegQueryValueEx( reg_key, subkey, NULL, &type, (BYTE*)g_buf, &buf_len );

		RegCloseKey( reg_key );

		// TODO: Put the value in the reg if failed (ie. status != 0)?
	}

	return ((status == ERROR_SUCCESS) ? CString( g_buf ) : defval);
}

void
CRegister::IntSet( const CString& key, const CString& subkey, int val )
{
	wsprintf( g_buf, "%d", val );
	StringSet( key, subkey, g_buf );
}

void
CRegister::DoubleSet( const CString& key, const CString& subkey, double val )
{
	// wsprintf( g_buf, "%f", val );  -- failed on vals like 0.001 :-(
	sprintf( g_buf, "%f", val );
	StringSet( key, subkey, g_buf );
}

void
CRegister::StringSet( const CString& key, const CString& subkey, const CString& val )
{
	HKEY reg_key;

	long reg_err = RegCreateKey( HKEY_LOCAL_MACHINE, key, &reg_key );
	if (reg_err == ERROR_SUCCESS )
	{
		wsprintf( g_buf, "%d", val );
		reg_err = RegSetValueEx( reg_key, subkey, NULL, REG_SZ, (BYTE*)(LPCSTR)val, val.GetLength() );

		RegCloseKey( reg_key );
	}

	return;
}

int
CRegister::Debug( const CString& subkey )
{
#if (_CI || _NST)
	CString key = RootPathV() + "\\Debug";
#else
	CString key = RootPathV() + "\\Preferences\\Debugging";
#endif
	return ( IntGet( key, subkey, 0 ) );
}

const CString
CRegister::RootPath()
{
#if (_CI)
	// return CString("Software\\Component_Innovation");
	return CString("Software\\Woodworking Automation Systems");
#else 
	#if (_NST)
		return CString("Software\\cad_nester");
	#else
		return CString("Software\\WE-CIM");
	#endif
#endif
}

const CString
CRegister::RootPathV()
{
	CString	root;
	CString	currv;

	root = RootPath();

#if (_CI || _NST)
	currv = StringGet( RootPath(), "CurrentVersion", "<error>" );
#else
	currv = "16.5";
#endif

	return (root + "\\" + currv);
}

