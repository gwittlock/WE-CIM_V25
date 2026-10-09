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
CRegister::IntGet( const CString& key, const CString& subkey )
{
	CString buf = StringGet( key, subkey );
	return ((buf.GetLength() > 0) ? atoi( buf ) : IUNDEFINED );
}

double
CRegister::DoubleGet( const CString& key, const CString& subkey )
{
	CString buf = StringGet( key, subkey );
	return ((buf.GetLength() > 0) ? atof( buf ) : UNDEFINED );
}

CString
CRegister::StringGet( const CString& key, const CString& subkey )
{
	HKEY reg_key;

	long status = RegOpenKey( HKEY_LOCAL_MACHINE, key, &reg_key );
	if (status == ERROR_SUCCESS)
	{
		static DWORD type;
		unsigned long buf_len = sizeof( g_buf );

		status = RegQueryValueEx( reg_key, subkey, NULL, &type, (BYTE*)g_buf, &buf_len );

		RegCloseKey( reg_key );
	}

	if (status != ERROR_SUCCESS)
		g_buf[0] = 0;

	return CString( g_buf );
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
	wsprintf( g_buf, "%f", val );
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


